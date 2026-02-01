#pragma once
#include <type_traits>
#include "AwaitTask.h"

// 비동기 멤버함수 생성용(Params에 참조 허용)
#define DECL_ASYNC_FUNC(FuntionName, ReturnType, Params) \
public: \
	/*  Async_##FuntionName##Params */ \
	template<typename ...Args> \
	void Async_##FuntionName(Args&&... args) \
	{ \
		Post([this, ...copyArgs = std::forward<Args>(args)]() mutable { \
			FuntionName(copyArgs...); \
		}); \
	} \
private: \
	ReturnType FuntionName##Params;

#define DEF_ASYNC_FUNC(ClassName, FunctionName, ReturnType, Params) \
	ReturnType ClassName::##FunctionName##Params


// 코루틴 멤버함수 생성용
#define DECL_COROUTINE_FUNC(ClassName, FuntionName, ReturnType, Params) \
	/*  Async_##FuntionName##Params */ \
public: \
	template<typename ...Args> \
	void Spawn_##FuntionName(Args&&... args) \
	{ \
		auto task = __##FuntionName(std::forward<Args>(args)...); \
		Post([task](){	\
			task.resume();	\
		}); \
	} \
	template<typename ...Args> \
	auto Await_##FuntionName(Args&&... args) \
	{ \
		return __##FuntionName(std::forward<Args>(args)...); \
	} \
private: \
	AwaitTask<ReturnType, ClassName> __##FuntionName##Params;

#define DEF_COROUTINE_FUNC(ClassName, FunctionName, ReturnType, Params) \
	AwaitTask<ReturnType, ClassName> ClassName::__##FunctionName##Params