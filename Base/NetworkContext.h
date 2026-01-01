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
		void Run();
		void Stop();

		Executor GetExecutor() const noexcept;

	private:
		std::shared_ptr<ContextImpl> _context;
		std::atomic_bool _stop;
	};
}