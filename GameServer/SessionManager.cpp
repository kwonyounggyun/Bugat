#include "stdafx.h"
#include "SessionManager.h"

namespace bugat
{
	void SessionManager::AddSession(PlayerId pid, std::shared_ptr<Session> session)
	{
		_sessions[pid] = session;
	}

	std::shared_ptr<Session> SessionManager::FindSession(PlayerId pid)
	{
		auto iter = _sessions.find(pid);
		if (iter == _sessions.end())
			return nullptr;

		return iter->second;
	}
	void SessionManager::CloseSession(PlayerId pid)
	{
		auto session = FindSession(pid);
		if (session)
		{

		}
	}
}
