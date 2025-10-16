#include "stdafx.h"
#include "GameAuth.h"
#include "GameConnection.h"
#include "../Network/Protocol.h"
#include "SessionManager.h"

namespace bugat
{
	void GameAuth::Handle(std::shared_ptr<GameConnection>& gameConnection, int type, const std::vector<char>& msg)
	{
		if (type == static_cast<int>(protocol::game::Type::REQ_CA_LOGIN))
		{
			auto req = flatbuffers::GetRoot<protocol::game::Req_CA_Login>(msg.data());
			/*
			* 일단은 인증 통과로
			*/

			auto& sessionManager = gameConnection->GetServer()->GetSessionManager();
			if(auto session = sessionManager.FindSession({ 1 }); session != nullptr)
			{
				session->Close();
				gameConnection->Close();
				return;
			}
		}
		else
			gameConnection->Close();
	}
}