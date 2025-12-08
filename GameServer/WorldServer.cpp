#include "stdafx.h"
#include "WorldServer.h"
#include "GameConnection.h"
#include "GameSession.h"
#include "../Network/Configure.h"
#include "Template.h"
#include "../Core/Log.h"

namespace bugat
{

	void WorldServer::Initialize()
	{
		GetLogicContext().Initialize(10);
		_threadGroup.Add(5, [this](ThreadInfo& info) {
			GetLogicContext().run();
			});

		net::Configure client;
		client.port = 5000;
		GetClientContext().Initialize(this, net::ConnectionFactory<GameConnection>(), client);
		_threadGroup.Add(5, [this](ThreadInfo& info) {
			GetClientContext().Run();
			});

		net::Configure server;
		server.port = 6000;
		GetClientContext().Initialize(this, net::ConnectionFactory<GameConnection>(), server);
		_threadGroup.Add(5, [this](ThreadInfo& info) {
			GetClientContext().Run();
			});
	}

	std::atomic<int> worldcount = 0;
	void WorldServer::OnAccept(std::shared_ptr<net::Connection>& conn)
	{
		auto gameSession = CreateSerializeObject<GameSession>(WorldLogicContext);
		gameSession->SetConnection(conn);
		std::weak_ptr<GameSession> weak = gameSession;

		conn->OnAccept += []() {
			auto value = worldcount.fetch_add(1);
			InfoLog("{}", value + 1);
			};
		conn->OnClose += [weak]() {
			auto value = worldcount.fetch_sub(1);
			InfoLog("{}", value - 1);

			if (auto session = weak.lock(); session)
				session->Close();
			};
		conn->OnRead += [weak](const net::Header& header, const std::vector<char>& msg) {
			if(auto session = weak.lock(); session)
				session->HandleMsg(header, msg);
			};

		PlayerId temp;
		temp.pid = 1;
		_sessionManager.AddSession(temp, gameSession);
	}
}