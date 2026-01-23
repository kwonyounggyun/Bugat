#pragma once
#include <coroutine>
#include "Event.h"

namespace bugat
{
	struct FinalDestroyer {
		constexpr bool await_ready() const noexcept { return false; }
		void await_suspend(std::coroutine_handle<> h) const noexcept {
			h.destroy();
		}
		constexpr void await_resume() const noexcept {}
	};

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
				auto h = HandleType::from_promise(*this);
				return AwaitTask{ h };
			}

			auto initial_suspend() { return std::suspend_always{}; }
			auto final_suspend() noexcept 
			{
				if (_callfunc)
					_callfunc(_value);

				return FinalDestroyer{};
			}

			void return_value(T value) 
			{
				_value = value;
			}

			void unhandled_exception() {}

			T _value;
			std::function<void(T)> _callfunc;
		};

		void resume() const
		{
			if (_h && !_h.done())
				_h.resume();
		}

		void SetCallback(std::function<void(T)>&& callfunc)
		{
			_h.promise()._callfunc = std::move(callfunc);
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

		struct promise_type
		{
			AwaitTask get_return_object()
			{
				auto h = HandleType::from_promise(*this);
				return AwaitTask{ h };
			}

			auto initial_suspend() { return std::suspend_always{}; }
			auto final_suspend() noexcept
			{
				if (_callfunc)
					_callfunc();

				return FinalDestroyer{ };
			}

			void return_void() {}

			void unhandled_exception() {}

			std::function<void()> _callfunc;
		};

		void resume() const
		{
			if (_h && !_h.done())
				_h.resume();
		}

		void SetCallback(std::function<void()>&& callfunc)
		{
			_h.promise()._callfunc = std::move(callfunc);
		}

		HandleType _h;
	};
}