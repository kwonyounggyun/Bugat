#pragma once
#include <stack>
#include <shared_mutex>

namespace bugat
{
	template<typename T, bool ThreadSafe = false, int AllocSize = 10>
	class ObjectPool
	{
	public:
		std::shared_ptr<T> GetObj()
		{
			if (_mems.empty())
				Alloc();

			auto top = _mems.top();
			_mems.pop();

			return std::shared_ptr<T>(top, [this](T* ptr) {
				Push(ptr);
				});
		}

		void Release()
		{
			_release = true;
			while (!_mems.empty())
			{
				auto top = _mems.top();
				_mems.pop();
				delete top;
			}
		}

	private:
		void Push(T* ptr)
		{
			if (_release)
				delete ptr;

			_mems.push(ptr);
		}

		void Alloc()
		{
			T* newMem = new T[AllocSize];

			for (int i = 0; i < AllocSize; i++)
				Push(newMem + i);
		}

	private:
		std::stack<T*> _mems;
		bool _release;
	};

	template<typename T, int AllocSize>
	class ObjectPool<T, true, AllocSize>
	{
	public:
		std::shared_ptr<T> GetObj()
		{
			std::lock_guard lock(_mtx);
			if (_mems.empty())
				Alloc();

			auto top = _mems.top();
			_mems.pop();

			return std::shared_ptr<T>(top, [this](T* ptr) {
				std::lock_guard lock(_mtx);
				Push(ptr);
				});
		}

		void Release()
		{
			std::lock_guard lock(_mtx);
			_release = true;
			while (!_mems.empty())
			{
				auto top = _mems.top();
				_mems.pop();
				delete top;
			}
		}

	private:
		void Push(T* ptr)
		{
			if (_release)
				delete ptr;

			_mems.push(ptr);
		}

		void Alloc()
		{
			T* newMem = new T[AllocSize];

			for (int i = 0; i < AllocSize; i++)
				Push(newMem + i);
		}

	private:
		std::shared_mutex _mtx;
		std::stack<T*> _mems;
		bool _release;
	};
}