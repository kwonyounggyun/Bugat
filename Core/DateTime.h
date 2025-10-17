#pragma once
#include <cstdint>
#include <chrono>

namespace bugat
{
	class DateTime
	{
	public:
		static int64_t NowSec();
		static int64_t NowMs();
		static std::tm NowTm();
		// 0:일요일 ~ 6:토요일
		static int32_t GetDayOfWeek(int64_t sec);
		// 1~31
		static int32_t GetDay(int64_t sec);
		// 0:1월 ~ 11:12월
		static int32_t GetMonth(int64_t sec);
		static std::tm GetTm(int64_t sec);
	};
}