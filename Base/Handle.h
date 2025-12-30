#pragma once
#include "Session.h"
#include "Protocol.h"
#include "Packet.h"
#include "../Core/Log.h"

namespace bugat
{
	class BaseHandle
	{
	public:
		BaseHandle() {}
		~BaseHandle() {}

		void operator()(std::shared_ptr<Session>& session, const std::shared_ptr<RecvPacket>& packet)
		{
			//if(session->IsAuth())
				_Handle(session, packet);
			//else if(header.type == static_cast<int>(bugat::protocol::game::Type::REQ_CA_LOGIN))
				//_Handle(session, header, msg);
		}

	protected:
		virtual void _Handle(std::shared_ptr<Session>& session, const std::shared_ptr<RecvPacket>& packet) = 0;
	};

#define DEFINE_FB_HANDLE(className) \
	class className##Handle : public BaseHandle \
	{ \
	public: \
		className##Handle() {} \
		~className##Handle() {} \
	protected: \
		virtual void _Handle(std::shared_ptr<Session>& session, const std::shared_ptr<RecvPacket>& packet) override \
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
		void __Handle(std::shared_ptr<Session>& session, const bugat::protocol::game::className* data); \
	};

#define DECLARE_FB_HANDLE(className) \
	void className##Handle::__Handle(std::shared_ptr<Session>& session, const bugat::protocol::game::className* data)

#define MAKE_FB_HANDLE(className) std::make_shared<className##Handle>()
}