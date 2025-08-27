#pragma once
#include "BaseConnection.h"

struct Configure;
class BaseServer
{
public:
	void Start(AnyConnectionFactory factory, Configure config);

	//楷搬等 Connection 包府 肺流 累己
	virtual void AfterAccept(std::shared_ptr<BaseConnection> conn) = 0;
};