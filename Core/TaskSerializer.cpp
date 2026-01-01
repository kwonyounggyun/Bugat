#include "pch.h"
#include "TaskSerializer.h"

namespace bugat
{
	int64_t TaskSerializer::Run()
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
}