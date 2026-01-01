#pragma once
#include <memory>
#include "SerializeObject.h"

namespace bugat
{
	struct Configure;
	class Connection;
	class AnyConnectionFactory;

	struct Executor;
	struct AcceptInfo;
	class Server : public SerializeObject
	{
	public:
		Server();
		virtual ~Server();

	public:
		void Accept(const Executor& executor, AnyConnectionFactory factory, Configure config);

		//楷搬等 Connection 包府 肺流 累己
		virtual void OnAccept(std::shared_ptr<Connection>& conn) = 0;

	protected:
		virtual void Update() = 0;

	private:
		std::vector<std::shared_ptr<AcceptInfo>> _acceptInfos;
	};
}