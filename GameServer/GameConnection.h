#pragma once
#include "BaseConnection.h"

namespace bugat
{
	class GameSession;
	class GameConnection : public BaseConnection
	{
	public:
		GameConnection() {}
		virtual ~GameConnection() {}
	};
}

