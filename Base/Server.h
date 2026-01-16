#pragma once
#include <memory>
#include "SerializeObject.h"
#include "NetworkContext.h"

namespace bugat
{
	struct Configure;
	class Connection;
	class AnyConnectionFactory;

	struct AcceptInfo;
	class Server : public SerializeObject
	{
	public:
		Server();
		virtual ~Server();

	public:
		void Accept(const NetworkContext& executor, AnyConnectionFactory factory, Configure config);

		//楷搬等 Connection 包府 肺流 累己
		virtual void OnAccept(TSharedPtr<Connection>& conn) = 0;

	protected:
		virtual void Update() = 0;

	private:
		std::vector<std::shared_ptr<AcceptInfo>> _acceptInfos;
	};
}