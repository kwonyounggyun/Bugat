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

	template<typename Func, typename ComplitionToken = void>
	class TaskModel : public TaskConcept
	{
	public:
		TaskModel(Func&& task, ComplitionToken&& token = {}) : _task(std::forward<Func>(task)), _compl(std::forward<ComplitionToken>(token)) {}
		virtual ~TaskModel() {}
		virtual void Run() override
		{
			_compl(_task());
		}

	private:
		Func _task;
		ComplitionToken _compl;
	};

	template<typename Func>
	class TaskModel<Func, void> : public TaskConcept
	{
	public:
		TaskModel(Func&& task) : _task(std::forward<Func>(task)) {}
		virtual ~TaskModel() {}
		virtual void Run() override
		{
			_task();
		}

	private:
		Func _task;
	};

	class AnyTask
	{
	public:
		template<typename Func, typename CompletionToken>
		AnyTask(Func&& func, CompletionToken&& token)
			: _task(std::make_unique<TaskModel<Func, CompletionToken>>(std::forward<Func>(func), std::forward<CompletionToken>(token)))
		{

		}

		template<typename Func>
		AnyTask(Func&& func)
			: _task(std::make_unique<TaskModel<Func, void>>(std::forward<Func>(func)))
		{

		}

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

		template<typename Func, typename CompletionToken = void>
		requires std::invocable<CompletionToken, std::invoke_result_t<Func>>
		void Post(Func&& func, CompletionToken&& token)
		{
			_que.push(new AnyTask(std::forward<Func>(func), std::forward<CompletionToken>(token)));
			OnPost(_taskCount.fetch_add(1, std::memory_order_release) + 1);
		}

		template<typename Func>
		requires std::invocable<Func>
		void Post(Func&& func)
		{
			_que.push(new AnyTask(std::forward<Func>(func)));
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