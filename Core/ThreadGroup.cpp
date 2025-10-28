#include "pch.h"
#include "ThreadGroup.h"

namespace bugat
{
	bool ThreadGroup::IsStop()
	{
		return _stop.load(std::memory_order_acquire);
	}

	bool ThreadGroup::Join()
	{
		if (false == IsStop())
			return false;

		for (auto& thread : _threads)
		{
			thread->join();
			delete thread;
		}

		_threads.clear();

		return true;
	}

	void ThreadGroup::Stop()
	{
		_stop.store(true, std::memory_order_release);
	}
}