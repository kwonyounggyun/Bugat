#include "stdafx.h"
#include "LoginServer.h"
#include "LoginConnection.h"
#include "LoginSession.h"

namespace bugat
{
	LoginServer::LoginServer()
	{
	}

	LoginServer::~LoginServer()
	{
	}

	void LoginServer::OnAccept(std::shared_ptr<bugat::net::Connection>& conn)
	{
		auto castConn = std::dynamic_pointer_cast<LoginConnection>(conn);
		if (castConn)
		{
			auto session = CreateSerializeObject<LoginSession>();
			session->SetServer(this);
			castConn->SetSession(session.get());
			castConn->Start();
		}
		else
			conn->Close();
	}
}
