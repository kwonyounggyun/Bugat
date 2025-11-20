#include "stdafx.h"
#include "GameHandler.h"
#include "../Network/Protocol.h"

namespace bugat
{
	void GameHandler::Init()
	{
		_handles[static_cast<int>(bugat::protocol::game::Type::REQ_CA_LOGIN)] = MAKE_HANDLE(Req_CA_Login);
	}

	void GameHandler::Handle(std::shared_ptr<Session>& session, const net::Header& header, const std::vector<char>& msg)
	{
		auto iter = _handles.find(header.type);
		if (iter == _handles.end())
			return;
		
		(*_handles[header.type])(session, header, msg);
	}
}