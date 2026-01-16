#pragma once
#include <atomic>
#include "NonCopyable.h"

namespace bugat
{
    class SpinLock;
    class ScopedLock;
    class LockObject : public NonCopyable
    {
        friend class SpinLock;
        friend class ScopedLock;
    private:
        bool TryLock()
        {
            return !_flag.test_and_set(std::memory_order_acquire);
        }
        void Unlock()
        {
            _flag.clear(std::memory_order_release);
        }

    private:
        std::atomic_flag _flag;
    };

    class SpinLock
    {
    public:
        SpinLock(LockObject obj) : _obj(obj)
        {
            while (false == _obj.TryLock());
        }

        ~SpinLock()
        {
            _obj.Unlock();
        }

    private:
        LockObject& _obj;
    };

    class ScopedLock
    {
    public:
        explicit ScopedLock(LockObject& obj) : _obj(obj)
        {
            result = _obj.TryLock();
        }

        ~ScopedLock()
        {
            if (result)
                _obj.Unlock();
        }

        explicit operator bool() const {
            return result;
        }

    private:
        LockObject& _obj;
        bool result;
    };
}