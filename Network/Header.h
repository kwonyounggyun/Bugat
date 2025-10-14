#pragma once

namespace bugat::net
{
#pragma pack(push, 1)
	struct Header
	{
		size_t size;
		int type;
	};
#pragma pack(pop)
}