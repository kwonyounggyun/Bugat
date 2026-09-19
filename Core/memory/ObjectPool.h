#pragma once
#ifdef OBJECTPOOL_H_DEBUG
	#include "Log.h"
#endif

#include <memory>

/*
* T is object type.
* MemPool must have void* Get(), void Release(void*) function. 
*/
template<typename T, typename MemPool>
class ObjectPool
{
public:
	template<typename ...ARGS>
	std::shared_ptr<T> Get(ARGS&&... args)
	{
		auto ptr = _pool.Get();
#ifdef OBJECTPOOL_H_DEBUG
		DebugLog("Get {}", (void*)ptr);
#endif
		auto obj = new (reinterpret_cast<T*>(ptr))T(std::forward<ARGS>(args)...);
		return std::shared_ptr<T>(reinterpret_cast<T*>(obj), [this](T* ptr) {
			Release(ptr);
#ifdef OBJECTPOOL_H_DEBUG
			DebugLog("Release {}", (void*)ptr);
#endif
			});
	}

	void Release(T* obj)
	{
		_pool.Release(obj);
	}

private:
	MemPool _pool;
};