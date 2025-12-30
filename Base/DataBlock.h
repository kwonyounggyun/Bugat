#pragma once
#include "Header.h"

namespace bugat
{
	constexpr int DefaultBlockSize = MAX_PACKET_SIZE * 5;
	constexpr int MarginSize = 100;

	class DataBlock
	{
	public:
		DataBlock() : _buf({ 0, }), _head(0), _tail(0), _maxSize(DefaultBlockSize) {}

		char* GetRemainBuf()
		{
			return &_buf[_tail];
		}

		int GetRemainSize()
		{
			return _maxSize - _tail;
		}

		char* GetBuf(int pos)
		{
			if (pos < 0 || pos >= GetRemainSize())
				return nullptr;

			return &_buf[pos];
		}

		char* GetData()
		{
			return &_buf[_head];
		}

		int GetDataSize()
		{
			return _tail - _head;
		}

		bool Consume(int size)
		{
			if (size > GetDataSize())
				return false;

			_head += size;
			return true;
		}

		bool Update(int size)
		{
			if ((_tail + size) >= DefaultBlockSize)
				return false;

			_tail += size;
			if ((DefaultBlockSize - _tail) < MarginSize)
				_maxSize = _tail;

			return true;
		}

		int GetHead() const { return _head; }

	private:
		char _buf[DefaultBlockSize];
		int _head, _tail;
		int _maxSize;
	};
}