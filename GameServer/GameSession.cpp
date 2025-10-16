#include "stdafx.h"
#include "GameSession.h"

#include "GameConnection.h"

namespace bugat
{
	GameSession::GameSession(std::shared_ptr<GameConnection>& gameConnection) : _gameConnection(gameConnection)
	{
	}

	GameSession::~GameSession()
	{ 
	}
}