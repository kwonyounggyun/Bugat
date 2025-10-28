#pragma once
#include <thread>
#include <vector>
#include <atomic>
#include <type_traits>

namespace bugat
{
	struct ThreadInfo
	{
		std::thread::id _id;
		int _index;
	};

	class ThreadGroup
	{
	public:
		ThreadGroup() {}
		~ThreadGroup() {}

		template<typename TaskFunc, typename ...ARGS>
		requires std::invocable<TaskFunc, ThreadInfo&, ARGS&...>
		void Add(int threadCount, TaskFunc&& task, ARGS&... args)
		{
			for (int i = 0; i < threadCount; i++)
			{
				_threads.push_back(new std::thread([this, i, task, args...]() mutable {
					ThreadInfo info;
					info._id = std::this_thread::get_id();
					info._index = i;
					while (false == IsStop())
					{
						task(info, args...);
					}
					})
				);
			}
		}

		bool IsStop();
		void Stop();
		bool Join();

	private:
		std::vector<std::thread*> _threads;
		std::atomic_bool _stop;
	};
}

