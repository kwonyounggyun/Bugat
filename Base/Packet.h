#pragma once
#include "Header.h"
#include "DataBlock.h"
#include "../Core/Memory.h"
#include <flatbuffers/flatbuffers.h>

namespace bugat
{
	template<typename Header>
	class SendPacket
	{
	public:
		SendPacket(Header& header, const std::shared_ptr<flatbuffers::FlatBufferBuilder>& fb) : _header(header), _fb(fb) {}

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

	template<typename Header, int MaxPacketSize>
	class RecvPacket : public RefCountable
	{
	public:
		using HeaderType = Header;
		using DataBlockType = DataBlock<MaxPacketSize>;
		static constexpr int HeaderSize = sizeof(HeaderType);
		static constexpr int PacketSize = MaxPacketSize;

		RecvPacket(TSharedPtr<DataBlockType>& block, int pos) : _block(block), _pos(pos) {}
		~RecvPacket() {}

		template<typename T>
		bool Get(const T*& message) const
		{
			if (auto buf = _block->GetBuf(_pos + HeaderSize); buf != nullptr)
			{
				message = flatbuffers::GetRoot<T>(buf);
				return true;
			}
			return false;
		}

		HeaderType GetHeader() const { return *(reinterpret_cast<HeaderType*>(_block->GetBuf(_pos))); }

	private:
		TSharedPtr<DataBlockType> _block;
		int _pos;
	};

	using TCPSendPacket = SendPacket<TCPHeader>;
	using UDPSendPacket = SendPacket<UDPHeader>;

	using TCPRecvPacket = RecvPacket<TCPHeader, MAX_TCP_PACKET_SIZE>;
	using UDPRecvPacket = RecvPacket<UDPHeader, MAX_UDP_PACKET_SIZE>;
}