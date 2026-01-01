#pragma once
#define MAX_PACKET_SIZE 4096

namespace bugat
{
	constexpr int MAX_TCP_PACKET_SIZE = 4096;
	constexpr int MAX_UDP_PACKET_SIZE = 1500;

#pragma pack(push, 1)
	struct Header
	{
		int size;
		int type;
	};

	struct TCPHeader
	{
		int size;
		int type;
	};

	struct UDPHeader
	{
		int size;
		int type;
	};
#pragma pack(pop)

	constexpr int HEADER_SIZE = sizeof(Header);
}