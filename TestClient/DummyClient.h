#pragma once
#include "../Base/Session.h"
#include "../Base/UDPConnection.h"

namespace bugat
{
	class Connection;
	class DummyClient : public Session
	{
	public:
		DummyClient();
		virtual ~DummyClient();
		virtual void HandleMsg(const std::shared_ptr<TCPRecvPacket>& packet);

		UDPConnection& GetUDPConnection() { return *_udpConnection; }

	private:
		std::shared_ptr<UDPConnection> _udpConnection;
	};
}