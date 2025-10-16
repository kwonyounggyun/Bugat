#pragma once

namespace bugat
{
	namespace net
	{
		class Header;
	}

	class BaseConnection;
	class Session;
	class LoginHandler
	{
	public:
		static void HandleLoginRequest(const std::shared_ptr<BaseConnection>& conn, int type, const std::vector<char*>& msg);

	private:
		static void Auth(const std::shared_ptr<BaseConnection>& conn, int type, const std::vector<char*>& msg);
		static void HandleMsg(const std::shared_ptr<Session>& session, int type, const std::vector<char*>& msg);
	};
}

