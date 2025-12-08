#pragma once
#include <memory>
#include <unordered_map>
#include "BaseHandle.h"
#include "../Core/Singleton.h"

namespace bugat
{
	class GameSession;
	class GameHandler : public Singleton<GameHandler>
	{
	public:
		void Handle(std::shared_ptr<Session>& session, const net::Header& header, const std::vector<char>& msg);
		void Init();

	private:
		std::unordered_map<int, std::shared_ptr<BaseHandle>> _handles;
	};

	DEFINE_FB_HANDLE(Req_CS_Login)
	DEFINE_FB_HANDLE(Req_CS_Move)
}