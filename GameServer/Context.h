#pragma once
#include "SerializeObject.h"
#include <queue>
#include <array>
#include <atomic>
#include "../Core/LockFreeQueue.h"

namespace bugat
{
	using TaskSerializer = core::TaskSerializer;

	struct SerializerQueue;
	class Context
	{
	public:
		Context() : _threadCount(0), _stop(false) {};
		virtual ~Context() = default;

		void Initialize(uint32_t threadCount);
		void run();
		void post(std::weak_ptr<TaskSerializer> serializeObject);
		void stop();

	private:
		std::array<std::atomic<SerializerQueue*>, 100> _globalQue;
		LockFreeQueue<SerializerQueue*> _waitQue;

		std::atomic<uint32_t> _swapCounter;
		std::atomic<uint32_t> _globalCounter;
		std::atomic<uint32_t> _threadCounter;

		std::atomic<uint64_t> _executeTaskCounter;

		uint32_t _threadCount;
		uint32_t _globalQueSize;
		
		std::atomic<bool> _stop;

	public:
		thread_local static SerializerQueue* _localQue;
	};
}