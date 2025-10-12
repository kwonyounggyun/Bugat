#pragma once
template <typename T>
class Singleton
{
public:
	Singleton() = default;
	virtual ~Singleton() = 0 {};

	static T& Instance()
	{
		static T instance;
		return instance;
	}

	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;
};