#include "stdafx.h"
#include "Client.h"
#include "../Network/Connection.h"

namespace bugat::test
{
	Client::~Client()
	{
	}
	void Client::Connect(boost::asio::io_context& io, std::string ip, short port)
	{
		_connection = std::make_unique<bugat::net::Connection>(io);
		_connection->Connect(ip, port);
	}

	void Client::Send(char* buf, int size)
	{
		_connection->Send(buf, size);
	}
}
