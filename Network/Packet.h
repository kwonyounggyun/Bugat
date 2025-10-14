#pragma once
#include "Header.h"
#include <flatbuffers/flatbuffers.h>

namespace bugat::net
{
	struct Packet
	{
		Packet(int type, std::shared_ptr<flatbuffers::FlatBufferBuilder>& fb)
		{
			_header.type = type;
			_header.size = fb->GetSize();
		}
		std::vector<std::tuple<uint8_t*, size_t>> data()
		{
			std::vector<std::tuple<uint8_t*, size_t>> bufs;
			bufs.push_back({ reinterpret_cast<uint8_t*>(&_header), sizeof(_header) });
			bufs.push_back({ _fb->GetBufferPointer(), _fb->GetSize() });
			return bufs;
		}

		Header _header;
		std::shared_ptr<flatbuffers::FlatBufferBuilder> _fb;
	};
}