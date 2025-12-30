#pragma once
#include <memory>
#include "SerializeObject.h"

namespace bugat
{
	struct Configure;
	class Connection;
	class AnyConnectionFactory;

	struct Executor;
	class Server : public SerializeObject
	{
	public:
		Server();
		~Server();

	public:
		AwaitTask<void> Accept(const Executor& executor, AnyConnectionFactory factory, Configure config);

	protected:
		//楷搬等 Connection 包府 肺流 累己
		virtual void OnAccept(std::shared_ptr<Connection>& conn) = 0;
	};
}