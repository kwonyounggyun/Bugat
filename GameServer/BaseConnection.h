#pragma once
#include "../Base/Connection.h"

namespace bugat
{
	class Session;
	class BaseConnection : public Connection
	{
	public:
		BaseConnection() {}
		virtual ~BaseConnection() {}
	};
}

