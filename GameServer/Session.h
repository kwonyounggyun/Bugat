#pragma once
#include "../Network/Connection.h"
#include "../Network/Packet.h"
#include "../Core/SerializeObject.h"

namespace bugat
{
	class Session : public bugat::core::SerializeObject<Session>
	{
	public:
		Session() {};
		virtual ~Session() {}

		void Send(int type, std::shared_ptr<flatbuffers::FlatBufferBuilder>& fb);
		void SetConnection(std::shared_ptr<net::Connection>& conn) { _connection = conn; }

		void Close();

		virtual void HandleMsg(const net::Header& header, const std::vector<char>& msg);
	private:
		std::shared_ptr<net::Connection> _connection;
	};
}

