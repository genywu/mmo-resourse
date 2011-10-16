#pragma once
#include "specialequip.h"
#include "Shape.h"
class CHeadWear:public CSpecialEquip
{
public:
	enum WING_STATUS 
	{ 
		HeadWear_IDLE = CShape::ACT_IDLE,
		HeadWear_STAND = CShape::ACT_STAND,
		HeadWear_WALK = CShape::ACT_WALK,
		HeadWear_RUN = CShape::ACT_RUN,
		//WING_HURT = CShape::ACT_BEATTACK,
	};
	CHeadWear(void);
	~CHeadWear(void);
	void Display(CShape * pParentObj);
	void ChangeStatus(DWORD eStaus);
};
