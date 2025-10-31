#include "stdafx.h"
#include "WorldServer.h"
#include "GameConnection.h"
#include "GameSession.h"
#include "../Network/Configure.h"
#include "Template.h"
#include "GameAuth.h"

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

	void WorldServer::OnAccept(std::shared_ptr<net::Connection>& conn)
	{
		auto gameSession = CreateSerializeObject<GameSession>(WorldLogicContext);
		gameSession->SetConnection(conn);
		_sessionManager.
	}
}