#include "stdafx.h"
#include	"Context.h"
#include <thread>

namespace bugat
{
	thread_local SerializerQueue* Context::_localQue = nullptr;

	struct SerializerQueue
	{
		SerializerQueue() {}
		~SerializerQueue() {}

		int64_t run()
		{
			int64_t count = 0;
			_que.ConsumeAll([&count](std::weak_ptr<TaskSerializer>& weak)
				{
					if (auto sptr = weak.lock(); sptr)
					{
						auto executeTaskCount = sptr->Run();
						count += executeTaskCount;
					}
				});

			return count;
		}

		bool push(std::weak_ptr<TaskSerializer>&& val)
		{
			_que.Push(std::move(val));

			return true;
		}

		LockFreeQueue<std::weak_ptr<TaskSerializer>> _que;
	};

	void Context::Initialize(uint32_t threadCount)
	{
		_globalQueSize = threadCount * 2;
		for (int i = 0; i < _globalQueSize; i++)
		{
			_globalQue[i].store(new SerializerQueue(), std::memory_order_seq_cst);
			_waitQue.Push(new SerializerQueue());
		}

		_globalCounter.store(0);
		_threadCounter.store(0);
		_swapCounter.store(0);

		_threadCount = threadCount;

		_executeTaskCounter.store(0);
	}

	void Context::run()
	{
		auto threadIdx = _threadCounter.fetch_add(1);
		_localQue = new SerializerQueue();

		while (false == _stop.load())
		{
			auto swapIndx = _swapCounter.fetch_add(1) % _globalQueSize;
			auto expect = _globalQue[swapIndx].load();
			while (false == _globalQue[swapIndx].compare_exchange_strong(expect, _localQue));

			_waitQue.Push(expect);
			if (true == _waitQue.Pop(_localQue))
			{
				auto count = _localQue->run();
				if (count > 0)
					_executeTaskCounter.fetch_add(count);
			}

			std::this_thread::sleep_for(std::chrono::microseconds(1));
		}
	}
	void Context::post(std::weak_ptr<TaskSerializer> serializeObject)
	{
		auto idx = _globalCounter.fetch_add(1) % _globalQueSize;
		auto que = _globalQue[idx].load();
		que->push(std::move(serializeObject));
	}

	void Context::stop()
	{
		_stop.store(true);
	}
}