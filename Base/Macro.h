#pragma once
#include <type_traits>
#include "AwaitTask.h"

#define GET_MACRO(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, PASER_NAME, ...) PASER_NAME
#define EXPAND(x) x
#define PARAM_PASER_PROXY(...) EXPAND(GET_MACRO(__VA_ARGS__))

// 변환 매크로 (type, name) -> type& name
#define REF_PARSE_PAIR(type, name) type& name

// 개수별 전개
#define REF_PARSE0()
#define REF_PARSE1(t1, n1) REF_PARSE_PAIR(t1, n1)
#define REF_PARSE2(t1, n1, t2, n2) REF_PARSE1(t1, n1), REF_PARSE1(t2, n2)
#define REF_PARSE3(t1, n1, t2, n2, t3, n3) REF_PARSE1(t1, n1), REF_PARSE2(t2, n2, t3, n3)
#define REF_PARSE4(t1, n1, t2, n2, t3, n3, t4, n4) REF_PARSE1(t1, n1), REF_PARSE3(t2, n2, t3, n3, t4, n4)
#define REF_PARSE5(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5) REF_PARSE1(t1, n1), REF_PARSE4(t2, n2, t3, n3, t4, n4, t5, n5)
#define REF_PARSE6(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6) REF_PARSE1(t1, n1), REF_PARSE5(t2, n2, t3, n3, t4, n4, t5, n5, t6, n6)
#define REF_PARSE7(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7) REF_PARSE1(t1, n1), REF_PARSE6(t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7)
#define REF_PARSE8(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8) REF_PARSE1(t1, n1), REF_PARSE7(t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8)
#define REF_PARSE9(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8, t9, n9) REF_PARSE1(t1, n1), REF_PARSE8(t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8, t9, n9)
#define REF_PARSE10(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8, t9, n9, t10, n10) REF_PARSE1(t1, n1), REF_PARSE9(t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8, t9, n9, t10, n10)

#define REF_PARAM_PASER(...) EXPAND(PARAM_PASER_PROXY(__VA_ARGS__, \
    REF_PARSE10, ERROR10, \
    REF_PARSE9,  ERROR9,  \
    REF_PARSE8,  ERROR8,  \
    REF_PARSE7,  ERROR7,  \
    REF_PARSE6,  ERROR6,  \
    REF_PARSE5,  ERROR5,  \
    REF_PARSE4,  ERROR4,  \
    REF_PARSE3,  ERROR3,  \
    REF_PARSE2,  ERROR2,  \
    REF_PARSE1,  REF_PARSE0) (__VA_ARGS__))

// 변환 매크로 (type, name) -> type name
#define COPY_PARSE_PAIR(type, name) type name

// 개수별 전개
#define COPY_PARSE0()
#define COPY_PARSE1(t1, n1) COPY_PARSE_PAIR(t1, n1)
#define COPY_PARSE2(t1, n1, t2, n2) COPY_PARSE1(t1, n1), COPY_PARSE1(t2, n2)
#define COPY_PARSE3(t1, n1, t2, n2, t3, n3) COPY_PARSE1(t1, n1), COPY_PARSE2(t2, n2, t3, n3)
#define COPY_PARSE4(t1, n1, t2, n2, t3, n3, t4, n4) COPY_PARSE1(t1, n1), COPY_PARSE3(t2, n2, t3, n3, t4, n4)
#define COPY_PARSE5(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5) COPY_PARSE1(t1, n1), COPY_PARSE4(t2, n2, t3, n3, t4, n4, t5, n5)
#define COPY_PARSE6(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6) COPY_PARSE1(t1, n1), COPY_PARSE5(t2, n2, t3, n3, t4, n4, t5, n5, t6, n6)
#define COPY_PARSE7(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7) COPY_PARSE1(t1, n1), COPY_PARSE6(t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7)
#define COPY_PARSE8(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8) COPY_PARSE1(t1, n1), COPY_PARSE7(t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8)
#define COPY_PARSE9(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8, t9, n9) COPY_PARSE1(t1, n1), COPY_PARSE8(t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8, t9, n9)
#define COPY_PARSE10(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8, t9, n9, t10, n10) COPY_PARSE1(t1, n1), COPY_PARSE9(t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8, t9, n9, t10, n10)

