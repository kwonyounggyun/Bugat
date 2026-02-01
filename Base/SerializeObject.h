#pragma once
#include "../Core/TaskSerializer.h"
#include "../Core/ObjectPool.h"
#include "ObjectId.h"
#include "Memory.h"
#include "Context.h"
#include "AwaitTask.h"
#include "Macro.h"

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

		void SetContext(Context* context) { _context = context; }
		auto GetContext() { return _context; }

		ID_Type GetObjectId() const { return _objectId; }

	protected:
		virtual void OnRun(int64_t remainCount);
		virtual void OnPost(int64_t remainCount);

	private:
		ID_Type _objectId;
		Context* _context;
	};

	/// <summary>
	/// 특정 Context에 종속된 객체를 생성해주는 전역함수.
	/// </summary>
	/// <typeparam name="T">객체 타입으로 RefCountable을 상속받은 타입만 가능</typeparam>
	/// <typeparam name="...ARGS">생성자 파라미터 타입</typeparam>
	/// <param name="context">해당 객체가 실행될 Context</param>
	/// <param name="...args">생성자 파라미터</param>
	/// <returns>생성된 객체</returns>
	template<typename T, typename ...ARGS>
		requires std::is_base_of_v<SerializeObject, T>
	TSharedPtr<T> CreateSerializeObject(Context* context, ARGS&&... args)
	{
		auto sptr = GetObjectPool<T, 1000>().Get(std::forward<ARGS>(args)...);
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