#include "stdafx.h"
#include	"Context.h"
#include <thread>

namespace bugat
{
	thread_local SpecialQueue* Context::_localQue = nullptr;

	struct WeakWrapper
	{
		WeakWrapper(std::weak_ptr<TaskSerializer>& weak) : _weak(weak) {}
		~WeakWrapper() {}
		std::weak_ptr<TaskSerializer> _weak;
	};

	struct SpecialQueue
	{
		SpecialQueue() {}
		~SpecialQueue() {}

		int64_t run()
		{
			int64_t count = 0;
			_que.ConsumeAll([&count](WeakWrapper* weakWrapper)
				{
					if (auto sptr = weakWrapper->_weak.lock(); sptr)
					{
						auto executeTaskCount = sptr->Run();
						count += executeTaskCount;
					}
					delete weakWrapper;
				});

			return count;
		}

		bool push(std::weak_ptr<TaskSerializer>& val)
		{
			auto ptr = new WeakWrapper(val);
			_que.Push(ptr);

			return true;
		}

		LockFreeQueue<WeakWrapper*> _que;
	};

	void Context::Initialize(uint32_t threadCount)
	{
		_globalQueSize = threadCount * 2;
		for (int i = 0; i < _globalQueSize; i++)
		{
			_globalQue[i].store(new SpecialQueue(), std::memory_order_seq_cst);
			_waitQue.Push(new SpecialQueue());
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
		_localQue = new SpecialQueue();

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
		que->push(serializeObject);
	}

	void Context::stop()
	{
		_stop.store(true);
	}
}