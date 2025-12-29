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

			struct Member
			{
				Member(std::weak_ptr<Pool> pool, std::shared_ptr<MemoryBlock>& block) : _pool(pool), _block(block) {}
				~Member() 
				{
					_pool.reset();
					_block.reset();
				}

				bool Return()
				{
					if (auto sptr = _pool.lock(); sptr)
					{
						sptr->Push(this);
						return true;
					}

					return false;
				}

				std::shared_ptr<T> Get()
				{
					// ptr은 MemoryBlock이 관리하기 때문에 delete 하지 않음.
					return std::shared_ptr<T>(&obj, [memPtr = this](T* ptr) {
						ptr->~T();
						if (false == memPtr->Return())
						{
							// block이 실제 메모리라 member의 소멸자 호출 보다 먼저 해제되면 안되기때문에 임시 저장
							auto block = memPtr->_block;
							memPtr->~Member();
						}
						});
				}

				std::weak_ptr<Pool> _pool;
				std::shared_ptr<MemoryBlock> _block; //집나간동안 해제되면안됨..
				T obj;
			};

		public:
			Pool() {}
			~Pool()
			{
				_mems.ConsumeAll([](Member* mem) {
					mem->~Member();
					});
				_blocks.clear();
			}

			template<typename ...Args>
			std::shared_ptr<T> Get(Args&&... args)
			{
				Member* mem;
				while(false == _mems.Pop(mem))
					Alloc();

				auto sptr = mem->Get();
				new(sptr.get())T(std::forward<Args>(args)...);
				return sptr;
			}

		private:
			void Push(Member* member)
			{
				_mems.Push(member);
			}

			void Alloc()
			{
				if (true == _isAllocating.test_and_set(std::memory_order_acquire))
					return;

				void* memoryBlock = operator new(sizeof(Member) * AllocSize);
				auto blockPtr = std::make_shared<MemoryBlock>(memoryBlock);
				_blocks.push_back(blockPtr);

				auto castBlock = static_cast<Member*>(memoryBlock);
				for (int i = 0; i < AllocSize; i++)
				{
					new(castBlock + i)Member(this->weak_from_this(), blockPtr);
					Push(castBlock + i);
				}

				_isAllocating.clear(std::memory_order_release);
			}

		private:
			LockFreeQueue<Member*> _mems;
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