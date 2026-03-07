#pragma once
#include <memory>
#include <atomic>
#include "free_list.hpp"
#include "tagged_ptr.hpp"
#include "Memory.h"

namespace bugat
{
    template<typename T>
    class LockFreeQueue
    {
        struct Node;

        using node_ptr_t = lockfree::tagged_ptr<Node>;
        inline static const node_ptr_t nullHandle = node_ptr_t(0, 0);

        struct alignas(TAGGED_PTR_MAX_ALIGN) Node
        {
            Node() : _next(node_ptr_t(nullptr, 0)) {}
            Node(T& val, node_ptr_t next) : _value(val), _next(node_ptr_t(next)) {}
            Node(T&& val, node_ptr_t next) : _value(std::move(val)), _next(node_ptr_t(next)) {}
            ~Node() = default;

            std::atomic<node_ptr_t> _next;
            T _value;
        };

        using pool_t = lockfree::freelist_stack<Node, std::allocator<Node>>;


    public:
        LockFreeQueue() : _size(0)
        {
            auto temp = _pool.construct();
            node_ptr_t tempPtr(temp, 0);
            tempPtr->_next.store(nullHandle, std::memory_order_release);
            _head->store(tempPtr);
            _tail->store(tempPtr);
        }

        ~LockFreeQueue()
        {
            Clear();
        }

        bool IsLockFree() const
        {
            return _head->is_lock_free() && _tail->is_lock_free() && _size->is_lock_free() && _pool.IsLockFree();
        }

        int64_t Push(T& value)
        {
            auto node = _pool.construct(value, nullHandle);
            return InternalPush(node);
        }

        int64_t Push(T&& value)
        {
            auto node = _pool.construct(std::move(value), nullHandle);
            return InternalPush(node);
        }

        bool Pop(T& output)
        {
            while (true)
            {
                auto tail = _tail->load(std::memory_order_acquire);
                auto head = _head->load(std::memory_order_acquire);
                auto popNode = head->_next.load(std::memory_order_acquire);

                if (head == _head->load(std::memory_order_relaxed))
                {
                    if (head.get_ptr() == tail.get_ptr())
                    {
                        if (popNode.get_ptr() == nullptr)
                            return false;

                        _tail->compare_exchange_weak(tail, node_ptr_t(popNode.get_ptr(), tail.get_next_tag()), std::memory_order_acq_rel, std::memory_order_acquire);
                    }
                    else
                    {
                        if (popNode.get_ptr() != nullptr)
                        {
                            T value = popNode->_value;

                            auto newNode = node_ptr_t(popNode.get_ptr(), head.get_next_tag());

                            if (_head->compare_exchange_weak(head, newNode, std::memory_order_acq_rel, std::memory_order_acquire))
                            {
                                auto size = _size->fetch_sub(1, std::memory_order_relaxed);
                                output = std::move(value);

                                _pool.destruct(head.get_ptr());
                                return true;
                            }
                        }
                    }
                }
            }
        }

        template<typename Func>
        bool ConsumeOne(Func&& func)
        {
            T value;
            if (false == Pop(value))
                return false;

            func(value);
            return true;
        }

        template<typename Func>
        int64_t ConsumeAll(Func&& func)
        {
            int64_t count = 0;
            while (ConsumeOne(func)) count++;

            return count;
        }

        template<typename Func>
        int64_t Consume(int64_t count, Func&& func)
        {
            int64_t executeCount = 0;
            while (count > 0)
            {
                if (false == ConsumeOne(func))
                    break;

                count--;
                executeCount++;
            }

            return executeCount;
        }

        void Clear()
        {
            ConsumeAll([](T& value) {
                });
        }

        int64_t GetSize() { return _size->load(std::memory_order_acquire); }

    private:
        int64_t InternalPush(Node* node)
        {
            while (true)
            {
                auto tail = _tail->load(std::memory_order_acquire);
                auto next = tail->_next.load(std::memory_order_acquire);

                if (tail == _tail->load(std::memory_order_relaxed))
                {
                    if (next.get_ptr() == nullptr)
                    {
                        node_ptr_t newNode(node, next.get_next_tag());

                        if (tail->_next.compare_exchange_weak(next, newNode, std::memory_order_acq_rel, std::memory_order_acquire))
                        {
                            _tail->compare_exchange_weak(tail, node_ptr_t(node, tail.get_next_tag()), std::memory_order_acq_rel, std::memory_order_acquire);
                            break;
                        }
                    }
                    else
                    {
                        _tail->compare_exchange_weak(tail, node_ptr_t(next.get_ptr(), tail.get_next_tag()), std::memory_order_acq_rel, std::memory_order_acquire);
                    }
                }
            }

            auto size = _size->fetch_add(1, std::memory_order_relaxed);
            return size + 1;
        }

    private:
        CacheLinePadding<std::atomic<node_ptr_t>> _head;
        CacheLinePadding<std::atomic<node_ptr_t>> _tail;
        CacheLinePadding<std::atomic<int64_t>> _size;

        pool_t _pool;
    };
}