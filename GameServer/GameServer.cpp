// GameServer.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//
#include "stdafx.h"
#include <iostream>

#include "../Core/ObjectPool.h"

#include "../Base/Context.h"
#include "../Base/NetworkContext.h"
#include "../Base/SerializeObject.h"

#include "WorldServer.h"
#include "GameHandler.h"

#include "../Core/ThreadGroup.h"

#include "../Base/Configure.h"

#include "ContextHolder.h"
#include "GameConnection.h"

#include "../Base/RedisClient.h"
#include "../Base/RedisCommand.h"

int main()
{
    using namespace bugat;

    ThreadGroup threads;
    LogicContext.Initialize(10);
    threads.Add(5, [](ThreadInfo& info) {
        LogicContext.Run();
        });

    threads.Add(5, [](ThreadInfo& info) {
        NetClientContext.Run();
        });

 /*   threads.Add(1, [](ThreadInfo& info) {
        NetServerContext.RunOne();
        });*/

    WorldInstance.SetContext(&LogicContext);

    GameHandler::Instance().Init();
    WorldInstance.Initialize();
    Configure config;
    config.port = 9000;
    config.acceptTaskCount = 10;
    WorldInstance.Accept(NetClientContext.GetExecutor(), ConnectionFactory<GameConnection>(LogicContext), config);
    
    RedisClient redis(NetClientContext.GetExecutor());
    redis.Connect("127.0.0.1", 6379);

    auto redisT = std::thread([&]() {
        while (true)
        {
            auto pipe = CreateRedisPipeline(Set("test", 1), Set("test", 2), Get<int>("test"));

            redis.Execute(pipe, [](RedisErrorCode error, auto& result) {
                if (error == RedisErrorCode::Fail)
                {
                    ErrorLog("Redis Fail");
                    return;
                }

                auto& [setResult, setResult2, getResult] = result;
                });
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        });

    std::this_thread::sleep_for(std::chrono::seconds(10));
    //redis.Stop();
    {
       /* auto pipe = CreateRedisPipeline(Set("test", "teststs"), Get<int>("test"));
        redis.Execute(pipe, [](RedisError error, auto& result) {
            if (error)
                ErrorLog("Redis Error : {}", error.message());
            auto& [setResult, getResult] = result;
            });*/
    }

    threads.Join();
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
