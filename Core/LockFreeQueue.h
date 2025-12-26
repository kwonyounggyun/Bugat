#pragma once

namespace bugat
{
    template<typename T>
    class LockFreeQueue
    {
        struct Node
        {
            Node() : _next(nullptr) {}
            T _value;
            std::atomic<std::shared_ptr<Node>> _next;
        };

    public:
        LockFreeQueue() : _size(0)
        {
            auto temp = new Node();
            _head = std::shared_ptr<Node>(temp);
            _tail = temp;
        }

        int64_t Push(T& value)
        {
            auto node = new Node();
            node->_value = value;
            node->_next = nullptr;

            auto tail = _tail.load(std::memory_order_acquire);
            while (false == _tail.compare_exchange_strong(tail, node, std::memory_order_acq_rel, std::memory_order_acquire));
            tail->_next.store(std::shared_ptr<Node>(node), std::memory_order_release);

            auto size = _size.fetch_add(1, std::memory_order_release);
            return size + 1;
        }

        int64_t Push(T&& value)
        {
            auto node = new Node();
            node->_value = std::move(value);
            node->_next = nullptr;

            auto tail = _tail.load(std::memory_order_acquire);
            while (false == _tail.compare_exchange_strong(tail, node, std::memory_order_acq_rel, std::memory_order_acquire));
            tail->_next.store(std::shared_ptr<Node>(node), std::memory_order_release);

            auto size = _size.fetch_add(1, std::memory_order_release);
            return size + 1;
        }

        bool Pop(T& output)
        {
            auto head = _head.load(std::memory_order_acquire);
            std::shared_ptr<Node> popNode = nullptr;
            do
            {
                if (popNode = head->_next.load(std::memory_order_acquire); popNode == nullptr)
                    return false;
            } while (false == _head.compare_exchange_strong(head, popNode, std::memory_order_acq_rel, std::memory_order_acquire));
            _size.fetch_sub(1, std::memory_order_release);

            output = std::move(popNode->_value);
            head->_next = nullptr;

            return true;
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

        void Clear()
        {
            T node;
            while (GetSize() > 0)
                Pop(node);
        }

        int64_t GetSize() { return _size; }

    private:
        std::atomic<std::shared_ptr<Node>> _head;
        std::atomic<Node*> _tail;
        std::atomic<int64_t> _size;
    };
}