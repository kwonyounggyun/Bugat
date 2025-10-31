#include "stdafx.h"
#include "GameHandler.h"
#include "../Network/Protocol.h"

#include "Session.h"

namespace bugat
{
	void GameHandler::Handle(std::shared_ptr<Session>& session, const net::Header& header, const std::vector<char>& msg)
	{
		if (false == session->IsAuth())
		{
			if (header.type == static_cast<int>(protocol::game::Type::REQ_CA_LOGIN))
			{
				auto req = flatbuffers::GetRoot<protocol::game::Req_CA_Login>(msg.data());
				auto id = req->id();
				auto pw = req->pw();
			}
		}
		else
		{
			if (header.type == static_cast<int>(protocol::game::Type::REQ_CA_LOGIN))
				return;

			(*_handles[header.type])();
		}
	}
}