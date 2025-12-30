#pragma once
#include "Client.h"
#include "ClientConnection.h"
#include "../Base/NetworkContext.h"
#include "../Base/Configure.h"
#include "../Core/RWLockObject.h"
#include "../Core/Math.h"

namespace bugat
{
	class ClientConnectionFactory : public ConnectionFactoryConcept
	{
	public:
		ClientConnectionFactory(Context& context) : _context(context) {}
		virtual ~ClientConnectionFactory() {}
		virtual std::shared_ptr<Connection> Create() override
		{
			auto connection = std::static_pointer_cast<Connection>(std::make_shared<ClientConnection>());
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
		void AddClient(std::shared_ptr<Client>& client);
		void DeleteClient(Client::ID_Type id);
		void DeleteClient(std::shared_ptr<Client>& client);

		void AddAction(std::function<AwaitTask<void>(Client*)> action)
		{
			_actions.push_back(action);
		}

		const std::function<AwaitTask<void>(Client*)>& GetRandomAction() const
		{
			auto rand = Math::Random<int>(0, _actions.size() - 1);
			return _actions[rand];
		}

	private:
		RWLockObject<std::map<Client::ID_Type, std::shared_ptr<Client>>> _clients;
		std::vector<std::function<AwaitTask<void>(Client*)>> _actions;
	};
}