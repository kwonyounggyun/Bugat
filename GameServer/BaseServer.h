#pragma once
#include "../Network/Server.h"
#include "Context.h"

#include <map>

namespace bugat
{
	namespace net
	{
		class Connection;
		class AnyConnectionFactory;
		class Configure;
	}

	class BaseServer : public net::Server
	{
	public:
		BaseServer() {}
		virtual ~BaseServer() {}
		
		virtual void Start(net::AnyConnectionFactory&& factory, net::Configure& config);
		virtual void OnAccept(std::shared_ptr<bugat::net::Connection>& conn) override {}
		
		auto& GetContext() { return _context; }
	private:
		Context _context;
	};
}

