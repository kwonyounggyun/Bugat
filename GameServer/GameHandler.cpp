#include "stdafx.h"
#include "GameHandler.h"
#include "../Base/Protocol.h"

namespace bugat
{
	void GameHandler::Init()
	{
		_handles[static_cast<int>(bugat::protocol::game::Type::REQ_CS_MOVE)] = MAKE_FB_HANDLE(Req_CS_Move);
	}

	void GameHandler::Handle(std::shared_ptr<Session>& session, const std::shared_ptr<RecvPacket>& packet)
	{
		auto header = packet->GetHeader();
		auto iter = _handles.find(header.type);
		if (iter == _handles.end())
			return;
		
		auto& handle = iter->second;
		(*handle)(session, packet);
	}
}