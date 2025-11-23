#pragma once
#include "BaseConnection.h"

namespace bugat
{
	class LoginSession;
	class LoginConnection : public BaseConnection
	{
	public:
		LoginConnection() {}
		virtual ~LoginConnection() {}
	};
}