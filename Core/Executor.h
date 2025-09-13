#pragma once
#include <functional>
#include <memory>

namespace bugat
{
	class ExecutorConcept
	{
	public:
		virtual ~ExecutorConcept() = default;

		virtual void execute(std::function<void()>&& func) = 0;
	};

	template<typename Exec>
	struct ExcutorModel : ExecutorConcept
	{
	public:
		ExcutorModel(Exec e) : _exec(e) {}

		void execute(std::function<void()>&& f) override
		{
			_exec.execute(std::forward<std::function<void()>>(f));
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
		std::unique_ptr<ExecutorConcept> _ptr;
	};
}