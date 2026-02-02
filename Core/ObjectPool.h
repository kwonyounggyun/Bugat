#pragma once
#include <vector>
#include <atomic>
#include "Memory.h"
#include "LockObject.h"

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
				~MemoryBlock() {}

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
							auto block = std::move(memPtr->_block);
							memPtr->~Member();
						}
						});
				}

				Member* _next;
				T* _ptr;
				TSharedPtr<MemoryBlock> _block; //집나간동안 해제되면안됨..
			};

			struct Link
			{
				Link(Link* ptr) : _next(ptr) {}
				Link* _next;
			};

		public:
			Pool() : _end(new Link(nullptr))
			{
				_top.store(_end, std::memory_order_release);
			}

			~Pool()
			{
				auto curTop = _top.load(std::memory_order_acquire);
				while (true)
				{
					if (curTop == _end)
						break;

					if (true == _top.compare_exchange_strong(curTop, curTop->_next, std::memory_order_acq_rel))
						reinterpret_cast<Member*>(curTop)->~Member();
				}

				_blocks.clear();

				delete _end;
			}

			void Init(int defaultPoolSize)
			{
				Alloc(defaultPoolSize);
			}

			void Release()
			{
				for (auto& block : _blocks)
					block->Release();
			}

			template<typename ...Args>
			TSharedPtr<T> Get(Args&&... args)
			{
				auto mem = Pop();

				auto sptr = mem->Get(std::forward<Args>(args)...);
				return sptr;
			}

		private:
			Member* Pop()
			{
				auto curTop = _top.load(std::memory_order_acquire);
				while (true)
				{
					if (curTop == _end)
					{
						if (auto lock = ScopedLock(_allocLock); lock)
							Alloc(AllocSize);

						curTop = _top.load(std::memory_order_acquire);
						continue;
					}

					if (true == _top.compare_exchange_strong(curTop, curTop->_next, std::memory_order_acq_rel))
						break;
				}

				return reinterpret_cast<Member*>(curTop);
			}

			void Push(Member* member)
			{
				auto voidPtr = reinterpret_cast<void*>(member);
				auto curTop = _top.load(std::memory_order_acquire);
				new(voidPtr)Link(curTop);
				while (false == _top.compare_exchange_strong(curTop, reinterpret_cast<Link*>(voidPtr), std::memory_order_acq_rel))
					new(voidPtr)Link(curTop);
			}

			void Alloc(int size)
			{
				if (size == 0)
					return;

				auto memberSize = sizeof(Member);
				auto memberTotalSize = memberSize + sizeof(T);

				auto memoryBlockSize = sizeof(MemoryBlock);
				void* memoryBlock = ::operator new(memoryBlockSize + memberTotalSize * size);

				auto startBlockPtr = static_cast<char*>(memoryBlock) + memoryBlockSize;
				new(memoryBlock)MemoryBlock(TSharedPtr(this), startBlockPtr);
				auto blockPtr = TSharedPtr<MemoryBlock>(static_cast<MemoryBlock*>(memoryBlock));
				_blocks.push_back(blockPtr);

				for (int i = 0; i < size; i++)
				{
					auto memberPtr = startBlockPtr + (memberTotalSize * i);
					new(memberPtr)Member(blockPtr, reinterpret_cast<T*>(memberPtr + memberSize));
					auto cast = reinterpret_cast<Member*>(memberPtr);
					Push(cast);
				}
			}

		private:
			std::vector<TSharedPtr<MemoryBlock>> _blocks;
			LockObject _allocLock;

			std::atomic<Link*> _top;
			Link* _end;
		};

	public:
		ObjectPool() : _pool(new Pool())
		{
		}

		~ObjectPool()
		{
			_pool->Release();
			_pool.Reset();
		}

		void Init(int DefaultBlockSize = 0)
		{
			_pool->Init(DefaultBlockSize);
		}

		template<typename ...Args>
		TSharedPtr<T> Get(Args&&... args)
		{
			return _pool->Get(std::forward<Args>(args)...);
		}

	private:
		TSharedPtr<Pool> _pool;
	};

	/// <summary>
	/// 같은 타입의 객체를 한곳에서 할당 받기위해 제공하는 전역함수
	/// </summary>
	/// <typeparam name="T">객체 타입으로 RefCountable을 상속받은 타입만 가능</typeparam>
	/// <typeparam name="AllocSize">한번의 할당에 생성할 수 있는 객체 수</typeparam>
	/// <returns>오브젝트 풀</returns>
	template<typename T, int AllocSize = 10 >
	ObjectPool<T, AllocSize>& GetObjectPool()
	{
		static ObjectPool<T, AllocSize> pool;
		return pool;
	}
}