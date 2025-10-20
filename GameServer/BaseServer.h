#pragma once
#include "../Network/Server.h"
#include "Context.h"

#include <map>

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
		
		virtual void Start(net::AnyConnectionFactory&& factory, net::Configure& config);

		template<typename T>
		auto CreateSerializeObject()
		{
			auto object = std::make_shared<T>();
			object->SetContext(&_context);
			return object;
		}
		
	private:
		Context _context;
	};
}

