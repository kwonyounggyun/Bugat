#include "stdafx.h"
#include "GameHandler.h"

using namespace bugat::protocol::game;
namespace bugat
{
	DECLARE_FB_HANDLE(Req_CS_Login)
	{
	}

	DECLARE_FB_HANDLE(Req_CS_Move)
	{
		auto pos = data->pos();
		auto fb = FBCreate();
		fb->Finish(CreateRes_SC_Move(*fb, pos));

		session->Send(static_cast<int>(Type::RES_SC_MOVE), fb);
	}
}