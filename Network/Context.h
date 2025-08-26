#pragma once
#include "AnyExecutor.h"
#include <boost/lockfree/queue.hpp>

struct Func
{
	Func(const std::function<void()>& f) : _func(std::move(f)) {}
	std::function<void()> _func;
};

class Context;
class DefaultExecutor
{
public:
	DefaultExecutor(Context& c) : _context(c) {}
	void execute(const std::function<void()>&& f);

	Context& _context;
};

class Context
{
public:
	Context() : _executor(DefaultExecutor(*this)), _taskQueue(1024) {}
	~Context() {}

	int run()
	{
		int count = 0;
		while (run_one()) { count++; }
		return count;
	}

	bool run_one()
	{
		Func* out = nullptr;
		_taskQueue.pop(out);
		if (out) 
		{
			out->_func();
			delete out;
			return true;
		}

		return false;
	}

	void Post(const std::function<void()>&& f)
	{
		_taskQueue.push(new Func(f));
	}

	AnyExecutor& get_executor() { return _executor; }

private:
	AnyExecutor _executor;
	boost::lockfree::queue<Func*> _taskQueue;
};


void DefaultExecutor::execute(const std::function<void()>&& f)
{
	_context.Post(std::move(f));
}

template<typename ExecutionContext>
void Post(ExecutionContext& c, std::function<void()>&& f)
{
	c.get_executor().execute(std::move(f));
}

template<typename ExecutionContext, typename Awaitable>
void CoSpawn(ExecutionContext c, Awaitable&& awaitable)
{
	c.get_executor().execute([_awaitable = std::move(awaitable)]() {
		_awaitable.resume();
		});
}