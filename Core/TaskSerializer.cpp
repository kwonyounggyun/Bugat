#include "pch.h"
#include "TaskSerializer.h"

namespace bugat
{
	int64_t TaskSerializer::Run()
	{
		while (true == _runningGuard.test_and_set(std::memory_order_acquire));

		auto size = _que.GetSize();
		auto remainCount = _que.Consume(size, [](AnyTask& task)
			{
				task.Run();
			});

		_runningGuard.clear(std::memory_order_release);

		OnRun(remainCount);
		return size;
	}
}