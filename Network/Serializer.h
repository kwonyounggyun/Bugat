#include <atomic>


template<typename T, typename Fn>
class Serializer 
{
	void Push(Fn&& fn)
	{
		
	}

	void Run()
	{

	}

private:
	T _container;
	std::atomic<int> _count{ 0 };
};