#pragma once

namespace bugat
{
	class NonCopyable
	{
	public:
		NonCopyable() {}
		virtual ~NonCopyable() = 0 {}
		NonCopyable(NonCopyable&) = delete;
		NonCopyable& operator=(NonCopyable&) = delete;
	};
}
