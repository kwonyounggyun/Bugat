#include "stdafx.h"
#include "TestFunc.h"

namespace bugat
{
	class AwaitTestObject : public SerializeObject
	{
        DECL_COROUTINE_FUNC(Add, void, (int num))
        DECL_COROUTINE_FUNC(Complete, void, (std::function<void(int)> callback))
        DECL_COROUTINE_FUNC(GetCount, int, ())

    public:
        AwaitTestObject() : _count(0) {}

	private:
		int _count;
	};

    DEF_COROUTINE_FUNC(AwaitTestObject, Add, void, (int num))
	{
        _count += num;
		co_return;
	}

    DEF_COROUTINE_FUNC(AwaitTestObject, GetCount, int, ())
    {
        auto id = GetObjectId();
        co_return _count;
    }

    DEF_COROUTINE_FUNC(AwaitTestObject, Complete, void, (std::function<void(int)> callback))
    {
        callback(_count);
        co_return;
    }

    class AwaitTestObjects : public bugat::SerializeObject
    {
    public:
        AwaitTestObjects() : _completeCount(0) {}

        DECL_COROUTINE_FUNC(CheckComplete, void, (int targetCount))
        DECL_COROUTINE_FUNC(Check, void, (bugat::TSharedPtr<AwaitTestObject> target, int targetCount))
        
    public:
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
                    obj->Spawn_Add(1);
        }

       /* void CheckComplete(int targetCount)
        {
            for (auto& obj : _objects)
            {
                obj->Async_Complete([this, targetCount](int count) {
                    if (targetCount == count)
                        _completeCount.fetch_add(1, std::memory_order_release);
                    });
            }
        }*/

        void Complete() const
        {
            while (_objects.size() != _completeCount.load(std::memory_order_acquire))
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        std::vector<bugat::TSharedPtr<AwaitTestObject>> _objects;
        std::atomic<int> _completeCount;
    };

    DEF_COROUTINE_FUNC(AwaitTestObjects, CheckComplete, void, (int targetCount))
    {
        for (auto& obj : _objects)
        {
            Spawn_Check(obj, targetCount);
        }

        co_return;
    }

    DEF_COROUTINE_FUNC(AwaitTestObjects, Check, void, (bugat::TSharedPtr<AwaitTestObject> target, int targetCount))
    {
        //throw std::runtime_error("코루틴 내부에서 에러 발생!");
        auto count = co_await target->Await_GetCount();
        auto id = target->GetObjectId();
        if (targetCount == count)
        {
            _completeCount.fetch_add(1, std::memory_order_release);
            //InfoLog("{} Complete!", id.String());
        }
    }

	void AwaitTest(Context& context)
	{
		int threadCount = 10;
		int objectCount = 10000;
		int runningCount = 1000;

		std::vector<std::thread> threads;
		auto objects = CreateSerializeObject<AwaitTestObjects>(&context);
        objects->CreateObjects(objectCount);

        auto startMs = DateTime::NowMs();
        InfoLog("{} start [{}] {}", __FUNCTION__, std::this_thread::get_id(), startMs);

		for (int i = 0; i < threadCount; i++)
			threads.push_back(std::thread([&]() {
                objects->Run(runningCount);
				}));

		for (auto& t : threads)
			t.join();

		//objects->CheckComplete(threadCount * runningCount);
        objects->Spawn_CheckComplete(threadCount * runningCount);
		objects->Complete();
		auto endMs = DateTime::NowMs();
        InfoLog("{} end [{}] {} diff {}", __FUNCTION__, std::this_thread::get_id(), endMs, endMs - startMs);
	}
}