#pragma once
#include <memory>

namespace bugat
{
    template<typename T>
    class LockFreeQueue
    {
        struct Node
        {
            Node() : _next(nullptr) {}
            Node(const T& val) : _value(val), _next(nullptr) {}
            Node(T&& val) : _value(std::move(val)), _next(nullptr) {}

            T _value;
            std::atomic<std::shared_ptr<Node>> _next;
        };

    public:
        LockFreeQueue() : _size(0)
        {
            auto temp = std::make_shared<Node>();
            _head = temp;
            _tail = temp;
        }

        int64_t Push(T& value)
        {
            auto node = std::make_shared<Node>(value);
            return InternalPush(node);
        }

        int64_t Push(T&& value)
        {
            auto node = std::make_shared<Node>(std::move(value));
            return InternalPush(node);
        }

        bool Pop(T& output)
        {
            auto head = _head.load(std::memory_order_acquire);
            while (true)
            {
                auto tail = _tail.load(std::memory_order_acquire);
                auto popNode = head->_next.load(std::memory_order_acquire);

                if (popNode == nullptr)
                    return false;

                if (head == tail)
                {
                    _tail.compare_exchange_strong(tail, popNode, std::memory_order_acq_rel);
                    head = _head.load(std::memory_order_acquire);
                    continue;
                }

                if (false == _head.compare_exchange_strong(head, popNode, std::memory_order_acq_rel, std::memory_order_acquire))
                    continue;

                _size.fetch_sub(1, std::memory_order_relaxed);
                output = std::move(popNode->_value);

                head->_next.store(nullptr, std::memory_order_release);
                break;
            }

            return true;
        }

		//작업 하나를 소비한다.
        template<typename Func>
        bool ConsumeOne(Func&& func)
        {
            T value;
            if (false == Pop(value))
                return false;

            func(value);
            return true;
        }

		//큐가 빌때까지 모든 작업을 소비한다.
        template<typename Func>
        int64_t ConsumeAll(Func&& func)
        {
            int64_t count = 0;
            while (ConsumeOne(func)) count++;

            return count;
        }

		//지정한 수만 큼 소비하는데 큐가 비었으면 멈춘다.
        template<typename Func>
        int64_t Consume(int64_t count, Func&& func)
        {
            int64_t progressCount = 0;
            while (progressCount < count)
            {
                if (ConsumeOne(func))
                    progressCount++;
                else
                    break;
            }

            return progressCount;
        }

        void Clear()
        {
            T node;
            while (GetSize() > 0)
                Pop(node);
        }

        int64_t GetSize() { return _size; }

    private:
        int64_t InternalPush(std::shared_ptr<Node>& node)
        {
            while (true)
            {
                auto tail = _tail.load(std::memory_order_acquire);
                std::shared_ptr<Node> next = nullptr;
                if (tail->_next.compare_exchange_strong(next, node, std::memory_order_acq_rel, std::memory_order_acquire))
                {
                    _tail.compare_exchange_strong(tail, node, std::memory_order_acq_rel, std::memory_order_acquire);
                    break;
                }
                else
                    _tail.compare_exchange_strong(tail, next, std::memory_order_acq_rel, std::memory_order_acquire);
            }

            auto size = _size.fetch_add(1, std::memory_order_relaxed);
            return size + 1;
        }

    private:
        std::atomic<std::shared_ptr<Node>> _head;
        std::atomic<std::shared_ptr<Node>> _tail;
        std::atomic<int64_t> _size;
    };
}