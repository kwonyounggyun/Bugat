#pragma once
#include <functional>
#include <type_traits>

namespace bugat
{
	class TaskConcept
	{
	public:
		TaskConcept() {}
		virtual ~TaskConcept() = 0;
		virtual void Run() = 0;
	};

	inline TaskConcept::~TaskConcept() {}

	template<typename Func, typename ...ARGS>
	class TaskModel : public TaskConcept
	{
		using ReturnType = std::invoke_result_t<std::decay_t<Func>, ARGS...>;
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
		AnyTask(const AnyTask&) = default;
		AnyTask(AnyTask&&) = default;
		AnyTask& operator=(const AnyTask&) = default;
		AnyTask& operator=(AnyTask&&) = default;

		template<typename Func, typename ...ARGS>
		requires (!std::is_same_v<std::decay_t<Func>, AnyTask>)
		AnyTask(Func&& func, ARGS&&... args)
			: _task(std::make_shared<TaskModel<Func, ARGS...>>(std::forward<Func>(func), std::forward<ARGS>(args)...))
		{

		}

		void Run()
		{
			_task->Run();
		}

	private:
		std::shared_ptr<TaskConcept> _task;
	};
}