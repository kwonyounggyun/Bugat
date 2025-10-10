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
	/*boost::asio::awaitable<void> listener(Server* server, AnyConnectionFactory& factory, unsigned short port)
	{
		auto executor = co_await boost::asio::this_coro::executor;
		tcp::acceptor acceptor(executor, { tcp::v4(), port });
		for (;;)
		{
			tcp::socket socket = co_await acceptor.async_accept(boost::asio::use_awaitable);
			auto connection = factory.Create(server->GetIOContext(), socket);
			server->Accept(connection);
		}
	}*/

	Server::Server()
	{
	}

	Server::~Server()
	{
	}

	void Server::Start(AnyConnectionFactory factory, Configure config)
	{
		for (int i = 0; i < config.ioThreadCount; i++)
		{
			_ioThreads.emplace_back([this]() {
				while (true)
				{
					_ioContext.run();
				}
				});
		}

		//boost::asio::co_spawn(_ioContext, listener(this, factory, config.port), boost::asio::detached);
		boost::asio::co_spawn(_ioContext,
			[this, factory = std::move(factory), port = config.port]()->boost::asio::awaitable<void> {
				auto executor = co_await boost::asio::this_coro::executor;
				tcp::acceptor acceptor(executor, { tcp::v4(), port });
				for (;;)
				{
					auto connection = factory.Create(_ioContext);
					co_await acceptor.async_accept(*connection->_socket, boost::asio::use_awaitable);
					
					Accept(connection);
				}
			}
		, boost::asio::detached);
	}

	void Server::Accept(std::shared_ptr<Connection>& conn)
	{
		conn->Read();
		AfterAccept(conn);
	}
}
