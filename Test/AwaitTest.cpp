#include "stdafx.h"
#include "TestFunc.h"

namespace bugat
{
	class AwaitTestObject : public SerializeObject
	{
		DECL_ASYNC_FUNC(Add, (int num))
        DECL_ASYNC_FUNC(Complete, (std::function<void(int)> callback))

    public:
        AwaitTestObject() : _count(0) {}

	private:
		int _count;
	};

	DEF_ASYNC_FUNC(AwaitTestObject, Add, (int num))
	{
		_count += num;
		co_return;
	}

    DEF_ASYNC_FUNC(AwaitTestObject, Complete, (std::function<void(int)> callback))
    {
        callback(_count);
        co_return;
    }

    class AwaitTestObjects : public bugat::SerializeObject
    {
    public:
        AwaitTestObjects() : _completeCount(0) {}
        void CreateObjects(int count)
        {
            _objects.reserve(count);
            for (int i = 0; i < count; i++)
            {
                auto obj = CreateSerializeObject<AwaitTestObject>(GetContext());
                _objects.push_back(obj);
            }
        }

        void Run(int executeCount)
        {
            for (int i = 0; i < executeCount; i++)
                for (auto& obj : _objects)
                    obj->Async_Add(1);
        }

        void CheckComplete(int targetCount)
        {
            for (auto& obj : _objects)
            {
                obj->Async_Complete([this, targetCount](int count) {
                    if (targetCount == count)
                        _completeCount.fetch_add(1, std::memory_order_release);
                    });
            }
        }

        void Complete() const
        {
            while (_objects.size() != _completeCount.load(std::memory_order_acquire))
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        std::vector<bugat::TSharedPtr<AwaitTestObject>> _objects;
        std::atomic<int> _completeCount;
    };

	void AwaitTest(Context& context)
	{
		auto startMs = DateTime::NowMs();
        InfoLog("{} start [{}] {}", __FUNCTION__, std::this_thread::get_id(), startMs);
		
		int threadCount = 10;
		int objectCount = 1000;
		int runningCount = 1000;

		std::vector<std::thread> threads;
		auto objects = CreateSerializeObject<AwaitTestObjects>(&context);
        objects->CreateObjects(objectCount);

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