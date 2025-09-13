#pragma once

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_context.hpp>

namespace bugat::net
{
	using boost::asio::ip::tcp;

	class Connection : std::enable_shared_from_this<Connection>
	{
		friend class Server;
	public:
		Connection() = delete;
		explicit Connection(tcp::socket& socket) : _socket(std::move(socket)) {}
		~Connection() {}

		void Send(char* buf, int size);

	protected:
		void Read(boost::asio::io_context& _io);
		virtual void ProcessMsg(const std::vector<char>& msg) {};

	private:
		tcp::socket _socket;
	};

	/*
	* Connection을 상속받은 객체 생성 용
	*/
	class ConnectionFactoryConcept
	{
	public:
		virtual ~ConnectionFactoryConcept() {}
		virtual std::shared_ptr<Connection> Create(tcp::socket& socket) = 0;
	};

	template<typename T>
	class ConnectionFactory : public ConnectionFactoryConcept
	{
	public:
		virtual ~ConnectionFactory() {}
		virtual std::shared_ptr<Connection> Create(tcp::socket& socket) override
		{
			return std::dynamic_pointer_cast<Connection>(std::make_shared<T>(socket));
		}
	};

	class AnyConnectionFactory
	{
	public:
		template<typename T>
		AnyConnectionFactory(ConnectionFactory<T> factory) : _ptr(std::make_unique<ConnectionFactory<T>>(std::move(factory))) {};

		std::shared_ptr<Connection> Create(tcp::socket& socket)
		{
			return _ptr->Create(socket);
		}

	private:
		std::unique_ptr<ConnectionFactoryConcept> _ptr;
	};
}