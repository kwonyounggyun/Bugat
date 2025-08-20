//#include "Redis.h"
//#include <coroutine>
//#include <boost/redis/connection.hpp>
//#include <boost/asio/co_spawn.hpp>
//#include <boost/asio/detached.hpp>
//#include <boost/lockfree/stack.hpp>
//#include <boost/redis/request.hpp>
//#include <boost/asio/awaitable.hpp>
//
//template<class Awaitable>
//auto spawn_as_task(boost::asio::any_io_executor ex, Awaitable aw) -> Task<typename Awaitable::value_type>
//{
//	using result = typename Awaitable::value_type;
//	struct State {
//		std::shared_ptr<State> _self;
//		Task<result> _task;
//		boost::asio::any_io_executor _ex;
//		explicit State(Task<result>&& task, boost::asio::any_io_executor ex) : _task(std::move(task)), _ex(std::move(ex)) {}
//	};
//
//	Task<result> t = []() -> Task<result> { co_return result{}; }();
//	auto st = std::make_shared<State>(std::move(t), ex);
//	st->_self = st;
//
//	boost::asio::co_spawn(
//		ex,
//		std::move(aw),
//		[st](std::exception_ptr ep) {
//			auto& p = st->_task._handle.promise();
//			if (ep) p._error = ep;
//			if (p._continuation) boost::asio::dispatch(st->_ex, [c = p._continuation] { c.resume(); });
//			st->_self.reset();
//		});
//
//	return std::move(st->_task);
//}
//
//template<class Awaitable>
//auto spawn_as_task_void(boost::asio::any_io_executor ex, Awaitable aw) -> Task<typename boost::asio::awaitable<Awaitable>::value_type>
//{
//	struct State {
//		std::shared_ptr<State> self_keep;
//		Task<void> task;
//		(boost::asio::any_io_executor ex;
//		explicit State(Task<void>&& t, (boost::asio::any_io_executor e) : task(std::move(t)), ex(std::move(e)) {}
//	};
//	Task<void> t = []() -> Task<void> { co_return; }();
//	auto st = std::make_shared<State>(std::move(t), ex);
//	st->self_keep = st;
//
//	boost::asio::co_spawn(
//		ex,
//		std::move(aw),
//		[st](std::exception_ptr ep) {
//			auto& p = st->task._handle.promise();
//			if (ep) p._error = ep;
//			if (p._value) boost::asio::dispatch(st->ex, [c = p.cont] { c.resume(); });
//			st->self_keep.reset();
//		});
//
//	return std::move(st->task);
//}
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
//auto RedisClient::Start(std::string host, int port, int threadCount) -> Awaitable<void>
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
//auto RedisClient::Set(const std::string& key, const std::string& value) -> Task<void>
//{
//	Task<void> t;
//	t._handle.resume();
//	boost::asio::co_spawn(_context->_io, CommandSet(_context->_conn, key, value), boost::asio::detached);
//	Awaitable<void> waiter;
//	return Awaitable<void>();
//}