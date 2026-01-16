#include "pch.h"
#include "Connection.h"
#include "NetworkMessage.h"
#include "BoostAsio.h"

namespace bugat
{

	struct AwaitConnect
	{
		AwaitConnect(Connection* con, Resolver::results_type& endpoints) : _con(con), _endpoints(endpoints) {}
		bool await_ready() const noexcept { return false; }
		void await_suspend(std::coroutine_handle<> h) noexcept
		{
			_con->_socket->AsyncConnect(_endpoints, [this, h](const BoostError& error, const EndPoint& endpoint) {
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
			_con->_socket->AsyncReadSome(_buf, _size, [this, h](const BoostError& error, size_t transper) {
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
		AwaitSend(Connection* con, const std::list<boost::asio::const_buffer>& sendBuffer) : _con(con), _sendBuffer(sendBuffer) {}
		bool await_ready() const noexcept { return false; }
		void await_suspend(std::coroutine_handle<> h) noexcept
		{
			_con->_socket->AsyncWriteSome(_sendBuffer, [this, h](const BoostError& error, size_t transper) {
				_result = { error, transper };
				_con->Post([h]() {
					h.resume(); });
				});
		}

		auto await_resume()
		{
			return _result;
		}

		const std::list<boost::asio::const_buffer>& _sendBuffer;
		Connection* _con;
		std::tuple<BoostError, size_t> _result;
	};

	namespace Net
	{
		AwaitTask<void> Connect(TSharedPtr<Connection> connection, Resolver::results_type endpoints)
		{
			auto error = co_await AwaitConnect(connection.Get(), endpoints);
			if (error)
			{
				ErrorLog("Socket Connect Error[{}]! message {}", error.value(), error.message());
				connection->Close();
				co_return;
			}

			connection->Start();
			co_return;
		}

		class PacketHelper
		{
		public:
			PacketHelper() {}
			~PacketHelper() {}

			void Init(AnySendPacket* packet)
			{
				auto bufs = packet->GetBufs();
				for (auto iter = bufs.begin(); iter != bufs.end(); iter++)
					_bufs.push_back(boost::asio::buffer(std::get<0>(*iter), std::get<1>(*iter)));
			};

			auto& GetBufs() const { return _bufs; }
			void Update(size_t size)
			{
				for (auto iter = _bufs.begin(); iter != _bufs.end();)
				{
					auto bufSize = iter->size();
					if (size >= bufSize)
					{
						iter = _bufs.erase(iter);
						size -= bufSize;
						continue;
					}

					const char* ptr = static_cast<const char*>(iter->data());
					*iter = boost::asio::buffer(ptr + size, bufSize - size);
					break;
				}
			}

			bool IsEmpty()
			{
				return _bufs.empty();
			}

			void Clear()
			{
				_bufs.clear();
			}

		private:
			std::list<boost::asio::const_buffer> _bufs;
		};

		AwaitTask<void> Send(TSharedPtr<Connection> connection)
		{
			AnySendPacket* sendingPacket = nullptr;
			PacketHelper helper;
			while (false == connection->Disconnected())
			{
				if (sendingPacket == nullptr)
				{
					if (false == connection->PopSendPacket(sendingPacket))
					{
						co_await AwaitAlways(connection.Get());
						continue;
					}
					else
						helper.Init(sendingPacket);
				}
				
				auto [error, transfer] = co_await AwaitSend(connection.Get(), helper.GetBufs());
				if (error)
				{
					auto id = connection->GetObjectId();
					if (error.value() == boost::asio::error::eof)
						InfoLog("Socket Close ID[{}]! message {}", error.value(), error.message(), id.String());
					else
						ErrorLog("Socket Close Error[{}] ID[{}]! message {}", error.value(), error.message(), id.String());

					break;
				}

				helper.Update(transfer);

				if (helper.IsEmpty())
				{
					helper.Clear();
					delete sendingPacket;
					sendingPacket = nullptr;

#if defined(_DEBUG)
					connection->_sendCount.fetch_add(1);
#endif
				}
			}

			connection->Close();
			co_return;
		}

		AwaitTask<void> Recv(TSharedPtr<Connection> connection)
		{
			NetworkMessage<TCPRecvPacket> msg;
			while (false == connection->Disconnected())
			{
				auto bufInfo = msg.GetBufInfo();
				auto [error, transper] = co_await AwaitRecv(connection.Get(), bufInfo.buf, bufInfo.size);
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

				TSharedPtr<TCPRecvPacket> packet = nullptr;
				while (msg.GetNetMessage(packet))
				{
					connection->OnRead(packet);
#if defined(_DEBUG)
					connection->_recvCount.fetch_add(1);
#endif
				}
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
			try
			{
				_socket->Close();
			}
			catch(std::exception& ex)
			{
				ErrorLog("Socket Close Error {}", ex.what());
			}
			_socket.reset();
		}

		OnClose();
	}

	void Connection::SetSocket(std::unique_ptr<TCPSocket>& socket)
	{
		_socket = std::move(socket);
	}

	Connection::Connection() : _socket(nullptr), _state(ConnectionState::Connecting)
	{
	}

	Connection::~Connection()
	{
	}

	void Connection::Connect(const NetworkContext& executor, std::string ip, short port)
	{
		_socket = std::make_unique<TCPSocket>(executor.GetExecutor());
		Resolver resolver(executor.GetExecutor());
		EndPoint ep(boost::asio::ip::make_address(ip), port);
		Resolver::results_type endpoints = resolver.resolve(ep);

		CoSpawn(*this, Net::Connect(this, endpoints));
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

		CoSpawn(*this, Net::Send(this));
		CoSpawn(*this, Net::Recv(this));

		OnConnect();

		return true;
    }

	TSharedPtr<Connection> AnyConnectionFactory::Create(std::unique_ptr<TCPSocket>& socket) const
	{
		auto connection = _ptr->Create();
		connection->SetSocket(socket);
		return connection;
	}
}