#include "pch.h"
#include "UDPConnection.h"
#include "NetworkMessage.h"
#include "Packet.h"

namespace bugat
{
	struct UDPInfo
	{
		UDPInfo(Executor& executor, unsigned short port) : _socket(executor, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port)), _strand(boost::asio::make_strand(executor)) {}

		Strand _strand;
		UDPSocket _socket;
	};

	struct AwaitRecvFrom
	{
		AwaitRecvFrom(UDPConnection* con, char* buf, int size) : _con(con), _buf(buf), _size(size){}
		bool await_ready() { return false; }
		void await_suspend(std::coroutine_handle<> h)
		{
			_con->_info->_socket.async_receive_from(
				boost::asio::buffer(_buf, _size), _endpoint,
				boost::asio::bind_executor(_con->_info->_strand,
					[this, h](boost::system::error_code ec, std::size_t recv) {
						_ec = ec;
						_recv = recv;
						_con->Post([h] {h.resume(); });
					}));
		}

		std::tuple<UDPEndPoint, boost::system::error_code, std::size_t>  await_resume() {
			return { _endpoint, _ec, _recv };
		}

		UDPConnection* _con;
		UDPEndPoint _endpoint;
		char* _buf;
		int _size;

		boost::system::error_code _ec;
		std::size_t _recv;
	};

	namespace Net
	{
		AwaitTask<void> RecvFrom(std::shared_ptr<UDPConnection> connection)
		{
			NetworkMessage<UDPRecvPacket> msg;
			while (true)
			{
				auto bufInfo = msg.GetBufInfo();
				co_await AwaitRecvFrom(connection.get(), bufInfo.buf, bufInfo.size);
			}
		}
	}

	void UDPConnection::Recv(Executor& executor, int recieverCount, unsigned short port)
	{
		_info = std::make_shared<UDPInfo>(executor, port);
		CoSpawn(*this, Net::RecvFrom(std::static_pointer_cast<UDPConnection>(shared_from_this())));
	}
}
