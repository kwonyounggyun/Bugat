#pragma once
#include <coroutine>
#include <optional>
#include <exception>

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
			_continuation = h;
		}

		void await_resume()
		{
			if (self._handle.promise()._error)
			{
				std::rethrow_exception(self._handle.promise()._error);
			}
		}
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