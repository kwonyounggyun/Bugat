#pragma once
#include "../Network/Server.h"
#include "../Core/Context.h"

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

	template<typename Context, typename ConnectionFactory>
	class BaseServer : public net::Server
	{
	public:
		BaseServer() {}
		virtual ~BaseServer() {}
		
		void Start(net::Configure& config)
		{
			net::Server::Start(ConnectionFactory(), config);
			for (int i = 0; i < 5; i++)
			{
				_logicThreads.create_thread([this]()
					{
						while (true)
						{
							_logicContext.run();
						}
					});
			}
		}

		void Post(std::function<void()>&& func)
		{
			_logicContext.post(func);
		}
		
	private:
		Context _logicContext;
		boost::thread_group _logicThreads;
	};
}

