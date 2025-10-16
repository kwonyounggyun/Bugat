#include "stdafx.h"
#include "LoginConnection.h"

namespace bugat
{
	void LoginConnection::OnAccept()
	{
	}

	void LoginConnection::OnClose()
	{
	}

	void LoginConnection::OnRead(const net::Header& header, const std::vector<char>& msg)
	{
		auto session = GetSession();
		session->Post([session, header, msg = std::move(msg)]() {

			});
	}
}