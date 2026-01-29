#include "stdafx.h"
#include "TestFunc.h"

namespace bugat
{
    class ObjectPoolTestObject : public SerializeObject
    {
    public:
        ObjectPoolTestObject(int& create, int& destory) : _create(create), _destory(destory)
        {
            _create++;
        }
        ~ObjectPoolTestObject()
        {
            _destory++;
        }

    private:
        int& _create;
        int& _destory;
    };

    void ObjectPoolTest(Context& context)
    {
        int threadCount = 10;
        int runningCount = 100000;

        std::vector<std::thread> threads;

        auto startMs = DateTime::NowMs();
        InfoLog("{} start [{}] {}", __FUNCTION__, std::this_thread::get_id(), startMs);

        std::atomic<int64_t> totalCreate;
        std::atomic<int64_t> totalDestory;
        for (int i = 0; i < threadCount; i++)
            threads.push_back(std::thread([&]() {
                int create = 0;
                int destory = 0;
                for (int j = 0; j < runningCount; j++)
                {
                    auto object = CreateSerializeObject<ObjectPoolTestObject>(&context, create, destory);
                }

                totalCreate.fetch_add(create);
                totalDestory.fetch_add(destory);
                }));

        for (auto& t : threads)
            t.join();

        int64_t target = threadCount * runningCount;
        while (true)
        {
            if (totalCreate.load() == target && totalDestory.load() == target)
                break;
        }

        auto endMs = DateTime::NowMs();
        InfoLog("{} end [{}] {} diff {}", __FUNCTION__, std::this_thread::get_id(), endMs, endMs - startMs);
    }
}