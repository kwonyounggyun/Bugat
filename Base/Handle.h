#pragma once
#include "Session.h"
#include "Protocol.h"
#include "Packet.h"

#include "../Core/Memory.h"
#include "../Base/Log.h"

namespace bugat
{
	template<typename T>
	class TCPPacketHandle : public RefCountable
	{
	public:
		TCPPacketHandle() {}
		virtual ~TCPPacketHandle() {}

		void operator()(TSharedPtr<Session>& session, const TSharedPtr<TCPRecvPacket>& packet)
		{
			auto castSession = StaticPointerCast<T>(session);
			_Handle(castSession, packet);
		}

	protected:
		virtual void _Handle(TSharedPtr<T>& session, const TSharedPtr<TCPRecvPacket>& packet) = 0;
	};

#define DEFINE_FB_HANDLE(sessionType, className) \
	class className##Handle : public TCPPacketHandle<sessionType> \
	{ \
	public: \
		className##Handle() {} \
		virtual ~className##Handle() {} \
	protected: \
		virtual void _Handle(TSharedPtr<sessionType>& session, const TSharedPtr<TCPRecvPacket>& packet) override \
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
		void __Handle(TSharedPtr<sessionType>& session, const bugat::protocol::game::className* packet); \
	};

#define DECLARE_FB_HANDLE(sessionType, className) \
	void className##Handle::__Handle(TSharedPtr<sessionType>& session, const bugat::protocol::game::className* packet)

#define MAKE_FB_HANDLE(className) TSharedPtr<className##Handle>(new className##Handle())
}