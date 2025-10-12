#include "stdafx.h"
#include "ClientHandler.h"

namespace bugat::handle
{
	using namespace bugat::packet;

	ClientHandler::ClientHandler()
	{
		_handleMap[static_cast<int>(bugat::packet::game::Type::Weapon)] = &ClientHandler::ResSC_Login;
	}

	void ClientHandler::Handle(const bugat::test::Client* client, const bugat::net::Header& header, const std::vector<char>& msg)
	{
		_handleMap[header.type](client, header, msg);
	}
}
