#pragma once
#include "../Core/Singleton.h"
#include <map>
#include "Client.h"
#include "../Core/RWLockObject.h"

namespace bugat
{
	class ClientManager : public Singleton<ClientManager>
	{
	public:
		void Add(std::shared_ptr<Client>& client)
		{
			auto wlock =_clients.LockWrite();
			wlock->emplace(client->GetObjectId(), client);
		}

		void Delete(std::shared_ptr<Client>& client)
		{
			Delete(client->GetObjectId());
		}

		void Delete(Client::ID_Type objectId)
		{
			auto wlock = _clients.LockWrite();
			wlock->erase(objectId);
		}

	private:
		RWLockObject<std::map<Client::ID_Type, std::shared_ptr<Client>>> _clients;
	};
}