#pragma once
#include "../Network/Server.h"
#include "Session.h"

#include <map>

namespace bugat
{
	class BaseServer : public bugat::net::Server
	{
	public:
		BaseServer() {};
		virtual ~BaseServer() {};
		virtual void AfterAccept(std::shared_ptr<bugat::net::Connection>& conn) override 
		{
			
		};

	private:
		std::map<uint64_t, std::shared_ptr<bugat::net::Connection>> _waitAuth;
	};
}

