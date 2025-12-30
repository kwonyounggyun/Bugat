#pragma once
#include "LockFreeQueue.h"
#include "Task.h"
#include "AwaitTask.h"

namespace bugat
{
	class TaskSerializer
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
			_que.Push(AnyTask(std::forward<Func>(func), std::forward<ARGS>(args)...));
			OnPost(_taskCount.fetch_add(1, std::memory_order_release) + 1);
		}

		template<typename Await>
		requires std::is_same_v<Await, AwaitTask<typename Await::ValueType>>
		void Post(Await&& awaitable)
		{
			_que.Push(AnyTask(std::forward<Await>(awaitable)));
			OnPost(_taskCount.fetch_add(1, std::memory_order_release) + 1);
		}

		/*
		* 반드시 하나의 스레드에서만 호출되어야 한다.
		*/
		int64_t Run()
		{
			while (true == _runningGuard.test_and_set(std::memory_order_acquire));

			auto size = _taskCount.load(std::memory_order_acquire);
			auto executeCount = _que.Consume(size, [](AnyTask& task)
				{
					task.Run();
				});

			auto preCount = _taskCount.fetch_sub(executeCount, std::memory_order_release);
			_runningGuard.clear(std::memory_order_release);

			OnRun(preCount - executeCount);
			return executeCount;
		}

		virtual void OnRun(int64_t remainCount) {}
		virtual void OnPost(int64_t remainCount) {}

		int64_t GetCount() const { return _taskCount; }

	private:
		LockFreeQueue<AnyTask> _que;
		std::atomic<int64_t> _taskCount{ 0 };
		std::atomic_flag _runningGuard;
	};
}