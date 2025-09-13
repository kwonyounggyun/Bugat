#pragma once

namespace bugat::net
{
	struct Configure
	{
		std::string ip;
		unsigned short port;
		int ioThreadCount;
	};
}
