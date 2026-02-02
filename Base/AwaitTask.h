#pragma once
#include <coroutine>
#include "../Core/Exception.h"
#include "Context.h"
#include "SerializeObject.h"

namespace bugat
{
	struct FinalDestroyer {
		constexpr bool await_ready() const noexcept { return false; }
		void await_suspend(std::coroutine_handle<> h) const noexcept {
			h.destroy();
		}
		constexpr void await_resume() const noexcept {}
	};

	template<typename T, typename Executor = SerializeObject>
	class AwaitTask
	{
	public:
		using ValueType = T;

		struct promise_type;
		using HandleType = std::coroutine_handle<promise_type>;
		
		struct promise_type
		{
			promise_type() : _executor(nullptr) {}
			template<typename ...ARGS>
			promise_type(Executor& executor, ARGS&&... args) : _executor(&executor) {}
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

			void unhandled_exception() 
			{
#if defined(_DEBUG)
				try
				{
					std::rethrow_exception(std::current_exception());
				}
				catch(std::exception e)
				{
					throw CoroutineException(e);
				}
#else
				std::terminate();
#endif
			}

			T _value;
			TSharedPtr<Executor> _executor;
			std::function<void(T)> _callfunc;
		};

		void resume() const
		{
			if (_h && !_h.done())
				_h.resume();
		}

		template<typename CallerType>
		struct Await
		{
			Await(CallerType& caller, HandleType h) : _caller(caller), _h(h) {}
			bool await_ready() { return false; }
			void await_suspend(std::coroutine_handle<> h)
			{
				_h.promise()._callfunc = [h, this](T value) {
					_result = value;
					_caller->Post([h]() {
						h.resume();
						});
					};

				if (_h.promise()._executor != nullptr)
				{
					_h.promise()._executor->Post([copy = _h]() {
						copy.resume();
						});
				}
				else
					_h.resume();
			}
			T await_resume()
			{
				return _result;
			}

			HandleType _h;
			CallerType _caller;
			T _result;
		};

		auto operator co_await()->Await<std::decay_t<decltype(Context::Executor())>>
		{
			return Await(Context::Executor(), _h);
		}

		HandleType _h;
	};

	template<typename Executor>
	class AwaitTask<void, Executor>
	{
	public:
		using ValueType = void;

		struct promise_type;
		using HandleType = std::coroutine_handle<promise_type>;

		struct promise_type
		{
			promise_type() : _executor(nullptr) {}
			template<typename ...ARGS>
			promise_type(Executor& executor, ARGS&&... args) : _executor(&executor) {}
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

			void unhandled_exception()
			{
#if defined(_DEBUG)
				try
				{
					std::rethrow_exception(std::current_exception());
				}
				catch (std::exception e)
				{
					throw CoroutineException(e);
				}
#else
				std::terminate();
#endif
			}

			TSharedPtr<Executor> _executor;
			std::function<void()> _callfunc;
		};

		void resume() const
		{
			if (_h && !_h.done())
				_h.resume();
		}

		template<typename CallerType>
		struct Await
		{
			Await(CallerType& caller, HandleType h) : _caller(caller), _h(h) {}
			bool await_ready() { return false; }
			void await_suspend(std::coroutine_handle<> h)
			{
				_h.promise()._callfunc = [h, this]() {
					_caller->Post([h]() {
						h.resume();
						});
					};

				if (_h.promise()._executor != nullptr)
				{
					_h.promise()._executor->Post([h = _h]() {
						h.resume();
						});
				}
				else
					_h.resume();
			}
			void await_resume() {}

			HandleType _h;
			CallerType _caller;
		};

		auto operator co_await()->Await<std::decay_t<decltype(Context::Executor())>>
		{
			return Await(Context::Executor(), _h);
		}

		HandleType _h;
	};

	struct AwaitAlways
	{
		AwaitAlways(TaskSerializer* serializer) : _serializer(serializer) {}
		bool await_ready() const noexcept { return false; }
		void await_suspend(std::coroutine_handle<> h) noexcept
		{
			_serializer->Post([h]() {
				h.resume();
				});
		}

		void await_resume()
		{
		}

		TaskSerializer* _serializer;
	};
}