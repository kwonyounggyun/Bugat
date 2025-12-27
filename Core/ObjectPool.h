#pragma once
#include <stack>
#include <vector>
#include <memory>
#include <atomic>
#include "LockFreeQueue.h"

namespace bugat
{
	template<typename T, int AllocSize>
	class ObjectPool
	{
		class Pool : public std::enable_shared_from_this<Pool>
		{
			struct MemoryBlock
			{
				MemoryBlock(void* p) : _ptr(p) {}
				~MemoryBlock()
				{
					delete[] _ptr;
				}

				void* _ptr;
			};

			struct Member : public std::enable_shared_from_this<Member>
			{
				Member(std::weak_ptr<Pool> pool, std::shared_ptr<MemoryBlock>& block, T* ptr) : _pool(pool), _block(block), _ptr(ptr) {}
				~Member()
				{
					// 반환 시 Pool에 다시 푸시
					if (auto sptr = _pool.lock(); sptr)
						sptr->Push(_block, _ptr);
				}

				std::shared_ptr<T> Get()
				{
					// 커스텀 삭제자를 사용하여 Pool이 살아있는 동안만 메모리가 유지되도록 함, ptr은 MemoryBlock이 관리하기 때문에 delete 하지 않음.
					return std::shared_ptr<T>(_ptr, [mem = this->shared_from_this()](T* ptr) {
						ptr->~T();
						});
				}

				std::weak_ptr<Pool> _pool;
				std::shared_ptr<MemoryBlock> _block;
				T* _ptr;
			};

		public:
			Pool() {}
			~Pool()
			{
				_mems.Clear();
				_blocks.clear();
			}

			template<typename ...Args>
			std::shared_ptr<T> Get(Args&&... args)
			{
				std::shared_ptr<Member> mem;
				while(false == _mems.Pop(mem))
					Alloc();

				auto sptr = mem->Get();
				new(sptr.get())T(std::forward<Args>(args)...);
				return sptr;
			}

		private:
			void Push(std::shared_ptr<MemoryBlock>& block, T* ptr)
			{
				_mems.Push(std::make_shared<Member>(this->weak_from_this(), block, ptr));
			}

			void Alloc()
			{
				if (true == _isAllocating.test_and_set(std::memory_order_acquire))
					return;

				void* memoryBlock = operator new(sizeof(T) * AllocSize);
				auto blockPtr = std::make_shared<MemoryBlock>(memoryBlock);
				_blocks.push_back(blockPtr);

				auto castBlock = static_cast<T*>(memoryBlock);
				for (int i = 0; i < AllocSize; i++)
					Push(blockPtr, castBlock + i);

				_isAllocating.clear(std::memory_order_release);
			}

		private:
			LockFreeQueue<std::shared_ptr<Member>> _mems;
			std::vector<std::shared_ptr<MemoryBlock>> _blocks;
			std::atomic_flag _isAllocating;
		};

	public:
		ObjectPool() : _pool(std::make_shared<Pool>()) {}

		template<typename ...Args>
		std::shared_ptr<T> Get(Args&&... args)
		{
			return _pool->Get(std::forward(args)...);
		}

	private:
		std::shared_ptr<Pool> _pool;
	};
}