#pragma once
#include <boost/lockfree/queue.hpp>
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

	template<typename Func, typename ComplitionToken = void>
	class TaskModel : public TaskConcept
	{
	public:
		TaskModel(Func&& task, ComplitionToken&& token = {}) : _task(std::forward<Func>(task)), _compl(std::forward<ComplitionToken>(token)) {}
		virtual ~TaskModel() {}
		virtual void Run() override
		{
			_task(_compl);
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
		~TaskSerializer() {}

		template<typename Func>
		requires std::invocable<Func>
		void Post(Func&& func)
		{
			_que.push(new AnyTask(std::forward<Func>(func)));
		}

		int Run()
		{
			auto count = 0;
			while (_que.empty())
			{
				_que.consume_all([&count](AnyTask* task) {
					task->Run();
					delete task;
					count++;
					});
			}

			return count;
		}

	private:
		boost::lockfree::queue<AnyTask*> _que;
	};
}