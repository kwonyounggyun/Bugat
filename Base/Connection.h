#pragma once

#include "../Core/ObjectId.h"
#include "../Core/LockFreeQueue.h"
#include "../Core/Event.h"

#include "../Core/AwaitTask.h"
#include "SerializeObject.h"
#include "Packet.h"
#include "Boost.h"

namespace bugat
{
	enum class ConnectionState
	{
		Disconnected,
		Connecting,
		Connected,
	};

	class AnySendPacket;
	class Server;
	class Connection : public SerializeObject
	{
		friend class Server;
		friend class AnyConnectionFactory;
		
		friend struct AwaitConnect;
		friend struct AwaitRecv;
		friend struct AwaitSend;

	public:
		Event<> OnAccept;
		Event<> OnClose;
		Event<const std::shared_ptr<RecvPacket>&> OnRead;


		Connection() : _socket(nullptr), _state(ConnectionState::Connecting) {}
		virtual ~Connection() {}

		AwaitTask<void> Connect(const Executor& executor, std::string ip, short port);

		/*
		* T는 반드시 std::vector<std::tuple<uint8_t*, size_t>> data() 함수를 구현해야한다.
		*/
		template<typename T>
		void Send(T&& packet)
		{ 
			_sendQue.Push(new AnySendPacket(packet));
		}

		bool Start();
		void Close();

		auto GetId() const { return _id; }

		bool Disconnected() const { return _state == ConnectionState::Disconnected; }

	private:
		AwaitTask<void> Send();
		AwaitTask<void> Recv();

	private:
		std::unique_ptr<Socket> _socket;
		ObjectId<Connection> _id;
		std::atomic<ConnectionState> _state;

		LockFreeQueue<AnySendPacket*> _sendQue;
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

		std::shared_ptr<Connection> Create(std::unique_ptr<Socket>& socket) const
		{
			auto connection = _ptr->Create();
			connection->_socket = std::move(socket);
			return connection;
		}

	private:
		std::unique_ptr<ConnectionFactoryConcept> _ptr;
	};
}