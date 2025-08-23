#pragma once
#include <coroutine>
#include <optional>
#include <exception>

template<typename T>
class Task;

template<>
class Task<void>
{
public:
	struct promise_type
	{
		Task<void> get_return_object()
		{
			return Task<void>{ std::coroutine_handle<Task<void>::promise_type>::from_promise(*this) };
		}

		// Initial suspend point (suspend immediately)
		std::suspend_always initial_suspend() { return {}; }

		// Final suspend point (suspend before destruction)
		std::suspend_always final_suspend() noexcept { return {}; }

		// Handle co_return (for void return)
		void return_void() {}

		// Handle exceptions
		void unhandled_exception() {
		}

		std::exception_ptr _error;
	};

	struct awaiter
	{
		Task& self;
		bool await_ready() const noexcept
		{
			return false;
		}

		void await_suspend(std::coroutine_handle<> h) noexcept
		{
			_h = h;
		}

		void await_resume()
		{
			if (self._handle.promise()._error)
			{
				std::rethrow_exception(self._handle.promise()._error);
			}
		}

		std::coroutine_handle<> _h
	};

	using handle = std::coroutine_handle<promise_type>;
	explicit Task(handle h) : _handle(h) {}
	Task(Task&& other) noexcept : _handle(std::exchange(other._handle, {})) {}
	~Task() { if (_handle) _handle.destroy(); }

	auto operator co_await() noexcept
	{
		return awaiter{ *this };
	}

	handle _handle;
};

template<typename T>
class Task
{
public:
	struct promise_type
	{
		Task<T> get_return_object()
		{
			return Task<T>{ std::coroutine_handle<promise_type>::from_promise(*this) };
		}

		// Initial suspend point (suspend immediately)
		std::suspend_always initial_suspend() { return {}; }

		// Final suspend point (suspend before destruction)
		std::suspend_always final_suspend() noexcept { return {}; }

		void return_value(const T& value) {
			_value = std::move(value);
		}

		// Handle exceptions
		void unhandled_exception() {
			_error = std::current_exception();
		}

		T _value;
		std::exception_ptr _error;
	};

	struct awaitable
	{
		Task& self;
		bool await_ready() const noexcept
		{
			return false;
		}

		void await_suspend(std::coroutine_handle<> h) noexcept
		{
			_continuation = h;
		}

		T await_resume()
		{
			if (self._handle.promise()._error)
			{
				std::rethrow_exception(self._handle.promise()._error);
			}
			return std::move(self._handle.promise()._value);
		}
	};

	using handle = std::coroutine_handle<promise_type>;
	explicit Task(handle h) : _handle(h) {}
	Task(Task&& other) noexcept : _handle(std::exchange(other._handle, {})) {}
	~Task() { if (_handle) _handle.destroy(); }

	auto operator co_await() noexcept
	{
		return awaitable{ *this };
	}

	handle _handle;
};

template<class Awaitable>
auto spawn_as_task(boost::asio::any_io_executor ex, Awaitable aw)
{
	using result = typename Awaitable::value_type;
	struct State {
		std::shared_ptr<State> _self;
		Task<result> _task;
		boost::asio::any_io_executor _ex;
		explicit State(Task<result>&& task, boost::asio::any_io_executor ex) : _task(std::move(task)), _ex(std::move(ex)) {}
	};

	auto st = std::make_shared<State>(Task<result>{}, ex);
	st->_self = st;

	co_await boost::asio::co_spawn(
		ex,
		std::move(aw),
		boost::asio::use_awaitable);
}

template<class Awaitable>
auto spawn_as_task_void(boost::asio::any_io_executor ex, Awaitable aw) -> Task<typename boost::asio::awaitable<Awaitable>::value_type>
{
	struct State {
		std::shared_ptr<State> self_keep;
		Task<void> task;
		boost::asio::any_io_executor ex;
		explicit State(Task<void>&& t, (boost::asio::any_io_executor e) : task(std::move(t)), ex(std::move(e)) {}
	};
	Task<void> t = []() -> Task<void> { co_return; }();
	auto st = std::make_shared<State>(std::move(t), ex);
	st->self_keep = st;

	boost::asio::co_spawn(
		ex,
		std::move(aw),
		[st](std::exception_ptr ep) {
			auto& p = st->task._handle.promise();
			if (ep) p._error = ep;
			if (p._value) boost::asio::dispatch(st->ex, [c = p.cont] { c.resume(); });
			st->self_keep.reset();
		});

	return std::move(st->task);
}