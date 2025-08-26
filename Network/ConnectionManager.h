#pragma once
#include <map>
#include <memory>


class ConnectionManager
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

