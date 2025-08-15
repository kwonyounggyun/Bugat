#include "ConnectionManager.h"
#include "ThreadGroup.h"
#include <boost/asio.hpp>

using tcp = boost::asio::ip::tcp;

void ConnectionManager::Start(int port)
{
	_acceptor = std::make_shared<tcp::acceptor>(_ioThreadGroup.GetIO(), tcp::endpoint(tcp::v4(), port));
	Listen();
}

void ConnectionManager::Listen()
{
	auto conn = std::make_shared<Connection>(_acceptor->get_executor());
	co_await _acceptor->async_accept(conn->GetSocket(), [conn, this](boost::system::error_code ec) {
		if (!ec)
		{
			std::cout << "클라이언트 연결됨: " << conn->GetSocket().remote_endpoint() << "\n";
			conn->Read();
		}

		Accept();
		});
}