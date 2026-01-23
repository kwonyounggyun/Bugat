#pragma once
#include "../Core/TaskSerializer.h"
#include "../Core/ObjectPool.h"
#include "ObjectId.h"
#include "Memory.h"

namespace bugat
{
	template<typename T>
	class AwaitPost;

	class Context;
	class SerializeObject : public TaskSerializer
	{
	public:
		using ID_Type = ObjectId<SerializeObject>;
		SerializeObject() : _context(nullptr) {}
		virtual ~SerializeObject() {}

		template<typename Await>
		AwaitPost<typename Await::ValueType> AwitePost(TSharedPtr<SerializeObject> caller, Await&& awaitable)
		{
			return AwaitPost<typename Await::ValueType>(caller, this, std::move(awaitable));
		}

		virtual void OnRun(int64_t remainCount);
		virtual void OnPost(int64_t remainCount);
		void SetContext(Context* context) { _context = context; }
		auto GetContext() { return _context; }

		ID_Type GetObjectId() const { return _objectId; }

	private:
		ID_Type _objectId;
		Context* _context;
	};

	template<typename T>
	class AwaitPost
	{
	public:
		AwaitPost(TSharedPtr<SerializeObject> caller, TSharedPtr<SerializeObject> callee, AwaitTask<T>&& task) : _caller(caller), _callee(callee), _task(std::move(task)) {}

		auto await_ready() {}
		auto await_suspend(std::coroutine_handle<> h)
		{
			_task.SetCallback([h, this](T value) {
				_result = value;
				_caller->Post([h]() {
					h.resume();
					});
				});
			_callee->Post(std::move(_task));
		}

		auto await_resume()
		{
			return _result;
		}

	private:
		T _result;
		TSharedPtr<SerializeObject> _caller;
		TSharedPtr<SerializeObject> _callee;
		AwaitTask<T> _task;
	};

	template<>
	class AwaitPost<void>
	{
	public:
		AwaitPost(TSharedPtr<SerializeObject> caller, TSharedPtr<SerializeObject> callee, AwaitTask<void>&& task) : _caller(caller), _callee(callee), _task(std::move(task)) {}

		auto await_ready() {}
		auto await_suspend(std::coroutine_handle<> h)
		{
			_task.SetCallback([h, this]() {
				_caller->Post([h]() {
					h.resume();
					});
				});
			_callee->Post(std::move(_task));
		}

		void await_resume() { }

	private:
		TSharedPtr<SerializeObject> _caller;
		TSharedPtr<SerializeObject> _callee;
		AwaitTask<void> _task;
	};

	template<typename T, typename ...ARGS>
		requires std::is_base_of_v<SerializeObject, T>
	TSharedPtr<T> CreateSerializeObject(Context* context, ARGS&&... args)
	{
		static ObjectPool<T, 1000> objPool;
		auto sptr = objPool.Get(std::forward(args)...);
		sptr->SetContext(context);
		return sptr;
	}

	template<typename Executor, typename AwaitTask>
		requires std::is_base_of_v<SerializeObject, Executor>
	void CoSpawn(Executor& executor, AwaitTask&& awaitask)
	{
		executor.Post([awaitask]() {
			awaitask.resume();
			});
	}
}