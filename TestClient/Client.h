#pragma once
#include "..\Core\SerializeObject.h"
#include "../Network/Header.h"

namespace boost
{
	namespace asio
	{
		class io_context;
	}
}

namespace bugat::net
{
	struct Connection;
}

namespace bugat::test
{
	class Client : public bugat::core::SerializeObject<Client>
	{
	public:
		Client() {};
		virtual ~Client();

		bool Connect(boost::asio::io_context& io, std::string ip, short port);
		void Send(char* buf, int size);

		void PushMsg(const bugat::net::Header& header, const std::vector<char>& msg);

	private:
		std::shared_ptr<bugat::net::Connection> _connection;
	};
}