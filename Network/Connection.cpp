#include "pch.h"
#include "Connection.h"
#include "NetworkMessage.h"

#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/connect.hpp>

namespace bugat::net
{
	bool Connection::Connect(std::string ip, short port)
	{
		tcp::resolver resolver(_socket->get_executor());
		tcp::endpoint ep(boost::asio::ip::make_address(ip), port);
		tcp::resolver::results_type endpoints = resolver.resolve(ep);

		try
		{
			auto result = boost::asio::connect(*_socket, endpoints);
		}
		catch (std::exception& e)
		{
			std::cout << "Connect failed. IP: " << ip << " Port: " << port << " Reason: " << e.what() << std::endl;
			return false;
		}

		Read();
		return true;
	}

	void Connection::Send(char* buf, int size)
    {
        boost::asio::async_write(_socket, boost::asio::buffer(buf, size), boost::asio::deferred);
    }

    void Connection::Read()
    {
		boost::asio::co_spawn(_socket->get_executor(), [&]()->boost::asio::awaitable<void> {
			NetworkMessage _msg;
			Header header;
			try
			{
				for (;;)
				{
					auto block = _msg.GetNewDataBlock();
					std::size_t n = co_await boost::asio::async_read(_socket, boost::asio::buffer(block->GetBuf(), block->GetSize()), boost::asio::use_awaitable);
					std::vector<char> recievedMsg;
					if (_msg.GetNetMessage(header, recievedMsg))
						ProcessMsg(header, recievedMsg);
				}
			}
			catch (std::exception&)
			{
			}
			});
    }
}