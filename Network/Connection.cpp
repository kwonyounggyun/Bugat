#include "pch.h"
#include "Connection.h"
#include "NetworkMessage.h"

#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio.hpp>

namespace bugat::net
{
	struct MessageReader
	{
	public:
		boost::asio::awaitable<size_t> operator()(const std::shared_ptr<Connection>& connection, char* buf, int size)
		{
			co_return co_await connection->_socket->async_read_some(boost::asio::buffer(buf, size), boost::asio::use_awaitable);
		}
	};

	struct MessageWriter
	{
	public:
		boost::asio::awaitable<size_t> operator()(const std::shared_ptr<Connection>& connection)
		{
			auto& packet = connection->sendingPacket;
			if (packet == nullptr)
			{
				if (false == connection->_sendQue.pop(packet))
				{
					connection->_sendTimer->expires_at(std::chrono::steady_clock::time_point::max());
					boost::system::error_code ec;
					co_await connection->_sendTimer->async_wait(boost::asio::redirect_error(boost::asio::use_awaitable, ec));
					co_return 0;
				}
			}
			
			auto transfer = co_await connection->_socket->async_write_some(packet->GetBufs(), boost::asio::use_awaitable);
			packet->Update(transfer);
			if (packet->IsEmpty())
			{
				delete packet;
				packet = nullptr;
			}

			co_return transfer;
		}
	};

	struct MessageProcessor
	{
	public:
		void operator()(const std::shared_ptr<Connection>& connection, const Header& header, const std::vector<char>& msg)
		{
			return connection->OnRead(header, msg);
		}
	};

	struct ConnectionCloser
	{
	public:
		void operator()(const std::shared_ptr<Connection>& connection)
		{
			auto expect = ConnectionState::Connected;
			while (expect != ConnectionState::Disconnected)
			{
				if (true == connection->_state.compare_exchange_strong(expect, ConnectionState::Disconnected, std::memory_order_acquire))
					break;
			}

			if (expect == ConnectionState::Disconnected)
				return;

			if (connection->_socket != nullptr)
			{
				boost::system::error_code ec;
				connection->_socket->close(ec);
				if (ec)
					std::cout << "Socket close failed. Reason: " << ec.message() << std::endl;

				connection->_socket.reset();
			}

			connection->OnClose();
		}
	};

	bool Connection::Connect(std::string ip, short port)
	{
		tcp::resolver resolver(_socket->get_executor());
		tcp::endpoint ep(boost::asio::ip::make_address(ip), port);
		tcp::resolver::results_type endpoints = resolver.resolve(ep);

		try
		{
			auto result = boost::asio::connect(*_socket, endpoints);
		}
		catch (std::exception& e)
		{
			std::cout << "Connect failed. IP: " << ip << " Port: " << port << " Reason: " << e.what() << std::endl;
			return false;
		}

		Start();
		return true;
	}

	void Connection::Close()
	{
		ConnectionCloser()(shared_from_this());
	}

    void Connection::Start()
    {
		_sendTimer = std::make_unique< boost::asio::steady_timer>(_socket->get_executor(), std::chrono::steady_clock::time_point::max());
		boost::asio::co_spawn(_socket->get_executor(), [connection = shared_from_this()]()->boost::asio::awaitable<void> {
			NetworkMessage msg;
			Header header;
			try
			{
				/*
				* 외부 노출 하기 싫어서...
				*/
				auto messageReader = MessageReader();
				auto messageProcessor = MessageProcessor();
				
				while(false == connection->Disconnected())
				{
					auto block = msg.GetDataBlock();
					auto readSize = co_await messageReader(connection, block->GetBuf(), block->GetSize());
					if (readSize > 0)
						msg.Update(block, readSize);

					std::vector<char> recievedMsg;
					if (msg.GetNetMessage(header, recievedMsg))
						messageProcessor(connection, header, recievedMsg);
				}
			}
			catch (std::exception&)
			{
			}

			ConnectionCloser()(connection);
			});

		boost::asio::co_spawn(_socket->get_executor(), [connection = shared_from_this()]()->boost::asio::awaitable<void> {
			try
			{
				auto messageWriter = MessageWriter();
				while (false == connection->Disconnected())
				{
					auto writeSize = co_await messageWriter(connection);
				}
			}
			catch (std::exception&)
			{
			}

			ConnectionCloser()(connection);
			});

		OnAccept();

		auto expect = ConnectionState::Connecting;
		if (false == _state.compare_exchange_strong(expect, ConnectionState::Connected, std::memory_order_acq_rel));
			return;
    }

	void Connection::SendNotify()
	{
		_sendTimer->cancel();
	}
}