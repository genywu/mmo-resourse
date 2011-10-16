#include "StdAfx.h"
#include "Pet.h"
#include "Player.h"
#include "Monster.h"
#include "AI/PetAI.h"
#include "AI/AIFactory.h"
#include "Goods/CGoods.h"
#include "../Public/StudySkillCondition.h"
#include "script/Script.h"
#include "script/VariableList.h"
#include "ServerRegion.h"


#ifdef _DEBUG
long g_lTimer = 0;
#endif

#ifdef _DEBUG
#define PRINT_TO_CLIENT(__strLog) \
	GetGame()->SendNotifyMessage(__strLog, m_HostID, NOTIFY_COLOR_RED); \
	PutoutLog("Pet", LT_ERROR, __strLog)

#else
#define PRINT_TO_CLIENT(__strLog) \
	PutoutLog("Pet", LT_ERROR, __strLog)
#endif

// 设置long型属性值
#define SET_LONG_PROP(PVAL, STRING, ENUM) \
	st.lType = 4; \
	st.pValue = (long*)(PVAL); \
	st.attrEnumValue = ENUM; \
	st.minValue = minValue[4]; \
	st.maxValue = maxValue[4]; \
	m_Attrs[STRING] = st

// 设置包括最大值和最小值的long型属性值
#define SET_LONG_PROP_EX(PVAL, STRING, ENUM, MIN, MAX) \
	st.lType = 4; \
	st.pValue = (long*)(PVAL); \
	st.attrEnumValue = ENUM; \
	st.minValue = (MIN); \
	st.maxValue = (MAX); \
	m_Attrs[STRING] = st

