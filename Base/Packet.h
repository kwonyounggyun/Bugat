#pragma once
#include "Header.h"
#include "DataBlock.h"
#include <flatbuffers/flatbuffers.h>

namespace bugat
{
	class SendPacket
	{
	public:
		SendPacket(int type, std::shared_ptr<flatbuffers::FlatBufferBuilder>& fb)
		{
			_header.type = type;
			_header.size = fb->GetSize();
			_fb = fb;
		}
		std::vector<std::tuple<uint8_t*, size_t>> data()
		{
			std::vector<std::tuple<uint8_t*, size_t>> bufs;
			bufs.push_back({ reinterpret_cast<uint8_t*>(&_header), sizeof(_header) });
			bufs.push_back({ _fb->GetBufferPointer(), _fb->GetSize() });
			return bufs;
		}

	private:
		Header _header;
		std::shared_ptr<flatbuffers::FlatBufferBuilder> _fb;
	};

	class RecvPacket
	{
	public:
		RecvPacket(std::shared_ptr<DataBlock>& block, int pos, Header header) : _block(block), _pos(pos), _header(header) {}
		~RecvPacket() {}

		template<typename T>
		bool Get(const T*& message) const
		{
			if (auto buf = _block->GetBuf(_pos); buf != nullptr)
			{
				message = flatbuffers::GetRoot<T>(buf);
				return true;
			}
			return false;
		}

		Header GetHeader() const { return _header; }

	private:
		std::shared_ptr<DataBlock> _block;
		int _pos;
		Header _header;
	};
}