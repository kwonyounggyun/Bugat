#include "pch.h"
#include "NetworkContext.h"
#include "Configure.h"
#include "Connection.h"
#include "boost/asio.hpp"
#include "boost/asio/co_spawn.hpp"

namespace bugat::net
{
	NetworkContext::NetworkContext() : _ioContext(std::make_shared<boost::asio::io_context>()), _stop(false)
	{

	}

	void NetworkContext::Initialize(Server* server, AnyConnectionFactory factory, Configure config)
	{
		boost::asio::co_spawn(*_ioContext,
			[ioContext = _ioContext, server, factory = std::move(factory), port = config.port]()->boost::asio::awaitable<void> {
				auto executor = co_await boost::asio::this_coro::executor;
				tcp::acceptor acceptor(executor, { tcp::v4(), port });
				for (;;)
				{
					auto socket = std::make_unique<tcp::socket>(*ioContext);
					co_await acceptor.async_accept(*socket, boost::asio::use_awaitable);
					auto connection = factory.Create(socket);
					server->OnAccept(connection);
					connection->Start();
				}
			}
		, boost::asio::detached);
	}

	void NetworkContext::Run()
	{
		while (false == _stop.load(std::memory_order_acquire))
			_ioContext->run();
	}

	void NetworkContext::Post(std::function<void()>&& func)
	{
		boost::asio::post(*_ioContext, func);
	}

	void NetworkContext::Stop()
	{
		_stop.store(true, std::memory_order_release);
	}
}