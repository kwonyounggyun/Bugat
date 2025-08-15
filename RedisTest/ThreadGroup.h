#pragma once
#include <functional>
#include <vector>
#include <thread>
#include <algorithm>
#include "AnyContext.h"

/*
* type T should be a class that has a run() and post() method.
*/
class ThreadGroup
{
public:
	template<typename T>
	ThreadGroup(T context) : _context(std::make_unique<AnyContext>(context)){};
	~ThreadGroup() {};

	bool Start(int threadCount)
	{
		if (_stop.load())
			return false;

		_threads.resize(threadCount, std::thread([this]() {
			while (!_stop.load())
			{
				try {
					_context->run();
				}
				catch (const std::exception& e) {
				}
			}
			}));

		return true;
	}

	void Stop()
	{
		auto expect = false;
		if (_stop.compare_exchange_strong(expect, true))
		{
			std::for_each(_threads.begin(), _threads.end(), [](std::thread& t) {
				t.join();
				});

			_threads.clear();
		}
	}

	void Post(typename T::task_type&& task)
	{
		_context.post(std::move(task));
	}

private:
	std::unique_ptr<AnyContext> _context;
	std::atomic_bool _stop{ false };
	std::vector<std::thread> _threads;
};