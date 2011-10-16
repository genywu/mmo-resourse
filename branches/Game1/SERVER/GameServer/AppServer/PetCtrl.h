// Fox@20081118----------------------------------------------
// File:    PetCtrl.h
// Brief:   CPetCtrl用于控制宠物AI.
//
// Author:  Fox (yulefox@gmail.com)
// Date:    Nov.18, 2008
// Fox@20081118----------------------------------------------

#include "../public/GUID.h"
#include "../public/BasePetServerDef.h"
#include "../../NETS/NetServer/MsgRegister.h"

#pragma once

class CEntityGroup;
class CDataEntityManager;
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
	bool DecodeFromDataBlock(DBReadSet& setReadDB);				// 配置解码

	bool CodeToDataBlockForClient(DBWriteSet& setWriteDB,
		CPlayer* pPlayer);										// GS==>Client编码
	bool CodeToDataBlock(DBWriteSet& setWriteDB,
		CPlayer* pPlayer);										// GS==>WS编码
	bool DecodeFromDataBlock(DBReadSet& setReadDB,
		CPlayer* pPlayer);										// WS==>GS解码

	CPet* GetPet(const CGUID& rID);
	void DelPet(CPet* pPet, long lOpType);						// 删除(未)宠物
	void ActivatePet(CPet* pPet);								// 激活宠物
	bool RelivePet(CPet* pPet, long lInitType);					// 复活宠物
	void OnDeletePet(void);										// 宠物删除处理

	void GetGrowCoefs(long lIndex, GROW_LIST& rList)			// 获取宠物成长系数
	{
		ITR_GROW_CONF itr = m_GrowConfPets.find(lIndex);
		if( itr != m_GrowConfPets.end() )
		{
			rList = itr->second;
		}
	}

	const tagPetConf& GetPetConfig(void)						// 宠物统一配置
	{
		return m_ConfPet;
	}
	const PET_LIST& GetPetsConfig(void)							// 宠物列表配置
	{
		return m_ConfPets;
	}
	void InformClientAI(CPet* pPet, long lOpType);				// 通知客户端AI事件

	// 宠物脚本接口
	CPet* CreatePet(CPlayer* pPlayer,
		long lType,
		long lIndexID);											// 创建宠物
	bool DelPet(const CGUID& rID);								// 删除宠物(为HOUSE)
	bool RelivePet(CPlayer* pPlayer, long lType, long lIndex);	// 复活宠物
	bool AddPetSkill(CPlayer* pPlayer,
		long lType,
		long lID,
		long lLevel,
		long lPos);												// 添加宠物技能

private:
	void InformWS(CPlayer* pPlayer);
	void InformClient(CPlayer* pPlayer, CPet* pPet, bool bAdd);

private:
	tagPetConf		m_ConfPet;
	GROW_CONF		m_GrowConfPets;								// 宠物成长
	PET_LIST		m_ConfPets;									// 宠物列表配置
	vector<CPet*>	m_DetectPets;								// 叛离宠物列表
	PETS			m_Pets;										// 激活宠物的列表

	DECLARE_MSG_MAP();
};