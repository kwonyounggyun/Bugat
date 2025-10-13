#include "pch.h"
#include "Connection.h"
#include "NetworkMessage.h"

#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/connect.hpp>

namespace bugat::net
{
	struct SocketGetter
	{
	public:
		std::unique_ptr<tcp::socket>& operator()(const std::shared_ptr<Connection>& connection)
		{
			return connection->_socket;
		}
	};

	struct MessageProcessor
	{
	public:
		void operator()(const std::shared_ptr<Connection>& connection, const Header& header, const std::vector<char>& msg)
		{
			return connection->ProcessMsg(header, msg);
		}
	};

	struct SocketCloser
	{
	public:
		void operator()(const std::shared_ptr<Connection>& connection)
		{
			connection->_state = ConnectionState::Disconnected;

			if(connection->_socket == nullptr)
				return;

			boost::system::error_code ec;
			connection->_socket->close(ec);
			if(ec)
			{
				std::cout << "Socket close failed. Reason: " << ec.message() << std::endl;
			}

			connection->_socket.reset();
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

		_state = ConnectionState::Connected;

		Read();
		return true;
	}

	void Connection::Send(char* buf, int size)
    {
		if (false == Disconnected())
			boost::asio::async_write(*_socket, boost::asio::buffer(buf, size), boost::asio::deferred);
    }

	void Connection::Close()
	{
		_state = ConnectionState::Disconnected;

		AfterClose();
	}

    void Connection::Read()
    {
		_state = ConnectionState::Connected;
		boost::asio::co_spawn(_socket->get_executor(), [connection = shared_from_this()]()->boost::asio::awaitable<void> {
			NetworkMessage _msg;
			Header header;
			try
			{
				/*
				* 외부 노출 하기 싫어서...
				*/
				auto socketGetter = SocketGetter();
				auto messageProcessor = MessageProcessor();
				while(false == connection->Disconnected())
				{
					auto block = _msg.GetNewDataBlock();
					std::size_t n = co_await boost::asio::async_read(*socketGetter(connection), boost::asio::buffer(block->GetBuf(), block->GetSize()), boost::asio::use_awaitable);
					std::vector<char> recievedMsg;
					if (_msg.GetNetMessage(header, recievedMsg))
						messageProcessor(connection, header, recievedMsg);
				}
			}
			catch (std::exception&)
			{
			}

			SocketCloser()(connection);
			});
    }
}