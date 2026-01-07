#pragma once
#include "Session.h"
#include "Protocol.h"
#include "Packet.h"
#include "../Base/Log.h"

namespace bugat
{
	template<typename T>
	class TCPPacketHandle
	{
	public:
		TCPPacketHandle() {}
		virtual ~TCPPacketHandle() {}

		void operator()(std::shared_ptr<Session>& session, const std::shared_ptr<TCPRecvPacket>& packet)
		{
			auto castSession = std::static_pointer_cast<T>(session);
			_Handle(castSession, packet);
		}

	protected:
		virtual void _Handle(std::shared_ptr<T>& session, const std::shared_ptr<TCPRecvPacket>& packet) = 0;
	};

#define DEFINE_FB_HANDLE(sessionType, className) \
	class className##Handle : public TCPPacketHandle<sessionType> \
	{ \
	public: \
		className##Handle() {} \
		virtual ~className##Handle() {} \
	protected: \
		virtual void _Handle(std::shared_ptr<sessionType>& session, const std::shared_ptr<TCPRecvPacket>& packet) override \
		{ \
			const bugat::protocol::game::className* p = nullptr; \
			if(true == packet->Get<bugat::protocol::game::className>(p)) \
			{ \
				__Handle(session, p); \
			} \
			else \
			{ \
				ErrorLog("Packet Error"); \
			} \
		} \
		void __Handle(std::shared_ptr<sessionType>& session, const bugat::protocol::game::className* packet); \
	};

#define DECLARE_FB_HANDLE(sessionType, className) \
	void className##Handle::__Handle(std::shared_ptr<sessionType>& session, const bugat::protocol::game::className* packet)

#define MAKE_FB_HANDLE(className) std::make_shared<className##Handle>()
}