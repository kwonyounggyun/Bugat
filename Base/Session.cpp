#include "pch.h"
#include "Session.h"
#include "Protocol.h"

namespace bugat
{
	void Session::Send(int type, std::shared_ptr<flatbuffers::FlatBufferBuilder>& fb)
	{
		Post([this, type, fb]() {
			if (_connection)
				_connection->Send(SendPacket(type, fb));
			});
	}

	void Session::Close()
	{
		Post([this]() {
			if (_connection)
			{
				_connection->Close();
				_connection = nullptr;
			}

			OnClose();
			});
	}
}
