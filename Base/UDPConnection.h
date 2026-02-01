#pragma once
#include "SerializeObject.h"

namespace bugat
{
	struct Executor;
	struct UDPInfo;
	class UDPConnection : public SerializeObject
	{
		friend struct AwaitRecvFrom;
		DECL_COROUTINE_FUNC(UDPConnection, RecvFrom, void, ())

	public:
		void Recv(Executor& executor, int recieverCount, unsigned short port);

	private:
		std::shared_ptr<UDPInfo> _info;
	};
}