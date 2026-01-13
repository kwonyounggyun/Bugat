#pragma once
#include <boost/redis/src.hpp>

namespace bugat
{
	using RedisRequest = boost::redis::request;
	template<typename ...Res>
	using RedisResponse = boost::redis::response<Res...>;
	using BoostRedisConnection = boost::redis::connection;

	using RedisError = boost::system::error_code;
}