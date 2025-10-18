#pragma once
#include "../Core/SerializeObject.h"
#include "../Core/RWLockObject.h"
#include <deque>
#include <queue>
#include <unordered_map>
#include <vector>
#include <atomic>
#include <boost/lockfree/queue.hpp>

namespace bugat
{
	template<typename T>
	using SerializeObject = core::SerializeObject<T>;
	using TaskSerializer = core::TaskSerializer;

	class LogicContext
	{
		struct WeakWrapper
		{
			WeakWrapper(std::weak_ptr<TaskSerializer>& weak) : _weak(weak) {}
			~WeakWrapper() {}
			std::weak_ptr<TaskSerializer> _weak;
		};

		struct SpecialQueue
		{
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

			void push(std::weak_ptr<TaskSerializer>& val)
			{
				_que.push(new WeakWrapper(val));
			}

			boost::lockfree::queue<WeakWrapper*> _que;
		};

	public:
		LogicContext() {};
		virtual ~LogicContext() = default;

		void Initialize(uint32_t threadCount)
		{
			_globalQue.reserve(threadCount);
			for (int i = 0; i < threadCount; i++)
				_globalQue.push_back(std::atomic<std::shared_ptr<SpecialQueue>>(std::make_shared<SpecialQueue>()));

			auto lock = _waitQue.LockWrite();
			for (int i = 0; i < threadCount; i++)
				lock->push(std::make_shared<SpecialQueue>());

			_globalCounter.store(0);
			_threadCounter.store(0);
			_swapCounter.store(0);

			_threadCount = threadCount;
		}

		void run()
		{
			auto threadIdx = _threadCounter.fetch_add(1);
			_localQue = std::make_shared<SpecialQueue>();

			while (true)
			{
				{
					auto lock = _waitQue.LockWrite();

					auto swapIndx = _swapCounter.fetch_add(1) % _globalQue.size();
					auto swap = _globalQue[swapIndx].load(std::memory_order_acquire);
					_globalQue[swapIndx].store(_localQue, std::memory_order_acq_rel);
					lock->push(swap);
					if (false == lock->empty())
					{
						_localQue = lock->front();
						lock->pop();
					}
				}

				_localQue->run();
			}
		}

		template <typename ObjectType>
		void post(std::shared_ptr<SerializeObject<ObjectType>>& serializeObject)
		{
			auto idx = _globalCounter.fetch_add(1) % _globalQue.size();
			_globalQue[idx].load()->push(serializeObject);
		}

	private:
		std::vector<std::atomic<std::shared_ptr<SpecialQueue>>> _globalQue;
		RWLockObject<std::queue<std::shared_ptr<SpecialQueue>>> _waitQue;

		std::atomic<uint32_t> _swapCounter;
		std::atomic<uint32_t> _globalCounter;
		std::atomic<uint32_t> _threadCounter;

		int _threadCount;
		thread_local static std::shared_ptr<SpecialQueue> _localQue;
	};
}