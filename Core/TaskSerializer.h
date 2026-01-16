#pragma once
#include <atomic>

#include "LockFreeQueue.h"
#include "Task.h"
#include "AwaitTask.h"
#include "Memory.h"
#include "LockObject.h"

namespace bugat
{
	class TaskSerializer : public RefCountable
	{
	public:
		TaskSerializer() {}
		virtual ~TaskSerializer()
		{
			_que.Clear();
		}

		template<typename Func, typename ...ARGS>
		requires std::invocable<Func, ARGS&...>
		void Post(Func&& func, ARGS&&... args)
		{
			OnPost(_que.Push(AnyTask(std::forward<Func>(func), std::forward<ARGS>(args)...)));
		}

		template<typename Await>
		requires std::is_same_v<Await, AwaitTask<typename Await::ValueType>>
		void Post(Await&& awaitable)
		{
			OnPost(_que.Push(AnyTask(std::forward<Await>(awaitable))));
		}

		/*
		* 반드시 하나의 스레드에서만 호출되어야 한다.
		*/
		int64_t Run();

		virtual void OnRun(int64_t remainCount) {}
		virtual void OnPost(int64_t remainCount) {}

	private:
		LockFreeQueue<AnyTask> _que;
		LockObject _runningGuard;
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