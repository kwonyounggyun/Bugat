#include "stdafx.h"
#include "Dummys.h"
#include "../Core/DateTime.h"

namespace bugat
{
	AwaitTask<void> RandomAction(Dummys* dummys, Client* client)
	{
		int64_t lastUpdate = 0;
		while (true)
		{
			auto now = DateTime::NowMs();
			if ((now - lastUpdate) < 10)
			{
				co_await AwaitAlways(client);
				continue;
			}

			lastUpdate = now;
			auto& action = dummys->GetRandomAction();
			(*client, action(client));
			co_await AwaitAlways(client);
		}
	}

	void Dummys::Start(int count, ClientConnectionFactory factory, NetworkContext& netContext, Configure config)
	{
		for (int i = 0; i < count; i++)
		{
			auto connection = factory.Create();
			auto client = CreateSerializeObject<Client>(connection->GetContext());
			client->SetConnection(connection);
			connection->OnConnect += [client, this]() mutable {
				AddClient(client);
				};

			connection->OnRead += [client](const std::shared_ptr<TCPRecvPacket>& pack) {
				client->HandleMsg(pack);
				};

			connection->OnClose += [client]() { client->Close(); };

			connection->Connect(netContext.GetExecutor(), config.ip, config.port);
		}
	}

	void Dummys::AddClient(std::shared_ptr<Client>& client)
	{
		auto lock = _clients.LockWrite();
		lock->emplace(client->GetObjectId(), client);

		CoSpawn(*client, RandomAction(this, client.get()));
	}

	void Dummys::DeleteClient(Client::ID_Type id)
	{
		auto lock = _clients.LockWrite();
		lock->erase(id);
	}

	void Dummys::DeleteClient(std::shared_ptr<Client>& client)
	{
		DeleteClient(client->GetObjectId());
	}
}