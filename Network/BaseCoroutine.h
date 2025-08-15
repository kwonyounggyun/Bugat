#pragma once
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
			_value = value;
		}

		// Handle exceptions
		void unhandled_exception() {
		}

		T _value;
	};
private:
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
	};

private:
	std::coroutine_handle<promise_type> _handler;
};