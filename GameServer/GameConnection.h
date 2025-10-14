#pragma once
#include "../Network/Connection.h"
#include "Session.h"

namespace bugat
{
	class GameConnection : public bugat::net::Connection
	{
	public:
		GameConnection() : _session(nullptr) {}
		virtual ~GameConnection() {}

	protected:
		virtual void OnAccept() override;
		virtual void OnClose() override;
		virtual void OnRead(const net::Header& header, const std::vector<char>& msg) override;

	private:
		std::shared_ptr<Session> _session;
	};
}

