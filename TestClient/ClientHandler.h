#pragma once
#include "../Core/Singleton.h"
#include "../Base/Handle.h"
#include "../Base/Packet.h"

#include "DummyClient.h"
#include <unordered_map>

namespace bugat
{
	class ClientHandler : public Singleton<ClientHandler>
	{
	public:
		void Handle(std::shared_ptr<Session>& session, const std::shared_ptr<TCPRecvPacket>& packet);
		void Init();

	private:
		std::unordered_map<int, std::shared_ptr<TCPPacketHandle<DummyClient>>> _handles;
	};

	DEFINE_FB_HANDLE(DummyClient, Res_SC_Login)
	DEFINE_FB_HANDLE(DummyClient, Res_SC_Move)
}
