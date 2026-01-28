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

    class __deleter;
    struct __add_ref_count;
    struct __sub_ref_count;
    struct __set_deleter;
    struct __get_deleter;
    class RefCountable
    {
        friend struct __add_ref_count;
        friend struct __sub_ref_count;
        friend struct __set_deleter;
        friend struct __get_deleter;
    protected:
        RefCountable() :_refCount(0), _deleter(nullptr) {}
        virtual ~RefCountable() {}

    private:
        std::atomic<int> _refCount;
        __deleter* _deleter;
    };

    class __deleter
    {
    public:
        __deleter(std::function<void(void*)>&& deleter) : _deleter(deleter) {}
        void operator()(void* ptr)
        {
            _deleter(ptr);
        }
    private:
        std::function<void(void*)> _deleter;
    };

    struct __add_ref_count
    {
        void operator()(RefCountable* refCountObj)
        {
            refCountObj->_refCount.fetch_add(1);
        }
    };

    struct __sub_ref_count
    {
        int operator()(RefCountable* refCountObj)
        {
            return refCountObj->_refCount.fetch_sub(1);
        }
    };

    struct __set_deleter
    {
        void operator()(RefCountable* refCountObj, __deleter* deleter)
        {
            refCountObj->_deleter = deleter;
        }
    };

    struct __get_deleter
    {
        __deleter* operator()(RefCountable* refCountObj)
        {
            return refCountObj->_deleter;
        }
    };

    template<typename T>
    class TSharedPtr
    {
    public:
        TSharedPtr(T* ptr = nullptr) : _ptr(ptr)
        {
            if (_ptr) __add_ref_count()(_ptr);
        }

        TSharedPtr(T* ptr, const std::function<void(T*)>& deleter) : _ptr(ptr)
        {
            if (_ptr)
            {
                __add_ref_count()(_ptr);
                if (deleter)
                {
                    auto deleterPtr = new __deleter([deleter](void* ptr) {
                        T* tptr = static_cast<T*>(ptr);
                        deleter(tptr);
                        });
                    __set_deleter()(_ptr, deleterPtr);
                }
            }
        }

		TSharedPtr(const TSharedPtr& other) : _ptr(other._ptr)
        {
            if (_ptr) __add_ref_count()(_ptr);
        }

        TSharedPtr(TSharedPtr&& other) : _ptr(other._ptr)
        {
            other._ptr = nullptr;
        }

        template<typename U>
        TSharedPtr(const TSharedPtr<U>& other) : _ptr(other.Get())
        {
            if (_ptr) __add_ref_count()(_ptr);
        }

        ~TSharedPtr()
        {
            Reset();
        }

        TSharedPtr& operator=(std::nullptr_t) 
        {
            Reset();

            return *this;
        }

        TSharedPtr& operator=(const TSharedPtr& other)
        {
            if (this == &other)
                return *this;

            if (*this != other)
            {
                Reset();
                _ptr = other._ptr;
                if (_ptr) __add_ref_count()(_ptr);
            }
            return *this;
        }

        TSharedPtr& operator=(TSharedPtr&& other)
        {
            if (this == &other)
                return *this;

            Reset();
            _ptr = other._ptr;
            other._ptr = nullptr;

            return *this;
        }

        bool operator!=(const TSharedPtr& other) const
        {
            return _ptr != other._ptr;
        }

        template<typename U>
		requires std::is_base_of_v<T, U>
        TSharedPtr& operator=(const TSharedPtr<U>& other)
        {
            if (*this != other)
            {
                Reset();
                _ptr = other.Get();
                if (_ptr) _ptr->AddRef();
            }
            return *this;
        }

        template<typename U>
        bool operator!=(const TSharedPtr<U>& other) const
        {
            return _ptr != other.Get();
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
                if (__sub_ref_count()(_ptr) == 1)
                {
                    if (auto deleter = __get_deleter()(_ptr); deleter)
                    {
                        (*deleter)(_ptr);
                        delete deleter;
                    }
                    else
                        delete _ptr;
                }
                _ptr = nullptr;
			}
        }

    private:
        T* _ptr = nullptr;
    };

    template<typename T, typename S>
	TSharedPtr<T> StaticPointerCast(const TSharedPtr<S>& sp)
    {
        return TSharedPtr<T>(static_cast<T*>(sp.Get()));
    }
}