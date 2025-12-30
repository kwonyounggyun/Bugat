#include "pch.h"
#include "SerializeObject.h"
#include "Context.h"

namespace bugat
{
	void SerializeObject::OnRun(int64_t remainCount)
	{
		if (remainCount > 0)
		{
			if (_context)
				_context->Post(shared_from_this());
		}
	}

	void SerializeObject::OnPost(int64_t remainCount)
	{
		if (remainCount == 1)
		{
			if (_context)
				_context->Post(shared_from_this());
		}
	}
}