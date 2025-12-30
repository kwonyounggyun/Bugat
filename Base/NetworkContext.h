#pragma once
#include <atomic>
#include "../Core/NonCopyable.h"

namespace bugat
{
	struct Executor;
	struct ContextImpl;
	class NetworkContext : public NonCopyable
	{
	public:
		NetworkContext();

		void Initialize();
		int64_t Run();
		void RunOne();
		void Stop();

		Executor GetExecutor() const noexcept;

	private:
		std::shared_ptr<ContextImpl> _context;
		std::atomic_bool _stop;
	};
}