#pragma once
#include <boost/lockfree/queue.hpp>
#include <type_traits>

namespace bugat
{
	class TaskSerializer
	{
	private:
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
			_taskCount.fetch_add(1, std::memory_order_release);
		}

		template<typename Func>
		requires std::invocable<Func>
		void Post(Func&& func)
		{
			_que.push(new AnyTask(std::forward<Func>(func)));
			_taskCount.fetch_add(1, std::memory_order_release);
		}

		/*
		* 반드시 하나의 스레드에서만 호출되어야 한다.
		*/
		int Run()
		{
			while (true == _runningGuard.test_and_set(std::memory_order_acquire));

			auto curCount = _taskCount.load(std::memory_order_acquire);
			auto count = curCount;
			while (count > 0)
			{
				_que.consume_one([](AnyTask* task) 
				{ 
					task->Run(); 
					delete task; 
				});
				count--;
			}

			_taskCount.fetch_sub(curCount, std::memory_order_release);
			_runningGuard.clear(std::memory_order_release);

			return count;
		}

	private:
		boost::lockfree::queue<AnyTask*> _que;
		std::atomic<int> _taskCount{ 0 };
		std::atomic_flag _runningGuard;
	};
}