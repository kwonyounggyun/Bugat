#include "pch.h"
#include "NetworkMessage.h"
#include "Packet.h"

namespace bugat
{
	NetworkMessage::NetworkMessage() : _size(0)
	{
	}

	NetworkMessage::~NetworkMessage()
	{
	}

	void NetworkMessage::Update(int size)
	{
		_curBlock->Update(size);
	}

	BufInfo NetworkMessage::GetBufInfo()
	{
		if (_curBlock->GetRemainSize() < MAX_PACKET_SIZE)
		{
			auto newBlock = _dataBlockPool.Get();
			memcpy_s(newBlock->GetRemainBuf(), newBlock->GetRemainSize(), _curBlock->GetData(), _curBlock->GetDataSize());
			_curBlock = newBlock;
		}

		return BufInfo(_curBlock->GetRemainBuf(), _curBlock->GetRemainSize());
	}

	bool NetworkMessage::GetNetMessage(std::shared_ptr<RecvPacket>& packet)
	{
		if (_curBlock->GetDataSize() < HEADER_SIZE)
			return false;

		Header* header = reinterpret_cast<Header*>(_curBlock->GetData());
		if (_curBlock->GetDataSize() < header->size + HEADER_SIZE)
			return false;

		packet = std::make_shared<RecvPacket>(_curBlock, _curBlock->GetHead() + HEADER_SIZE, *header);
		return true;
	}
}