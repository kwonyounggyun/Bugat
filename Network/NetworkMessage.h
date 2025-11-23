#pragma once

#include "../Core/ObjectPool.h"
#include "Header.h"
#include <list>
#include <vector>

namespace bugat::net
{
	constexpr int DefaultBlockSize = 10240;
	constexpr int MarginSize = 100;

	class NetworkMessage
	{
	public:
		class DataBlock
		{
			friend class NetworkMessage;
		public:
			DataBlock() : _buf({ 0, }), _head(0), _tail(0), _maxSize(DefaultBlockSize) {}

			char* GetBuf()
			{
				return &_buf[_tail];
			}

			int GetSize()
			{
				return _maxSize - _tail;
			}

		private:
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

		private:
			char _buf[DefaultBlockSize];
			int _head, _tail;
			int _maxSize;
		};

	public:
		NetworkMessage() : _size(0), _remainHeaderSize(sizeof(Header)) 
		{
			_dataBlockPool = ObjectPoolFactory::Create<DataBlock, 1>();
		}
		~NetworkMessage()
		{
			_listDataBlock.clear();
			_dataBlockPool = nullptr;
		}

		void Update(std::shared_ptr<DataBlock>& block, int size)
		{
			block->Update(size);
			_size += size;
		}

		auto GetDataBlock()
		{
			auto iter = _listDataBlock.rbegin();
			if (iter == _listDataBlock.rend() || (*iter)->GetSize() == 0)
			{
				auto block = _dataBlockPool->Get();
				AddDataBlock(block);
				return block;
			}

			return *iter;
		}

		bool AddDataBlock(std::shared_ptr<DataBlock>& block)
		{
			auto blockSize = block->GetDataSize();
			_listDataBlock.push_back(block);
			_size += blockSize;

			return true;
		}

		bool GetNetMessage(Header& header, std::vector<char>& message)
		{
			if (0 != _remainHeaderSize)
			{
				if (_size < _remainHeaderSize)
					return false;

				auto headerPtr = reinterpret_cast<char*>(&_header) + sizeof(Header) - _remainHeaderSize;
				for (auto iter = _listDataBlock.begin(); iter != _listDataBlock.end();)
				{
					auto blockSize = (*iter)->GetDataSize();
					if (blockSize < _remainHeaderSize)
					{
						memcpy(headerPtr, (*iter)->GetData(), blockSize);
						_size -= blockSize;
						_remainHeaderSize -= blockSize;
						headerPtr += blockSize;
						iter = PopBlock(iter);
					}
					else
					{
						memcpy(headerPtr, (*iter)->GetData(), _remainHeaderSize);
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
				auto blockSize = (*iter)->GetDataSize();

				if (blockSize < remainSize)
				{
					memcpy(messageBuf, (*iter)->GetData(), blockSize);
					_size -= blockSize;
					remainSize -= blockSize;
					messageBuf += blockSize;
					iter = PopBlock(iter);
				}
				else
				{
					memcpy(messageBuf, (*iter)->GetData(), remainSize);
					_size -= remainSize;
					(*iter)->Consume(remainSize);
					remainSize = 0;
					break;
				}
			}

			_remainHeaderSize = sizeof(Header);
			header = _header;

			return true;
		}
	private:
		std::list<std::shared_ptr<DataBlock>>::iterator PopBlock(std::list<std::shared_ptr<DataBlock>>::iterator& iter)
		{
			if((*iter)->GetSize() == 0 && (*iter)->GetDataSize() == 0)
				return _listDataBlock.erase(iter);

			return iter;
		}

	private:
		Header _header;
		int _remainHeaderSize;

		std::list<std::shared_ptr<DataBlock>> _listDataBlock;
		std::shared_ptr<bugat::ObjectPool<DataBlock, 1>> _dataBlockPool;
		int _size = 0;
	};
}