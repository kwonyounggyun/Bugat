#include "stdafx.h"
#include "TestFunc.h"

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
        TestObjects() : _completeCount(0) {}
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
            for (int i = 0; i < executeCount; i++)
                for (auto& obj : _objects)
                    obj->AddCount();
        }

        void CheckComplete(int targetCount)
        {
            for (auto& obj : _objects)
            {
                obj->Complete([this, targetCount](int count) {
                    if(targetCount == count)
                        _completeCount.fetch_add(1, std::memory_order_release);
                    });
            }
		}

        void Complete() const
        {
            while (_objects.size() != _completeCount.load(std::memory_order_acquire))
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        std::vector<bugat::TSharedPtr<TestObject>> _objects;
        std::atomic<int> _completeCount;
    };

    void RunObjectTest(Context& context)
    {
        int threadCount = 10;
        int objectCount = 10000;
        int runningCount = 1000;

        std::vector<std::thread> threads;
        auto objects = CreateSerializeObject<TestObjects>(&context);
        objects->CreateObjects(objectCount);

        auto startMs = DateTime::NowMs();
        InfoLog("{} start [{}] {}", __FUNCTION__, std::this_thread::get_id(), startMs);

        for (int i = 0; i < threadCount; i++)
            threads.push_back(std::thread([&]() {
            objects->Run(runningCount);
                }));

        for (auto& t : threads)
            t.join();

        objects->CheckComplete(threadCount * runningCount);
        objects->Complete();

        auto endMs = DateTime::NowMs();
        InfoLog("{} end [{}] {} diff {}", __FUNCTION__, std::this_thread::get_id(), endMs, endMs - startMs);
    }
}
