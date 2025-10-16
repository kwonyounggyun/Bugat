#pragma once
#include "TaskSerializer.h"
#include "ObjectId.h"

namespace bugat::core
{
	template <typename T>
	class SerializeObject : public TaskSerializer, public std::enable_shared_from_this<T>
	{
	public:
		SerializeObject() {}
		virtual ~SerializeObject() {}

		virtual void OnRun(int remainCount) override
		{
			if (remainCount > 0)
			{

			}
		}

		virtual void OnPost(int remainCount) override
		{
			if (remainCount == 1)
			{

			}
		}

	private:
		ObjectId<T> objectId;
	};
}