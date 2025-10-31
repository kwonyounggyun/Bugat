#pragma once
#include "../Network/Server.h"
#include "../Network/NetworkContext.h"
#include "Context.h"
#include "../Core/Singleton.h"
#include "../Core/ThreadGroup.h"
#include "SessionManger.h"

namespace bugat
{
	namespace net
	{
		class Connection;
	}

	class WorldServer : public net::Server, public Singleton<WorldServer>
	{
	public:
		void Initialize();
		auto& GetLogicContext() { return _logicContext; }
		auto& GetClientContext() { return _netClientContext; }
		auto& GetServerContext() { return _netServerContext; }

		// Server을(를) 통해 상속됨
		void OnAccept(std::shared_ptr<net::Connection>& conn) override;

	private:


		SessionManager<GameSession> _sessionManager;

		// 클라이언트 메세지 처리용
		net::NetworkContext _netClientContext;
		// 서버간 메세지, API 처리용
		net::NetworkContext _netServerContext;
		// 게임 로직 처리용
		Context _logicContext;

		ThreadGroup _threadGroup;
	};

#define GetWorld WorldServer::Instance()
#define WorldLogicContext GetWorld.GetLogicContext()
#define WorldClientContext GetWorld.GetClientContext()
#define WorldServerContext GetWorld.GetServerContext()
}

