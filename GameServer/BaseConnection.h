#pragma once
#include "../Network/Connection.h"
#include "Session.h"

namespace bugat
{
	class Session;
	class BaseConnection : public net::Connection
	{
	public:
		BaseConnection() : _session(nullptr) {}
		virtual ~BaseConnection() = 0 {}

		virtual void OnAccept() override {};
		virtual void OnClose() override {};
		virtual void OnRead(const net::Header& header, const std::vector<char>& msg) override;

		void SetSession(Session* session) { _session = session; }
		Session* GetSession() const { return _session; }

	private:
		Session* _session;
	};
}

