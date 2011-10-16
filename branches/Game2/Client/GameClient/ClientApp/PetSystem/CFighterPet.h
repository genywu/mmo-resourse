/*战宠  nikai 2009/1/10*/
#pragma once
class CPet;
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
	virtual void AI();
	virtual bool Display();
	virtual bool DecodeFromDataBlock(DBReadSet& setReadDB, bool bExData = true);
};