#pragma once
#include "../Base/Session.h"

namespace bugat
{
	class Connection;
	class Client : public Session
	{
	public:
		Client() 
		{
		};

		virtual ~Client();
		virtual void OnClose();
		virtual void HandleMsg(const std::shared_ptr<TCPRecvPacket>& packet);
	};
}