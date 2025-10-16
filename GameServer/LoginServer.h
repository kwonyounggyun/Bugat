#pragma once
#include "BaseServer.h"

namespace bugat
{
	class LoginServer : public BaseServer
	{
	public:
		LoginServer();
		virtual ~LoginServer();
		virtual void OnAccept(std::shared_ptr<bugat::net::Connection>& conn) override;
	};
}

