#pragma once
#include <atomic>
#include <array>
#include "../Core/LockFreeQueue.h"
#include "../Core/Counter.h"
#include "../Core/NonCopyable.h"

namespace bugat
{
	class SerializeObject;
	class Context : public NonCopyable
	{
	public:
		Context() : _stop(false) {};
		virtual ~Context() = default;

		void Initialize();
		int64_t Run();
		bool RunOne();
		void Post(std::weak_ptr<SerializeObject> serializeObject);
		void Stop();

	private:
		LockFreeQueue<std::weak_ptr<SerializeObject>> _que;
		Counter<1000> _executeTaskCounter;
		
		std::atomic<bool> _stop;
	};
}