#include "pch.h"
#include "Session.h"
#include "Protocol.h"
#include "Header.h"

namespace bugat
{
	void Session::Send(int type, std::shared_ptr<flatbuffers::FlatBufferBuilder>& fb)
	{
		Post([this, type, fb]() {
			if (_connection)
			{
				TCPHeader header;
				header.size = fb->GetSize();
				header.type = type;
				_connection->Send(TCPSendPacket(header, fb));
			}
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
