#pragma once

#include "Header.h"
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_context.hpp>

namespace bugat::net
{
	using boost::asio::ip::tcp;

	class Connection : std::enable_shared_from_this<Connection>
	{
		friend class Server;
		friend class AnyConnectionFactory;
	public:
		Connection() : _socket(nullptr) {}
		~Connection() {}

		bool Connect(std::string ip, short port);
		void Send(char* buf, int size);

	protected:
		void Read();
		virtual void ProcessMsg(const Header& header, const std::vector<char>& msg) {};

	private:
		std::unique_ptr<tcp::socket> _socket;
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