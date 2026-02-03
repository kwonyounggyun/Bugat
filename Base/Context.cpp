#include "pch.h"
#include	"Context.h"
#include <thread>
#include <stacktrace>
#include "SerializeObject.h"
#include "../Core/Exception.h"

namespace bugat
{
	thread_local SerializerQueue* Context::_localQue = nullptr;
	thread_local TSharedPtr<SerializeObject> Context::_currentExecutor = nullptr;

	class SerializerQueue
	{
	public:
		SerializerQueue() {}
		~SerializerQueue() {}

		int64_t Run()
		{
			int64_t count = 0;
			auto& currentExecutor = Context::_currentExecutor;
			while (true == _que.Pop(currentExecutor))
			{
				if (currentExecutor)
				{
					auto executeTaskCount = currentExecutor->Run();
					count += executeTaskCount;
				}
			}
			currentExecutor = nullptr;

			return count;
		}

		bool push(TSharedPtr<SerializeObject>&& val)
		{
			_que.Push(std::move(val));

			return true;
		}

	private:
		LockFreeQueue<TSharedPtr<SerializeObject>> _que;
	};

	Context::Context(uint32_t queCount) : _stop(false), _globalQueSize(queCount), _globalQue(queCount)
	{
		_globalCounter->store(0);
		_swapCounter->store(0);
		_executeTaskCounter->store(0);

		_threadCounter.store(0);
	}

	Context::~Context()
	{
		for (auto& que : _globalQue)
			delete que->load(std::memory_order_acquire);

		_waitQue.ConsumeAll([](SerializerQueue* que) {
			delete que;
			});
	}

	void Context::Initialize()
	{
		for (uint32_t i = 0; i < _globalQueSize; i++)
		{
			_globalQue[i]->store(new SerializerQueue(), std::memory_order_seq_cst);
			_waitQue.Push(new SerializerQueue());
		}
	}

	void Context::Run()
	{
		auto threadIdx = _threadCounter.fetch_add(1);
		_localQue = new SerializerQueue();

		while (false == _stop.load())
		{
			if (_localQue == nullptr)
				return;
			auto swapIndx = _swapCounter->fetch_add(1) % _globalQueSize;
			auto expect = _globalQue[swapIndx]->load();
			while (false == _globalQue[swapIndx]->compare_exchange_strong(expect, _localQue));

			_waitQue.Push(expect);
			if (_waitQue.Pop(_localQue))
			{
				try
				{
					auto count = _localQue->Run();
					if (count > 0)
						_executeTaskCounter->fetch_add(count);
				}
				catch (CoroutineException e)
				{
					InfoLog("{}\n{}", e._exception.what(), e._trace);
				}
				catch (std::exception e)
				{
#if defined(_DEBUG)
					InfoLog("{}\n{}", e.what(), std::stacktrace::current());
#else
					std::terminate();
#endif
				}
			}
		}

		delete _localQue;
	}
	void Context::Post(TSharedPtr<SerializeObject> serializeObject)
	{
		auto idx = _globalCounter->fetch_add(1) % _globalQueSize;
		auto que = _globalQue[idx]->load();
		que->push(std::move(serializeObject));
	}

	void Context::Stop()
	{
		_stop.store(true);
	}

	TSharedPtr<SerializeObject>& Context::Executor()
	{
		return _currentExecutor;
	}
}