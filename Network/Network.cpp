// Network.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "framework.h"
#include "NetworkMessage.h"
#include "NetworkContext.h"
#include "Connection.h"
#include "Server.h"
#include "Configure.h"


using namespace bugat;
class TestServer : public bugat::net::Server
{
public:
	// Server을(를) 통해 상속됨
	void OnAccept(std::shared_ptr<net::Connection>& conn) override
	{
	}

	void Start()
	{
		net::Configure config;
		config.ioThreadCount = 5;
		config.port = 9000;

		_context.Initialize(this, net::ConnectionFactory<net::Connection>(), config);
	}

private:
	net::NetworkContext _context;
};

// TODO: This is an example of a library function
void fnNetwork()
{
	TestServer server;
	
	
	server.Start();
	//bugat::net::NetworkMessage msg;
}
