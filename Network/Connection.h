#pragma once

#include "Header.h"
#include "..\Core\ObjectId.h"
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_context.hpp>

namespace bugat::net
{
	using boost::asio::ip::tcp;

	enum class ConnectionState
	{
		Disconnected,
		Connecting,
		Connected,
	};

	class Connection : std::enable_shared_from_this<Connection>
	{
		friend class Server;
		friend class AnyConnectionFactory;
		friend struct SocketGetter;
		friend struct MessageProcessor;
		friend struct SocketCloser;
	public:
		Connection() : _socket(nullptr), _state(ConnectionState::Connecting) {}
		~Connection() {}

		bool Connect(std::string ip, short port);
		void Send(char* buf, int size);
		void Close();

		auto GetId() const { return _id; }

		bool Disconnected() const { return _state == ConnectionState::Disconnected; }

	protected:
		void Read();

		/*
		* Close에서 호출된다. Close호출이후 들어오는 클라이언트 처리 패킷은 모두 무시한다.
		* Close 호출 자체가 유저가 종료했을때나 비정상 종료로 호출되는것이므로 이후 들어오는 패킷은 유저가 발생시킨 이벤트로 생각하지 않는다.
		*/
		virtual void AfterClose() {};
		virtual void ProcessMsg(const Header& header, const std::vector<char>& msg) {};

	private:
		std::unique_ptr<tcp::socket> _socket;
		core::ObjectId<Connection> _id;
		ConnectionState _state;
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

		std::shared_ptr<Connection> Create(boost::asio::io_context& io) const
		{
			auto socket = std::make_unique<tcp::socket>(io);
			auto connection = _ptr->Create();
			connection->_socket = std::move(socket);
			return connection;
		}

	private:
		std::unique_ptr<ConnectionFactoryConcept> _ptr;
	};
}