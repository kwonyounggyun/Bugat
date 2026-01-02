#pragma once
#include "../Core/Singleton.h"
#include "../Core/ThreadGroup.h"

#include "../Base/Server.h"
#include "../Base/NetworkContext.h"
#include "../Base/Context.h"

#include "SessionManger.h"
#include "GameSession.h"

namespace bugat
{
	class WorldServer : public Server, public SharedSingleton<WorldServer>
	{
	public:
		void Initialize();

	protected:
		// Server을(를) 통해 상속됨
		virtual void OnAccept(std::shared_ptr<Connection>& conn) override;
		// Server을(를) 통해 상속됨
		virtual void Update() override;

	private:
		SessionManager _sessionManager;
	};

#define WorldInstance (*WorldServer::Instance())
}

