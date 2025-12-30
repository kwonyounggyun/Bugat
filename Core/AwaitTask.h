#pragma once
#include <coroutine>
#include "Event.h"

namespace bugat
{
	template<typename T>
	class AwaitTask
	{
	public:
		using ValueType = T;

		struct promise_type;
		using HandleType = std::coroutine_handle<promise_type>;

		struct FinalDestroyer : std::suspend_always {
			void await_suspend(HandleType h) const noexcept {
				auto s = std::move(h.promise()._self);
			}
		};

		struct HandleHolder
		{
			HandleHolder(HandleType h) : _h(h) {}
			~HandleHolder()
			{
				if (_h)
					_h.destroy();
			}

			HandleType _h;
		};
		
		struct promise_type
		{
			AwaitTask get_return_object()
			{
				auto h = HandleType::from_promise(*this);
				_self = std::make_shared<HandleHolder>(h);
				return AwaitTask{ h };
			}

			auto initial_suspend() { return std::suspend_always{}; }
			auto final_suspend() noexcept 
			{
				if (_continuation)
					_continuation.resume();

				return FinalDestroyer{};
			}

			void return_value(T value) 
			{
				_value = value;
			}

			void unhandled_exception() {}

			T _value;
			std::coroutine_handle<> _continuation;
			std::shared_ptr<HandleHolder> _self;
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

		void resume() const
		{
			if (_h && !_h.done())
				_h.resume();
		}

		Awaiter operator co_await()
		{
			return Awaiter{ _h };
		}

		HandleType _h;
	};

	template<>
	class AwaitTask<void>
	{
	public:
		using ValueType = void;

		struct promise_type;
		using HandleType = std::coroutine_handle<promise_type>;

		struct FinalDestroyer : std::suspend_always {
			void await_suspend(HandleType h) const noexcept {
				auto s = std::move(h.promise()._self);
			}
		};

		struct HandleHolder
		{
			HandleHolder(HandleType h) : _h(h) {}
			~HandleHolder()
			{
				if (_h)
					_h.destroy();
			}

			HandleType _h;
		};

		struct promise_type
		{
			AwaitTask get_return_object()
			{
				auto h = HandleType::from_promise(*this);
				_self = std::make_shared<HandleHolder>(h);
				return AwaitTask{ h };
			}

			auto initial_suspend() { return std::suspend_always{}; }
			auto final_suspend() noexcept
			{
				if (_continuation)
					_continuation.resume();

				return FinalDestroyer{};
			}

			void return_void() {}

			void unhandled_exception() {}

			std::coroutine_handle<> _continuation;
			std::shared_ptr<HandleHolder> _self;
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

		void resume() const
		{
			if (_h && !_h.done())
				_h.resume();
		}

		Awaiter operator co_await()
		{
			return Awaiter{ _h };
		}

		HandleType _h;
	};
}