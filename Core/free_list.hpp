#include "tagged_ptr.hpp"
#include <memory>
#include <cstring>

#ifndef BUGAT_LOCKFREE_FREE_LIST_INCLUDED
#define BUGAT_LOCKFREE_FREE_LIST_INCLUDED
/// <summary>
/// �޸� ���������ʰ� �����ϴ� ����Ʈ
/// </summary>
namespace bugat::lockfree
{
	template<typename T, typename Alloc = std::allocator<T>>
	class freelist_stack : public Alloc
	{
		struct freelist_node
		{
			tagged_ptr<freelist_node> _next;
		};

		using freelist_node_ptr = tagged_ptr<freelist_node>;

	public:
		freelist_stack() : allocCount(0), deallocCount(0)
		{
		}

		template<typename Allocator>
		freelist_stack(const Allocator& other) : Alloc(other)
		{
		}

		~freelist_stack()
		{
			auto top = _top.load(std::memory_order_acquire);
			while (top.get_ptr() != nullptr)
			{
				auto next = top->_next;
				T* ptr = reinterpret_cast<T*>(top.get_ptr());
				Alloc::deallocate(ptr, 1);
				top = next;
			}
		}

		template<typename ...ARGS>
		T* construct(ARGS&&... args)
		{
			auto ptr = allocate();
			new(ptr)T(std::forward<ARGS>(args)...);
			return ptr;
		}

		void destruct(T* ptr)
		{
			ptr->~T();
			deallocate(ptr);
		}

		bool IsLockFree() const
		{
			return _top.is_lock_free();
		}

	private:
		T* allocate()
		{
			auto top = _top.load(std::memory_order_acquire);
			while(true)
			{
				if (top.get_ptr() == nullptr)
				{
					auto ptr = Alloc::allocate(1);
					std::memset(ptr, 0x00, sizeof(T));
					//std::cout << "alloc : " << ++allocCount << std::endl;
					return ptr;
				}

				freelist_node* newNodePtr = top->_next.get_ptr();
				freelist_node_ptr newNode(newNodePtr, top.get_next_tag());
				if (_top.compare_exchange_weak(top, newNode))
					return reinterpret_cast<T*>(top.get_ptr());
			}
		}

		void deallocate(T* ptr)
		{
			auto nodePtr = reinterpret_cast<freelist_node*>(ptr);
			auto top = _top.load(std::memory_order_acquire);
			
			while(true)
			{
				freelist_node_ptr newNode(nodePtr, top.get_tag());
				newNode->_next.set(top.get_ptr(), 0);
				if (_top.compare_exchange_weak(top, newNode))
				{
					//std::cout << "dealloc : " << ++deallocCount << std::endl;
					return;
				}
			} 
		}

	private:
		int allocCount;
		int deallocCount;
		std::atomic<freelist_node_ptr> _top;
	};
}
#endif