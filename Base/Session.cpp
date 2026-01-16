#include "pch.h"
#include "Session.h"
#include "Protocol.h"
#include "Header.h"

namespace bugat
{
	void Session::Send(int type, std::shared_ptr<flatbuffers::FlatBufferBuilder>& fb)
	{
		if (auto con = _connection; con)
		{
			if (con->Disconnected())
				return;

			TCPHeader header;
			header.size = fb->GetSize();
			header.type = type;
			con->Send(TCPSendPacket(header, fb));
		}
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
