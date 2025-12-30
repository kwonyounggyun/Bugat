#pragma once
#include "NonCopyable.h"
namespace bugat
{
	template <typename T>
	class Singleton : NonCopyable
	{
	public:
		Singleton() = default;
		virtual ~Singleton() {};

		static T& Instance()
		{
			static T instance;
			return instance;
		}
	};

	template <typename T>
	class SharedSingleton : NonCopyable
	{
	public:
		SharedSingleton() = default;
		virtual ~SharedSingleton() {};

		static std::shared_ptr<T>& Instance()
		{
			static std::shared_ptr<T> instance = std::make_shared<T>();
			return instance;
		}
	};
}