#define COPY_PARAM_PASER(...) EXPAND(PARAM_PASER_PROXY(__VA_ARGS__, \
    COPY_PARSE10, ERROR10, \
    COPY_PARSE9,  ERROR9,  \
    COPY_PARSE8,  ERROR8,  \
    COPY_PARSE7,  ERROR7,  \
    COPY_PARSE6,  ERROR6,  \
    COPY_PARSE5,  ERROR5,  \
    COPY_PARSE4,  ERROR4,  \
    COPY_PARSE3,  ERROR3,  \
    COPY_PARSE2,  ERROR2,  \
    COPY_PARSE1,  COPY_PARSE0) (__VA_ARGS__))

// 변환 매크로 (type, name) -> name
#define VALUE_PARSE_PAIR(type, name) name

// 개수별 전개
#define VALUE_PARSE0()
#define VALUE_PARSE1(t1, n1) VALUE_PARSE_PAIR(t1, n1)
#define VALUE_PARSE2(t1, n1, t2, n2) VALUE_PARSE1(t1, n1), VALUE_PARSE1(t2, n2)
#define VALUE_PARSE3(t1, n1, t2, n2, t3, n3) VALUE_PARSE1(t1, n1), VALUE_PARSE2(t2, n2, t3, n3)
#define VALUE_PARSE4(t1, n1, t2, n2, t3, n3, t4, n4) VALUE_PARSE1(t1, n1), VALUE_PARSE3(t2, n2, t3, n3, t4, n4)
#define VALUE_PARSE5(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5) VALUE_PARSE1(t1, n1), VALUE_PARSE4(t2, n2, t3, n3, t4, n4, t5, n5)
#define VALUE_PARSE6(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6) VALUE_PARSE1(t1, n1), VALUE_PARSE5(t2, n2, t3, n3, t4, n4, t5, n5, t6, n6)
#define VALUE_PARSE7(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7) VALUE_PARSE1(t1, n1), VALUE_PARSE6(t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7)
#define VALUE_PARSE8(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8) VALUE_PARSE1(t1, n1), VALUE_PARSE7(t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8)
#define VALUE_PARSE9(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8, t9, n9) VALUE_PARSE1(t1, n1), VALUE_PARSE8(t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8, t9, n9)
#define VALUE_PARSE10(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8, t9, n9, t10, n10) VALUE_PARSE1(t1, n1), VALUE_PARSE9(t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8, t9, n9, t10, n10)

#define VALUE_PARAM_PASER(...) EXPAND(PARAM_PASER_PROXY(__VA_ARGS__, \
    VALUE_PARSE10, ERROR10, \
    VALUE_PARSE9,  ERROR9,  \
    VALUE_PARSE8,  ERROR8,  \
    VALUE_PARSE7,  ERROR7,  \
    VALUE_PARSE6,  ERROR6,  \
    VALUE_PARSE5,  ERROR5,  \
    VALUE_PARSE4,  ERROR4,  \
    VALUE_PARSE3,  ERROR3,  \
    VALUE_PARSE2,  ERROR2,  \
    VALUE_PARSE1,  VALUE_PARSE0) (__VA_ARGS__))

// 변환 매크로 (type, name) -> const type& name
#define CONST_REF_PARSE_PAIR(type, name) const type& name

