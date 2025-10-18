#pragma once
#include "TaskSerializer.h"
#include "ObjectId.h"

namespace bugat::core
{
	template <typename T>
	class SerializeObject : public TaskSerializer
	{
	public:
		SerializeObject() {}
		virtual ~SerializeObject() {}

		virtual void OnRun(int remainCount) override
		{
			if (remainCount > 0)
			{
				//이거면 로컬 큐에
			}
		}

		virtual void OnPost(int remainCount) override
		{
			if (remainCount == 1)
			{
				//이거면 글로벌 큐에
			}
		}

	private:
		ObjectId<T> objectId;
	};
}