#include "Redis.h"
#include <coroutine>
#include <boost/redis/connection.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/lockfree/stack.hpp>

class Impl
{
public:
	Impl() {}
	~Impl() {}

	Impl(const Impl& other) = delete;
	Impl& operator=(const Impl& other) = delete;

	boost::redis::request req;
	std::vector<boost::redis::resp3::node> nodes;
};

void AddCommand(RedisClient::RedisRequest& req, const std::string& command, const std::initializer_list<std::string>& params)
{
	req.AddCommand(command, params);
}

RedisClient::RedisRequest::RedisRequest() : _pImpl(std::make_unique<Impl>())
{
}

RedisClient::RedisRequest::~RedisRequest()
{
	_pImpl.reset();
}

void RedisClient::RedisRequest::AddCommand(const std::string& command, const std::initializer_list<std::string>& params)
{
	_pImpl->req.push_range(command, params.begin(), params.end());
}

auto Connect(std::shared_ptr<boost::redis::connection>& conn, boost::redis::config& cfg) -> boost::asio::awaitable<void>
{
	auto ex = co_await boost::asio::this_coro::executor;
	conn = std::make_shared<boost::redis::connection>(ex);
	conn->async_run(cfg, {}, boost::asio::consign(boost::asio::detached, conn));
}

class RedisContext
{
public:
	boost::asio::io_context _io;
	std::shared_ptr<boost::redis::connection> _conn{ nullptr };
};

RedisClient::RedisClient() : _context(std::make_unique<RedisContext>())
{
	_context->_io.run();
}

RedisClient::~RedisClient()
{
	_context->_io.stop();
}

void RedisClient::Start(std::string host, int port, int threadCount)
{
	boost::redis::config config;
	config.addr = boost::redis::address{ host, std::to_string(port)};
	boost::asio::co_spawn(_context->_io, Connect(_context->_conn, config), boost::asio::detached);


}
