#include "stdafx.h"
#include "LoginHandler.h"
#include "BaseConnection.h"

namespace bugat
{
	void LoginHandler::HandleLoginRequest(const std::shared_ptr<BaseConnection>& conn, int type, const std::vector<char*>& msg)
	{
		//// 인증이 안됐을때 오는 패킷은 모두 버린다.
		//if (conn->isAuthorized())
		//{
		//	auto session = conn->GetSession();
		//	if (session)
		//	{
		//		session->Post([session, type, msg = std::move(msg)]() {
		//			LoginHandler::HandleMsg(session, type, msg);
		//			});
		//	}
		//}
	}

	void LoginHandler::HandleMsg(const std::shared_ptr<Session>& session, int type, const std::vector<char*>& msg)
	{

	}
}