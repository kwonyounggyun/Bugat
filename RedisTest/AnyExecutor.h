#pragma once
#include <functional>
#include <memory>

class ExecuteConcept 
{
public:
	virtual ~ExecuteConcept() = default;

	virtual void execute(std::function<void()>&& func) = 0;
};

template<typename Exec>
struct ExcutorModel : ExecuteConcept
{
public:
	ExcutorModel(Exec e) : _exec(e) {}
	void executor(std::function<void()>&& f) override
	{
		_exec.execute(std::move(f));
	}

private:
	Exec _exec;
};

class AnyExecutor
{
public:
	template<typename Exec>
	AnyExecutor(Exec e) : _ptr(std::make_unique<ExcutorModel<Exec>>(e)) {}

	void execute(std::function<void()>&& func)
	{
		_ptr->execute(std::move(func));
	}

private:
	std::unique_ptr<ExecuteConcept> _ptr;
};