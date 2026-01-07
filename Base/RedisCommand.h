#pragma once
#include <string>
#include <optional>
#include "../Core/NonCopyable.h"
#include "BoostRedis.h"

namespace bugat
{
	class RedisCommand : NonCopyable
	{
	public:
		virtual void Command(RedisRequest& req) = 0;
	};

	template<typename T>
	class Set : public RedisCommand
	{
	public:
		using resType = std::string;
		Set(const std::string& key, const T& value) : _key(key), _value(value) {}
		virtual ~Set() {}

		virtual void Command(RedisRequest& req)
		{
			req.push("SET", _key, _value);
		}

	private:
		const std::string& _key;
		const T& _value;
	};

	template<typename T>
	class Get : public RedisCommand
	{
	public:
		using resType = std::optional<T>;
		Get(const std::string& key) : _key(key) {}

		virtual void Command(RedisRequest& req)
		{
			req.push("GET", _key);
		}

	private:
		const std::string& _key;
	};
}