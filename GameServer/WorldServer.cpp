#include "stdafx.h"
#include "WorldServer.h"
#include "GameConnection.h"
#include "GameSession.h"
#include "../Core/Log.h"
#include "../Base/PlayerId.h"

namespace bugat
{

	void WorldServer::Initialize()
	{
	}

	std::atomic<int> tempcount = 0;
	void WorldServer::OnAccept(std::shared_ptr<Connection>& conn)
	{
		auto gameSession = CreateSerializeObject<GameSession>(GetContext());
		gameSession->SetConnection(conn);
		std::weak_ptr<GameSession> weak = gameSession;

		auto sessionId = gameSession->GetObjectId();
		gameSession->OnClose += [this, sessionId]() {
			_sessionManager.Del(sessionId);
			};

		conn->OnConnect += [sessionId, gameSession, this]() mutable {
			_sessionManager.Add(gameSession->GetObjectId(), gameSession);
			};

		conn->OnClose += [weak]() {
			if (auto session = weak.lock(); session)
				session->Close();
			};

		conn->OnRead += [weak](const std::shared_ptr<TCPRecvPacket>& packet) {
			if(auto session = weak.lock(); session)
				session->HandleMsg(packet);
			};
	}

	void WorldServer::Update()
	{
	}
}