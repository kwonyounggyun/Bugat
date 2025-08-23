#pragma once
#include "AnyExecutor.h"
#include <boost/lockfree/queue.hpp>

class Context;
class DefaultExecutor
{
public:
	DefaultExecutor(Context& c) : _context(c) {}
	void execute(const std::function<void()>&& f)
	{
		_context.Post(std::move(f));
	}

	Context& _context;
};

struct FuncConcept
{
	virtual void run() = 0;
};

template<class FuncType>
struct FuncModel : FuncConcept
{
	FuncModel(FuncType&& f) : _func(f) {}
	virtual void run() override
	{
		_func();
	}

	FuncType _func;
};

struct AnyFunc
{
	template<typename FuncType>
	AnyFunc(FuncType&& f) : _ptr(std::make_unique<FuncModel<FuncType>>(std::forward(f))) {}

	void run()
	{
		_ptr->run();
	}

private:
	std::unique_ptr<FuncConcept> _ptr;
};

struct Func
{
	Func(const std::function<void()>& f) : _func(std::move(f)) {}
	std::function<void()> _func;
};

class Context
{
public:
	Context() : _executor(DefaultExecutor(*this)) {}
	~Context() {}

	int run()
	{
		int count = 0;
		while (run_one()) { count++; }
		return count;
	}

	bool run_one()
	{
		AnyFunc* out = nullptr;
		_taskQueue.pop(out);
		if (out)
		{
			out->run();
			delete out;
			return true;
		}

		return false;
	}

private:
	void Post(const std::function<void()>&& f)
	{
		_taskQueue.push(new AnyFunc(f));
	}

private:
	AnyExecutor _executor;
	boost::lockfree::queue<AnyFunc*> _taskQueue;
};