// Network.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "framework.h"
#include "NetworkMessage.h"
#include "Connection.h"
#include "Server.h"
#include "Configure.h"

// TODO: This is an example of a library function
void fnNetwork()
{
	bugat::net::Server server;
	bugat::net::Configure config;
	config.ioThreadCount = 5;
	config.port = 9000;
	
	server.Start(bugat::net::ConnectionFactory<bugat::net::Connection>(), config);
	//bugat::net::NetworkMessage msg;
}
