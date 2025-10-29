#pragma once
#include "Context.h"
#include "SerializeObject.h"
#include <type_traits>

namespace bugat
{
	template<typename T, typename ...ARGS>
	requires std::is_base_of_v<SerializeObject, T>
	std::shared_ptr<T> CreateSerializeObject(Context& context, ARGS&... args)
	{
		auto sPtr = std::make_shared<T>(args...);
		sPtr->SetContext(&context);
		return sPtr;
	}
}