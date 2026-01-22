#pragma once
#include <type_traits>
#include "../Core/AwaitTask.h"

#define DECL_ASYNC_FUNC(FuntionName, Params) \
public: \
	/*  Async_##FuntionName##Params */ \
	template<typename ...Args> \
	void Async_##FuntionName(Args&&... args) \
	{ \
		Post(##FuntionName##_Impl(std::forward<Args>(args)...)); \
	} \
private: \
	AwaitTask<void> FuntionName##_Impl##Params;

#define DEF_ASYNC_FUNC(ClassName, FunctionName, Params) \
	AwaitTask<void> ClassName::##FunctionName##_Impl##Params