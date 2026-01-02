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

	void DummyClient::HandleMsg(const std::shared_ptr<TCPRecvPacket>& packet)
	{
		Post([weak = weak_from_this(), packet]() mutable {
			if (auto sPtr = weak.lock(); sPtr != nullptr)
			{
				auto session = std::static_pointer_cast<Session>(sPtr);
				ClientHandler::Instance().Handle(session, packet);
			}
			});
	}
}
