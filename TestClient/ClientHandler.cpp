#include "stdafx.h"
#include "ClientHandler.h"
#include "../Base/Protocol.h"
#include "../Core/Log.h"

using namespace bugat::protocol::game;
namespace bugat
{
	void ClientHandler::Init()
	{
		_handles[static_cast<int>(bugat::protocol::game::Type::RES_SC_LOGIN)] = MAKE_FB_HANDLE(Res_SC_Login);
		_handles[static_cast<int>(bugat::protocol::game::Type::RES_SC_MOVE)] = MAKE_FB_HANDLE(Res_SC_Move);
	}

	void ClientHandler::Handle(std::shared_ptr<Session>& session, const std::shared_ptr<TCPRecvPacket>& packet)
	{
		auto header = packet->GetHeader();
		auto iter = _handles.find(header.type);
		if (iter == _handles.end())
			return;

		auto& handle = *(iter->second);
		handle(session, packet);
	}

	
	DECLARE_FB_HANDLE(DummyClient, Res_SC_Login)
	{
	}

    DECLARE_FB_HANDLE(DummyClient, Res_SC_Move)
    {
        auto pos = packet->pos();
        if (pos)
        {
            InfoLog("{} {} {}", pos->x(), pos->y(), pos->z());
        }
        else
        {
            ErrorLog("Position data is null");
        }
    }
}
