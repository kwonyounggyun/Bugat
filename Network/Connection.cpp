#include "pch.h"
#include "Connection.h"
#include "NetworkMessage.h"

#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace bugat::net
{
    void Connection::Send(char* buf, int size)
    {
        boost::asio::async_write(_socket, boost::asio::buffer(buf, size), boost::asio::deferred);
    }

    void Connection::Read(boost::asio::io_context& _io)
    {
		boost::asio::co_spawn(_io, [&]()->boost::asio::awaitable<void> {
			NetworkMessage _msg;
			try
			{
				for (;;)
				{
					auto block = _msg.GetNewDataBlock();
					std::size_t n = co_await boost::asio::async_read(_socket, boost::asio::buffer(block->GetBuf(), block->GetSize()), boost::asio::use_awaitable);
					std::vector<char> recievedMsg;
					if (_msg.GetNetMessage(recievedMsg))
						ProcessMsg(recievedMsg);
				}
			}
			catch (std::exception&)
			{
			}
			});
    }
}