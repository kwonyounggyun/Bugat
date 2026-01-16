#pragma once
#include "../Core/Singleton.h"

#include "../Base/Context.h"
#include "../Base/NetworkContext.h"

namespace bugat
{
	class ContextHolder : public Singleton<ContextHolder>
	{
	public:
		ContextHolder() : _logicContext(10), _netClientContext(), _netServerContext() 
		{
			_logicContext.Initialize();
			_netServerContext.Initialize();
			_netClientContext.Initialize();
		}

		Context& GetLogicContext() { return _logicContext; }
		NetworkContext& GetNetClientContext() { return _netClientContext; }
		NetworkContext& GetNetServerContext() { return _netServerContext; }

	private:
		// 클라이언트 메세지 처리용
		NetworkContext _netClientContext;
		// 서버간 메세지, API 처리용
		NetworkContext _netServerContext;
		// 게임 로직 처리용
		Context _logicContext;
	};

#define CTXHolder ContextHolder::Instance()
#define LogicContext CTXHolder.GetLogicContext()
#define NetClientContext CTXHolder.GetNetClientContext()
#define NetServerContext CTXHolder.GetNetServerContext()
}