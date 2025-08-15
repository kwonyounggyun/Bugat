#pragma once
#include <stack>

template<typename T, int AllocSize = 10>
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