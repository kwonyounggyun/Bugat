#pragma once
#include "BaseServer.h"

namespace bugat
{
	class AuthServer : public BaseServer
	{
	public:
		AuthServer();
		virtual ~AuthServer();
		virtual void AfterAccept(std::shared_ptr<bugat::net::Connection>& conn) override;
	};
}

