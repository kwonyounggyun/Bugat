#include "stdafx.h"
#include "Session.h"

namespace bugat
{
	void Session::Send(int size, char* buf)
	{
		_connection->Send(buf, size);
	}

	void Session::Close()
	{
		_messageBlock = true;

		if (_connection)
		{
			_connection->Close();
			_connection = nullptr;
		}
	}
}
