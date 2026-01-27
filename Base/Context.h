#pragma once
#include <atomic>
#include <vector>
#include "../Core/LockFreeQueue.h"
#include "../Core/Memory.h"

namespace bugat
{
	class SerializerQueue;
	class SerializeObject;

	class Context
	{
		friend class SerializerQueue;
	public:
		explicit Context(uint32_t queCount = 10);
		virtual ~Context() = default;

		void Initialize();
		void Run();
		void Post(TSharedPtr<SerializeObject> serializeObject);
		void Stop();
		
		static TSharedPtr<SerializeObject>& Executor();

	private:
		std::vector<CacheLinePadding<std::atomic<SerializerQueue*>>> _globalQue;
		LockFreeQueue<SerializerQueue*> _waitQue;

		CacheLinePadding<std::atomic<uint32_t>> _swapCounter;
		CacheLinePadding<std::atomic<uint32_t>> _globalCounter;
		CacheLinePadding<std::atomic<uint64_t>> _executeTaskCounter;

		std::atomic<uint32_t> _threadCounter;
		uint32_t _globalQueSize;
		std::atomic<bool> _stop;

		thread_local static SerializerQueue* _localQue;
		thread_local static TSharedPtr<SerializeObject> _currentExecutor;
	};
}