#pragma once
#include <atomic>
#include <condition_variable>
#include "NonCopyable.h"

namespace bugat
{
	template<class ObjectType>
	class RWLockObject
	{
	public:
		class ReadObject : NonCopyable
		{
		public:
			explicit ReadObject(RWLockObject& object) : _rwObject(object) {}
			virtual ~ReadObject() 
			{
				_rwObject.UnLockRead();
			}

			const ObjectType* const operator ->()
			{
				return &_rwObject._obj;
			}

		private:
			RWLockObject& _rwObject;
		};

		class WriteObject : NonCopyable
		{
		public:
			explicit WriteObject(RWLockObject& object) : _rwObject(object) {}
			virtual ~WriteObject()
			{
				_rwObject.UnLockWrite();
			}

			ObjectType* operator ->()
			{
				return &_rwObject._obj;
			}

		private:
			RWLockObject& _rwObject;
		};

		ReadObject LockRead()
		{
			std::unique_lock lock(_mtx);
			_readerCV.wait(lock, [&]() {
				return _writerCount == 0;
				});
			++_readerCount;
			return ReadObject(*this);
		}

		WriteObject LockWrite()
		{
			std::unique_lock lock(_mtx);
			++_writerCount;
			_writerCV.wait(lock, [&]() {
				return _readerCount == 0 && _writerActive == false;
				});
			_writerActive = true;
			return WriteObject(*this);
		}

	private:
		void UnLockRead()
		{
			std::unique_lock lock(_mtx);
			if (--_readerCount == 0)
				_writerCV.notify_one();
		}

		void UnLockWrite()
		{
			std::unique_lock lock(_mtx);
			--_writerCount;
			_writerActive = false;
			if (--_writerCount > 0)
				_writerCV.notify_one();
			else
				_readerCV.notify_all();
		}

		std::mutex _mtx;
		std::condition_variable _readerCV;
		std::condition_variable _writerCV;

		int _readerCount = 0;     // 현재 읽는 중인 reader 수
		int _writerCount = 0;     // 대기 + 실행 중인 writer 수
		bool _writerActive = false;

		ObjectType _obj;
	};
}