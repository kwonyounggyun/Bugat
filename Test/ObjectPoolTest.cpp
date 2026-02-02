#include "stdafx.h"
#include "TestFunc.h"
#include <array>

namespace bugat
{
    class ObjectPoolTestObject : public SerializeObject
    {
    public:
        ObjectPoolTestObject(std::atomic<int>& create, std::atomic<int>& destory) : _create(create), _destory(destory)
        {
            _create++;
        }
        virtual ~ObjectPoolTestObject()
        {
            _destory++;
        }

        DECL_ASYNC_FUNC(Test, void, ());

    private:
        std::atomic<int>& _create;
        std::atomic<int>& _destory;
        int _count;
    };

    DEF_ASYNC_FUNC(ObjectPoolTestObject, Test, void, ())
    {
        _count++;
    }

    void ObjectPoolTest(Context& context)
    {
        constexpr int threadCount = 10;
        int runningCount = 100000;

        std::vector<std::thread> threads;

        auto startMs = DateTime::NowMs();
        InfoLog("{} start [{}] {}", __FUNCTION__, std::this_thread::get_id(), startMs);

        ObjectPool<ObjectPoolTestObject, 5> pool;

        std::array<std::atomic<int>, threadCount> create(0);
        std::array<std::atomic<int>, threadCount> destory(0);
        for (int i = 0; i < threadCount; i++)
            threads.push_back(std::thread([&pool, &context, &create, &destory, runningCount, i]() {
                for (int j = 0; j < runningCount; j++)
                {
                    auto object = pool.Get(create[i], destory[i]);
                    object->SetContext(&context);
                    object->Async_Test();
                }
                }));

        for (auto& t : threads)
            t.join();

        int64_t target = threadCount * runningCount;
        while (true)
        {
            auto createCount = 0;
            auto destroyCount = 0;
            for (auto& c : create)
                createCount += c.load();

            for (auto& d : destory)
                destroyCount += d.load();

            if (createCount == target && destroyCount == target)
                break;
        }

        auto endMs = DateTime::NowMs();
        InfoLog("{} end [{}] {} diff {}", __FUNCTION__, std::this_thread::get_id(), endMs, endMs - startMs);
    }
}