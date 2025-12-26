#pragma once
#include <functional>
#include <vector>
#include <utility>

namespace bugat
{
    template<typename... Args>
    class Event
    {
    public:
        using Handler = std::function<void(Args...)>;
        int Subscribe(Handler h)
        {
            const int id = ++_nextId;
            _handlers.emplace_back(id, std::move(h));
            return id;
        }

        void Unsubscribe(int id)
        {
            for (auto it = _handlers.begin(); it != _handlers.end(); ++it)
            {
                if (it->first == id)
                {
                    _handlers.erase(it);
                    return;
                }
            }
        }

        void Invoke(Args... args)
        {
            for (auto& [id, handler] : _handlers)
            {
                handler(args...);
            }
        }

        void operator+=(Handler handle)
        {
            Subscribe(handle);
        }

        void operator=(Handler handle) = delete;

        // C#처럼 operator()로도 부를 수 있게
        void operator()(Args... args) { Invoke(std::forward<Args>(args)...); }

    private:
        int _nextId = 0;
        std::vector<std::pair<int, Handler>> _handlers;
    };
}
