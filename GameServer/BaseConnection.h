#pragma once
#include "../Network/Connection.h"
#include "Session.h"

namespace bugat
{
	class Session;
	class BaseConnection : public net::Connection
	{
	public:
		BaseConnection() {}
		virtual ~BaseConnection() = 0 {}
	};
}

