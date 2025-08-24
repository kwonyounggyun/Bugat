#pragma once
#include "RedisCommand.h"
#include "Task.h"

class Impl;
class RedisContext;

template<typename Func>
class BoostTask
{
public:
    BoostTask(Func&& f) : _f(std::move(f)) {}
    struct promise_type
    {
        BoostTask get_return_object() { return BoostTask{ std::coroutine_handle<promise_type>::from_promise(*this) }; }
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void return_void() {}
        void unhandled_exception() { std::exit(1); }
    };

    struct awaiter
    {
        BoostTask& self;
        bool await_ready() const noexcept { return false; }
        void await_suspend(std::coroutine_handle<> h) noexcept
        {
            self->_f(h);
        }
        void await_resume() {}
    };

    awaiter operator co_await() noexcept
    {
        return awaiter{ *this };
    }

    Func _f;
    std::coroutine_handle<promise_type> _handle;
};

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

	auto Start(std::string host, int port, int threadCount) -> Task<void>;
    auto Set(const std::string& key, const std::string& value) -> BoostTask<void>;

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

