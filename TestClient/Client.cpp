#include "stdafx.h"
#include "Client.h"
#include "ClientConnection.h"
#include "ClientHandler.h"

namespace bugat
{
	Client::~Client()
	{
	}

	void Client::OnClose()
	{
	}

	void Client::HandleMsg(const std::shared_ptr<RecvPacket>& packet)
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
