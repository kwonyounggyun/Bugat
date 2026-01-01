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

	void GameSession::HandleMsg(const std::shared_ptr<TCPRecvPacket>& packet)
	{
		Post([weak = weak_from_this(), packet]() {
			if (auto sPtr = weak.lock(); sPtr != nullptr)
			{
				auto session = std::static_pointer_cast<Session>(sPtr);
				GameHandler::Instance().Handle(session, packet);
			}
			});
	}
}