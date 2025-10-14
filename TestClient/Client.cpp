#include "stdafx.h"
#include "Client.h"
#include "ClientConnection.h"
#include "ClientHandler.h"

namespace bugat::test
{
	Client::~Client()
	{
	}

	bool Client::Connect(boost::asio::io_context& io, std::string ip, short port)
	{
		_connection = CreateClientConnection(io);
		std::static_pointer_cast<ClientConnection>(_connection)->SetClient(shared_from_this());
		return _connection->Connect(ip, port);
	}

	void Client::Send(int type, std::shared_ptr<flatbuffers::FlatBufferBuilder>& fb)
	{
		_connection->Send(net::Packet(type, fb));
	}

	void Client::PushMsg(const bugat::net::Header& header, const std::vector<char>& msg)
	{
		Post([this, header, msg = std::move(msg)]() {
			bugat::handle::ClientHandler::Instance().Handle(this, header, msg);
			});
	}
}
