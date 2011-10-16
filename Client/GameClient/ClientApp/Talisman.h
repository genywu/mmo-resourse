#pragma once

#include "specialequip.h"



class CTalisman:public CSpecialEquip 
{
	
	

public:
	enum TALISMAN_STATUS { TS_IDLE,TS_STAND,TS_WALK,TS_RUN,TS_HURT};
	CTalisman();
	~CTalisman();

	void ChangeStatus(DWORD eStaus);

	

};