#pragma once
#include "../Network/Connection.h"
#include "Session.h"

namespace bugat
{
	class Session;
	class BaseConnection : public net::Connection
	{
	public:
		BaseConnection() {}
		virtual ~BaseConnection() = 0 {}

		virtual void OnAccept() override {};
		virtual void OnClose() override {};
		virtual void OnRead(const net::Header& header, const std::vector<char>& msg) override;
	};
}

