#pragma once
#include "../Core/ObjectPool.h"
#include <list>
#include <vector>

namespace bugat
{
	struct BufInfo
	{
		char* buf;
		int size;
	};

	template<typename PacketType>
	class NetworkMessage
	{
	public:
		using HeaderType = PacketType::HeaderType;
		using DataBlockType = PacketType::DataBlockType;

		NetworkMessage()
		{
		}

		~NetworkMessage()
		{
		}

		void Update(int size)
		{
			_curBlock->Update(size);
		}

		BufInfo GetBufInfo()
		{
			if (!_curBlock)
			{
				auto newBlock = _dataBlockPool.Get();
				_curBlock = newBlock;
			}
			else if (_curBlock->GetRemainSize() < PacketType::PacketSize)
			{
				auto newBlock = _dataBlockPool.Get();
				memcpy_s(newBlock->GetRemainBuf(), newBlock->GetRemainSize(), _curBlock->GetData(), _curBlock->GetDataSize());
				_curBlock = newBlock;
			}

			return BufInfo(_curBlock->GetRemainBuf(), PacketType::PacketSize);
		}

		bool GetNetMessage(TSharedPtr<PacketType>& packet)
		{
			if (_curBlock->GetDataSize() < PacketType::HeaderSize)
				return false;

			HeaderType* header = reinterpret_cast<HeaderType*>(_curBlock->GetData());
			if (_curBlock->GetDataSize() < header->size + PacketType::HeaderSize)
				return false;

			auto ptr = new PacketType(_curBlock, _curBlock->GetHead());
			packet = TSharedPtr<PacketType>(ptr);
			_curBlock->Consume(header->size + PacketType::HeaderSize);
			return true;
		}

	private:
		TSharedPtr<DataBlockType> _curBlock;
		ObjectPool<DataBlockType, 2> _dataBlockPool;
	};
}