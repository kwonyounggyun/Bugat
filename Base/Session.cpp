#include "pch.h"
#include "Session.h"
#include "Protocol.h"

namespace bugat
{
	void Session::Send(int type, std::shared_ptr<flatbuffers::FlatBufferBuilder>& fb)
	{
		_connection->Send(SendPacket(type, fb));
	}

	void Session::Close()
	{
		if (_connection)
		{
			_connection->Close();
			_connection = nullptr;
		}

		OnClose();
	}
}
