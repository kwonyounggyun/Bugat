#pragma once
#include "../Core/TaskSerializer.h"
#include "../Core/ObjectId.h"
#include "Context.h"
#include <memory>

namespace bugat
{
	class Context;

	template <typename T>
	class SerializeObject : public core::TaskSerializer, public std::enable_shared_from_this<T>
	{
	public:
		SerializeObject() {}
		virtual ~SerializeObject() {}

		virtual void OnRun(int64_t remainCount) 
		{
			if (remainCount <= 0)
				return;

			if(_context)
				_context->post(std::static_pointer_cast<TaskSerializer>(this->shared_from_this()));
		}
		virtual void OnPost(int64_t remainCount) 
		{
			if (remainCount == 1)
				return;

			if (_context)
				_context->post(std::static_pointer_cast<TaskSerializer>(this->shared_from_this()));
		}

		void SetContext(Context* context)
		{
			_context = context;
		}

		auto GetObjectId() const
		{
			return objectId;
		}

		auto& GetObjectType() const
		{
			return typeid(core::ObjectId<T>);
		}

	private:
		core::ObjectId<T> objectId;
		Context* _context;
	};
}