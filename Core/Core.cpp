// Core.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "framework.h"
#include "RWLockObject.h"
#include <map>
#include "ObjectPool.h"

using namespace bugat;
// TODO: This is an example of a library function
void fnCore()
{

	RWLockObject<std::map<int, int>> obj;
	auto lock = obj.LockRead();

	ObjectPool<int, false, 10> k1;
	ObjectPool<int, true, 10> k2;
}
