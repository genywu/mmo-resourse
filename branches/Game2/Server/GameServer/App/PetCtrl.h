/**
* @file    PetCtrl.h
* @author  Fox (yulefox@gmail.com)
* @date    Nov.18, 2008
* @brief   CPetCtrl用于控制宠物AI.
*/

#pragma once

#include "../Net/MsgRegister.h"
#include "../../../Public/Common/BasePetDef.h"
#include "../../../Public/Common/PetSetup.h"
#include "../../../Public/ServerPublic/Server/BaseDef/BasePetServerDef.h"

class CPlayer;

class CPetCtrl
{
public:
	CPetCtrl(void);
	~CPetCtrl(void);

	// 配置、编解码接口
public:
	/// 取得宠物功能开关
	long GetFuncSwitch(void) { return m_ConfPet.nFuncSwitch; }

	/// 根据宠物Index获取成长系数
	void GetGrowCoefs(long lIndex, GROW_LIST& rList);

	/// 取得宠物统一配置
	const tagPetConf& GetPetConfig(void) { return m_ConfPet; }
	
	/// 取得宠物列表配置
	const PET_LIST& GetPetsConfig(void)	{ return m_ConfPets; }

	/// 配置解码
	bool DecodeFromDataBlock(DBReadSet& setReadDB);

	/// 宠物GS==>Client编码
	bool CodeToDataBlockForClient(DBWriteSet& setWriteDB,CPlayer* pPlayer);
	
	/// 宠物GS==>WS编码
	bool CodeToDataBlock(DBWriteSet& setWriteDB,CPlayer* pPlayer);

	/// 宠物WS==>GS解码
	bool DecodeFromDataBlock(DBReadSet& setReadDB, CPlayer* pPlayer);

public:

	void InformClientAI(CPet* pPet, long lOpType);				// 通知客户端AI事件

	/// 激活宠物
	void ActivePet(CPet* pPet);

	//void DelPet(CPet* pPet, long lOpType);						// 删除(未)宠物

	// 宠物脚本接口

	/// 根据宠物索引ID创建宠物
	/// @param  pPlayer		玩家
	/// @param  type		宠物类型
	/// @param  id			宠物索引ID
	/// @return	创建宠物成功与否
	bool CreatePet(CPlayer* pPlayer,long lType, long lIndexID, bool IsWild);

	/// 删除宠物
	/// @param  pPlayer		玩家
	/// @param  guid		宠物GUID
	/// @return	成功与否
	bool DelPet(CPlayer* pPlayer, const CGUID& guid);

	/// 激活宠物
	/// @param player	玩家
	/// @param  guid		宠物GUID
	/// @return	成功与否
	bool ActivePet(CPlayer* player, const CGUID& guid);

	/// 删除激活宠物
	/// @param player	玩家
	/// @param  guid		宠物GUID
	/// @return	成功与否
	bool UnActivePet(CPlayer* player, const CGUID& guid);

	/// 设置宠物属性
	/// @param player	玩家
	/// @param attrName 属性名
	/// @param attrVal 属性值
	/// @param guid 宠物GUID
	/// @return	成功与否
	bool SetPetAttr(CPlayer* player, const char *attrName, double attrVal, const CGUID& guid);

	/// 获取宠物属性值
	/// @param player	玩家
	/// @param attrName 属性名
	/// @param guid 宠物GUID
	/// @return 属性值
	long GetPetAttr(CPlayer* player, const char* attrName, const CGUID& guid);


	/// 添加宠物技能
	/// @param  player		玩家
	/// @param  guid		宠物GUID
	/// @param  id			技能ID
	/// @param  level		技能等级
	/// @param	pos			位置
	/// @return	添加宠物技能成功与否
	bool AddPetSkill(CPlayer* player,
		const CGUID& guid,
		long id,
		long level,
		long pos);												// 添加宠物技能

	/// 重置宠物天资
	/// @param  player		玩家
	/// @param  guid		宠物GUID
	/// @return	重置天资成功与否
	bool ResetPetTalent(CPlayer* player,const CGUID& guid);

	/// 后天资质强化
	/// @param  player		玩家
	/// @param  guid		宠物GUID
	/// @param	val			强化系数(道具强化效果值)
	/// @return	后天资质强化成功与否
	bool UpgradePetDAfterVal(CPlayer* player,const CGUID& guid,long val);

	/// 重置后天资质强化次数
	/// @param  player		玩家
	/// @param  guid		宠物GUID
	/// @return	重置后天资质强化次数成功与否
	bool ResetPetDAfterCnt(CPlayer* player,const CGUID& guid);

	/// 重置宠物悟性点
	/// @param  player		玩家
	/// @param  guid		宠物GUID
	/// @return	重置宠物悟性点成功与否
	bool ResetPetSavvy(CPlayer* player,const CGUID& guid);

	/// 取得侍从品级
	/// @param  player		玩家
	/// @param  guid		宠物GUID
	/// @return 侍从品级
	long GetPetGrade(CPlayer* player,const CGUID& guid);

	/// 取得侍从后天资质已强化次数
	/// @param  player		玩家
	/// @param  guid		宠物GUID
	/// @return 侍从后天已强化次数
	long GetPetDAfterCnt(CPlayer* player, const CGUID& guid);

	/// 取得玩家最大可拥有侍从数目
	/// @return 玩家最大可拥有侍从数目
	long GetMaxServantNum(void);

	/// 根据强化类型取得强化材料列表
	/// @param  player		玩家
	/// @param  guid		宠物GUID
	/// @param  type		强化类型
	/// @param  goods		材料列表
	void GetMaterialList(CPlayer* player, const CGUID& guid, long type, vector<tagMaterial>& goods);

	/// 侍从经验获取
	/// @param  player		玩家
	/// @param  guid		宠物GUID
	/// @param  val			经验值
	void IncPetExp(CPlayer* player, const CGUID& guid, long val);

	/// 侍从修为经验获取
	/// @param  player		玩家
	/// @param  guid		宠物GUID
	/// @param  val			修为经验值
	void IncPetDExp(CPlayer* player, const CGUID& guid, long val);

	/// 以下脚本函数仅为测试使用
	/// 宠物升级
	/// @param  player		玩家
	/// @param  guid		宠物GUID
	/// @param  newLev		宠物新等级
	/// @return 宠物升级是否成功
	bool PetLevUp(CPlayer* player, const CGUID& guid, long newLev);

	/// 宠物道行升级
	/// 宠物重置天资
	/// 宠物后天天资强化
	/// 宠物品质强化
	/// 宠物

private:
	/// 设置WS存盘点
	void InformWS(CPlayer* pPlayer);

	/// 通知客户端在场景中添加宠物
	void InformClient(CPlayer* pPlayer, CPet* pPet, bool bAdd);

	/// 重置宠物
	bool ReSetPet(CPet* pPet, long lInitType, bool IsWild);

	/// 添加宠物
	void OnCreatePet(CPet* pPet);

	/// 根据GUID获取宠物
	CPet* GetPet(CPlayer* player, const CGUID& guid);

private:
	tagPetConf		m_ConfPet;									///< 宠物系统统一配置
	GROW_CONF		m_GrowConfPets;								///< 宠物成长配置
	PET_LIST		m_ConfPets;									///< 宠物列表配置
};