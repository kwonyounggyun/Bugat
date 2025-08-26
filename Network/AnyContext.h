#pragma once
#include <functional>
#include <memory>

class ContextConcept
{
public:
	virtual ~ContextConcept() = default;

	virtual void run() = 0;
};

template<typename Context>
struct ContextModel : ContextConcept
{
public:
	ContextModel(Context c) : _context(c) {}
	void run() override
	{
		_context.run();
	}

private:
	Context _context;
};

class AnyContext
{
public:
	template<typename Context>
	AnyContext(Context c) : _ptr(std::make_unique<ContextModel<Context>>(c)) {}

	void run()
	{
		_ptr->run();
	}

private:
	std::unique_ptr<ContextConcept> _ptr;
};