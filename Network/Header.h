#pragma once

namespace bugat::net
{
#pragma pack(push, 1)
	struct Header
	{
		int size;
		int type;
	};
#pragma pack(pop)
}