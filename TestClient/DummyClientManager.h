#pragma once
#include "DummyClient.h"
#include "../Base/ObjectManager.h"

namespace bugat
{
	struct DummyIDHash
	{
		int operator()(DummyClient::ID_Type& key, int size)
		{
			return key.count % size;
		}
	};

	using DummyClientManager = ObjectManager<DummyIDHash, DummyClient::ID_Type, DummyClient, 2000>;
}