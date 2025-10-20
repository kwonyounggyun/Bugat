#pragma once
#include "SerializeObject.h"
#include "../Core/RWLockObject.h"
#include <deque>
#include <queue>
#include <unordered_map>
#include <array>
#include <atomic>
#include <type_traits>
#include <boost/lockfree/queue.hpp>

namespace bugat
{
	using TaskSerializer = core::TaskSerializer;

	class Context
	{
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
							sptr->Run();
						delete weakWrapper;
						count++;
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

	public:
		Context() : _threadCount(0), _waitQue(1024) {};
		virtual ~Context() = default;

		void Initialize(uint32_t threadCount)
		{
			for (int i = 0; i < threadCount * 2; i++)
			{
				_globalQue[i].store(new SpecialQueue(), std::memory_order_seq_cst);
				_waitQue.push(new SpecialQueue());
			}

			_globalCounter.store(0);
			_threadCounter.store(0);
			_swapCounter.store(0);

			_threadCount = threadCount;
		}

		void run()
		{
			auto threadIdx = _threadCounter.fetch_add(1);
			_localQue = new SpecialQueue();

			while (true)
			{
				auto swapIndx = _swapCounter.fetch_add(1) % _threadCount;
				auto expect = _globalQue[swapIndx].load();
				while (false == _globalQue[swapIndx].compare_exchange_strong(expect, _localQue));

				_waitQue.push(expect);
				if(true == _waitQue.pop(_localQue))
					auto count = _localQue->run();
			}
		}

		void post(std::weak_ptr<TaskSerializer> serializeObject)
		{
			auto idx = _globalCounter.fetch_add(1) % _threadCount;
			auto que = _globalQue[idx].load();
			while(false == que->push(serializeObject))
				auto que = _globalQue[++idx].load();
		}

	private:
		std::array<std::atomic<SpecialQueue*>, 100> _globalQue;
		boost::lockfree::queue<SpecialQueue*> _waitQue;

		std::atomic<uint32_t> _swapCounter;
		std::atomic<uint32_t> _globalCounter;
		std::atomic<uint32_t> _threadCounter;

		int _threadCount;
		thread_local static SpecialQueue* _localQue;
	};
}