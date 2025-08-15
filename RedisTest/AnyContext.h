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
struct ContextModel : ExecuteConcept
{
public:
	ContextModel(Context e) : _exec(e) {}
	void run() override
	{
		_exec.run();
	}

private:
	Context _exec;
};

class AnyContext
{
public:
	template<typename Context>
	AnyContext(Context c) : _ptr(std::make_unique<ExcutorModel<Context>>(c)) {}

	void run()
	{
		_ptr->run();
	}

private:
	std::unique_ptr<ContextConcept> _ptr;
};