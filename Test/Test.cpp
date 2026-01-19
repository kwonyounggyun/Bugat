// Test.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <iostream>
#include "../Base/Context.h"
#include "../Base/NetworkContext.h"
#include "../Base/SerializeObject.h"
#include "../Base/DateTime.h"
#include "../Base/Log.h"

#include "../Core/ThreadGroup.h"

namespace bugat
{
    class TestObject : public bugat::SerializeObject
    {
    public:
        void AddCount()
        {
            Post([this]() {
                count++;
                });
        }

        void Complete(std::function<void(int c)> comp)
        {
            Post([this, comp]() {
                comp(count);
                });
        }

        int count = 0;
    };

    class TestObjects : public bugat::SerializeObject
    {
    public:
        TestObjects() : _completeCount(0), _complete(false) {}
        void CreateObjects(int count)
        {
            _objects.reserve(count);
            for (int i = 0; i < count; i++)
            {
                auto obj = CreateSerializeObject<TestObject>(GetContext());
                _objects.push_back(obj);
            }
        }

        void Run(int executeCount)
        {
            auto startMs = bugat::DateTime::NowMs();
            InfoLog("start [{}] {}", std::this_thread::get_id(), startMs);
            for (int i = 0; i < executeCount; i++)
                for (auto& obj : _objects)
                    obj->AddCount();

            for (auto& obj : _objects)
                obj->Complete([this, obj, executeCount](int count) {
                if (count == executeCount)
                    Post([this]() {
                    _completeCount++;
                    if (_completeCount == _objects.size())
                        _complete.store(true, std::memory_order_release);
                        });
                    });

            auto endMs = bugat::DateTime::NowMs();
            InfoLog("end [{}] {}   diff {}", std::this_thread::get_id(), endMs, endMs - startMs);
        }

        void Complete() const
        {
            while (false == _complete.load(std::memory_order_acquire));
        }

        std::vector<bugat::TSharedPtr<TestObject>> _objects;
        int _completeCount;
        std::atomic_bool _complete;
    };

    void RunObjectTest(Context& context)
    {
        auto startMs = DateTime::NowMs();
        InfoLog("Thread start [{}] {}", std::this_thread::get_id(), startMs);
        std::vector<std::thread> threads;
        for (int i = 0; i < 5; i++)
            threads.push_back(std::thread([&context]() {
            auto objects = CreateSerializeObject<TestObjects>(&context);
            objects->CreateObjects(5000);
            objects->Run(10000);
            objects->Complete();
                }));

        for (auto& t : threads)
            t.join();

        auto endMs = DateTime::NowMs();
        InfoLog("Thread end [{}] {}   diff {}", std::this_thread::get_id(), endMs, endMs - startMs);
    }
}

int main()
{
    using namespace bugat;

    Context context(10);
    context.Initialize();

    ThreadGroup threads;
    threads.Add(5, [&context](ThreadInfo& info) {
        context.Run();
        });

    RunObjectTest(context);

    context.Stop();
    threads.Stop();
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
