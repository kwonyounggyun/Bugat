#pragma once

#include <list>
#include "ObjectPool.h"

template<typename Header, int DefaultBlockSize = 1024>
class NetworkMessage
{
	class DataBlock
	{
	public:
		DataBlock() : _head(0), _tail(0) {}

		int GetSize()
		{
			return _tail - _head;
		}

		bool Update(int size)
		{
			if (size > DefaultBlockSize)
				return false;

			_tail = size;
			return true;
		}

		bool Consume(int size)
		{
			if (size > GetSize())
				return false;

			_head += size;
			return true;
		}

		char* GetBuf()
		{
			return &_buf[_head];
		}

	private:
		char _buf[DefaultBlockSize];
		int _head, _tail;
	};

public:
	NetworkMessage() : _size(0), _remainHeaderSize(sizeof(Header)) {}
	~NetworkMessage()
	{
		_listDataBlock.clear();
		_dataBlockPool.Release();
	}

	auto GetNewDataBlock()
	{
		return _dataBlockPool.GetObj();
	}

	bool AddDataBlock(std::shared_ptr<DataBlock>& block)
	{
		_listDataBlock.push_back(block);
		_size += block->GetSize();
	}

	bool GetNetMessage(std::vector<char>& message)
	{
		if (0 != _remainHeaderSize)
		{
			if (_size < _remainHeaderSize)
				return false;

			auto headerPtr = reinterpret_cast<char*>(&_header) + sizeof(Header) - _remainHeaderSize;
			for (auto iter = _listDataBlock.begin(); iter != _listDataBlock.end();)
			{
				auto blockSize = (*iter)->GetSize();
				if (blockSize < _remainHeaderSize)
				{
					memcpy(headerPtr, (*iter)->GetBuf(), blockSize);
					_size -= blockSize;
					_remainHeaderSize -= blockSize;
					headerPtr += blockSize;
					iter = _listDataBlock.erase(iter);
				}
				else
				{
					memcpy(headerPtr, (*iter)->GetBuf(), _remainHeaderSize);
					_size -= _remainHeaderSize;
					(*iter)->Consume(_remainHeaderSize);
					_remainHeaderSize = 0;
					break;
				}
			}
		}

		if (_header.size > _size)
			return false;

		message.resize(_header.size, 0);
		auto messageBuf = message.data();
		auto remainSize = _header.size;
		for (auto iter = _listDataBlock.begin(); iter != _listDataBlock.end();)
		{
			auto blockSize = (*iter)->GetSize();
			if (blockSize < remainSize)
			{
				memcpy(messageBuf, (*iter)->GetBuf(), blockSize);
				_size -= blockSize;
				remainSize -= blockSize;
				messageBuf += blockSize;
				iter = _listDataBlock.erase(iter);
			}
			else
			{
				memcpy(messageBuf, (*iter)->GetBuf(), remainSize);
				_size -= remainSize;
				(*iter)->Consume(remainSize);
				remainSize = 0;
				break;
			}
		}

		_remainHeaderSize = sizeof(Header);

		return true;
	}

private:
	Header _header;
	int _remainHeaderSize;

	std::list<std::shared_ptr<DataBlock>> _listDataBlock;
	ObjectPool<DataBlock, 10> _dataBlockPool;
	int _size = 0;
};