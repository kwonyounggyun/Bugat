#pragma once
#include "../Core/Singleton.h"
#include "../Network/Header.h"
#include "../Network/Protocol.h"

#include "Client.h"
#include <unordered_map>
#include <functional>

namespace bugat::handle
{
	class ClientHandler : public Singleton<ClientHandler>
	{
	public:
		ClientHandler();
		virtual ~ClientHandler() {}
		void Handle(const bugat::test::Client* client, const bugat::net::Header& header, const std::vector<char>& msg);

		static void ResSC_Login(const bugat::test::Client* client, const bugat::net::Header& header, const std::vector<char>& msg);

	private:
		std::unordered_map<int, void(*)(const bugat::test::Client* client, const bugat::net::Header&, const std::vector<char>&)> _handleMap;
	};
}
