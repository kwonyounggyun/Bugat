#pragma once
#include "../Core/RWLockObject.h"
#include "../Core/Singleton.h"
#include "Actor.h"

namespace bugat
{
	static std::shared_ptr<Actor> SpawnActor(int key)
	{
	}

	class ActorManager : NonCopyable
	{
		static ActorManager& Instance()
		{
			static Singleton<ActorManager> manager;
			return manager.Instance();
		}

		RWLockObject<std::map<int64_t, Actor>> _mapActor;
	};
}