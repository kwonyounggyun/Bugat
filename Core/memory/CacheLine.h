#pragma once
#include <type_traits>

namespace bugat::memory
{
    template<typename T>
    class alignas(std::hardware_destructive_interference_size) CacheLinePadding
    {
    public:
        template<typename... Args>
        explicit CacheLinePadding(Args&&... args) : _value(std::forward<Args>(args)...) {}

        T* operator->() { return &_value; }
        const T* operator->() const { return &_value; }

    private:
        T _value;
    };
}