#include "../Core.h"
#include "BaseConnection.h"
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include "../NetworkMessage.h"

awaitable<void> BaseConnection::Read()
{
    NetworkMessage<Header> _msg; NetworkMessage<Header> _msg;
    try
    {
        for (;;)
        {
            auto block = _msg.GetNewDataBlock();
            std::size_t n = co_await boost::asio::async_read(_socket, boost::asio::buffer(block->GetBuf(), block->GetSize()), use_awaitable);
            std::vector<char> recievedMsg;
            if (_msg.GetNetMessage(recievedMsg))
                ProcessMsg(recievedMsg);
        }
    }
    catch (std::exception&)
    {
    }
}

void BaseConnection::Send(char* buf, int size)
{
    boost::asio::async_write(_socket, boost::asio::buffer(buf, size), boost::asio::deferred);
}
