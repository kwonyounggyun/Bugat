#include "stdafx.h"
#include "GameConnection.h"

#include "GameSession.h"

namespace bugat
{
	void GameConnection::OnAccept()
	{
	}

	void GameConnection::OnClose()
	{
		if(_session)
		{
			_session->Close();
			_session = nullptr;
		}
	}

	void GameConnection::OnRead(const net::Header& header, const std::vector<char>& msg)
	{
		if (_session)
			_session->HandleMsg(header, msg);
	}

	void GameConnection::Auth(const net::Header& header, const std::vector<char>& msg)
	{

	}
}