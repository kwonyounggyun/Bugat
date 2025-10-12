#pragma once
#include <atomic>

namespace bugat::core
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
			timestamp = 0;
			count = GetCount();
		}
	};
}