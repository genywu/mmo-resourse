/************************************************************************/
/* 宠物精灵 模块  nikai 2009/1/10/                                    */
/************************************************************************/
#pragma once
#include "CPet.h"
class CFairy:public	CPet
{
private:
	PET_FOOD_SATIETY			m_emFoodsSatiefy;	//宠物饱食度

public:
	CFairy();
	virtual ~CFairy();
	virtual void AI();
	virtual bool Display();

	inline void SetSatiefyState(PET_FOOD_SATIETY emSatify)	{m_emFoodsSatiefy = emSatify;}
	
	PET_FOOD_SATIETY GetSatietyState(void);	//获得精灵饱食度等级

	virtual bool DecodeFromDataBlock(DBReadSet& setReadDB, bool bExData = true);
};