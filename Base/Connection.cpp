#include "pch.h"
#include "Connection.h"
#include "NetworkMessage.h"

namespace bugat
{
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

	namespace Net
	{
		AwaitTask<void> Connect(std::shared_ptr<Connection> connection, Resolver::results_type endpoints)
		{
			auto error = co_await AwaitConnect(connection.get(), endpoints);
			if (error)
			{
				ErrorLog("Socket Connect Error[{}]! message {}", error.value(), error.message());
				connection->Close();
				co_return;
			}

			connection->OnConnect();
			connection->Start();
			co_return;
		}

		AwaitTask<void> Send(std::shared_ptr<Connection> connection)
		{
			AnySendPacket* sendingPacket = nullptr;
			while (false == connection->Disconnected())
			{
				if (sendingPacket == nullptr)
				{
					if (false == connection->PopSendPacket(sendingPacket))
					{
						co_await AwaitAlways(connection.get());
						continue;
					}
				}

				auto [error, transfer] = co_await AwaitSend(connection.get(), sendingPacket);
				if (error)
				{
					auto id = connection->GetObjectId();
					if (error.value() == boost::asio::error::eof)
						InfoLog("Socket Close ID[{}]! message {}", error.value(), error.message(), id.String());
					else
						ErrorLog("Socket Close Error[{}] ID[{}]! message {}", error.value(), error.message(), id.String());

					break;
				}

				sendingPacket->Update(transfer);

				if (sendingPacket->IsEmpty())
				{
					delete sendingPacket;
					sendingPacket = nullptr;
				}
			}

			connection->Close();
			co_return;
		}

		AwaitTask<void> Recv(std::shared_ptr<Connection> connection)
		{
			NetworkMessage<TCPRecvPacket> msg;
			while (false == connection->Disconnected())
			{
				auto bufInfo = msg.GetBufInfo();
				auto [error, transper] = co_await AwaitRecv(connection.get(), bufInfo.buf, bufInfo.size);
				if (error)
				{
					auto id = connection->GetObjectId();
					if (error.value() == boost::asio::error::eof)
						InfoLog("Socket Close ID[{}]! message {}", error.value(), error.message(), id.String());
					else
						ErrorLog("Socket Close Error[{}] ID[{}]! message {}", error.value(), error.message(), id.String());

					break;
				}

				msg.Update(transper);

				std::shared_ptr<TCPRecvPacket> packet = nullptr;
				if (msg.GetNetMessage(packet))
					connection->OnRead(packet);
			}

			connection->Close();
			co_return;
		}
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

	void Connection::Connect(const Executor& executor, std::string ip, short port)
	{
		_socket = std::make_unique<Socket>(executor);
		Resolver resolver(executor);
		EndPoint ep(boost::asio::ip::make_address(ip), port);
		Resolver::results_type endpoints = resolver.resolve(ep);

		auto sptr = std::static_pointer_cast<Connection>(shared_from_this());
		CoSpawn(*this, Net::Connect(sptr, endpoints));
	}

	bool Connection::PopSendPacket(AnySendPacket*& packet)
	{
		return _sendQue.Pop(packet);
	}

	bool Connection::Start()
    {
		auto expect = ConnectionState::Connecting;
		if (false == _state.compare_exchange_strong(expect, ConnectionState::Connected, std::memory_order_acq_rel))
			return false;

		auto sptr = std::static_pointer_cast<Connection>(shared_from_this());
		CoSpawn(*this, Net::Send(sptr));
		CoSpawn(*this, Net::Recv(sptr));

		return true;
    }
}