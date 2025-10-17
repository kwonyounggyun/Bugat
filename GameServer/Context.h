#pragma once
#include "../Core/SerializeObject.h"
#include <deque>
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
				_deque.consume_all([&count](WeakWrapper* weakWrapper)
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
				_deque.push(new WeakWrapper(val));
			}

			boost::lockfree::queue<WeakWrapper*> _deque;
		};

	public:
		LogicContext() {}
		virtual ~LogicContext() = default;
		void run()
		{
			auto threadIdx = _threadIndex.fetch_add(1);
			_localQue = std::make_shared<SpecialQueue>();
			_globalQue.emplace(threadIdx, std::make_shared<SpecialQueue>());

			while (true)
			{
				_localQue->run();

				auto loadQue = _globalQue[threadIdx].load();
				_globalQue[threadIdx].store(_localQue);
				_localQue = loadQue;
			}
		}

		template <typename ObjectType>
		void post(std::shared_ptr<SerializeObject<ObjectType>>& serializeObject)
		{
			auto idx = _globalIndex.fetch_add(1) % _threadIndex.load();
			_globalQue[idx].load()->push_back(serializeObject);
		}

		std::unordered_map<uint32_t, std::atomic<std::shared_ptr<SpecialQueue>>> _globalQue;
		std::atomic<uint32_t> _globalIndex;
		std::atomic<uint32_t> _threadIndex;
		thread_local static std::shared_ptr<SpecialQueue> _localQue;
	};
}