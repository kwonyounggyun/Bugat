#include "stdafx.h"
#include "Session.h"

namespace bugat
{
	void Session::Send(int size, char* buf)
	{
		_connection->Send(buf, size);
	}
}
