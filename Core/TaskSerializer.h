#pragma once
#include <atomic>

#include "LockFreeQueue.h"
#include "Task.h"
#include "Memory.h"
#include "LockObject.h"

#define TASKSERIALIZER_ERROR UINT64_MAX

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

		/*
		* 반드시 하나의 스레드에서만 호출되어야 한다.
		*/
		int64_t Run();

	protected:
		virtual void OnRun(int64_t remainCount) {}
		virtual void OnPost(int64_t remainCount) {}

	private:
		LockFreeQueue<AnyTask> _que;
		LockObject _runningGuard;
	};
}