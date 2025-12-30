#include "pch.h"
#include "Connection.h"
#include "NetworkMessage.h"

namespace bugat
{
	struct AwaitAlways
	{
		AwaitAlways(TaskSerializer* serializer) : _serializer(serializer) {}
		bool await_ready() const noexcept { return false; }
		void await_suspend(std::coroutine_handle<> h) noexcept
		{
			_serializer->Post([h]() {
				h.resume();
				});
		}

		void await_resume()
		{
		}

		TaskSerializer* _serializer;
	};

	struct AwaitConnect
	{
		AwaitConnect(Connection* con, Resolver::results_type& endpoints) : _con(con), _endpoints(endpoints) {}
		bool await_ready() const noexcept { return false; }
		void await_suspend(std::coroutine_handle<> h) noexcept
		{
			boost::asio::async_connect(*_con->_socket, _endpoints, [this, h](const BoostError& error, const EndPoint& endpoint) {
				_result = error;
				_con->Post([h]() {
					h.resume(); });
				});
		}

		auto await_resume()
		{
			return _result;
		}

		Resolver::results_type& _endpoints;
		Connection* _con;
		BoostError _result;
	};

	struct AwaitRecv
	{
		AwaitRecv(Connection* con, char* buf, int size) : _con(con), _buf(buf), _size(size) {}
		bool await_ready() const noexcept { return false; }
		void await_suspend(std::coroutine_handle<> h) noexcept
		{
			_con->_socket->async_read_some(boost::asio::buffer(_buf, _size), [this, h](const BoostError& error, size_t transper) {
				_result = { error, transper };
				_con->Post([h]() {
					h.resume(); });
				});
		}

		auto await_resume()
		{
			return _result;
		}

		char* _buf;
		int _size;
		Connection* _con;
		std::tuple<BoostError, size_t> _result;
	};

	struct AwaitSend
	{
		AwaitSend(Connection* con, AnySendPacket* packet) : _con(con), _packet(packet) {}
		bool await_ready() const noexcept { return false; }
		void await_suspend(std::coroutine_handle<> h) noexcept
		{
			_con->_socket->async_write_some(_packet->GetBufs(), [this, h](const BoostError& error, size_t transper) {
				_result = { error, transper };
				_con->Post([h]() {
					h.resume(); });
				});
		}

		auto await_resume()
		{
			return _result;
		}

		AnySendPacket* _packet;
		Connection* _con;
		std::tuple<BoostError, size_t> _result;
	};

	AwaitTask<void> Connection::Connect(const Executor& executor, std::string ip, short port)
	{
		_socket = std::make_unique<Socket>(executor);
		Resolver resolver(executor);
		EndPoint ep(boost::asio::ip::make_address(ip), port);
		Resolver::results_type endpoints = resolver.resolve(ep);

		auto error = co_await AwaitConnect(this, endpoints);
		if (error)
		{
			ErrorLog("Socket Connect Error[{}]! message {}", error.value(), error.message());
			_socket.reset();
			co_return;
		}

		Start();
		co_return;
	}

	void Connection::Close()
	{
		auto expect = ConnectionState::Connected;
		while (true)
		{
			if (true == _state.compare_exchange_strong(expect, ConnectionState::Disconnected, std::memory_order_acq_rel, std::memory_order_acquire))
				break;

			if (expect == ConnectionState::Disconnected)
				return;
		}

		if (_socket != nullptr)
		{
			boost::system::error_code ec;
			_socket->close(ec);
			if (ec)
				std::cout << "Socket close failed. Reason: " << ec.message() << std::endl;

			_socket.reset();
		}

		OnClose();
	}

	AwaitTask<void> Connection::Send()
	{
		AnySendPacket* sendingPacket = nullptr;
		while (false == Disconnected())
		{
			if (sendingPacket == nullptr)
			{
				if (false == _sendQue.Pop(sendingPacket))
				{
					co_await AwaitAlways(this);
					continue;
				}
			}

			auto [error, transfer] = co_await AwaitSend(this, sendingPacket);
			if(error)
			{
				if (error.value() != boost::asio::error::eof)
					ErrorLog("Socket Close Error[{}]! message {}", error.value(), error.message());

				break;
			}

			sendingPacket->Update(transfer);

			if (sendingPacket->IsEmpty())
			{
				delete sendingPacket;
				sendingPacket = nullptr;
			}
		}
		
		Close();
		co_return;
	}

	AwaitTask<void> Connection::Recv()
	{
		NetworkMessage msg;
		while (false == Disconnected())
		{
			auto bufInfo = msg.GetBufInfo();
			auto [error, transper] = co_await AwaitRecv(this, bufInfo.buf, bufInfo.size);
			if (error)
			{
				if (error.value() != boost::asio::error::eof)
					ErrorLog("Socket Close Error[{}]! message {}", error.value(), error.message());

				break;
			}

			msg.Update(transper);

			std::shared_ptr<RecvPacket> packet = nullptr;
			if (msg.GetNetMessage(packet))
				OnRead(packet);
		}

		Close();
		co_return;
	}

	bool Connection::Start()
    {
		auto con = shared_from_this();
		CoSpawn(con, Send());
		CoSpawn(con, Recv());

		auto expect = ConnectionState::Connecting;
		if (false == _state.compare_exchange_strong(expect, ConnectionState::Connected, std::memory_order_acq_rel))
			return false;

		OnAccept();

		return true;
    }
}