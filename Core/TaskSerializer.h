#pragma once
#include <atomic>

#include "LockFreeQueue.h"
#include "Task.h"
#include "Memory.h"
#include "LockObject.h"

namespace bugat
{
	constexpr int64_t TASKSERIALIZER_ERROR = (1LL << 60);

	class TaskSerializer : public RefCountable
	{
	public:
		TaskSerializer() : _taskCount(0) {}
		virtual ~TaskSerializer()
		{
			_que.Clear();
		}

		template<typename Func, typename ...ARGS>
		requires std::invocable<Func, ARGS&...>
		void Post(Func&& func, ARGS&&... args)
		{
			if (true == _que.Push(AnyTask(std::forward<Func>(func), std::forward<ARGS>(args)...)))
			{
				auto taskCount = _taskCount.fetch_add(1, std::memory_order_acq_rel) + 1;
				OnPost(taskCount);
			}
			else
				OnPost(TASKSERIALIZER_ERROR);
		}

		/*
		* 반드시 하나의 스레드에서만 호출되어야 한다.
		*/
		int64_t Run();

	protected:
		virtual void OnRun(int64_t remainCount) {}
		virtual void OnPost(int64_t remainCount) {}

	private:
		LockFreeQueue<AnyTask> _que;
		std::atomic<int64_t> _taskCount;
	};
}