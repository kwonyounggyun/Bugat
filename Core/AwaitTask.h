#pragma once
#include <optional>
#include <coroutine>
#include <memory>

namespace bugat
{
	template<typename T>
	class AwaitTask
	{
	public:
		using ValueType = T;

		struct promise_type;
		using HandleType = std::coroutine_handle<promise_type>;
		
		struct promise_type
		{
			AwaitTask get_return_object()
			{
				return AwaitTask{ HandleType::from_promise(*this) };
			}

			auto initial_suspend() { return std::suspend_always{}; }
			auto final_suspend() noexcept 
			{
				if (_continuation)
					_continuation.resume();

				return std::suspend_always{}; 
			}

			void return_value(T value) 
			{
				_value = value;
			}

			void unhandled_exception() {}

			T _value;
			std::coroutine_handle<> _continuation;
		};

		struct Awaiter
		{
			bool await_ready() const noexcept { return _h.done(); }
			void await_suspend(std::coroutine_handle<> awaiting_handle) noexcept
			{
				_h.promise()._continuation = awaiting_handle;
				_h.resume();
			}

			T await_resume()
			{
				return _h.promise()._value;
			}

			HandleType _h;
		};

		AwaitTask(HandleType handle) : _h(handle) {}
		~AwaitTask()
		{
			if (_h)
				_h.destroy();
		}

		void resume()
		{
			if (!_h.done())
				_h.resume();
		}

		Awaiter operator co_await()
		{
			return Awaiter{ _h };
		}

	private:
		HandleType _h;
	};

	template<>
	class AwaitTask<void>
	{
	public:
		using ValueType = void;

		struct promise_type;
		using HandleType = std::coroutine_handle<promise_type>;

		struct promise_type
		{
			AwaitTask get_return_object()
			{
				return AwaitTask{ HandleType::from_promise(*this) };
			}

			auto initial_suspend() { return std::suspend_always{}; }
			auto final_suspend() noexcept
			{
				if (_continuation)
					_continuation.resume();

				return std::suspend_always{};
			}

			void return_void() {}

			void unhandled_exception() {}

			std::coroutine_handle<> _continuation;
		};

		struct Awaiter
		{
			bool await_ready() const noexcept { return _h.done(); }
			void await_suspend(std::coroutine_handle<> awaiting_handle) noexcept
			{
				_h.promise()._continuation = awaiting_handle;
				_h.resume();
			}

			void await_resume() {	}

			HandleType _h;
		};

		AwaitTask(HandleType handle) : _h(handle) {}
		~AwaitTask()
		{
			if (_h)
				_h.destroy();
		}

		void resume()
		{
			if (!_h.done())
				_h.resume();
		}

		Awaiter operator co_await()
		{
			return Awaiter{ _h };
		}

	private:
		HandleType _h;
	};
}