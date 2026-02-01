#include "pch.h"
#include "TaskSerializer.h"

namespace bugat
{
	int64_t TaskSerializer::Run()
	{
		int64_t size = 0;
		int64_t remainCount = 0;
		if (auto lock = ScopedLock(_runningGuard); lock)
		{
			size = _que.GetSize();
			remainCount = _que.Consume(size, [](AnyTask& task)
				{
					task.Run();
				});
		}
		else
		{
			return TASKSERIALIZER_ERROR;
		}

		OnRun(remainCount);
		return size;
	}
}