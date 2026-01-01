#pragma once
#include <type_traits>
#include <map>

#include "../Core/RWLockObject.h"
#include "../Base/PlayerId.h"
#include "../Base/Session.h"

namespace bugat
{
	template<typename T>
	requires std::is_base_of_v<Session, T>
	class SessionManager
	{
	public:
		void AddSession(PlayerId& pid, const std::shared_ptr<T>& session)
		{
			auto write = _mapSession.LockWrite();
			write->emplace(pid, session);
		}

		void DelSession(PlayerId& pid)
		{
			auto write = _mapSession.LockWrite();
			write->erase(pid);
		}

		std::shared_ptr<T> FindSession(PlayerId& pid)
		{
			auto read = _mapSession.LockRead();
			auto iter = read->find(pid);
			if (iter == read->end())
				return nullptr;

			return iter->second;
		}

	private:
		RWLockObject<std::map<PlayerId, std::shared_ptr<T>>> _mapSession;
	};
}