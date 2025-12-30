#include "stdafx.h"
#include "ClientHandler.h"
#include "../Base/Protocol.h"

using namespace bugat::protocol::game;
namespace bugat
{
	void ClientHandler::Init()
	{
		_handles[static_cast<int>(bugat::protocol::game::Type::REQ_CS_MOVE)] = MAKE_FB_HANDLE(Req_CS_Move);
	}

	void ClientHandler::Handle(std::shared_ptr<Session>& session, const std::shared_ptr<RecvPacket>& packet)
	{
		auto header = packet->GetHeader();
		auto iter = _handles.find(header.type);
		if (iter == _handles.end())
			return;

		auto& handle = *(iter->second);
		handle(session, packet);
	}

	
	DECLARE_FB_HANDLE(Req_CS_Login)
	{
	}

	DECLARE_FB_HANDLE(Req_CS_Move)
	{
		auto pos = data->pos();
		auto fb = FBCreate();
		fb->Finish(CreateRes_SC_Move(*fb, pos));

		session->Send(static_cast<int>(bugat::protocol::game::Type::RES_SC_MOVE), fb);
	}
}
