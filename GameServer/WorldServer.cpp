#include "stdafx.h"
#include "WorldServer.h"
#include "GameConnection.h"
#include "GameSession.h"
#include "../Base/Log.h"
#include "../Base/PlayerId.h"

namespace bugat
{

	void WorldServer::Initialize()
	{
	}

	std::atomic<int> tempcount = 0;
	void WorldServer::OnAccept(TSharedPtr<Connection>& conn)
	{
		auto gameSession = CreateSerializeObject<GameSession>(GetContext());
		gameSession->SetConnection(conn);

		auto sessionId = gameSession->GetObjectId();
		gameSession->OnClose += [this, sessionId]() {
			_sessionManager.Del(sessionId);
			};

		conn->OnConnect += [sessionId, gameSession, this]() mutable {
			_sessionManager.Add(gameSession->GetObjectId(), gameSession);
			};

		conn->OnClose += [gameSession]() {
			gameSession->Close();
			};

		conn->OnRead += [gameSession](const TSharedPtr<TCPRecvPacket>& packet) {
			gameSession->HandleMsg(packet);
			};
	}

	void WorldServer::Update()
	{
	}
}