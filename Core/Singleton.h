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
}