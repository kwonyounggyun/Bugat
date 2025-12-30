#include "pch.h"
#include	"Context.h"
#include <thread>
#include "SerializeObject.h"

namespace bugat
{
	void Context::Initialize()
	{
	}

	void Context::Run()
	{
		while (false == _stop.load())
		{
			RunOne();
		}
	}

	bool Context::RunOne()
	{
		std::weak_ptr<SerializeObject> weak;
		if (false == _que.Pop(weak))
			return false;

		if (auto sptr = weak.lock(); sptr)
		{
			auto taskCount = sptr->Run();
			_executeTaskCounter.Update(taskCount);
		}

		return false;
	}

	void Context::Post(std::weak_ptr<SerializeObject> serializeObject)
	{
		_que.Push(std::move(serializeObject));
	}

	void Context::Stop()
	{
		_stop.store(true);
	}
}