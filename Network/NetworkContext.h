#pragma once
#include <atomic>
#include <functional>

namespace boost::asio
{
	class io_context;
}

namespace bugat::net
{
	struct Configure;
	class Server;
	class AnyConnectionFactory;
	class NetworkContext
	{
	public:
		NetworkContext();

		void Initialize(Server* server, AnyConnectionFactory factory, Configure config);
		void Run();
		void Post(std::function<void()>&& func);
		void Stop();

	private:
		std::shared_ptr<boost::asio::io_context> _ioContext;
		std::atomic_bool _stop;
	};
}