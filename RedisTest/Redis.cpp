//#include "Redis.h"
//#include <boost/redis/connection.hpp>
//#include <boost/asio/co_spawn.hpp>
//#include <boost/asio/detached.hpp>
//#include <boost/lockfree/stack.hpp>
//#include <boost/redis/request.hpp>
//#include <boost/asio/awaitable.hpp>
//
//class Impl
//{
//public:
//	Impl() {}
//	~Impl() {}
//
//	Impl(const Impl& other) = delete;
//	Impl& operator=(const Impl& other) = delete;
//
//	boost::redis::request req;
//	std::vector<boost::redis::resp3::node> nodes;
//};
//
//void AddCommand(RedisClient::RedisRequest& req, const std::string& command, const std::initializer_list<std::string>& params)
//{
//	req.AddCommand(command, params);
//}
//
//RedisClient::RedisRequest::RedisRequest() : _pImpl(std::make_unique<Impl>())
//{
//}
//
//RedisClient::RedisRequest::~RedisRequest()
//{
//	_pImpl.reset();
//}
//
//void RedisClient::RedisRequest::AddCommand(const std::string& command, const std::initializer_list<std::string>& params)
//{
//	_pImpl->req.push_range(command, params.begin(), params.end());
//}
//
//auto Connect(std::shared_ptr<boost::redis::connection>& conn, boost::redis::config& cfg) -> boost::asio::awaitable<void>
//{
//	auto ex = co_await boost::asio::this_coro::executor;
//	conn = std::make_shared<boost::redis::connection>(ex);
//	co_return conn->async_run(cfg, {}, boost::asio::consign(boost::asio::detached, conn));
//}
//
//class RedisContext
//{
//public:
//	boost::asio::io_context _io;
//	std::shared_ptr<boost::redis::connection> _conn{ nullptr };
//};
//
//RedisClient::RedisClient() : _context(std::make_unique<RedisContext>())
//{
//	_context->_io.run();
//}
//
//RedisClient::~RedisClient()
//{
//	_context->_io.stop();
//}
//
//auto RedisClient::Start(std::string host, int port, int threadCount) -> Task<void>
//{
//	boost::redis::config config;
//	config.addr = boost::redis::address{ host, std::to_string(port) };
//	boost::asio::co_spawn(_context->_io, Connect(_context->_conn, config), boost::asio::detached);
//}
//
//auto CommandSet(std::shared_ptr<boost::redis::connection>& conn, const std::string& key, const std::string& value) -> boost::asio::awaitable<void>
//{
//	boost::redis::response<boost::redis::ignore_t> resp;
//	boost::redis::request req;
//	req.push("SET", "key", "value");
//	co_await conn->async_exec(req, resp);
//	co_return;
//}
//
//auto RedisClient::Set(const std::string& key, const std::string& value) -> BoostTask<void>
//{
//	auto task = BoostTask([&](std::coroutine_handle<> h) { 
//		boost::asio::co_spawn(_context->_io, CommandSet(_context->_conn, key, value), [h]() {
//			h.resume();
//			});
//		});
//	co_await task;
//	co_return;
//}