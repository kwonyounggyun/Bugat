#pragma once
#include "BaseConnection.h"

namespace bugat
{
	class GameSession;
	class GameConnection : public BaseConnection
	{
	public:
		GameConnection() {}
		virtual ~GameConnection() {}

	protected:
		virtual void OnAccept() override;
		virtual void OnClose() override;
		virtual void OnRead(const net::Header& header, const std::vector<char>& msg) override;

		void Auth(const net::Header& header, const std::vector<char>& msg);
	};
}

