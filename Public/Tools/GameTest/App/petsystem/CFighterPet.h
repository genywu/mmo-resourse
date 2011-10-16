#pragma once
#include "CPet.h"
class CFighterPet:public CPet
{
public:
	CFighterPet();
	virtual ~CFighterPet();
protected:
	//战宠装备栏位
	enum  EquipmentIndex
	{
		EI_WEAPON = 0,
		EI_ARMOR  = 1,
		EI_RING	  = 2
	}m_EquipIndex;

public:
	virtual bool DecodePetFromDataBlock(DBReadSet& setReadDB, bool bExData = true);
	virtual void AI();
};