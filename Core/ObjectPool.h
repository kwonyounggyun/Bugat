#pragma once
#include <stack>
#include <vector>
#include <memory>

namespace bugat
{
	template<typename T, int AllocSize>
	class ObjectPool
	{
		friend class ObjectPoolFactory;

		struct MemoryBlock
		{
			MemoryBlock(std::weak_ptr<ObjectPool>& pool, void* p) : _pool(pool), _ptr(p) {}
			~MemoryBlock()
			{
				delete[] _ptr;
			}

			void SetIdx(int idx) { _idx = idx; }
			void Push(T* ptr)
			{
				_pool.lock()->Push(ptr, _idx);
			}

			std::weak_ptr<ObjectPool> _pool;
			void* _ptr;
			int _idx;
		};

		ObjectPool() : _allocCount(0) {}
	public:
		~ObjectPool()
		{
			while(!_mems.empty())
				_mems.pop();

			_blocks.clear();
		}

		template<typename ...Args>
		std::shared_ptr<T> Get(Args&&... args)
		{
			if (_mems.empty())
				Alloc();

			auto [memory, blockIdx] = _mems.top();
			_mems.pop();

			auto blockPtr = _blocks[blockIdx];
			new(memory)T(std::forward<Args>(args)...);
			auto blockPtr1 = _blocks[blockIdx];
			auto blockPtr2 = _blocks[blockIdx];
			auto blockPtr3 = _blocks[blockIdx];
			auto blockPtr4 = _blocks[blockIdx];
			auto blockPtr5 = _blocks[blockIdx];
			return std::shared_ptr<T>(memory, [blockPtr](T* ptr) {
				ptr->~T();
				blockPtr->Push(ptr);
				});
		}

	private:
		void Push(T* ptr, int blockIdx)
		{
			_mems.emplace(ptr, blockIdx);
		}

		void Alloc()
		{
			void* memoryBlock = operator new(sizeof(T) * AllocSize);
			auto blockPtr = std::make_shared<MemoryBlock>(_self, memoryBlock);
			_blocks.push_back(blockPtr);
			auto idx = _blocks.size() - 1;
			blockPtr->SetIdx(idx);

			auto castBlock = static_cast<T*>(memoryBlock);
			for (int i = 0; i < AllocSize; i++)
				Push(castBlock + i, idx);
		}

	private:
		std::stack<std::tuple<T*, int>> _mems;
		std::vector<std::shared_ptr<MemoryBlock>> _blocks;
		uint32_t _allocCount;

		std::weak_ptr<ObjectPool> _self;
	};

	class ObjectPoolFactory
	{
	public:
		template<typename T, int AllocSize = 10>
		static std::shared_ptr<ObjectPool<T, AllocSize>> Create()
		{
			auto ptr = new ObjectPool<T, AllocSize>();
			auto sptr = std::shared_ptr<ObjectPool<T, AllocSize>>(ptr);
			sptr->_self = sptr;
			return sptr;
		}
	};

	/*template<typename T, int AllocSize>
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
	};*/
}