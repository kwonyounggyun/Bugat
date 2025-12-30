#pragma once
#include <atomic>
#include "DateTime.h"

namespace bugat
{
	template<typename T>
	struct ObjectId
	{
		uint64_t timestamp;
		uint64_t count;

		static uint64_t GetCount()
		{
			static std::atomic<uint64_t> globalCount{ 0 };
			return globalCount.fetch_add(1);
		}

		ObjectId()
		{
			timestamp = DateTime::NowSec();
			count = GetCount();
		}

		bool operator==(const ObjectId& other) const
		{ 
			return timestamp == other.timestamp && count == other.count;
		}
		
		bool operator<(const ObjectId& other) const
		{ 
			return timestamp != other.timestamp ? timestamp < other.timestamp : count < other.count;
		}

		bool operator!=(const ObjectId& other) const
		{ 
			return timestamp != other.timestamp || count != other.count;
		}

		bool operator>(const ObjectId& other) const
		{ 
			return timestamp != other.timestamp ? timestamp > other.timestamp : count > other.count;
		}
	};
}