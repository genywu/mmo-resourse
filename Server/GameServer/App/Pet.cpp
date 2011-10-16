#include "StdAfx.h"
#include "Pet.h"
#include "StateAI/PetAI.h"
#include "Goods/CGoods.h"
#include "Monster.h"
#include "Player.h"
#include "LuaScript/Script.h"
#include "LuaScript/ScriptSys.h"
#include "../../../Public/Common/StudySkillCondition.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef _DEBUG
long g_lTimer = 0;
#endif

#ifdef _DEBUG
#define PRINT_TO_CLIENT(__strLog) \
	GetGame()->SendNotifyMessage(__strLog,m_HostID,eNOTIFYPOS_CENTER);\
    Log4c::Error(PET_MODULE,"%-15s %s",__FUNCTION__,__strLog)

#else
#define PRINT_TO_CLIENT(__strLog) \
	Log4c::Error(PET_MODULE,"%-15s %s",__FUNCTION__,__strLog)
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
	SET_LONG_PROP(&m_DiffAttrs.l##ATTR, #ATTR, PET_ATTR_##ATTR); \
	sprintf(strLog, #ATTR" --> m_DiffAttrs.l"#ATTR);\
	Log4c::Trace(PET_MODULE, strLog);

// 侍从的基本属性
#define SET_SERVANTBASE_LONG_PROP(ATTR) \
	SET_LONG_PROP(&m_DiffAttrs.ServantAttr.l##ATTR,#ATTR,PET_ATTR_##ATTR); \
	sprintf(strLog, #ATTR" --> m_DiffAttrs.ServantAttr.l"#ATTR);\
	Log4c::Trace(PET_MODULE, strLog);

// 侍从的强化属性
#define SET_SERVANT_EXTEND_PROP(TYPE, ATTR) \
	SET_LONG_PROP(&m_DiffAttrs.ServantAttr.##TYPE##Val.l##ATTR, #TYPE#ATTR, PET_ATTR_##TYPE##_##ATTR); \
	sprintf(strLog, #TYPE#ATTR" --> m_DiffAttrs.ServantAttr.l"#TYPE"Val.l"#ATTR);\
	Log4c::Trace(PET_MODULE, strLog);

#define SET_BASE_LONG_PROP_EX(ATTR, MIN, MAX) \
	SET_LONG_PROP_EX(&m_DiffAttrs.l##ATTR, #ATTR, PET_ATTR_##ATTR, MIN, MAX)

// 临时属性
#define SET_TEMP_LONG_PROP(ATTR) \
	SET_LONG_PROP(&m_TempAttrs.l##ATTR, #ATTR, PET_ATTR_##ATTR); \
	sprintf(strLog, #ATTR" --> m_TempAttrs.l"#ATTR);\
	Log4c::Trace(PET_MODULE, strLog);

// 基本+临时属性
#define SET_BASE_TEMP_LONG_PROP(ATTR) \
	SET_LONG_PROP(&m_BaseAttrs.l##ATTR, "B_"#ATTR, PET_ATTR_##ATTR); \
	sprintf(strLog, "B_"#ATTR" --> m_BaseAttrs.l"#ATTR);\
	Log4c::Trace(PET_MODULE, strLog); \
	SET_LONG_PROP(&m_TempAttrs.l##ATTR, "E_"#ATTR, PET_ATTR_##ATTR); \
	sprintf(strLog, #ATTR" --> m_TempAttrs.l"#ATTR);\
	Log4c::Trace(PET_MODULE, strLog);

// 属性换算公式
#define CALC_EXCH_SET_ATTR(ATTRS, ATTRD) \
	m_TempAttrs.l##ATTRD = (m_BaseAttrs.l##ATTRS + m_TempAttrs.l##ATTRS) * UNI_CONF.f##ATTRS##2##ATTRD + m_BaseAttrs.l##ATTRD
//SetAttr(#ATTRD,  (m_BaseAttrs.l##ATTRS + m_TempAttrs.l##ATTRS) * UNI_CONF.f##ATTRS##2##ATTRD + m_BaseAttrs.l##ATTRD);

// 属性换算公式
#define CALC_EXCH_CHG_ATTR(ATTRS, ATTRD) \
	m_TempAttrs.l##ATTRD += (m_BaseAttrs.l##ATTRS + m_TempAttrs.l##ATTRS) * UNI_CONF.f##ATTRS##2##ATTRD + m_BaseAttrs.l##ATTRD
//ChgAttr("E_"#ATTRD,  (m_BaseAttrs.l##ATTRS + m_TempAttrs.l##ATTRS) * UNI_CONF.f##ATTRS##2##ATTRD + m_BaseAttrs.l##ATTRD);

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
	//m_pVariableList = OBJ_CREATE(CVariableList);
	m_pAI = new StateAI::PetAI( this );
	//m_pAI->SetOwner(this);
	SetType(TYPE_PET);
    m_mover.Init( this );
	static long l = 0;
	Log4c::Trace(ROOT_MODULE, "%ld Constructor.", ++l);
}

CPet::~CPet(void)
{
	if( IsHangUp() )
	{
		DelFromRegion(PET_OT_DISAPPEAR);
	}

	//DelectAllScripByShape(GetExID());

	CancelAllTimer();
	ClearActiveSkills();
	ClearPassiveSkill();
	ClearAllBuff();
	delete m_pAI;
	//CAIFactory::DeleteAI(m_pAI);
	//OBJ_RELEASE(CVariableList, m_pVariableList);
	static long l = 0;
	Log4c::Trace(ROOT_MODULE, "%ld Destructor.", --l);
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
	_snprintf(szInfo,256,"CPet::OnTimeOut(TimerType:%d,Name:%s) Start",
		pTimerVar?pTimerVar->TimerType:0,
		GetName());
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szInfo);
#endif

	// 1) 侍从的HP、MP、内力、忠诚度、寿命

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

			case PET_EnergyRenew:
				{
					OnEnergyRenew();
				}
				break;

			case PET_LoyaltyRenew:
				{
					OnLoyaltyRenew();
				}
				break;

			case PET_LifeRenew:
				{
					OnLifeRenew();
				}
				break;
			case PET_SearchEnemy:
				{

					if( GetTarget() == NULL )
					{
						m_pAI->Resume( 0 );
						//m_pAI->AddAIEvent(CBaseAI::AET_MOVE);
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

    case ChildObjType_ShapeMove:
        m_mover.OnTimeOut();
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
		Log4c::Error(PET_MODULE,"%-15s %s",__FUNCTION__, strInfo);*/

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

	StateAI::BaseAI* pAI = GetAI();
	if ( !CanMove() ||
		!CheckMoveStep(fCurX, fCurY) ||
		NULL == pAI )
	{
		OnCannotMove();
		return;
	}

	// 添加一个验证消息数据
	AddMoveMsg(fCurX, fCurY);

	long lDir = GetLineDir(GetPosX(), GetPosY(), fCurX, fCurY);
	SetDir(lDir);
	SetPosXY(fCurX, fCurY);
	if( pAI->OnMove(fDestX, fDestY) )
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
	msg.Add( GetPosX() );
	msg.Add( GetPosY() );
	msg.Add( static_cast<LONG>(0) );
	msg.SendToPlayer(m_HostID);
}

// 宠物受到攻击
long CPet::OnBeenAttacked(CMoveShape* pAttacker, long lHp)
{
	return CMoveShape::OnBeenAttacked(pAttacker, lHp);
}

/// 宠物死亡
void CPet::OnDied(void)
{
	Log4c::Trace(PET_MODULE, FormatText("GS_PET_4",m_pHost->GetName(),GetName(),GetExID().tostring()));//玩家%s的宠物(%s: %s) 死亡

	// 死亡规则
	ChangeStateType(PET_STATE_AWAIT);
	OnChangeLoy(UNI_CONF.loyaltyMax*UNI_CONF.loyaDieRate/100);
	OnChangeLife(UNI_CONF.maxLife*UNI_CONF.lifeDieDownRate/100);
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
			SetAttr("E_EnergyRecoverSpeed", -GetAttr("B_EnergyRecoverSpeed"));
		}
		break;

	case STATE_PEACE:
		{
			m_pAI->SetTarget( NULL_GUID, 0);
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

/// 向其他客户端更新属性
bool CPet::UpdateAttributeToOtherClient()
{
	if (!m_BaseDirtyAttrs.empty())
	{
		CMessage msg(MSG_S2C_SHAPE_CHANGE_PROPERTIES);
		msg.Add(GetType());
		msg.Add(GetExID());
		DBWriteSet setWriteDB;
		msg.GetDBWriteSet(setWriteDB);

		WORD wAttrNum = m_BaseDirtyAttrs.size();
		setWriteDB.AddToByteArray(wAttrNum);
		if( wAttrNum > 0 )
		{
			CodeChgAttrs(setWriteDB, m_BaseDirtyAttrs);
		}
		msg.SendToPlayers(GetSelectSelfPlayer(), true);
		m_BaseDirtyAttrs.clear();
		return true;
	}
	return false;
}

long  CPet::UpdateAttribute(BOOL bImmediately)
{
	if(!bImmediately)
	{
		RegisterUpdateAttrTimer();
		return true;
	}

	UpdateAttributeToClient(true);
	return true;
}

long CPet::OnUpdateAttributeTimeOut()
{
	UpdateAttributeToClient(false);
	return true;
}

void CPet::OnPropertyValueChanged(const char* strName, LONG lValue)
{
	//如果基础属性发生改变
	//要重新计算相关其他二级属性
	if(NULL == strName)	return;
	if(0 == strncmp(strName,"B_Strenth",9) || 0 == strncmp(strName,"Strenth",7) ||
		0 == strncmp(strName,"B_Dexterity",9) || 0 == strncmp(strName,"Dexterity",9) ||
		0 == strncmp(strName,"B_Con",8) || 0 == strncmp(strName,"Con",3) ||
		0 == strncmp(strName,"B_Intellect",8) || 0 == strncmp(strName,"Intellect",9) ||
		0 == strncmp(strName,"B_Spiritualism",8) || 0 == strncmp(strName,"Spiritualism",12))
	{
		InitialFightProperty();
	}
}

// 初始化临时属性
void CPet::InitialTempAttrs(void)
{
	m_TempAttrs = tagPetTemp();

	m_TempAttrs.lMaxExp = UNI_CONF.Exps[m_DiffAttrs.lLevel-1];
	m_TempAttrs.lMaxDExp = UNI_CONF.DExps[m_DiffAttrs.ServantAttr.lDLevel-1];
	m_TempAttrs.lRace = m_BaseAttrs.lRace;
	m_TempAttrs.lTakeLev = m_BaseAttrs.lTakeLevel;
	m_TempAttrs.lPetType = m_BaseAttrs.lPetType;
	m_TempAttrs.lNominateMode = m_BaseAttrs.lNominateMode;
	m_TempAttrs.lSkillNum = m_Skills.size();
	m_TempAttrs.lFightType = PET_FIGHETYPE_FIGHT;
	m_TempAttrs.lMoveType = PET_MOVETYPE_FOLLOW;
	m_TempAttrs.lLoyLev = CalcLoyLevel();

	InitialOrigAttrs(1, m_DiffAttrs.lLevel);
}

void CPet::InitialFightProperty(void)
{
	CALC_EXCH_SET_ATTR(Strenth, Hit);				// 力量--命中
	CALC_EXCH_SET_ATTR(Strenth, Dodge);				// 力量--闪避
	CALC_EXCH_SET_ATTR(Strenth, MAtkSpeed);			// 力量--吟唱速度
	CALC_EXCH_SET_ATTR(Strenth, PhysicsAtk);		// 力量--物理伤害
	CALC_EXCH_SET_ATTR(Strenth, PhysicsDef);		// 力量--物理防御
	CALC_EXCH_SET_ATTR(Strenth, MAtk);				// 力量--魔法伤害
	CALC_EXCH_SET_ATTR(Strenth, MDef);				// 力量--魔法防御

	CALC_EXCH_CHG_ATTR(Dexterity, Hit);				// 身法--命中
	CALC_EXCH_CHG_ATTR(Dexterity, Dodge);			// 身法--闪避
	CALC_EXCH_CHG_ATTR(Dexterity, MAtkSpeed);		// 身法--吟唱速度
	CALC_EXCH_CHG_ATTR(Dexterity, PhysicsAtk);		// 身法--物理伤害
	CALC_EXCH_CHG_ATTR(Dexterity, PhysicsDef);		// 身法--物理防御
	CALC_EXCH_CHG_ATTR(Dexterity, MAtk);			// 身法--魔法伤害
	CALC_EXCH_CHG_ATTR(Dexterity, MDef);			// 身法--魔法防御

	CALC_EXCH_CHG_ATTR(Con, Hit);					// 根骨--命中
	CALC_EXCH_CHG_ATTR(Con, Dodge);					// 根骨--闪避
	CALC_EXCH_CHG_ATTR(Con, MAtkSpeed);				// 根骨--吟唱速度
	CALC_EXCH_CHG_ATTR(Con, PhysicsAtk);			// 根骨--物理伤害
	CALC_EXCH_CHG_ATTR(Con, PhysicsDef);			// 根骨--物理防御
	CALC_EXCH_CHG_ATTR(Con, MAtk);					// 根骨--魔法伤害
	CALC_EXCH_CHG_ATTR(Con, MDef);					// 根骨--魔法防御

	CALC_EXCH_CHG_ATTR(Intellect, Hit);				// 意志--命中
	CALC_EXCH_CHG_ATTR(Intellect, Dodge);			// 意志--闪避
	CALC_EXCH_CHG_ATTR(Intellect, MAtkSpeed);		// 意志--吟唱速度
	CALC_EXCH_CHG_ATTR(Intellect, PhysicsAtk);		// 意志--物理伤害
	CALC_EXCH_CHG_ATTR(Intellect, PhysicsDef);		// 意志--物理防御
	CALC_EXCH_CHG_ATTR(Intellect, MAtk);			// 意志--魔法伤害
	CALC_EXCH_CHG_ATTR(Intellect, MDef);			// 意志--魔法防御

	CALC_EXCH_CHG_ATTR(Spiritualism, Hit);			// 灵性--命中
	CALC_EXCH_CHG_ATTR(Spiritualism, Dodge);		// 灵性--闪避
	CALC_EXCH_CHG_ATTR(Spiritualism, MAtkSpeed);	// 灵性--吟唱速度
	CALC_EXCH_CHG_ATTR(Spiritualism, PhysicsAtk);	// 灵性--物理伤害
	CALC_EXCH_CHG_ATTR(Spiritualism, PhysicsDef);	// 灵性--物理防御
	CALC_EXCH_CHG_ATTR(Spiritualism, MAtk);			// 灵性--魔法伤害
	CALC_EXCH_CHG_ATTR(Spiritualism, MDef);			// 灵性--魔法防御
}

// 初始化原始属性和对应属性
void CPet::InitialOrigAttrs(long lOldLevel, long lCurLevel)
{
	if(m_GrowCoefs.empty()) return;

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
	m_TempAttrs.l##ATTR += m_BaseAttrs.l##ATTR; \
	//SetAttr(#ATTR, m_TempAttrs.l##ATTR)

	CALC_TEMP_ATTR(MaxHp);							// Hp
	CALC_TEMP_ATTR(MaxMp);							// Mp
	CALC_TEMP_ATTR(MaxEnergy);						// 内力

	CALC_TEMP_ATTR(Strenth);						// 力量
	CALC_TEMP_ATTR(Dexterity);						// 身法
	CALC_TEMP_ATTR(Con);							// 根骨
	CALC_TEMP_ATTR(Intellect);						// 意志
	CALC_TEMP_ATTR(Spiritualism);					// 灵性

	///  取得品质对应的天资值
	// 品质对天资修正 ＝ 品质 * 天资上限
#define CALC_TALENT_VAL(ATTR) \
	m_BaseAttrs.lMax##ATTR * UNI_CONF.Quality[m_DiffAttrs.ServantAttr.lQuality] / 1000

	/// 取得最终天资
	// 最终天资 ＝ 当前天资 ＋ 品质对天资修正 ＋ 后天天资 
#define CALC_LAST_TALENT_VAL(ATTR) \
	m_DiffAttrs.ServantAttr.talentVal.l##ATTR \
	+ CALC_TALENT_VAL(ATTR) \
	+ m_DiffAttrs.ServantAttr.dAfterVal.l##ATTR

	/// 每次升级基本属性提升量
#define CALC_UP_LEV_ATTR(ATTR) \
	lCurLev  = GetAttr("Level"); \
	lastTalent = CALC_LAST_TALENT_VAL(ATTR); \
	baseAttr = m_BaseAttrs.l##ATTR; \
	m_TempAttrs.l##ATTR += lCurLev*baseAttr*lastTalent/1000

	// 天资影响基本属性计算
	int   lCurLev;
	double lastTalent;
	long baseAttr;
	CALC_UP_LEV_ATTR(Strenth);
	CALC_UP_LEV_ATTR(Dexterity);
	CALC_UP_LEV_ATTR(Con);
	CALC_UP_LEV_ATTR(Intellect);
	CALC_UP_LEV_ATTR(Spiritualism);

	// 战斗属性计算
	InitialFightProperty();

	if( 0 == GetAttr("Hp") || lOldLevel != lCurLevel )
	{
		SetAttr("Hp", GetMaxHP());
		SetAttr("Mp", GetMaxMP());
		SetAttr("Energy", GetMaxEnergy());
	}
	else
	{
		SetAttr("Hp", m_DiffAttrs.lHp);
		SetAttr("Mp", m_DiffAttrs.lMp);
		SetAttr("Energy", m_DiffAttrs.lEnergy);
	}

	if( STATE_FIGHT == GetState() )
	{
		SetAttr("E_HpRecoverSpeed", -GetAttr("B_HpRecoverSpeed"));
		SetAttr("E_MpRecoverSpeed", -GetAttr("B_MpRecoverSpeed"));
		SetAttr("E_EnergyRecoverSpeed", -GetAttr("B_EnergyRecoverSpeed"));
	}
}

// 初始化激活宠物基本属性
bool CPet::InitialBaseAttrs(const tagPetBase& rPet, long lInitType,long IsWild)
{
	m_BaseAttrs = rPet;
	SetGraphicsID(rPet.lPicID);

	// 取得宠物成长系数配置
	GetInst(CPetCtrl).GetGrowCoefs(m_BaseAttrs.lIndex, m_GrowCoefs);

	if( SAT_CAN_BEATK != CanBeAttacked() )
	{
		God();
	}

	if( NeedBeSaved() )
	{
		m_TempAttrs.cSaveFlag = 1;
	}

	if( rPet.lInvisible > 0 )
	{
		SetInvisibleState(false);
	}

	if( 0 == rPet.lMoveType )
	{
		SetMoveable(FALSE);
	}

	switch( lInitType )
	{
	case IT_CREATE:
		{
			InitialSkills();
			SetPetName(rPet.szOriName);
			//SetActiveAttr(1);
			SetHostName(m_pHost->GetName());

			m_DiffAttrs.lLevel	= 1;
			if (IsServant())
			{
				OnReSetTalent();
				m_DiffAttrs.lLife = m_BaseAttrs.lLife;
				m_DiffAttrs.lLoyalty = m_BaseAttrs.lLoyalty;
				m_DiffAttrs.ServantAttr.lWild = IsWild;
				// 品质
				m_DiffAttrs.ServantAttr.lQuality = GetBaseQualityRadomVal();
			}		
		}
		break;

	case IT_RELIVE:
		{
			switch(GetPetType())
			{
			case PET_TYPE_SERVANT:
				{
					m_DiffAttrs.lLoyalty = m_BaseAttrs.lLoyalty;
				}
				break;
			}
		}
	}

	//SetActiveAttr(m_DiffAttrs.lStateType);
	//if( rPet.lExistTime != -1)
	//{
	//	return false;
	//}
	SetName(m_DiffAttrs.szName);

	return true;
}

// 初始化宠物的存盘或切服的保存属性
bool CPet::InitialDiffAttrs(long lInitType, bool IsWild)
{
	SetWild(IsWild);

	C_PET_LIST rPets = GetInst(CPetCtrl).GetPetsConfig();
	C_ITR_PET_LIST itrPet = rPets.find(m_DiffAttrs.lIndex);
	bool bRes = false;
	if( itrPet != rPets.end() &&
		InitialBaseAttrs(itrPet->second, lInitType,IsWild) )
	{
		bRes = true;
	}

	return bRes;
}

// 设置主人
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

// 从场景中刪除
bool CPet::DelFromRegion(long lOpType)
{
	//if( lOpType != PET_OT_CHANGE_REGION && !m_pAI->IsHibernated() )
	//{
	//	m_pAI->Hibernate();
	//	m_pAI->DeleteAIEvent(CBaseAI::AET_MOVE);
	//}
	if( lOpType != PET_OT_CHANGE_REGION && GetState() != CShape::STATE_HANGUP )
	{
		ChangeStateTo(CShape::STATE_HANGUP);
		m_pAI->Stop();
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

	//SetState(STATE_DELET);
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
	StateAI::PetAI *ai = (StateAI::PetAI*) m_pAI;
	ai->SetHost( m_pHost );
	ai->SetTarget( NULL_GUID, 0 );
	//CancelAllTimer();
	//((CPetAI*)m_pAI)->SetHost(m_pHost);
	//m_pAI->SetTarget(0, NULL_GUID);
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
	tagTimerVar* pTimer = OBJ_CREATE(tagTimerVar);
	pTimer->TimerType = COT_PET_EVENT;
	pTimer->lvar = eType;
	m_Timers[eType] = TIMER_SCHEDULE(this,
		(void*)pTimer,
		lSpringTime,
		lIntervalTime);
}

// 注册渐变事件: 随时间有所损益的所有宠物属性, 目前已知:
// 1) 侍从的HP、MP、内力、忠诚度、寿命
bool CPet::RegShadeEvent(void)
{
	RegEvent(PET_HPRenew, UNI_CONF.nHPMPInt, UNI_CONF.nHPMPInt);				// 宠物刷新HP
	RegEvent(PET_MPRenew, UNI_CONF.nHPMPInt, UNI_CONF.nHPMPInt);				// 宠物刷新MP
	RegEvent(PET_EnergyRenew, UNI_CONF.nHPMPInt, UNI_CONF.nHPMPInt);			// 宠物刷新内力
	RegEvent(PET_LoyaltyRenew, UNI_CONF.loyaDownUnit, UNI_CONF.loyaDownUnit);	// 宠物刷新忠诚度
	RegEvent( PET_LifeRenew , UNI_CONF.lifeSubUnit , UNI_CONF.lifeSubUnit);		// 寿命衰减定时器
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
			m_pAI->SetTarget(pTarget->GetExID(), pTarget->GetType());
		}
		else
		{
			m_pAI->SetTarget( NULL_GUID, 0);
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

// 侍从的HP
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
		char szGUID[64];
		GetExID().tostring(szGUID);
		Log4c::Trace(ROOT_MODULE,"Player%s 's Pet(%s: %s) HP == 0.",m_pHost->GetName(),GetName(),szGUID);
		OnDied();	// 触发死亡
	}

	SetAttr("Hp", lCurHP);
}

// 侍从的MP
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

// 侍从的内力
void CPet::OnEnergyRenew(void)
{
	long lCurEnergy = GetEnergy();
	long lMaxEnergy = GetMaxEnergy();
	long lEnergyRecoverSpeed = GetAttr("C_EnergyRecoverSpeed");
	if (lCurEnergy == lMaxEnergy && lEnergyRecoverSpeed >= 0 
		|| 0 == lEnergyRecoverSpeed)
		return;

	lCurEnergy += lEnergyRecoverSpeed;
	if (lCurEnergy > lMaxEnergy)
	{
		lCurEnergy = lMaxEnergy;
	}
	else if (lCurEnergy < 0)
	{
		lCurEnergy = 0;
	}

	SetAttr("Energy", lCurEnergy);
}

/// 侍从的忠诚度
void CPet::OnLoyaltyRenew(void)
{
	long lLoyaltyRenw = UNI_CONF.loyaDownVal;
	OnChangeLoy(lLoyaltyRenw);

	long loyaltyVal = GetAttr("Loyalty");
	if ( loyaltyVal <= UNI_CONF.loyaltyMin )
	{
        Log4c::Trace(PET_MODULE, FormatText("GS_PET_5",m_pHost->GetName(),GetName(),
            GetExID().tostring(), loyaltyVal, UNI_CONF.loyaltyMin));//Player%s 's Pet(%s: %s) 忠诚度降低为%d，低于%d下限，无法召唤 .
        char szLog[256];
		sprintf(szLog, "宠物%s忠诚度太低，无法召唤", GetName());
		PRINT_TO_CLIENT(szLog);

		// 设置状态为待机状态
		ChangeStateType(PET_STATE_AWAIT);
	}
}

/// 侍从的寿命
void CPet::OnLifeRenew(void)
{
	long lLifeSubRate = UNI_CONF.lifeSubRate;
	OnChangeLife(lLifeSubRate);

	long lifeVal = GetAttr("Life");
	if(lifeVal <= 0)
	{
		Log4c::Trace(PET_MODULE, FormatText("GS_PET_6",m_pHost->GetName(),GetPetName(),GetExID().tostring()));//Player%s 's Pet(%s: %s) 寿命 == 0.
        Log4c::Trace(PET_MODULE, FormatText("GS_PET_7",GetName()));//宠物%s寿命为零，无法召唤
		//PRINT_TO_CLIENT(szLog);

		// 设置状态为沉睡状态
		ChangeStateType(PET_STATE_SLEEP);
	}
}

// 5) 宠物生命终结
void CPet::OnLifeEnd(void)
{

}

// 激活宠物
void CPet::OnActive(void)
{
	SetStateType(PET_STATE_ACTIVE);
	InitAI();
	ChangeStateTo(STATE_PEACE);
	InitSkills();
	RegShadeEvent();
	InitialByHost(IT_LOGIN_GS);
}

// 创建宠物
void CPet::OnCreate(void)
{
	InitNameValueMap();
	InitialTempAttrs();
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

void CPet::SetActiveAttr(long lActiveType)
{
	if( 0 == lActiveType )
	{
		if( IsActive() )
		{
			m_pHost->DelFightPet(GetExID());
		}
		m_DiffAttrs.lStateType = lActiveType;
	}
	else if( lActiveType != 0 )
	{
		m_pHost->AddFightPet(this);
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

/// 计算忠诚度等级
long CPet::CalcLoyLevel(void)
{
	long l = 0;
	long lMaxLv = UNI_CONF.loyaltyLvNum;
	const int* Loy = UNI_CONF.loyalties;
	while( Loy[l] < m_DiffAttrs.lLoyalty && l < lMaxLv ) ++l;

	return l;
}

// 初始化属性字段
void CPet::InitNameValueMap(void)
{
	CMoveShape::InitNameValueMap();
	m_Attrs.clear();

	// 基本属性初始化
	tagPty st;
	char strLog[256];

	static long minValue[7] = { 0x0, 0, 0, (long)-99999999.999f, -0x7FFFFFFF, -0x7FFF, 0};
	static long maxValue[7] = { 0xFF, 0xFFFF, 0x7FFFFFFF, (long)99999999.999f, 0x7FFFFFFF, 0x7FFF, 1};

	long* pVal = NULL;
	long lLevel = m_DiffAttrs.lLevel;

	SET_BASE_LONG_PROP(Hp);								// 气血
	SET_BASE_LONG_PROP(Mp);								// 法力
	SET_BASE_LONG_PROP(Energy);							// 内力

	SET_BASE_LONG_PROP(Loyalty);						// 忠诚度
	SET_BASE_LONG_PROP(Level);							// 等级
	SET_BASE_LONG_PROP(Exp);							// 经验
	SET_BASE_LONG_PROP(Life);							// 寿命
	SET_BASE_LONG_PROP(StateType);						// 状态类型

	SET_SERVANTBASE_LONG_PROP(Wild);					// 野生标志
	SET_SERVANTBASE_LONG_PROP(Ages);					// 代数
	SET_SERVANTBASE_LONG_PROP(DLevel);					// 道行等级
	SET_SERVANTBASE_LONG_PROP(DExp);					// 修为经验
	SET_SERVANTBASE_LONG_PROP(Quality);					// 品质
	SET_SERVANTBASE_LONG_PROP(Speciality);				// 特性偏好
	SET_SERVANTBASE_LONG_PROP(DAfterValCount);			// 后天强化次数
	SET_SERVANTBASE_LONG_PROP(StarLev);					// 星等
	SET_SERVANTBASE_LONG_PROP(StarLevCount);			// 星等强化次数

#define SET_SERVANT_EXTEND_PROPS(TYPE) \
	SET_SERVANT_EXTEND_PROP(TYPE,Strenth); \
	SET_SERVANT_EXTEND_PROP(TYPE,Dexterity); \
	SET_SERVANT_EXTEND_PROP(TYPE,Con); \
	SET_SERVANT_EXTEND_PROP(TYPE,Intellect); \
	SET_SERVANT_EXTEND_PROP(TYPE,Spiritualism)

	//其他侍从属性
	SET_SERVANT_EXTEND_PROPS(talent);
	SET_SERVANT_EXTEND_PROPS(dAfter);
	SET_SERVANT_EXTEND_PROPS(syncretize);
	SET_SERVANT_EXTEND_PROPS(stoneUp);
	SET_SERVANT_EXTEND_PROPS(savvy);

	SET_TEMP_LONG_PROP(MaxExp);							// 经验上限
	SET_TEMP_LONG_PROP(MaxDExp);						// 修为经验上限
	SET_TEMP_LONG_PROP(Race);							// 种族
	SET_TEMP_LONG_PROP(TakeLev);						// 携带等级
	SET_TEMP_LONG_PROP(FightType);						// 战斗模式
	SET_TEMP_LONG_PROP(MoveType);						// 移动模式
	SET_TEMP_LONG_PROP(Country);						// 国家
	SET_TEMP_LONG_PROP(LoyLev);							// 忠诚度等级

	SET_BASE_TEMP_LONG_PROP(MaxHp);						// 气血上限
	SET_BASE_TEMP_LONG_PROP(MaxMp);						// 法力上限
	SET_BASE_TEMP_LONG_PROP(MaxEnergy);					// 内力上限

	SET_BASE_TEMP_LONG_PROP(HpRecoverSpeed);			// 基本Hp回复速度
	SET_BASE_TEMP_LONG_PROP(MpRecoverSpeed);			// 基本Mp回复速度
	SET_BASE_TEMP_LONG_PROP(EnergyRecoverSpeed);		// 基本内力回复速度

	SET_BASE_TEMP_LONG_PROP(Strenth);					// 力量
	SET_BASE_TEMP_LONG_PROP(Dexterity);					// 身法
	SET_BASE_TEMP_LONG_PROP(Con);						// 根骨
	SET_BASE_TEMP_LONG_PROP(Intellect);					// 意志
	SET_BASE_TEMP_LONG_PROP(Spiritualism);				// 灵性

	SET_BASE_TEMP_LONG_PROP(Hit);						// 命中
	SET_BASE_TEMP_LONG_PROP(Dodge);						// 闪避
	SET_BASE_TEMP_LONG_PROP(MAtkSpeed);					// 吟唱速度
	SET_BASE_TEMP_LONG_PROP(PhysicsAtk);				// 物理伤害
	SET_BASE_TEMP_LONG_PROP(PhysicsDef);				// 物理防御
	SET_BASE_TEMP_LONG_PROP(MAtk);						// 魔法攻击
	SET_BASE_TEMP_LONG_PROP(MDef);						// 魔法防御

	SET_BASE_TEMP_LONG_PROP(ElemDef0);					// 水抗
	SET_BASE_TEMP_LONG_PROP(ElemDef1);					// 土抗
	SET_BASE_TEMP_LONG_PROP(ElemDef2);					// 木抗
	SET_BASE_TEMP_LONG_PROP(ElemDef3);					// 金抗
	SET_BASE_TEMP_LONG_PROP(ElemDef4);					// 火抗
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
		Log4c::Trace(PET_MODULE, FormatText("GS_PET_8",szName, lVal));//试图修改基本属性[%s]->%ld.
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
		else if (strcmp("Life", szName) == 0)
		{
			long lMaxVal = GetMaxLife();
			if (lVal < 0) lVal = 0;
			if (lVal > lMaxVal ) lVal = lMaxVal;
		}
		else if (strcmp("Loyalty", szName) == 0)
		{
			long lMaxVal = GetMaxLoyalty();
			if (lVal < 0) lVal = 0;
			if (lVal > lMaxVal ) lVal = lMaxVal;
		}
		if (strcmp("Energy", szName) == 0)
		{
			long lMaxVal = GetMaxEnergy();
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
			break;
		}
		OnPropertyValueChanged(szName, lVal);
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
			Log4c::Trace(PET_MODULE, FormatText("GS_PET_9", szAttr));//添加属性[%s].
		}
		else
		{
			CodeChgAttr(setWriteDB, 0, 0, 0);
			Log4c::Error(PET_MODULE,"%-15s %s",__FUNCTION__, FormatText("GS_PET_10",szAttr));//%-15s 无法添加属性[%s].
		}
	}
}

// 设置宠物等级
void CPet::SetLevel(long lLevel)
{
	long lOldLevel = GetLevel();
	SetAttr("Level", lLevel);
	InitialOrigAttrs(lOldLevel, lLevel);
}

// 修正经验
void CPet::IncExp(long lDelta)
{
	/// 如果是侍从，他的忠诚度会影响他获得的经验
	if ( IsServant() )
	{
		int loyLev = CalcLoyLevel();
		lDelta = lDelta * UNI_CONF.layaExp[loyLev] / 100 ;
	}
	long lExp = m_DiffAttrs.lExp +lDelta;
	if( 0 > lExp )
	{
		Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,FormatText("GS_PET_0", lExp));//得到一个负的经验[%d], 执行返回!
		return;
	}

	// 经验最多存储当前升级经验的3倍
	long lMaxExp = GetAttr("MaxExp") * UNI_CONF.maxExpMultiple;
	if( lExp >= lMaxExp)
	{
		lExp = lMaxExp;
	}

	SetAttr("Exp", lExp);
	UpdateAttribute(TRUE);
	GetGame()->UpdateSavePlayer((CPlayer*)GetHost(), SAVE_DETAIL_PETS);
}

/// 宠物获取经验
void CPet::AddExp(CMonster* pMonster)
{
	if (pMonster==NULL)
		return;

	ulong lDelta = pMonster->GetExpDead() * UNI_CONF.normalExp / 100;

	long levDiff = GetLevel() - pMonster->GetLevel();
	if (levDiff<0)
	{
		IncExp(lDelta);
	}
	else if (levDiff > UNI_CONF.expsMaxLevDiff)
	{
		Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,FormatText("GS_PET_1", UNI_CONF.expsMaxLevDiff));//侍从等级大于怪物等级%d以上, 无法获取经验!
		return;
	}
	else
	{
		lDelta = lDelta * UNI_CONF.levDiffExpRate[levDiff-UNI_CONF.expsBaseLevDiff] /100;
		IncExp(lDelta);
	}
}

/// 宠物获取修为经验
void CPet::AddDExp(CMonster* pMonster)
{
	if (pMonster==NULL)
		return;

	ulong lDelta = pMonster->GetDExpDead() * UNI_CONF.normalDExp / 100;

	long levDiff = GetLevel() - pMonster->GetLevel();
	if (levDiff<0)
	{
		IncDExp(lDelta);
	}
	else if (levDiff > UNI_CONF.expsMaxLevDiff)
	{
		Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,FormatText("GS_PET_2", UNI_CONF.expsMaxLevDiff));
		return;
	}
	else
	{
		lDelta = lDelta * UNI_CONF.levDiffDExpRate[levDiff-UNI_CONF.expsBaseLevDiff] /100;
		IncDExp(lDelta);
	}
}

/// 宠物升级
bool CPet::OnUpgradeLev(void)
{
	long lLevel = GetLevel();
	long lExp = GetAttr("Exp");
	long lMaxExp = GetAttr("MaxExp");
	long lHostLev = m_pHost->GetLevel();

	if (lLevel>=lHostLev+UNI_CONF.moreHostLevel)
	{
		char str[100];
		sprintf_s(str,"宠物级别大于玩家5级，无法进行升级操作!");
		PRINT_TO_CLIENT(str);
		return false;
	}

	if (lExp >= lMaxExp)
	{
		lExp -= lMaxExp;
		SetAttr("Exp", lExp);
		SetAttr("MaxExp", UNI_CONF.Exps[++lLevel]);
		SetLevel(lLevel);

		// 忠诚度随侍从升级增加
		OnChangeLoy(UNI_CONF.loyaUpGrageAdd);

		UpdateAttribute(TRUE);
		GetGame()->UpdateSavePlayer((CPlayer*)GetHost(), SAVE_DETAIL_PETS);
	}
	else
	{
		char str[100];
		sprintf_s(str,"经验不足，无法升级!");
		PRINT_TO_CLIENT(str);
		return false;
	}
	return true;
}

// 修正修为经验
void CPet::IncDExp(long lDelta)
{
	/// 如果是侍从，他的忠诚度会影响他获得的经验
	/*if ( IsServant() )
	{
	int   loyLev =  CalcLoyLevel();
	lDelta = lDelta * UNI_CONF.layaExp[loyLev] / 100 ;
	}*/
	long lDExp = m_DiffAttrs.ServantAttr.lDExp +lDelta;
	if( 0 > lDExp )
	{
		Log4c::Error(ROOT_MODULE,"%-18s %s",__FUNCTION__,FormatText("GS_PET_3", lDExp));//得到一个负的修为经验[%d], 执行返回!
		return;
	}

	long lMaxDExp = GetAttr("MaxDExp") * UNI_CONF.maxDExpMultiple;
	if( lDExp >= lMaxDExp)
	{
		lDExp = lMaxDExp;
	}

	SetAttr("DExp", lDExp);
	UpdateAttribute(TRUE);
	GetGame()->UpdateSavePlayer((CPlayer*)GetHost(), SAVE_DETAIL_PETS);
}
/// 宠物提升道行等级
bool CPet::OnUpgradeDLev(void)
{
	long lLevel = GetLevel();
	long lDLevel = GetDLevel();
	long lDExp = GetAttr("DExp");
	long lMaxDExp = GetAttr("MaxDExp");

	if (lDLevel>=lLevel)
	{
		char str[100];
		sprintf_s(str,"道行不能超过侍从等级!");
		PRINT_TO_CLIENT(str);
		return false;
	}

	if (lDExp >= lMaxDExp)
	{
		lDExp -= lMaxDExp;
		SetAttr("DExp", lDExp);
		SetAttr("MaxDExp", UNI_CONF.DExps[++lDLevel]);
		//SetLevel(lLevel);
		UpdateAttribute(TRUE);
		GetGame()->UpdateSavePlayer((CPlayer*)GetHost(), SAVE_DETAIL_PETS);		
	}
	else
	{
		char str[100];
		sprintf_s(str,"修为不足，无法升级道行!");
		PRINT_TO_CLIENT(str);
		return false;
	}
	return true;
}

// 修正坐标
bool CPet::ModifyPos(INIT_TYPE lInitType)
{
	long lNo = GetPetType() - 1;
	if( lNo < 0 ) lNo = S_MAX_CLASSES - 1;
	bool bRes = false;
	if( IT_SAME_GS == lInitType || Distance(m_pHost) >= UNI_CONF.MaxDistances[lNo] )
	{
		InitialByHost(lInitType);
		SetMoveType(PET_MOVETYPE_FOLLOW);
		bRes = true;
	}

	return bRes;
}

// 设置行走模式
void CPet::SetMoveType(long lType)
{
	if(CanMove())
	{
		SetAttr("MoveType", lType);
		SetTarget(NULL);

		if( PET_FIGHETYPE_FIGHT == GetFightType() )
		{
			m_pAI->Resume( CONTINUE_TIME ); //AddAIEvent(CBaseAI::AET_MOVE);
		}
	}
}

// 设置攻击模式
void CPet::SetFightType(long lType)
{
	if(SAT_CAN_FIGHT == CanFight())
	{
		SetAttr("FightType", lType);
		if( lType != PET_FIGHETYPE_FIGHT )
		{
			SetTarget(NULL);
		}
		else if( NULL == GetTarget() )
		{
			m_pAI->Resume( CONTINUE_TIME ); //AddAIEvent(CBaseAI::AET_MOVE);
		}
	}
}

/// 设置状态模式
void CPet::SetStateType(long lState)
{
	m_DiffAttrs.lStateType = lState;
}

/// 转换状态模式
void CPet::ChangeStateType(long lNewState)
{
	long lOldState = GetStateType();
	switch(lNewState)
	{
		// 沉睡状态, 宠物寿命为零，自动转化为此状态
	case PET_STATE_SLEEP:
		if (lOldState != PET_STATE_ACTIVE)
		{
			//m_pHost->SendNotifyMessage("状态错误，不可能由非激活状态转化为沉睡状态！",NOTIFY_COLOR_RED);
			m_pHost->SendNotifyMessage("[Colour 'ffff0000']状态错误，不可能由非激活状态转化为沉睡状态！");
			return;
		}
		if (!GetInst(CPetCtrl).UnActivePet(m_pHost, GetExID()))
			return;
		m_pHost->SetActivePetGUID(NULL_GUID);
		break;

		// 待机状态 宠物出生为此状态
	case PET_STATE_AWAIT:
		if (lOldState == PET_STATE_ACTIVE)
		{
			if (!GetInst(CPetCtrl).UnActivePet(m_pHost, GetExID()))
				return;
			m_pHost->SetActivePetGUID(NULL_GUID);
		}
		else if (lOldState==PET_STATE_FIGHT)
		{
			m_pHost->DelFightPet(GetExID());
		}
		else
		{
			m_pHost->SendNotifyMessage("[Colour 'ffff0000']状态错误");
			return;
		}
		break;

		// 出战状态
	case PET_STATE_FIGHT:
		if (lOldState==PET_STATE_AWAIT)
		{
			if (GetLoyalty() <= UNI_CONF.loyaltyMin)
			{
				m_pHost->SendNotifyMessage("[Colour 'ffff0000']忠诚度太低，无法出战！");
				return;
			}

			m_pHost->AddFightPet(GetExID());
		}
		else if (lOldState==PET_STATE_ACTIVE)
		{
			if (!GetInst(CPetCtrl).UnActivePet(m_pHost, GetExID()))
				return;
			m_pHost->SetActivePetGUID(NULL_GUID);
			m_pHost->AddFightPet(GetExID());
		}
		else
		{
			m_pHost->SendNotifyMessage("[Colour 'ffff0000']状态错误，不可能由非激活状态或非待机状态转化为沉睡状态！");
			return;
		}
		break;

		// 激活状态
	case PET_STATE_ACTIVE:
		if (lOldState==PET_STATE_FIGHT)
		{
			if (!GetInst(CPetCtrl).ActivePet(m_pHost, GetExID()))
				return;
			m_pHost->SetActivePetGUID(GetExID());
			m_pHost->DelFightPet(GetExID());
		}
		else
		{
			m_pHost->SendNotifyMessage("[Colour 'ffff0000']状态错误，只能由出战状态转化为激活状态！");
			return;
		}
		break;

	default:
		m_pHost->SendNotifyMessage("[Colour 'ffff0000']宠物无此状态，请检查！");
		break;
	}

	SetStateType(lNewState);

	CMessage msg(MSG_S2C_PET_CHANGE_STATE);
	msg.Add(GetExID());
	msg.Add((long)lNewState);
	msg.SendToPlayer(m_pHost->GetExID());
}

/// 获取状态模式
long CPet::GetStateType(void)
{
	return m_DiffAttrs.lStateType;
}

// 获取行走模式
long CPet::GetMoveType(void)
{
	return m_TempAttrs.lMoveType;
}

// 获取攻击模式
long CPet::GetFightType(void)
{
	return m_TempAttrs.lFightType;
}

/// 是否拥有技能
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

/// 创建时初始化技能
long CPet::InitialSkills(void)
{
	// 添加普通攻击技能（默认该技能为侍从自动释放）
	long lID = m_BaseAttrs.OrdinarilySkill.lID;
	long lLev = m_BaseAttrs.OrdinarilySkill.lLevel;
	if( lID != 0 )
	{
		tagSkill tSkill;
		tSkill.lPos = m_TempAttrs.lSkillNum++;
		tSkill.lType = PET_SKILL_ORDINARILY;
		tSkill.lID = lID;
		tSkill.lLevel = lLev;
		tSkill.bAuto = true;
		m_Skills[tSkill.lPos] = tSkill;
		ModifySkill(tSkill);
	}

	// 添加天赋技能（默认侍从的天赋技能只有一种）
	lID = m_BaseAttrs.GeniusSkill.lID;
	lLev = m_BaseAttrs.GeniusSkill.lLevel;
	if( lID != 0 )
	{		
		tagSkill tSkill;
		tSkill.lPos = m_TempAttrs.lSkillNum++;
		tSkill.lType = PET_SKILL_GENIUS;
		tSkill.lID = lID;
		tSkill.lLevel = lLev;
		m_Skills[tSkill.lPos] = tSkill;
		ModifySkill(tSkill);
	}

	return 0 ;
}

/// 获取技能
tagSkill* CPet::GetSkill(long lPos)
{
	ITR_SKILL_LIST itr = m_Skills.find(lPos);
	if( itr != m_Skills.end() )
		return &itr->second;
	return NULL;
}

/// 设置宠物技能快捷键
void CPet::SetSkillHotKey(long lPos, long lActPos)
{
	tagSkill* pSkill = GetSkill(lPos);
	if( pSkill != NULL )
	{
		pSkill->lActPos = lActPos;
	}
}

/// 检查技能修正条件
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
		char szGUID[128];
		GetExID().tostring(szGUID);
        Log4c::Error(PET_MODULE,"%-15s 宠物%s在修改技能%ld(Lv.%ld)时出错: 技能不存在",__FUNCTION__,szGUID, rSkill.lID, rSkill.lLevel);
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

/// 修正技能（侍从技能镶嵌时覆盖原有技能）
bool CPet::ModifySkill(tagSkill& rSkill)
{
	CNewSkill* pSkill = CanModifySkill(rSkill);
	CNewSkillProperties::tagSkill *pTSkill = NULL;
	if( NULL == pSkill || NULL == (pTSkill = pSkill->GetNewSkill(rSkill.lLevel)) )
	{
		char szGUID[128];
		GetExID().tostring(szGUID);
        Log4c::Error(PET_MODULE,"%-15s %s",__FUNCTION__,FormatText("GS_PET_11",szGUID, rSkill.lID, rSkill.lLevel));//宠物%s在修改技能%ld(Lv.%ld)时出错: 等级不存在
		return false;
	}

	CMessage msg(MSG_S2C_SKILL_ADD_PET);
	msg.Add(GetExID());
	msg.AddEx(&rSkill, sizeof(rSkill));
    msg.Add(GetSkillValue( rSkill.lID , rSkill.lLevel, "CoolDown" ));
    msg.Add((float)GetSkillValue( rSkill.lID , rSkill.lLevel , "MaxAtkDistance" ));
    msg.Add((float)GetSkillValue( rSkill.lID , rSkill.lLevel , "MinAtkDistance" ));
    msg.Add(GetSkillValue( rSkill.lID , rSkill.lLevel , "SMp" ));
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
	if( NULL == pTSkill )
	{
		pTSkill = AddSkill(tagSkill());
	}

	pTSkill->lID = lID;
	pTSkill->lLevel = lLevel;
	return ModifySkill(*pTSkill);
}

/// 添加技能
tagSkill* CPet::AddSkill(tagSkill& rSkill)
{
	if( 0 == rSkill.lPos ) rSkill.lPos = (long)m_Skills.size();
	m_Skills[rSkill.lPos] = rSkill;
	return &m_Skills[rSkill.lPos];
}

/// 通过技能物品添加技能 @技能镶嵌 －技能物品配置说明
bool CPet::AddSkill(CGoods* pGoods, long lPos, long lHoleType)
{
	tagSkill tSkill;
	tSkill.lID = pGoods->GetAddonPropertyValues(GAP_ADD_SKILL, 1);
	tSkill.lLevel = pGoods->GetAddonPropertyValues(GAP_ADD_SKILL, 2);
	tSkill.lType = pGoods->GetAddonPropertyValues(GAP_COLOR, 1);
	tSkill.lPos = lPos;

	if (lHoleType!=tSkill.lType)
	{
		// 提示 技能槽不对应
		return false;
	}

	if(CanAddSkill(pGoods))
	{
		// 镶嵌成功率
		int addRate = pGoods->GetAddonPropertyValues(GAP_COLOR, 1);
		if (random(100)>addRate)
		{
			// 提示 镶嵌失败 #人品啊
			// 删除物品，删除当前栏位技能
			m_pHost->DelGoods(pGoods->GetExID(), 1);
			m_Skills.erase(lPos);
			return false;
		}
        return AddSkill(tSkill)!=NULL;        
	}
	return false;
}

/// 技能镶嵌条件判断
bool CPet::CanAddSkill(CGoods* pGoods)
{
	//@ 物品附加栏位值未确定，这里先用@GAP_ADD_SKILL 先代替
	int addLev = pGoods->GetAddonPropertyValues(GAP_ADD_SKILL, 1);
	int needMoney = pGoods->GetAddonPropertyValues(GAP_ADD_SKILL, 1);
	long preSkillID = pGoods->GetAddonPropertyValues(GAP_ADD_SKILL, 1);
	if (GetLevel()<addLev)
	{
		// 提示 等级不足
		return false;
	}
	if (m_pHost->GetMoney()<needMoney)
	{
		// 提示 玩家金钱不足
		return false;
	}
	if (preSkillID!=0)
	{
		if (!HasSkill(preSkillID))
		{
			// 提示 未有前置技能
			return false;
		}
	}
	return true;
}

/// 检查技能使用条件
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

	long lActTime = GetSkillValue( pSkill->lID , pSkill->lLevel, "ActTime" );
	if ( lActTime > 0 &&
		IsInAct() )
	{
		return SKILL_USE_FAILED_INVALID_ACT;
	}

	m_TempAttrs.lLastUseSkillTime = lCurTime;
    pSkill->lEndCooldown = lCurTime + GetSkillValue( pSkill->lID , pSkill->lLevel, "CoolDown" );

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
		StateAI::PetAI *ai = (StateAI::PetAI*) m_pAI;
		ai->Attack(pSkill->lID, pTarget->GetTileX(), pTarget->GetTileY(), pTarget);
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

	Script::RunEnv env( this, NULL, static_cast<CRegion*>( GetFather() ) );
	GetInst( ScriptSys ).RunScript( env, szScriptName );
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

	if(bExData)
	{
		long lNum = (long)m_Skills.size();
		setWriteDB.AddToByteArray(lNum);
		ITR_SKILL_LIST itr = m_Skills.begin();
		bool bFlag = false;
		CNewSkill* pSkillProperty = NULL;
		CNewSkillProperties::tagSkill* pSkill = NULL;
		long lCurTime = timeGetTime();
		long lRestTime = 0;
		for( ; itr != m_Skills.end(); ++itr )
		{
			bFlag = false;
			setWriteDB.AddToByteArray((long)itr->first);           
            setWriteDB.AddToByteArray((long)GetSkillValue( itr->second.lID , itr->second.lLevel, "CoolDown" ));
            setWriteDB.AddToByteArray((float)GetSkillValue( itr->second.lID , itr->second.lLevel,"MinAtkDistance" ));
            setWriteDB.AddToByteArray((float)GetSkillValue( itr->second.lID , itr->second.lLevel,"MaxAtkDistance" ));
            setWriteDB.AddToByteArray((DWORD)GetSkillValue( itr->second.lID , itr->second.lLevel,"SMp" ));
			lRestTime = lCurTime - itr->second.lEndCooldown;
			if( lRestTime < 0 ) lRestTime = 0;
			setWriteDB.AddToByteArray(lRestTime);
		}
	}
	else
	{
		setWriteDB.AddToByteArray(GetPetType());
		setWriteDB.AddToByteArray(m_DiffAttrs.lLevel);
		setWriteDB.AddToByteArray(m_DiffAttrs.lHp);
		setWriteDB.AddToByteArray(m_DiffAttrs.lMp);
		setWriteDB.AddToByteArray(m_DiffAttrs.lEnergy);
		setWriteDB.AddToByteArray(GetMaxHP());
		setWriteDB.AddToByteArray(GetMaxMP());
		setWriteDB.AddToByteArray(GetMaxEnergy());
	}
	return true;
}

bool CPet::CodeAttrToClient(DBWriteSet& setWriteDB)
{
	setWriteDB.AddToByteArray(&m_DiffAttrs, sizeof(tagPetDiff));
	setWriteDB.AddToByteArray(&m_TempAttrs, sizeof(tagPetTemp));
	long lNum = (long)m_Skills.size();
	setWriteDB.AddToByteArray(lNum);
	ITR_SKILL_LIST itr = m_Skills.begin();
	for( ; itr != m_Skills.end(); ++itr )
	{
		setWriteDB.AddToByteArray(&itr->second, sizeof(tagSkill));
	}

	return true;
}

// GS==>WS编码
bool CPet::CodeToDataBlock(DBWriteSet& setWriteDB,
						   bool bExData)
{
	CMoveShape::CodeToDataBlock(setWriteDB, bExData);

	setWriteDB.AddToByteArray(&m_DiffAttrs, sizeof(m_DiffAttrs));
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
	setReadDB.GetBufferFromByteArray(m_HostID);		// 主人
	long lSkillNum = setReadDB.GetLongFromByteArray();
	tagSkill tSkill;
	for( long l=0; l<lSkillNum; ++l )
	{
		setReadDB.GetBufferFromByteArray(&tSkill, sizeof(tSkill));
		AddSkill(tSkill);							// 添加技能
	}

	// 初始化基本属性和差异属性
	bool isWild = m_DiffAttrs.ServantAttr.lWild?true:false;
	if (!InitialDiffAttrs(IT_LOGIN_GS,isWild))
		return false;

	/// 初始化临时属性
	OnCreate();
	return true;
}

CMoveShape* CPet::GetHost(void)
{
	if(NULL == m_pHost)
	{
        Log4c::Error(PET_MODULE,"%-15s %s",__FUNCTION__,FormatText("GS_PET_12",GetExID().tostring()));
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
		CBaseObject* pObj = pRgn->FindChildObject(eType, rID);
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


/// 宠物类型是否是侍从
bool CPet::IsServant(void)
{
	return (GetPetType()==PET_TYPE_SERVANT)? true: false;
}

/// 随机天资算法
long CPet::GetBaseTalentRadomVal(void)
{
	int	 randx = random(100);
	int  i = 0 , sum = 0 , nsum = 0;
	for (i = 0 ; i < PET_QUALITY_NUM;  ++i)
	{
		long lrand = UNI_CONF.talentAlogRate[i][2] - UNI_CONF.talentAlogRate[i][1] +1;
		sum += UNI_CONF.talentAlogRate[i][0];
		if ( randx < sum && nsum <= randx)
		{
			long talent = UNI_CONF.talentAlogRate[i][1] + random( lrand );
            Log4c::Trace(PET_MODULE,"%d.",talent);
			return talent;
		}
		nsum = sum;
	}  
    Log4c::Error(PET_MODULE,"%-15s %s",__FUNCTION__,GetText("GS_PET_13"));//随机天资算法错误，请检查！
	return 0;
}

/// 品质随机算法
long CPet::GetBaseQualityRadomVal(void)
{
	int randx = random(10000);
	for (int i = 0; i < PET_QUALITY_NUM; ++i)
	{
		int sum = m_BaseAttrs.lQualityRate[i];
		if (sum>randx)
		{
			return i;
		}
	}
    Log4c::Error(PET_MODULE,"%-15s %s",__FUNCTION__,GetText("GS_PET_14"));//品质随机算法错误，请检查！
	return -1;
}


/// 天资重置检测
//bool CPet::OnResetTalentCheck()
//{
//	if (!IsServant()) return false;
//
//	int  lev = m_DiffAttrs.lLevel;
//	int	 index = 1;
//	if ( lev <= UNI_CONF.nResetLev )
//		index = 0;
//	SetAttr("TalentLevel",index);
//	return true ;
//}

/// 天资重置算法
bool CPet::OnReSetTalent()
{
	if (!IsServant()) return false;

#define CALC_TALENT(ATTR) \
	m_DiffAttrs.ServantAttr.talentVal.l##ATTR = (GetBaseTalentRadomVal() * \
	(m_BaseAttrs.lMax##ATTR-m_BaseAttrs.lMin##ATTR)) / 100 + m_BaseAttrs.lMin##ATTR

	// 天资随机
	CALC_TALENT(Strenth);
	CALC_TALENT(Dexterity);
	CALC_TALENT(Con);
	CALC_TALENT(Intellect);
	CALC_TALENT(Spiritualism);

	// 等级置1	
	m_DiffAttrs.lLevel = 1; 
	// 经验	
	m_DiffAttrs.lExp   = 0;

	// 道行等级置1
	m_DiffAttrs.ServantAttr.lDLevel = 1;
	// 修为
	m_DiffAttrs.ServantAttr.lDExp = 0;

	// 特性偏好
	m_DiffAttrs.ServantAttr.lSpeciality = random(PET_BASE_NUM);

	// 悟性点
	memset(&m_DiffAttrs.ServantAttr.savvyVal,0,sizeof(tagPetBaseAttr));
	// 魂石强化置0
	memset(&m_DiffAttrs.ServantAttr.stoneUpVal,0,sizeof(tagPetBaseAttr));

	return true;
}

/// 后天资质强化检测
bool CPet::DAfterValCheck(void)
{
	int	DAfterCount = m_DiffAttrs.ServantAttr.lDAfterValCount;

	// 当前强化次数 小于 最大可强化次数<此处以宠物的品级值来判断>
	if ( DAfterCount >= m_BaseAttrs.lGrade)
	{
		return false;
	}
	return true;
}

/// 后天资质强化
bool CPet::OnDAfterValUpGrade(long DVal)
{
	if (!IsServant()) return false;

	int val;
	int	DAfterCount = m_DiffAttrs.ServantAttr.lDAfterValCount;
	int srval = UNI_CONF.dAfterStartRate - (DAfterCount-1)*UNI_CONF.dAfterDropRate;

#define DAFTER_UPGRADE(ATTR) \
	val   = DVal * (random(100-srval+1)+srval) /100; \
	m_DiffAttrs.ServantAttr.dAfterVal.l##ATTR += val;

	DAFTER_UPGRADE(Strenth);
	DAFTER_UPGRADE(Dexterity);
	DAFTER_UPGRADE(Con);
	DAFTER_UPGRADE(Intellect);
	DAFTER_UPGRADE(Spiritualism);

	// 增加后天资质强化次数
	m_DiffAttrs.ServantAttr.lDAfterValCount++;

	return true;
}

/// 重置后天资质强化次数
bool CPet::OnResetDAfterCnt(void)
{
	if (!IsServant()) return false;

	if ( m_DiffAttrs.ServantAttr.lDAfterValCount < 1 )
	{
		// 后天资质强化次数<1 提示信息“当前侍从并没有被强化资质，无需进行重塑”
		return false;
	}
	m_DiffAttrs.ServantAttr.dAfterVal.ReSet();
	m_DiffAttrs.ServantAttr.lDAfterValCount = 0;
	return true;
}

/// 侍从融合
bool CPet::OnServantSyncretism(void)
{
	// 条件判断
	return false;
}

/// 侍从融合算法
void CPet::CopyServantAttr(const CPet* pPet, const CPet* lPet, long lPLev)
{	
	// 重新计算天资
	CALC_ELF_SYNCRETISM(pPet, lPet, lPLev);

	m_DiffAttrs.ServantAttr.lAges++;
	m_DiffAttrs.lLevel = 1;
}

/// 融合计算公式<以后整理>
void CPet::CALC_ELF_SYNCRETISM(const CPet* pPet, const CPet* lPet, long lPLev)
{
	m_DiffAttrs.ServantAttr.talentVal.lStrenth = 
		pPet->m_DiffAttrs.ServantAttr.talentVal.lStrenth 
		+ lPet->m_DiffAttrs.ServantAttr.talentVal.lStrenth / 3 * lPLev;

	m_DiffAttrs.ServantAttr.talentVal.lDexterity = 
		pPet->m_DiffAttrs.ServantAttr.talentVal.lDexterity 
		+ lPet->m_DiffAttrs.ServantAttr.talentVal.lDexterity / 3 * lPLev;

	m_DiffAttrs.ServantAttr.talentVal.lCon = 
		pPet->m_DiffAttrs.ServantAttr.talentVal.lCon 
		+ lPet->m_DiffAttrs.ServantAttr.talentVal.lCon / 3 * lPLev;

	m_DiffAttrs.ServantAttr.talentVal.lIntellect = 
		pPet->m_DiffAttrs.ServantAttr.talentVal.lIntellect 
		+ lPet->m_DiffAttrs.ServantAttr.talentVal.lIntellect / 3 * lPLev;

	m_DiffAttrs.ServantAttr.talentVal.lSpiritualism = 
		pPet->m_DiffAttrs.ServantAttr.talentVal.lSpiritualism 
		+ lPet->m_DiffAttrs.ServantAttr.talentVal.lSpiritualism / 3 * lPLev;
}

/// 品质强化算法条件判断
bool CPet::OnCElfPensValueUpGradeCheck()
{
	if (!IsServant())	return false;

	int quality = m_DiffAttrs.ServantAttr.lQuality;
	if (quality == PET_QUALITY_BRILLIANT)
	{
		/// 已经是最高品质
		char str[100];
		sprintf_s(str,"品质不能再强化!");
		PRINT_TO_CLIENT(str);
		return false;
	}
	if ( m_DiffAttrs.lLevel < UNI_CONF.qualityMinLev)
	{
		/// 等级不够
		/// CMessage msg(MSG_S2C_PET_ELF_UPGRADE_MSGTYPE);
		/// msg.Add(PET_ELF_LEVELUPGR_MSGTYPE);
		char str[100];
		sprintf_s(str,"等级不够!");
		PRINT_TO_CLIENT(str);
		return false;
	}

	int lExp = m_DiffAttrs.lExp;
	// rate为配置信息
	if ( lExp < UNI_CONF.Exps[m_DiffAttrs.lLevel] * UNI_CONF.qualityExpProp)	
	{
		/// 经验不够,发送强化失败消息
		char str[100];
		sprintf_s(str,"经验不够!");
		PRINT_TO_CLIENT(str);
		return false;
	}
	// 虚空精华数目计算
	//int  NNum = (lev +1)*UNI_ELFCONF.QDelta[lev] * m_BaseAttrs.lSNiceNum ;
	//SetAttr("NENice",NNum);
	return true;
}

/// 品质强化算法算法
bool CPet::OnCElfPensValueUpGrade(long val)
{
	///  附加的成功率
	int DropVal = GetDownRate(UNI_CONF.qualityLoyaProp);	
	/// 获取强化成功率
	int SuccessVal = (100 + DropVal) >= 100 ? 100 : (100 + DropVal) ;
	SuccessVal +=val;

	int srval = random(100)+1;
	if ( srval <= SuccessVal )
	{
#ifdef	_DEBUG
		/// 强化成功
		/// CMessage msg(MSG_S2C_PET_ELF_UPGRADE_MSGTYPE);
		/// msg.Add(PET_ELF_PENVALUEUPGR_MSGTYPE);
		/// 经验-5%
		char str[100];
		sprintf_s(str,"品质强化成功!");
		PRINT_TO_CLIENT(str);
#endif
		return true;
	}
	/// 强化失败
	else
	{
#ifdef	_DEBUG
		char str[100];
		sprintf_s(str,"品质强化失败!");
		PRINT_TO_CLIENT(str);
#endif
		return false;
	}
}

/// 取得品质强化所需虚空精华数目
long CPet::GetLessNice()
{
	int quality = m_DiffAttrs.ServantAttr.lQuality;
	//return (quality +1)*UNI_CONF.QDelta[lev] * m_BaseAttrs.lSNiceNum ;
	return 1;
}

/// 分配悟性点
bool  CPet::SetSawyPointValue(const tagPetBaseAttr& rtag) 
{
	long  lSawyV = m_DiffAttrs.ServantAttr.savvyVal.GetTotalVal();
	long  total  = rtag.GetTotalVal() + lSawyV;

	long  slev   = m_DiffAttrs.lLevel * UNI_CONF.savvyCnt;
	/// 悟性点总合 等于 悟性等级
	if ( total <= slev )
	{
		m_DiffAttrs.ServantAttr.savvyVal.SetElfBase(rtag);
		return true;
	}
	return false;
}

/// 重置悟性点
bool CPet::OnResetSavvy(void)
{
	if (!IsServant())	return false;

	m_DiffAttrs.ServantAttr.savvyVal.ReSet();
	return true;
}


/// 魂石强化条件判断
bool CPet::OnFStoneUpgradeCheck()
{
	if (!IsServant())	return false;

	/// 等级
	long  lev   =  m_DiffAttrs.lLevel;
	long  levLimit = /*UNI_CONF.nFSUpGradeMulite*/10 * m_DiffAttrs.ServantAttr.lFStoneUpgradeFlag + UNI_CONF.fStoneMinLev;
	/// 判断等级
	if ( lev >= UNI_CONF.fStoneMinLev && lev <= UNI_CONF.fStoneMaxLev  && lev >= levLimit ) 
	{
		int lExp = m_DiffAttrs.lExp;
		/// 判断经验
		if ( lExp >= (UNI_CONF.Exps[lev] * UNI_CONF.fStoneExpProp / 100) ) 
		{
#ifdef	_DEBUG
			/// 经验不够,发送强化失败消息
			char str[100];
			sprintf_s(str,"经验不够,不能进行魂石强化");
			PRINT_TO_CLIENT(str);
#endif
			return true;
		}
	}	
	return  false;
}

/// 魂石强化
bool  CPet::OnFStoneUpgrade(long fstone)
{
	// 等级
	long  lev   =  m_DiffAttrs.lLevel;
	long  lsfup =  m_DiffAttrs.ServantAttr.lFStoneUpgradeFlag;
	long  rTemp =  UNI_CONF.fStoneSuccesRate[lsfup];	
	long  Addval=  fstone;			

	/// 现在的成功率
	rTemp *=(1.0 + Addval/100.0);			
	int lExp = m_DiffAttrs.lExp;
	if ( lExp < (UNI_CONF.Exps[lev] * UNI_CONF.fStoneExpProp / 100) )
	{
		// 经验不够,发送强化失败消息
		return false;
	}

	/// 随机值，
	int srandx = random(1000)+1;
	/// 强化成功
	if ( srandx <= rTemp )
	{
		/// 经验 -5%   lsfup+1   
		/// 基本属性+++
		char str[100];
		sprintf_s(str,"魂石强化成功");
		PRINT_TO_CLIENT(str);
		return true;
	}
	/// 没有成功
	else
	{
		char str[100];
		sprintf_s(str,"魂石强化失败");
		PRINT_TO_CLIENT(str);
		return false;
	}
}


/// 顿悟神位强化条件检测
bool CPet::OnlFUpgradeCheck()
{
	if (!IsServant())	return false;

	int lev = m_DiffAttrs.lLevel;
	if (lev < UNI_CONF.exclamaMinLev)
	{
		char str[100];
		sprintf_s(str,"顿悟神位强化等级不够");
		PRINT_TO_CLIENT(str);
		return false;
	}
	return true;
}

/// 根据宠物忠诚度强化成功率的百分比
int  CPet::GetDownRate(int drop)
{
	int DownVal  = 0;
	int lLoyalty = GetAttr("Loyalty");//m_DiffAttrs.lLoyalty;

	if (lLoyalty< UNI_CONF.exclamaLoyaProp)
	{
		DownVal -=drop;
	}

	return DownVal;
}

/// 顿悟神位强化 
bool CPet::OnlFUpgrade( long rate )
{
	/// 附加成功概率
	int fsval = m_DiffAttrs.ServantAttr.lFUpgradeCount;
	int rDrop = GetDownRate(UNI_CONF.exclamaDropProp);	
	int randx = random(10000)+1;


	/// 万分比
	float rTemp = UNI_CONF.exclamaSuccesRate[fsval] * 10 ;
	rTemp *= (1.0 + rDrop/100.0+rate/100.0)*10;

	/// 强化成功
	if ( randx <= rTemp )
	{
		fsval ++;
		m_DiffAttrs.ServantAttr.lFUpgradeCount ++;

		char str[100];
		sprintf_s(str,"顿悟神位强化成功");
		PRINT_TO_CLIENT(str);
		/// Send to Client operator Successful
		return true;
	}
	else	/// 强化失败，降低升级经验
	{
		/// 经验-5% 
		int lExp = m_DiffAttrs.lExp;
		lExp = lExp * (100 - UNI_CONF.exclamaExpProp) /100.0;

		m_DiffAttrs.lExp = lExp;
		char str[100];
		sprintf_s(str,"顿悟神位强化失败");
		PRINT_TO_CLIENT(str);
		return false;
	}
}

/// 星等强化条件检测
bool CPet::OnStarLevUpGradeCheck()
{
	if (!IsServant())	return false;

	int Slev= GetAttr("StarLev");
	if ( Slev > UNI_CONF.starMaxLev)
	{
		return false;
	}
	return true;
}

/// 星等强化
bool CPet::OnStarLevUpGrade(long rates)
{
	/// 提高强化成功率
	int rate = rates;			
	//m_AddUpGradeRate = 0;
	int Slev = GetAttr("StarLev");
	int rRate= (100 + rate)/100 * UNI_CONF.starSuccessRate[Slev];
	int randx= random(100)+1;
	/// 强化成功 
	if ( randx < rRate)
	{
		Slev ++;
		if ( Slev >= 5 )
		{
			/// 发送【系统】想学习你千辛万苦，精灵提升
			char str[100];
			sprintf_s(str,"星等强化成功,发送【系统】想学习你千辛万苦，精灵提升");
			PRINT_TO_CLIENT(str);
		}
		else 
		{
			/// 发送 【系统】你强化精灵成功
			char str[100];
			sprintf_s(str,"星等强化成功,发送【系统】:你强化精灵成功");
			PRINT_TO_CLIENT(str);
		}
		return true;
	}
	else
	{
		/// 发送 【系统】你强化精灵失败.
		char str[100];
		sprintf_s(str,"星等强化成功,发送【系统】:你强化精灵失败");
		PRINT_TO_CLIENT(str);
		return false;
	}
}

/// 改变忠诚度
void CPet::OnChangeLoy(long lDelta)
{
	long tLoyalty = GetAttr("Loyalty");//m_DiffAttrs.lLoyalty ;
	long tMaxLoyalty = UNI_CONF.loyaltyMax;

	tLoyalty += lDelta;
	if ( tLoyalty > tMaxLoyalty )
	{
		tLoyalty = tMaxLoyalty;
	}
	else if (tLoyalty<=0)
	{
		tLoyalty = 0;
	}
	SetAttr("Loyalty",tLoyalty);

	// 更新忠诚度等级状态
	int oldLoyLev = GetAttr("LoyLev");
	int newLoylev = CalcLoyLevel();
	SetAttr("LoyLev", newLoylev);

	if ( newLoylev != oldLoyLev )
	{
        char szLog[256];
		sprintf(szLog, "忠诚度等级由%d更新为%d.", oldLoyLev, newLoylev);
		PRINT_TO_CLIENT(szLog);
	}
    Log4c::Error(PET_MODULE,"%-15s %s",__FUNCTION__,FormatText("GS_PET_15",tLoyalty));//更新忠诚度值:: [Loyalty ]-->%ld.
}

/// 改变寿命值
void CPet::OnChangeLife(long lDelta)
{
	long tLife = GetAttr("Life");
	long tMaxLife = UNI_CONF.maxLife;

	tLife += lDelta;
	if ( tLife > tMaxLife)
	{
		tLife = tMaxLife;
	}
	else if (tLife<=0)
	{
		tLife = 0;
	}
	SetAttr("Life",tLife);
    Log4c::Error(PET_MODULE,"%-15s %s",__FUNCTION__,FormatText("GS_PET_16",tLife));//寿命值更新: [Life ]-->%ld.
}

/// 最大精灵携带数的增加
long CPet::AddServantTakeNum(long num)
{
	CPlayer * Phost = (CPlayer*)GetHost();
	//return Phost->ChangeMaxServantNum(num);
	return 1;
}

///// 品质强化时，检测玩家精灵
//const CGUID & CPet::IsTypeElfLessLev()
//{
//	return m_pHost->GetQueryElfTypeWithLev(GetLevel(),m_DiffAttrs.szName);
//}

/// 删除宠物
//long    CPet::DelSameElf(CGUID & guid)
//{
//	CPet *pPet = GetInst(CPetCtrl).GetPet(guid);
//	GetInst(CPetCtrl).DelPet(pPet,PET_OT_DELETE);	
//	return 0;
//}


/// 得到Lev需要的升级经验
inline	long CPet::GetALevNeedExp(long  _lev)
{		
	if ( _lev < UNI_CONF.lMaxLevel )
	{
		return UNI_CONF.Exps[_lev];
	}
	return -1;
}

///  获得每次升级基本属性的提升量
inline long	CPet::GetUpGradeUpValue(long _talentval , long _BaseVal)
{
	//return (GetTalentValue(_talentval) - 1.0 ) * _BaseVal + _BaseVal;
	return 0;
}

/// 召唤精灵, 判断其条件
bool   CPet::VacationPet( long lev )
{
	/// 忠诚度太低不能进行召唤
	int lLoyalty=m_DiffAttrs.lLoyalty;		// GetAttr("Loyalty");


	if ( lLoyalty < UNI_CONF.loyaltyMin)
	{
		char str[100];
		sprintf_s(str,"召唤由于等级限制忠诚度太低!");
		PRINT_TO_CLIENT(str);
		return false;
	}
	if ( m_DiffAttrs.lLevel < lev )
	{
#ifdef _DEBUG
		char str[100];
		sprintf_s(str,"召唤精灵由于等级限制，而失败!");
		PRINT_TO_CLIENT(str);
#endif
		return false;
	}
	return true;
}

/// 取得宠物品级
long CPet::GetGrade(void)
{
	if (!IsServant())
	{
		return -1;
	}
	return m_BaseAttrs.lGrade;
}

/// 取得宠物后天资质已强化次数
long CPet::GetDAfterCnt(void)
{
	if (!IsServant())
	{
		return -1;
	}
	return m_DiffAttrs.ServantAttr.lDAfterValCount;
}

bool CPet::OnServantUpGrade(long uId , long uRate)
{
	if ( uId == PET_QUALITYUP_ID )
	{
		return OnCElfPensValueUpGrade(uRate);
	}
	//else if ( uId == PET_FFUPGRADEID )
	//{
	//	return OnlFUpgrade( uRate );
	//}
	else if ( uId == PET_FSTONEUP_ID )
	{
		return OnFStoneUpgrade(uRate);
	}
	else if ( uId == PET_STARTUP_ID)
	{
		return OnStarLevUpGrade( uRate );
	}
	else if ( uId == PET_TALENT_RESET_ID)
	{
		return OnReSetTalent();
	}
	return false;
}