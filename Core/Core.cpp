// Core.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "framework.h"
#include "lock/RWLockObject.h"
#include <map>
#include "memory/ObjectPool.h"
#include "memory/MemoryPool.h"
#include "TaskSerializer.h"
#include "ThreadGroup.h"

// TODO: This is an example of a library function

int add(int i)
{
	return i;
}

class TestSerializer : public bugat::TaskSerializer
{
};

void fnCore()
{

	bugat::RWLockObject<std::map<int, int>> obj;
	auto lock = obj.LockRead();

	ObjectPool<TestSerializer, TLSMemoryPool<TestSerializer>> k2;
	{
		auto testObj1 = k2.Get();
		auto testObj2 = k2.Get();
		auto testObj3 = k2.Get();
		auto testObj4 = k2.Get();
	}

	bugat::TaskSerializer serial;
	std::function<void()> func = []() {};
	serial.Post(func);
	serial.Post([]() {});
	auto test = std::make_shared<TestSerializer>();

	serial.Post([](int a)->int {
		return 1;
		}, 1);

	auto s = std::string("test");
	serial.Post([](std::string a) {}, std::string("test"));
	serial.Post([](std::string a) {}, s);
	serial.Run();

	bugat::ThreadGroup t;
	t.Add(5, [](bugat::ThreadInfo& threadInfo, std::string& s) {}, s);


	//std::make_unique<TaskModel<std::function<void()>, void>>(func);
	//serial.Post(std::bind(add, 1));
}
