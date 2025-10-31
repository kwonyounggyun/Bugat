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

	void GameSession::HandleMsg(const net::Header& header, const std::vector<char>& msg)
	{
		Post([weak = weak_from_this()](const net::Header& header, const std::vector<char>& msg) mutable {
			if (auto sPtr = weak.lock(); sPtr != nullptr)
			{
				auto gameSession = std::static_pointer_cast<GameSession>(std::move(sPtr));
				GameHandler::Handle(gameSession, header, msg);
			}
			}, header, msg);
	}
}