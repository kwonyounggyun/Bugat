#pragma once
#include <string>
#include <type_traits>
#include <memory>
#include <vector>
#include "JsonObject.h"

class RedisRequest;
void AddCommand(RedisRequest& req, const std::string& command, const std::initializer_list<std::string>& params);

template<typename T>
struct SetCommand
{
public:
	using resType = std::decay_t<decltype(std::ignore)>;
	SetCommand(const std::string& key, T& value) : _key(key), _value(value) {}
	virtual ~SetCommand() {}
	
	virtual void Command(RedisRequest& req)
	{
		if constexpr (std::is_class_v<T>)
			AddCommand(req, "SET", { _key, ObjectToJson<T>(_value) });
		else
			AddCommand(req, "SET", { _key, _value });
	}

private:
	std::string _key;
	T _value;
};

template<typename Res>
struct GetCommand
{
	using resType = Res;
	GetCommand(const std::string& key) : _key(key) {}

	virtual void Command(RedisRequest& req)
	{
		AddCommand(req, "GET", { _key });
	}

private:
	std::string _key;
};