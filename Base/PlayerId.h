#pragma once
#include <cstdint>

namespace bugat
{
	struct PlayerId
	{
		uint64_t pid;

		bool operator==(const PlayerId& other) const { return pid == other.pid; }
		bool operator<(const PlayerId& other) const { return pid < other.pid; }
		bool operator!=(const PlayerId& other) const { return pid != other.pid; }
		bool operator>(const PlayerId& other) const { return pid > other.pid; }
	};
}
