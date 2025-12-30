#pragma once
#include "../Core/TaskSerializer.h"
#include "../Core/ObjectId.h"
#include "../Core/ObjectPool.h"
#include <memory>
#include "../Core/Log.h"

namespace bugat
{
	class Context;
	class SerializeObject : public TaskSerializer, public std::enable_shared_from_this<SerializeObject>
	{
	public:
		using ID_Type = ObjectId<SerializeObject>;
		SerializeObject() : _context(nullptr) {}
		virtual ~SerializeObject() {}

		virtual void OnRun(int64_t remainCount);
		virtual void OnPost(int64_t remainCount);
		void SetContext(Context* context) { _context = context; }
		auto GetContext() { return _context; }

		ID_Type GetObjectId() const { return _objectId; }

	private:
		ID_Type _objectId;
		Context* _context;
	};

	template<typename T, typename Context, typename ...ARGS>
		requires std::is_base_of_v<SerializeObject, T>
	std::shared_ptr<T> CreateSerializeObject(Context context, ARGS&&... args)
	{
		static ObjectPool<T, 1000> objPool;
		auto sptr = objPool.Get(std::forward(args)...);
		sptr->SetContext(context);
		return sptr;
	}

	template<typename Executor, typename AwaitTask>
		requires std::is_convertible_v<Executor, std::shared_ptr<SerializeObject>>
	void CoSpawn(Executor& executor, AwaitTask&& awaitask)
	{
		executor->Post([awaitask]() {
			awaitask.resume();
			});
	}
}