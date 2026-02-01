#include "pch.h"
#include "SerializeObject.h"
#include "Context.h"
#include "../Core/Memory.h"

namespace bugat
{
	void SerializeObject::OnRun(int64_t remainCount)
	{
		if (TASKSERIALIZER_ERROR == remainCount)
		{
			CriticalLog("SerializeObject was executed on two thread!! serializeobject should only be executed on one thread.");
			return;
		}

		if (remainCount > 0)
		{
			if (_context)
				_context->Post(TSharedPtr(this));
		}
	}

	void SerializeObject::OnPost(int64_t remainCount)
	{
		if (TASKSERIALIZER_ERROR == remainCount)
		{
			CriticalLog("SerializeObject was executed on two thread!! serializeobject should only be executed on one thread.");
			return;
		}

		if (remainCount == 1)
		{
			if (_context)
				_context->Post(TSharedPtr(this));
		}
	}
}