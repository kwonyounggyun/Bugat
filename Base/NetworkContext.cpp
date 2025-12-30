#include "pch.h"
#include "NetworkContext.h"
#include "Connection.h"
#include "Boost.h"

namespace bugat
{
	struct ContextImpl
	{
		int64_t Run()
		{
			return _ioContext.run();
		}

		Executor GetExcutor()
		{
			return Executor{ _ioContext };
		}

		BoostContext _ioContext;
	};

	NetworkContext::NetworkContext() : _context(std::make_shared<ContextImpl>()), _stop(false)
	{

	}

	void NetworkContext::Initialize()
	{
	}

	void NetworkContext::Run()
	{
		while (false == _stop.load(std::memory_order_acquire))
			_context->Run();
	}

	void NetworkContext::Stop()
	{
		_stop.store(true, std::memory_order_release);
	}

	Executor NetworkContext::GetExecutor() const noexcept
	{
		return _context->GetExcutor();
	}
}