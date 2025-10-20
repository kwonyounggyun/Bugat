#pragma once
#include "Session.h"

namespace bugat
{
	class GameConnection;
	class GameSession : public Session
	{
	public:
		GameSession(std::shared_ptr<GameConnection>& gameConnection);
		virtual ~GameSession();

	private:
		std::shared_ptr<GameConnection> _gameConnection;
	};
}