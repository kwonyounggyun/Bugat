#pragma once
#include <boost/lockfree/queue.hpp>
#include <type_traits>

namespace bugat::core
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
		template<typename Func, typename ...ARGS>
		AnyTask(Func&& func, ARGS&&... args)
			: _task(std::make_unique<TaskModel<Func, ARGS...>>(std::forward<Func>(func), std::forward<ARGS>(args)...))
		{

		}

		/*template<typename Func>
		AnyTask(Func&& func)
			: _task(std::make_unique<TaskModel<Func, void>>(std::forward<Func>(func)))
		{

		}*/

		void Run()
		{
			_task->Run();
		}

	private:
		std::unique_ptr<TaskConcept> _task;
	};

	class TaskSerializer
	{
	public:
		TaskSerializer() : _que(1024) {}
		virtual ~TaskSerializer()
		{
			AnyTask* task = nullptr;
			while (_que.pop(task))
			{
				delete task;
			}
		}

		template<typename Func, typename ...ARGS>
		requires std::invocable<Func, ARGS&...>
		void Post(Func&& func, ARGS&&... args)
		{
			_que.push(new AnyTask(std::forward<Func>(func), std::forward<ARGS>(args)...));
			OnPost(_taskCount.fetch_add(1, std::memory_order_release) + 1);
		}

		/*
		* 반드시 하나의 스레드에서만 호출되어야 한다.
		*/
		int64_t Run()
		{
			while (true == _runningGuard.test_and_set(std::memory_order_acquire));

			auto executeCount = _que.consume_all([](AnyTask* task) 
				{
					task->Run(); 
					delete task;
				});

			auto preCount = _taskCount.fetch_sub(executeCount, std::memory_order_release);
			_runningGuard.clear(std::memory_order_release);

			OnRun(preCount - executeCount);
			return executeCount;
		}

		virtual void OnRun(int64_t remainCount) {}
		virtual void OnPost(int64_t remainCount) {}

		int64_t GetCount() const { return _taskCount; }

	private:
		boost::lockfree::queue<AnyTask*> _que;
		std::atomic<int64_t> _taskCount{ 0 };
		std::atomic_flag _runningGuard;
	};
}