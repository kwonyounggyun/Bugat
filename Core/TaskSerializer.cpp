#include "pch.h"
#include "TaskSerializer.h"

namespace bugat
{
	int64_t TaskSerializer::Run()
	{
		int64_t size = 0;
		int64_t executeCount = 0;

		size = _que.GetSize();
		for (int i = 0; i < size; i++)
		{
			AnyTask task;
			if (_que.Pop(task))
			{
				task.Run();
				executeCount++;
			}
			else
				break;
		}

		auto taskCount = _taskCount.fetch_sub(executeCount, std::memory_order_acq_rel) - executeCount;
		OnRun(taskCount);
		return executeCount;
	}
}