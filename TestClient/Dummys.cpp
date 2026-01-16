#include "stdafx.h"
#include "Dummys.h"
#include "../Base/DateTime.h"

namespace bugat
{
	AwaitTask<void> RandomAction(Dummys* dummys, DummyClient* client)
	{
		int64_t lastUpdate = 0;
		int64_t count = 0;
		while (count < 1000)
		{
		/*	auto now = DateTime::NowMs();
			if ((now - lastUpdate) < 10)
			{
				co_await AwaitAlways(client);
				continue;
			}

			lastUpdate = now;*/
			auto& action = dummys->GetRandomAction();
			(*client, action(client));
			co_await AwaitAlways(client);
			count++;
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
				CoSpawn(*dummyClient, RandomAction(this, dummyClient.Get()));
				};

			connection->OnRead += [dummyClient](const TSharedPtr<TCPRecvPacket>& pack) {
				dummyClient->HandleMsg(pack);
				};

			connection->OnClose += [dummyClient]() { dummyClient->Close(); };

			connection->Connect(netContext, config.ip, config.port);
		}
	}
}