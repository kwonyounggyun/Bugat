#include "stdafx.h"
#include "Client.h"
#include "ClientConnection.h"

namespace bugat::test
{
	Client::~Client()
	{
	}

	bool Client::Connect(boost::asio::io_context& io, std::string ip, short port)
	{
		_connection = CreateClientConnection(io);
		return _connection->Connect(ip, port);
	}

	void Client::Send(char* buf, int size)
	{
		_connection->Send(buf, size);
	}
}
