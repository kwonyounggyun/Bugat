#pragma once
#include <memory>
#include <unordered_map>
#include "../Base/Handle.h"
#include "../Core/Singleton.h"

namespace bugat
{
	class GameSession;
	class GameHandler : public Singleton<GameHandler>
	{
	public:
		void Handle(std::shared_ptr<Session>& session, const std::shared_ptr<RecvPacket>& packet);
		void Init();

	private:
		std::unordered_map<int, std::shared_ptr<BaseHandle>> _handles;
	};

	DEFINE_FB_HANDLE(Req_CS_Login)
	DEFINE_FB_HANDLE(Req_CS_Move)
}