// GameServer.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//
#include "stdafx.h"
#include <iostream>

#include "Context.h"
#include <thread>
#include "../Core/ObjectPool.h"

#include "../Core/TaskSerializer.h"

#include <boost/lockfree/queue.hpp>

using namespace bugat;

struct Test : public SerializeObject<Test>
{
    int value[1000];
};

int main()
{

    boost::lockfree::queue<bugat::core::AnyTask*> queue(10);
    boost::lockfree::queue<bugat::core::AnyTask*> queue1(100);
    boost::lockfree::queue<bugat::core::AnyTask*> queue2(1000);

    Context context;

    context.Initialize(10);

    std::vector<std::thread*> threads;

    auto tsize = sizeof(bugat::TaskSerializer);
    auto asize = sizeof(bugat::core::AnyTask);
    auto pool = ObjectPoolFactory::Create<Test, 100000>();
    std::vector<std::shared_ptr<Test>> objects(100000);
    objects[0] = pool->Get();
    for (int j = 1; j < 100000; j++)
    {
        objects[j] = std::move(pool->Get());
        objects[j]->SetContext(&context);
    }

    for(int i = 0; i < 10; i++)
    {
        threads.push_back(new std::thread([&context]() {
            context.run();
            }));
	}

    
    std::vector<std::thread*> testThreads;
    for (int i = 0; i < 10; i++)
    {
        testThreads.push_back(new std::thread([&context, &objects, i]() {
            auto id = std::this_thread::get_id();
            for (int j = 0; j < 1000; j++)
            {
                for (int k = i * 10000; k < i * 10000 + 10000; k++)
                {
                    objects[j]->Post([id, i]() {
						//std::cout << "Thread ID: " << id << ", Object Index: " << i << std::endl;
                        auto k = i + 100;

                        });
                }
            }

            while (true);
            }));
    }

    for(auto thread : testThreads)
    {
        thread->join();
        delete thread;
	}

    for (auto thread : threads)
    {
        thread->join();
        delete thread;
	}

    std::cout << "Hello World!\n";
}

// 프로그램 실행: <Ctrl+F5> 또는 [디버그] > [디버깅하지 않고 시작] 메뉴
// 프로그램 디버그: <F5> 키 또는 [디버그] > [디버깅 시작] 메뉴

// 시작을 위한 팁: 
//   1. [솔루션 탐색기] 창을 사용하여 파일을 추가/관리합니다.
//   2. [팀 탐색기] 창을 사용하여 소스 제어에 연결합니다.
//   3. [출력] 창을 사용하여 빌드 출력 및 기타 메시지를 확인합니다.
//   4. [오류 목록] 창을 사용하여 오류를 봅니다.
//   5. [프로젝트] > [새 항목 추가]로 이동하여 새 코드 파일을 만들거나, [프로젝트] > [기존 항목 추가]로 이동하여 기존 코드 파일을 프로젝트에 추가합니다.
//   6. 나중에 이 프로젝트를 다시 열려면 [파일] > [열기] > [프로젝트]로 이동하고 .sln 파일을 선택합니다.
