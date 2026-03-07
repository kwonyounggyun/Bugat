#include "pch.h"
#include "TaskSerializer.h"

namespace bugat
{
	int64_t TaskSerializer::Run()
	{
		int64_t size = 0;
		int64_t executeCount = 0;
		if (auto lock = ScopedLock(_runningGuard); lock)
		{
			size = _que.GetSize();
			executeCount = _que.Consume(size, [](AnyTask& task)
				{
					task.Run();
				});
		}
		else
		{
			OnRun(TASKSERIALIZER_ERROR);
			return 0;
		}

		OnRun(_taskCount.fetch_sub(executeCount) - executeCount);
		return executeCount;
	}
}