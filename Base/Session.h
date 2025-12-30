#pragma once
#include "Connection.h"
#include "Server.h"
#include "SerializeObject.h"
#include "Packet.h"


namespace bugat
{
	class Session : public SerializeObject
	{
	public:
		Session() : _isAuth(false) {};
		virtual ~Session() {}

		void Send(int type, std::shared_ptr<flatbuffers::FlatBufferBuilder>& fb);
		void SetConnection(std::shared_ptr<Connection> conn) { _connection = conn; }

		void Close();
		virtual void OnClose() = 0;

		void SetServer(Server* server) { _server = server; }
		Server* GetServer() const { return _server; }
		virtual void HandleMsg(const std::shared_ptr<RecvPacket>& packet) = 0;

		bool IsAuth() { return _isAuth; }

	private:
		std::shared_ptr<Connection> _connection;
		bool _isAuth;
		Server* _server{ nullptr };
	};
}

