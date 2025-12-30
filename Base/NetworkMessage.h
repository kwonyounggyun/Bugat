#pragma once

#include "../Core/ObjectPool.h"
#include <list>
#include <vector>

namespace bugat
{
	class DataBlock;
	class RecvPacket;

	struct BufInfo
	{
		char* buf;
		int size;
	};

	class NetworkMessage
	{
	public:
		NetworkMessage();
		~NetworkMessage();

		void Update(int size);
		BufInfo GetBufInfo();
		bool GetNetMessage(std::shared_ptr<RecvPacket>& packet);

	private:
		std::shared_ptr<DataBlock> _curBlock;
		bugat::ObjectPool<DataBlock, 2> _dataBlockPool;
		int _size = 0;
	};


	
}