#pragma once
#include "../Network/Server.h"
#include "../Core/Singleton.h"

namespace bugat
{
	class LoginServer : public net::Server, public Singleton<LoginServer>
	{
	public:
		LoginServer();
		virtual ~LoginServer();
		virtual void OnAccept(std::shared_ptr<bugat::net::Connection>& conn) override;
	};
}

