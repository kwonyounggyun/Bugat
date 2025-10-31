#pragma once
#include "../Core/TaskSerializer.h"
#include "../Core/ObjectId.h"
#include "Context.h"
#include <memory>

namespace bugat
{
	class Context;
	class SerializeObject : public core::TaskSerializer, public std::enable_shared_from_this<SerializeObject>
	{
	public:
		SerializeObject() : _context(nullptr) {}
		virtual ~SerializeObject() {}

		virtual void OnRun(int64_t remainCount) 
		{
			if (remainCount > 0)
			{
				if (_context)
					_context->post(shared_from_this());
			}
		}
		virtual void OnPost(int64_t remainCount)
		{
			if (remainCount == 1)
			{
				if (_context)
					_context->post(shared_from_this());
			}
		}

		void SetContext(Context* context)
		{
			_context = context;
		}

		auto GetObjectId() const
		{
			return _objectId;
		}

	private:
		core::ObjectId<SerializeObject> _objectId;
		Context* _context;
	};
}