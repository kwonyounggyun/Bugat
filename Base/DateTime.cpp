#include "pch.h"
#include "DateTime.h"
#include <boost/date_time.hpp>

namespace bugat
{
	static boost::posix_time::ptime epoch(boost::gregorian::date(2025, 1, 1));

	int64_t DateTime::NowSec()
	{
		boost::posix_time::ptime now = boost::posix_time::microsec_clock::universal_time();
		boost::posix_time::time_duration diff = now - epoch;
		return diff.total_seconds();
	}

	int64_t DateTime::NowMs()
	{
		boost::posix_time::ptime now = boost::posix_time::microsec_clock::universal_time();
		boost::posix_time::time_duration diff = now - epoch;
		return diff.total_milliseconds();
	}

	std::tm DateTime::NowTm()
	{
		return GetTm(NowSec());
	}

	int32_t DateTime::GetDayOfWeek(int64_t sec)
	{
		boost::posix_time::ptime time = epoch + boost::posix_time::seconds(sec);
		auto tm = boost::posix_time::to_tm(time);
		return tm.tm_wday;
	}

	int32_t DateTime::GetDay(int64_t sec)
	{
		boost::posix_time::ptime time = epoch + boost::posix_time::seconds(sec);
		auto tm = boost::posix_time::to_tm(time);
		return tm.tm_mday;
	}

	int32_t DateTime::GetMonth(int64_t sec)
	{
		boost::posix_time::ptime time = epoch + boost::posix_time::seconds(sec);
		auto tm = boost::posix_time::to_tm(time);
		return tm.tm_mon;
	}

	std::tm DateTime::GetTm(int64_t sec)
	{
		boost::posix_time::ptime time = epoch + boost::posix_time::seconds(sec);
		auto tm = boost::posix_time::to_tm(time);
		return tm;
	}
}