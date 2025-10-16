#pragma once
#include "BaseConnection.h"

namespace bugat
{
	class LoginSession;
	class LoginConnection : public BaseConnection
	{
	public:
		LoginConnection() : _loginSession(nullptr) {}
		virtual ~LoginConnection() {}
	protected:
		virtual void OnAccept() override;
		virtual void OnClose() override;
		virtual void OnRead(const net::Header& header, const std::vector<char>& msg) override;

	private:
		std::shared_ptr<LoginSession> _loginSession;
	};
}