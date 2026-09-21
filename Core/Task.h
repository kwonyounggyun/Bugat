#pragma once
#ifdef TASK_H_DEBUG
	#include "Log.h"
#endif
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


	class AnyTask
	{
	public:
		AnyTask() {};
		AnyTask(AnyTask& other) noexcept
		{
			_task = other._task;
			other._task = nullptr;
		}
		AnyTask(AnyTask&& other) noexcept
		{
			_task = other._task;
			other._task = nullptr;
		}
		AnyTask& operator=(AnyTask& other) noexcept
		{
			_task = other._task;
			other._task = nullptr;
			return *this;
		}
		AnyTask& operator=(AnyTask&& other) noexcept
		{
			_task = other._task;
			other._task = nullptr;
			return *this;
		}

		template<typename Func, typename ...ARGS>
		requires (!std::is_same_v<std::decay_t<Func>, AnyTask>)
		AnyTask(Func&& func, ARGS&&... args)
			: _task(new TaskModel<Func, ARGS...>(std::forward<Func>(func), std::forward<ARGS>(args)...))
		{

		}

		~AnyTask() 
		{
			if(_task != nullptr)
				delete _task;
#ifdef TASK_H_DEBUG
			else
				DebugLog("Not del...");
#endif
		}

		void Run()
		{
			if (_task != nullptr)
				_task->Run();
#ifdef TASK_H_DEBUG
			else
				DebugLog("Must not do...");
#endif
		}

	private:
		TaskConcept* _task;
	};
}