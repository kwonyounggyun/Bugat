#pragma once
#include <exception>
#include <stacktrace>

struct CoroutineException
{
	CoroutineException(std::exception& exception) : _exception(exception), _trace(std::stacktrace::current()){}
	std::exception _exception;
	std::stacktrace _trace;
};