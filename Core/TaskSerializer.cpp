#include "pch.h"
#include "TaskSerializer.h"

namespace bugat
{
	int64_t TaskSerializer::Run()
	{
		int64_t size = _taskCount.load(std::memory_order_acquire);
		int64_t executeCount = 0;
		if (auto lock = ScopedLock(_runningGuard); lock)
		{
			for (int i = 0; i < size; i++)
			{
				auto result = _que.ConsumeOne([](AnyTask& task) {
						task.Run();
					});
				if (result)
					executeCount++;
			}
		}
		else
		{
			OnRun(TASKSERIALIZER_ERROR);
			return 0;
		}

		auto remain = _taskCount.fetch_sub(executeCount, std::memory_order_acq_rel) - executeCount;
		OnRun(remain);
		return executeCount;
	}
}