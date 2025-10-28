#include "pch.h"
#include "Server.h"
#include "Configure.h"
#include "Connection.h"
#include "NetworkMessage.h"

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/read.hpp>

namespace bugat::net
{
	Server::Server()
	{
	}

	Server::~Server()
	{
	}
}
