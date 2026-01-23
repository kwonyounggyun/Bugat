#pragma once
#include <type_traits>
#include "../Core/AwaitTask.h"

#define DECL_ASYNC_FUNC(FuntionName, ReturnType, Params) \
public: \
	/*  Async_##FuntionName##Params */ \
	template<typename ...Args> \
	void Async_##FuntionName(Args&&... args) \
	{ \
		Post(##FuntionName(std::forward<Args>(args)...)); \
	} \
	template<typename ...Args> \
	auto Await_##FuntionName(TSharedPtr<SerializeObject> obj, Args&&... args) \
	{ \
		return AwaitPost(obj, this, ##FuntionName(std::forward<Args>(args)...)); \
	} \
private: \
	AwaitTask<ReturnType> FuntionName##Params;

#define DEF_ASYNC_FUNC(ClassName, FunctionName, ReturnType, Params) \
	AwaitTask<ReturnType> ClassName::##FunctionName##Params