#pragma once
#include "../Base/Session.h"

namespace bugat
{
	class GameSession : public Session
	{
	public:
		GameSession();
		virtual ~GameSession();

		virtual void HandleMsg(const std::shared_ptr<RecvPacket>& packet) override;
		virtual void OnClose() {}
	};
}