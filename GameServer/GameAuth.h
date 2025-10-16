#pragma once
#include <memory>

namespace bugat
{
	class GameConnection;
	class GameAuth
	{
	public:
		static void Handle(std::shared_ptr<GameConnection>& gameConnection, int type, const std::vector<char>& msg);
	};
}