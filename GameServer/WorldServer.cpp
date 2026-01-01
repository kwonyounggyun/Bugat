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

		auto id = conn->GetObjectId();
		conn->OnAccept += [id, gameSession, this]() {
			InfoLog("Connect : {}", id.String());

			PlayerId temp;
			temp.pid = tempcount.fetch_add(1);
			_sessionManager.AddSession(temp, gameSession);
			};
		conn->OnClose += [weak, id]() {
			InfoLog("Disconnect : {}", id.String());

			if (auto session = weak.lock(); session)
				session->Close();
			};
		conn->OnRead += [weak](const std::shared_ptr<RecvPacket>& packet) {
			if(auto session = weak.lock(); session)
				session->HandleMsg(packet);
			};
	}
	void WorldServer::Update()
	{
	}
}