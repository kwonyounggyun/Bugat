#pragma once
#include "../AnyContext.h"

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/use_awaitable.hpp>

using boost::asio::awaitable;
using boost::asio::use_awaitable;
using boost::asio::ip::tcp;


struct Header
{
	int size;
	int type;
};

class BaseConnection
{
public:
	explicit BaseConnection(tcp::socket& socket) : _socket(std::move(socket)) {}
	~BaseConnection() {}

	awaitable<void> Read();
	void Send(char* buf, int size);

	virtual void ProcessMsg(const std::vector<char>& msg) {}

private:
	tcp::socket _socket;
};