#include "stdafx.h"
#include "ClientHandler.h"
#include "../Network/Packet/headers/Protocol.h"

namespace bugat::Handle
{
	using namespace bugat::packet;
	void ClientHandler::Handle(std::vector<char>& msg)
	{
		game::PacketHeader* header = reinterpret_cast<PacketHeader*>(msg.data());
		switch (header->type)
		{
		case (int)Protocol::S2C_PING:
		{
			break;
		}
		default:
			break;
		}
	}
}
