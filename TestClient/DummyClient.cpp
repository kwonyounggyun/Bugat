#include "stdafx.h"
#include "DummyClient.h"
#include "ClientConnection.h"
#include "ClientHandler.h"

namespace bugat
{
	DummyClient::DummyClient()
	{
	}

	DummyClient::~DummyClient()
	{
	}

	void DummyClient::HandleMsg(const TSharedPtr<TCPRecvPacket>& packet)
	{
		auto sptr = TSharedPtr(this);
		Post([sptr, packet]() mutable {
			if (sptr)
			{
				ClientHandler::Instance().Handle(sptr, packet);
			}
			});
	}
}
