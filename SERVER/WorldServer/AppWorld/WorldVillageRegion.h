//////////////////////////////////////////////////////////////////////////
//worldvillageregion.h
//村战战地图类，负责村战的数据处理
//Author：Joe
//Create Date:2005.7.5
//////////////////////////////////////////////////////////////////////////


#pragma once
#include "worldwarregion.h"

class CWorldVillageRegion :
	public CWorldWarRegion
{
public:
	CWorldVillageRegion(void);
	~CWorldVillageRegion(void);

public:
	BOOL Load();	// 读取
	void SetEnterPosXY(CPlayer* pPlayer);
};
