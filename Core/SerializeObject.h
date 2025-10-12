#pragma once
#include "TaskSerializer.h"
#include "ObjectId.h"

namespace bugat::core
{
	template <typename T>
	class SerializeObject : public TaskSerializer, public std::enable_shared_from_this<T>
	{
	private:
		ObjectId<T> objectId;
	};
}