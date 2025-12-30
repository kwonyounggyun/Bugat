#pragma once
#include "../Base/Session.h"

namespace bugat
{
	struct Connection;
}

namespace bugat
{
	class Client : public Session
	{
	public:
		Client() 
		{
		};

		virtual ~Client();
		virtual void OnClose();
		virtual void HandleMsg(const std::shared_ptr<RecvPacket>& packet);
	};
}