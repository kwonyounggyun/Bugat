#include "stdafx.h"
#include "GameConnection.h"

#include "GameSession.h"
#include "GameAuth.h"

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
		if (_gameSession)
			_gameSession->HandleMsg(header, msg);
		else
		{
			auto sPtr = shared_from_this();
			GameAuth::Handle(sPtr, header, msg);
		}
	}

	void GameConnection::Auth(const net::Header& header, const std::vector<char>& msg)
	{

	}
}