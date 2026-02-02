#pragma once

#include "../Core/LockFreeQueue.h"
#include "../Core/Event.h"

#include "ObjectId.h"
#include "SerializeObject.h"
#include "Packet.h"
#include "Context.h"
#include "NetworkContext.h"
#include "Macro.h"

namespace bugat
{
	enum class ConnectionState
	{
		Disconnected,
		Connecting,
		Connected,
	};

	struct TCPSocket;
	struct Executor;
	class AnySendPacket;
	class Server;

	class Connection : public SerializeObject
	{
		friend class Server;
		
		friend struct AwaitConnect;
		friend struct AwaitRecv;
		friend struct AwaitSend;

		DECL_COROUTINE_FUNC(Connection, Send, void, ());
		DECL_COROUTINE_FUNC(Connection, Recv, void, ());
		DECL_COROUTINE_FUNC(Connection, Connect, void, (Executor, executor, std::string, ip, short, port));

	public:
		Event<> OnConnect;
		Event<> OnClose;
		Event<const TSharedPtr<TCPRecvPacket>&> OnRead;


		Connection();
		virtual ~Connection();

		void Connect(const NetworkContext& executor, std::string ip, short port);

		/*
		* T는 반드시 std::vector<std::tuple<uint8_t*, size_t>> data() 함수를 구현해야한다.
		*/
		template<typename T>
		void Send(T&& packet)
		{ 
			_sendQue.Push(new AnySendPacket(packet));
		}

		bool PopSendPacket(AnySendPacket*& packet);

		bool Start();
		void Close();

		auto GetId() const { return _id; }

		bool Disconnected() const { return _state == ConnectionState::Disconnected; }
		bool Connected() const { return _state == ConnectionState::Connected; }
		void SetSocket(std::unique_ptr<TCPSocket>& socket);

	private:
		std::unique_ptr<TCPSocket> _socket;
		ObjectId<Connection> _id;
		std::atomic<ConnectionState> _state;

		LockFreeQueue<AnySendPacket*> _sendQue;

#if defined(_DEBUG)
	public:
		std::atomic<int> _recvCount;
		std::atomic<int> _sendCount;
#endif
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
		AnySendPacket(T& packet) : _ptr(std::make_unique<SendPacketModel<T>>(packet)) { };

		auto GetBufs() const { return _ptr->GetBufs(); }

	private:
		std::unique_ptr<SendPacketConcept> _ptr;
	};

	/*
	* Connection을 상속받은 객체 생성 용
	*/
	class ConnectionFactoryConcept
	{
	public:
		virtual ~ConnectionFactoryConcept() {}
		virtual TSharedPtr<Connection> Create() = 0;
	};

	template<typename T>
	class ConnectionFactory : public ConnectionFactoryConcept
	{
	public:
		ConnectionFactory(Context& context) : _context(context) {}
		virtual ~ConnectionFactory() {}
		virtual TSharedPtr<Connection> Create() override
		{
			auto connection = CreateSerializeObject<T>(&_context);
			return connection;
		}

	private:
		Context& _context;
	};

	class AnyConnectionFactory
	{
	public:
		template<typename T>
		AnyConnectionFactory(ConnectionFactory<T> factory) : _ptr(std::make_unique<ConnectionFactory<T>>(std::move(factory))) {};
		AnyConnectionFactory(AnyConnectionFactory&& other) 
		{ 
			_ptr = std::move(other._ptr);
		}

		TSharedPtr<Connection> Create(std::unique_ptr<TCPSocket>& socket) const;

	private:
		std::unique_ptr<ConnectionFactoryConcept> _ptr;
	};
}