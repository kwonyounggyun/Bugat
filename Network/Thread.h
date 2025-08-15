#pragma once
#include <thread>
#include <queue>
#include <functional>

template<typename T>
class Thread
{
	Thread(std::function<void ()>&& loopFunc) : _thread(std::move(loopFunc))
	{
	}

	std::thread _thread;
	std::queue<typename T::task_type> _taskQueue;
};
