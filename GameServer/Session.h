#pragma once
#include "../Network/Connection.h"
#include "../Network/Packet.h"
#include "SerializeObject.h"
#include "BaseServer.h"

namespace bugat
{
	class Session : public SerializeObject<Session>
	{
	public:
		Session() {};
		virtual ~Session() {}

		void Send(int type, std::shared_ptr<flatbuffers::FlatBufferBuilder>& fb);
		void SetConnection(std::shared_ptr<net::Connection>& conn) { _connection = conn; }

		void Close();
		virtual void OnClose() = 0;

		void SetServer(BaseServer* server) { _server = server; }
		BaseServer* GetServer() const { return _server; }
		virtual void HandleMsg(const net::Header& header, const std::vector<char>& msg);

	private:
		std::shared_ptr<net::Connection> _connection;
		BaseServer* _server{ nullptr };
	};
}

