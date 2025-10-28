#pragma once
#include <boost/asio/io_context.hpp>
#include <memory>

namespace bugat::net
{
	struct Configure;
	class Connection;
	class AnyConnectionFactory;

	class Server
	{
	public:
		Server();
		~Server();

		//楷搬等 Connection 包府 肺流 累己
		virtual void OnAccept(std::shared_ptr<Connection>& conn) = 0;
	};
}