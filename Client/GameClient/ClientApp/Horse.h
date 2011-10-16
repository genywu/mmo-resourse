#pragma once

#include "specialequip.h"
#include "Shape.h"
class CMoveShape;


class CHorse:public CSpecialEquip
{

public:
	enum HORSE_STATUS 
	{
		HS_IDLE = CShape::ACT_IDLE,
		HS_STAND = CShape::ACT_STAND,
		HS_WALK = CShape::ACT_WALK,
		HS_RUN = CShape::ACT_RUN,
		HS_HURT = CShape::ACT_BEATTACK,
		HS_MISS = CShape::ACT_MISS,
		HS_MEGRIM = CShape::ACT_MEGRIM,

	};
	CHorse();
	~CHorse();
	void ChangeStatus(DWORD eStaus,bool bforceupdate = false);
	void Display(CMoveShape * pParentObj, bool bIsUpdateAnim = true);			//ÏÔÊ¾×øÆï
	void SetHorseType(int type){m_iHorsetype = type;}
	int GetHorseType(){return m_iHorsetype;}

private:
	int m_iHorsetype;

};