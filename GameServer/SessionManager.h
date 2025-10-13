#pragma once
#include "Session.h"
#include "PlayerId.h"
#include <shared_mutex>

namespace bugat
{
	class SessionManager
	{
	public:
		SessionManager() {}
		~SessionManager() {}

		void AddSession(PlayerId pid, std::shared_ptr<Session> session);
		std::shared_ptr<Session> FindSession(PlayerId pid);
		void CloseSession(PlayerId pid);

	private:
		std::map<PlayerId, std::shared_ptr<Session>> _sessions;
		std::shared_mutex _mutex;
	};
}

