#pragma once
#include <atomic>
#include <new>
#include <functional>

namespace bugat
{
    template<typename T>
    class alignas(std::hardware_destructive_interference_size) CacheLinePadding
    {
    public:
        T* operator->() { return &_value; }

    private:
        T _value;
    };

    class RefCountable
    {
    public:
        void AddRef()
        {
            _refCount.fetch_add(1);
        }
        
        int SubRef()
        {
            return _refCount.fetch_sub(1);
        }

    protected:
        RefCountable() :_refCount(0) {}
        virtual ~RefCountable() {}

    protected:
        std::atomic<int> _refCount;
    };

    template<typename T>
    class TSharedPtr
    {
    public:
        TSharedPtr(T* ptr) : _ptr(ptr)
        {
            if (_ptr) _ptr->AddRef();
        }

        TSharedPtr(T* ptr, std::function<void (T*)> deletor) : _ptr(ptr), _deletor(deletor)
        {
            if (_ptr) _ptr->AddRef();
        }

        ~TSharedPtr()
        {
            Reset();
        }

        TSharedPtr(const TSharedPtr& other) : _ptr(other._ptr), _deletor(other._deletor)
        {
            if (_ptr) _ptr->AddRef();
        }

        TSharedPtr(TSharedPtr&& other) : _ptr(other._ptr), _deletor(other._deletor)
        {
            other._ptr = nullptr;
            other._deletor = nullptr;
        }

        TSharedPtr& operator=(const TSharedPtr& other)
        {
            if (this != &other)
            {
                Reset();
                _ptr = other._ptr;
                _deletor = other._deletor;
                if (_ptr) _ptr->AddRef();
            }
            return *this;
        }

        explicit operator bool() const {
            return _ptr != nullptr;
        }

        bool operator!() const {
            return _ptr == nullptr;
        }

        T* operator->() const { return _ptr; }
        T& operator*() const { return *_ptr; }
        T* Get() const { return _ptr; }
        void Reset()
        {
            if (_ptr)
            {
                if (_ptr->SubRef() == 1)
                {
                    if (_deletor)
                        _deletor(_ptr);
                    else
                        delete _ptr;
                }
                _ptr = nullptr;
                _deletor = nullptr;
			}
        }

    private:
        T* _ptr = nullptr;
        std::function<void(T*)> _deletor;
    };

    template<typename T, typename S>
	TSharedPtr<T> StaticPointerCast(const TSharedPtr<S>& sp)
    {
        return TSharedPtr<T>(static_cast<T*>(sp.Get()));
    }
}