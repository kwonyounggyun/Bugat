#pragma once
#include "BaseConnection.h"
#include "ThreadGroup.h"
#include "../AnyContext.h"

struct Configure;
struct IOContext;
class BaseServer
{
public:
	explicit BaseServer(AnyContext context);
	~BaseServer();
	void Start(AnyConnectionFactory factory, Configure config);

	//楷搬等 Connection 包府 肺流 累己
	virtual void AfterAccept(std::shared_ptr<BaseConnection>& conn) {}

private:
	 AnyContext& _context;
	ThreadGroup<IOContext> _threadGroup;
};