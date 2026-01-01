#pragma once

namespace bugat
{
	template<int Size>
	class DataBlock
	{
	public:
		static constexpr int Size = Size;
		static constexpr int MaxSize = Size * 5;
		DataBlock() : _buf({ 0, }), _head(0), _tail(0) {}

		char* GetRemainBuf()
		{
			return &_buf[_tail];
		}

		int GetRemainSize()
		{
			return MaxSize - _tail;
		}

		char* GetBuf(int pos)
		{
			if (pos < 0 || pos >= MaxSize)
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
			if ((_tail + size) > MaxSize)
				return false;

			_tail += size;

			return true;
		}

		int GetHead() const { return _head; }

	private:
		char _buf[MaxSize];
		int _head, _tail;
	};
}