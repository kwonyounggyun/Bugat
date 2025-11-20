#pragma once
#include "Session.h"

namespace bugat
{
	class GameSession : public Session
	{
	public:
		GameSession();
		virtual ~GameSession();

		virtual void HandleMsg(const net::Header& header, const std::vector<char>& msg) override;
		virtual void OnClose() {}
	};
}