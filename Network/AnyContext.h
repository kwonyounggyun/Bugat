#pragma once
#include <functional>
#include <memory>
#include "AnyExecutor.h"

class ContextConcept
{
public:
	virtual ~ContextConcept() = default;

	virtual void run() = 0;
	virtual void post(std::function<void()>&& f) = 0;
	virtual AnyExecutor get_executor() = 0;
};

template<typename Context>
struct ContextModel : ContextConcept
{
public:
	ContextModel(Context& c) : _context(c) {}
	virtual void run() override
	{
		_context.run();
	}

	virtual void post(std::function<void()>&& f) override
	{
		_context.post(std::move(f));
	}

	virtual AnyExecutor get_executor() override
	{
		return _context.get_executor();
	}

private:
	Context& _context;
};

class AnyContext
{
public:
	template<typename Context>
	AnyContext(Context& c) : _ptr(std::make_unique<ContextModel<Context>>(c)) {}

	void run()
	{
		_ptr->run();
	}

	void post(std::function<void()>&& f)
	{
		_ptr->post(std::move(f));
	}

	virtual AnyExecutor get_executor()
	{
		return _ptr->get_executor();
	}

private:
	std::unique_ptr<ContextConcept> _ptr;
};

#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>

boost::asio::awaitable<void> Testeeee()
{
	co_await boost::asio::this_coro::executor;
	co_return;
}

void co_spawn(AnyContext context)
{
	boost::asio::co_spawn(context.get_executor(), Testeeee(), boost::asio::deferred);
}