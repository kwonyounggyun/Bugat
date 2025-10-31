#pragma once
#include "../Network/Connection.h"
#include "../Network/Packet.h"
#include "../Network/Server.h"
#include "SerializeObject.h"


namespace bugat
{
	namespace net
	{
		class Server;
	}

	class Session : public SerializeObject
	{
	public:
		Session() : _isAuth(false) {};
		virtual ~Session() {}

		void Send(int type, std::shared_ptr<flatbuffers::FlatBufferBuilder>& fb);
		void SetConnection(std::shared_ptr<net::Connection>& conn) { _connection = conn; }

		void Close();
		virtual void OnClose() = 0;

		void SetServer(net::Server* server) { _server = server; }
		net::Server* GetServer() const { return _server; }
		virtual void HandleMsg(const net::Header& header, const std::vector<char>& msg);

		bool IsAuth() { return _isAuth; }

	private:
		std::shared_ptr<net::Connection> _connection;
		bool _isAuth;
		net::Server* _server{ nullptr };
	};
}

