#pragma once
#include <atomic>

namespace bugat
{
	struct Executor;
	struct ContextImpl;
	class NetworkContext
	{
	public:
		NetworkContext();

		void Initialize();
		void Run();
		void Stop();

		Executor GetExecutor() const noexcept;

	private:
		std::shared_ptr<ContextImpl> _context;
		std::atomic_bool _stop;
	};
}