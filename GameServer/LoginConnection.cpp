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
		session->Post([](const net::Header& header, const std::vector<char>& msg) {

			}, header, msg);
	}
}