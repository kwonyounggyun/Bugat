#pragma once
#include "../Base/PlayerId.h"
#include "../Base/ObjectManager.h"
#include "GameSession.h"

namespace bugat
{
	struct PlayerIdHash
	{
		int operator()(PlayerId& key, int size)
		{
			return key.pid % size;
		}
	};
	
	struct GameSessionIDHash
	{
		int operator()(GameSession::ID_Type& key, int size)
		{
			return key.count % size;
		}
	};

	using SessionManager = ObjectManager<GameSessionIDHash, GameSession::ID_Type, GameSession, 2000>;
}