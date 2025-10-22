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

	auto k2 = bugat::ObjectPoolFactory::Create<int, 10>();

	{
		auto testObj1 = k2->Get();
		auto testObj2 = k2->Get();
		auto testObj3 = k2->Get();
		auto testObj4 = k2->Get();
	}

	bugat::core::TaskSerializer serial;
	std::function<void()> func = []() {};
	serial.Post(func);
	serial.Post([]() {});

	serial.Post([](int a)->int {
		return 1;
		}, 1);

	auto s = std::string("test");
	serial.Post([](std::string a) {}, std::string("test"));
	serial.Post([](std::string a) {}, s);
	serial.Run();
	//std::make_unique<TaskModel<std::function<void()>, void>>(func);
	//serial.Post(std::bind(add, 1));
}
