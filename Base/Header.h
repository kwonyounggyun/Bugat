#pragma once
#define MAX_PACKET_SIZE 4096

namespace bugat
{
#pragma pack(push, 1)
	struct Header
	{
		int size;
		int type;
	};
#pragma pack(pop)

	constexpr int HEADER_SIZE = sizeof(Header);
}