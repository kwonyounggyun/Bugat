#pragma once
#include "../Core/Singleton.h"
#include <map>
#include "DummyClient.h"
#include "../Core/RWLockObject.h"

namespace bugat
{
	class ClientManager : public Singleton<ClientManager>
	{
	public:
		void Add(std::shared_ptr<DummyClient>& client)
		{
			auto wlock =_clients.LockWrite();
			wlock->emplace(client->GetObjectId(), client);
		}

		void Delete(std::shared_ptr<DummyClient>& client)
		{
			Delete(client->GetObjectId());
		}

		void Delete(DummyClient::ID_Type objectId)
		{
			auto wlock = _clients.LockWrite();
			wlock->erase(objectId);
		}

	private:
		RWLockObject<std::map<DummyClient::ID_Type, std::shared_ptr<DummyClient>>> _clients;
	};
}