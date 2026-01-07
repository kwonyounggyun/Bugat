#pragma once
#include "SerializeObject.h"
#include "BoostAsio.h"

namespace bugat
{
	struct UDPInfo;
	class UDPConnection : public SerializeObject
	{
		friend struct AwaitRecvFrom;
	public:
		void Recv(Executor& executor, int recieverCount, unsigned short port);

	private:
		std::shared_ptr<UDPInfo> _info;
	};
}