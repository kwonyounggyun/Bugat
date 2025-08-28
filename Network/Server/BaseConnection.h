#pragma once
#include "../AnyContext.h"

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/use_awaitable.hpp>
#include "NonCopyable.h"

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
	BaseConnection() = delete;
	explicit BaseConnection(tcp::socket& socket) : _socket(std::move(socket)) {}
	~BaseConnection() {}

	awaitable<void> Read();
	void Send(char* buf, int size);

protected:
	virtual void ProcessMsg(const std::vector<char>& msg) = 0;

private:
	tcp::socket _socket;
};

/*
* BaseConnection을 상속받은 객체 생성 용
*/
class ConnectionFactoryConcept
{
public:
	virtual ~ConnectionFactoryConcept() {}
	virtual std::shared_ptr<BaseConnection> Create(tcp::socket& socket) = 0;
};

template<typename T>
class ConnectionFactory : public ConnectionFactoryConcept
{
public:
	virtual ~ConnectionFactory<T>() {}
	virtual std::shared_ptr<BaseConnection> Create(tcp::socket& socket) override
	{
		return std::dynamic_pointer_cast<BaseConnection>(std::make_shared<T>(socket));
	}
};

class AnyConnectionFactory
{
public:
	template<typename T>
	AnyConnectionFactory(ConnectionFactory<T> factory) : _ptr(std::make_unique<ConnectionFactory<T>>(std::move(factory))) {};

	std::shared_ptr<BaseConnection> Create(tcp::socket& socket)
	{
		return _ptr->Create(socket);
	}

private:
	std::unique_ptr<ConnectionFactoryConcept> _ptr;
};