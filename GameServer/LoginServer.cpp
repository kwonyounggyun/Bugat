#include "stdafx.h"
#include "LoginServer.h"
#include "LoginConnection.h"
#include "LoginSession.h"

namespace bugat
{
	template<typename T>
	auto CreateSerializeObject(std::shared_ptr<LoginConnection>& conn, Context& context)
	{
		auto object = std::make_shared<T>(conn);
		object->SetContext(&context);
		return object;
	}

	template<typename T>
	auto CreateSerializeObject(Context& context)
	{
		auto object = std::make_shared<T>();
		object->SetContext(&context);
		return object;
	}

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
			auto session = CreateSerializeObject<LoginSession>(castConn, GetContext());
			session->SetServer(this);
			castConn->SetSession(session.get());
			castConn->Start();
		}
		else
			conn->Close();
	}
}
