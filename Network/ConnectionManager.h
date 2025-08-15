#pragma once
#include <map>
#include <memory>
#include "BaseCoroutine.h"

class Connection
{
public:
	Connection(const std::function<void()>&& deletor = []() {}) : _deletor(deletor)
	{
	}

	Connection() = default;
	~Connection()
	{
		_deletor();
	}

	void Read()
	{

	}

	void Send()
	{

	}

	void Accept()
	{

	}

private:
	std::function<void()> _deletor;
};

class ConnectionManager : public BaseCoroutine<int>
{
public:
	ConnectionManager() = default;
	~ConnectionManager() = default;

	void Start(int port);
	void Stop();

private:
	void Listen();

private:
	std::map<int64_t, std::shared_ptr<Connection>> _mapClient;
};

