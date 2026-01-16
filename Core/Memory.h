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

        void SetDeleter(std::function<void(void*)>&& deleter)
        {
            _deleter = std::move(deleter);
		}

        std::function<void(void*)>& GetDeleter() { return _deleter; }

    protected:
        RefCountable() :_refCount(0), _deleter(nullptr) {}
        virtual ~RefCountable() {}

    protected:
        std::atomic<int> _refCount;
        std::function<void(void*)> _deleter;
    };

    template<typename T>
    class TSharedPtr
    {
    public:
        TSharedPtr(T* ptr = nullptr) : _ptr(ptr)
        {
            if (_ptr) _ptr->AddRef();
        }

        TSharedPtr(T* ptr, const std::function<void(T*)>& deleter) : _ptr(ptr)
        {
            if (_ptr) _ptr->AddRef();
            if(deleter)
                _ptr->SetDeleter([deleter](void* ptr) {
				    T* tptr = static_cast<T*>(ptr);
				    deleter(tptr);
                    });
        }

		TSharedPtr(const TSharedPtr& other) : _ptr(other._ptr)
        {
            if (_ptr) _ptr->AddRef();
        }

        TSharedPtr(TSharedPtr&& other) : _ptr(other._ptr)
        {
            other._ptr = nullptr;
        }

        template<typename U>
        TSharedPtr(const TSharedPtr<U>& other) : _ptr(other.Get())
        {
            if (_ptr) _ptr->AddRef();
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
                if (_ptr) _ptr->AddRef();
            }
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
                if (_ptr->SubRef() == 1)
                {
					if (auto deleter = _ptr->GetDeleter(); deleter)
                        deleter(_ptr);
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