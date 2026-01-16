#pragma once
#include "DummyClient.h"
#include "ClientConnection.h"
#include "../Base/NetworkContext.h"
#include "../Base/Configure.h"
#include "../Core/Math.h"
#include "../Base/Counter.h"

#include "DummyClientManager.h"

namespace bugat
{
	class ClientConnectionFactory : public ConnectionFactoryConcept
	{
	public:
		ClientConnectionFactory(Context& context) : _context(context) {}
		virtual ~ClientConnectionFactory() {}
		virtual TSharedPtr<Connection> Create() override
		{
			auto connection = CreateSerializeObject<ClientConnection>(&_context);
			connection->SetContext(&_context);
			return connection;
		}

	private:
		Context& _context;
	};

	class Dummys
	{
	public:
		Dummys() {}
		void Start(int count, ClientConnectionFactory factory, NetworkContext& netContext, Configure config);

		void AddAction(std::function<void(DummyClient*)> action)
		{
			_actions.push_back(action);
		}

		const std::function<void(DummyClient*)>& GetRandomAction() const
		{
			auto rand = Math::Random<int>(0, _actions.size() - 1);
			return _actions[rand];
		}

	private:
		DummyClientManager _clients;
		std::vector<std::function<void(DummyClient*)>> _actions;
	};
}