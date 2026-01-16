#include "stdafx.h"
#include "GameSession.h"
#include "GameConnection.h"
#include "GameHandler.h"

namespace bugat
{
	GameSession::GameSession()
	{
	}

	GameSession::~GameSession()
	{ 
	}

	void GameSession::HandleMsg(const TSharedPtr<TCPRecvPacket>& packet)
	{
		auto sptr = TSharedPtr(this);
		Post([sptr, packet]() {
			auto session = StaticPointerCast<Session>(sptr);
			GameHandler::Instance().Handle(session, packet);
			});
	}
}