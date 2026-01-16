#pragma once
#include "../Base/Session.h"

namespace bugat
{
	class GameSession : public Session
	{
	public:
		GameSession();
		virtual ~GameSession();

		virtual void HandleMsg(const TSharedPtr<TCPRecvPacket>& packet) override;
	};
}