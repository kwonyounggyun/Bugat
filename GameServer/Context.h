#pragma once
#include "SerializeObject.h"
#include <queue>
#include <array>
#include <atomic>
#include <boost/lockfree/queue.hpp>

namespace bugat
{
	using TaskSerializer = core::TaskSerializer;

	struct SpecialQueue;
	class Context
	{
	public:
		Context() : _threadCount(0), _waitQue(1024) {};
		virtual ~Context() = default;

		void Initialize(uint32_t threadCount);
		void run();
		void post(std::weak_ptr<TaskSerializer> serializeObject);

	private:
		std::array<std::atomic<SpecialQueue*>, 100> _globalQue;
		boost::lockfree::queue<SpecialQueue*> _waitQue;

		std::atomic<uint32_t> _swapCounter;
		std::atomic<uint32_t> _globalCounter;
		std::atomic<uint32_t> _threadCounter;

		std::atomic<uint32_t> _executeTaskCounter;

		uint32_t _threadCount;
		uint32_t _globalQueSize;
	public:
		thread_local static SpecialQueue* _localQue;
	};
}