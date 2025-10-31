#include "stdafx.h"
#include "LoginServer.h"
#include "LoginConnection.h"
#include "LoginSession.h"
#include "Template.h"

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
		/*auto castConn = std::dynamic_pointer_cast<LoginConnection>(conn);
		if (castConn)
		{
			auto session = CreateSerializeObject<LoginSession>(GetContext());
			session->SetServer(this);
			castConn->SetSession(session);
			castConn->Start();
		}
		else
			conn->Close();*/
	}
}
