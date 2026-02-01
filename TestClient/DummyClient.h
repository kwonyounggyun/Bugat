#pragma once
#include <atomic>
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
		virtual void HandleMsg(const TSharedPtr<TCPRecvPacket>& packet);

		UDPConnection& GetUDPConnection() { return *_udpConnection; }
		int GetCount() { return _count.fetch_add(1); }

	private:
		std::shared_ptr<UDPConnection> _udpConnection;
		std::atomic<int> _count;
	};
}