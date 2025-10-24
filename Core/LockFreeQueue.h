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
            std::atomic<Node*> _next;
        };

    public:
        LockFreeQueue() : _size(0)
        {
            _head = _tail = new Node();
        }

        void Push(T&& value)
        {
            auto node = new Node();
            node->_value = std::forward<T>(value);
            node->_next = nullptr;

            auto tail = _tail.load(std::memory_order_acquire);
            while (false == _tail.compare_exchange_strong(tail, node));
            tail->_next.store(node, std::memory_order_release);

            _size.fetch_add(1, std::memory_order_release);
        }

        bool Pop(T& output)
        {
            auto head = _head.load(std::memory_order_acquire);
            Node* popNode = nullptr;
            do
            {
                if (popNode = head->_next; popNode == nullptr)
                    return false;
            } while (false == _head.compare_exchange_strong(head, popNode));
            _size.fetch_sub(1, std::memory_order_release);

            output = std::move(popNode->_value);
            delete head;

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
            while (ConsumeOne(std::forward<Func>(func))) count++;

            return count;
        }

        void Clear()
        {
            T node;
            while (GetSize() <= 0)
                Pop(node);
        }

        int64_t GetSize() { return _size; }

    private:
        std::atomic<Node*> _head;
        std::atomic<Node*> _tail;
        std::atomic<int64_t> _size;
    };
}