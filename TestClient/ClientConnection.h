#pragma once
#include "../Network/Connection.h"
#include "../Network/Header.h"

namespace bugat::test
{
	class ClientConnection : public bugat::net::Connection
	{
	protected:
		virtual void ProcessMsg(const bugat::net::Header& header, const std::vector<char>& msg);
	};

	static std::shared_ptr<bugat::net::Connection> CreateClientConnection(boost::asio::io_context& io)
	{
		return bugat::net::AnyConnectionFactory(bugat::net::ConnectionFactory<ClientConnection>()).Create(io);
	}
}

