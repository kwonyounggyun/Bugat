// Core.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "framework.h"
#include "RWLockObject.h"
#include <map>
#include "ObjectPool.h"
#include "TaskSerializer.h"

// TODO: This is an example of a library function

int add(int i)
{
	return i;
}

void fnCore()
{

	bugat::RWLockObject<std::map<int, int>> obj;
	auto lock = obj.LockRead();

	bugat::ObjectPool<int, 10> k2;

	{
		auto testObj1 = k2.GetObj();
		auto testObj2 = k2.GetObj();
		auto testObj3 = k2.GetObj();
		auto testObj4 = k2.GetObj();
	}

	bugat::core::TaskSerializer serial;
	std::function<void()> func = []() {};
	serial.Post(func);
	serial.Post([]() {});

	serial.Post([]()->int {
		return 1;
		}, [](int result) {});
	serial.Run();
	//std::make_unique<TaskModel<std::function<void()>, void>>(func);
	//serial.Post(std::bind(add, 1));
}
