#pragma once
#include "Session.h"

namespace bugat
{
	class LoginConnection;
	class LoginSession : public bugat::Session
	{
	public:
		LoginSession(std::shared_ptr<LoginConnection>& loginConnection) : _loginConnection(loginConnection) {};
		virtual ~LoginSession() {}

		virtual void OnClose() override;

	private:
		std::shared_ptr<LoginConnection> _loginConnection;
	}
}