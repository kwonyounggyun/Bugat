#pragma once
#include <stack>
#include <vector>
#include <memory>
#include <atomic>
#include "LockFreeQueue.h"
#include "Memory.h"

namespace bugat
{
	template<typename T, int AllocSize = 10>
	requires std::is_base_of_v<RefCountable, T>
	class ObjectPool
	{
		class Pool : public RefCountable
		{
			struct MemoryBlock : public RefCountable
			{
				MemoryBlock(TSharedPtr<Pool> pool, void* p) : _pool(pool), _ptr(p) {}
				~MemoryBlock()
				{
					::operator delete(_ptr);
				}

				TSharedPtr<Pool>& GetPool()
				{
					return _pool;
				}

				void Release()
				{
					_pool = nullptr;
				}

				TSharedPtr<Pool> _pool;
				void* _ptr;
			};

			struct Member
			{
				Member(TSharedPtr<MemoryBlock>& block, T* ptr) : _block(block), _ptr(ptr) {}
				~Member() 
				{
					_block.Reset();
				}

				bool Return()
				{
					if (auto& sptr = _block->GetPool(); sptr)
					{
						sptr->Push(this);
						return true;
					}

					return false;
				}

				template<typename ...Args>
				TSharedPtr<T> Get(Args&&... args)
				{
					// ptr은 MemoryBlock이 관리하기 때문에 delete 하지 않음.
					new(_ptr)T(std::forward<Args>(args)...);
					return TSharedPtr<T>(_ptr, [memPtr = this](T* ptr) {
						ptr->~T();
						if (false == memPtr->Return())
						{
							// block이 실제 메모리라 member의 소멸자 호출 보다 먼저 해제되면 안되기때문에 임시 저장
							auto block = memPtr->_block;
							memPtr->~Member();
						}
						});
				}

				TSharedPtr<MemoryBlock> _block; //집나간동안 해제되면안됨..
				T* _ptr;
			};

		public:
			Pool() 
			{
			}

			~Pool()
			{
				_mems.ConsumeAll([](Member* mem) {
					mem->~Member();
					});
				while(!_blocks.empty())
					_blocks.pop();
			}

			void Init(int defaultPoolSize)
			{
				Alloc(defaultPoolSize);
			}

			template<typename ...Args>
			TSharedPtr<T> Get(Args&&... args)
			{
				Member* mem;
				while (false == _mems.Pop(mem))
				{
					if (_mems.GetSize() > 0)
						continue;
					
					Alloc(AllocSize);
				}

				auto sptr = mem->Get(std::forward<Args>(args)...);
				return sptr;
			}

		private:
			void Push(Member* member)
			{
				_mems.Push(member);
			}

			void Alloc(int size)
			{
				if (size == 0)
					return;

				if (true == _isAllocating.test_and_set(std::memory_order_acquire))
					return;

				auto memberSize = sizeof(Member);
				auto totalSize = memberSize + sizeof(T);

				void* memoryBlock = ::operator new(totalSize * size);
				new(memoryBlock)MemoryBlock(TSharedPtr(this), memoryBlock);
				auto blockPtr = TSharedPtr<MemoryBlock>(reinterpret_cast<MemoryBlock*>(memoryBlock));
				_blocks.push(blockPtr);

				auto castPtr = static_cast<char*>(memoryBlock);
				for (int i = 0; i < size; i++)
				{
					auto memberPtr = castPtr + (totalSize * i);
					new(memberPtr)Member(blockPtr, reinterpret_cast<T*>(memberPtr + memberSize));
					Push(reinterpret_cast<Member*>(memberPtr));
				}

				_isAllocating.clear(std::memory_order_release);
			}

		private:
			LockFreeQueue<Member*> _mems;
			std::stack<TSharedPtr<MemoryBlock>> _blocks;
			std::atomic_flag _isAllocating;
		};

	public:
		ObjectPool() : _pool(new Pool())
		{
		}

		void Init(int DefaultBlockSize = 0)
		{
			_pool->Init(DefaultBlockSize);
		}

		template<typename ...Args>
		TSharedPtr<T> Get(Args&&... args)
		{
			return _pool->Get(std::forward(args)...);
		}

	private:
		TSharedPtr<Pool> _pool;
	};
}