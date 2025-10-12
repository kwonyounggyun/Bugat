#include "stdafx.h"
#include "ClientConnection.h"

namespace bugat::test
{
	void ClientConnection::ProcessMsg(const bugat::net::Header& header, const std::vector<char>& msg)
	{
		_client->PushMsg(header, msg);
	}
}