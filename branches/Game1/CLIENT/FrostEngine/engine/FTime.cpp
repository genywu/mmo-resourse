#include "StdAfx.h"
#include "ftime.h"
#include "../../../public/tools.h"

//CLASS__(time::Clock)___
void time::Clock::Update()
{
	m_dwCurTime = GetCurTickCount();
}


//CLASS__(time::Manager)___
IMPLEMENT_SINGLETON(time::Manager);
void time::Manager::Create()
{
	//´ıÌí¼Ó:
}

void time::Manager::Destroy()
{
	//´ıÌí¼Ó:
}
void time::Manager::Update()
{
	m_cClock.Update();

	//´ıÌí¼Ó:
}

