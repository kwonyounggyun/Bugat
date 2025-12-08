#pragma once
#include "Session.h"
#include "../Network/Header.h"
#include "../Network\Protocol.h"

namespace bugat
{
	class BaseHandle
	{
	public:
		BaseHandle() {}
		~BaseHandle() {}

		void operator()(std::shared_ptr<Session>& session, const net::Header& header, const std::vector<char>& msg)
		{
			if (header.size != msg.size())
				return;

			//if(session->IsAuth())
				_Handle(session, header, msg);
			//else if(header.type == static_cast<int>(bugat::protocol::game::Type::REQ_CA_LOGIN))
				//_Handle(session, header, msg);
		}

	protected:
		virtual void _Handle(std::shared_ptr<Session>& session, const net::Header& header, const std::vector<char>& msg) = 0;
	};

#define DEFINE_FB_HANDLE(className) \
	class className##Handle : public BaseHandle \
	{ \
	public: \
		className##Handle() {} \
		~className##Handle() {} \
	protected: \
		virtual void _Handle(std::shared_ptr<Session>& session, const net::Header& header, const std::vector<char>& msg) override \
		{ \
			auto data = flatbuffers::GetRoot<bugat::protocol::game::className>(msg.data()); \
			__Handle(session, data); \
		} \
		void __Handle(std::shared_ptr<Session>& session, const bugat::protocol::game::className* data); \
	};

#define DECLARE_FB_HANDLE(className) \
	void className##Handle::__Handle(std::shared_ptr<Session>& session, const bugat::protocol::game::className* data)

#define MAKE_FB_HANDLE(className) std::make_shared<className##Handle>()
}