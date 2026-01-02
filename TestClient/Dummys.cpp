#include "stdafx.h"
#include "Dummys.h"
#include "../Core/DateTime.h"

namespace bugat
{
	AwaitTask<void> RandomAction(Dummys* dummys, DummyClient* client)
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
			auto dummyClient = CreateSerializeObject<DummyClient>(connection->GetContext());
			dummyClient->SetConnection(connection);

			dummyClient->OnClose += [this, dummyClient]() {
				_clients.Del(dummyClient->GetObjectId());
				};

			connection->OnConnect += [dummyClient, this]() mutable {
				_clients.Add(dummyClient->GetObjectId(), dummyClient);
				CoSpawn(*dummyClient, RandomAction(this, dummyClient.get()));
				};

			connection->OnRead += [dummyClient](const std::shared_ptr<TCPRecvPacket>& pack) {
				dummyClient->HandleMsg(pack);
				};

			connection->OnClose += [dummyClient]() { dummyClient->Close(); };

			connection->Connect(netContext.GetExecutor(), config.ip, config.port);
		}
	}
}