//.客户端宠物系统//
//	AddBy: Nikai	2008.11
#pragma once
#include "..\MoveShape.h"
#include "..\Player.h"
#include "..\App\petsystem\BasePetDef.h"

class CPet : public CMoveShape
{
public:
	//在战斗模式下的返回值
	enum eFightModeRet
	{
		FMR_NO_SKILL				= 0,	//没可能用技能
		FMR_SKILL_CD_NOT_ARRIVAL	= 1,	//技能冷却时间未到
		FMR_SKILL_NOT_ENOUGH_MP		= 2,	//技能所需MP不足
		FMR_SKILL_ACTION_NOT_FINISH = 3,	//技能动作没完成
		FMR_SKILL_IS_NOT_AUTO		= 4,	//技能不是自动释放
		FMR_SKILL_LEVEL_FAIL		= 5,	//当前等级某种条件失败
		FMR_Trace_Near				= 10,				//继续追踪 太近
		FMR_Trace_Far				= 11,				//继续追踪 太远
		FMR_Attack					= 12,				//攻击
		FMR_No_TARGET				= 13,				//没有攻击目标 或者攻击目标死亡
		FMR_SKILL_NOT_FINISH		= 14,	//技能没结束
		FMR_Other		,					//其他情况
	};
public:
	CPet();
	virtual ~CPet();

protected:
	//	对应的套接字
	long m_lSocketID;
	//	主人
	CPlayer* m_pMaster;
	//	场景
	CClientRegion* m_pRegion;

	CGUID m_MasterGuid;		//	主人guid
	long m_lPetType;		//	宠物类型
	bool m_bActive;			//	是否激活

	CGUID m_guidCurAtk;
	CMoveShape* m_pCurAtkShape;
	CMoveShape* m_pPassiveAtkShape;

	//	技能
	long m_lCurUseSkillPos;
	long m_lLastUseSkillPos;

	//	宠物属性
	tagPetDiff	* m_tagPetAttr;
	//	宠物逻辑
	tagPetDetail	* m_tagPetLogic;
	long m_lCurFightMode;

	map<long,tagPetSkill>	m_mapSkillList;

	//	移动
	float m_lStartAimX;		//	开始跟随的初始位置
	float m_lStartAimY;
	long m_lLastPosX;		//	上次坐标
	long m_lLastPosY;


public:
	long GetPetSocketID()			{ return m_lSocketID; }
	void SetPetSocketID(long lSocketID) { m_lSocketID = lSocketID; }

	const CGUID & GetMasterGuid()	{ return m_MasterGuid; }
	void SetMasterGuid(const CGUID& guid) { m_MasterGuid = guid; }

	CClientRegion* GetPetRegion()		{ return m_pRegion; }
	void SetPetRegion(CClientRegion* pRegion)	{ m_pRegion = pRegion; }

	CPlayer* GetMaster();
	void SetMaster(CPlayer* pMaster)	{ m_pMaster = pMaster; }

	void SetActive(bool b)		{ m_bActive = b; }
	bool IsActive()				{ return m_bActive; }
	long GetPetType()			{ return m_lPetType; }

	virtual bool DecodePetFromDataBlock(DBReadSet& setReadDB, bool bExData = true,CPlayer* pMaster = NULL,CClientRegion* pRegion = NULL);	//宠物编解码
	void DecordChangePropertyFromDataBlock(DBReadSet& setReadDB);		//更新宠物属性

	virtual void UpdateProperty(CMessage *pMsg);
	void SetPetAttibute(WORD wAttrType,long lData);
	//	进入场景
	void EnterRegion(bool bSelf = false );

	CMoveShape* GetCurAtkShape()	{ return m_pCurAtkShape; }
	CMoveShape* GetPassiveAtkShape(){ return m_pPassiveAtkShape; }
	const CGUID& GetAtkGuid()		{ return m_guidCurAtk; }

	void SetCurAtkShape(CMoveShape* pShape)  { m_pCurAtkShape = pShape; }
	void SetCurAtkShape(const CGUID& guid)	{ m_guidCurAtk = guid; }
	void SetPassiveAtkShape(CMoveShape* pShape) { m_pPassiveAtkShape = pShape; }


	//	--	移动
	//	自动寻路到目标（A*）
	long AutoMoveAI(long lPosX,long lPosY);
	//	移动到指定格子
	void MoveToCell(float fAimX,float fAimY);
	//	继续移动
	long ContinueMoveTo(long lAimX,long lAimY);
	void UpdateAimPos( CMoveShape* pAimShape );
	//	跟随目标
	long FollowObjectAI( CShape* pAimObject );
	long FollowObjectAI( long lAimX,long lAimY );

	virtual void AI();
	//	战斗AI
	long SkillAI();
	long PetFightAI();
	void AutoRetaliateAI();

	void UsePetSkill(long);

	void AddSkill(tagPetSkill& skillinfo);
};