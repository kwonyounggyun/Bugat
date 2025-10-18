#pragma once
#include "../Network/Server.h"
#include "Context.h"

#include <map>
#include <boost/thread.hpp>

namespace bugat
{
	namespace net
	{
		class Connection;
		class AnyConnectionFactory;
		class Configure;
	}

	class BaseServer : public net::Server
	{
	public:
		BaseServer() {}
		virtual ~BaseServer() {}
		
		void Start(net::AnyConnectionFactory factory, net::Configure& config)
		{
			net::Server::Start(factory, config);
			for (int i = 0; i < 5; i++)
			{
				_logicThreads.create_thread([this]()
					{
						_logicContext.run();
					});
			}
		}

		template <typename ObjectType>
		void post(std::shared_ptr<SerializeObject<ObjectType>>& serializeObject)
		{
			_logicContext.post(serializeObject);
		}
		
	private:
		LogicContext _logicContext;
		boost::thread_group _logicThreads;
	};
}

