#pragma once
#include "Session.h"

namespace bugat
{
	class LoginConnection;
	class LoginSession : public bugat::Session
	{
	public:
		LoginSession() {}
		virtual ~LoginSession() {}

		virtual void OnClose() override;
	};
}