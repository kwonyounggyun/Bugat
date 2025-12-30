// Network.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "framework.h"
#include "NetworkMessage.h"
#include "NetworkContext.h"
#include "Connection.h"
#include "Server.h"
#include "Configure.h"
#include "Context.h"


using namespace bugat;
class TestServer : public bugat::Server
{
public:
	// Server을(를) 통해 상속됨
	void OnAccept(std::shared_ptr<Connection>& conn) override
	{
	}

	void Start()
	{
		
	}
};

Context _logic;
NetworkContext _network;

// TODO: This is an example of a library function
void fnNetwork()
{
	auto server = CreateSerializeObject<TestServer>(&_logic);
	
	Configure config;
	config.port = 9000;

	bugat::CoSpawn(server, server->Accept(_network.GetExecutor(), ConnectionFactory<Connection>(), config));
}
