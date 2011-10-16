/**
* @file    PetCtrl.h
* @author  Fox (yulefox@gmail.com)
* @date    Nov.18, 2008
* @brief   CPetCtrl用于控制宠物AI.
*/

#pragma once

#include "../../../../Public/ServerPublic/Server/BaseDef/BasePetServerDef.h"

class CEntityGroup;
class CPlayer;

class CPetCtrl
{
public:
	CPetCtrl(void);
	~CPetCtrl(void);

public:
	/// 宠物配置开关
	long GetFuncSwitch(void) { return m_ConfPet.nFuncSwitch; }

	/// 加载宠物配置
	bool LoadConfig(void);

	/// 配置编码
	bool CodeToDataBlock(DBWriteSet& setWriteDB);

	/// WS==>DB编码
	bool CodeToDataBlock(CPlayer* pPlayer, CEntityGroup* pDBPetList);

	/// DB==>WS解码
	bool DecodeFromDataBlock(CEntityGroup* pDBPetList, CPlayer* pPlayer);

	/// WS==>GS编码
	bool CodeToDataBlock(DBWriteSet& setWriteDB, CPlayer* pPlayer);

	/// GS==>WS解码
	bool DecodeFromDataBlock(DBReadSet& setReadDB, CPlayer* pPlayer);
private:
	/// 读取宠物基本配置
	bool LoadPetBaseConfig(void);

	/// 读取宠物经验配置
	bool LoadPetExpConfig(void);

	/// 读取宠物列表
	bool LoadPetList(void);

	/// 读取宠物成长配置
	bool LoadPetGrowList(void);

private:
	tagPetConf		m_ConfPet;								///< 宠物系统统一配置
	GROW_CONF		m_GrowConfPets;							///< 宠物成长
	PET_LIST		m_ConfPets;								///< 宠物列表配置
};