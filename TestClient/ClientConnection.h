#pragma once
#include "../Base/Connection.h"

namespace bugat
{
	class ClientConnection : public Connection
	{
	public:
		ClientConnection();
		~ClientConnection() {}
	};
}

