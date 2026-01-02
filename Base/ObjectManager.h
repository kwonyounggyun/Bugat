#pragma once
#include "map"
#include "../Core/RWLockObject.h"

namespace bugat
{
	template<typename Key, typename ObjectType>
	class ObjectAgent
	{
	public:
		bool Add(Key& key, std::shared_ptr<ObjectType>& object)
		{
			auto write = _objects.LockWrite();
			auto [iter, result] = write->emplace(key, object);
			return result;
		}

		void Del(Key& key)
		{
			auto write = _objects.LockWrite();
			write->erase(key);
		}

		std::shared_ptr<ObjectType> Find(Key& key)
		{
			auto read = _objects.LockRead();
			auto iter = read->find(key);
			if (iter == read->end())
				return nullptr;

			return iter->second;
		}

	private:
		RWLockObject<std::map<Key, std::shared_ptr<ObjectType>>> _objects;
	};

	template<typename Hash, typename Key, typename ObjectType, int AgentCount>
	class ObjectManager
	{
	public:
		bool Add(Key key, std::shared_ptr<ObjectType>& object)
		{
			return GetAgent(key).Add(key, object);
		}

		void Del(Key key)
		{
			return GetAgent(key).Del(key);
		}

		std::shared_ptr<ObjectType> Find(Key key)
		{
			return GetAgent(key).Find(key);
		}

	protected:
		ObjectAgent<Key, ObjectType>& GetAgent(Key& key)
		{
			auto idx = _hash(key, AgentCount);
			return _agents[idx];
		}

	private:
		std::array<ObjectAgent<Key, ObjectType>, AgentCount> _agents;
		Hash _hash;
	};
}