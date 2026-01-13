#include "pch.h"
#include "Server.h"
#include "Connection.h"
#include "Configure.h"
#include "BoostAsio.h"

namespace bugat
{
	struct AwaitAccept
	{
		AwaitAccept(SerializeObject* server, Acceptor& acceptor, TCPSocket* socket)
			: _server(server), _acceptor(acceptor), _socket(socket)
		{
		}
		bool await_ready() const noexcept { return false; }
		void await_suspend(std::coroutine_handle<> h) noexcept
		{
			_socket->AsyncAccept(_acceptor, [this, h](const BoostError& ec) {
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
		TCPSocket* _socket;
		BoostError _result;
	};

	struct AcceptInfo
	{
	public:
		AcceptInfo(const Executor& executor, unsigned short port, AnyConnectionFactory& factory) : _executor(executor), _acceptor(executor, { TCP::v4(), port }), _factory(std::move(factory))
		{
			_acceptor.listen(boost::asio::socket_base::max_listen_connections);
		}
		Acceptor& GetAcceptor() { return _acceptor; }
		AnyConnectionFactory& GetFactory() { return _factory; }
		const Executor& GetExecutor() { return _executor; }

	private:
		Acceptor _acceptor;
		AnyConnectionFactory _factory;
		Executor _executor;
	};

	namespace Net
	{
		AwaitTask<void> Accept(std::shared_ptr<Server> server, std::shared_ptr<AcceptInfo> info)
		{
			for (;;)
			{
				auto socket = std::make_unique<TCPSocket>(info->GetExecutor());
				auto error = co_await AwaitAccept{ server.get(), info->GetAcceptor(), socket.get()};
				if (error)
				{
					break;
				}

				auto connection = info->GetFactory().Create(socket);
				server->OnAccept(connection);
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
		auto info = std::make_shared<AcceptInfo>(executor, config.port, factory);
		_acceptInfos.push_back(info);
		auto sptr = std::static_pointer_cast<Server>(shared_from_this());
		for (auto i = 0; i < config.acceptTaskCount; i++)
			CoSpawn(*this, Net::Accept(sptr, info));
	}
}
