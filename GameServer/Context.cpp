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
		SpecialQueue() : _que(1024) {}
		~SpecialQueue() {}

		int64_t run()
		{
			int64_t count = 0;
			_que.consume_all([&count](WeakWrapper* weakWrapper)
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
			if (false == _que.push(ptr))
			{
				delete ptr;
				return false;
			}

			return true;
		}

		boost::lockfree::queue<WeakWrapper*> _que;
	};

	void Context::Initialize(uint32_t threadCount)
	{
		_globalQueSize = threadCount * 2;
		for (int i = 0; i < _globalQueSize; i++)
		{
			_globalQue[i].store(new SpecialQueue(), std::memory_order_seq_cst);
			_waitQue.push(new SpecialQueue());
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

			_waitQue.push(expect);
			if (true == _waitQue.pop(_localQue))
			{
				auto count = _localQue->run();
				auto preCount = _executeTaskCounter.fetch_add(count);
				if ((preCount + count) >= 100000000)
					stop();
			}

			std::this_thread::sleep_for(std::chrono::microseconds(1));
		}
	}
	void Context::post(std::weak_ptr<TaskSerializer> serializeObject)
	{
		auto idx = _globalCounter.fetch_add(1) % _globalQueSize;
		auto que = _globalQue[idx].load();
		while (false == que->push(serializeObject))
			auto que = _globalQue[++idx].load();
	}

	void Context::stop()
	{
		_stop.store(true);
	}
}