// Fox@20081118----------------------------------------------
// File:    PetCtrl.h
// Brief:   CPetCtrl用于控制宠物AI.
//
// Author:  Fox (yulefox@gmail.com)
// Date:    Nov.18, 2008
// Fox@20081118----------------------------------------------

#include "../public/BasePetServerDef.h"

#pragma once

class CEntityGroup;
class CPlayer;
class CMonster;
struct tagDataBlockWriteSet;

class CPetCtrl
{
public:
	CPetCtrl(void);
	virtual ~CPetCtrl(void);

public:
	long GetFuncSwitch(void) { return m_ConfPet.nFuncSwitch; }
	bool LoadConfig(void);									// 宠物配置
	bool CodeToDataBlock(DBWriteSet& setWriteDB);			// 配置编码

	bool CodeToDataBlock(CPlayer* pPlayer,
		CEntityGroup* pDBPetList);							// WS==>DB编码
	bool DecodeFromDataBlock(CEntityGroup* pDBPetList,
		CPlayer* pPlayer);									// DB==>WS解码

	bool CodeToDataBlock(DBWriteSet& setWriteDB,
		CPlayer* pPlayer);									// WS==>GS编码
	bool DecodeFromDataBlock(DBReadSet& setReadDB,
		CPlayer* pPlayer);									// GS==>GS解码

private:
	tagPetConf		m_ConfPet;								// 宠物系统统一配置
	GROW_CONF		m_GrowConfPets;							// 宠物成长
	PET_LIST		m_ConfPets;								// 宠物列表配置
};