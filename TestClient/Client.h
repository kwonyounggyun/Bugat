#pragma once
#include "../GameServer/SerializeObject.h"
#include "../Network/Packet.h"

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
	class Client : public bugat::SerializeObject<Client>
	{
	public:
		Client() {};
		virtual ~Client();

		bool Connect(boost::asio::io_context& io, std::string ip, short port);
		void Send(int type, std::shared_ptr<flatbuffers::FlatBufferBuilder>& fb);

		void PushMsg(const bugat::net::Header& header, const std::vector<char>& msg);

	private:
		std::shared_ptr<bugat::net::Connection> _connection;
	};
}