#pragma once
#ifdef MEMORYPOOL_HPP_DEBUG
	#include "Log.h"
#endif
#include <ctype.h>

#ifndef MEMORYPOOL_HPP
#define MEMORYPOOL_HPP

constexpr int PTR_SIZE = sizeof(uintptr_t*);

class MemoryPoolBase
{
public:
	MemoryPoolBase() {}
	virtual ~MemoryPoolBase() {}
	virtual void* Get() = 0;
	virtual void Release(void* ptr) = 0;
};

template<int SIZE, int COUNT>
class MemoryPool : public MemoryPoolBase
{
private:
	class Chunk
	{
	public:
		Chunk(void* ptr, Chunk* next) : _ptr(ptr), _next(next) {}

		Chunk* _next;
		void* _ptr;
	};

public:
	MemoryPool() : _chunk_list(nullptr), _head(nullptr)
	{

	}

	~MemoryPool()
	{
	}

	virtual void* Get() override
	{
		if (_head == nullptr)
		{
			Alloc();
		}
		auto ret = _head;
		_head = reinterpret_cast<uintptr_t*>(*_head);
#ifdef MEMORYPOOL_HPP_DEBUG
		DebugLog("get - head : {}, out {}", (void*)_head, (void*)ret);
#endif
		return ret;
	}

	virtual void Release(void* ptr) override
	{
		*reinterpret_cast<uintptr_t*>(ptr) = reinterpret_cast<uintptr_t>(_head);
		_head = reinterpret_cast<uintptr_t*>(ptr);
#ifdef MEMORYPOOL_HPP_DEBUG
		DebugLog("release - head : {}, next {}", (void*)_head, (void*)*_head);
#endif
	}

private:
	// when this function is called, _head is nullptr, so we need to allocate a new chunk and set _head to the first block of the new chunk.
	void Alloc()
	{
		Chunk* new_chunk = AllocChunk();
		if (_chunk_list == nullptr)
		{
			_chunk_list = new_chunk;
		}
		else
		{
			Chunk* cur = _chunk_list;
			while (cur->_next != nullptr) cur = cur->_next;
			cur->_next = new_chunk;
		}

		_head = reinterpret_cast<uintptr_t*>(new_chunk->_ptr);
		auto pre_head = _head;

#ifdef MEMORYPOOL_HPP_DEBUG
		DebugLog("alloc - head : {}, next : {}", (void*)_head, (void*)*_head);
#endif

		int count = 1;
		while (count < COUNT)
		{
			_head = reinterpret_cast<uintptr_t*>(reinterpret_cast<char*>(_head) + SIZE);
			*_head = reinterpret_cast<uintptr_t>(pre_head);
			pre_head = _head;
#ifdef MEMORYPOOL_HPP_DEBUG
			DebugLog("alloc - head : {}, next : {}", (void*)_head, (void*)*_head);
#endif
			count++;
		}
	}

	Chunk* AllocChunk()
	{

		auto c = reinterpret_cast<Chunk*>(::malloc(SIZE * COUNT + sizeof(Chunk)));

		::memset(c, 0x00, SIZE * COUNT + sizeof(Chunk));
		c->_next = nullptr;
		c->_ptr = reinterpret_cast<void*>(reinterpret_cast<char*>(c) + sizeof(Chunk));
#ifdef MEMORYPOOL_HPP_DEBUG
		DebugLog("alloc totoal byte: {}", SIZE * COUNT + sizeof(Chunk));
		DebugLog("chunk ptr: {}, mem start ptr {}, end ptr : {}", (void*)c, (void*)c->_ptr, (void*)(reinterpret_cast<char*>(c) + SIZE * COUNT + sizeof(Chunk)));
#endif

		return c;
	}

	Chunk* _chunk_list;
	uintptr_t* _head;
};
#endif
