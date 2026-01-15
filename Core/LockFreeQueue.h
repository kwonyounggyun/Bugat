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
            _dummy = std::make_shared<Node>();
            auto temp = std::make_shared<Node>();
			temp->_next.store(_dummy, std::memory_order_release);
            _head = temp;
            _tail = temp;
        }

        int64_t Push(T& value)
        {
            auto node = std::make_shared<Node>(value);
			node->_next.store(_dummy, std::memory_order_release);
            return InternalPush(node);
        }

        int64_t Push(T&& value)
        {
            auto node = std::make_shared<Node>(std::move(value));
            node->_next.store(_dummy, std::memory_order_release);
            return InternalPush(node);
        }

        bool Pop(T& output)
        {
			int64_t outRemainCount;
			return Pop(output, outRemainCount);
        }

        /// <summary>
        /// 데이터를 하나 꺼낸다. outRemainCount는 현재 큐 사이즈를 반환하는데 atomic연산 순서에 따라 -
        /// </summary>
        /// <param name="output">꺼낸 요소</param>
        /// <param name="outRemainCount">현재 큐 사이즈</param>
        /// <returns></returns>
        bool Pop(T& output, int64_t& outRemainCount)
        {
            outRemainCount = 0;
            auto head = _head.load(std::memory_order_acquire);
            while (true)
            {
                auto tail = _tail.load(std::memory_order_acquire);
                auto popNode = head->_next.load(std::memory_order_acquire);

                if (popNode == _dummy)
                    return false;

                if (head == tail)
                {
                    _tail.compare_exchange_strong(tail, popNode, std::memory_order_acq_rel, std::memory_order_acquire);
                    head = _head.load(std::memory_order_acquire);
                    continue;
                }

                if (false == _head.compare_exchange_strong(head, popNode, std::memory_order_acq_rel, std::memory_order_acquire))
                    continue;

                outRemainCount = _size.fetch_sub(1, std::memory_order_relaxed) - 1;
                output = std::move(popNode->_value);

                head->_next.store(nullptr, std::memory_order_release);
                break;
            }

            return true;
        }

        /// <summary>
        /// 작업하나를 소비한다.
        /// </summary>
        /// <typeparam name="Func"></typeparam>
        /// <param name="func">value타입을 파라미터로 받는 함수객체</param>
        /// <returns>
        /// 성공 true, 실패 false
        /// </returns>
        template<typename Func>
        bool ConsumeOne(Func&& func)
        {
            T value;
            if (false == Pop(value))
                return false;

            func(value);
            return true;
        }

        /// <summary>
        /// 큐가 빌때까지 모든 작업을 소비한다.
        /// </summary>
        /// <typeparam name="Func"></typeparam>
        /// <param name="func">value타입을 파라미터로 받는 함수객체</param>
        /// <returns>진행한 작업 수</returns>
        template<typename Func>
        int64_t ConsumeAll(Func&& func)
        {
            int64_t count = 0;
            while (ConsumeOne(func)) count++;

            return count;
        }

        /// <summary>
        /// 지정한 수만 큼 소비하는데 큐가 비었으면 멈춘다.
        /// </summary>
        /// <typeparam name="Func"></typeparam>
        /// <param name="count">지정 카운트</param>
        /// <param name="func"></param>
        /// <returns>
        /// 남은 작업 수.
        /// </returns>
        template<typename Func>
        int64_t Consume(int64_t count, Func&& func)
        {
            int64_t remainCount = 0;
            while (count > 0)
            {
                T value;
                if (false == Pop(value, remainCount))
                    return remainCount;

                func(value);
                count--;
            }

            return remainCount;
        }

        void Clear()
        {
            T node;
            while (GetSize() > 0)
                Pop(node);
        }

        int64_t GetSize() { return _size.load(std::memory_order_acquire); }

    private:
        int64_t InternalPush(std::shared_ptr<Node>& node)
        {
            while (true)
            {
                auto tail = _tail.load(std::memory_order_acquire);
                auto next = _dummy;

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
        // 빈노드용 공유포인터
        std::shared_ptr<Node> _dummy;
    };
}