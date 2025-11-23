#pragma once

#include "Header.h"
#include "../Core/ObjectId.h"
#include "../Core/LockFreeQueue.h"
#include "../Core/Event.h"
#include "Server.h"
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/buffer.hpp>

namespace boost::asio
{
	class io_context;
}

namespace bugat::net
{
	using boost::asio::ip::tcp;

	enum class ConnectionState
	{
		Disconnected,
		Connecting,
		Connected,
	};

	class AnySendPacket;
	class Server;
	class Connection : public std::enable_shared_from_this<Connection>
	{
		friend class Server;
		friend class AnyConnectionFactory;
		friend struct MessageReader;
		friend struct MessageWriter;
		friend struct MessageProcessor;
		friend struct ConnectionCloser;

	public:
		Event<> OnAccept;
		Event<> OnClose;
		Event<const Header&, const std::vector<char>&> OnRead;

	public:
		Connection() : _socket(nullptr), _state(ConnectionState::Connecting) {}
		virtual ~Connection() {}

		bool Connect(std::string ip, short port);

		/*
		* T는 반드시 std::vector<std::tuple<uint8_t*, size_t>> data() 함수를 구현해야한다.
		*/
		template<typename T>
		void Send(T&& packet)
		{ 
			_sendQue.Push(new AnySendPacket(packet));
			SendNotify();
		}

		bool Start();
		void Close();

		auto GetId() const { return _id; }

		bool Disconnected() const { return _state == ConnectionState::Disconnected; }

	private:
		void SendNotify();

	private:
		std::unique_ptr<tcp::socket> _socket;
		core::ObjectId<Connection> _id;
		std::atomic<ConnectionState> _state;

		LockFreeQueue<AnySendPacket*> _sendQue;
		AnySendPacket* sendingPacket{ nullptr };
		std::unique_ptr<boost::asio::steady_timer> _sendTimer;
	};

	class SendPacketConcept
	{
	public:
		virtual ~SendPacketConcept() {}
		virtual std::vector<std::tuple<uint8_t*, size_t>> GetBufs() = 0;
	};

	template<typename T>
	class SendPacketModel : public SendPacketConcept
	{
	public:
		SendPacketModel(T& packet) : _packet(std::move(packet)) {}
		virtual ~SendPacketModel() {}
		virtual std::vector<std::tuple<uint8_t*, size_t>> GetBufs() override { return _packet.data(); }

		T _packet;
	};

	class AnySendPacket
	{
	public:
		AnySendPacket() {}
		~AnySendPacket() {}

		template<typename T>
		AnySendPacket(T& packet) : _ptr(std::make_unique<SendPacketModel<T>>(packet))
		{
			auto bufs = _ptr->GetBufs();
			for (auto iter = bufs.begin(); iter != bufs.end(); iter++)
				_bufs.push_back(boost::asio::buffer(std::get<0>(*iter), std::get<1>(*iter)));
		};

		auto GetBufs() const { return _bufs; }
		void Update(size_t size)
		{
			for (auto iter = _bufs.begin(); iter != _bufs.end();)
			{
				auto bufSize = iter->size();
				if (size > bufSize)
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

	private:
		std::list<boost::asio::const_buffer> _bufs;
		std::unique_ptr<SendPacketConcept> _ptr;
	};

	/*
	* Connection을 상속받은 객체 생성 용
	*/
	class ConnectionFactoryConcept
	{
	public:
		virtual ~ConnectionFactoryConcept() {}
		virtual std::shared_ptr<Connection> Create() = 0;
	};

	template<typename T>
	class ConnectionFactory : public ConnectionFactoryConcept
	{
	public:
		virtual ~ConnectionFactory() {}
		virtual std::shared_ptr<Connection> Create() override
		{
			return std::dynamic_pointer_cast<Connection>(std::make_shared<T>());
		}
	};

	class AnyConnectionFactory
	{
	public:
		template<typename T>
		AnyConnectionFactory(ConnectionFactory<T> factory) : _ptr(std::make_unique<ConnectionFactory<T>>(std::move(factory))) {};

		std::shared_ptr<Connection> Create(std::unique_ptr<tcp::socket>& socket) const
		{
			auto connection = _ptr->Create();
			connection->_socket = std::move(socket);
			return connection;
		}

	private:
		std::unique_ptr<ConnectionFactoryConcept> _ptr;
	};
}