// 개수별 전개
#define CONST_REF_PARSE0()
#define CONST_REF_PARSE1(t1, n1) CONST_REF_PARSE_PAIR(t1, n1)
#define CONST_REF_PARSE2(t1, n1, t2, n2) CONST_REF_PARSE1(t1, n1), CONST_REF_PARSE1(t2, n2)
#define CONST_REF_PARSE3(t1, n1, t2, n2, t3, n3) CONST_REF_PARSE1(t1, n1), CONST_REF_PARSE2(t2, n2, t3, n3)
#define CONST_REF_PARSE4(t1, n1, t2, n2, t3, n3, t4, n4) CONST_REF_PARSE1(t1, n1), CONST_REF_PARSE3(t2, n2, t3, n3, t4, n4)
#define CONST_REF_PARSE5(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5) CONST_REF_PARSE1(t1, n1), CONST_REF_PARSE4(t2, n2, t3, n3, t4, n4, t5, n5)
#define CONST_REF_PARSE6(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6) CONST_REF_PARSE1(t1, n1), CONST_REF_PARSE5(t2, n2, t3, n3, t4, n4, t5, n5, t6, n6)
#define CONST_REF_PARSE7(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7) CONST_REF_PARSE1(t1, n1), CONST_REF_PARSE6(t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7)
#define CONST_REF_PARSE8(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8) CONST_REF_PARSE1(t1, n1), CONST_REF_PARSE7(t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8)
#define CONST_REF_PARSE9(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8, t9, n9) CONST_REF_PARSE1(t1, n1), CONST_REF_PARSE8(t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8, t9, n9)
#define CONST_REF_PARSE10(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8, t9, n9, t10, n10) CONST_REF_PARSE1(t1, n1), CONST_REF_PARSE9(t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8, t9, n9, t10, n10)

#define CONST_REF_PARAM_PASER(...) EXPAND(PARAM_PASER_PROXY(__VA_ARGS__, \
    CONST_REF_PARSE10, ERROR10, \
    CONST_REF_PARSE9,  ERROR9,  \
    CONST_REF_PARSE8,  ERROR8,  \
    CONST_REF_PARSE7,  ERROR7,  \
    CONST_REF_PARSE6,  ERROR6,  \
    CONST_REF_PARSE5,  ERROR5,  \
    CONST_REF_PARSE4,  ERROR4,  \
    CONST_REF_PARSE3,  ERROR3,  \
    CONST_REF_PARSE2,  ERROR2,  \
    CONST_REF_PARSE1,  CONST_REF_PARSE0) (__VA_ARGS__))

// 비동기 멤버함수 생성용(Params에 참조 허용)
#define _DECL_ASYNC_FUNC(FunctionName, ReturnType, RefParam, ...) \
public: \
	template<typename ...Args> \
	void Async_##FunctionName(Args&&... args) \
	{ \
		Post([this, ...copyArgs = std::forward<Args>(args)]() mutable { \
			FunctionName(copyArgs...); \
		}); \
	} \
private: \
	ReturnType FunctionName##RefParam;

#define DECL_ASYNC_FUNC(FunctionName, ReturnType, Params) \
    _DECL_ASYNC_FUNC(FunctionName, ReturnType, (REF_PARAM_PASER##Params), VALUE_PARAM_PASER##Params)

#define _DEF_ASYNC_FUNC(ClassName, FunctionName, ReturnType, Params) \
	ReturnType ClassName::##FunctionName##Params

#define DEF_ASYNC_FUNC(ClassName, FunctionName, ReturnType, Params) \
    _DEF_ASYNC_FUNC(ClassName, FunctionName, ReturnType, (REF_PARAM_PASER##Params))

// 코루틴 멤버함수 생성용
#define _DECL_COROUTINE_FUNC(ClassName, FunctionName, ReturnType, Params) \
public: \
	template<typename ...Args> \
	void Spawn_##FunctionName(Args&&... args) \
	{ \
		auto task = FunctionName(args...); \
		Post([task](){	\
			task.resume();	\
		}); \
	} \
	AwaitTask<ReturnType, ClassName> FunctionName##Params;

#define DECL_COROUTINE_FUNC(ClassName, FunctionName, ReturnType, Params) \
    _DECL_COROUTINE_FUNC(ClassName, FunctionName, ReturnType, (COPY_PARAM_PASER##Params))

#define _DEF_COROUTINE_FUNC(ClassName, FunctionName, ReturnType, Params) \
	AwaitTask<ReturnType, ClassName> ClassName::##FunctionName##Params

#define DEF_COROUTINE_FUNC(ClassName, FunctionName, ReturnType, Params) \
    _DEF_COROUTINE_FUNC(ClassName, FunctionName, ReturnType, (COPY_PARAM_PASER##Params))