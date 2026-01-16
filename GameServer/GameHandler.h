#pragma once
#include <memory>
#include <unordered_map>
#include "../Base/Handle.h"
#include "../Core/Singleton.h"
#include "GameSession.h"

namespace bugat
{
	class GameHandler : public Singleton<GameHandler>
	{
	public:
		void Handle(TSharedPtr<Session>& session, const TSharedPtr<TCPRecvPacket>& packet);
		void Init();

	private:
		std::unordered_map<int, TSharedPtr<TCPPacketHandle<GameSession>>> _handles;
	};

	DEFINE_FB_HANDLE(GameSession, Req_CS_Login)
	DEFINE_FB_HANDLE(GameSession, Req_CS_Move)
}