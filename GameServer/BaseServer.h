#pragma once
#include "../Network/Server.h"

namespace bugat
{
	class BaseServer : public bugat::net::Server
	{
	public:
		BaseServer() {};
		virtual ~BaseServer() {};
		virtual void AfterAccept(std::shared_ptr<bugat::net::Connection>& conn) override;
	};
}

