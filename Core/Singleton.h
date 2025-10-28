#pragma once
#include "NonCopyable.h"
namespace bugat
{
	template <typename T>
	class Singleton : NonCopyable
	{
	public:
		Singleton() = default;
		virtual ~Singleton() = 0 {};

		static T& Instance()
		{
			static T instance;
			return instance;
		}
	};
}