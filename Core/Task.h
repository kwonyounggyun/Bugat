#pragma once
#include <functional>
#include <type_traits>

namespace bugat
{
	class TaskConcept
	{
	public:
		TaskConcept() {}
		virtual ~TaskConcept() = 0 {}
		virtual void Run() = 0;
	};

	template<typename Func, typename ...ARGS>
	class TaskModel : public TaskConcept
	{
		using ReturnType = std::invoke_result_t<Func, ARGS...>;
	public:
		TaskModel(Func&& task, ARGS&&... args) : _task(std::bind(std::forward<Func>(task), std::forward<ARGS>(args)...)) {}
		virtual ~TaskModel() {}
		virtual void Run() override
		{
			_task();
		}

	private:
		std::function<ReturnType()> _task;
	};

	/*template<typename Func>
	class TaskModel<Func, void> : public TaskConcept
	{
		using ReturnType = std::invoke_result_t<Func>;
	public:
		TaskModel(Func&& task) : _task(std::forward<Func>(task)) {}
		virtual ~TaskModel() {}
		virtual void Run() override
		{
			_task();
		}

	private:
		std::function<ReturnType()> _task;
	};*/


	class AnyTask
	{
	public:
		AnyTask() {};

		template<typename Func, typename ...ARGS>
		AnyTask(Func&& func, ARGS&&... args)
			: _task(std::make_unique<TaskModel<Func, ARGS...>>(std::forward<Func>(func), std::forward<ARGS>(args)...))
		{

		}

		void Run()
		{
			_task->Run();
		}

	private:
		std::unique_ptr<TaskConcept> _task;
	};
}