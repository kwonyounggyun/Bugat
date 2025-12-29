#pragma once
#include <atomic>
#include "DateTime.h"

namespace bugat
{
	/*
	* Milliseconds 간격으로 카운터 값을 업데이트 한다.
	* 여러스레드에서 동시에 Update를 호출할 수 있다.
	* Update가 호출되는 순간 시간을 체크함으로 정확한 간격은 아니다.
	*/
	template<int Milliseconds>
	class Counter
	{
	public:
		Counter() : _lastUpdateMs(DateTime::NowMs()) { }
		void Update(int64_t value)
		{
			auto pre = _curCount.fetch_add(value, std::memory_order_release);
			auto lastUpdateMs = _lastUpdateMs.load(std::memory_order_acquire);
			auto curMs = DateTime::NowMs();
			if (auto term = curMs - lastUpdateMs; term > Milliseconds)
			{
				if (_lastUpdateMs.compare_exchange_strong(lastUpdateMs, curMs, std::memory_order_acq_rel, std::memory_order_acquire))
				{
					_curCount.store(0, std::memory_order_release);
					auto count = static_cast<int64_t>(static_cast<double>(pre + value) / term * Milliseconds);
					_preCount.store(count, std::memory_order_release);
				}
			}
		}

		int64_t GetCount() const
		{
			return _preCount.load(std::memory_order_acquire);
		}

	private:
		std::atomic<int64_t> _curCount;
		std::atomic<int64_t> _preCount;
		std::atomic<int64_t> _lastUpdateMs;
	};
}