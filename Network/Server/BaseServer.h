#pragma once

struct Configure;
class BaseServer
{
public:
	void Start(Configure configs);

	virtual void ProcessConnection();
};