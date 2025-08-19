#pragma once
#include "RedisCommand.h"

#include <coroutine>

template<typename T>
class Awaitable
{
public:
	struct promise_type
	{
		Awaitable<typename T> get_return_object()
		{
			return Awaitable<typename T>{ std::coroutine_handle<promise_type>::from_promise(*this) };
		}

		// Initial suspend point (suspend immediately)
		std::suspend_always initial_suspend() { return {}; }

		// Final suspend point (suspend before destruction)
		std::suspend_always final_suspend() noexcept { return {}; }

		void return_value(T value) {
			_value = std::move(value);
		}

		// Handle exceptions
		void unhandled_exception() {
		}

		std::optional<T> _value;
		std::exception_ptr _error;
		std::coroutine_handle<> _continuation;
	};

	struct awaiter
	{
		Awaitable& self;
		bool await_ready() const noexcept 
		{
			return false;
		}

		void await_suspend(std::coroutine_handle<> h) noexcept
		{
			self._handler.promise()._continuation = h;
		}

		T await_resume() 
		{
			if (self._handler.promise()._error)
			{
				std::rethrow_exception(self._handler.promise()._error);
			}
			return std::move(*(self._handler.promise()._value));
		}

		auto operator co_await() noexcept
		{
			return awaiter{ *this };
		}
	};

	std::coroutine_handle<promise_type> _handler;
};

template<>
class Awaitable<void>
{
public:
	struct promise_type
	{
		Awaitable<void> get_return_object()
		{
			return Awaitable<void>{ std::coroutine_handle<promise_type>::from_promise(*this) };
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
		std::coroutine_handle<> _continuation;
	};

	struct awaiter
	{
		Awaitable& self;
		bool await_ready() const noexcept
		{
			return false;
		}

		void await_suspend(std::coroutine_handle<> h) noexcept
		{
			self._handler.promise()._continuation = h;
		}

		void await_resume()
		{
			if (self._handler.promise()._error)
			{
				std::rethrow_exception(self._handler.promise()._error);
			}
		}

		auto operator co_await() noexcept
		{
			return awaiter{ *this };
		}
	};

	std::coroutine_handle<promise_type> _handler;
};

class Impl;
class RedisContext;
class RedisClient
{
public:
    class RedisRequest
    {
    public:
        RedisRequest();
        ~RedisRequest();

        void AddCommand(const std::string& command, const std::initializer_list<std::string>& params);

        std::unique_ptr<Impl> _pImpl;
    };

    template<typename ...CMD>
    class RedisCommands
    {
    public:
        using ResultType = std::tuple<typename CMD::resType...>;

        RedisCommands(CMD& ...cmds) {
            (cmds.Command(_req), ...);
        }
        ~RedisCommands() {}

        RedisRequest _req;
    };

public:
    RedisClient();
    ~RedisClient();

	auto Start(std::string host, int port, int threadCount) -> Awaitable<void>;
	auto Set(const std::string& key, const std::string& value) -> Awaitable<void>;

    template<typename ...CMDS>
    RedisCommands<CMDS...>::ResultType Execute(const CMDS&... cmd)
    {
        RedisCommands command((cmd, ...));
        command._req._pImpl;
        return Execute(command);
    }

private:
    template<typename CMD>
    CMD::ResultType Execute(const CMD& cmd)
    {
        cmd.GetRequest();
    }

private:
    std::unique_ptr<RedisContext> _context;
};

