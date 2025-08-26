#include "../Core.h"
#include "BaseServer.h"
#include "Configure.h"
#include "Connection.h"

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/write.hpp>

using boost::asio::ip::tcp;
using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;
namespace this_coro = boost::asio::this_coro;

awaitable<void> listener(boost::asio::io_context& context, Configure config)
{
	auto executor = co_await this_coro::executor;
	tcp::acceptor acceptor(executor, { tcp::v4(), config.port });
	for (;;)
	{
		tcp::socket socket = co_await acceptor.async_accept(use_awaitable);
		auto connection = Connection(socket);
		//co_spawn(context, echo(std::move(socket)), detached);
	}
}

void BaseServer::Start(Configure config)
{
	boost::asio::io_context context;

	co_spawn(context, listener(context, config), detached);
}