// 基本属性
#define SET_BASE_LONG_PROP(ATTR) \
	SET_LONG_PROP(&m_DiffAttrs.l##ATTR, #ATTR, PET_ATTR_##ATTR)

#define SET_BASE_LONG_PROP_EX(ATTR, MIN, MAX) \
	SET_LONG_PROP_EX(&m_DiffAttrs.l##ATTR, #ATTR, PET_ATTR_##ATTR, MIN, MAX)

// 临时属性
#define SET_TEMP_LONG_PROP(ATTR) \
	SET_LONG_PROP(&m_TempAttrs.l##ATTR, #ATTR, PET_ATTR_##ATTR)

// 基本+临时属性
#define SET_BASE_TEMP_LONG_PROP(ATTR) \
	SET_LONG_PROP(&m_BaseAttrs.l##ATTR, "B_"#ATTR, PET_ATTR_##ATTR); \
	SET_LONG_PROP(&m_TempAttrs.l##ATTR, "E_"#ATTR, PET_ATTR_##ATTR)

// 属性换算公式
#define CALC_EXCH_SET_ATTR(ATTRS, ATTRD) \
	SetAttr("E_"#ATTRD,  (m_BaseAttrs.l##ATTRS + m_TempAttrs.l##ATTRS) * UNI_CONF.f##ATTRS##2##ATTRD);

// 属性换算公式
#define CALC_EXCH_CHG_ATTR(ATTRS, ATTRD) \
	ChgAttr("E_"#ATTRD,  (m_BaseAttrs.l##ATTRS + m_TempAttrs.l##ATTRS) * UNI_CONF.f##ATTRS##2##ATTRD);

CPet::CPet(void)
: m_lAICount(0)
, m_dwMoveValiStartTime(timeGetTime())
, m_dwLastRecvMsgTime(m_dwMoveValiStartTime)
, m_fMaxPeriodMoveSpeed(0.0f)
, m_guMoveValiStartRgnID(NULL_GUID)
, m_lMoveValiStartPosX(0)
, m_lMoveValiStartPosY(0)
, m_fMoveValiStartSpeed(0.0f)
, m_fMoveValiTotalDistance(0.0f)
, m_fMoveValiRecvMsgCount(0.0f)
, m_pHost(NULL)
{
	m_pVariableList = MP_NEW CVariableList;
	m_pAI = CAIFactory::CreateAI(CAIFactory::AI_TYPE_PET);
	m_pAI->SetOwner(this);
	SetType(TYPE_PET);
	static long l = 0;
	char szLog[128];
	_snprintf(szLog, 128, "%ld Constructor.", ++l);
	PutoutLog("PetCD", LT_DEFAULT, szLog);
}

CPet::~CPet(void)
{
	if( IsHangUp() )
	{
		DelFromRegion(PET_OT_DISAPPEAR);
	}

	DelectAllScripByShape(GetExID());

	CancelAllTimer();
	ClearActiveSkills();
	ClearPassiveSkill();
	ClearAllBuff();
	CAIFactory::DeleteAI(m_pAI);
	MP_DELETE(m_pVariableList);

	static long l = 0;
	char szLog[128];
	_snprintf(szLog, 128, "%ld Destructor.", --l);
	PutoutLog("PetCD", LT_DEFAULT, szLog);
}

// 网络消息响应
void CPet::OnMessage(CMessage* pMsg)
{
}

// 超时响应
long CPet::OnTimeOut(DWORD timerid,
					 DWORD curtime,
					 const void* var)
{
	const tagTimerVar* pTimerVar = (const tagTimerVar*)var;

#ifdef _RUNSTACKINFO_
	char szInfo[256]="";
	_snprintf(szInfo,256,"CPlayer::OnTimeOut(TimerType:%d,Name:%s) Start",
		pTimerVar?pTimerVar->TimerType:0,
		GetName());
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szInfo);
#endif

	// 1) 战宠, 召唤兽的的HP
	// 2) 精灵, 战宠, 召唤兽的MP
	// 3) 精灵, 战宠的饱食度
	// 4) 战宠的忠诚度
	// 5) 召唤兽的生存时间

	if( IsDied() || !IsActive() )
	{
		CMoveShape::OnTimeOut(timerid, curtime, var);
		return 0;
	}

	switch( pTimerVar->TimerType )
	{
	case COT_PET_EVENT:
		{
			switch(pTimerVar->lvar)
			{
			case PET_HPRenew:
				{
					OnHPRenew();
				}
				break;

			case PET_MPRenew:
				{
					OnMPRenew();
				}
				break;

			case PET_GorgeRenew:
				{
					OnGorgeRenew();
				}
				break;

			case PET_LoyaltyRenew:
				{
					OnLoyaltyRenew();
				}
				break;

			case PET_SearchEnemy:
				{
					
					if( GetTarget() == NULL )
					{
						m_pAI->AddAIEvent(CBaseAI::AET_MOVE);
					}
				}
				break;

			case PET_LifeTimeout:
				{
					OnLifeEnd();
				}
				break;
			}
		}
		break;

	default:
		{
			CMoveShape::OnTimeOut(timerid, curtime, var);
		}
		break;
	}

	return 0;
}

// 处理实体间消息函数
long CPet::Proc(eEventType type,
				const tagProcVar* pVar)
{
	return 0;
}

// 添加一个移动消息
void CPet::AddMoveMsg(float fNewX, float fNewY)
{
	float fXOffset = fNewX - GetPosX();
	float fYOffset = fNewY - GetPosY();
	float fDis = sqrt( fYOffset * fYOffset + fXOffset * fXOffset );

	m_fMoveValiTotalDistance += fDis;
	// 如果总共收到一定个数消息以后，进行验证。
	++m_fMoveValiRecvMsgCount;

	DWORD dwCurTime = timeGetTime();
	float fMoveSpeed = fDis/(float)(dwCurTime-m_dwLastRecvMsgTime);
	if( fMoveSpeed > m_fMaxPeriodMoveSpeed )
		m_fMaxPeriodMoveSpeed = fMoveSpeed;
	m_dwLastRecvMsgTime = dwCurTime;
}

// 验证每一步的移动
bool CPet::CheckMoveStep(float fCurX, float fCurY)
{
	CServerRegion *pRegion = (CServerRegion*)GetFather();
	if( NULL == pRegion ||
		pRegion->GetType() != TYPE_REGION )
		return false;

	long lSX = GetTileX();
	long lSY = GetTileY();
	int nMaxDis = max((abs(lSX-(long)fCurX)), (abs(lSY-(long)fCurY)));
	if( nMaxDis > 2 )
		return false;

	// 判断阻挡
	BYTE byBlock = pRegion->GetBlock(fCurX,fCurY);	
	if( byBlock != CRegion::BLOCK_NO &&
		byBlock != CRegion::BLOCK_AIM )
		return false;

	return true;
}

// 移动速度验证
bool CPet::CheckMoveSpeedValidate(void)
{
	// 为了验证的有效性, 时间大于等于1秒才验证
	DWORD dwTimeDif = timeGetTime() - m_dwMoveValiStartTime;
	float fTotalMoveDis = (float)dwTimeDif * m_fMoveValiStartSpeed;

	// 估计应该接受消息的个数
	long nMsgCount = (long)(m_fMoveValiTotalDistance * 2.3f);
	float fAverSpeed = m_fMoveValiStartSpeed;
	if( dwTimeDif != 0 )
	{
		fAverSpeed = m_fMoveValiTotalDistance / dwTimeDif;
	}

	if( (fAverSpeed-m_fMoveValiStartSpeed) >= 0.0005f &&
		(m_fMaxPeriodMoveSpeed-m_fMoveValiStartSpeed) > 0.0008 )
	{
		/*char strInfo[256] = "";
		_snprintf(strInfo, 256,
			"PetMoveError,TimeDif:%d,TotalTimeDis:%f,RealMoveDis:%f",
			dwTimeDif, fTotalMoveDis, m_fMoveValiTotalDistance);
		PutStringToFile("MoveTest", strInfo);
		PutoutLog("Pet", LT_ERROR, strInfo);*/

		// 如果在同一个场景，则设为验证的初始位置
		//if( m_guMoveValiStartRgnID == GetRegionGUID() )
		//SetPosition(m_lMoveValiStartPosX, m_lMoveValiStartPosY);
		//else		// 进行踢下线处理
		//{
		//	GetGame()->KickPlayer(m_HostID);
		//	return false;
		//}
	}

	ResetMoveValiData();
	return true;
}

// 重设相关数据
void CPet::ResetMoveValiData(bool bAll)
{
	m_dwMoveValiStartTime		= timeGetTime();
	m_dwLastRecvMsgTime			= m_dwMoveValiStartTime;
	m_fMaxPeriodMoveSpeed		= 0.0f;
	m_guMoveValiStartRgnID		= GetRegionGUID();
	m_lMoveValiStartPosX		= GetTileX();		// 开始移动的X位置
	m_lMoveValiStartPosY		= GetTileY();		// 开始移动的Y位置
	m_fMoveValiStartSpeed		= GetSpeed();
	m_fMoveValiTotalDistance	= 0.0f;				// 移动的总距离
	m_fMoveValiRecvMsgCount		= 0;				// 收到的移动消息个数
}

// 宠物移动
void CPet::OnQuestMoveStep(float fCurX,
						   float fCurY,
						   float fDestX,
						   float fDestY)
{
	if ( !GetInstruction() )	// 不能使用玩家使用指令时直返回
		return;

	if( ModifyPos(IT_SAME_RGN) )
	{
		return;
	}

	CBaseAI* pAI = GetAI();
	if ( !CanMove() ||
		!CheckMoveStep(fCurX, fCurY) ||
		NULL == pAI )
	{
		OnCannotMove();
		return;
	}

	// 添加一个验证消息数据
	AddMoveMsg(fCurX, fCurY);

	long lDir = GetLineDir(GetTileX(), GetTileY(), fCurX, fCurY);
	SetDir(lDir);
	SetPosXY(fCurX, fCurY);
	if( pAI->MoveTo(fDestX, fDestY) )
	{
		if( m_fMoveValiRecvMsgCount > MoveValiMaxMsgCount )
		{
			CheckMoveSpeedValidate();
		}
	}
}

// 宠物停止移动
void CPet::OnQuestStopMove(float fCurX,
						   float fCurY,
						   short lDir)
{
	if( !CheckMoveStep(fCurX, fCurY) )
	{
		OnCannotMove();
		return;
	}

	// 添加一个验证消息数据
	AddMoveMsg(fCurX, fCurY);
	SetDir(lDir);
	SetPosXY(fCurX, fCurY);	
	OnStopMove();
	ResetMoveValiData();
}

void CPet::OnCannotMove(void)
{
	CMessage msg( MSG_S2C_SHAPE_SETPOS );
	msg.Add( GetType() );
	msg.Add( GetExID());
	msg.Add( static_cast<LONG>(GetPosX()) );
	msg.Add( static_cast<LONG>(GetPosY()) );
	msg.Add( static_cast<LONG>(0) );
	msg.SendToPlayer(m_HostID);
}

// 宠物受到攻击
long CPet::OnBeenAttacked(CMoveShape* pAttacker, long lHp)
{
	return CMoveShape::OnBeenAttacked(pAttacker, lHp);
}

// 5) 宠物死亡
void CPet::OnDied(void)
{
	char szLog[512];
	char szGUID[64];
	GetExID().tostring(szGUID);
	_snprintf(szLog, 512, "Player%s 's Pet(%s: %s) will be delete.",
		m_pHost->GetName(),
		GetName(),
		szGUID);
	PutoutLog("PetCD", LT_DEFAULT, szLog);

	long lOpType = 0;
	if( GetPetType() == PET_TYPE_TRAP )
	{
		GetInst(CPetCtrl).DelPet(this, PET_OT_TRAP_END);
	}
	else
	{
		GetInst(CPetCtrl).DelPet(this, PET_OT_BEEN_KILLED);
	}
}

// 当速度发生改变以后触发
void CPet::OnSpeedChanged(void)
{
	ResetMoveValiData();
}

void CPet::SetPosition(LONG lX, LONG lY)
{
	if( GetTileX() != lX || GetTileY() != lY )
	{
		CMoveShape::SetPosition(lX,lY);
		ResetMoveValiData(true);
	}
}

void CPet::SetSlipPos(LONG lX, LONG lY, float fSpeed, float fDir)
{
	CMoveShape::SetSlipPos(lX, lY, fSpeed, fDir);
	ResetMoveValiData();
}

// 可否被攻击
bool CPet::IsAttackAble(CMoveShape* pAttacker)
{
	bool bRes = false;
	if( SAT_CAN_BEATK != CanBeAttacked() )
	{
		bRes = m_pHost->IsAttackAble(pAttacker);
	}

	return bRes;
}

// 更新状态
void CPet::UpdateCurrentState(void)
{
}

// 改变状态
void CPet::ChangeStateTo(CShape::eSTATE eState)
{
	if( !IsActive() || GetState() == eState ) return;

	switch( eState )
	{
	case STATE_FIGHT:
		{
			SetAttr("E_HpRecoverSpeed", -GetAttr("B_HpRecoverSpeed"));
			SetAttr("E_MpRecoverSpeed", -GetAttr("B_MpRecoverSpeed"));
		}
		break;

	case STATE_PEACE:
		{
			m_pAI->SetTarget(0, NULL_GUID);

			CALC_EXCH_SET_ATTR(Consti, HpRecoverSpeed);
			CALC_EXCH_SET_ATTR(Wisdom, MpRecoverSpeed);
		}
		break;
	}

	CMoveShape::ChangeStateTo(eState);
	if( eState != m_pHost->GetState() &&
		(STATE_FIGHT == eState ||
		STATE_PEACE == eState) )
	{
		m_pHost->ChangeStateTo(eState);
		UpdateAttribute(TRUE);
	}
}

bool CPet::ChangeAttribute(std::string& attrName,
						   long value)
{
	long lVal = ChgAttr(attrName.c_str(), value);
	if( INVILID_VALUE == lVal )
		return false;
	return true;
}

bool CPet::SetAttribute(std::string& attrName,
						long value)
{
	long lVal = SetAttr(attrName.c_str(), value);
	if( INVILID_VALUE == lVal )
		return false;
	return true;
}

long CPet::GetAttribute(std::string attrName)
{
	return GetAttr(attrName.c_str());
}

// 更新属性
long CPet::UpdateAttributeToClient(bool bAll)
{
	WORD wAttrNum = 0;
	// 向主人更新详细属性
	if( bAll && (!m_DirtyAttrs.empty() ) )
	{
		CMessage msg(MSG_S2C_SHAPE_CHANGE_PROPERTIES);
		msg.Add(GetType());
		msg.Add(GetExID());
		DBWriteSet setWriteDB;
		msg.GetDBWriteSet(setWriteDB);
		wAttrNum = m_DirtyAttrs.size();
		setWriteDB.AddToByteArray(wAttrNum);
		if( wAttrNum > 0 )
		{
			CodeChgAttrs(setWriteDB, m_DirtyAttrs);
		}
		msg.SendToPlayer(m_pHost->GetExID(), false);
		m_DirtyAttrs.clear();
	}

	// 向周围玩家更新基本属性
	if( !bAll && !m_BaseDirtyAttrs.empty() )
	{
		CMessage msg(MSG_S2C_SHAPE_CHANGE_PROPERTIES);
		msg.Add(GetType());
		msg.Add(GetExID());
		DBWriteSet setWriteDB;
		msg.GetDBWriteSet(setWriteDB);

		wAttrNum = m_BaseDirtyAttrs.size();
		setWriteDB.AddToByteArray(wAttrNum);
		if( wAttrNum > 0 )
		{
			CodeChgAttrs(setWriteDB, m_BaseDirtyAttrs);
		}
		msg.SendToPlayers(GetSelectSelfPlayer(), true);
		m_BaseDirtyAttrs.clear();
	}

	return true;
}

long  CPet::UpdateAttribute(BOOL bImmediately)
{
	UpdateAttributeToClient(true);	
	if( !m_BaseDirtyAttrs.empty() )
	{
		RegisterUpdateAttrTimer();
	}
	return true;
}

long  CPet::OnUpdateAttributeTimeOut()
{
	UpdateAttributeToClient(false);
	return true;
}

// 初始化临时属性
void CPet::InitialTempAttrs(void)
{
	m_TempAttrs = tagPetTemp();

	if( NeedBeSaved() )
	{
		m_TempAttrs.cSaveFlag = 1;
		if( PET_TYPE_ELF == GetPetType() ||
			PET_TYPE_HUNTER == GetPetType() )
		{
			SetAttr("MaxExp", UNI_CONF.Exps[m_DiffAttrs.lLevel]);
			SetAttr("Gorge", m_DiffAttrs.lGorge);
			SetAttr("Loyalty", m_DiffAttrs.lLoyalty);
			SetAttr("Exp", m_DiffAttrs.lExp);
			CalcGorgeLevel();
		}
	}

	ITR_SKILL_LIST itr = m_Skills.begin();
	for( ; itr != m_Skills.end(); ++itr )
	{
		if( PET_SKILL_GENIUS == itr->second.lType )
		{
			++m_TempAttrs.lGeniusSkillNum;
		}
		else if( PET_SKILL_NULL != itr->second.lType )
		{
			++m_TempAttrs.lStudySkillNum;
		}
	}

	GetInst(CPetCtrl).GetGrowCoefs(m_DiffAttrs.lIndex, m_GrowCoefs);

	SetAttr("Country", m_TempAttrs.lCountry);
	long lHostLevel = m_pHost->GetLevel();
	long lPetType = GetPetType();
	if( PET_TYPE_CONJURED ==lPetType || PET_TYPE_TRAP == lPetType || PET_TYPE_ORNA == lPetType )
	{
		SetAttr("Level", lHostLevel);
		InitialOrigAttrs(1, lHostLevel);
	}
	else
	{
		SetAttr("Level", m_DiffAttrs.lLevel);
		InitialOrigAttrs(1, m_DiffAttrs.lLevel);
	}

	if( PET_TYPE_TRAP == lPetType )
	{
		SetAttr("FightType", PET_AS_ACTIVE);
		SetAttr("MoveType", PET_AS_STOP);
	}
	else
	{
		SetAttr("FightType", PET_AS_DEFEND);
		SetAttr("MoveType", PET_AS_FOLLOW);
	}
}

// 初始化原始属性和对应属性
void CPet::InitialOrigAttrs(long lOldLevel, long lCurLevel)
{
	if( m_GrowCoefs.empty() ) return;

	// 原始属性计算公式
#define CALC_LEVEL_TEMP_ATTR(ATTR, LEV_START, LEV_END) \
	m_TempAttrs.l##ATTR += m_BaseAttrs.l##ATTR * (LEV_END - LEV_START) * pCoefs->f##ATTR

	long lID = 0;
	tagPetOrig* pCoefs = NULL;
	long lLevelStart = 0;
	long lLevelEnd = 0;

#define CALC_TEMP_ATTR(ATTR) \
	lID = 0; \
	pCoefs = &m_GrowCoefs[lID]; \
	lLevelStart = lOldLevel; \
	lLevelEnd = pCoefs->lLevel; \
	while( lCurLevel > lLevelEnd  && lID < m_GrowCoefs.size() ) \
	{ \
	if( lLevelStart < lLevelEnd ) \
	{ \
	CALC_LEVEL_TEMP_ATTR(ATTR, lLevelStart, lLevelEnd); \
	lLevelStart = lLevelEnd; \
	} \
	pCoefs = &m_GrowCoefs[++lID]; \
	lLevelEnd = pCoefs->lLevel; \
	} \
	if( lCurLevel > lLevelStart ) \
	{ \
	CALC_LEVEL_TEMP_ATTR(ATTR, lLevelStart, lCurLevel); \
	} \
	SetAttr("E_"#ATTR, m_TempAttrs.l##ATTR)

	CALC_TEMP_ATTR(MaxHp);
	CALC_TEMP_ATTR(MaxMp);
	CALC_TEMP_ATTR(Strenth);
	CALC_TEMP_ATTR(Agility);
	CALC_TEMP_ATTR(Consti);
	CALC_TEMP_ATTR(Intell);
	CALC_TEMP_ATTR(Wisdom);
	CALC_TEMP_ATTR(Spirit);
	CALC_TEMP_ATTR(Charm);
	CALC_TEMP_ATTR(Luck);

	CALC_EXCH_SET_ATTR(Consti, MaxHp);
	CALC_EXCH_SET_ATTR(Consti, HpRecoverSpeed);
	CALC_EXCH_SET_ATTR(Consti, Def);
	CALC_EXCH_SET_ATTR(Consti, CriDef);
	CALC_EXCH_SET_ATTR(Wisdom, MaxMp);
	CALC_EXCH_SET_ATTR(Wisdom, MpRecoverSpeed);
	CALC_EXCH_SET_ATTR(Wisdom, Absorb);
	CALC_EXCH_SET_ATTR(Wisdom, MDef);
	CALC_EXCH_SET_ATTR(Strenth, Block);
	CALC_EXCH_SET_ATTR(Strenth, MinAtk);
	CALC_EXCH_SET_ATTR(Strenth, MaxAtk);
	CALC_EXCH_CHG_ATTR(Agility, MinAtk);
	CALC_EXCH_CHG_ATTR(Agility, MaxAtk);
	CALC_EXCH_SET_ATTR(Strenth, Cri);
	CALC_EXCH_SET_ATTR(Strenth, Parry);
	CALC_EXCH_CHG_ATTR(Agility, Parry);
	CALC_EXCH_SET_ATTR(Agility, Hit);
	CALC_EXCH_SET_ATTR(Agility, Dodge);
	CALC_EXCH_SET_ATTR(Intell, MAtk);
	CALC_EXCH_SET_ATTR(Intell, MCri);
	CALC_EXCH_SET_ATTR(Spirit, WillDef);
	CALC_EXCH_SET_ATTR(Spirit, MCriDef);

	if( 0 == GetAttr("Hp") ||
		lOldLevel != lCurLevel )
	{
		SetAttr("Hp", GetMaxHP());
		SetAttr("Mp", GetMaxMP());
	}
	else
	{
		SetAttr("Hp", m_DiffAttrs.lHp);
		SetAttr("Mp", m_DiffAttrs.lMp);
	}

	if( STATE_FIGHT == GetState() )
	{
		SetAttr("E_HpRecoverSpeed", -GetAttr("B_HpRecoverSpeed"));
		SetAttr("E_MpRecoverSpeed", -GetAttr("B_MpRecoverSpeed"));
	}
}

// 初始化激活宠物基本属性
bool CPet::InitialBaseAttrs(const tagPetBase& rPet, long lInitType)
{
	m_BaseAttrs = rPet;
	SetGraphicsID(rPet.lPicID);

	if( SAT_CAN_BEATK != CanBeAttacked() )
	{
		God();
	}

	if( rPet.lInvisible > 0 )
	{
		SetInvisibleState(false);
	}

	if( 0 == rPet.lMoveType )
	{
		SetMoveable(FALSE);
	}

	long lPetType = GetPetType();
	switch( lInitType )
	{
	case IT_CREATE:
		{
			InitialSkills();
			strcpy_s(m_DiffAttrs.szName, 32, rPet.szOriName);
			SetActiveAttr(1);
			strcpy_s(m_DiffAttrs.szHostName, 32, m_pHost->GetName());

			SYSTEMTIME stTime;
			GetLocalTime(&stTime);
			m_DiffAttrs.tBirthday[0] = stTime.wYear;
			m_DiffAttrs.tBirthday[1] = stTime.wMonth;
			m_DiffAttrs.tBirthday[2] = stTime.wDay;
			m_DiffAttrs.tBirthday[3] = stTime.wHour;
			m_DiffAttrs.tBirthday[4] = stTime.wMinute;
			m_DiffAttrs.tBirthday[5] = stTime.wSecond;
			m_DiffAttrs.lLevel	= 1;
		}

	case IT_RELIVE:
		{
			switch( GetPetType() )
			{
			case PET_TYPE_ELF:
			case PET_TYPE_HUNTER:
				{
					m_DiffAttrs.lGorge		= m_BaseAttrs.lGorge;
					m_DiffAttrs.lLoyalty	= m_BaseAttrs.lLoyalty;
				}
				break;

			case PET_TYPE_CONJURED:
			case PET_TYPE_TRAP:
				{
					m_DiffAttrs.lGorge	= timeGetTime() + m_BaseAttrs.lExistTime;
				}
				break;
			}
		}
	}

	SetActiveAttr(m_DiffAttrs.lActive);
	if( rPet.lExistTime != -1 && m_DiffAttrs.lGorge < timeGetTime() )
	{
		return false;
	}
	SetName(m_DiffAttrs.szName);

	return true;
}

// 初始化宠物的存盘或切服的保存属性
bool CPet::InitialDiffAttrs(long lInitType)
{
	C_PET_LIST rPets = GetInst(CPetCtrl).GetPetsConfig();
	C_ITR_PET_LIST itrPet = rPets.find(m_DiffAttrs.lIndex);
	bool bRes = false;
	if( itrPet != rPets.end() &&
		InitialBaseAttrs(itrPet->second, lInitType) )
	{
		bRes = true;
	}

	return bRes;
}

// 通过怪物初始化激活
bool CPet::InitialByTarget(CMonster* pTarget)
{
	m_fPosX = pTarget->GetPosX();
	m_fPosY = pTarget->GetPosY();

	m_DiffAttrs.lLevel = pTarget->GetLevel();
	return true;
}

void CPet::SetHost(CPlayer* pPlayer)
{
	if( NULL == pPlayer )
	{
		m_HostID = NULL_GUID;
	}
	else
	{
		m_HostID = pPlayer->GetExID();
	}
	m_pHost = pPlayer;
}

// 添加到场景中
bool CPet::AddToRegion(void)
{
	bool bRes = false;
	CServerRegion* pRgn = dynamic_cast<CServerRegion*>(m_pHost->GetFather());
	if( pRgn != NULL )
	{
		SetRegionGUID(pRgn->GetExID());
		SetFather(pRgn);
		SetPosition(m_pHost->GetTileX(), m_pHost->GetTileY());
		pRgn->AddObject(this);
		pRgn->AddShapeToAround(this);
	}

	return bRes;
}

// 從场景中刪除
bool CPet::DelFromRegion(long lOpType)
{
	if( lOpType != PET_OT_CHANGE_REGION && !m_pAI->IsHibernated() )
	{
		m_pAI->Hibernate();
		m_pAI->DeleteAIEvent(CBaseAI::AET_MOVE);
	}

	CServerRegion* pRgn = dynamic_cast<CServerRegion*>(GetFather());
	if (pRgn != NULL)
	{
		pRgn->DelShapeToAround(this);
		if (m_pHost != NULL &&
			pRgn == m_pHost->GetFather())
		{
			pRgn->DelShapeToPlayer(this, m_HostID);
		}
		pRgn->DeleteChildObject(this);
	}

	SetState(STATE_DELET);
	return true;
}

// 通过主人初始化激活
bool CPet::InitialByHost(INIT_TYPE lInitType)
{
	CBaseObject* pRgn = NULL;
	switch(lInitType)
	{
	case IT_SAME_GS:		// 同一GS切换
		{
			DelFromRegion(PET_OT_CHANGE_REGION);
			AddToRegion();
		}
		break;

	case IT_LOGIN_GS:		// 不同GS切换(登录游戏)
		{
			AddToRegion();
		}
		break;

	case IT_SAME_RGN:		// 同一场景切换
		{
			SetPosition(m_pHost->GetTileX(), m_pHost->GetTileY());
		}
		break;
	}

	m_TempAttrs.lCountry = m_pHost->GetCountry();

	if( m_BaseAttrs.lMoveType != 0 )
	{
		SetSpeed(m_pHost->GetSpeed());
	}

	return true;
}

// 初始化AI
void CPet::InitAI(void)
{
	CancelAllTimer();
	((CPetAI*)m_pAI)->SetHost(m_pHost);
	m_pAI->SetTarget(0, NULL_GUID);
}

// 初始化技能
void CPet::InitSkills(void)
{
	ITR_SKILL_LIST itr = m_Skills.begin();
	for( ; itr != m_Skills.end(); ++itr )
	{
		OnBeginSkill(itr->second.lID, itr->second.lLevel);
	}
}

// 事件注册
inline void CPet::RegEvent(PET_EVENT_TYPE eType,
						   long lSpringTime,
						   long lIntervalTime)
{
	lSpringTime += timeGetTime();
	tagTimerVar* pTimer = CreateTimerVar(1);
	pTimer->TimerType = COT_PET_EVENT;
	pTimer->lvar = eType;
	m_Timers[eType] = TIMER_SCHEDULE(this,
		(void*)pTimer,
		lSpringTime,
		lIntervalTime);
}

// 注册渐变事件: 随时间有所损益的所有宠物属性, 目前已知:
// 1) 战宠, 召唤兽的的HP
// 2) 精灵, 战宠, 召唤兽的MP
// 3) 召唤兽的生存时间
// 4) 战宠的忠诚度
// 5) 精灵, 战宠的饱食度
bool CPet::RegShadeEvent(void)
{
	long lType = GetPetType();
	if( lType != PET_TYPE_TRAP )
	{
		RegEvent(PET_HPRenew, UNI_CONF.nHPMPInt, UNI_CONF.nHPMPInt);				// 宠物刷新HP
	}

	if( lType != PET_TYPE_TRAP )
	{
		RegEvent(PET_MPRenew, UNI_CONF.nHPMPInt, UNI_CONF.nHPMPInt);				// 宠物刷新MP
	}
	else
	{
		RegEvent(PET_SearchEnemy, UNI_CONF.nAISpringTime, UNI_CONF.nAISpringTime);	// 陷阱周期寻敌
	}

	if( PET_TYPE_ELF == lType ||
		PET_TYPE_HUNTER == lType )
	{
		RegEvent(PET_GorgeRenew, UNI_CONF.nGorgeInt, UNI_CONF.nGorgeInt);			// 宠物刷新饱食度
		RegEvent(PET_LoyaltyRenew, UNI_CONF.nLoyaltyInt, UNI_CONF.nLoyaltyInt);		// 宠物刷新忠诚度
	}

	if( m_BaseAttrs.lExistTime != -1 )
	{
#ifdef _DEBUG
		g_lTimer = m_BaseAttrs.lExistTime % 1000;
#endif
		RegEvent(PET_LifeTimeout, m_BaseAttrs.lExistTime);							// 宠物生命终结
	}

	return true;
}

// 设置宠物当前目标
void CPet::SetTarget(CMoveShape* pTarget)
{
	if( m_pAI != NULL )
	{
		if( pTarget != NULL )
		{
			m_pAI->SetTarget(pTarget->GetType(), pTarget->GetExID());
		}
		else
		{
			m_pAI->SetTarget(0, NULL_GUID);
		}
	}
}

// 保存客户端技能设置
void CPet::SaveSkillSetting(long lPos, long lActPos, bool bAuto)
{
	tagSkill* pSkill = GetSkill(lPos);
	if (pSkill != NULL)
	{
		pSkill->lActPos = lActPos;
		pSkill->bAuto = bAuto;
	}
}

// 返回宠物当前目标
const CMoveShape* CPet::GetTarget(void)
{
	if( m_pAI != NULL )
	{
		return m_pAI->GetTarget();
	}

	return NULL;
}

// 1) 战宠, 召唤兽的的HP
void CPet::OnHPRenew(void)
{
	long lCurHP = GetHP();
	long lMaxHP = GetMaxHP();
	long lHpRecoverSpeed = GetAttr("C_HpRecoverSpeed");
	if( lCurHP == lMaxHP && lHpRecoverSpeed > 0 ||
		0 == lHpRecoverSpeed )
		return;

	lCurHP += lHpRecoverSpeed;
	if( lCurHP > lMaxHP )
	{
		lCurHP = lMaxHP;
	}
	else if( lCurHP <= 0 )
	{
		lCurHP = 0;
		char szLog[512];
		char szGUID[64];
		GetExID().tostring(szGUID);
		_snprintf(szLog, 512, "Player%s 's Pet(%s: %s) HP == 0.",
			m_pHost->GetName(),
			GetName(),
			szGUID);
		PutoutLog("PetCD", LT_DEFAULT, szLog);

		OnDied();	// 触发死亡
	}

	SetAttr("Hp", lCurHP);
}

// 2) 精灵, 战宠, 召唤兽的MP
void CPet::OnMPRenew(void)
{
	long lCurMP = GetMP();
	long lMaxMP = GetMaxMP();
	long lMpRecoverSpeed = GetAttr("C_MpRecoverSpeed");
	if( lCurMP == lMaxMP && lMpRecoverSpeed >= 0 ||
		0 == lMpRecoverSpeed )
		return;

	lCurMP += lMpRecoverSpeed;
	if( lCurMP > lMaxMP )
	{
		lCurMP = lMaxMP;
	}
	else if( lCurMP < 0 )
	{
		lCurMP = 0;
	}

	SetAttr("Mp", lCurMP);
}

// 3) 战宠的忠诚度
void CPet::OnLoyaltyRenew(void)
{
}

// 4) 精灵, 战宠的饱食度
void CPet::OnGorgeRenew(void)
{
	long lGorge = m_DiffAttrs.lGorge;
	long lMaxGorge = UNI_CONF.nMaxGorge;
	if( lGorge > lMaxGorge )
	{
		lGorge = lMaxGorge;
		return;
	}

	long lGorgeRenew = GetAttr("C_GorgeRenew");
	lGorge += lGorgeRenew;
	if( lGorge > lMaxGorge )
	{
		lGorge = lMaxGorge;
	}
	else if( lGorge <= 0 )
	{
		char szLog[512];
		char szGUID[64];
		GetExID().tostring(szGUID);
		_snprintf(szLog, 512, "Player%s 's Pet(%s: %s) Gorge == 0.",
			m_pHost->GetName(),
			GetName(),
			szGUID);
		PutoutLog("PetCD", LT_DEFAULT, szLog);

		GetInst(CPetCtrl).DelPet(this, PET_OT_DETECT);	// 触发叛离
		return;
	}


	SetAttr("Gorge", lGorge);

	// 饱食度升级, 处理对应影响属性
	long lOld = m_TempAttrs.lGorgeLevel;
	long lNew = CalcGorgeLevel();

	if( lOld != lNew )
	{
		OnGorgeUpdate(lOld);
	}
}

void CPet::OnGorgeUpdate(long lOld)
{
	//const tagGorge& rGorgeEff = UNI_CONF.GorgeEffs[m_TempAttrs.lGorgeLevel];
	//long lVal = rGorgeEff.nLoyalty;
	//ChgAttr("E_LoyaltyRenew", lVal);
	//lVal = GetAttr("B_HpRecoverSpeed") * rGorgeEff.fHPMP;
	//ChgAttr("E_HpRecoverSpeed", lVal);
	//lVal = GetAttr("B_MpRecoverSpeed") * rGorgeEff.fHPMP;
	//SetAttr("E_MpRecoverSpeed", lVal);
}

// 5) 宠物生命终结
void CPet::OnLifeEnd(void)
{
	switch( GetPetType() )
	{
	case PET_TYPE_CONJURED:
		{
			GetInst(CPetCtrl).DelPet(this, PET_OT_HOUSE);
		}
		break;

	case PET_TYPE_TRAP:
		{
			//long lSkillID = m_BaseAttrs.GeniusSkills[0].lID;
			//long lSkillLevel = m_BaseAttrs.GeniusSkills[0].lID;
			//ITR_SKILL_LIST itr = m_Skills.find(0);
			//if( itr != m_Skills.end() && UseSkill(&itr->second) )
			//{
			//	m_pAI->Attack(itr->second.lID);
			//}
			WORD s = GetState();
			if( s != STATE_DIED &&
				s != STATE_DELET )
			{
				GetInst(CPetCtrl).DelPet(this, PET_OT_TRAP_END);
			}
		}
		break;
	}
}

// 激活宠物
void CPet::OnActive(void)
{
	InitAI();
	InitNameValueMap();
	InitialTempAttrs();
	ChangeStateTo(STATE_PEACE);
	InitSkills();
	RegShadeEvent();
}

// 收藏宠物
void CPet::OnHouse(long lOpType)
{
	// 宠物下线或消失时仍然为激活状态
	if( lOpType != PET_OT_PLAYER_EXIT )
	{
		SetActiveAttr(0);
		CancelAllTimer();
	}
	else
	{
		ChangeStateTo(STATE_HANGUP);
	}

	StopAllSkills();
	StopPassiveSkills();

	//注销属性更新定时器
	CancelUpdateAttrTimer();

	if (lOpType != PET_OT_BEEN_KILLED)
	{
		DelFromRegion(lOpType);
	}
}

// 宠物升级
void CPet::OnLevelUpdate(void)
{
	long lSkillID = 0;
	if( PET_TYPE_ELF == GetPetType() ||
		PET_TYPE_HUNTER == GetPetType() )
	{
		for( int i=m_TempAttrs.lGeniusSkillNum; i <= UNI_CONF.lGeniusSkillNum; ++i )
		{
			const tagSkillOpen& cond = UNI_CONF.GeniusSkillOpen[i];
			if( cond.nLevel == m_DiffAttrs.lLevel )
			{
				lSkillID = AddGeniusSkill();
				// 追加技能失败
				if( 0 == lSkillID )
				{
					char szLog[512];
					_snprintf(szLog, 512, "宠物[%s]升级(%ld)追加天赋技能失败.",
						GetName(), GetLevel());
					PutoutLog("Pet", LT_ERROR, szLog);
				}
				break;
			}
		}

		for( int i=m_TempAttrs.lStudySkillNum; i <= UNI_CONF.lStudySkillNum; ++i )
		{
			const tagSkillOpen& cond = UNI_CONF.StudySkillOpen[i];
			if( cond.nLevel == m_DiffAttrs.lLevel )
			{
				lSkillID = AddStudySkill();
				// 开启技能失败
				if( 0 == lSkillID )
				{
					char szLog[512];
					_snprintf(szLog, 512, "宠物[%s]升级(%ld)追加插槽技能失败.",
						GetName(), GetLevel());
					PutoutLog("Pet", LT_ERROR, szLog);
				}
				break;
			}
		}
	}
}

void CPet::SetActiveAttr(long lActiveType)
{
	if( 0 == lActiveType )
	{
		if( IsActive() )
		{
			m_pHost->DelActivePet(GetPetType());
		}
		m_DiffAttrs.lActive = lActiveType;
	}
	else if( lActiveType != 0 )
	{
		m_pHost->AddActivePet(this);
	}
}

void CPet::CancelAllTimer(void)
{
	map<long, long>::iterator itr = m_Timers.begin();
	for( ; itr != m_Timers.end(); ++itr )
	{
		TIMER_CANCEL(itr->second);
	}
	m_Timers.clear();
}

// 计算饱食度等级
long CPet::CalcGorgeLevel(void)
{
	long l = 0;
	long lMaxLv = UNI_CONF.lGorgeLvNum;
	const int* Gorges = UNI_CONF.Gorges;
	while( Gorges[l] < m_DiffAttrs.lGorge && l < lMaxLv ) ++l;
	SetAttr("GorgeLevel", l);

	return l;
}

// 初始化属性字段
void CPet::InitNameValueMap(void)
{
	CMoveShape::InitNameValueMap();
	m_Attrs.clear();

	// 基本属性初始化
	tagPty st;

	static long minValue[7] = { 0x0, 0, 0, (long)-99999999.999f, -0x7FFFFFFF, -0x7FFF, 0};
	static long maxValue[7] = { 0xFF, 0xFFFF, 0x7FFFFFFF, (long)99999999.999f, 0x7FFFFFFF, 0x7FFF, 1};

	long* pVal = NULL;
	long lLevel = m_DiffAttrs.lLevel;

	SET_BASE_LONG_PROP(Hp);
	SET_BASE_LONG_PROP(Mp);
	SET_BASE_LONG_PROP(Gorge);
	SET_BASE_LONG_PROP(Loyalty);
	SET_BASE_LONG_PROP(Level);
	SET_BASE_LONG_PROP(Exp);

	SET_TEMP_LONG_PROP(GorgeLevel);
	SET_TEMP_LONG_PROP(MaxExp);
	SET_TEMP_LONG_PROP(FightType);
	SET_TEMP_LONG_PROP(MoveType);
	SET_TEMP_LONG_PROP(Country);

	SET_BASE_TEMP_LONG_PROP(MaxHp);
	SET_BASE_TEMP_LONG_PROP(MaxMp);
	SET_BASE_TEMP_LONG_PROP(HpRecoverSpeed);
	SET_BASE_TEMP_LONG_PROP(MpRecoverSpeed);

	SET_BASE_TEMP_LONG_PROP(Strenth);
	SET_BASE_TEMP_LONG_PROP(Agility);
	SET_BASE_TEMP_LONG_PROP(Consti);
	SET_BASE_TEMP_LONG_PROP(Intell);
	SET_BASE_TEMP_LONG_PROP(Wisdom);
	SET_BASE_TEMP_LONG_PROP(Spirit);
	SET_BASE_TEMP_LONG_PROP(Charm);
	SET_BASE_TEMP_LONG_PROP(Luck);
	SET_BASE_TEMP_LONG_PROP(Parry);
	SET_BASE_TEMP_LONG_PROP(Block);
	SET_BASE_TEMP_LONG_PROP(Pierce);
	SET_BASE_TEMP_LONG_PROP(Absorb);
	SET_BASE_TEMP_LONG_PROP(Hit);
	SET_BASE_TEMP_LONG_PROP(Dodge);
	SET_BASE_TEMP_LONG_PROP(Immunity);
	SET_BASE_TEMP_LONG_PROP(MAtkSpeed);
	SET_BASE_TEMP_LONG_PROP(MinAtk);
	SET_BASE_TEMP_LONG_PROP(MaxAtk);
	SET_BASE_TEMP_LONG_PROP(Def);
	SET_BASE_TEMP_LONG_PROP(MAtk);
	SET_BASE_TEMP_LONG_PROP(MDef);
	SET_BASE_TEMP_LONG_PROP(Cri);
	SET_BASE_TEMP_LONG_PROP(CriDef);
	SET_BASE_TEMP_LONG_PROP(MCri);
	SET_BASE_TEMP_LONG_PROP(MCriDef);
	SET_BASE_TEMP_LONG_PROP(WillDef);
	SET_BASE_TEMP_LONG_PROP(ElemDef0);
	SET_BASE_TEMP_LONG_PROP(ElemDef1);
	SET_BASE_TEMP_LONG_PROP(ElemDef2);
	SET_BASE_TEMP_LONG_PROP(ElemDef3);
	SET_BASE_TEMP_LONG_PROP(ElemDef4);
	SET_BASE_TEMP_LONG_PROP(ElemDef5);
}

// 获取宠物属性, 当属性字段不存在时, 返回值为0.
long CPet::GetAttr(const char* szName)
{
	if( NULL == szName ||
		strlen(szName) <2 )
		return 0;

	long lVal = CMoveShape::GetAttribute(string(szName));
	if(lVal != INVILID_VALUE)
		return lVal;

	string strName = szName;
	if( '_' == szName[1] && 'C' == strName[0] ) // 取得当前值
	{
		strName[0] = 'E';
		long lEVal = GetAttr(strName.c_str());
		strName[0] = 'B';
		long lBVal = GetAttr(strName.c_str());
		lVal = lEVal + lBVal;

		return lVal;
	}

	lVal = 0;
	ITR_ATTR_MAP itr = m_Attrs.find(szName);
	if( m_Attrs.end() != itr )
	{
		const tagPty& rAttr = itr->second;
		switch( rAttr.lType )
		{
		case 0:	// byte
			return *(rAttr.plValue);

		case 1:	// word
			return *(rAttr.pwValue);

		case 2:	// dword
			return *(rAttr.pdwValue);

		case 3:	// float
			return *(rAttr.pfValue);

		case 4:	// long
			return *(rAttr.pValue);

		case 5:	// short
			return *(rAttr.psValue);

		case 6:	// bool
			return *(rAttr.pbValue);
		}
	}

	return lVal;
}

long CPet::SetAttr(const char* szName, long lVal)
{
	if( '_' == szName[1] && 'B' == szName[0] )
	{
		PutoutLog("Pet", LT_DEFAULT, "试图修改基本属性[%s]->%ld.",
			szName, lVal);
		return 0;
	}

	long lRet = CMoveShape::SetValue(szName, lVal);
	if( INVILID_VALUE != lRet )
	{
		AddDirtyAttr(szName);		// 脏属性数据标记
		return lRet;
	}

	char szLog[512];
	ITR_ATTR_MAP itr = m_Attrs.find(szName);
	if( m_Attrs.end() != itr )
	{
		tagPty& rAttr = itr->second;
		if( strcmp("Hp", szName) == 0 )
		{
			long lMaxVal = GetMaxHP();
			if( lVal < 0 ) lVal = 0;
			if (lVal > lMaxVal ) lVal = lMaxVal;
		} 
		else if( strcmp("Mp", szName) == 0 )
		{
			long lMaxVal = GetMaxMP();
			if( lVal < 0 ) lVal = 0;
			if (lVal > lMaxVal ) lVal = lMaxVal;
		}

		switch (rAttr.lType)
		{
		case 0:	// byte
			{
				*(rAttr.plValue) = (BYTE)lVal;
			}
			break;

		case 1:	// word
			{
				*(rAttr.pwValue) = (WORD)lVal;
			}
			break;
		case 2:	// dword
			{
				*(rAttr.pdwValue) = (DWORD)lVal;
			}
			break;

		case 3:	// float
			{
				*(rAttr.pfValue) = (float)lVal;
			}
			break;

		case 4:	// long
			{
				*(rAttr.pValue) = (long)lVal;
			}
			break;

		case 5:	// short
			{
				*(rAttr.psValue) = (short)lVal;
			}
			break;

		case 6:	// bool
			{
				*(rAttr.pbValue) = lVal==0?false:true;
			}
			break;
		default:
			{
				lVal = INVILID_VALUE;
			}
		}
		AddDirtyAttr(szName);		// 脏属性数据标记
	}
	else
	{
		_snprintf(szLog, 512, "更新属性失败: [%-10s]-->%ld.",
			szName,
			lVal);
		PRINT_TO_CLIENT(szLog);
		lVal = INVILID_VALUE;
	}
	return lVal;
}

long CPet::ChgAttr(const char* szName, long lDelta)
{
	if( CMoveShape::ChangeAttribute(string(szName), lDelta) )
	{
		return CMoveShape::GetAttribute(szName);
	}

	long lVal = INVILID_VALUE;
	ITR_ATTR_MAP itr = m_Attrs.find(szName);
	if( m_Attrs.end() != itr )
	{
		lVal = GetAttr(szName);
		lVal += lDelta;
		lVal = SetAttr(szName, lVal);
	}

	return lVal;
}

// 脏属性数据标记
void CPet::AddDirtyAttr(const char* szName)
{
	m_DirtyAttrs.insert(szName);

	if( '_' == szName[1] && ('C' == szName[0] || 'B' == szName[0]) )
	{
		char szLog[512];
		char szGUID[64];
		GetExID().tostring(szGUID);
		_snprintf(szLog, 512, "玩家%s试图更新宠物%s的属性%s.",
			m_pHost->GetName(),
			szGUID,
			szName);
		PRINT_TO_CLIENT(szLog);
	}
	else
	{
		if( "Level" == szName ||
			"Hp" == szName ||
			"E_MaxHp" == szName )
		{
			m_BaseDirtyAttrs.insert(szName);
		}
		UpdateAttribute(TRUE);
	}
}

// 编码属性
void CPet::CodeChgAttr(DBWriteSet& setWriteDB,
					   short sEnum,
					   char cType,
					   long lValue)
{
	setWriteDB.AddToByteArray(sEnum);
	switch (cType)
	{
	case 0:		// byte
		setWriteDB.AddToByteArray(cType);
		setWriteDB.AddToByteArray((BYTE)lValue);
		break;
	case 1:		// word
		setWriteDB.AddToByteArray(cType);
		setWriteDB.AddToByteArray((WORD)lValue);
		break;
	case 2:		// dword
		setWriteDB.AddToByteArray(cType);
		setWriteDB.AddToByteArray((DWORD)lValue);
		break;
	case 3:		// float
		setWriteDB.AddToByteArray(cType);
		setWriteDB.AddToByteArray((float)lValue);
		break;
	case 4:		// long
		setWriteDB.AddToByteArray(cType);
		setWriteDB.AddToByteArray((long)lValue);
		break;
	case 5:		// short
		setWriteDB.AddToByteArray(cType);
		setWriteDB.AddToByteArray((short)lValue);
		break;
	case 6:		// bool
		setWriteDB.AddToByteArray(cType);
		setWriteDB.AddToByteArray((char)lValue);
		break;
	default:
		break;
	}
}

// 编码脏属性
void CPet::CodeChgAttrs(DBWriteSet& setWriteDB,
						set<string>& rAttrs)
{
	int i = 0;

	ITR_ATTR_MAP itrAttr;
	set<string>::iterator itr = rAttrs.begin();
	for( ; itr != rAttrs.end(); ++itr )
	{
		char* szAttr = const_cast<char*>(itr->c_str());
		if( (itrAttr = m_Attrs.find(szAttr)) != m_Attrs.end() ||
			(itrAttr = m_mapShapeNameValue.find(szAttr)) != m_mapShapeNameValue.end() )
		{
			if( '_' == szAttr[1] )
			{
				szAttr[0] = 'C';
			}
			long lValue = GetAttr(szAttr);
			CodeChgAttr(setWriteDB,
				itrAttr->second.attrEnumValue,
				itrAttr->second.lType,
				lValue);
		}
		else
		{
			CodeChgAttr(setWriteDB, 0, 0, 0);
			PutoutLog("Pet", LT_ERROR, "无法添加属性[%s].", szAttr);
		}
	}
}

// 设置宠物等级
void CPet::SetLevel(long lLevel)
{
	long lOldLevel = GetLevel();
	SetAttr("Level", lLevel);
	InitialOrigAttrs(lOldLevel, lLevel);
	OnLevelUpdate();
}

// 修正经验
void CPet::IncExp(long lDelta)
{
	long lExp = m_DiffAttrs.lExp +lDelta;
	if( 0 > lExp )
	{
		AddErrorLogText("得到一个负的经验[%d], 执行返回!", lExp);
		return;
	}

	// 升级
	bool bUpLevel = false;
	long lLevel = GetLevel();
	while( lExp >= GetAttr("MaxExp") )
	{
		bUpLevel = true;

		// 已经是最大等级
		if( UNI_CONF.lMaxLevel == lLevel )
		{
			lExp = GetAttr("MaxExp");
			break;
		}
		else
		{
			lExp -= GetAttr("MaxExp");
			SetAttr("MaxExp", UNI_CONF.Exps[++lLevel]);
		}
	}

	if( bUpLevel )
	{
		SetLevel(lLevel);
	}

	SetAttr("Exp", lExp);
	UpdateAttribute(TRUE);
	GetGame()->UpdateSavePlayer((CPlayer*)GetHost(), SAVE_DETAIL_PETS);
}

// 修正坐标
bool CPet::ModifyPos(INIT_TYPE lInitType)
{
	long lNo = GetPetType() - 1;
	if( lNo < 0 ) lNo = S_MAX_CLASSES - 1;
	bool bRes = false;
	if( IT_SAME_GS == lInitType || Distance(m_pHost) >= UNI_CONF.MaxDistances[lNo] )
	{
		if( PET_TYPE_TRAP == GetPetType() )
		{
			OnDied();
		}
		else
		{
			InitialByHost(lInitType);
			SetMoveType(PET_AS_FOLLOW);
			bRes = true;
		}
	}

	return bRes;
}

// 设置行走模式
void CPet::SetMoveType(long lType)
{
	if( CanMove() )
	{
		SetAttr("MoveType", lType);
		SetTarget(NULL);

		if( PET_AS_ACTIVE == GetFightType() )
		{
			m_pAI->AddAIEvent(CBaseAI::AET_MOVE);
		}
	}
}

// 设置攻击模式
void CPet::SetFightType(long lType)
{
	if( SAT_CAN_FIGHT == CanFight() )
	{
		SetAttr("FightType", lType);
		if( lType != PET_AS_ACTIVE )
		{
			SetTarget(NULL);
		}
		else if( NULL == GetTarget() )
		{
			m_pAI->AddAIEvent(CBaseAI::AET_MOVE);
		}
	}
}

// 获取行走模式
long CPet::GetMoveType(void)
{
	return GetAttr("MoveType");
}

// 获取攻击模式
long CPet::GetFightType(void)
{
	return GetAttr("FightType");
}

bool CPet::OnTrapSpring(void)
{
	CMoveShape* pTarget = m_pAI->GetTarget();
	bool bRes = false;
	CNewSkill* pSkillProperty = NULL;
	CNewSkill::stStaticParam* pParam = NULL;
	tagSkill* pSkill = GetSkill(0);
	if( pSkill != NULL &&
		(pSkillProperty = CNewSkillFactory::QueryNewSkillProperty(pSkill->lID)) != NULL &&
		(pParam = pSkillProperty->GetStaticParam(pSkill->lLevel)) != NULL &&
		Distance((CShape*)GetTarget()) < pParam->lMaxAtkDistance )
	{
		bRes = UseSkill(pSkill, pTarget);
	}

	return bRes;
}

bool CPet::HasSkill(long lID)
{
	ITR_SKILL_LIST itr = m_Skills.begin();
	for( ; itr != m_Skills.end(); ++itr )
	{
		if( lID == itr->second.lID )
			return true;
	}

	return false;
}

// 追加一种天赋技能
long CPet::AppendGeniusSkill(void)
{
	long lID = 0;
	long lNum = 0;
	if( m_TempAttrs.lGeniusSkillNum < UNI_CONF.lGeniusSkillsSize )
	{
		lID = m_BaseAttrs.GeniusSkills[m_TempAttrs.lGeniusSkillNum].lID;
		if( lID != 0 )
		{
			tagSkill tSkill;
			tSkill.lPos = m_TempAttrs.lGeniusSkillNum++;
			tSkill.lType = PET_SKILL_GENIUS;
			tSkill.lID = lID;
			tSkill.lLevel = 1;
			m_Skills[tSkill.lPos] = tSkill;
			ModifySkill(tSkill);
		}
	}

	return lID;
}

// 添加天赋技能
long CPet::AddGeniusSkill(void)
{
	switch (GetPetType())
	{
	case PET_TYPE_ELF:
		{
		}
		break;

	case PET_TYPE_HUNTER:
		{
			long lNum = m_TempAttrs.lGeniusSkillNum;

			// 天赋技能已全部开启
			if( UNI_CONF.lGeniusSkillNum == lNum )
			{
				return 0;
			}

			// 非初始化状态, 有一定几率追加一种天赋技能
			if( lNum >= UNI_CONF.nInitGeniusSkillMin )
			{
				// 追加成功
				if( random(100) <= UNI_CONF.GeniusSkillOpen[lNum].nRate )
				{
					return AppendGeniusSkill();
				}
				// 追加失败
				return 0;
			}

			// 初始状态下, 添加天赋技能
			while( lNum < UNI_CONF.nInitGeniusSkillMin )
			{
				lNum = random(UNI_CONF.nInitGeniusSkillMax);
			}

			while( lNum-- > 0 )
			{
				AppendGeniusSkill();
			}
		}
		break;

	case PET_TYPE_CONJURED:
	case PET_TYPE_TRAP:
		{
			long lNum = m_BaseAttrs.lSkillNum;
			tagSkill tSkill;
			for (long i=0; i<lNum; ++i)
			{
				tSkill.lPos = m_TempAttrs.lGeniusSkillNum++;
				tSkill.lType = PET_SKILL_GENIUS;
				tSkill.lID = m_BaseAttrs.GeniusSkills[i].lID;
				tSkill.lLevel = m_BaseAttrs.GeniusSkills[i].lLevel;
				m_Skills[tSkill.lPos] = tSkill;
				ModifySkill(tSkill);
			}
		}
		break;
	}

	return 0;
}

// 添加插槽技能
long CPet::AddStudySkill(void)
{
	long lRes = 0;
	switch( GetPetType() )
	{
	case PET_TYPE_ELF:
		{
			lRes = OpenSkillHole();
		}
		break;

	case PET_TYPE_CONJURED:
		{
		}
		break;
	}

	return lRes;
}

// 创建时初始化技能
long CPet::InitialSkills(void)
{
	switch( GetPetType() )
	{
	case PET_TYPE_ELF:
		{
			AddStudySkill();
		}
		break;
	}

	AddGeniusSkill();

	return 0 ;
}

// 开启技能槽
long CPet::OpenSkillHole(long lType, long lPos)
{
	if( UNI_CONF.lStudySkillNum == m_TempAttrs.lStudySkillNum )
		return PET_SKILL_NULL;

	tagSkill tSkill;

	if( 0 == lType )
	{
		lType = random(100);
		if( lType < 33 )
			lType = PET_SKILL_RED;
		else if( lType < 66 )
			lType = PET_SKILL_YELLOW;
		else if( lType < 100 )
			lType = PET_SKILL_BLUE;
		else
			return PET_SKILL_NULL;		// 未开启
	}

	if( 0 == lPos )
	{
		lPos = UNI_CONF.lGeniusSkillNum + m_TempAttrs.lStudySkillNum++;
	}

	tSkill.lPos = lPos;
	tSkill.lType = lType;
	m_Skills[lPos] = tSkill;

	CMessage msg(MSG_S2C_PET_AI);
	msg.Add(GetExID());
	msg.Add((long)PET_OT_AI_ADD_SKILL);
	msg.Add(tSkill.lPos);
	msg.Add(tSkill.lType);
	msg.SendToPlayer(m_HostID);

	return lType;
}

// 关闭技能槽
void CPet::CloseSkillHole(long lPos)
{
	--m_TempAttrs.lStudySkillNum;
	m_Skills.erase(lPos);
}

// 获取技能
tagSkill* CPet::GetSkill(long lPos)
{
	ITR_SKILL_LIST itr = m_Skills.find(lPos);
	if( itr != m_Skills.end() )
		return &itr->second;
	return NULL;
}

// 设置宠物技能快捷键
void CPet::SetSkillHotKey(long lPos, long lActPos)
{
	tagSkill* pSkill = GetSkill(lPos);
	if( pSkill != NULL )
	{
		pSkill->lActPos = lActPos;
	}
}

// 检查技能修正条件
CNewSkill* CPet::CanModifySkill(tagSkill& rSkill)
{
	// 宠物已死亡
	if( IsDied() )
	{
		return NULL;
	}

	// 技能列表无此技能
	CNewSkill* pSkill = CNewSkillFactory::QueryNewSkillProperty(rSkill.lID);
	if( NULL == pSkill )
	{
		char szLog[512];
		char szGUID[128];
		GetExID().tostring(szGUID);
		_snprintf(szLog, 512, "宠物%s在修改技能%ld(Lv.%ld)时出错: 技能不存在",
			szGUID, rSkill.lID, rSkill.lLevel);
		PutoutLog("Pet", LT_ERROR, szLog);
		return NULL;
	}

	// 技能类型
	if( pSkill->GetSkillType() != rSkill.lType )
	{
		//OccuJudgeStudy();
	}

	// 宠物类型
	if( pSkill->GetSkillType() != m_BaseAttrs.lPetType )
	{
	}

	return pSkill;
}

// 修正技能
bool CPet::ModifySkill(tagSkill& rSkill)
{
	CNewSkill* pSkill = CanModifySkill(rSkill);
	CNewSkillProperties::tagSkill *pTSkill = NULL;
	if( NULL == pSkill ||
		NULL == (pTSkill = pSkill->GetNewSkill(rSkill.lLevel)) )
	{
		char szLog[512];
		char szGUID[128];
		GetExID().tostring(szGUID);
		_snprintf(szLog, 512, "宠物%s在修改技能%ld(Lv.%ld)时出错: 等级不存在",
			szGUID, rSkill.lID, rSkill.lLevel);
		PutoutLog("Pet", LT_ERROR, szLog);
		return false;
	}

	CMessage msg(MSG_S2C_SKILL_ADD_PET);
	msg.Add(GetExID());
	msg.AddEx(&rSkill, sizeof(rSkill));
	if ( pTSkill->pStaticParam != NULL )
	{
		msg.Add(pTSkill->pStaticParam->lCoolDownTime);
		msg.Add(pTSkill->pStaticParam->lMinAtkDistance);
		msg.Add(pTSkill->pStaticParam->lMaxAtkDistance);
		msg.Add(pTSkill->pStaticParam->lConsMp);
	}
	else
	{
		msg.Add((long)0);
		msg.Add((long)0);
		msg.Add((long)0);
		msg.Add((long)0);
	}
	msg.SendToPlayer(m_HostID);

	GetGame()->UpdateSavePlayer((CPlayer*)GetHost(), SAVE_DETAIL_PETS);
	if( ModuType_Skill_Passive == pSkill->GetSkillType() )
	{
		OnBeginSkill(rSkill.lID, rSkill.lLevel);
	}

	StudySkillScript(rSkill.lID, rSkill.lLevel);
	return true;
}

// 修正技能
bool CPet::ModifySkill(long lID, long lLevel, long lPos)
{
	// 技能未开启
	tagSkill* pTSkill = GetSkill(lPos);
	if( PET_TYPE_ELF == GetPetType() && NULL == pTSkill )
	{
		return false;
	}
	if( NULL == pTSkill )
	{
		pTSkill = AddSkill(tagSkill());
	}

	pTSkill->lID = lID;
	pTSkill->lLevel = lLevel;
	return ModifySkill(*pTSkill);
}

// 添加技能
tagSkill* CPet::AddSkill(tagSkill& rSkill)
{
	if( 0 == rSkill.lPos ) rSkill.lPos = (long)m_Skills.size();
	m_Skills[rSkill.lPos] = rSkill;
	return &m_Skills[rSkill.lPos];
}

bool CPet::AddSkill(CGoods* pGoods, long lPos)
{
	//tagSkill tSkill;
	//tSkill.lID = pGoods->GetAddonPropertyValues(GAP_ADD_SKILL, 1);
	//tSkill.lLevel = pGoods->GetAddonPropertyValues(GAP_ADD_SKILL, 2);
	//tSkill.lType = pGoods->GetAddonPropertyValues(GAP_COLOR, 1);
	//tSkill.lPos = lPos;

	//if( CanAddSkill(rSkill) )
	//{
	//	return AddSkill(tSkill);
	//}
	return false;
}

// 检查技能使用条件
long CPet::CanUseSkill(tagSkill* pSkill, CMoveShape* pTarget, long& lSkillType)
{
	long lCurTime = timeGetTime();

	if (this->IsDied() || pTarget->IsDied())
	{
		return SKILL_USE_FAILED_INVALID_STATE;
	}

	// 技能释放间隔
	if( m_TempAttrs.lLastUseSkillTime - m_TempAttrs.lMAtkSpeed > lCurTime )
	{
		return SKILL_USE_FAILED_INVALID_TIME;
	}

	// 冷却时间
	if( pSkill->lEndCooldown > lCurTime )
	{	
		return SKILL_USE_FAILED_INVALID_TIME;
	}

	//if (dwCurrTime<m_dwIntonateStartTime + m_dwIntonateTime)
	//{
	//	//吟唱时间
	//	return SKILL_USE_FAILED_INVALID_INTONATE;		
	//}

	CNewSkill* pSkillProperty = CNewSkillFactory::QueryNewSkillProperty(pSkill->lID);
	if ( NULL == pSkillProperty )
	{
		return SKILL_USE_FAILED_INVALID_UNKNOWN;
	}

	CNewSkill::stStaticParam* pParam = pSkillProperty->GetStaticParam(pSkill->lLevel);
	long lActTime = 0;
	if ( pParam != NULL &&
		pParam->lActTime > 0 &&
		IsInAct() )
	{
		return SKILL_USE_FAILED_INVALID_ACT;
	}
	//if (pSkillProperty && pTarget)
	//{
	//	if (pSkillProperty->GetTarget()==0 && pSkillProperty->GetDeadUse()==0 && pTarget->GetState()==STATE_DIED)
	//	{
	//		return SKILL_USE_FAILED_INVALID_STATE;
	//	}
	//}



	//string str = "bUseFighterSkill";
	//if (GetAttribute(str) == FALSE)
	//{
	//	if (pSkillProperty->GetUseType()==1)
	//	{
	//		//不能使用战士类技能
	//		return SKILL_USE_FAILED_INVALID_STATE;
	//	}
	//}
	//str = "bUseRabbSkill";
	//if (GetAttribute(str) == FALSE)
	//{
	//	if (pSkillProperty->GetUseType()==2)
	//	{
	//		//不能使法师类技能
	//		return SKILL_USE_FAILED_INVALID_STATE;
	//	}
	//}

	m_TempAttrs.lLastUseSkillTime = lCurTime;
	pSkill->lEndCooldown = lCurTime + pParam->lCoolDownTime;
	lSkillType = pSkillProperty->GetSkillType();

	return -1;
}

// 使用技能
bool CPet::UseSkill(tagSkill* pSkill, CMoveShape* pTarget)
{
	long lSkillType = -1;
	long lRet = CanUseSkill(pSkill, pTarget, lSkillType);
	if ( lRet != -1 )
	{
		CMessage msg(MSG_S2C_SKILL_USE);
		msg.Add((CHAR)SKILL_USE_RESULT_FAILED);
		msg.Add( GetExID() );
		msg.Add(pSkill->lID);
		msg.Add((BYTE)pSkill->lLevel);
		msg.Add((DWORD)0);
		msg.Add((BYTE)lRet);
		msg.SendToPlayer(m_HostID);
		return false;
	}

	switch( lSkillType )
	{
	case ModuType_Buff:
	case ModuType_DeBuff:
		{
			AddNewState(this, pSkill->lID, pSkill->lLevel,13);
		}
		break;

	case ModuType_Skill_Active:
		{
			AddActiveSkill(pSkill->lID, pSkill->lLevel, pTarget);
		}
		break;

	case ModuType_Skill_Passive:
		{
			AddPassSkill(pSkill->lID, pSkill->lLevel);
		}
		break;
	default:
		{
			AddActiveSkill(pSkill->lID, pSkill->lLevel, pTarget);
		}
		break;
	}

	if( pTarget != NULL )
	{
		m_pAI->Attack(pSkill->lID, pTarget->GetTileX(), pTarget->GetTileY(), pTarget);
	}

	return true;
}

// 触发技能学习脚本
void CPet::StudySkillScript(long lID, long lLevel)
{
	CStudySkillCondition::tagCondition* ptgCon =
		CStudySkillCondition::GetCondition(lID, lLevel);
	if ( ptgCon != NULL )
	{
		vector<CStudySkillCondition::tagConditionEx*>::iterator itr
			= ptgCon->vec_ConditionEx.begin();
		for( ; itr != ptgCon->vec_ConditionEx.end(); ++itr )
		{
			if( CStudySkillCondition::CONDITIONEX_SCRIPT == (*itr)->wConditionExType )
			{
				PetRunScript((*itr)->strValue.c_str());
			}
		}
	}
}

//给该玩家运行一个脚本
void CPet::PetRunScript(const char* szScriptName)
{
	if( NULL == szScriptName ) return;

	stRunScript sScript;
	sScript.pszFileName = const_cast<char*>(szScriptName);
	sScript.desShape = NULL;
	sScript.pRegion = dynamic_cast<CRegion*>(GetFather());
	sScript.srcShape = this;
	RunScript(&sScript, const_cast<char*>(GetGame()->GetScriptFileData(szScriptName)));
}

// GS==>Client编码
bool CPet::CodeToDataBlock_ForClient(DBWriteSet& setWriteDB,
									 bool bExData)
{
#ifdef _RUNSTACKINFO3_
	char pszStatckInfo[1024]="";
	_snprintf(pszStatckInfo,1024,"CPet::CodeToDataBlock_ForClient(name:%s) Start",GetName());
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStatckInfo);
#endif

	CMoveShape::CodeToDataBlock_ForClient(setWriteDB, bExData);
	setWriteDB.AddToByteArray(m_HostID);

	if( bExData )
	{
		long lNum = (long)m_Skills.size();
		setWriteDB.AddToByteArray(lNum);
		ITR_SKILL_LIST itr = m_Skills.begin();
		bool bFlag = false;
		CNewSkillProperties* pSkillProperty = NULL;
		CNewSkillProperties::tagSkill* pSkill = NULL;
		long lCurTime = timeGetTime();
		long lRestTime = 0;
		for( ; itr != m_Skills.end(); ++itr )
		{
			bFlag = false;
			setWriteDB.AddToByteArray(&itr->second, sizeof(tagSkill));
			pSkillProperty = CNewSkillFactory::QueryNewSkillProperty(itr->second.lID);
			if( pSkillProperty != NULL)
			{
				pSkill = pSkillProperty->GetNewSkill(itr->second.lLevel);
				if( pSkill != NULL && pSkill->pStaticParam != NULL )
				{
					bFlag = true;
				}
			}

			if( bFlag )
			{
				setWriteDB.AddToByteArray(pSkill->pStaticParam->lCoolDownTime);
				setWriteDB.AddToByteArray(pSkill->pStaticParam->lMinAtkDistance);
				setWriteDB.AddToByteArray(pSkill->pStaticParam->lMaxAtkDistance);
				setWriteDB.AddToByteArray(pSkill->pStaticParam->lConsMp);
			} 
			else
			{
				setWriteDB.AddToByteArray((long)0);
				setWriteDB.AddToByteArray((long)0);
				setWriteDB.AddToByteArray((long)0);
				setWriteDB.AddToByteArray((long)0);
			}
			lRestTime = lCurTime - itr->second.lEndCooldown;
			if( lRestTime < 0 ) lRestTime = 0;
			setWriteDB.AddToByteArray(lRestTime);
		}
	}

	// 非陷阱
	else
	{
		setWriteDB.AddToByteArray(m_DiffAttrs.lLevel);
		setWriteDB.AddToByteArray(m_DiffAttrs.lHp);
		setWriteDB.AddToByteArray(GetMaxHP());
	}

	return true;
}

// GS==>WS编码
bool CPet::CodeToDataBlock(DBWriteSet& setWriteDB,
						   bool bExData)
{
	CMoveShape::CodeToDataBlock(setWriteDB, bExData);

	setWriteDB.AddToByteArray(&m_DiffAttrs, sizeof(m_DiffAttrs));
	setWriteDB.AddToByteArray(&m_TempAttrs, sizeof(m_TempAttrs));
	setWriteDB.AddToByteArray(m_HostID);		// 主人
	long lNum = (long)m_Skills.size();
	setWriteDB.AddToByteArray(lNum);
	ITR_SKILL_LIST itr = m_Skills.begin();
	for( ; itr != m_Skills.end(); ++itr )
	{
		setWriteDB.AddToByteArray(&itr->second, sizeof(tagSkill));
	}

	return true;
}

// WS==>GS解码
bool CPet::DecodeFromDataBlock(DBReadSet& setReadDB,
							   bool bExData)
{
	CMoveShape::DecodeFromDataBlock(setReadDB, bExData);

	setReadDB.GetBufferFromByteArray(&m_DiffAttrs, sizeof(m_DiffAttrs));
	setReadDB.GetBufferFromByteArray(&m_TempAttrs, sizeof(m_TempAttrs));
	setReadDB.GetBufferFromByteArray(m_HostID);		// 主人
	long lSkillNum = setReadDB.GetLongFromByteArray();
	tagSkill tSkill;
	for( long l=0; l<lSkillNum; ++l )
	{
		setReadDB.GetBufferFromByteArray(&tSkill, sizeof(tSkill));
		AddSkill(tSkill);							// 添加技能
	}

	// 完成基本初始化工作
	return InitialDiffAttrs(IT_LOGIN_GS);
}

CMoveShape* CPet::GetHost(void)
{
	if( NULL == m_pHost )
	{
		char szLog[512];
		char szGUID[128];
		GetExID().tostring(szGUID);
		_snprintf(szLog, 512, "宠物%s主人为NULL.",
			szGUID);
		PutoutLog("Pet", LT_ERROR, szLog);
	}

	return (CMoveShape*)m_pHost;
}

void CPet::SetSkillCooldown(long lID, DWORD dwTime)
{
}

// 是否可被GUID为rID的MoveShape所见
bool CPet::IsVisible(OBJECT_TYPE eType, const CGUID& rID)
{
	bool bRet = true;
	CBaseObject* pRgn = GetFather();
	if( pRgn != NULL )
	{
		CBaseObject* pObj = GetFather()->FindChildObject(eType, rID);
		CMoveShape* pShape = dynamic_cast<CMoveShape*>(pObj);
		if( pShape != NULL )
		{
			bRet = CMoveShape::IsVisible(pShape);
		}
	}
	return bRet;
}

long CPet::AddActiveSkill(DWORD dwSkillID,
						  DWORD dwLevel,
						  CMoveShape *pTarget,
						  long lGoodsIndex,
						  long lContainerID,
						  long lPos)
{
	SetInstanceSkill(NULL);
	CMoveShape::InitSkillInstance(dwSkillID, dwLevel,0,0,0,NULL_GUID,3);
	return 1;
}