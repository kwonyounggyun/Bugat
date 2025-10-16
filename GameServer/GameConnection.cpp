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
		if(_gameSession)
		{
			_gameSession->Close();
			_gameSession = nullptr;
		}
	}

	void GameConnection::OnRead(const net::Header& header, const std::vector<char>& msg)
	{
	}
}