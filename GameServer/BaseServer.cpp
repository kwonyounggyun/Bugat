#include "stdafx.h"
#include "BaseServer.h"
#include "../Network/Configure.h"

namespace bugat
{
    void BaseServer::Start(net::AnyConnectionFactory&& factory, net::Configure& config)
    {
        net::Server::Start(std::move(factory), config);
    }
}