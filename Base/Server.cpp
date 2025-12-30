#include "pch.h"
#include "Server.h"
#include "Connection.h"
#include "Configure.h"
#include "Boost.h"


namespace bugat
{
	struct AwaitAccept
	{
		AwaitAccept(SerializeObject* server, Acceptor& acceptor, Socket* socket)
			: _server(server), _acceptor(acceptor), _socket(socket)
		{
		}
		bool await_ready() const noexcept { return false; }
		void await_suspend(std::coroutine_handle<> h) noexcept
		{
			_acceptor.async_accept(*_socket, [this, h](const BoostError& ec) {
					_result = ec;
					_server->Post([h]() {
						h.resume(); 
						});
				});
		}
		BoostError await_resume()
		{
			return _result;
		}

		SerializeObject* _server;
		Acceptor& _acceptor;
		Socket* _socket;
		BoostError _result;
	};

	namespace Net
	{
		AwaitTask<void> Accept(std::shared_ptr<Server> server, const Executor& executor, AnyConnectionFactory factory, Configure config)
		{
			Acceptor acceptor(executor, { TCP::v4(), config.port });
			for (;;)
			{
				auto socket = std::make_unique<Socket>(executor);
				auto error = co_await AwaitAccept{ server.get(), acceptor, socket.get()};
				if (error)
				{
					break;
				}

				auto connection = factory.Create(socket);
				server->OnAccept(connection);
				connection->OnAccept();
				connection->Start();
			}

			co_return;
		}
	}

	Server::Server()
	{
	}

	Server::~Server()
	{
	}

	void Server::Accept(const Executor& executor, AnyConnectionFactory factory, Configure config)
	{
		auto sptr = std::static_pointer_cast<Server>(shared_from_this());
		CoSpawn(*this, Net::Accept(sptr, executor, std::move(factory), config));
	}
}
