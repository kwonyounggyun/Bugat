#include "stdafx.h"
#include "Session.h"

namespace bugat
{
	void Session::Send(int type, std::shared_ptr<flatbuffers::FlatBufferBuilder>& fb)
	{
		_connection->Send(net::Packet(type, fb));
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

	void Session::HandleMsg(const net::Header& header, const std::vector<char>& msg)
	{
	}
}
