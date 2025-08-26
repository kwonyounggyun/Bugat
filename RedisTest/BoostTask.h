#pragma once
#include <coroutine>
#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>

template<typename T, typename Executor>
class BoostTask;

template<typename Executor>
class BoostTask<void, Executor>
{
public:
    explicit BoostTask(boost::asio::io_context& io, boost::asio::awaitable<void, Executor>&& awaitable) : _io(io), _awaitable(std::move(awaitable)){}
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
            boost::asio::co_spawn(self._io, self._awaitable, [h](std::exception_ptr e) {
                h.resume();
                });
        }
        void await_resume() {}
    };

    awaiter operator co_await() noexcept
    {
        return awaiter{ *this };
    }

public:
    boost::asio::io_context& _io;
    boost::asio::awaitable<void, Executor> _awaitable;
    std::coroutine_handle<promise_type> _handle;
};

template<typename T, typename Executor>
class BoostTask
{
public:
    BoostTask(boost::asio::io_context& io, boost::asio::awaitable<T, Executor>&& awaitable) : _io(io), _awaitable(std::move(awaitable)) {}
    struct promise_type
    {
        BoostTask get_return_object() { return BoostTask{ std::coroutine_handle<promise_type>::from_promise(*this) }; }
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        auto return_value() 
        {
            return _value;
        }
        void unhandled_exception() { std::exit(1); }

        std::optional<T> _value;
    };

    struct awaiter
    {
        BoostTask& self;
        bool await_ready() const noexcept { return false; }
        void await_suspend(std::coroutine_handle<> h) noexcept
        {
            boost::asio::co_spawn(self._io, self._awaitable, [h](std::exception_ptr e, T result){
                h.resume();
                });
        }
        void await_resume() {}
    };

    awaiter operator co_await() noexcept
    {
        return awaiter{ *this };
    }

    boost::asio::io_context& _io;
    boost::asio::awaitable<T, Executor> _awaitable;
    std::coroutine_handle<promise_type> _handle;
};