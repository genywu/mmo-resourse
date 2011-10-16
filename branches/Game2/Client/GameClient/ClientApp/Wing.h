#pragma once

#include "SpecialEquip.h"
#include "Shape.h"
class CWing:public CSpecialEquip
{

public:
	enum WING_STATUS 
	{ 
		WING_IDLE = CShape::ACT_IDLE,
		WING_STAND = CShape::ACT_STAND,
		WING_WALK = CShape::ACT_WALK,
		WING_RUN = CShape::ACT_RUN,
		//WING_HURT = CShape::ACT_BEATTACK,
	};

	CWing();
	~CWing();
	//void Copy(CSpecialEquip* psequip);
	void ChangeStatus(DWORD eStaus);
	void Display(CShape * pParentObj);

};