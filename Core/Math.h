#pragma once
#include <random>

namespace bugat
{
	class Math
	{
	public:
		template<typename T>
		static T Random(T min, T max)
		{
			static std::random_device rd;
			static std::mt19937 gen(rd());

			std::uniform_int_distribution<T> dis(min, max);
			return dis(gen);
		}
	};
}