#pragma once
#include <chrono>

/*
* Only operate on system clock time, not steady clock time.
* This header file is used milliseconds, and convert between time_t, tm, and milliseconds.
* String methods are used for logging, and the format is "YYYY-MM-DD HH:MM:SS". but NowStringTime is "YYYY-MM-DD HH".
*/
#define ONE_MILLISECOND (long long)1
#define ONE_SECOND ONE_MILLISECOND * 1000
#define ONE_MINUTE ONE_SECOND * 60
#define ONE_HOUR ONE_MINUTE * 60
#define ONE_DAY ONE_HOUR * 24
#define ONE_WEAK ONE_DAY * 7

class DateTime
{
public:
	static auto Now()
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	}

	static std::tm NowTm()
	{
		auto now = std::chrono::system_clock::now();
		std::time_t now_c = std::chrono::system_clock::to_time_t(now);
		std::tm tm;
		localtime_s(&tm, &now_c);
		return tm;
	}

	static std::string NowString()
	{
		auto nowTm = NowTm();
		return std::format("{}-{:02d}-{:02d} {:02d}:{:02d}:{:02d}", nowTm.tm_year + 1900, nowTm.tm_mon + 1, nowTm.tm_mday, nowTm.tm_hour, nowTm.tm_min, nowTm.tm_sec);
	}

	static std::string NowStringTime()
	{
		auto nowTm = NowTm();
		return std::format("{}-{:02d}-{:02d} {:02d}", nowTm.tm_year + 1900, nowTm.tm_mon + 1, nowTm.tm_mday, nowTm.tm_hour);
	}

	static auto CountToString(long long count)
	{
		auto tm = CountToTm(count);
		return std::format("{}-{:02d}-{:02d} {:02d}:{:02d}:{:02d}", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	}

	static auto TmToCount(std::tm& tm)
	{
		std::time_t time = std::mktime(&tm);
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::from_time_t(time).time_since_epoch()).count();
	}

	static std::tm CountToTm(long long count)
	{
		std::chrono::milliseconds ms(count);
		std::chrono::time_point<std::chrono::system_clock> tp(ms);
		std::time_t time = std::chrono::system_clock::to_time_t(tp);
		std::tm tm;
		localtime_s(&tm, &time);
		return tm;
	}

	static auto MonthStartFromCount(long long count)
	{
		auto tm = CountToTm(count);
		tm.tm_mday = 1;
		tm.tm_hour = 0;
		tm.tm_min = 0;
		tm.tm_sec = 0;
		return TmToCount(tm);
	}

	static auto MonthEndFromCount(long long count)
	{
		auto tm = CountToTm(count);
		tm.tm_mday = 1;
		tm.tm_hour = 0;
		tm.tm_min = 0;
		tm.tm_sec = 0;
		tm.tm_mon += 1;
		return TmToCount(tm) - 1;
	}

	static auto WeekStartFromCount(long long count)
	{
		auto tm = CountToTm(count);
		tm.tm_mday -= tm.tm_wday;
		tm.tm_hour = 0;
		tm.tm_min = 0;
		tm.tm_sec = 0;
		return TmToCount(tm);
	}

	static auto WeekEndFromCount(long long count)
	{
		auto tm = CountToTm(count);
		tm.tm_mday -= tm.tm_wday;
		tm.tm_hour = 0;
		tm.tm_min = 0;
		tm.tm_sec = 0;
		tm.tm_mday += 7;
		return TmToCount(tm) - 1;
	}
	
	static auto DayStartFromCount(long long count)
	{
		auto tm = CountToTm(count);
		tm.tm_hour = 0;
		tm.tm_min = 0;
		tm.tm_sec = 0;
		return TmToCount(tm);
	}

	static auto DayEndFromCount(long long count)
	{
		auto tm = CountToTm(count);
		tm.tm_hour = 0;
		tm.tm_min = 0;
		tm.tm_sec = 0;
		tm.tm_mday += 1;
		return TmToCount(tm) - 1;
	}

	static auto MoveYear(long long count, int year)
	{
		auto tm = CountToTm(count);
		tm.tm_year += year;
		return TmToCount(tm) + count % 1000;
	}

	static auto MoveMonth(long long count, int month)
	{
		auto tm = CountToTm(count);
		tm.tm_mon += month;
		return TmToCount(tm) + count % 1000;
	}
	
	static auto MoveWeak(long long count, int weak)
	{
		return count + (ONE_WEAK * weak);
	}

	static auto MoveDay(long long count, int day)
	{
		return count + (ONE_DAY * day);
	}

	static auto MoveHour(long long count, int hour)
	{
		return count + (ONE_HOUR * hour);
	}

	static auto MoveMinute(long long count, int minute)
	{
		return count + (ONE_MINUTE * minute);
	}

	static auto MoveSecond(long long count, int second)
	{
		return count + (ONE_SECOND * second);
	}

	static auto MoveMillisecond(long long count, int millisecond)
	{
		return count + (ONE_MILLISECOND * millisecond);
	}
};