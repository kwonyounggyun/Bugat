#include "stdafx.h"
#include "ClientHandler.h"

namespace bugat::handle
{
	void ClientHandler::ResSC_Login(const bugat::test::Client* client, const bugat::net::Header& header, const std::vector<char>& msg)
	{
		auto res = bugat::packet::game::GetResSC_Login(msg.data());
	}
}