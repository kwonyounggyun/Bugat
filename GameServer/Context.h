#pragma once
#include <atomic>
#include <array>
#include "../Core/LockFreeQueue.h"

namespace bugat
{
	struct SerializerQueue;
	class SerializeObject;
	class Context
	{
	public:
		Context() : _stop(false) {};
		virtual ~Context() = default;

		void Initialize(uint32_t queCount = 100);
		void run();
		void post(std::weak_ptr<SerializeObject> serializeObject);
		void stop();

	private:
		std::array<std::atomic<SerializerQueue*>, 100> _globalQue;
		LockFreeQueue<SerializerQueue*> _waitQue;

		std::atomic<uint32_t> _swapCounter;
		std::atomic<uint32_t> _globalCounter;
		std::atomic<uint32_t> _threadCounter;

		std::atomic<uint64_t> _executeTaskCounter;
		 
		uint32_t _globalQueSize;
		
		std::atomic<bool> _stop;

	public:
		thread_local static SerializerQueue* _localQue;
	};
}