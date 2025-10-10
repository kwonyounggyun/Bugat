#pragma once

namespace boost
{
	namespace asio
	{
		class io_context;
	}
}

namespace bugat::net
{
	struct Connection;
}

namespace bugat::test
{
	class Client
	{
	public:
		Client() {};
		virtual ~Client();


		bool Connect(boost::asio::io_context& io, std::string ip, short port);
		void Send(char* buf, int size);

	private:
		std::shared_ptr<bugat::net::Connection> _connection;
	};
}