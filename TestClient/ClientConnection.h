#pragma once
#include "../Network/Connection.h"
#include "../Network/Header.h"
#include "Client.h"

namespace bugat::test
{
	class ClientConnection : public bugat::net::Connection
	{
	public:
		void SetClient(std::shared_ptr<Client> client) { _client = std::move(client); }

	protected:
		virtual void ProcessMsg(const bugat::net::Header& header, const std::vector<char>& msg);

	private:
		std::shared_ptr<Client> _client;
	};

	static std::shared_ptr<bugat::net::Connection> CreateClientConnection(boost::asio::io_context& io)
	{
		return bugat::net::AnyConnectionFactory(bugat::net::ConnectionFactory<ClientConnection>()).Create(io);
	}
}

