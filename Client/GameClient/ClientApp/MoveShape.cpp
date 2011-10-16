#include "stdafx.h"
#include "MoveShape.h"
#include "player.h"
#include "monster.h"
#include "Effect.h"
#include "ClientRegion.h"
#include "Other/StateListXml.h"
#include "Other\DeputyOccuSystemClient.h"
#include "Skills/SkillListXml.h"
#include "Skills/SkillListIni.h"
#include "../../EngineExtend\gamemodelmanager.h"
#include "../../EngineExtend\textureprojective.h"
#include "../../EngineExtend\gamemap.h"
#include "../../EngineExtend\DisplayModel.h"
#include "other/Chat.h"
#include "Other\FaceActionList.h"

#include "Other/AudioList.h"
#include "PetSystem/Pet.h"
#include "Player.h"
#include "../../GameClient/GameControl.h"
#include "../../GameClient/ClientApp/Skills/SkillXml.h"
#include "../GameClient/Game.h"
#include "../../Public/Common/Public.h"
#include "Other/GlobeSetup.h"
#include "../../Public/Common/SkillRelated.h"
//Vector3 d;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//#define  NOTANGPENG

long CMoveShape::s_lIdleMinCount = 1000;		// 最早进入idle动作的记数
long CMoveShape::s_lIdleMaxCount = 50000;	// 最迟进入ilde动作的记数
DWORD CMoveShape::m_dwMoveTimeCounter = 0;
DWORD CMoveShape::m_dwLastMoveTimeCounter = 0;

float	CMoveShape::highhurt_monster = 0.0f;
float	CMoveShape::highhurt_boss = 0.0f;
float	CMoveShape::highhurt_knight = 0.0f;
float	CMoveShape::highhurt_bow = 0.0f;
float	CMoveShape::highhurt_magic = 0.0f;
float	CMoveShape::mildhurt_monster  = 0.0f;
float	CMoveShape::mildhurt_boss  = 0.0f;
float	CMoveShape::mildhurt_knight  = 0.0f;
float	CMoveShape::mildhurt_bow  = 0.0f;
float	CMoveShape::mildhurt_magic  = 0.0f;

CMoveShape::CMoveShape(void)
{
	m_fDestX = .5f;
	m_fDestY = .5f;
	m_lStopCount = 0;
	m_lIdleCount = 0;
    m_moveOffsetX = 0;
    m_moveOffsetY = 0;
	m_bIdleStart = false;

	m_strTalkText = "";
	m_lTalkLiveTimer = 1000;

	m_pForceMove=NULL;
	m_bInForceMoving=false;

	m_lMoveSoundCount = 13;
	m_dwEndMoveTime = 0;
	m_dwLastUpdatePosTime = 0;

	m_dwCurSkillActTime = 0;
	m_dwCurSkillStartTime = 0;
	// 平滑移位相关
	m_bIsSlipState = false;
	m_fSlipSpeed = 0.f;
	m_fSlipDestPosX = 0.f;
	m_fSlipDestPosY = 0.f;
	m_fSlipDir = 0.f;
	m_dwSlipEndMoveTime = 0;
	m_dwSlipLastUpdatePosTime = 0;

	m_bIsArrive = true;

	//对应采集物名字
	m_strCollectionName = "";
	//采集物id
	m_dwCollectID = 0;
	//能被采集的次数
	m_dwCollectTimes = 0;
	//是否正在被采集
	m_bIsBeenCollect = false;
	//是否能被采集
	m_bCanBeenCollect = false;

	m_bRun					= true;			// 跑
	m_bAntiInvisibleable    = true;			// 不能反隐形
	m_bInvisible			= true;			// 不能隐形	
	m_bPretendDead			= true;			// 不能装死	

    m_sMoveable					= 1;			//可移动标志符
    m_sSelfControl				= 1;			//可控制标志符
    m_sUseSkill					= 1;			//可使用技能标志符
    m_sMoveDamage				= 1;			//可移除技能作用标志符
    m_sBeenAttack				= 1;			//可被攻击标志符
    m_sRandomMove				= 1;		    //不随机跑动

	m_bUseRabbSkill			= true;			// 可以使用法师技能
	m_bUseFighterSkill		= true;			// 可以使用战士技能
	m_bUseItem				= true;			// 可以使用物品
	m_bInstruction			= true;			// 可以使用指令
	m_bUnAttack				= true;			// 可以攻击标志符
	m_bAddHp				= true;			// 可以hp增加标志符
	m_bHide					= true;			// 不能隐蔽
	m_bForceAttact			= true;			// 不能强制攻击
	
	m_lChangeGraphicstype	= 0;
	m_lChangeGraphicsID		= 0;
	m_bNotRide					= true;			// 没有乘骑
	m_bStateColorPlay = FALSE;
	m_bColorFade	  = true;
	m_bColorFadeFlag = true;
	m_fColorFadePer = 1.0f;
	ZeroMemory(&m_CurrentStateExForStateColor, sizeof(tagStateEx));

	// 附加效果相关定义
	m_fXIncrement  = 0.0f;
	m_fYIncrement  = 0.0f;
	m_fShakeMax    = 0.0f;
	m_fShakeMin	   = 0.0f;
	m_bRestoration = false;
	ZeroMemory(m_listAddEffect, sizeof(tagAddEffect) * 2);

	for (int i = 0; i < 12; ++i)
		m_fActionCustomSpeedRate[i] = 1.0f;

	ZeroMemory(&m_stCurrentFaceAnim, sizeof(tagFaceAnim));
	ZeroMemory(&m_stCurrentActionAnim, sizeof(tagActionAnim));
	m_stCurrentActionAnim.iActionIndex = -1;
	m_iActionChannel = 0;
	m_bPlayActionAudio = false;
	m_strActionAudioFile = "";

	m_dwIntonateTime = 0;
	m_dwIntonateStartTime = 0;
	m_wIntonateSkillID = 0;
	m_lIntonateSkillKey = 0;
	m_bIntonateAlpha = 0;
	m_bIsLToR = true;
	m_busesimplemodel = false;
	m_iLastColorLevel = 0;
    m_PhaseID = NULL_GUID;
}



/*
* 功能: 析构函数
* 摘要: -
* 参数: -
* 返回值: -
* 作者: -
* 创建日期: -
* 修改日志:
*	2009.02.09 - lpf
*		增加了跳血列表的清除
*/
CMoveShape::~CMoveShape(void)
{
	SAFE_DELETE(m_pForceMove);
	DeleteCurSkillList();		// 删除技能列表

	m_listBloodText.clear();
}

// 初始化变量名到值得影射
void CMoveShape::InitNameValueMap()
{
	tagPty st;// 0: byte 1:WORD 2:DWORD	3:float 4:long 5:short 6:bool
	st.lType=3; st.pfValue=&m_fSpeed;				m_PropertyEnumToName[S_ATTR_SPEED]="Speed";	   					m_mapNameValue["Speed"] = st;
	st.lType=1; st.pwValue=&m_wState;				m_PropertyEnumToName[S_ATTR_SHAPESTATE]="ShapeState";			m_mapNameValue["ShapeState"] = st;	
	st.lType=6; st.pbValue=&m_bInvisible;			m_PropertyEnumToName[S_ATTR_INVISIBLETIME]="Invisible";			m_mapNameValue["Invisible"] = st;		//隐形
	st.lType=6; st.pbValue=&m_bHide;				m_PropertyEnumToName[S_ATTR_HIDE]="Hide";						m_mapNameValue["Hide"] = st;			//隐蔽
	st.lType=6; st.pbValue=&m_bPretendDead;			m_PropertyEnumToName[S_ATTR_PRETENDDEADTIME]="PretendDead";		m_mapNameValue["PretendDead"]=st;		//假死
	st.lType=4; st.pValue=&m_lChangeGraphicstype;	m_PropertyEnumToName[S_ATTR_CHANGEMODELTYPE]="ChangeModelType";	m_mapNameValue["ChangeModelType"]=st;	//改变类型
	st.lType=4; st.pValue=&m_lChangeGraphicsID;		m_PropertyEnumToName[S_ATTR_CHANGEMODELID]="ChangeModelID";		m_mapNameValue["ChangeModelID"]=st;	//改变模型id
	st.lType=6; st.pbValue=&m_bInstruction;			m_PropertyEnumToName[S_ATTR_USEINSTRUCTION]="UnUseInstruction";	m_mapNameValue["UnUseInstruction"]=st;	//使用指令
	
    st.lType=5; st.psValue=&m_sMoveable;			m_PropertyEnumToName[S_ATTR_MOVEABLE]="sMoveable";		   		m_mapNameValue["sMoveable"]=st;			//能否移动
    st.lType=5; st.psValue=&m_sSelfControl;			m_PropertyEnumToName[S_ATTR_SELFCONTROL]="sSelfControl";		m_mapNameValue["sSelfControl"]=st;	    //能否自我控制
    st.lType=5; st.psValue=&m_sUseSkill;			m_PropertyEnumToName[S_ATTR_USEALLSKILL]="sUseAllSKill";		m_mapNameValue["sUseAllSKill"]=st;		//是否可以使用技能
    st.lType=5; st.psValue=&m_sMoveDamage;			m_PropertyEnumToName[S_ATTR_MOVEDAMAGE]="sMoveDamage";		   	m_mapNameValue["sMoveDamage"]=st;		//是否移动攻击效果
    st.lType=5; st.psValue=&m_sBeenAttack;			m_PropertyEnumToName[S_ATTR_CANBEENATTACK]="sCanBeenAttack";	m_mapNameValue["sCanBeenAttack"]=st;	//能否被攻击
    st.lType=5; st.psValue=&m_sRandomMove;			m_PropertyEnumToName[S_ATTR_RAMDOMMOVE]="sRandomMove";			m_mapNameValue["sRandomMove"]=st;		//随机跑动

	st.lType=6; st.pbValue=&m_bUseFighterSkill;		m_PropertyEnumToName[S_ATTR_USEFIGHTSKILL]="UseFighterSkill";	m_mapNameValue["UseFighterSkill"]=st;	//使用战士技能
	st.lType=6; st.pbValue=&m_bUseRabbSkill;		m_PropertyEnumToName[S_ATTR_USERABBSKILL]="UseRabbSkill";		m_mapNameValue["UseRabbSkill"]=st;		//使用法师技能
	st.lType=6; st.pbValue=&m_bUseItem;				m_PropertyEnumToName[S_ATTR_USEITEM]="UseItem";					m_mapNameValue["UseItem"]=st;			//使用道具
	st.lType=6; st.pbValue=&m_bAntiInvisibleable;	m_PropertyEnumToName[S_ATTR_ANTIINVISIBLEABLE]="AntiInvisibleable";	m_mapNameValue["AntiInvisibleable"]=st;//反隐形
	st.lType=6; st.pbValue=&m_bAddHp;				m_PropertyEnumToName[S_ATTR_ADDHP]="AddHp";						m_mapNameValue["AddHp"]=st;		    //不能增加HP
	st.lType=6; st.pbValue=&m_bUnAttack;			m_PropertyEnumToName[S_ATTR_UNATTACK]="UnAttack";				m_mapNameValue["UnAttack"]=st;		    //不能被攻击
	st.lType=6; st.pbValue=&m_bForceAttact;			m_PropertyEnumToName[S_ATTR_FORCEATTACK]="ForceAttack";			m_mapNameValue["ForceAttack"]=st;		//强制攻击

	st.lType=2; st.pdwValue=&m_dwCollectID;			m_PropertyEnumToName[S_ATTR_COLLECTIONID]="CollectionID";		m_mapNameValue["CollectionID"]=st;		 //采集物id
	st.lType=2; st.pdwValue=&m_dwCollectTimes;		m_PropertyEnumToName[S_ATTR_COLLECTIONTIMES]="CollectionTimes";	m_mapNameValue["CollectionTimes"]=st;	 //能采集的次数
	st.lType=6; st.pbValue=&m_bIsBeenCollect;		m_PropertyEnumToName[S_ATTR_ISBEENCOLLECT]="IsBeenCollect";		m_mapNameValue["IsBeenCollect"]=st;	 //是否正在被采集
	st.lType=6; st.pbValue=&m_bCanBeenCollect;		m_PropertyEnumToName[S_ATTR_CANBEENCOLLECT]="CanBeenCollect";	m_mapNameValue["CanBeenCollect"]=st;	 //能否被采集

	st.lType=6; st.pbValue=&m_bNotRide;		m_PropertyEnumToName[S_ATTR_NOTRIDE]="Ride";	m_mapNameValue["Ride"]=st;	 //是否坐骑
}

void CMoveShape::SetState(WORD l)
{
	if(l < STATE_PEACE || l>STATE_OPEN)
	{
		Log4c::Warn(ROOT_MODULE,"对对象(type:%d,name:%s)设置了错误的状态(%d)",GetType(),GetName(),l);
		return;
	}
	CShape::SetState(l);
}

bool g_bForceUpdate = false;



/*
* 功能: 设定动作
* 摘要: -
* 参数: l - 动作类型
* 返回值: -
* 作者: -
* 创建日期: -
* 修改日志:
*	2008.02.15 - lpf
*		增加了"处理当前类型动画时间长度"
*	2008.02.18 - lpf
*		将"处理当前类型动画时间长度"修改为"处理当前类型动画速率"
*	2008.03.27 - lpf
*		增加了对上一个动作速率进行保存的处理
*	2008.04.22 - lpf
*		在玩家和怪物的动作设定中增加了关于仪态动作的设置
*	2008.12.26 - lpf
*		增加了玩家仪态动作之间的插值功能
*	2009.04.08 - lpf
*		修正了在设定采矿动作时,"当前动作播放的次数上限"设定为-1,即无限循环
*/
void CMoveShape::SetAction(WORD l)
{
	if(l < ACT_STAND || (l>ACT_SEL_X5 && l<ACT_PERFORM) || l>ACT_MAX_PERFORM)
	{
		Log4c::Warn(ROOT_MODULE,"对对象(type:%d,name:%s)设置了错误的动作(%d)",GetType(),GetName(),l);
		return;
	}

	// 动作没有变化，直接返回
	if (m_wAction==l && g_bForceUpdate==false)
		return;

 	if (m_lType == TYPE_PET && l == ACT_RUN)
	{
		CPet *pPet = (CPet*)this;
		pPet->ConfusePetStandInfo();
	}
	// 本人使用的技能
	if(this == GetGame()->GetMainPlayer() && m_wAction < ACT_PERFORM)
	{
		SetCurSkillActTime(0);
		SetCurSkillStartTime(0);
	}
	g_bForceUpdate = false;

	//得到类型
	DWORD dwOldAction = m_wAction;
	DWORD dwCurrentTime = GetCurTickCount();
	//---------------------------------------------
	m_wAction = l;

	// 进入非站立动作，清除Idle计数
	if (m_wAction!=ACT_STAND)
	{
		m_lIdleCount = 0;
		m_bIdleStart = false;
	}

	AnimInfo::tagActionInfo *pActionInfo = m_pAnimInfo->GetActionInfo();
	pActionInfo->bStayToLastFrame = false;
	pActionInfo->dwCurActionLoopCount = 0;

	if (m_wAction == CShape::ACT_STAND || m_wAction == CShape::ACT_INTONATE
		|| m_wAction == CShape::ACT_WALK || m_wAction == CShape::ACT_RUN 
		|| m_wAction == CShape::ACT_MEGRIM || m_wAction == CShape::ACT_ALREADYDIED ||
		GetType() == TYPE_EFFECT )
	{
		pActionInfo->bCurActionLooped = TRUE;
	}
	else
	{
		pActionInfo->bCurActionLooped = FALSE;
	}

	//设置动作的内插
	if ( GetType() == TYPE_EFFECT || GetType() == TYPE_GOODS || GetType() == TYPE_BUILD || GetType() == TYPE_SUMMON || GetType() == TYPE_CITYGATE )
	{
	}
	else
	{
		//从任何姿势回到站立
		if (m_wAction == CShape::ACT_STAND)
		{
			pActionInfo->bInterpolation = TRUE;
			pActionInfo->dwOldAction = pActionInfo->dwCurAction;
			pActionInfo->dwOldActionTimeElapse = pActionInfo->dwCurActionElapseTime;
			pActionInfo->fOldActionSpeedRate = pActionInfo->fCurActionSpeedRate;
			pActionInfo->dwInterpolationElapseTime = 0;
			pActionInfo->dwInterpolationStartTime  = dwCurrentTime;
			pActionInfo->dwInterpolationTimeLength = 240;
			if (m_pDisplayModel)
			{
				m_pDisplayModel->SetAllTrailEmitter(FALSE);
			}

		}
	}

	pActionInfo->dwCurActionStartTime = dwCurrentTime;

	// 处理当前类型动画速率
	if (l > -1 && l < 12)
		m_pAnimInfo->SetCurActionSpeedRate(m_fActionCustomSpeedRate[l]);

	//---------------------------------------------
	static char str[32];
	long lType = GetType();
	long lGraphicsID = GetGraphicsID();
	float fDir = GetNowDir();
	long lState = 0;
	long lAct = m_wAction;
	//是否有武器
	DWORD dwWeaponIndex=0;
	//武器动作类型
	DWORD dwWeaponActType = 0;
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(this);
	switch(lType)
	{
		// 玩家
	case TYPE_PLAYER:
		{
			//状态							                      
			static char *strState[] =  
			{
				"B",	//和平
				"E",	//空手
				"S",	//单手剑
				"H",	//单手锤
				"K",	//刀
				"X",	//槍
				"A",	//双手斧
				"W",	//双手杖
				"B",	//棍
				"M",	//拳套
				"T",	//環
                "G"     //琴
			};
			//坐骑动作类型							                                      
			static char *strActionHorse[] = 
			{
				"S",	//站立  
				"I",	//站立idle    
				"W",	//行走 
				"R",	//跑步
				"H",	//被攻击  
				"903",	//吟唱 
				"B",	//格档
				"Z",	//招架 
				"K",	//闪避 
				"Y",	//眩晕 
				"D00",	//死亡
				"SIQU", //拾取
				"D01",  //已经死亡
				"D01",  //已经死亡
				"OPEN", //开门
				"AROP", //已经开门
				"DOWN"  //摔倒
			};
			//动作类型							                                      
			static char *strAction[] = 
			{
				"B00",	//站立  
				"I00",	//站立idle    
				"W00",	//行走 
				"R00",	//跑步
				"H00",	//被攻击  
				"903",	//吟唱 
				"LK0",	//格档
				"P00",	//招架 
				"T00",	//闪避 
				"Y00",	//眩晕 
				"D00",	//死亡
				"PIK", //拾取
				"D01",  //已经死亡
				"OPEN", //开门
				"AROP", //已经开门
				"DOWN"  //摔倒
			};
			//攻击动作
			static char *strAttackAction[] = 
			{
				"K00",	//动作1
				"K01",	//动作2
				"K02"	//动作3
			};
			static char* strsingleperson[]=
			{
				"0",
				"1"
			};
			static char* strhorsetype[]=
			{
				"1",
				"2",
				"3"
			};
			// 得到主角自己武器类型
			if (pPlayer==GetGame()->GetMainPlayer())
			{
				CGoods* pEquip = pPlayer->GetEquip(CPlayer::EQUIP_WEAPON);
				//是否有武器
				dwWeaponIndex = pEquip?pEquip->GetProperty()->dwIndex:0;
				//武器啥子动作类型
				dwWeaponActType = pEquip?pEquip->GetProperty()->dwWeaponActType:0;
			}
			// 其他玩家武器类型
			else	
			{
				dwWeaponIndex = pPlayer->GetEquipIndex(CPlayer::EQUIP_WEAPON);
				CGoodsList::tagGoods2 *pG = CGoodsList::GetProperty(dwWeaponIndex);
				if (pG)
					dwWeaponActType = pG->BaseProperty.dwWeaponActType;
				else
					dwWeaponActType = 0;
			}

			//施法动作
			if (lAct >= CShape::ACT_PERFORM && lAct < ACT_MAX)
			{
				//如果没有武器(空手)
				if (dwWeaponIndex == 0)
				{
					lState = 1;
				}
				else
				{
					lState = dwWeaponActType + 2;
				}

				if(lAct >= ACT_MINE && lAct <= ACT_MINEMAX)
				{
					sprintf_s(str,"C%03d",lAct - CShape::ACT_PERFORM);
					m_iActionCount = -1;
				}else if (lAct >= ACT_FACEACTION && lAct <= ACT_FACEACTIONMAX)
				{
					sprintf_s(str, "V%03d", lAct);
					pActionInfo->bInterpolation = TRUE;
					pActionInfo->dwOldAction = pActionInfo->dwCurAction;
					pActionInfo->dwOldActionTimeElapse = pActionInfo->dwCurActionElapseTime;
					pActionInfo->fOldActionSpeedRate = pActionInfo->fCurActionSpeedRate;
					pActionInfo->dwInterpolationElapseTime = 0;
					pActionInfo->dwInterpolationStartTime  = dwCurrentTime;
					pActionInfo->dwInterpolationTimeLength = 250;
				}else
				{
					sprintf_s(str,"%s%03s",strState[lState],strAttackAction[GetAttackAction()]);
				}
			}
			else
			{

				//和平状态
				if (GetState() == CShape::STATE_PEACE)
				{
					lState = 0;
				}
				else
				{
					//如果没有武器(空手)
					if (dwWeaponIndex == 0)
					{
						lState = 1;
					}
					else
					{
						lState = dwWeaponActType + 2;
					}
				}
				if (lAct == ACT_PICK||lAct == ACT_FALLDOWN||lAct == ACT_MEGRIM)
				{

					if(GetGame()->GetRegion()!=NULL&& GetGame()->GetRegion()->GetSpaceType() == RST_Warter)
					{
						if (lAct == ACT_MEGRIM)
						{
							sprintf_s(str,"%04s","YY00");
						}
						else
						{
							sprintf_s(str,"%04s",strAction[lAct]);
						}
					}
					else if (pPlayer->IsRide())
					{
						sprintf_s(str,"R%s%s%s",strActionHorse[lAct],strsingleperson[0],strhorsetype[0]);
					}
					else
					{
						sprintf_s(str,"%04s",strAction[lAct]);
					}

				}
				else
				{
					//游泳动作此处加
					if(GetGame()->GetRegion()!=NULL&& (GetGame()->GetRegion()->GetSpaceType() == RST_Warter) )
					{

						sprintf_s(str,"Y%03s",strAction[lAct]);
						pActionInfo->bInterpolation = TRUE;
						pActionInfo->dwOldAction = pActionInfo->dwCurAction;
						pActionInfo->dwOldActionTimeElapse = pActionInfo->dwCurActionElapseTime;
						pActionInfo->fOldActionSpeedRate = pActionInfo->fCurActionSpeedRate;
						pActionInfo->dwInterpolationElapseTime = 0;
						pActionInfo->dwInterpolationStartTime  = dwCurrentTime;
						pActionInfo->dwInterpolationTimeLength = 240;


					}
					//隐形动作
					else if(!GetIsConceal())
					{

						if (lAct == ACT_PICK||lAct == ACT_FALLDOWN||lAct == ACT_MEGRIM)
						{
							sprintf_s(str,"%04s",strAction[lAct]);
						}
						else
						{
							sprintf_s(str,"N%03s",strAction[lAct]);
						}
						pActionInfo->bInterpolation = TRUE;
						pActionInfo->dwOldAction = pActionInfo->dwCurAction;
						pActionInfo->dwOldActionTimeElapse = pActionInfo->dwCurActionElapseTime;
						pActionInfo->fOldActionSpeedRate = pActionInfo->fCurActionSpeedRate;
						pActionInfo->dwInterpolationElapseTime = 0;
						pActionInfo->dwInterpolationStartTime  = dwCurrentTime;
						pActionInfo->dwInterpolationTimeLength = 240;
					}
					else
					{
						if (lAct == ACT_IDLE)
						{
							pActionInfo->bInterpolation = TRUE;
							pActionInfo->dwOldAction = pActionInfo->dwCurAction;
							pActionInfo->dwOldActionTimeElapse = pActionInfo->dwCurActionElapseTime;
							pActionInfo->fOldActionSpeedRate = pActionInfo->fCurActionSpeedRate;
							pActionInfo->dwInterpolationElapseTime = 0;
							pActionInfo->dwInterpolationStartTime  = dwCurrentTime;
							pActionInfo->dwInterpolationTimeLength = 240;
						}
						if (pPlayer->IsRide())
						{
							sprintf_s(str,"R%s%s%s",strActionHorse[lAct],strsingleperson[0],strhorsetype[0]);
						}
						else
						{
							sprintf_s(str,"%s%03s",strState[lState],strAction[lAct]);

						}

					}

				}
			}
			if (m_pDisplayModel&&m_pDisplayModel->GetLoadState() == Load_Did)
			{
				std::map<DWORD,GameModel::Action*>* mapaction = CGameModelManager::GetInstance()->GetGameModel(m_pDisplayModel->GetGameModelID())->GetActionArray();
				if (mapaction->find(MAKEFOURCC(str[0],str[1],str[2],str[3])) != mapaction->end())
				{
					pActionInfo->dwCurAction = MAKEFOURCC(str[0],str[1],str[2],str[3]);
				}
				else
				{
					pActionInfo->dwCurAction = MAKEFOURCC('S','S','0','0');
				}

			}
			else
			{
				pActionInfo->dwCurAction = MAKEFOURCC(str[0],str[1],str[2],str[3]);
			}

		}
		break;
		//宠物
	case	TYPE_PET:
		{	
			static DWORD dwSTD0 = MAKEFOURCC('S','T','D','0');		// 和平站立
			static DWORD dwIDLE = MAKEFOURCC('I','D','L','E');		// 和平空闲
			static DWORD dwSTD1 = MAKEFOURCC('S','T','D','1');		// 战斗站立
			static DWORD dwWLK0 = MAKEFOURCC('W','L','K','0');		// 和平行走
			static DWORD dwRUN  = MAKEFOURCC('W','L','K','1');		// 跑步
			static DWORD dwWLK1 = MAKEFOURCC('W','L','K','1');		// 战斗行走
			static DWORD dwDIED = MAKEFOURCC('D','I','E','D');		// 死亡
			static DWORD dwDIE1 = MAKEFOURCC('D','I','E','1');		// 死亡
			static DWORD dwATK0 = MAKEFOURCC('A','T','K','0');		// 攻击1
			static DWORD dwATK1 = MAKEFOURCC('A','T','K','1');		// 攻击2
			static DWORD dwHURT = MAKEFOURCC('H','U','R','T');		// 受伤
			static DWORD dwMEGR = MAKEFOURCC('M','E','G','R');		// 眩晕
			static DWORD dwDOWN = MAKEFOURCC('D','O','W','N');		// 跌倒
			static DWORD dwINTO = MAKEFOURCC('I','N','T','O');		// 吟唱

			lState = GetState();
			switch (lAct)
			{
			case CShape::ACT_STAND:
				{
					if(lState == CShape::STATE_PEACE)
					{
						pActionInfo->bInterpolation = TRUE;
						pActionInfo->dwOldAction = pActionInfo->dwCurAction;
						pActionInfo->dwOldActionTimeElapse = pActionInfo->dwCurActionElapseTime;
						pActionInfo->fOldActionSpeedRate = pActionInfo->fCurActionSpeedRate;
						pActionInfo->dwInterpolationElapseTime = 0;
						pActionInfo->dwInterpolationStartTime  = dwCurrentTime;
						pActionInfo->dwInterpolationTimeLength = 240;
						pActionInfo->dwCurAction = dwSTD0;
					}
					else if(lState == CShape::STATE_FIGHT)
					{
						pActionInfo->bInterpolation = FALSE;
						pActionInfo->dwOldAction = pActionInfo->dwCurAction;
						pActionInfo->dwOldActionTimeElapse = pActionInfo->dwCurActionElapseTime;
						pActionInfo->fOldActionSpeedRate = pActionInfo->fCurActionSpeedRate;
						pActionInfo->dwInterpolationElapseTime = 0;
						pActionInfo->dwInterpolationStartTime  = dwCurrentTime;
						pActionInfo->dwInterpolationTimeLength = 50;
						pActionInfo->dwCurAction = dwSTD1;
					}
					else
						Log4c::Warn(ROOT_MODULE,"怪物在错误的状态下执行站立动作。");
				}
				break;
			case CShape::ACT_IDLE:
				{
					if(lState == CShape::STATE_PEACE)
					{
						pActionInfo->bInterpolation = TRUE;
						pActionInfo->dwOldAction = pActionInfo->dwCurAction;
						pActionInfo->dwOldActionTimeElapse = pActionInfo->dwCurActionElapseTime;
						pActionInfo->fOldActionSpeedRate = pActionInfo->fCurActionSpeedRate;
						pActionInfo->dwInterpolationElapseTime = 0;
						pActionInfo->dwInterpolationStartTime  = dwCurrentTime;
						pActionInfo->dwInterpolationTimeLength = 240;
						pActionInfo->dwCurAction = dwIDLE;
					}
					else
						Log4c::Warn(ROOT_MODULE,"怪物在错误的状态下执行IDLE动作。");
				}
				break;
			case CShape::ACT_WALK:
				{
					pActionInfo->bInterpolation = FALSE;
					pActionInfo->dwOldAction = pActionInfo->dwCurAction;
					pActionInfo->dwOldActionTimeElapse = pActionInfo->dwCurActionElapseTime;
					pActionInfo->fOldActionSpeedRate = pActionInfo->fCurActionSpeedRate;
					pActionInfo->dwInterpolationElapseTime = 0;
					pActionInfo->dwInterpolationStartTime  = dwCurrentTime;
					pActionInfo->dwInterpolationTimeLength = 240;
					if(lState == CShape::STATE_PEACE)
						pActionInfo->dwCurAction = dwWLK0;
					else if(lState == CShape::STATE_FIGHT)
						pActionInfo->dwCurAction = dwWLK1;
					else
						Log4c::Warn(ROOT_MODULE,"怪物在错误的状态下执行行走动作。");
				}
				break;
			case CShape::ACT_RUN:
				{
					pActionInfo->bInterpolation = FALSE;
					pActionInfo->dwOldAction = pActionInfo->dwCurAction;
					pActionInfo->dwOldActionTimeElapse = pActionInfo->dwCurActionElapseTime;
					pActionInfo->fOldActionSpeedRate = pActionInfo->fCurActionSpeedRate;
					pActionInfo->dwInterpolationElapseTime = 0;
					pActionInfo->dwInterpolationStartTime  = dwCurrentTime;
					pActionInfo->dwInterpolationTimeLength = 240;
					pActionInfo->dwCurAction = dwRUN;
					break;
				}
			case CShape::ACT_DIED:
				pActionInfo->bInterpolation = TRUE;
				pActionInfo->dwOldAction = pActionInfo->dwCurAction;
				pActionInfo->dwOldActionTimeElapse = pActionInfo->dwCurActionElapseTime;
				pActionInfo->fOldActionSpeedRate = pActionInfo->fCurActionSpeedRate;
				pActionInfo->dwInterpolationElapseTime = 0;
				pActionInfo->dwInterpolationStartTime  = dwCurrentTime;
				pActionInfo->dwInterpolationTimeLength = 240;
				pActionInfo->dwCurAction = dwDIED;
				break;
			case CShape::ACT_ALREADYDIED:
				pActionInfo->dwCurAction = dwDIE1;
				break;
			case CShape::ACT_ATTACK:
				if (GetAttackAction()==0)
					pActionInfo->dwCurAction = dwATK0;
				else
					pActionInfo->dwCurAction = dwATK1;
				break;
			case CShape::ACT_BEATTACK:
				pActionInfo->dwCurAction = dwHURT;
				break;
			case CShape::ACT_MEGRIM:
				pActionInfo->dwCurAction = dwMEGR;	
				break;
			case CShape::ACT_FALLDOWN:
				pActionInfo->dwCurAction = dwDOWN;	
				break;
			default:	
				{	// 技能动作
					if(lAct >= ACT_PERFORM && lAct <= ACT_MAX_PERFORM)
					{
						// 怪物的仪态动作
						if (lAct >= ACT_FACEACTION && lAct <= ACT_FACEACTIONMAX)
						{
							sprintf_s(str, "V%03d", lAct);
							pActionInfo->bInterpolation = TRUE;
							pActionInfo->dwOldAction = pActionInfo->dwCurAction;
							pActionInfo->dwOldActionTimeElapse = pActionInfo->dwCurActionElapseTime;
							pActionInfo->fOldActionSpeedRate = pActionInfo->fCurActionSpeedRate;
							pActionInfo->dwInterpolationElapseTime = 0;
							pActionInfo->dwInterpolationStartTime  = dwCurrentTime;
							pActionInfo->dwInterpolationTimeLength = 250;
							pActionInfo->dwCurAction = MAKEFOURCC(str[0],str[1],str[2],str[3]);	
						}
						else
						{
							sprintf(str, "S%03d", lAct - CShape::ACT_PERFORM);
							pActionInfo->dwCurAction = MAKEFOURCC(str[0],str[1],str[2],str[3]);	
						}

					}
					else
					{
						Log4c::Warn(ROOT_MODULE,"错误的动作类型:%d",lAct);
					}
				}
				break;
			}
			break;
		}
		// 怪物、NPC
	case TYPE_MONSTER:
		//case TYPE_NPC:
		{
			static DWORD dwSTD0 = MAKEFOURCC('S','T','D','0');		// 和平站立
			static DWORD dwIDLE = MAKEFOURCC('I','D','L','E');		// 和平空闲
			static DWORD dwSTD1 = MAKEFOURCC('S','T','D','1');		// 战斗站立
			static DWORD dwWLK0 = MAKEFOURCC('W','L','K','0');		// 和平行走
			static DWORD dwWLK1 = MAKEFOURCC('W','L','K','1');		// 战斗行走
			static DWORD dwDIED = MAKEFOURCC('D','I','E','D');		// 死亡
			static DWORD dwDIE1 = MAKEFOURCC('D','I','E','1');		// 死亡
			static DWORD dwATK0 = MAKEFOURCC('A','T','K','0');		// 攻击1
			static DWORD dwATK1 = MAKEFOURCC('A','T','K','1');		// 攻击2
			static DWORD dwHURT = MAKEFOURCC('H','U','R','T');		// 受伤
			static DWORD dwMEGR = MAKEFOURCC('M','E','G','R');		// 眩晕
			static DWORD dwDOWN = MAKEFOURCC('D','O','W','N');		// 跌倒
			//static DWORD dwINTO = MAKEFOURCC('I','N','T','O');		// 吟唱

			lState = GetState();
			switch (lAct)
			{
			case CShape::ACT_STAND:
				{
					if(lState == CShape::STATE_PEACE)
					{
						pActionInfo->bInterpolation = TRUE;
						pActionInfo->dwOldAction = pActionInfo->dwCurAction;
						pActionInfo->dwOldActionTimeElapse = pActionInfo->dwCurActionElapseTime;
						pActionInfo->fOldActionSpeedRate = pActionInfo->fCurActionSpeedRate;
						pActionInfo->dwInterpolationElapseTime = 0;
						pActionInfo->dwInterpolationStartTime  = dwCurrentTime;
						pActionInfo->dwInterpolationTimeLength = 240;
						pActionInfo->dwCurAction = dwSTD0;
					}
					else if(lState == CShape::STATE_FIGHT)
					{
						pActionInfo->bInterpolation = FALSE;
						pActionInfo->dwOldAction = pActionInfo->dwCurAction;
						pActionInfo->dwOldActionTimeElapse = pActionInfo->dwCurActionElapseTime;
						pActionInfo->fOldActionSpeedRate = pActionInfo->fCurActionSpeedRate;
						pActionInfo->dwInterpolationElapseTime = 0;
						pActionInfo->dwInterpolationStartTime  = dwCurrentTime;
						pActionInfo->dwInterpolationTimeLength = 50;
						pActionInfo->dwCurAction = dwSTD1;
					}
					else
						Log4c::Warn(ROOT_MODULE,"怪物在错误的状态下执行站立动作。");
				}
				break;
			case CShape::ACT_IDLE:
				{
					if(lState == CShape::STATE_PEACE)
					{
						pActionInfo->bInterpolation = TRUE;
						pActionInfo->dwOldAction = pActionInfo->dwCurAction;
						pActionInfo->dwOldActionTimeElapse = pActionInfo->dwCurActionElapseTime;
						pActionInfo->fOldActionSpeedRate = pActionInfo->fCurActionSpeedRate;
						pActionInfo->dwInterpolationElapseTime = 0;
						pActionInfo->dwInterpolationStartTime  = dwCurrentTime;
						pActionInfo->dwInterpolationTimeLength = 240;
						pActionInfo->dwCurAction = dwIDLE;
					}
					else
						Log4c::Warn(ROOT_MODULE,"怪物在错误的状态下执行IDLE动作。");
				}
				break;
			case CShape::ACT_WALK:
				{
					pActionInfo->bInterpolation = FALSE;
					pActionInfo->dwOldAction = pActionInfo->dwCurAction;
					pActionInfo->dwOldActionTimeElapse = pActionInfo->dwCurActionElapseTime;
					pActionInfo->fOldActionSpeedRate = pActionInfo->fCurActionSpeedRate;
					pActionInfo->dwInterpolationElapseTime = 0;
					pActionInfo->dwInterpolationStartTime  = dwCurrentTime;
					pActionInfo->dwInterpolationTimeLength = 240;
					if(lState == CShape::STATE_PEACE)
						pActionInfo->dwCurAction = dwWLK0;
					else if(lState == CShape::STATE_FIGHT)
						pActionInfo->dwCurAction = dwWLK1;
					else
						Log4c::Warn(ROOT_MODULE,"怪物在错误的状态下执行行走动作。");
				}
				break;
			case CShape::ACT_DIED:
				pActionInfo->bInterpolation = TRUE;
				pActionInfo->dwOldAction = pActionInfo->dwCurAction;
				pActionInfo->dwOldActionTimeElapse = pActionInfo->dwCurActionElapseTime;
				pActionInfo->fOldActionSpeedRate = pActionInfo->fCurActionSpeedRate;
				pActionInfo->dwInterpolationElapseTime = 0;
				pActionInfo->dwInterpolationStartTime  = dwCurrentTime;
				pActionInfo->dwInterpolationTimeLength = 240;
				pActionInfo->dwCurAction = dwDIED;
				break;
			case CShape::ACT_ALREADYDIED:
				pActionInfo->dwCurAction = dwDIE1;
				break;
			case CShape::ACT_ATTACK:
				if (GetAttackAction()==0)
					pActionInfo->dwCurAction = dwATK0;
				else
					pActionInfo->dwCurAction = dwATK1;
				break;
			case CShape::ACT_BEATTACK:
				pActionInfo->dwCurAction = dwHURT;
				break;
			case CShape::ACT_MEGRIM:
				pActionInfo->dwCurAction = dwMEGR;	
				break;
			case CShape::ACT_FALLDOWN:
				pActionInfo->dwCurAction = dwDOWN;	
				break;
			default:	
				{	// 技能动作
					if(lAct >= ACT_PERFORM && lAct <= ACT_MAX_PERFORM)
					{
						// 怪物的仪态动作
						if (lAct >= ACT_FACEACTION && lAct <= ACT_FACEACTIONMAX)
						{
							sprintf_s(str, "V%03d", lAct);
							pActionInfo->bInterpolation = TRUE;
							pActionInfo->dwOldAction = pActionInfo->dwCurAction;
							pActionInfo->dwOldActionTimeElapse = pActionInfo->dwCurActionElapseTime;
							pActionInfo->fOldActionSpeedRate = pActionInfo->fCurActionSpeedRate;
							pActionInfo->dwInterpolationElapseTime = 0;
							pActionInfo->dwInterpolationStartTime  = dwCurrentTime;
							pActionInfo->dwInterpolationTimeLength = 250;
							pActionInfo->dwCurAction = MAKEFOURCC(str[0],str[1],str[2],str[3]);	
						}
						else
						{
							sprintf(str, "S%03d", lAct - CShape::ACT_PERFORM);
							pActionInfo->dwCurAction = MAKEFOURCC(str[0],str[1],str[2],str[3]);	
						}

					}
					else
					{
						Log4c::Warn(ROOT_MODULE,"错误的动作类型:%d",lAct);
					}
				}
				break;
			}

			// 调用缺省动作
		}
		break;
	case TYPE_BUILD:
	case TYPE_CITYGATE:
		{
			static DWORD dwSTND = MAKEFOURCC('S','T','N','D');
			static DWORD dwDIED = MAKEFOURCC('D','I','E','D');
			static DWORD dwOPEN = MAKEFOURCC('O','P','E','N');
			static DWORD dwHURT = MAKEFOURCC('H','U','R','T');

			switch (lAct)
			{
			case CShape::ACT_STAND:
				pActionInfo->dwCurAction = dwSTND;
				break;

			case CShape::ACT_DIED:
				pActionInfo->dwCurAction = dwDIED;
				break;

			case CShape::ACT_BEATTACK:
				pActionInfo->dwCurAction = dwHURT;
				break;

			case CShape::ACT_OPEN:
				pActionInfo->dwCurAction = dwOPEN;
				break;
			}

			// 调用缺省动作

		}
		break;
		// 物品
	case TYPE_EFFECT:
	case TYPE_SKILL:
		{
			pActionInfo->dwCurAction = MAKEFOURCC('S', 'T', 'N', 'D');
		}
		break;
		// 王峰：采集物
	case TYPE_COLLECTION:
		{
			pActionInfo->dwCurAction = CShape::ACT_ALREADYDIED == lAct ? MAKEFOURCC('D','I','E','1') : MAKEFOURCC('D','I','E','D');
		}
		break;
	}
}



/*
* 功能: MoveShape的人工智能
* 摘要: -
* 参数: -
* 返回值: -
* 作者: -
* 创建日期: -
* 修改日志:
*	2008.12.01 - lpf
* 		调整了"跳血数字的颜色和高度控制"代码段中的关于跳血移动的参数,使之看上去效果的更和谐;
*	2008.12.03 - lpf
* 		调整了"跳血数字的颜色和高度控制",目前字符放大到最大以后,不再进行缩小显示,而直接渐出,并处理的新增加的缩放参数;
*	2008.12.12 - lpf
* 		调整了"跳血数字的速度"增加了0.01;
*/
void CMoveShape::AI()
{
	CShape::AI();

	// 说话的文字颜色控制
	if (m_strTalkText.size())
	{
		m_lTalkLiveTimer -= 1;
		if (m_lTalkLiveTimer <= 0)
		{
			m_strTalkText = "";
		}
	}

	// 跳血数字的颜色和高度控制
	DWORD dwCurTime = timeGetTime();	
	static const DWORD dwBloodTextTimeLength = 5000;
	for(list<tagBloodText>::iterator it=m_listBloodText.begin(); it!=m_listBloodText.end();)
	{
		tagBloodText *pst = &(*it);
		if (pst)
		{
			DWORD dwTimeElapse = dwCurTime - pst->dwStartTime;
			DWORD dwSliceIndex = 3;
			if ( dwTimeElapse < 50)
			{
				dwSliceIndex = 0;
			}else
			{
				if (dwTimeElapse < 120)
				{
					dwSliceIndex = 1;
				}else
				{
					if (dwTimeElapse < 300)
					{
						dwSliceIndex = 2;
					}
				}
			}

			switch(dwSliceIndex)
			{
			case 0: 
				{
					float fRatio = (float)dwTimeElapse / 50.0f;
					pst->lAlpha = 128 + (int)(128.0f * fRatio);
					pst->fCurScale = pst->fScale * (0.5f + 0.5f * fRatio);
				}
				break;
			case 1: 
				pst->fCurScale = pst->fScale;
				pst->fCurScale2 = pst->fScale;
				pst->lAlpha = 255;
				break;
			case 2: 
				{
					float fRatio = (float)(dwTimeElapse - 120) / 120.0f;
					//pst->fCurScale = (pst->fScale + 1.0f) * (1.0f - (fRatio) * 0.5f);
					//pst->lAlpha = 192 - (int)(64.0f * fRatio);

					pst->lAlpha -= 5;
					if (pst->lAlpha <0 )
						pst->lAlpha = 0;

					pst->fPos		+= 0.05f;
					pst->fCurScale2 += 0.008f;
				}
				break;
			default: // 400 - ?
				/*pst->fCurScale = pst->fScale * 0.5f*/;

				pst->lAlpha -= 5;
				if (pst->lAlpha <0 )
					pst->lAlpha = 0;

				pst->fPos += 0.06f;
				pst->fCurScale2 += 0.008f;
				break;
			}

			// 文字删除
			if (dwTimeElapse > dwBloodTextTimeLength)
			{
				m_listBloodText.erase(it++);
			}
			else
			{
				++it;
			}
		}
		else
			m_listBloodText.erase(it++);
	}

	//// 硬直
	//if(m_dwStiffStartTime)
	//{
	//	if(GetCurTickCount()-m_dwStiffStartTime<500)
	//	{
	//		return;
	//	}
	//	m_dwStiffStartTime=0;
	//}

	if (GetType()!=TYPE_MONSTER && 
		GetType()!=TYPE_PLAYER &&
		/*GetType()!=TYPE_NPC &&*/ GetType() != TYPE_PET &&
		GetType()!=TYPE_SUMMON)
	{
		return;
	}

	AI_StateEx();

	switch( GetAction() )
	{
	case ACT_STAND:
	case ACT_IDLE:
		AI_Stand();
		break;

	case ACT_WALK:
		AI_Walk();
		break;

	case ACT_RUN:
		AI_Run();
		break;

	case ACT_ATTACK:
		AI_Attack();
		break;

	case ACT_BEATTACK:
		AI_BeAttack();
		break;

	case ACT_DIED:
		AI_Died();
		break;

	case ACT_BLOCK:				// 格档
	case ACT_PARRY:				// 招架
	case ACT_MISS:				// 闪避
	case ACT_MEGRIM:			// 眩晕
		AI_Perform();
		break;

	default:
		if (GetAction()>=ACT_PERFORM)
			AI_Perform();
		else
			OnAttackEvent();
		break;
	}
	AI_SlipMove();
	AI_Skill();
	AIEffectByOrder();
}

void CMoveShape::DecordChangeProperty(BYTE* pByte,long& lPos)
{
	WORD lNum = _GetWordFromByteArray(pByte,lPos);
	WORD pro_type = 0;
	BYTE data_type = 0;
	WORD data_length = 0;
	long data = 0;
	BYTE* byte_Array = NULL;
	WORD* word_Array = NULL;
	DWORD* dword_Array = NULL;
	float* flaot_Array = NULL;
	long*  long_Array = NULL;
	short* short_Array = NULL;
	char*  char_Array = NULL;
	bool*  bool_Array = NULL;

	for (int i = 0;i<lNum;i++)
	{
		pro_type = _GetWordFromByteArray(pByte,lPos);
		data_type = _GetByteFromByteArray(pByte, lPos);
		data_length = _GetWordFromByteArray(pByte,lPos);		
		if (data_length<=0) continue;

		//根据数据类型获取具体的数据值
		switch(data_type)
		{
		case ATTR_DATA_BYTE:
			data = _GetByteFromByteArray(pByte, lPos);
			break;
		case ATTR_DATA_WORD:
			data = _GetWordFromByteArray(pByte,lPos);
			break;
		case ATTR_DATA_DWORD:
			data = _GetDwordFromByteArray(pByte,lPos);
			break;
		case ATTR_DATA_FLOAT:
			data = (long)_GetFloatFromByteArray(pByte,lPos);
			break;
		case ATTR_DATA_LONG:
			data = _GetLongFromByteArray(pByte,lPos);
			break;
		case ATTR_DATA_SHORT:
			data = _GetShortFromByteArray(pByte,lPos);
			break;
		case ATTR_DATA_BOOL:
			data = _GetByteFromByteArray(pByte, lPos);
			break;
		case ATTR_DATA_BYTEARRAY:
			byte_Array = new BYTE[data_length];
			if (!byte_Array) return;
			for (int i = 0;i<data_length;i++)
			{
				byte_Array[i] = _GetByteFromByteArray(pByte, lPos);
			}
			break;
		case ATTR_DATA_WORDARRAY:
			word_Array = new WORD[data_length];
			if (!word_Array) return;
			for (int i = 0;i<data_length;i++)
			{
				word_Array[i] = _GetWordFromByteArray(pByte, lPos);
			}
			break;
		case ATTR_DATA_DWORDARRAY:
			dword_Array = new DWORD[data_length];
			if (!dword_Array) return;
			for (int i = 0;i<data_length;i++)
			{
				dword_Array[i] = _GetDwordFromByteArray(pByte,lPos);
			}
			break;
		case ATTR_DATA_FLOATARRAY:
			flaot_Array = new float[data_length];
			if (!flaot_Array) return;
			for (int i = 0;i<data_length;i++)
			{
				flaot_Array[i] = _GetFloatFromByteArray(pByte,lPos);
			}
			break;
		case ATTR_DATA_LONGARRAY:
			long_Array = new long[data_length];
			if (!long_Array) return;
			for (int i = 0;i<data_length;i++)
			{
				long_Array[i] = _GetLongFromByteArray(pByte,lPos);
			}
			break;
		case ATTR_DATA_SHORTARRAY:
			short_Array = new short[data_length];
			if (!short_Array) return;
			for (int i = 0;i<data_length;i++)
			{
				short_Array[i] = _GetShortFromByteArray(pByte,lPos);
			}
			break;
		case ATTR_DATA_CHARARRAY:
			char_Array = new char[data_length];
			if (!char_Array) return;
			_GetStringFromByteArray(pByte, lPos, char_Array);
			break;
		case ATTR_DATA_BOOLARRAY:
			bool_Array = new bool[data_length];
			if (!bool_Array) return;
			for (int i = 0;i<data_length;i++)
			{
				data = _GetByteFromByteArray(pByte, lPos);
				if (data!=0)
				{
					bool_Array[i] = true;
				}else
					bool_Array[i] = false;
			}
			break;
		default:
			break;
		}						

		//设置相应属性值
		itProETN it = m_PropertyEnumToName.find(pro_type);
		if(it != m_PropertyEnumToName.end())
		{
			SetProperty( (*it).second.c_str(),data);
		}

		SAFEDELETE(byte_Array);
		SAFEDELETE(word_Array);
		SAFEDELETE(dword_Array);
		SAFEDELETE(flaot_Array);
		SAFEDELETE(long_Array);
		SAFEDELETE(short_Array);
		SAFEDELETE(char_Array);
		SAFEDELETE(bool_Array);
	}
}


void CMoveShape::DecodeChangePropertyFromDataBlock(DBReadSet& setReadDB)
{
	WORD lNum = setReadDB.GetWordFromByteArray();
	WORD pro_type = 0;
	BYTE data_type = 0;
	WORD data_length = 0;
	long data = 0;
	BYTE* byte_Array = NULL;
	WORD* word_Array = NULL;
	DWORD* dword_Array = NULL;
	float* flaot_Array = NULL;
	long*  long_Array = NULL;
	short* short_Array = NULL;
	char*  char_Array = NULL;
	bool*  bool_Array = NULL;

	for (int i = 0;i<lNum;i++)
	{
		pro_type = setReadDB.GetWordFromByteArray();
		data_type = setReadDB.GetByteFromByteArray();
		data_length = setReadDB.GetWordFromByteArray();		
		if (data_length<=0) continue;

		//根据数据类型获取具体的数据值
		switch(data_type)
		{
		case ATTR_DATA_BYTE:
			data = setReadDB.GetByteFromByteArray();
			break;
		case ATTR_DATA_WORD:
			data = setReadDB.GetWordFromByteArray();
			break;
		case ATTR_DATA_DWORD:
			data = setReadDB.GetDwordFromByteArray();
			break;
		case ATTR_DATA_FLOAT:
			data = (long)setReadDB.GetFloatFromByteArray();
			break;
		case ATTR_DATA_LONG:
			data = setReadDB.GetLongFromByteArray();
			break;
		case ATTR_DATA_SHORT:
			data = setReadDB.GetShortFromByteArray();
			break;
		case ATTR_DATA_BOOL:
			data = setReadDB.GetByteFromByteArray();
			break;
		case ATTR_DATA_BYTEARRAY:
			byte_Array = new BYTE[data_length];
			if (!byte_Array) return;
			for (int i = 0;i<data_length;i++)
			{
				byte_Array[i] = setReadDB.GetByteFromByteArray();
			}
			break;
		case ATTR_DATA_WORDARRAY:
			word_Array = new WORD[data_length];
			if (!word_Array) return;
			for (int i = 0;i<data_length;i++)
			{
				word_Array[i] = setReadDB.GetWordFromByteArray();
			}
			break;
		case ATTR_DATA_DWORDARRAY:
			dword_Array = new DWORD[data_length];
			if (!dword_Array) return;
			for (int i = 0;i<data_length;i++)
			{
				dword_Array[i] = setReadDB.GetDwordFromByteArray();
			}
			break;
		case ATTR_DATA_FLOATARRAY:
			flaot_Array = new float[data_length];
			if (!flaot_Array) return;
			for (int i = 0;i<data_length;i++)
			{
				flaot_Array[i] = setReadDB.GetFloatFromByteArray();
			}
			break;
		case ATTR_DATA_LONGARRAY:
			long_Array = new long[data_length];
			if (!long_Array) return;
			for (int i = 0;i<data_length;i++)
			{
				long_Array[i] = setReadDB.GetLongFromByteArray();
			}
			break;
		case ATTR_DATA_SHORTARRAY:
			short_Array = new short[data_length];
			if (!short_Array) return;
			for (int i = 0;i<data_length;i++)
			{
				short_Array[i] = setReadDB.GetShortFromByteArray();
			}
			break;
		case ATTR_DATA_CHARARRAY:
			char_Array = new char[data_length];
			if (!char_Array) return;
			setReadDB.GetStringFromByteArray(char_Array,data_length);
			break;
		case ATTR_DATA_BOOLARRAY:
			bool_Array = new bool[data_length];
			if (!bool_Array) return;
			for (int i = 0;i<data_length;i++)
			{
				data = setReadDB.GetByteFromByteArray();
				if (data!=0)
				{
					bool_Array[i] = true;
				}else
					bool_Array[i] = false;
			}
			break;
		default:
			break;
		}						

		//设置相应属性值
		itProETN it = m_PropertyEnumToName.find(pro_type);
		if(it != m_PropertyEnumToName.end())
		{
			SetProperty( (*it).second.c_str(),data);
		}

		SAFEDELETE(byte_Array);
		SAFEDELETE(word_Array);
		SAFEDELETE(dword_Array);
		SAFEDELETE(flaot_Array);
		SAFEDELETE(long_Array);
		SAFEDELETE(short_Array);
		SAFEDELETE(char_Array);
		SAFEDELETE(bool_Array);
	}
}

//更新MoveShape的属性
void CMoveShape::UpdateProperty(CMessage* pMsg)
{
	DBReadSet setReadDB;
	pMsg->GetDBReadSet(setReadDB);
	CMoveShape::DecodeChangePropertyFromDataBlock(setReadDB);
}

// 站AI
long CMoveShape::AI_Stand()
{
	if( OnAttackEvent() == RET_AI_BREAK )
	{
		return RET_AI_BREAK;
	}

	// 和平状态的IDLE(怪物和NPC只有这一个状态)
	if (GetState()==STATE_PEACE)
	{
		if (m_bIdleStart)
		{
			//判断播放结束
			AnimInfo::tagActionInfo *pActionInfo = m_pAnimInfo->GetActionInfo();
			m_bDisplayFinish =  (pActionInfo->dwCurActionLoopCount > 0);
			if (m_bDisplayFinish)
			{
				SetAction(ACT_IDLE);
				m_lIdleCount = 0;
				m_bIdleStart = false;
			}
		}
		else
		{
			m_lIdleCount++;
			if (m_lIdleCount>s_lIdleMinCount && random(s_lIdleMaxCount) < m_lIdleCount-s_lIdleMinCount)
			{
				m_bIdleStart = true;
				m_lIdleCount = 0;
			}

		}
	}

	return RET_AI_CONTINUE;
}

// 走AI
long CMoveShape::AI_Walk()
{
	if( OnAttackEvent() == RET_AI_BREAK )
	{
		return RET_AI_BREAK;
	}

	if( MoveAI() == RET_AI_BREAK )
	{
		return RET_AI_BREAK;
	}
	return RET_AI_CONTINUE;
}

// 跑AI
long CMoveShape::AI_Run()
{
	if( OnAttackEvent() == RET_AI_BREAK )
	{
		return RET_AI_BREAK;
	}

	if( MoveAI() == RET_AI_BREAK )
	{
		return RET_AI_BREAK;
	}
	return RET_AI_CONTINUE;
}

// 攻擊AI
long CMoveShape::AI_Attack()
{
	//	GetGame()->SetOnAttacted(false);

	if (OnAttackEvent()==RET_AI_BREAK)
	{
		return RET_AI_BREAK;
	}

	return RET_AI_CONTINUE;
}

// 施法AI
long CMoveShape::AI_Perform()
{
	if (OnAttackEvent()==RET_AI_BREAK)
	{
		return RET_AI_BREAK;
	}

	return RET_AI_CONTINUE;
}

// 被攻擊AI
long CMoveShape::AI_BeAttack()
{
	if( OnAttackEvent() == RET_AI_BREAK )
	{
		return RET_AI_BREAK;
	}

	//	GetGame()->SetOnAttacted(false);
	return RET_AI_CONTINUE;
}

// 死亡AI
long CMoveShape::AI_Died()
{
	//判断播放结束
	AnimInfo::tagActionInfo *pActionInfo = m_pAnimInfo->GetActionInfo();
	if (pActionInfo->dwCurActionLoopCount > 0)
	{
		SetAction(ACT_ALREADYDIED);
	}
	return RET_AI_CONTINUE;
}



/*
* 功能: 异常状态的AI
* 摘要: -
* 参数: -
* 返回值: AI后续执行状态
* 作者: -
* 创建日期: -
* 修改日志:
*	2008.01.14 - lpf
*		在该函数中增加了对模型变色时的渐进渐出效果
*	2008.04.04 - lpf
*		调整了异常状态中具有优先显示的颜色(目前只有隐蔽)不会被后发生的异常状态颜色给替换掉.
*	2008.07.07 - lpf
*		判断了在异常状态结束时,需要强制设定模型颜色前对模型是否为空进行了判断.
*	2008.10.20 - lpf
*		使用了新的对象异常状态规则,来显示异常状态下的对象颜色.
*	2008.10.22 - lpf
*		在"是否处理颜色动画效果"时,对当前显示的渐变模型ID进行了判断;
*	2008.11.17 - 刘科
*		修改状态的特效处理方式，每个阶段一个特效列表
*	2009.05.15 - lpf
*		在"其它对象在隐身状态,取消被选择状态"增加了主玩家是否具有反隐能力的判断;
*	2009.07.17 - lpf
*		"是否处理颜色动画效果"中静态变量iLastColorLevel会导致不同实例的MOVESHOP对象颜色显示却是同步的错误现象;
*/
long CMoveShape::AI_StateEx()
{	
	list<tagStateEx>::iterator it;

	for (it = m_listStateEx.begin(); it != m_listStateEx.end();)
	{
		tagStateEx * pStateEx = &(*it);

		// 异常状态时间结束
		if (pStateEx->dwStandingTime != 0)
		{
			DWORD dwCurTickCount = GetCurTickCount();

			if (pStateEx->dwStartTime + pStateEx->dwStandingTime < dwCurTickCount)
				pStateEx->bEndSelf = true;

			// 处于非隐形状态
			if (GetIsConceal())
			{
				if (m_bStateColorPlay &&
					pStateEx->dwStartTime + pStateEx->dwStandingTime - 2000 < dwCurTickCount)
					m_bColorFadeFlag = false;
			}
		}

		CStateListXml::tagStateList * stStateList = CStateListXml::GetState(pStateEx->dwID);
		if(stStateList)
		{
			if (pStateEx->bEndSelf)			//处理异常状态结束
			{
				DWORD dwEffect = 0;
				DWORD dwEffectIndex = 0;
				DWORD dwLocator = 0;
				bool  bIsOrderPlay = false;
				vector<CStateListXml::tagStateEffect> vecStateEffect = stStateList->vecStateLevel[(pStateEx->dwLevel) - 1].vecMidEffect;
				// 删除中间阶段特效
				for (size_t i=0; i<vecStateEffect.size();++i)
				{
					// 获得添加状态的指定等级的特效信息
					dwEffect		= vecStateEffect[i].dwEffectID;
					dwEffectIndex	= vecStateEffect[i].dwEffectIndex;
					bIsOrderPlay	= vecStateEffect[i].bIsOrderPlay;
					// 从本对象身上删除所有的特效
					if(!bIsOrderPlay)
						DelEffect(dwEffect,dwEffectIndex);
				}
				DelAllEffectByOrder();
				// 添加结束阶段特效
				vecStateEffect = stStateList->vecStateLevel[(pStateEx->dwLevel) - 1].vecEndEffect;
				for (size_t i=0; i<vecStateEffect.size();++i)
				{
					// 获得添加状态的指定等级的特效信息
					dwEffect		= vecStateEffect[i].dwEffectID;
					dwEffectIndex	= vecStateEffect[i].dwEffectIndex;
					dwLocator		= vecStateEffect[i].dwEffectPosition;		//施放位置
					eLOCATOR Loc;
					switch(dwLocator)
					{
					case 0: Loc = LOCATOR_WIND; break;
					case 1: Loc = LOCATOR_HEAD; break;
					case 2: Loc = LOCATOR_FOOTCENTER; break;
					case 3: Loc = LOCATOR_LEFTHAND; break;
					case 4: Loc = LOCATOR_RIGHTHAND; break;
					case 5: Loc = LOCATOR_LEFTFOOT; break;
					case 6: Loc = LOCATOR_RIGHTFOOT; break;
					default: Loc = LOCATOR_FOOTCENTER; break;
					}
					// 非隐蔽状态才结束，播放结束时的特效
					if (GetIsConceal())
					{
						if(dwEffect)
							AddEffectLocator(dwEffect, this, Loc, false, 0.0f, dwEffectIndex);
					}
				}
			}
			else
			{
				DWORD dwEffect = 0;
				DWORD dwEffectIndex = 0;
				DWORD dwLocator = 0;
				DWORD dwCycle = 0;
				bool  bIsOrderPlay = false;			// 是否是顺序播放的特效
				bool  bBeginEffectFinish = true;	// 开始阶段的所有特效是否播放完毕
				bool  bMidEffectFinish = true;		// 中间阶段的所有特效是否播放完毕
				vector<CStateListXml::tagStateEffect> vecStateEffect = stStateList->vecStateLevel[(pStateEx->dwLevel) - 1].vecBeginEffect;
				// 开始阶段特效
				for (size_t i=0; i<vecStateEffect.size();++i)
				{
					// 获得添加状态的指定等级的特效信息
					dwEffect		= vecStateEffect[i].dwEffectID;
					dwEffectIndex	= vecStateEffect[i].dwEffectIndex;
					// 隐身状态
					if(!GetIsConceal())
						DelEffect(dwEffect, dwEffectIndex);
					CEffect * pEffect = FindEffect(dwEffect, dwEffectIndex);
					if(pEffect)
					{
						bBeginEffectFinish = false;
						break;
					}
				}
				// 隐身状态
				if (!GetIsConceal())
					m_bStateColorPlay = TRUE;
				// 中间阶段特效
				vecStateEffect = stStateList->vecStateLevel[(pStateEx->dwLevel) - 1].vecMidEffect;
				for (size_t i=0; i<vecStateEffect.size();++i)
				{
					// 获得添加状态的指定等级的特效信息
					dwEffect		= vecStateEffect[i].dwEffectID;
					dwEffectIndex	= vecStateEffect[i].dwEffectIndex;
					dwLocator		= vecStateEffect[i].dwEffectPosition;		//施放位置
					dwCycle			= vecStateEffect[i].dwEffectCycle;			//是否循环
					bIsOrderPlay	= vecStateEffect[i].bIsOrderPlay;			// 是否顺序播放
					bool bCycle		= ((dwCycle == 0) ? false : true);
					eLOCATOR Loc;
					switch(dwLocator)
					{
					case 0: Loc = LOCATOR_WIND; break;
					case 1: Loc = LOCATOR_HEAD; break;
					case 2: Loc = LOCATOR_FOOTCENTER; break;
					case 3: Loc = LOCATOR_LEFTHAND; break;
					case 4: Loc = LOCATOR_RIGHTHAND; break;
					case 5: Loc = LOCATOR_LEFTFOOT; break;
					case 6: Loc = LOCATOR_RIGHTFOOT; break;
					default: Loc = LOCATOR_FOOTCENTER; break;
					}
					// 开始阶段的特效全部释放完毕，且身上没有添加过中间阶段的特效
					CEffect * pEffect = NULL;
					// 不是顺序播放特效在玩家身上找
					if(!bIsOrderPlay)
						pEffect = FindEffect(dwEffect, dwEffectIndex);
					else
						pEffect = FindEffectByOrder(dwEffect, dwEffectIndex);
					if(bBeginEffectFinish && pEffect == NULL)
					{
						// 如果当前对象没有隐身状态则添加状态的特效
						if(GetIsConceal())
						{
							if(!bIsOrderPlay)// 不是按顺序播放的特效
								AddEffectLocator(dwEffect, this, Loc, bCycle, 0.0f, dwEffectIndex);
							else
								AddEffectToList(dwEffect, this, Loc, bCycle, 0.0f, dwEffectIndex);
						}
						else	// 否则不显示状态特效
						{
							if(!bIsOrderPlay)// 不是按顺序播放的特效
								DelEffect(dwEffect, dwEffectIndex);
							else
								DelEffectByOrder(dwEffect, dwEffectIndex);
						}
					}
					else
					{
						long lType, lID;
						GetCurGraphicTypeID(lType, lID);
						if (pEffect && pEffect->GetHostGraphicsID() != lID)		// 判断角色模型是否改变
						{
							if(!bIsOrderPlay) // 不是按顺序播放的特效
							{
								DelEffect(dwEffect, dwEffectIndex);									// 删除角色模型改变前的特效
								AddEffectLocator(dwEffect, this, Loc, bCycle, 0.0f, dwEffectIndex);	// 增加新模型的特效
							}
							else
							{
								DelEffectByOrder(dwEffect, dwEffectIndex);
								AddEffectToList(dwEffect, this, Loc, bCycle, 0.0f, dwEffectIndex);
							}
						}
					}
				}
			}
		}

		if (pStateEx->bEndSelf)
		{
			if (m_bStateColorPlay && m_CurrentStateExForStateColor.dwID == pStateEx->dwID)
			{
				bool bIsEnd = true;
				for (list<tagStateEx>::iterator it2 = m_listStateEx.begin(); it2 != m_listStateEx.end(); ++it2)
				{
					tagStateEx & pStateEx2 = (*it2);
					if (pStateEx2.dwID == m_CurrentStateExForStateColor.dwID && pStateEx2.dwLevel == m_CurrentStateExForStateColor.dwLevel &&
						!pStateEx2.bEndSelf)
					{
						bIsEnd = false;
					}
				}

				if (bIsEnd)
				{
					ZeroMemory(&m_CurrentStateExForStateColor, sizeof(tagStateEx));
					m_bStateColorPlay = FALSE;
					m_bColorFade = false;
					m_bColorFadeFlag	= true;
					m_fColorFadePer		= 1.0f;
					m_colorModel		= s_colorModelDefault;

					if (m_pDisplayModel != NULL)
						m_pDisplayModel->SetModelColorCompel(m_colorModel);
				}
			}

			// 当前的动作处于该状态对应的动作，在状态结束时，设置动作为站立
			CStateListXml::tagStateList *pSL = CStateListXml::GetState(pStateEx->dwID);
			if(pSL)
			{
				if(pSL->wAction == GetAction())
					SetAction(ACT_STAND);
			}
			it = m_listStateEx.erase(it++);	// 删除这个异常状态

			if (m_CurrentStateExForStateColor.dwID == 0)
			{
				for (list<tagStateEx>::iterator it3 = m_listStateEx.begin(); it3 != m_listStateEx.end(); ++it3)
				{
					tagStateEx & pStateEx3 = (*it3);

					CStateListXml::tagStateList * pSL = CStateListXml::GetState(pStateEx3.dwID);
					if (pSL != NULL)
					{
						BOOL bStateColorPlay = (BOOL)pSL->vecStateLevel[(pStateEx3.dwLevel) - 1].vecBuffColor.size();
						if (bStateColorPlay)
						{
							m_CurrentStateExForStateColor.dwID = pStateEx3.dwID;
							m_CurrentStateExForStateColor.dwLevel = pStateEx3.dwLevel;
							m_CurrentStateExForStateColor.dwStartTime = GetCurTickCount();
							m_CurrentStateExForStateColor.dwStandingTime = pStateEx3.dwStandingTime;
							m_CurrentStateExForStateColor.bEndSelf = false;
							m_bStateColorPlay = TRUE;
							m_bColorFade	  = pSL->vecStateLevel[(pStateEx3.dwLevel) - 1].bColorFade;					//是否异常状态颜色动画渐变
						}
					}
				}
			}
		}else
			++it;
	}

	// 是否处理颜色动画效果
	if (m_bStateColorPlay && m_CurrentStateExForStateColor.dwID != 0)
	{
		// 记录最后一次更新时间
		static DWORD dwLastTime = timeGetTime();
		CStateListXml::tagStateList * pStateListForColor = CStateListXml::GetState(m_CurrentStateExForStateColor.dwID);
		if (pStateListForColor != NULL)
		{
			DWORD dwInterval = pStateListForColor->vecStateLevel[(m_CurrentStateExForStateColor.dwLevel) - 1].dwInterval;					//模型颜色变化间隔时间

			// 每隔一段间隔时间变化一次颜色
			if (timeGetTime() - dwLastTime > dwInterval)
			{
				if (m_iLastColorLevel < (int)pStateListForColor->vecStateLevel[(m_CurrentStateExForStateColor.dwLevel) - 1].vecBuffColor.size())
				{
					m_colorModel = pStateListForColor->vecStateLevel[(m_CurrentStateExForStateColor.dwLevel) - 1].vecBuffColor[m_iLastColorLevel];
					++m_iLastColorLevel;
					dwLastTime = timeGetTime();
				}else
					m_iLastColorLevel = 0;
			}
		}
	}

	//假死状态
	if (IsInDieAwayState())
		SetAction(ACT_DIED);
	// 其它对象在隐身状态,取消被选择状态
	if(!GetIsConceal() && this != GetGame()->GetMainPlayer() &&
		GetGame()->GetMainPlayer()->GetIsSpy())
	{
		// 非队友
		if(!GetGame()->GetMainPlayer()->IsTeammate(GetExID()))
		{
			if(this == GetGame()->GetGameControl()->GetSelectedShape())
				GetGame()->GetGameControl()->SetSelectedShape(NULL);
			if(this == GetGame()->GetGameControl()->GetCurAttractShape())
				GetGame()->GetGameControl()->SetCurAttractShape(NULL);
            if ( this == GetGame()->GetGameControl()->GetSelectedShape() )
            {
                FireUIEvent("ObjectPage", "Closed");
            }
		}
	}

	// //重生状态
	//if(IsHaveStateEx(82)&& GetState() == STATE_DIED )
	//	SetAction(ACT_STAND);

	return RET_AI_CONTINUE;
}

// 平滑移位AI
void CMoveShape::AI_SlipMove()
{
	// 达到了平滑目的点
	if(GetIsSlipState() && GetCurTickCount() >= GetSlipEndMoveTime())
	{
		//纠正误差
		if(GetTileX() != (long)GetSlipDestPosX() || GetTileY() != (long)GetSlipDestPosY() )
			SetPosXY(GetSlipDestPosX(), GetSlipDestPosY());
		SetIsSlipState(false);
	}
}

// 技能AI
void CMoveShape::AI_Skill()
{
	if(m_mapCurSkill.size()>0)
	{
		vector<long> vecDelSkill;
		map<long, CSkillXml*>::iterator it;
		for (it = m_mapCurSkill.begin(); it != m_mapCurSkill.end(); it++)
		{
			if(it->second->GetIsDeleted())
				vecDelSkill.push_back(it->first);
			else
			{
				it->second->AI();
				if(it->second->GetIsDeleted())
					vecDelSkill.push_back(it->first);
			}
		}
		if(vecDelSkill.size()>0)
		{
			for (vector<long>::iterator vit = vecDelSkill.begin(); vit != vecDelSkill.end(); vit++)
			{
				DeleteCurSkill((*vit));
			}
			vecDelSkill.clear();
		}
	}
}
// 是否有某种状态
bool CMoveShape::IsHaveStateEx(DWORD dwID)
{
	list<tagStateEx>::iterator it;
	for(it = m_listStateEx.begin(); it != m_listStateEx.end();it++)
	{
		tagStateEx* pStateEx = &(*it);
		if (pStateEx && pStateEx->dwID == dwID)
			return true;
	}

	return false;
}
// 是否处于隐蔽状态
//bool CMoveShape::IsInHideState()
//{
//	if(IsHaveStateEx(55))
//		return true;
//	else 
//		return false; 
//}
// 是否处于假死状态
bool CMoveShape::IsInDieAwayState()
{
	if (IsHaveStateEx(81))
		return true;
	else 
		return false;
}



/*
* 功能: MoveShape的显示
* 摘要: 该函数执行模型主体的显示,并处理了光照效果
* 参数: -
* 返回值: 执行成功返回ture,否则返回false
* 作者: -
* 创建日期: -
* 修改日志:
*	2008.01.10 - lpf
*		在该函数中增加了对模型附加特效的渲染(如闪光,震动)
*	2008.01.16 - lpf
*		增加了处理异常状态的颜色动画播放
*	2008.03.10 - lpf
*		增加了处理异常状态的颜色里绘制透明模型的功能
*	2008.03.11 - lpf
*		在处理异常状态的颜色,增加了m_dwCurrentModelCol的Alpha通道值的修改,因为该值在人物手持武器渲染时会用到
*	2008.09.10 - lpf
*		在处理异常状态的颜色时,增加了对是否颜色渐变的控制
*	2008.09.18 - lpf
*		恢复了9.10的修改
*	2008.10.17 - lpf
*		增加了对特效渐进，渐出的处理
*	2009.05.22 - lpf
*		修正取消光照设定之后,没有处理异常状态颜色的BUG,使得人物无法完成渐隐功能
*	2009.07.17 - lpf
*		修正了在处理异常状态颜色的渐隐时,会出现不处理骨骼数据的情况,之前造成的瞬间黑屏应该就是这个问题
*/
bool CMoveShape::Display()
{
	// 光照向量
	static D3DXVECTOR3 vLightVector(-0.613793f, -0.789465f, 0.001831f);

	CShape::Display();

	// 判断函数执行条件
	if (!IsNeedDisplay() ||
		m_pDisplayModel == NULL ||
		m_pDisplayModel->GetLoadState() != Load_Did)
		return false;

	// 得到相机
	CClientRegion * pRegion = GetGame()->GetRegion();
	if(pRegion == NULL)
		return false;

	render::Camera * pCamera = pRegion->GetCamera();

	// 处理模型震动效果
	if (m_listAddEffect[ADDEFFECT_SHAKE].bPlayState)
	{
		if (timeGetTime() - m_listAddEffect[ADDEFFECT_SHAKE].dwStartTime >= m_listAddEffect[ADDEFFECT_SHAKE].dwTimeLength)
		{
			m_fXIncrement								= 0.0f;
			m_fYIncrement								= 0.0f;
			m_bRestoration								= false;
			m_listAddEffect[ADDEFFECT_SHAKE].bPlayState = false;
		}

		if (m_bRestoration)
		{
			float fExtent  = (float)m_listAddEffect[ADDEFFECT_SHAKE].dwValue * 0.01f;
			m_fXIncrement  = (float)rand() / RAND_MAX * fExtent + m_fShakeMin;
			m_fYIncrement  = (float)rand() / RAND_MAX * fExtent + m_fShakeMin;
			m_bRestoration = false;
		}
		else
		{
			m_fXIncrement  *= -1.0f;
			m_fYIncrement  *= -1.0f;
			m_bRestoration = true;
		}
	}
	/*D3DXMATRIX mats;
	D3DXMatrixScaling(&mats,9,9,9);
	m_pAnimInfo->SetScalMatrix(&mats);*/

	// 处理模型相关数据
	m_pAnimInfo->SetupAnimInfo(GetPosX() + m_fXIncrement, GetPosY() + m_fYIncrement, GetHeight(), GetNowDir(), pCamera);

	//m_pDisplayModel->SetRegionAmbient(GetGame()->GetRegion()->GetGameMap()->GetSunLightAmbientColorforBuild());

	if (m_bStateColorPlay)			//处理异常状态的颜色动画播放
	{
		// AddBy--- 彭谦----当颜色的RGB都为FF的时候,才是透明效果,否则为颜色变换效果 ,变色时关闭光照
		DWORD dwColor;
		bool  bIsColorFade = false;

		/*if (GetGame()->GetSetup()->lLight)
		{*/
		//m_pDisplayModel->SetModelColor(0xFFcfcfcf);
		//m_pDisplayModel->ProcessDirectionalLight(pCamera->GetPosition(),(D3DXVECTOR3*)&vLightVector,0xFF808080);
		GameMap *pGameMap = pRegion->GetGameMap();
		dwColor = pGameMap->GetGridColor(GetTileY(),GetTileX());

		int iMA, iMR, iMG, iMB;
		DWORD dw(m_colorModel);
		if (m_colorModel != s_colorModelDefault)
		{
			bIsColorFade = true;

			//变色
			if ((dw & 0x00ffffff) != 0x00ffffff)
			{
				m_pDisplayModel->SetDirLightEnable(false);
				m_pDisplayModel->ProcessAnimJoint(m_pAnimInfo);

				// 处理是否渐变
				if (m_bColorFade)
				{
					int iR = (dw & 0x00ff0000) >> 16;
					int iG = (dw & 0x0000ff00) >> 8;
					int iB = (dw & 0x000000ff);

					int SR = (dwColor & 0x00ff0000) >> 16;
					int SG = (dwColor & 0x0000ff00) >> 8;
					int SB = (dwColor & 0x000000ff);

					// 处理颜色渐进，渐出
					if (m_bColorFadeFlag)
					{
						iMR = (int)((float)SR * m_fColorFadePer);
						iMG = (int)((float)SG * m_fColorFadePer);
						iMB = (int)((float)SB * m_fColorFadePer);
						m_fColorFadePer -= 0.01f;

						if (iMR < iR) iMR = iR;
						if (iMG < iG) iMG = iG;
						if (iMB < iB) iMB = iB;
						if (m_fColorFadePer < 0.0f) m_fColorFadePer = 0.0f;
					}
					else
					{
						iMR = iR + (int)((float)SR * m_fColorFadePer);
						iMG = iG + (int)((float)SR * m_fColorFadePer);
						iMB = iB + (int)((float)SR * m_fColorFadePer);
						m_fColorFadePer += 0.01f;

						if (iMR > 0xff) iMR = 0x000000ff;
						if (iMG > 0xff)	iMG = 0x000000ff;
						if (iMB > 0xff)	iMB = 0x000000ff;
						if (m_fColorFadePer > 1.0f) m_fColorFadePer = 1.0f;
					}
					m_dwCurrentModelCol = (0xff << 24 | (DWORD)iMR << 16 | (DWORD)iMG << 8 | (DWORD)iMB);
					m_pDisplayModel->SetModelColorCompel(m_dwCurrentModelCol);
				}else
				{
					m_dwCurrentModelCol = dw;
					m_pDisplayModel->SetModelColorCompel(m_dwCurrentModelCol);
				}
			}
			//渐隐
			else
			{
				bIsColorFade = false;
				m_pDisplayModel->SetDirLightEnable(GetGame()->GetSetup()->lLight>0);
				m_pDisplayModel->ProcessAnimJoint(m_pAnimInfo);

				// 处理是否渐变
				if (m_bColorFade)
				{
					int iA = (dw & 0xff000000) >> 24;
					int iR = (dwColor & 0x00ff0000) >> 16;
					int iG = (dwColor & 0x0000ff00) >> 8;
					int iB = (dwColor & 0x000000ff);

					// 处理颜色渐进，渐出
					if (m_bColorFadeFlag)
					{
						iMA = (int)(255.0f * m_fColorFadePer);
						m_fColorFadePer -= 0.01f;
						if (iMA < iA) iMA = iA;
						if (m_fColorFadePer < 0.0f) m_fColorFadePer = 0.0f;
					}
					else
					{
						iMA = iA + (int)(255.0f * m_fColorFadePer);
						m_fColorFadePer += 0.01f;
						if (iMA > 0xff) iMA = 0x000000ff;
						if (m_fColorFadePer > 1.0f) m_fColorFadePer = 1.0f;
					}
					m_dwCurrentModelCol = ((DWORD)iMA << 24 | (DWORD)iR << 16 | (DWORD)iG << 8 | (DWORD)iB);
					m_pDisplayModel->SetModelColorCompel(m_dwCurrentModelCol);
					m_dwCurrentModelCol = ((DWORD)iMA << 24 | 0xff << 16 | 0xff << 8 | 0xff);

					//if (m_bIsNeedAlphaFade)
					//m_iAlphaFade = (DWORD)iMA;
				}else
				{
					m_dwCurrentModelCol = dw;
					m_pDisplayModel->SetModelColorCompel(m_dwCurrentModelCol);

					//if (m_bIsNeedAlphaFade)
					//m_iAlphaFade = (int)((m_dwCurrentModelCol & 0xff000000) >> 24);
				}
			}
		}else
		{
			m_pDisplayModel->SetDirLightEnable(GetGame()->GetSetup()->lLight>0);
			m_pDisplayModel->ProcessAnimJoint(m_pAnimInfo);
			m_dwCurrentModelCol = dwColor;
			m_pDisplayModel->SetModelColor(m_dwCurrentModelCol);
			m_dwCurrentModelCol = ((DWORD)0xff << 24 | 0xff << 16 | 0xff << 8 | 0xff);
		}

		//m_pDisplayModel->ProcessDirectionalLight(pCamera->GetPosition(), &vLightVector, m_dwCurrentModelCol);
		/*}
		else
		{
		GameMap *pGameMap = pRegion->GetGameMap();
		dwColor = pGameMap->GetGridColor(GetTileY(),GetTileX());
		m_pDisplayModel->SetDirLightEnable(false);
		m_pDisplayModel->ProcessAnimJoint(m_pAnimInfo);
		m_pDisplayModel->SetModelColor(dwColor);
		}*/

		// 处理模型渐变效果
		if (m_bIsNeedAlphaFade && m_eAlphaFadeMode == CAlphaFadeObjectList::ALPHA_FADE_IN)
		{
			long lElapseTime = timeGetTime() - m_lAlphaFadeInStartTime;
			if (lElapseTime > m_lAlphaFadeInTimeLength)
			{
				lElapseTime		 = m_lAlphaFadeInTimeLength;
				m_eAlphaFadeMode = CAlphaFadeObjectList::ALPHA_FADE_NONE;
			}

			float fAlphaValue = (float)lElapseTime / (float)m_lAlphaFadeInTimeLength;

			if (!bIsColorFade)
			{
				m_dwCurrentModelCol &= 0x00ffffff;
				m_dwCurrentModelCol |= ((DWORD)(fAlphaValue * 255.0f)) << 24;
			}

			m_pDisplayModel->MultiplyModelAlpha(fAlphaValue);
		}

		m_pDisplayModel->ProcessAnimTexture(m_pAnimInfo);
		m_pDisplayModel->ProcessAnimUV(m_pAnimInfo);
		m_pDisplayModel->ProcessEnvironmentMaps(pCamera->GetViewMatrix());
		m_pDisplayModel->ProcessParticleEmitter(m_pAnimInfo);
		m_pDisplayModel->ProcessTextureProjective(m_pAnimInfo);
		m_pDisplayModel->ProcessTrailEmitter(m_pAnimInfo);
		m_pDisplayModel->ProcessLocator(m_pAnimInfo);

		/*m_pDisplayModel->MultiplyModelColor(GetModelColor());
		m_pDisplayModel->MultiplyModelAlpha( (float)(GetModelColor()>>24)/255.0f );*/
		m_pDisplayModel->ProcessAnimColor(m_pAnimInfo);
		m_pDisplayModel->ProcessVisibility(m_pAnimInfo);

		if (((m_dwCurrentModelCol >> 24)) == 0xff && m_eAlphaFadeMode == CAlphaFadeObjectList::ALPHA_FADE_NONE)
			m_pDisplayModel->RenderModel();
		else
			m_pDisplayModel->RenderModelTransparent();
	}
	// 处理光照以及模型颜色效果
	else if (m_listAddEffect[ADDEFFECT_FLASH].bPlayState)			//闪光效果
	{
		if (timeGetTime() - m_listAddEffect[ADDEFFECT_FLASH].dwStartTime >= m_listAddEffect[ADDEFFECT_FLASH].dwTimeLength)
		{
			m_pDisplayModel->SetModelColorCompel(0xffffffff);
			m_listAddEffect[ADDEFFECT_FLASH].bPlayState = false;
		}

		DWORD dwColor = 0xff000000 |
			(m_listAddEffect[ADDEFFECT_FLASH].dwValue << 16) |
			(m_listAddEffect[ADDEFFECT_FLASH].dwValue << 8) |
			m_listAddEffect[ADDEFFECT_FLASH].dwValue;
		m_pDisplayModel->SetDirLightEnable(false);
		m_pDisplayModel->ProcessAnimJoint(m_pAnimInfo);
		m_pDisplayModel->ProcessAnimTexture(m_pAnimInfo);
		m_pDisplayModel->ProcessAnimUV(m_pAnimInfo);
		m_pDisplayModel->ProcessEnvironmentMaps(pCamera->GetViewMatrix());
		m_pDisplayModel->ProcessParticleEmitter(m_pAnimInfo);
		m_pDisplayModel->ProcessTextureProjective(m_pAnimInfo);
		m_pDisplayModel->ProcessTrailEmitter(m_pAnimInfo);
		m_pDisplayModel->ProcessLocator(m_pAnimInfo);
		m_pDisplayModel->RenderModelFlash(dwColor);
	}
	else			//正常模式下的渲染
	{
		DWORD dwColor;
		REGION_LIGHT  edn = GetGame()->GetRegion()->GetClientReionLightType();
		if (edn == RL_CAVE && this != GetGame()->GetMainPlayer())
			m_pDisplayModel->ForceToEnableLight();
		else
			m_pDisplayModel->SetDirLightEnable(GetGame()->GetSetup()->lLight>0?true:false);
		//m_pDisplayModel->MultiplyModelColor(dwColor);
		//m_pDisplayModel->MultiplyModelColor(dwColor);
		//m_pDisplayModel->SetAmbient(dwColor);

		//m_pDisplayModel->SetDirLightEnable(false);


		//m_pDisplayModel->MultiplyModelColor(GetModelColor());
		//m_pDisplayModel->MultiplyModelAlpha( (float)(GetModelColor()>>24)/255.0f );
		m_pDisplayModel->ProcessAnimJoint(m_pAnimInfo);
		if (GetGame()->GetSetup()->lLight)
		{
			//m_pDisplayModel->SetModelColor(0xFFcfcfcf);
			//m_pDisplayModel->ProcessDirectionalLight(pCamera->GetPosition(),(D3DXVECTOR3*)&vLightVector,0xFF808080);
			GameMap *pGameMap = pRegion->GetGameMap();
			dwColor = pGameMap->GetGridColor(GetTileY(),GetTileX());
			/***********************************************************************/
			/*  zhaohang 2010-11-16 fix 有float改为DWORD*/
			DWORD coefficientR = pGameMap->GetGridColorcoefficientforMoveShapR();
			DWORD coefficientG = pGameMap->GetGridColorcoefficientforMoveShapG();
			DWORD coefficientB = pGameMap->GetGridColorcoefficientforMoveShapB();
			/***********************************************************************/
			
			//颜色增加2倍 * 场景中格子颜色的影响系数
			DWORD r = ( (dwColor & 0x00FF0000) >> 16) * 2 + (DWORD)coefficientR;
			if (r > 255 )
				r = 255;
			DWORD g = ( (dwColor & 0x0000FF00) >>  8) * 2 + (DWORD)coefficientG;
			if (g > 255) 
				g = 255;
			DWORD b = (  dwColor & 0x000000FF) * 2 + (DWORD)coefficientB;
			if (b > 255)
				b = 255;
			dwColor = 0xff000000 | r << 16 | g << 8 | b;
			m_pDisplayModel->SetModelColor(pGameMap->GetSunLightAmbientColorforMoveShap());
			m_pDisplayModel->MultiplyModelColor(dwColor);

			//if (m_bIsNeedAlphaFade && m_eAlphaFadeMode == CAlphaFadeObjectList::ALPHA_FADE_NONE)
			//	m_iAlphaFade = (int)((dwColor & 0xff000000) >> 24);
		}
		else
		{
			dwColor = 0xffffffff;
			m_pDisplayModel->SetModelColor(dwColor);

			//if (m_bIsNeedAlphaFade && m_eAlphaFadeMode == CAlphaFadeObjectList::ALPHA_FADE_NONE)
			//	m_iAlphaFade = (int)((dwColor & 0xff000000) >> 24);
		}

		// 处理模型渐变效果
		if (m_bIsNeedAlphaFade && m_eAlphaFadeMode == CAlphaFadeObjectList::ALPHA_FADE_IN)
		{
			long lElapseTime = timeGetTime() - m_lAlphaFadeInStartTime;
			if (lElapseTime > m_lAlphaFadeInTimeLength)
			{
				lElapseTime		 = m_lAlphaFadeInTimeLength;
				m_eAlphaFadeMode = CAlphaFadeObjectList::ALPHA_FADE_NONE;
			}

			m_pDisplayModel->MultiplyModelAlpha((float)lElapseTime / (float)m_lAlphaFadeInTimeLength);
		}
		m_pDisplayModel->ProcessEnvironmentMaps(pCamera->GetViewMatrix());
		m_pDisplayModel->ProcessAnimTexture(m_pAnimInfo);
		m_pDisplayModel->ProcessAnimUV(m_pAnimInfo);
		m_pDisplayModel->ProcessParticleEmitter(m_pAnimInfo);
		m_pDisplayModel->ProcessTextureProjective(m_pAnimInfo);
		m_pDisplayModel->ProcessTrailEmitter(m_pAnimInfo);
		m_pDisplayModel->ProcessLocator(m_pAnimInfo);

		/*m_pDisplayModel->MultiplyModelColor(GetModelColor());
		m_pDisplayModel->MultiplyModelAlpha( (float)(GetModelColor()>>24)/255.0f );*/
		m_pDisplayModel->ProcessAnimColor(m_pAnimInfo);
		m_pDisplayModel->ProcessVisibility(m_pAnimInfo);

		if (m_eAlphaFadeMode == CAlphaFadeObjectList::ALPHA_FADE_NONE)
		{
			m_pDisplayModel->RenderModel();
		}
		else
			m_pDisplayModel->RenderModelTransparent();
	}
	SkillDisplay();
	DisPlayEffectByOrder();
	return true;
}



/*
* 功能: MoveShape根据绑定点的显示
* 摘要: 该函数执行模型主体的显示,并处理了光照效果
* 参数: -
* 返回值: 执行成功返回ture,否则返回false
* 作者: -
* 创建日期: -
* 修改日志:
*	2009.07.17 - lpf
*		修正了在处理异常状态颜色的渐隐时,会出现不处理骨骼数据的情况,之前造成的瞬间黑屏应该就是这个问题
*/
bool CMoveShape::DisplayByLocator(CDisplayModel *pParentModel,AnimInfo *pParentAnimInfo,DWORD locatorname)
{
	// 光照向量
	static D3DXVECTOR3 vLightVector(-0.613793f, -0.789465f, 0.001831f);

	CShape::Display();

	// 判断函数执行条件
	if (!IsNeedDisplay() ||
		m_pDisplayModel == NULL ||
		m_pDisplayModel->GetLoadState() != Load_Did)
		return false;

	// 得到相机
	CClientRegion * pRegion = GetGame()->GetRegion();
	if(pRegion == NULL)
		return false;

	render::Camera * pCamera = pRegion->GetCamera();


	// 处理模型震动效果
	if (m_listAddEffect[ADDEFFECT_SHAKE].bPlayState)
	{
		if (timeGetTime() - m_listAddEffect[ADDEFFECT_SHAKE].dwStartTime >= m_listAddEffect[ADDEFFECT_SHAKE].dwTimeLength)
		{
			m_fXIncrement								= 0.0f;
			m_fYIncrement								= 0.0f;
			m_bRestoration								= false;
			m_listAddEffect[ADDEFFECT_SHAKE].bPlayState = false;
		}

		if (m_bRestoration)
		{
			float fExtent  = (float)m_listAddEffect[ADDEFFECT_SHAKE].dwValue * 0.01f;
			m_fXIncrement  = (float)rand() / RAND_MAX * fExtent + m_fShakeMin;
			m_fYIncrement  = (float)rand() / RAND_MAX * fExtent + m_fShakeMin;
			m_bRestoration = false;
		}
		else
		{
			m_fXIncrement  *= -1.0f;
			m_fYIncrement  *= -1.0f;
			m_bRestoration = true;
		}
	}

	// 处理模型相关数据
	m_pDisplayModel->GetAnimInfobyLocator(m_pAnimInfo,pParentModel,pParentAnimInfo,locatorname);
	m_pAnimInfo->SetViewMatrix(pRegion->GetCamera()->GetViewMatrix());
	m_pAnimInfo->SetProjectedMatrix(pRegion->GetCamera()->GetProjectionMatrix());
	//m_pDisplayModel->SetRegionAmbient(GetGame()->GetRegion()->GetGameMap()->GetSunLightAmbientColorforBuild());

	if (m_bStateColorPlay)			//处理异常状态的颜色动画播放
	{
		// AddBy--- 彭谦----当颜色的RGB都为FF的时候,才是透明效果,否则为颜色变换效果 ,变色时关闭光照
		DWORD dwColor;
		bool bIsColorFade = false;

		/*if (GetGame()->GetSetup()->lLight)
		{*/
		//m_pDisplayModel->SetModelColor(0xFFcfcfcf);
		//m_pDisplayModel->ProcessDirectionalLight(pCamera->GetPosition(),(D3DXVECTOR3*)&vLightVector,0xFF808080);
		GameMap *pGameMap = pRegion->GetGameMap();
		dwColor = pGameMap->GetGridColor(GetTileY(),GetTileX());

		int iMA, iMR, iMG, iMB;
		DWORD dw(m_colorModel);
		if (m_colorModel != s_colorModelDefault)
		{
			bIsColorFade = true;

			//变色
			if ((dw & 0x00ffffff) != 0x00ffffff)
			{
				m_pDisplayModel->SetDirLightEnable(false);
				m_pDisplayModel->ProcessAnimJoint(m_pAnimInfo);

				// 处理是否渐变
				if (m_bColorFade)
				{
					int iR = (dw & 0x00ff0000) >> 16;
					int iG = (dw & 0x0000ff00) >> 8;
					int iB = (dw & 0x000000ff);

					int SR = (dwColor & 0x00ff0000) >> 16;
					int SG = (dwColor & 0x0000ff00) >> 8;
					int SB = (dwColor & 0x000000ff);

					// 处理颜色渐进，渐出
					if (m_bColorFadeFlag)
					{
						iMR = (int)((float)SR * m_fColorFadePer);
						iMG = (int)((float)SG * m_fColorFadePer);
						iMB = (int)((float)SB * m_fColorFadePer);
						m_fColorFadePer -= 0.01f;

						if (iMR < iR) iMR = iR;
						if (iMG < iG) iMG = iG;
						if (iMB < iB) iMB = iB;
						if (m_fColorFadePer < 0.0f) m_fColorFadePer = 0.0f;
					}
					else
					{
						iMR = iR + (int)((float)SR * m_fColorFadePer);
						iMG = iG + (int)((float)SR * m_fColorFadePer);
						iMB = iB + (int)((float)SR * m_fColorFadePer);
						m_fColorFadePer += 0.01f;

						if (iMR > 0xff) iMR = 0x000000ff;
						if (iMG > 0xff)	iMG = 0x000000ff;
						if (iMB > 0xff)	iMB = 0x000000ff;
						if (m_fColorFadePer > 1.0f) m_fColorFadePer = 1.0f;
					}
					m_dwCurrentModelCol = (0xff << 24 | (DWORD)iMR << 16 | (DWORD)iMG << 8 | (DWORD)iMB);
					m_pDisplayModel->SetModelColorCompel(m_dwCurrentModelCol);
				}else
				{
					m_dwCurrentModelCol = dw;
					m_pDisplayModel->SetModelColorCompel(m_dwCurrentModelCol);
				}
			}
			//渐隐
			else
			{
				bIsColorFade = false;
				m_pDisplayModel->SetDirLightEnable(GetGame()->GetSetup()->lLight>0);
				m_pDisplayModel->ProcessAnimJoint(m_pAnimInfo);

				// 处理是否渐变
				if (m_bColorFade)
				{
					int iA = (dw & 0xff000000) >> 24;
					int iR = (dwColor & 0x00ff0000) >> 16;
					int iG = (dwColor & 0x0000ff00) >> 8;
					int iB = (dwColor & 0x000000ff);

					// 处理颜色渐进，渐出
					if (m_bColorFadeFlag)
					{
						iMA = (int)(255.0f * m_fColorFadePer);
						m_fColorFadePer -= 0.01f;
						if (iMA < iA) iMA = iA;
						if (m_fColorFadePer < 0.0f) m_fColorFadePer = 0.0f;
					}
					else
					{
						iMA = iA + (int)(255.0f * m_fColorFadePer);
						m_fColorFadePer += 0.01f;
						if (iMA > 0xff) iMA = 0x000000ff;
						if (m_fColorFadePer > 1.0f) m_fColorFadePer = 1.0f;
					}
					m_dwCurrentModelCol = ((DWORD)iMA << 24 | (DWORD)iR << 16 | (DWORD)iG << 8 | (DWORD)iB);
					m_pDisplayModel->SetModelColorCompel(m_dwCurrentModelCol);
					m_dwCurrentModelCol = ((DWORD)iMA << 24 | 0xff << 16 | 0xff << 8 | 0xff);

					//if (m_bIsNeedAlphaFade)
					//m_iAlphaFade = (DWORD)iMA;
				}else
				{
					m_dwCurrentModelCol = dw;
					m_pDisplayModel->SetModelColorCompel(m_dwCurrentModelCol);

					//if (m_bIsNeedAlphaFade)
					//m_iAlphaFade = (int)((m_dwCurrentModelCol & 0xff000000) >> 24);
				}
			}
		}else
		{
			m_pDisplayModel->SetDirLightEnable(GetGame()->GetSetup()->lLight>0);
			m_pDisplayModel->ProcessAnimJoint(m_pAnimInfo);
			m_dwCurrentModelCol = dwColor;
			m_pDisplayModel->SetModelColor(m_dwCurrentModelCol);
			m_dwCurrentModelCol = ((DWORD)0xff << 24 | 0xff << 16 | 0xff << 8 | 0xff);
		}

		//m_pDisplayModel->ProcessDirectionalLight(pCamera->GetPosition(), &vLightVector, m_dwCurrentModelCol);
		//}
		/*else
		{
		GameMap *pGameMap = pRegion->GetGameMap();
		dwColor = pGameMap->GetGridColor(GetTileY(),GetTileX());
		m_pDisplayModel->SetDirLightEnable(false);
		m_pDisplayModel->ProcessAnimJoint(m_pAnimInfo);
		m_pDisplayModel->SetModelColor(dwColor);
		}*/

		// 处理模型渐变效果
		if (m_bIsNeedAlphaFade && m_eAlphaFadeMode == CAlphaFadeObjectList::ALPHA_FADE_IN)
		{
			long lElapseTime = timeGetTime() - m_lAlphaFadeInStartTime;
			if (lElapseTime > m_lAlphaFadeInTimeLength)
			{
				lElapseTime		 = m_lAlphaFadeInTimeLength;
				m_eAlphaFadeMode = CAlphaFadeObjectList::ALPHA_FADE_NONE;
			}

			float fAlphaValue = (float)lElapseTime / (float)m_lAlphaFadeInTimeLength;

			if (!bIsColorFade)
			{
				m_dwCurrentModelCol &= 0x00ffffff;
				m_dwCurrentModelCol |= ((DWORD)(fAlphaValue * 255.0f)) << 24;
			}

			m_pDisplayModel->MultiplyModelAlpha(fAlphaValue);
		}

		m_pDisplayModel->ProcessAnimTexture(m_pAnimInfo);
		m_pDisplayModel->ProcessAnimUV(m_pAnimInfo);
		m_pDisplayModel->ProcessEnvironmentMaps(pCamera->GetViewMatrix());
		m_pDisplayModel->ProcessParticleEmitter(m_pAnimInfo);
		m_pDisplayModel->ProcessTextureProjective(m_pAnimInfo);
		m_pDisplayModel->ProcessTrailEmitter(m_pAnimInfo);
		m_pDisplayModel->ProcessLocator(m_pAnimInfo);

		/*m_pDisplayModel->MultiplyModelColor(GetModelColor());
		m_pDisplayModel->MultiplyModelAlpha( (float)(GetModelColor()>>24)/255.0f );*/
		m_pDisplayModel->ProcessAnimColor(m_pAnimInfo);
		m_pDisplayModel->ProcessVisibility(m_pAnimInfo);

		if (((m_dwCurrentModelCol >> 24)) == 0xff && m_eAlphaFadeMode == CAlphaFadeObjectList::ALPHA_FADE_NONE)
			m_pDisplayModel->RenderModel();
		else
			m_pDisplayModel->RenderModelTransparent();
	}
	// 处理光照以及模型颜色效果
	else if (m_listAddEffect[ADDEFFECT_FLASH].bPlayState)			//闪光效果
	{
		if (timeGetTime() - m_listAddEffect[ADDEFFECT_FLASH].dwStartTime >= m_listAddEffect[ADDEFFECT_FLASH].dwTimeLength)
		{
			m_pDisplayModel->SetModelColorCompel(0xffffffff);
			m_listAddEffect[ADDEFFECT_FLASH].bPlayState = false;
		}

		DWORD dwColor = 0xff000000 |
			(m_listAddEffect[ADDEFFECT_FLASH].dwValue << 16) |
			(m_listAddEffect[ADDEFFECT_FLASH].dwValue << 8) |
			m_listAddEffect[ADDEFFECT_FLASH].dwValue;
		m_pDisplayModel->SetDirLightEnable(false);
		m_pDisplayModel->ProcessAnimJoint(m_pAnimInfo);
		m_pDisplayModel->ProcessAnimTexture(m_pAnimInfo);
		m_pDisplayModel->ProcessAnimUV(m_pAnimInfo);
		m_pDisplayModel->ProcessEnvironmentMaps(pCamera->GetViewMatrix());
		m_pDisplayModel->ProcessParticleEmitter(m_pAnimInfo);
		m_pDisplayModel->ProcessTextureProjective(m_pAnimInfo);
		m_pDisplayModel->ProcessTrailEmitter(m_pAnimInfo);
		m_pDisplayModel->ProcessLocator(m_pAnimInfo);
		m_pDisplayModel->RenderModelFlash(dwColor);
	}
	else			//正常模式下的渲染
	{
		DWORD dwColor;
		REGION_LIGHT  edn = GetGame()->GetRegion()->GetClientReionLightType();
		if (edn == RL_CAVE && this != GetGame()->GetMainPlayer())
			m_pDisplayModel->ForceToEnableLight();
		else
			m_pDisplayModel->SetDirLightEnable(GetGame()->GetSetup()->lLight>0?true:false);
		//m_pDisplayModel->MultiplyModelColor(dwColor);
		//m_pDisplayModel->MultiplyModelColor(dwColor);
		//m_pDisplayModel->SetAmbient(dwColor);

		//m_pDisplayModel->SetDirLightEnable(false);


		//m_pDisplayModel->MultiplyModelColor(GetModelColor());
		//m_pDisplayModel->MultiplyModelAlpha( (float)(GetModelColor()>>24)/255.0f );
		m_pDisplayModel->ProcessAnimJoint(m_pAnimInfo);
		if (GetGame()->GetSetup()->lLight)
		{
			//m_pDisplayModel->SetModelColor(0xFFcfcfcf);
			//m_pDisplayModel->ProcessDirectionalLight(pCamera->GetPosition(),(D3DXVECTOR3*)&vLightVector,0xFF808080);
			GameMap *pGameMap = pRegion->GetGameMap();
			dwColor = pGameMap->GetGridColor(GetTileY(),GetTileX());
			DWORD coefficientR = pGameMap->GetGridColorcoefficientforMoveShapR();
			DWORD coefficientG = pGameMap->GetGridColorcoefficientforMoveShapG();
			DWORD coefficientB = pGameMap->GetGridColorcoefficientforMoveShapB();
			//颜色增加2倍 * 场景中格子颜色的影响系数
			DWORD r = ( (dwColor & 0x00FF0000) >> 16) * 2 + coefficientR;
			if (r > 255 )
				r = 255;
			DWORD g = ( (dwColor & 0x0000FF00) >>  8) * 2 + coefficientG;
			if (g > 255) 
				g = 255;
			DWORD b = (  dwColor & 0x000000FF) * 2 + coefficientB;
			if (b > 255)
				b = 255;
			dwColor = 0xff000000 | r << 16 | g << 8 | b;
			m_pDisplayModel->SetModelColor(pGameMap->GetSunLightAmbientColorforMoveShap());
			m_pDisplayModel->MultiplyModelColor(dwColor);

			//if (m_bIsNeedAlphaFade && m_eAlphaFadeMode == CAlphaFadeObjectList::ALPHA_FADE_NONE)
			//	m_iAlphaFade = (int)((dwColor & 0xff000000) >> 24);
		}
		else
		{
			dwColor = 0xffffffff;
			m_pDisplayModel->SetModelColor(dwColor);

			//if (m_bIsNeedAlphaFade && m_eAlphaFadeMode == CAlphaFadeObjectList::ALPHA_FADE_NONE)
			//	m_iAlphaFade = (int)((dwColor & 0xff000000) >> 24);
		}

		// 处理模型渐变效果
		if (m_bIsNeedAlphaFade && m_eAlphaFadeMode == CAlphaFadeObjectList::ALPHA_FADE_IN)
		{
			long lElapseTime = timeGetTime() - m_lAlphaFadeInStartTime;
			if (lElapseTime > m_lAlphaFadeInTimeLength)
				m_eAlphaFadeMode = CAlphaFadeObjectList::ALPHA_FADE_NONE;

			m_pDisplayModel->MultiplyModelAlpha((float)lElapseTime / (float)m_lAlphaFadeInTimeLength);
		}

		m_pDisplayModel->ProcessAnimTexture(m_pAnimInfo);
		m_pDisplayModel->ProcessAnimUV(m_pAnimInfo);
		m_pDisplayModel->ProcessEnvironmentMaps(pCamera->GetViewMatrix());
		m_pDisplayModel->ProcessParticleEmitter(m_pAnimInfo);
		m_pDisplayModel->ProcessTextureProjective(m_pAnimInfo);
		m_pDisplayModel->ProcessTrailEmitter(m_pAnimInfo);
		m_pDisplayModel->ProcessLocator(m_pAnimInfo);

		/*m_pDisplayModel->MultiplyModelColor(GetModelColor());
		m_pDisplayModel->MultiplyModelAlpha( (float)(GetModelColor()>>24)/255.0f );*/
		m_pDisplayModel->ProcessAnimColor(m_pAnimInfo);
		m_pDisplayModel->ProcessVisibility(m_pAnimInfo);

		if (m_eAlphaFadeMode == CAlphaFadeObjectList::ALPHA_FADE_NONE)
			m_pDisplayModel->RenderModel();
		else
			m_pDisplayModel->RenderModelTransparent();
	}
	SkillDisplay();
	return true;
}



void CMoveShape::RendShadow(bool bsimpleshadow)
{
	D3DXVECTOR3 pos;
	pos.x = m_pAnimInfo->GetWorldMatrix()->_41;
	pos.y = GetHeight();
	pos.z = m_pAnimInfo->GetWorldMatrix()->_43;
	render::BoundingBox * pboundbox = m_pDisplayModel->GetSizeBox();
	float wid = (max(pboundbox->GetWidth(),pboundbox->GetLength()));
	float hei = pboundbox->GetHeight() * 2;
	wid *= m_pAnimInfo->GetScalMatrix()->_11;
	hei *= m_pAnimInfo->GetScalMatrix()->_22;
	float size = max(wid,hei) + 0.5f;
	if (size < 2.0f)
	{
		size = 2.0f;
	}
	/*if (size - (int)size < 0.5f)
	{
	size = (int)size == 0?1.0f:(int)size;
	}
	else
	{
	size = (int)(size + 1.5f);
	}*/
	if (m_pDisplayModel->GetRendShadow())
	{
		CTextureProjective::SetSimpleShadow(bsimpleshadow);
		CTextureProjective::BeginRendShadow(pos,(int)size);
		CTextureProjective::AddRendShadow(m_pDisplayModel,m_pAnimInfo);
		CTextureProjective::EndRendShadow();
	}

}

void CMoveShape::DisplayLayer2D(float pos)
{
	CGlobeSetup::tagSetup* pSetup = CGlobeSetup::GetSetup();

	// 对话
	if (m_strTalkText.size())
	{
		D3DCOLOR colorText = D3DCOLOR_ARGB(255,0,0,0);
		D3DCOLOR colorBack = pSetup->colorChatTextBack[CHAT_NORMAL];
		D3DCOLOR colorBackBar = pSetup->colorChatBackBar;

		BYTE rt = LOBYTE(HIWORD(colorText));
		BYTE gt = HIBYTE(LOWORD(colorText));
		BYTE bt = LOBYTE(LOWORD(colorText));
		BYTE rb = LOBYTE(HIWORD(colorBack));
		BYTE gb = HIBYTE(LOWORD(colorBack));
		BYTE bb = LOBYTE(LOWORD(colorBack));
		BYTE rbb = LOBYTE(HIWORD(colorBackBar));
		BYTE gbb = HIBYTE(LOWORD(colorBackBar));
		BYTE bbb = LOBYTE(LOWORD(colorBackBar));

		long lTalkLiveTimer;
		if (m_lTalkLiveTimer > 255)
		{
			lTalkLiveTimer = 255;
		}
		else
		{
			if(m_lTalkLiveTimer>100)
			{lTalkLiveTimer = (m_lTalkLiveTimer -= 5);}
			else
			{lTalkLiveTimer = 0;}
		}

		long lDistance = 0;
		if(this != GetGame()->GetMainPlayer())
			lDistance = GetGame()->GetMainPlayer()->Distance(this);
		// 超出10格说的话不显示
		if(lDistance <= 10)
		{
			DisplayChat(GetTalkText(),
				D3DCOLOR_ARGB(lTalkLiveTimer,rt,gt,bt),
				D3DCOLOR_ARGB(lTalkLiveTimer,rb,gb,bb),
				pos,
				D3DCOLOR_ARGB(lTalkLiveTimer/2,rbb,gbb,bbb));
		}
	}

	// 冒血
	for(list<tagBloodText>::iterator it=m_listBloodText.begin(); it!=m_listBloodText.end();)
	{
		tagBloodText *pst = &(*it);

		DWORD dwAlpha = pst->lAlpha << 24;
		D3DCOLOR color;
		D3DCOLOR colorBack;

		if(GetType() == TYPE_PLAYER)
		{
			color = (pSetup->colorBloodTextPlayer & 0x00ffffff) | dwAlpha;
			colorBack = (pSetup->colorBloodTextPlayerBack & 0x00ffffff) | dwAlpha;
		}
		else
		{
			color= (pSetup->colorBloodTextMonster & 0x00ffffff) | dwAlpha;
			colorBack= (pSetup->colorBloodTextMonsterBack & 0x00ffffff) | dwAlpha;
		}
		if(pst->bBlastAttack)
		{
			color= (pSetup->colorCriticalBloodText & 0x00ffffff) | dwAlpha;
			colorBack= (pSetup->colorCriticalBloodTextBack & 0x00ffffff) | dwAlpha;
		}
		// 疗伤
		if(pst->cType == AE_CURE)
		{
			color= (pSetup->colorAddBloodTextPlayer & 0x00ffffff) | dwAlpha;
			colorBack= (pSetup->colorAddBloodTextPlayerBack & 0x00ffffff) | dwAlpha;
		}
		switch(pst->cType)
		{
			// 减血伤害
		case AE_DAMAGE:
			DisplayDamage(pst->strText.c_str(), color, colorBack, pst->fPos, pst->fPosX, pst->fPosY, pst->fCurScale);
			break;
			// 吸收
		case AE_SUCK:
			DisplaySuck(pst->strText.c_str(), color, colorBack, pst->fPos, pst->fPosX, pst->fPosY, pst->fCurScale, pst->bBlastAttack);
			break;
			// 受伤
		case AE_BREAK:
			DisplayNumber(pst->strText.c_str(), color, colorBack, pst->fPos, pst->fPosX, pst->fPosY, pst->fCurScale, pst->fCurScale2, pst->bBlastAttack);
			break;
			// 格档
		case AE_BLOCK:
			DisplayBlock(pst->strText.c_str(), color, colorBack, pst->fPos, pst->fPosX, pst->fPosY, pst->fCurScale, pst->bBlastAttack);
			break;
			// 反弹
		case AE_REBOUND:
			DisplayRebound(pst->strText.c_str(), color, colorBack, pst->fPos, pst->fPosX, pst->fPosY, pst->fCurScale, pst->bBlastAttack);
			break;
			// 疗伤
		case AE_CURE:
			DisplayNumber(pst->strText.c_str(), color, colorBack, pst->fPos, pst->fPosX, pst->fPosY, pst->fCurScale, pst->fCurScale2, pst->bBlastAttack);
			break;
			// 闪避
		case AE_FULLMISS:
			DisplayMiss(color, colorBack, pst->fPos, pst->fPosX, pst->fPosY, pst->fCurScale, pst->bBlastAttack);
			break;
			// 招架
		case AE_PARRY:
			DisplayParry(color, colorBack, pst->fPos, pst->fPosX, pst->fPosY, pst->fCurScale, pst->bBlastAttack);
			break;
			// 抵抗
		case AE_RESIST:
			DisplayResist(color, colorBack, pst->fPos, pst->fPosX, pst->fPosY, pst->fCurScale, pst->bBlastAttack);
			break;
			// 丢失
		case AE_LOST:
			DisplayLost(color, colorBack, pst->fPos, pst->fPosX, pst->fPosY, pst->fCurScale, pst->bBlastAttack);
			break;
		default:

			break;
		}
		++it;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//	Shape行走处理
//////////////////////////////////////////////////////////////////////////////////////////////////

// 行走AI
long CMoveShape::MoveAI()
{
	// 播放玩家行走的声音
	if (GetType()==TYPE_PLAYER)
	{
		//int nPlayRunCount = (float)1/(GetSpeed()*15.0f);  
		//int nPlayWalkCount = (float)1/(GetSpeed()*25.0f); 
		DWORD nPlayRunCount = timeGetTime();  
		DWORD nPlayWalkCount = timeGetTime(); 
		//播放跑步音效
		if ((nPlayRunCount-m_lMoveSoundCount) >= 400&&GetAction()==ACT_RUN)
		{
			if(GetGame()->GetMainPlayer()==this||GetGame()->GetSetup()->bOthersFootstep)
			{
				GetGame()->GetAudioList()->Play(this,TYPE_REGION,GetAction(),Vector3(GetPosY(), GetHeight(), GetPosX()) );
			}
			m_lMoveSoundCount = timeGetTime();
		}
		//播放行走音效
		else if ((nPlayWalkCount - m_lMoveSoundCount) >= 450&&GetAction()==ACT_WALK)
		{
			if(GetGame()->GetMainPlayer()==this||GetGame()->GetSetup()->bOthersFootstep)
			{
				GetGame()->GetAudioList()->Play(this,TYPE_REGION,GetAction(),Vector3(GetPosY(), GetHeight(), GetPosX()) );
			}
			m_lMoveSoundCount = timeGetTime();
		}
		m_lMoveSoundCount++;
	}

	//// 计算移动的距离
	//float fDistance = GetSpeed()*static_cast<float>(m_dwMoveTimeCounter - m_dwLastMoveTimeCounter);

	////计算出坐标偏移
	//float fx = GetPosX() + fDistance * cosf(m_fDir);
	//float fy = GetPosY() + fDistance * sinf(m_fDir);	
	//
	//SetPosXY(fx, fy);
	//达到了目的点

	if( GetCurTickCount() >= GetEndMoveTime())
	{
		//到达格子
		OnArrive();
		if (m_lType == TYPE_PET && ((CPet*)this)->IsMainPlayerPet())
		{
			return	RET_AI_CONTINUE;
		}
		//纠正误差
		if(GetTileX() != (long)GetDestX() || GetTileY() != (long)GetDestY() )
			SetPosXY(GetDestX(), GetDestY());
	}

	return RET_AI_CONTINUE;
}

void CMoveShape::SetLastUpdatePosTime(DWORD dwTime)
{
	m_dwLastUpdatePosTime = dwTime;
}

// 到达目标点处理
long CMoveShape::OnArrive()
{
	SetIsArrive(true);

	if ( !IsInForceMoving() &&
		( this == GetGame()->GetMainPlayer() ||
		( m_lType == TYPE_PET && ((CPet*)this)->IsMainPlayerPet() ) ) )
		return RET_AI_CONTINUE;

	if(IsInForceMoving())
		SetInForceMove(false);

	SetAction( ACT_STAND );	
	return RET_AI_BREAK;
}

// 达到格子中心处理
long CMoveShape::OnArriveCell(long x, long y)
{
	return OnAttackEvent();
}



/*
* 功能: 处理攻击事件
* 摘要: -
* 参数: -
* 返回值: -
* 作者: -
* 创建日期: -
* 修改日志:
*	22009.02.04 - lpf
*		由于AE_BREAK事件有可能造成对象死亡,如果死亡后立即复活(在消息循环里复活的并改变了和平状态),就可能造成对象在消息循环里被设定和
*		平状态后,再次在AI这里被设定为战斗状态,而服务器的该对象已经是和平状态了,所以该对象在客户端的战斗状态就不会消失;现在注释掉战斗
*		状态在AI里的设定,将该战斗状态的设定交给服务器通过MSG_S2C_SHAPE_CHANGESTATE来处理;
*	22009.03.13 - lpf
*		在AE_BLOCK中的添加跳血文本时,增加了'-',以修正格挡时被攻击玩家数字会少一位;
*	22009.03.27 - lpf
*		在AE_LOST中的添加跳血文本时,判断如果是怪物也会添加跳血文本;
*/
long CMoveShape::OnAttackEvent()
{
	bool bAsyn = true;
	if(this == GetGame()->GetMainPlayer())
		bAsyn = false;
	while( !m_queueAttackEvent.empty() )
	{
		tagAttackEvent* p = &m_queueAttackEvent.front();
		switch(	p->cType )
		{
			// 减血伤害
		case AE_DAMAGE:
			{
				SetState(STATE_FIGHT);
				SetHp(p->lFinalHP);
				char str[32];
				// 头上显示字
				//如果当前的对象是和队友相关的
				CPlayer *pPlayer = GetGame()->GetMainPlayer();
				if(pPlayer->IsTeammate(p->Guidattracker) || 
					pPlayer->IsTeammate(this->GetExID()) ||
					this == pPlayer || p->Guidattracker == pPlayer->GetExID())
				{
					sprintf(str, "%d", p->lTotalLostPoint);
					AddBloodText(str,AE_DAMAGE, p->bBlastAttack,1.0f);	// 不考虑爆击情况
				}
				// 如果是主角的其他减少点数处理
				if(this == pPlayer)
				{
					for (int i = 0; i<(int)p->vtLost.size(); i++)
					{
						switch (p->vtLost[i].cLostType)
						{
						case DAMAGE_TYPE_HP:
							break;
						case DAMAGE_TYPE_MP:
							if ((long)(GetGame()->GetMainPlayer()->GetMp()) >= p->vtLost[i].lLostPoint)
								GetGame()->GetMainPlayer()->SetMp((WORD)((GetGame()->GetMainPlayer()->GetMp()) - p->vtLost[i].lLostPoint));
							break;
						}
					}
					//GetGame()->GetCGuiEx()->GetMainBarPageEx()->UpdatePlayerInforHar();

					// 攻击者不是自己
					if(p->Guidattracker != this->GetExID())
					{
						// 验证当前选中目标是否存在
						CGUID guid = CGUID::GUID_INVALID;
						long type = 0;
						//当前选中目标不为空
						if(GetGame()->GetGameControl()->GetSelectedShape())
						{
							guid = GetGame()->GetGameControl()->GetSelectedShape()->GetExID();
							type = GetGame()->GetGameControl()->GetSelectedShape()->GetType();
						}
						CMoveShape* pattracker = dynamic_cast<CMoveShape*>(GetGame()->GetRegion()->FindChildObject(type,guid));
						// 找不到当前选中的目标，设置攻击者为选中目标
						if(!pattracker)
						{
							// 攻击者为玩家
							pattracker = dynamic_cast<CPlayer*>(GetGame()->GetRegion()->FindChildObject(TYPE_PLAYER,p->Guidattracker));
							// 攻击者为monster
							if (!pattracker)
								pattracker = dynamic_cast<CMonster*>(GetGame()->GetRegion()->FindChildObject(TYPE_MONSTER,p->Guidattracker));
							// 攻击者为宠物
							//if (!pattracker)
							//	pattracker = dynamic_cast<CPet*>(GetGame()->GetRegion()->FindChildObject(TYPE_PET,p->Guidattracker));
							if(pattracker)
								GetGame()->GetGameControl()->SetSelectedShape(pattracker);
						}
					}
				}
				// 不是主角受伤
				else
				{	
					//CObjectPageEx *pObjPage = GetGame()->GetCGuiEx()->GetCObjectPageEx();
					//if(pObjPage)
					//{
					//	// 该目标shape是当前玩家选中的
					//	if(this==pObjPage->GetObjShape())
					//	{
					//		pObjPage->SetObjectHpInfo();
					//	}
					//}
                    if ( this == GetGame()->GetGameControl()->GetSelectedShape() )
                    {
                        FireUIEvent("ObjectPage", "ObjectAttr");
                    }
				}
				m_queueAttackEvent.pop();
			}
			break;
		case AE_SUCK:
			{
				SetState(STATE_FIGHT);
				if(GetAction()==ACT_STAND)
					SetAction(ACT_BLOCK);
				SetHp(p->lFinalHP);
				// 添加冒血数字
				//如果当前的对象是和队友相关的
				CPlayer *pPlayer = GetGame()->GetMainPlayer();
				if(pPlayer->IsTeammate(p->Guidattracker) || 
					pPlayer->IsTeammate(this->GetExID()) ||
					this == pPlayer || p->Guidattracker == pPlayer->GetExID())
				{
					char str[32];
					if(p->lTotalLostPoint == 0)
						sprintf(str,"");
					else
						sprintf(str, "%d", p->lTotalLostPoint);
					AddBloodText(str,AE_BLOCK, p->bBlastAttack,(p->bBlastAttack)?CGlobeSetup::GetSetup()->fCriticalTextScale:1.0f);
				}
				// 如果是主角的其他减少点数处理
				if (this == pPlayer)
				{
					for(int i=0; i<(int)p->vtLost.size(); i++)
					{
						switch (p->vtLost[i].cLostType)
						{
						case DAMAGE_TYPE_HP:
							break;
						case DAMAGE_TYPE_MP:
							if ((long)(GetGame()->GetMainPlayer()->GetMp()) >= p->vtLost[i].lLostPoint)
								GetGame()->GetMainPlayer()->SetMp((WORD)((GetGame()->GetMainPlayer()->GetMp())-p->vtLost[i].lLostPoint));
							//else
							;//GetGame()->GetMainPlayer()->SetMP(0);
							break;
						}
					}
					//GetGame()->GetCGuiEx()->GetMainBarPageEx()->UpdatePlayerInforHar();
					// 攻击者不是自己
					if(p->Guidattracker != this->GetExID())
					{
						// 验证当前选中目标是否存在
						CGUID guid = CGUID::GUID_INVALID;
						long type = 0;
						//当前选中目标不为空
						if(GetGame()->GetGameControl()->GetSelectedShape())
						{
							guid = GetGame()->GetGameControl()->GetSelectedShape()->GetExID();
							type = GetGame()->GetGameControl()->GetSelectedShape()->GetType();
						}
						CMoveShape* pattracker = dynamic_cast<CMoveShape*>(GetGame()->GetRegion()->FindChildObject(type,guid));
						// 找不到当前选中的目标，设置攻击者为选中目标
						if(!pattracker)
						{
							// 攻击者为玩家
							pattracker = dynamic_cast<CPlayer*>(GetGame()->GetRegion()->FindChildObject(TYPE_PLAYER,p->Guidattracker));
							// 攻击者为monster
							if (!pattracker)
								pattracker = dynamic_cast<CMonster*>(GetGame()->GetRegion()->FindChildObject(TYPE_MONSTER,p->Guidattracker));
							// 攻击者为宠物
							//if (!pattracker)
							//	pattracker = dynamic_cast<CPet*>(GetGame()->GetRegion()->FindChildObject(TYPE_PET,p->Guidattracker));
							if(pattracker)
								GetGame()->GetGameControl()->SetSelectedShape(pattracker);
						}
					}
				}
				// 不是主角受伤
				else
				{	
					//CObjectPageEx *pObjPage = GetGame()->GetCGuiEx()->GetCObjectPageEx();
					//if(pObjPage)
					//{
					//	// 该目标shape是当前玩家选中的
					//	if(this==pObjPage->GetObjShape())
					//	{
					//		pObjPage->SetObjectHpInfo();
					//	}
					//}
                    if ( this == GetGame()->GetGameControl()->GetSelectedShape() )
                    {
                        FireUIEvent("ObjectPage", "ObjectAttr");
                    }
				}
				m_queueAttackEvent.pop();
				// 吸收特效
				long lGraphicID(0);
				float x, y, height;
				if(GetLocatorPos(LOCATOR_WIND, x, y, height) && lGraphicID > 0)
					GetGame()->GetRegion()->PlayEffect(lGraphicID, x, y, height, p->fHurtedDir,bAsyn);
			}
			break;
			// 完全防御
		case AE_FULLMISS:
			{
				//SetState(STATE_FIGHT);
				if(GetAction()==ACT_STAND)
					SetAction(ACT_MISS);
				// 头上显示字
				// 如果当前的对象是和队友相关的
				CPlayer *pPlayer = GetGame()->GetMainPlayer();
				if(pPlayer->IsTeammate(p->Guidattracker) || 
					pPlayer->IsTeammate(this->GetExID()) ||
					this == pPlayer || p->Guidattracker == pPlayer->GetExID())
				{
					AddBloodText("闪避", AE_FULLMISS, p->bBlastAttack, (p->bBlastAttack)?CGlobeSetup::GetSetup()->fCriticalTextScale:1.0f);
				}
				if(this == pPlayer)
				{
					// 攻击者不是自己
					if(p->Guidattracker != this->GetExID())
					{
						// 验证当前选中目标是否存在
						CGUID guid = CGUID::GUID_INVALID;
						long type = 0;
						//当前选中目标不为空
						if(GetGame()->GetGameControl()->GetSelectedShape())
						{
							guid = GetGame()->GetGameControl()->GetSelectedShape()->GetExID();
							type = GetGame()->GetGameControl()->GetSelectedShape()->GetType();
						}
						CMoveShape* pattracker = dynamic_cast<CMoveShape*>(GetGame()->GetRegion()->FindChildObject(type,guid));
						// 找不到当前选中的目标，设置攻击者为选中目标
						if(!pattracker)
						{
							// 攻击者为玩家
							pattracker = dynamic_cast<CPlayer*>(GetGame()->GetRegion()->FindChildObject(TYPE_PLAYER,p->Guidattracker));
							// 攻击者为monster
							if (!pattracker)
								pattracker = dynamic_cast<CMonster*>(GetGame()->GetRegion()->FindChildObject(TYPE_MONSTER,p->Guidattracker));
							// 攻击者为宠物
							//if (!pattracker)
							//	pattracker = dynamic_cast<CPet*>(GetGame()->GetRegion()->FindChildObject(TYPE_PET,p->Guidattracker));
							if(pattracker)
								GetGame()->GetGameControl()->SetSelectedShape(pattracker);
						}
					}
				}
				m_queueAttackEvent.pop();
				long lGraphicID(0);
				float x, y, height;
				// 闪避特效
				if(GetLocatorPos(LOCATOR_WIND, x, y, height) && lGraphicID > 0)
					GetGame()->GetRegion()->PlayEffect(lGraphicID, x, y, height,-1.f,bAsyn);
				// 闪避声音
				GetGame()->GetAudioList()->Play(this,GetType(),ACT_MISS, Vector3(GetPosY(), GetHeight(), GetPosX()));
			}
			break;
			// 招架
		case AE_PARRY:
			{
				//SetState(STATE_FIGHT);
				if(GetAction()==ACT_STAND)
					SetAction(ACT_PARRY);
				// 头上显示字
				//如果当前的对象是和队友相关的
				CPlayer *pPlayer = GetGame()->GetMainPlayer();
				if(pPlayer->IsTeammate(p->Guidattracker) || 
					pPlayer->IsTeammate(this->GetExID()) ||
					this == pPlayer || p->Guidattracker == pPlayer->GetExID())
				{
					AddBloodText("招架", AE_PARRY, p->bBlastAttack,(p->bBlastAttack)?CGlobeSetup::GetSetup()->fCriticalTextScale:1.0f);
				}
				if(this == pPlayer)
				{
					// 攻击者不是自己
					if(p->Guidattracker != this->GetExID())
					{
						// 验证当前选中目标是否存在
						CGUID guid = CGUID::GUID_INVALID;
						long type = 0;
						//当前选中目标不为空
						if(GetGame()->GetGameControl()->GetSelectedShape())
						{
							guid = GetGame()->GetGameControl()->GetSelectedShape()->GetExID();
							type = GetGame()->GetGameControl()->GetSelectedShape()->GetType();
						}
						CMoveShape* pattracker = dynamic_cast<CMoveShape*>(GetGame()->GetRegion()->FindChildObject(type,guid));
						// 找不到当前选中的目标，设置攻击者为选中目标
						if(!pattracker)
						{
							// 攻击者为玩家
							pattracker = dynamic_cast<CPlayer*>(GetGame()->GetRegion()->FindChildObject(TYPE_PLAYER,p->Guidattracker));
							// 攻击者为monster
							if (!pattracker)
								pattracker = dynamic_cast<CMonster*>(GetGame()->GetRegion()->FindChildObject(TYPE_MONSTER,p->Guidattracker));
							// 攻击者为宠物
							//if (!pattracker)
							//	pattracker = dynamic_cast<CPet*>(GetGame()->GetRegion()->FindChildObject(TYPE_PET,p->Guidattracker));
							if(pattracker)
								GetGame()->GetGameControl()->SetSelectedShape(pattracker);
						}
					}
				}
				m_queueAttackEvent.pop();
				// 招架特效
				long lGraphicID(0);
				float x, y, height;
				if(GetLocatorPos(LOCATOR_WIND, x, y, height) && lGraphicID > 0)
					GetGame()->GetRegion()->PlayEffect(lGraphicID, x, y, height, p->fHurtedDir,bAsyn);
				// 招架声音
				GetGame()->GetAudioList()->Play(this,GetType(),ACT_PARRY, Vector3(GetPosY(), GetHeight(), GetPosX()));
			}
			break;
			// 格档
		case AE_BLOCK:
			{
				SetState(STATE_FIGHT);
				if(GetAction()==ACT_STAND)
					SetAction(ACT_BLOCK);
				DWORD dwOldHP = GetHp();
				SetHp(p->lFinalHP);
				// 添加冒血数字
				//如果当前的对象是和队友相关的
				CPlayer *pPlayer = GetGame()->GetMainPlayer();
				if(pPlayer->IsTeammate(p->Guidattracker) || 
					pPlayer->IsTeammate(this->GetExID()) ||
					this == pPlayer || p->Guidattracker == pPlayer->GetExID())
				{
					char str[32];
					sprintf(str, "-%d", p->lTotalLostPoint);
					AddBloodText(str,AE_BLOCK, p->bBlastAttack,(p->bBlastAttack)?CGlobeSetup::GetSetup()->fCriticalTextScale:1.0f);
				}
				// 如果是主角的其他减少点数处理
				if (this == pPlayer)
				{
					for(int i=0; i<(int)p->vtLost.size(); i++)
					{
						switch (p->vtLost[i].cLostType)
						{
						case DAMAGE_TYPE_HP:
							break;
						case DAMAGE_TYPE_MP:
							if ((long)(GetGame()->GetMainPlayer()->GetMp()) >= p->vtLost[i].lLostPoint)
								GetGame()->GetMainPlayer()->SetMp((WORD)((GetGame()->GetMainPlayer()->GetMp())-p->vtLost[i].lLostPoint));
							//else
							;//GetGame()->GetMainPlayer()->SetMP(0);
							break;
						}
					}
					//GetGame()->GetCGuiEx()->GetMainBarPageEx()->UpdatePlayerInforHar();
					// 攻击者不是自己
					if(p->Guidattracker != this->GetExID())
					{
						// 验证当前选中目标是否存在
						CGUID guid = CGUID::GUID_INVALID;
						long type = 0;
						//当前选中目标不为空
						if(GetGame()->GetGameControl()->GetSelectedShape())
						{
							guid = GetGame()->GetGameControl()->GetSelectedShape()->GetExID();
							type = GetGame()->GetGameControl()->GetSelectedShape()->GetType();
						}
						CMoveShape* pattracker = dynamic_cast<CMoveShape*>(GetGame()->GetRegion()->FindChildObject(type,guid));
						// 找不到当前选中的目标，设置攻击者为选中目标
						if(!pattracker)
						{
							// 攻击者为玩家
							pattracker = dynamic_cast<CPlayer*>(GetGame()->GetRegion()->FindChildObject(TYPE_PLAYER,p->Guidattracker));
							// 攻击者为monster
							if (!pattracker)
								pattracker = dynamic_cast<CMonster*>(GetGame()->GetRegion()->FindChildObject(TYPE_MONSTER,p->Guidattracker));
							// 攻击者为宠物
							//if (!pattracker)
							//	pattracker = dynamic_cast<CPet*>(GetGame()->GetRegion()->FindChildObject(TYPE_PET,p->Guidattracker));
							if(pattracker)
								GetGame()->GetGameControl()->SetSelectedShape(pattracker);
						}
					}
				}
				m_queueAttackEvent.pop();
				// 格档特效
				long lGraphicID(0);
				float x, y, height;
				if(GetLocatorPos(LOCATOR_WIND, x, y, height) && lGraphicID > 0)
					GetGame()->GetRegion()->PlayEffect(lGraphicID, x, y, height, p->fHurtedDir,bAsyn);
				// 格档声音
				GetGame()->GetAudioList()->Play(this,GetType(),ACT_BLOCK, Vector3(GetPosY(), GetHeight(), GetPosX()));
			}
			break;
			// 抵抗
		case AE_RESIST:
			{
				//SetState(STATE_FIGHT);
				// 头上显示字
				//如果当前的对象是和队友相关的
				CPlayer *pPlayer = GetGame()->GetMainPlayer();
				if(pPlayer->IsTeammate(p->Guidattracker) || 
					pPlayer->IsTeammate(this->GetExID()) ||
					this == pPlayer || p->Guidattracker == pPlayer->GetExID())
				{
					AddBloodText("抵抗",AE_RESIST, p->bBlastAttack,(p->bBlastAttack)?CGlobeSetup::GetSetup()->fCriticalTextScale:1.0f);
				}
				if(this == pPlayer)
				{
					// 攻击者不是自己
					if(p->Guidattracker != this->GetExID())
					{
						// 验证当前选中目标是否存在
						CGUID guid = CGUID::GUID_INVALID;
						long type = 0;
						//当前选中目标不为空
						if(GetGame()->GetGameControl()->GetSelectedShape())
						{
							guid = GetGame()->GetGameControl()->GetSelectedShape()->GetExID();
							type = GetGame()->GetGameControl()->GetSelectedShape()->GetType();
						}
						CMoveShape* pattracker = dynamic_cast<CMoveShape*>(GetGame()->GetRegion()->FindChildObject(type,guid));
						// 找不到当前选中的目标，设置攻击者为选中目标
						if(!pattracker)
						{
							// 攻击者为玩家
							pattracker = dynamic_cast<CPlayer*>(GetGame()->GetRegion()->FindChildObject(TYPE_PLAYER,p->Guidattracker));
							// 攻击者为monster
							if (!pattracker)
								pattracker = dynamic_cast<CMonster*>(GetGame()->GetRegion()->FindChildObject(TYPE_MONSTER,p->Guidattracker));
							// 攻击者为宠物
							//if (!pattracker)
							//	pattracker = dynamic_cast<CPet*>(GetGame()->GetRegion()->FindChildObject(TYPE_PET,p->Guidattracker));
							if(pattracker)
								GetGame()->GetGameControl()->SetSelectedShape(pattracker);
						}
					}
				}
				m_queueAttackEvent.pop();
				// 抵抗特效
				long lGraphicID(0);
				float x, y, height;
				if(GetLocatorPos(LOCATOR_WIND, x, y, height) && lGraphicID > 0)
					GetGame()->GetRegion()->PlayEffect(lGraphicID, x, y, height, p->fHurtedDir,bAsyn);
			}
			break;
			// 反弹
		case AE_REBOUND:
			{
				SetState(STATE_FIGHT);
				DWORD dwOldHP = GetHp();
				SetHp(p->lFinalHP);
				// 添加冒血数字
				//如果当前的对象是和队友相关的
				CPlayer *pPlayer = GetGame()->GetMainPlayer();
				if(pPlayer->IsTeammate(p->Guidattracker) || 
					pPlayer->IsTeammate(this->GetExID()) ||
					this == pPlayer || p->Guidattracker == pPlayer->GetExID())
				{
					char str[32];
					sprintf(str, "%d", p->lTotalLostPoint);
					AddBloodText(str,AE_REBOUND, p->bBlastAttack,(p->bBlastAttack)?CGlobeSetup::GetSetup()->fCriticalTextScale:1.0f);
				}
				// 如果是主角的其他减少点数处理
				if (this == pPlayer)
				{
					for(int i=0; i<(int)p->vtLost.size(); i++)
					{
						switch (p->vtLost[i].cLostType)
						{
						case DAMAGE_TYPE_HP:
							break;
						case DAMAGE_TYPE_MP:
							if ((long)(GetGame()->GetMainPlayer()->GetMp()) >= p->vtLost[i].lLostPoint)
								GetGame()->GetMainPlayer()->SetMp((WORD)((GetGame()->GetMainPlayer()->GetMp())-p->vtLost[i].lLostPoint));
							//else
							;//GetGame()->GetMainPlayer()->SetMP(0);
							break;
						}
					}
                    //GetGame()->GetCGuiEx()->GetMainBarPageEx()->UpdatePlayerInforHar();
					// 攻击者不是自己
					if(p->Guidattracker != this->GetExID())
					{
						// 验证当前选中目标是否存在
						CGUID guid = CGUID::GUID_INVALID;
						long type = 0;
						//当前选中目标不为空
						if(GetGame()->GetGameControl()->GetSelectedShape())
						{
							guid = GetGame()->GetGameControl()->GetSelectedShape()->GetExID();
							type = GetGame()->GetGameControl()->GetSelectedShape()->GetType();
						}
						CMoveShape* pattracker = dynamic_cast<CMoveShape*>(GetGame()->GetRegion()->FindChildObject(type,guid));
						// 找不到当前选中的目标，设置攻击者为选中目标
						if(!pattracker)
						{
							// 攻击者为玩家
							pattracker = dynamic_cast<CPlayer*>(GetGame()->GetRegion()->FindChildObject(TYPE_PLAYER,p->Guidattracker));
							// 攻击者为monster
							if (!pattracker)
								pattracker = dynamic_cast<CMonster*>(GetGame()->GetRegion()->FindChildObject(TYPE_MONSTER,p->Guidattracker));
							// 攻击者为宠物
							//if (!pattracker)
							//	pattracker = dynamic_cast<CPet*>(GetGame()->GetRegion()->FindChildObject(TYPE_PET,p->Guidattracker));
							if(pattracker)
								GetGame()->GetGameControl()->SetSelectedShape(pattracker);
						}
					}
				}
				m_queueAttackEvent.pop();
				// 反弹特效
				long lGraphicID(0);
				float x, y, height;
				if(GetLocatorPos(LOCATOR_WIND, x, y, height) && lGraphicID > 0)
					GetGame()->GetRegion()->PlayEffect(lGraphicID, x, y, height, p->fHurtedDir,bAsyn);
			}
			break;

			// 疗伤
		case AE_CURE:
			{
				// 怪物不设置疗伤动作
				//if(GetAction()==ACT_STAND && GetType() != TYPE_MONSTER)
				//	SetAction("疗伤动作");
				//如果当前的对象是和队友相关的，为他添加冒血数字
				CPlayer *pPlayer = GetGame()->GetMainPlayer();
				if(pPlayer->IsTeammate(p->Guidattracker) || 
					pPlayer->IsTeammate(this->GetExID()) ||
					this == pPlayer || p->Guidattracker == pPlayer->GetExID())
				{
					char str[32];
					sprintf(str,"+%d",p->lTotalLostPoint);
					AddBloodText(str, AE_CURE, p->bBlastAttack, (p->bBlastAttack)?CGlobeSetup::GetSetup()->fCriticalTextScale:1.0f);
				}
				// 该目标shape是当前玩家选中的
				/*CObjectPageEx *pObjPage = GetGame()->GetCGuiEx()->GetCObjectPageEx();
				if(pObjPage && this==pObjPage->GetObjShape())
				{
					pObjPage->SetObjectHpInfo();
				}*/
                if ( this == GetGame()->GetGameControl()->GetSelectedShape() )
                {
                    FireUIEvent("ObjectPage", "ObjectAttr");
                }
				m_queueAttackEvent.pop();
				// 加血特效
				long lGraphicID(0);
				float x, y, height;
				if(GetLocatorPos(LOCATOR_WIND, x, y, height) && lGraphicID > 0)
					GetGame()->GetRegion()->PlayEffect(lGraphicID, x, y, height, p->fHurtedDir,bAsyn);
			}
			break;

			// 受伤
		case AE_BREAK:
			{
				//SetState(STATE_FIGHT);

				// 怪物不设置受伤动作
				if(GetAction()==ACT_STAND && GetType() != TYPE_MONSTER)
					SetAction(ACT_BEATTACK);

				DWORD dwOldHP = GetHp();
				SetHp(p->lFinalHP);

				//如果当前的对象是和队友相关的，为他添加冒血数字
				CPlayer *pPlayer = GetGame()->GetMainPlayer();
				if(pPlayer->IsTeammate(p->Guidattracker) || 
					pPlayer->IsTeammate(this->GetExID()) ||
					this == pPlayer || p->Guidattracker == pPlayer->GetExID())
				{
					char str[32];
					sprintf(str,"-%d",p->lTotalLostPoint);
					AddBloodText(str, AE_BREAK, p->bBlastAttack, (p->bBlastAttack)?CGlobeSetup::GetSetup()->fCriticalTextScale:1.0f);
				}			

				// 如果是主角的其他减少点数处理
				if (this == pPlayer)
				{
					for(int i=0; i<(int)p->vtLost.size(); i++)
					{
						switch (p->vtLost[i].cLostType)
						{
						case DAMAGE_TYPE_HP:
							break;
						case DAMAGE_TYPE_MP:
							if ((long)(GetGame()->GetMainPlayer()->GetMp()) >= p->vtLost[i].lLostPoint)
								GetGame()->GetMainPlayer()->SetMp((WORD)((GetGame()->GetMainPlayer()->GetMp())-p->vtLost[i].lLostPoint));
							else
								;//GetGame()->GetMainPlayer()->SetMP(0);
							break;
						}
					}
					//GetGame()->GetCGuiEx()->GetMainBarPageEx()->UpdatePlayerInforHar();
					// 攻击者不是自己
					if(p->Guidattracker != this->GetExID())
					{
						// 验证当前选中目标是否存在
						CGUID guid = CGUID::GUID_INVALID;
						long type = 0;
						//当前选中目标不为空
						if(GetGame()->GetGameControl()->GetSelectedShape())
						{
							guid = GetGame()->GetGameControl()->GetSelectedShape()->GetExID();
							type = GetGame()->GetGameControl()->GetSelectedShape()->GetType();
						}
						CMoveShape* pattracker = dynamic_cast<CMoveShape*>(GetGame()->GetRegion()->FindChildObject(type,guid));
						// 找不到当前选中的目标，设置攻击者为选中目标
						if(!pattracker)
						{
							// 攻击者为玩家
							pattracker = dynamic_cast<CPlayer*>(GetGame()->GetRegion()->FindChildObject(TYPE_PLAYER,p->Guidattracker));
							// 攻击者为monster
							if (!pattracker)
								pattracker = dynamic_cast<CMonster*>(GetGame()->GetRegion()->FindChildObject(TYPE_MONSTER,p->Guidattracker));
							// 攻击者为宠物
							//if (!pattracker)
							//	pattracker = dynamic_cast<CPet*>(GetGame()->GetRegion()->FindChildObject(TYPE_PET,p->Guidattracker));
							if(pattracker)
								GetGame()->GetGameControl()->SetSelectedShape(pattracker);
						}
					}
				}
				// 不是主角受伤
				else
				{	
					//CObjectPageEx *pObjPage = GetGame()->GetCGuiEx()->GetCObjectPageEx();
					//if(pObjPage)
					//{
					//	// 该目标shape是当前玩家选中的
					//	if(this==pObjPage->GetObjShape())
					//	{
					//		pObjPage->SetObjectHpInfo();
					//	}
					//}
                    if ( this == GetGame()->GetGameControl()->GetSelectedShape() )
                    {
                        FireUIEvent("ObjectPage", "ObjectAttr");
                    }
				}

				m_queueAttackEvent.pop();

				long lGraphiscID(1);
				long lSoundID(1);
				/***********************************************************************/
				/* zhaohang fix */
				/***********************************************************************/
				CSkillListXml::tagSkill *pSkill = CSkillListXml::GetProperty(p->dwSkillID);
				if(pSkill)
				{
					bool  bFlashEffect		  = false;
					DWORD dwFlashEffectLum    = 0;
					DWORD dwFlashEffectTime   = 0;
					bool  bShakeEffect		  = false;
					DWORD dwShakeEffectExtent = 0;
					DWORD dwShakeEffectTime   = 0;
					bool  bHaveEnergy         = false;

					DWORD dwlev = pSkill->vectorLevels.size() >= p->bySkillLevel? p->bySkillLevel-1:((DWORD)pSkill->vectorLevels.size() - 1);
					CSkillListXml::tagLevel pLevel = pSkill->vectorLevels[dwlev];
					vector<CSkillListXml::tagLevelValue>::iterator pos;
					for(pos = pLevel.vecStepEnd.begin(); pos!= pLevel.vecStepEnd.end(); pos++)
					{
						// 播放命中受伤特效
						if(!strcmp((*pos).strKeyWord.c_str(),"HitEffect"))
						{
							CPlayer* pattracker = (CPlayer*)GetGame()->GetRegion()->FindChildObject(TYPE_PLAYER,p->Guidattracker);
							if (pattracker)
							{
								DWORD energyeffectid = pattracker->GetEnergyHitEffectID();
								if (energyeffectid && pSkill->dwSkillUseType == 1)
								{
									lGraphiscID = energyeffectid;
									bHaveEnergy = true;
								}
								else
								{
									lGraphiscID = (*pos).dwValue;
								}
							}
							else
							{
								lGraphiscID = (*pos).dwValue;
							}
							//float x, y, height;
							AddEffectLocator(lGraphiscID,this,LOCATOR_WIND,false,p->fHurtedDir,0,bAsyn);
							//if (GetLocatorPos(LOCATOR_WIND, x, y, height))
							//	GetGame()->GetRegion()->PlayEffect(lGraphiscID, x, y, height, p->fHurtedDir);
						}
						// 是否具有闪光效果
						else if (!strcmp((*pos).strKeyWord.c_str(), "FlashEffect"))
						{
							bFlashEffect = true;
						}
						// 闪光亮度
						else if (!strcmp((*pos).strKeyWord.c_str(), "FlashEffectLum"))
						{
							dwFlashEffectLum = (*pos).dwValue;
						}
						// 闪光持续时间
						else if (!strcmp((*pos).strKeyWord.c_str(), "FlashEffectTime"))
						{
							dwFlashEffectTime = (*pos).dwValue;
						}
						// 是否具有震动效果
						else if (!strcmp((*pos).strKeyWord.c_str(), "ShakeEffect"))
						{
							bShakeEffect = true;
						}
						// 播放角色震动效果
						else if (!strcmp((*pos).strKeyWord.c_str(), "ShakeEffectExtent"))
						{
							dwShakeEffectExtent = (*pos).dwValue;
						}
						// 播放角色震动效果
						else if (!strcmp((*pos).strKeyWord.c_str(), "ShakeEffectTime"))
						{
							dwShakeEffectTime = (*pos).dwValue;
						}
						// 播放除了物理技能以外的其他技能命中声音特效，如火球爆裂声
						else if(!strcmp((*pos).strKeyWord.c_str(),"HitSound"))
						{
							if(bHaveEnergy==false)
							{
								lSoundID = (*pos).dwValue;
								char strSound[32];
								sprintf(strSound,"sounds/skills/%d.wav",lSoundID);
								GetGame()->GetAudioList()->Play(strSound, GetPosX(),GetPosY(),GetHeight());
							}else{
								char strSound[32];
								lSoundID = 33;
								sprintf(strSound,"sounds/interfaces/i-%d.wav",lSoundID);
								GetGame()->GetAudioList()->Play(strSound, GetPosX(),GetPosY(),GetHeight());
							}
						}
					}

					// 设定附加闪光特效
					if (GetGame()->GetSetup()->bRoleFlash && bFlashEffect)
						SetupFlashEffect(dwFlashEffectLum, dwFlashEffectTime);

					// 设定附加震动特效
					if (GetGame()->GetSetup()->bRoleShake && bShakeEffect)
						SetupShakeEffect(dwShakeEffectExtent, dwShakeEffectTime);
				}

				// 根据受伤的程度播放相应的音效
				float fMildDamage=0;
				float fHighDamage=0;
				// 玩家不分职业和性别按统一的比例计算伤害程度
				if ( GetType() == TYPE_PLAYER )
				{
					fMildDamage = CMoveShape::mildhurt_knight;
					fHighDamage = CMoveShape::highhurt_knight;
				}

				//计算怪物是小伤害还是大伤害
				if ( GetType() == TYPE_MONSTER )
				{
					int iMonsterType = dynamic_cast<CMonster*>(this)->GetClass();
					switch ( iMonsterType )
					{
					case CMonster::MK_BOSS:
						{
							fMildDamage = CMoveShape::mildhurt_boss;
							fHighDamage = CMoveShape::highhurt_boss;
						}
						break;

					default :
						{
							fMildDamage = CMoveShape::mildhurt_monster;
							fHighDamage = CMoveShape::highhurt_monster;
						}
						break;
					}
				}

				// 盔甲发声
				if ( GetType() == TYPE_PLAYER && dynamic_cast<CPlayer*>(this)->GetEquip(CPlayer::EQUIP_BODY) )
				{
					CPlayer * pPlayer = dynamic_cast<CPlayer*>(this);					
					if ( p->lTotalLostPoint < (float)pPlayer->GetMaxHp()*fMildDamage )
					{
						// 弱击中盔甲发声
						GetGame()->GetAudioList()->Play(pPlayer->GetEquip(CPlayer::EQUIP_BODY),pPlayer->GetEquip(CPlayer::EQUIP_BODY)->GetType(),CGoods::GOOD_NORMAL, Vector3(pPlayer->GetPosY(), pPlayer->GetHeight(), pPlayer->GetPosX()));
					}
					else if ( p->lTotalLostPoint > (float)pPlayer->GetMaxHp()*fHighDamage )
					{
						// 重击中盔甲发声
						GetGame()->GetAudioList()->Play(pPlayer->GetEquip(CPlayer::EQUIP_BODY),pPlayer->GetEquip(CPlayer::EQUIP_BODY)->GetType(),CGoods::GOOD_EXCEPT, Vector3(pPlayer->GetPosY(), pPlayer->GetHeight(), pPlayer->GetPosX()));
						// 惨叫
						GetGame()->GetAudioList()->Play(this,GetType(),ACT_BEATTACK, Vector3(GetPosY(), GetHeight(), GetPosX()));
					}
					else
					{
						// 重击中盔甲发声
						GetGame()->GetAudioList()->Play(pPlayer->GetEquip(CPlayer::EQUIP_BODY),pPlayer->GetEquip(CPlayer::EQUIP_BODY)->GetType(),CGoods::GOOD_EXCEPT, Vector3(pPlayer->GetPosY(), pPlayer->GetHeight(), pPlayer->GetPosX()));
					}
				}
				else // 怪物 或 人（无装备）
				{
					enum
					{
						ACT_HURT01 = ACT_BEATTACK,			// 普通
						ACT_HURT02  = 12,					// 被重击		
					};

					CMoveShape * pMoveShape = dynamic_cast<CMoveShape*>(this);

					if ( p->lTotalLostPoint <= (float)pMoveShape->GetMaxHp()*fMildDamage )
					{
						// 弱伤害
						GetGame()->GetAudioList()->Play(this,GetType(),ACT_HURT01, Vector3(GetPosY(), GetHeight(), GetPosX()));
					}
					else if ( p->lTotalLostPoint > (float)pMoveShape->GetMaxHp()*fHighDamage )	// 强伤害
					{
						// 重击伤害
						GetGame()->GetAudioList()->Play(this,GetType(),ACT_HURT02, Vector3(GetPosY(), GetHeight(), GetPosX()));
						// 惨叫
						GetGame()->GetAudioList()->Play(this,GetType(),ACT_BEATTACK, Vector3(GetPosY(), GetHeight(), GetPosX()));
					}
					else	// 重击伤害
					{
						GetGame()->GetAudioList()->Play(this,GetType(),ACT_HURT01, Vector3(GetPosY(), GetHeight(), GetPosX()));
					}
				}

				return RET_AI_BREAK;
			}
			break;

			// 死亡
		case AE_DIED:
			{
				SetState(STATE_DIED);
				SetAction( ACT_DIED );

				//// 冒血
				//if(p->eDeathType==1)
				//{
				//	char str[32];
				//	sprintf(str,"%d",p->lTotalLostPoint);
				//	AddBloodText(str, AE_BREAK, p->bBlastAttack, (p->bBlastAttack)?CGlobeSetup::GetSetup()->fCriticalTextScale:1.0f);
				//}

				SetHp(0);
				// 更新选中目标信息
				//CObjectPageEx *pObjPage = GetGame()->GetCGuiEx()->GetCObjectPageEx();
				//// 该目标shape是当前玩家选中的
				//if(pObjPage && this==pObjPage->GetObjShape())
				//{
				//	pObjPage->SetObjectHpInfo();
				//}
                if ( this == GetGame()->GetGameControl()->GetSelectedShape() )
                {
                    FireUIEvent("ObjectPage", "ObjectAttr");
                }

				// 自己死亡了
				if( this == GetGame()->GetMainPlayer() )
				{
					/*CGoods* pHandGoods = GetGame()->GetCGuiEx()->GetPMainPlayerHand()->GetPGoodsOfMainPlayerHand();
					if (pHandGoods!=NULL)
					{
						pHandGoods->SetHaveShadowState(false);
					}
					GetGame()->GetCGuiEx()->GetPMainPlayerHand()->ClearMainPlayerHand();
					GetGame()->GetCGuiEx()->GetMainBarPageEx()->UpdatePlayerInforHar();*/
                    FireUIEvent("MainRole", "RoleAttr");
					// 清除缓慢补充队列
					GetGame()->GetMainPlayer()->m_listGrowHP.clear();
					GetGame()->GetMainPlayer()->m_listGrowMP.clear();
					// 停止移动和攻击
					GetGame()->GetGameControl()->AutoAttactStop();
					GetGame()->GetGameControl()->AutoMoveStop();
				}

				// 显示连击数
				if (p->lHitNum!=-1)
				{
					GetGame()->GetMainPlayer()->SetHitNum(p->lHitNum);
					GetGame()->GetMainPlayer()->SetLastHitTimer(GetCurTickCount());
				}

				// 清除异常状态
				//ClearStateEx();

				while(!m_queueAttackEvent.empty())
					m_queueAttackEvent.pop();

				GetGame()->GetAudioList()->Play(this,GetType(),ACT_DIED, Vector3(GetPosY(), GetHeight(), GetPosX()));
				return RET_AI_BREAK;
			}
			break;
			// 丢失
		case AE_LOST:
			{
				// 添加丢失
				// 头上显示字
				//如果当前的对象是和队友相关的
				CPlayer *pPlayer = GetGame()->GetMainPlayer();
				if(pPlayer->IsTeammate(p->Guidattracker) || 
					pPlayer->IsTeammate(this->GetExID()) ||
					this == pPlayer || p->Guidattracker == pPlayer->GetExID() ||
					this->GetType() == TYPE_MONSTER)
				{
					AddBloodText("丢失", AE_LOST, p->bBlastAttack, (p->bBlastAttack)?CGlobeSetup::GetSetup()->fCriticalTextScale:1.0f);
				}
				m_queueAttackEvent.pop();
				long lGraphicID(0);
				float x, y, height;
				// 闪避特效
				if(GetLocatorPos(LOCATOR_WIND, x, y, height) && lGraphicID > 0)
					GetGame()->GetRegion()->PlayEffect(lGraphicID, x, y, height,-1.f,bAsyn);
				// 闪避声音
				char* strSound="sounds/xxx.wav";
				GetGame()->GetAudioList()->Play(strSound,GetPosX(),GetPosY(),GetHeight());
			}
			break;
			// 改变方向
		case AE_CHANGEDIR:
			{
				if(GetAction() == ACT_STAND || GetAction() == ACT_IDLE)
				{
					long lDir = (long)p->fHurtedDir;
					SetDir((CShape::eDIR)lDir);
					m_queueAttackEvent.pop();
				}
				else if(m_queueAttackEvent.size() == 1)
				{
					return RET_AI_CONTINUE;
				}
				else
				{
					m_queueAttackEvent.push(m_queueAttackEvent.front());
					m_queueAttackEvent.pop();
				}
			}
			break;
		}
		//if(!m_queueAttackEvent.empty())
		//	m_queueAttackEvent.pop();
	}
	return RET_AI_CONTINUE;
}

// 获取目标方向
long CMoveShape::GetDestDir(long sx, long sy, long dx, long dy)
{
	long x = sx - dx;
	long y = sy - dy;

	// 向左
	if( x > 0 )
	{
		if( y > 0 )
		{
			return DIR_LEFTUP;
		}
		else if( y < 0 )
		{
			return DIR_LEFTDOWN;
		}
		else
		{
			return DIR_LEFT;
		}
	}
	else if( x < 0 )	// 向右
	{
		if( y > 0 )
		{
			return DIR_RIGHTUP;
		}
		else if( y < 0 )
		{
			return DIR_RIGHTDOWN;
		}
		else
		{
			return DIR_RIGHT;
		}
	}
	else	// 纵向
	{
		if( y > 0 )
		{
			return DIR_UP;
		}
		else
		{
			return DIR_DOWN;
		}
	}

	return 0;
}



/*
* 功能: 增加异常状态
* 摘要: -
* 参数: -
* 返回值: -
* 作者: -
* 创建日期: -
* 修改日志:
*	2008.10.20 - lpf
*		增加了异常状态的颜色缓冲在此初始化.
*/
void CMoveShape::AddStateEx(DWORD dwID, DWORD dwLevel, DWORD dwStandingTime, DWORD dwAdditionalData,BYTE bFlag)
{
	// 如果更新的Shape是当前选择的对象，添加状态信息
	/*CObjectPageEx *pObjPage = GetGame()->GetCGuiEx()->GetCObjectPageEx();
	if(pObjPage && (pObjPage->GetObjShape()==this))
	{
		pObjPage->AddObjState(dwID,dwLevel);
	}*/
	CStateListXml::tagStateList *pSL = CStateListXml::GetState(dwID);
	if(pSL)
	{
		DWORD dwEffect = 0;
		DWORD dwEffectIndex = 0;
		DWORD dwLocator = 0;
		bool bIsOrderPlay = false;
		if(dwLevel >= (DWORD)pSL->vecStateLevel.size())
			dwLevel = (DWORD)pSL->vecStateLevel.size();
		vector<CStateListXml::tagStateEffect> vecStateEffect = pSL->vecStateLevel[dwLevel-1].vecMidEffect;
		// 删除中间阶段特效
		for (size_t i=0; i<vecStateEffect.size();++i)
		{
			// 获得添加状态的指定等级的特效信息
			dwEffect		= vecStateEffect[i].dwEffectID;
			dwEffectIndex	= vecStateEffect[i].dwEffectIndex;
			bIsOrderPlay	= vecStateEffect[i].bIsOrderPlay;
			// 从本对象身上删除所有的特效
			if(!bIsOrderPlay)
				DelEffect(dwEffect,dwEffectIndex);
		}
		DelAllEffectByOrder();
		// 删除结束阶段特效
		vecStateEffect = pSL->vecStateLevel[dwLevel-1].vecEndEffect;
		for (size_t i=0; i<vecStateEffect.size();++i)
		{
			// 获得添加状态的指定等级的特效信息
			dwEffect		= vecStateEffect[i].dwEffectID;
			dwEffectIndex	= vecStateEffect[i].dwEffectIndex;
			// 从本对象身上删除所有的特效
			DelEffect(dwEffect,dwEffectIndex);
		}
		// 添加开始阶段特效
		vecStateEffect = pSL->vecStateLevel[dwLevel-1].vecBeginEffect;
		for (size_t i=0; i<vecStateEffect.size();++i)
		{
			// 获得添加状态的指定等级的特效信息
			dwEffect		= vecStateEffect[i].dwEffectID;
			dwEffectIndex	= vecStateEffect[i].dwEffectIndex;
			dwLocator		= vecStateEffect[i].dwEffectPosition;		//施放位置
			// 从本对象身上删除所有的特效
			DelEffect(dwEffect,dwEffectIndex);
			eLOCATOR Loc;
			switch(dwLocator)
			{
			case 0: Loc = LOCATOR_WIND; break;
			case 1: Loc = LOCATOR_HEAD; break;
			case 2: Loc = LOCATOR_FOOTCENTER; break;
			case 3: Loc = LOCATOR_LEFTHAND; break;
			case 4: Loc = LOCATOR_RIGHTHAND; break;
			case 5: Loc = LOCATOR_LEFTFOOT; break;
			case 6: Loc = LOCATOR_RIGHTFOOT; break;
			default: Loc = LOCATOR_FOOTCENTER; break;
			}
			// 为本对象添加该状态开始阶段的特效
			if(dwEffect)
				AddEffectLocator(dwEffect, this, Loc, false, 0.0f, dwEffectIndex);
		}

		// 该状态有对应的动作
		if(pSL->wAction > 0)
			SetAction(pSL->wAction);

		BOOL bStateColorPlay = (BOOL)pSL->vecStateLevel[(dwLevel) - 1].vecBuffColor.size();
		if (bStateColorPlay)
		{
			m_CurrentStateExForStateColor.dwID = dwID;
			m_CurrentStateExForStateColor.dwLevel = dwLevel;
			m_CurrentStateExForStateColor.dwStartTime = GetCurTickCount();
			m_CurrentStateExForStateColor.dwStandingTime = dwStandingTime;
			m_CurrentStateExForStateColor.bEndSelf = false;
			m_bStateColorPlay = TRUE;
			m_bColorFade	  = pSL->vecStateLevel[(dwLevel) - 1].bColorFade;					//是否异常状态颜色动画渐变
		}
	}

	// 本人添加状态
	if(this == GetGame()->GetMainPlayer())
	{
		// 如果该状态会让玩家停止移动
		if(pSL && pSL->bStopMove)
		{
			GetGame()->GetGameControl()->AutoAttactStop();
			GetGame()->GetGameControl()->AutoMoveStop();
		}
	}

	list<tagStateEx>::iterator it;
	for(it = m_listStateEx.begin(); it != m_listStateEx.end(); it++)
	{
		tagStateEx* pStateEx = &(*it);
		if (pStateEx && pStateEx->dwID==dwID && pStateEx->bEndSelf==false)
		{
			pStateEx->dwStartTime = GetCurTickCount();
			pStateEx->dwStandingTime = dwStandingTime;
			pStateEx->dwAdditionalData = dwAdditionalData;
			pStateEx->dwLevel = dwLevel;
			if(bFlag == 0 && pStateEx->lCount >= 0)
				pStateEx->lCount++;
			return;
		}
	}

	tagStateEx st;
	st.dwID = dwID;
	st.dwLevel = dwLevel;
	st.dwAdditionalData = dwAdditionalData;
	st.dwStartTime = GetCurTickCount();
	st.dwStandingTime =dwStandingTime;
	st.bEndSelf = false;
	st.lCount = 1;
	m_listStateEx.push_back(st);
}

void CMoveShape::RemoveStateEx(DWORD dwID,DWORD dwLevel)
{
	if(dwID==STATE_RIDE_HORSE)
	{
		CPlayer *pPlayer = (CPlayer *)this;
		pPlayer->GetHorse()->SetVisible(FALSE);
		g_bForceUpdate=true;
		SetAction(GetAction());
		return;
	}
	list<tagStateEx>::iterator it;
	for(it = m_listStateEx.begin(); it != m_listStateEx.end(); it++)
	{
		tagStateEx* pStateEx = &(*it);
		if (pStateEx && pStateEx->dwID==dwID && pStateEx->dwLevel == dwLevel)
		{
			pStateEx->lCount--;
			if (pStateEx->lCount<=0)
			{
				pStateEx->bEndSelf = true;
				AI_StateEx();
				// 当前的动作处于该状态对应的动作，在状态结束时，设置动作为站立
				CStateListXml::tagStateList *pSL = CStateListXml::GetState(dwID);
				if(pSL)
				{
					if(pSL->wAction == GetAction())
						SetAction(ACT_STAND);
				}
				// 如果更新的Shape是当前选择的对象，删除状态信息
				/*CObjectPageEx *pObjPage = GetGame()->GetCGuiEx()->GetCObjectPageEx();
				if(pObjPage && (pObjPage->GetObjShape()==this))
					pObjPage->RemoveObjState(dwID);*/
			}
			return;
		}
	}
}

// 清除所有异常状态
void CMoveShape::ClearStateEx()
{
	int x = GetGame()->GetMainPlayer()->GetTileX();
	int y = GetGame()->GetMainPlayer()->GetTileY();
	CRegion::tagCell *pCell = GetGame()->GetRegion()->GetCell(x, y);
	// 如果更新的Shape是当前选择的对象，删除状态信息
	/*CObjectPageEx *pObjPage = GetGame()->GetCGuiEx()->GetCObjectPageEx();
	if(pObjPage && (pObjPage->GetObjShape()==this))
	{
		pObjPage->ClearState();
	}*/
	list<tagStateEx>::iterator it;
	for(it = m_listStateEx.begin(); it != m_listStateEx.end(); it++)
	{
		tagStateEx* pStateEx = &(*it);
		if (pStateEx)
		{
			CStateListXml::tagStateList *pSL = CStateListXml::GetState(pStateEx->dwID);
			if (pSL && pSL->vecStateLevel[pStateEx->dwLevel-1].AfettleDeathIsClear && pCell->bSecurity == CRegion::SECURITY_FREE)
			{
				pStateEx->lCount = 0;
				pStateEx->bEndSelf = true;
			}
		}
	}
}

void CMoveShape::GetCurGraphicTypeID(long &lType,long &lID)
{
	if(m_lChangeGraphicsID==0)
		return CBaseObject::GetCurGraphicTypeID(lType,lID);
	else
	{
		lType = m_lChangeGraphicstype;
		lID = m_lChangeGraphicsID;
	}

}
// 解码
bool CMoveShape::DecordFromByteArray(BYTE* pByte, long& pos, bool bExData)
{
	CShape::DecordFromByteArray(pByte, pos, bExData);
	float fSpeedChangedRatio = _GetFloatFromByteArray(pByte, pos);
	SetActionSpeedRate(ACT_WALK,fSpeedChangedRatio);
	SetActionSpeedRate(ACT_RUN,fSpeedChangedRatio);

	m_bRun			        = (_GetByteFromByteArray(pByte, pos) == 0)?false : true;
	m_bAntiInvisibleable	= (_GetByteFromByteArray(pByte, pos) == 0)?false : true;
	m_bInvisible			= (_GetByteFromByteArray(pByte, pos) == 0)?false : true;
	m_bPretendDead			= (_GetByteFromByteArray(pByte, pos) == 0)?false : true;

	m_bUseRabbSkill			= (_GetByteFromByteArray(pByte, pos) == 0)?false : true;
	m_bUseFighterSkill		= (_GetByteFromByteArray(pByte, pos) == 0)?false : true;
	m_bUseItem				= (_GetByteFromByteArray(pByte, pos) == 0)?false : true;
	m_bInstruction			= (_GetByteFromByteArray(pByte, pos) == 0)?false : true;
	m_bUnAttack				= (_GetByteFromByteArray(pByte, pos) == 0)?false : true;
	m_bAddHp				= (_GetByteFromByteArray(pByte, pos) == 0)?false : true;
	m_bHide					= (_GetByteFromByteArray(pByte, pos) == 0)?false : true;
	m_bForceAttact			= (_GetByteFromByteArray(pByte, pos) == 0)?false : true;

	m_dwCollectID		= _GetDwordFromByteArray(pByte, pos);
	if (m_dwCollectID!=0)
	{
		const CCollectionLayout* CollectLayout = CDeputyOccuSystemClient::GetCollectionLayout(m_dwCollectID);
		if (CollectLayout!=NULL)
		{
			m_strCollectionName = CollectLayout->GetCollectionName();
		}
	}
	m_dwCollectTimes        = _GetDwordFromByteArray(pByte, pos);
	m_bIsBeenCollect        = (_GetByteFromByteArray(pByte, pos) == 0)?false : true;
	m_bCanBeenCollect		= (_GetByteFromByteArray(pByte, pos) == 0)?false : true;
	m_bNotRide					= (_GetByteFromByteArray(pByte, pos) == 0)?false : true;
	m_lChangeGraphicstype	= _GetLongFromByteArray(pByte, pos);
	m_lChangeGraphicsID		= _GetLongFromByteArray(pByte, pos);
	BYTE bDied				= _GetByteFromByteArray(pByte, pos);
	if (bDied)
	{
		// 设置死亡状态
		CShape::SetState(STATE_DIED);
		CShape::SetAction(ACT_ALREADYDIED);
	}
	if(!bExData)
	{
		//解压buff
		DecordBuffFromByteArray(pByte, pos);
		//解压debuff
		DecordDeBuffFromByteArray(pByte, pos);
	}
	//解压主动技能
	DecordActSkillFromByteArray(pByte, pos);
	//解压开关技能
	if(bExData)
		DecordOnOffSkillFromByteArray(pByte, pos);
	//设置移动目标点
	SetDestXY(GetPosX(),GetPosY());
	return true;
}

//解压buff
bool CMoveShape::DecordBuffFromByteArray(BYTE* pByte, long& pos)
{
	DWORD dwSize = _GetDwordFromByteArray(pByte, pos);
	for(DWORD i =0;i<dwSize;i++)
	{
		long lID =  _GetLongFromByteArray(pByte, pos);
		long lLvl = _GetLongFromByteArray(pByte, pos);
		DWORD dwRemianTime = _GetDwordFromByteArray(pByte, pos);
		AddStateEx(lID, lLvl, dwRemianTime);
	}
	return true;
}

//解压debuff
bool CMoveShape::DecordDeBuffFromByteArray(BYTE* pByte, long& pos)
{
	DWORD dwSize = _GetDwordFromByteArray(pByte, pos);
	for(DWORD i =0;i<dwSize;i++)
	{
		long lID =  _GetLongFromByteArray(pByte, pos);
		long lLvl = _GetLongFromByteArray(pByte, pos);
		DWORD dwRemianTime = _GetDwordFromByteArray(pByte, pos);
		AddStateEx(lID, lLvl, dwRemianTime);
	}
	return true;
}
//解压onoffskill
bool CMoveShape::DecordOnOffSkillFromByteArray(BYTE* pByte, long& pos)
{
	DWORD dwSize = _GetDwordFromByteArray(pByte, pos);
	for(DWORD i =0;i<dwSize;i++)
	{
		long lID =  _GetLongFromByteArray(pByte, pos);
		long lLvl = _GetLongFromByteArray(pByte, pos);
	}
	return true;
}
//解压主动使用技能
bool CMoveShape::DecordActSkillFromByteArray(BYTE* pByte, long& pos)
{
	bool bFlag = _GetByteFromByteArray(pByte, pos)==0?false:true;
	if(!bFlag)	return false;
	int num = _GetLongFromByteArray(pByte, pos);	// 正在使用的技能数
	for (int i = 0 ; i< num; i++)
	{
		tagDecordSkillList skillList;
		skillList.lSkillKey = _GetLongFromByteArray(pByte, pos);
		skillList.lSkillID =  _GetLongFromByteArray(pByte, pos);
		skillList.lSkillLvl = _GetLongFromByteArray(pByte, pos);
		//当前执行的阶段序号
		skillList.lProceNum = _GetLongFromByteArray(pByte, pos);

		skillList.lMouseX = _GetLongFromByteArray(pByte, pos);
		skillList.lMouseY = _GetLongFromByteArray(pByte, pos);

		DWORD dwDestObjNum = _GetDwordFromByteArray(pByte, pos);
		CGUID guid;
		for(DWORD i=0;i<dwDestObjNum;i++)
		{
			skillList.vecType.push_back( _GetLongFromByteArray(pByte, pos));
			_GetBufferFromByteArray(pByte, pos,guid);
			skillList.vecID.push_back(guid);
		}
		m_vecDecordSkill.push_back(skillList);
	}

	return true;
}

bool CMoveShape::DecodeFromDataBlock(DBReadSet& setReadDB, bool bExData)
{
	CShape::DecodeFromDataBlock(setReadDB, bExData);
	float fSpeedChangedRatio = setReadDB.GetFloatFromByteArray();
	SetActionSpeedRate(ACT_WALK,fSpeedChangedRatio);
	SetActionSpeedRate(ACT_RUN,fSpeedChangedRatio);
	m_bRun			= (setReadDB.GetByteFromByteArray() == 0)?false : true;
	m_bHide					= (setReadDB.GetByteFromByteArray() == 0)?false : true;
	m_bInvisible			= (setReadDB.GetByteFromByteArray() == 0)?false : true;
	m_bPretendDead			= (setReadDB.GetByteFromByteArray() == 0)?false : true;
	m_bNotRide				= (setReadDB.GetByteFromByteArray() == 0)?false : true;
	short m_sBeenAttack     = setReadDB.GetShortFromByteArray();
	setReadDB.GetBufferFromByteArray(m_PhaseID);                /// 增加相位的编码

	if(bExData)
	{
		m_bAntiInvisibleable	= (setReadDB.GetByteFromByteArray() == 0)?false : true;	
		m_bUseRabbSkill			= (setReadDB.GetByteFromByteArray() == 0)?false : true;
		m_bUseFighterSkill		= (setReadDB.GetByteFromByteArray() == 0)?false : true;
		m_bInstruction			= (setReadDB.GetByteFromByteArray() == 0)?false : true;
		m_bForceAttact			= (setReadDB.GetByteFromByteArray() == 0)?false : true;

		short m_sMoveable             = setReadDB.GetShortFromByteArray();
		short m_sRandomMove           = setReadDB.GetShortFromByteArray();
		short m_sSelfControl          = setReadDB.GetShortFromByteArray();
		short m_sMoveDamage           = setReadDB.GetShortFromByteArray();
		short m_sUseSkill             = setReadDB.GetShortFromByteArray();
	}

	m_lChangeGraphicstype	= setReadDB.GetLongFromByteArray();
	m_lChangeGraphicsID		= setReadDB.GetLongFromByteArray();
	BYTE bDied				= setReadDB.GetByteFromByteArray();
	if (bDied)
	{
		// 设置死亡状态
		CShape::SetState(STATE_DIED);
		CShape::SetAction(ACT_ALREADYDIED);
	}
	if(!bExData)
	{
		//解压buff
		DecodeBuffFromDataBlock(setReadDB);
		//解压debuff
		DecodeDeBuffFromDataBlock(setReadDB);
	}
	//解压主动技能
	//DecodeActSkillFromDataBlock(setReadDB);
	//解压开关技能
	if(bExData)
		DecodeOnOffSkillFromDataBlock(setReadDB);
	//设置移动目标点
	SetDestXY(GetPosX(),GetPosY());
	return true;
}
//解压buff
bool CMoveShape::DecodeBuffFromDataBlock(DBReadSet& setReadDB)
{
	DWORD dwSize = setReadDB.GetDwordFromByteArray();
	for(DWORD i =0;i<dwSize;i++)
	{
		long lID =  setReadDB.GetLongFromByteArray();
		long lLvl = setReadDB.GetLongFromByteArray();
		DWORD dwRemianTime = setReadDB.GetDwordFromByteArray();
		AddStateEx(lID, lLvl, dwRemianTime);
	}
	return true;
}
//解压debuff
bool CMoveShape::DecodeDeBuffFromDataBlock(DBReadSet& setReadDB)
{
	DWORD dwSize = setReadDB.GetDwordFromByteArray();
	for(DWORD i =0;i<dwSize;i++)
	{
		long lID =  setReadDB.GetLongFromByteArray();
		long lLvl = setReadDB.GetLongFromByteArray();
		DWORD dwRemianTime = setReadDB.GetDwordFromByteArray();
		AddStateEx(lID, lLvl, dwRemianTime);
	}
	return true;
}
//解压onoffskill
bool CMoveShape::DecodeOnOffSkillFromDataBlock(DBReadSet& setReadDB)
{
	DWORD dwSize = setReadDB.GetDwordFromByteArray();
	for(DWORD i =0;i<dwSize;i++)
	{
		long lID =  setReadDB.GetLongFromByteArray();
		long lLvl = setReadDB.GetLongFromByteArray();
	}
	return true;
}
//解压主动使用技能
bool CMoveShape::DecodeActSkillFromDataBlock(DBReadSet& setReadDB)
{
	bool bFlag = setReadDB.GetByteFromByteArray()==0?false:true;
	if(!bFlag)	return false;
	int num = setReadDB.GetLongFromByteArray();	// 正在使用的技能数
	for (int i = 0 ; i< num; i++)
	{
		tagDecordSkillList skillList;
		skillList.lSkillKey = setReadDB.GetLongFromByteArray();
		skillList.lSkillID =  setReadDB.GetLongFromByteArray();
		skillList.lSkillLvl = setReadDB.GetLongFromByteArray();
		//当前执行的阶段序号
		skillList.lProceNum = setReadDB.GetLongFromByteArray();

		skillList.lMouseX = setReadDB.GetLongFromByteArray();
		skillList.lMouseY = setReadDB.GetLongFromByteArray();

		DWORD dwDestObjNum = setReadDB.GetDwordFromByteArray();
		CGUID guid;
		for(DWORD i=0;i<dwDestObjNum;i++)
		{
			skillList.vecType.push_back( setReadDB.GetLongFromByteArray());
			setReadDB.GetBufferFromByteArray(guid);
			skillList.vecID.push_back(guid);
		}
		m_vecDecordSkill.push_back(skillList);
	}

	return true;
}

/*
* 功能: 设定角色闪光附加效果
* 摘要: 该成员函数负责启动一个角色的闪光效果,并设定其需要的状态值
* 参数: dwLum		  - 闪光的亮度值0~100
*		 dwTimeLength - 闪光持续时间(单位:毫秒)
* 返回值: -
* 作者: lpf
* 创建日期: 2008.01.10
*/
void CMoveShape::SetupFlashEffect(DWORD dwLum, DWORD dwTimeLength)
{
	m_listAddEffect[ADDEFFECT_FLASH].bPlayState   = true;
	m_listAddEffect[ADDEFFECT_FLASH].dwStartTime  = timeGetTime();
	m_listAddEffect[ADDEFFECT_FLASH].dwTimeLength = dwTimeLength;

	if (dwLum > 100)
		m_listAddEffect[ADDEFFECT_FLASH].dwValue = 100;
	else
		m_listAddEffect[ADDEFFECT_FLASH].dwValue = dwLum;
}



/*
* 功能: 设定角色震动附加效果
* 摘要: 该成员函数负责启动一个角色的震动效果,并设定其需要的状态值
* 参数: dwExtent	  - 震动幅度值(传入的整形值最终会被缩小100倍,变成浮点值进行实际应用)
*		 dwTimeLength - 震动持续时间(单位:毫秒)
* 返回值: -
* 作者: lpf
* 创建日期: 2008.01.10
*/
void CMoveShape::SetupShakeEffect(DWORD dwExtent, DWORD dwTimeLength)
{
	m_listAddEffect[ADDEFFECT_SHAKE].bPlayState   = true;
	m_listAddEffect[ADDEFFECT_SHAKE].dwStartTime  = timeGetTime();
	m_listAddEffect[ADDEFFECT_SHAKE].dwTimeLength = dwTimeLength;
	m_listAddEffect[ADDEFFECT_SHAKE].dwValue	  = dwExtent;

	m_fShakeMax = (float)dwExtent * 0.005f;			//(0.005f = 0.01f * 0.5f)
	m_fShakeMin = (float)dwExtent * 0.005f;

	srand((unsigned int)timeGetTime());
}



// 使用当前技能
void CMoveShape::UseCurSkill()
{
	if(m_vecDecordSkill.size()>0)
	{
		vector<tagDecordSkillList>::iterator it;
		for (it = m_vecDecordSkill.begin(); it != m_vecDecordSkill.end(); it ++)
		{
			if ((*it).lProceNum == 1)   // 吟唱阶段
			{
				/***********************************************************************/
				/* 赵航 fix */
				//CSkillXml *m_pSkill = new CSkillXml();
				/***********************************************************************/
				ConcreteSkillComp* SkillCom = new ConcreteSkillComp();
				ConcreteDecSkillXml* pSkill = new ConcreteDecSkillXml(SkillCom);
				/***********************************************************************/
				/* zhaohang fix */
				/***********************************************************************/
				CSkillListXml::tagSkill *pTagSkill = CSkillListXml::GetProperty((*it).lSkillID);
				if (pTagSkill && pTagSkill->dwSkillType == CSkillListXml::SKILLTYPE_PLAY)	// 直接施放
				{
					// 直接从开始阶段执行
					if(pSkill->StepBegin((*it).lSkillID, (*it).lSkillLvl, GetType(), GetExID(), (*it).lMouseX, (*it).lMouseY,GetDirEx(),(*it).vecType, (*it).vecID))
					{
						pSkill->SetSkillKey((*it).lSkillKey);
						AddCurSkillList((*it).lSkillKey, pSkill);
					}
					else
						SAFE_DELETE(pSkill);
					// 如果开始阶段成功就直接执行施放阶段
					if(pSkill&&pSkill->StepRun((*it).lSkillID, (*it).lSkillLvl, GetType(), GetExID(), (*it).lMouseX, (*it).lMouseY,GetDirEx(),(*it).vecType, (*it).vecID) == false)
					{
						//pSkill->SetIsDeleted(true);
						DeleteCurSkill((*it).lSkillKey);
					}
				}
				/***********************************************************************/
				/* zhaohang fix */
				/***********************************************************************/
				else if(pTagSkill && pTagSkill->dwSkillType == CSkillListXml::SKILLTYPE_INTONATE) // 吟唱施放
				{
					if(pSkill->StepBegin((*it).lSkillID, (*it).lSkillLvl, GetType(), GetExID(), (*it).lMouseX, (*it).lMouseY,GetDirEx(),(*it).vecType, (*it).vecID))
					{
						pSkill->SetSkillKey((*it).lSkillKey);
						AddCurSkillList((*it).lSkillKey, pSkill);
					}
					else
						SAFE_DELETE(pSkill);
					if(GetType() == TYPE_PLAYER && GetGame()->GetMainPlayer()->GetExID() == GetExID())
					{
						// 吟唱阶段允许玩家移动取消技能
						GetGame()->GetGameControl()->SetControlAble(true);
						GetGame()->GetGameControl()->SetMoveAble(true);
					}
				}
			}
			else					// 施放阶段
			{
				/***********************************************************************/
				/* 赵航 fix */
				//CSkillXml *m_pSkill = new CSkillXml();
				/***********************************************************************/
				ConcreteSkillComp* SkillCom = new ConcreteSkillComp();
				ConcreteDecSkillXml* pSkill = new ConcreteDecSkillXml(SkillCom);
				if(pSkill&&pSkill->StepRun((*it).lSkillID, (*it).lSkillLvl, GetType(), GetExID(), (*it).lMouseX, (*it).lMouseY,GetDirEx(),(*it).vecType, (*it).vecID))
				{
					pSkill->SetSkillKey((*it).lSkillKey);
					AddCurSkillList((*it).lSkillKey, pSkill);
				}
				else
				{
					SAFE_DELETE(pSkill);
				}
				if(GetType() == TYPE_PLAYER && GetGame()->GetMainPlayer()->GetExID() == GetExID())
				{
					GetGame()->GetGameControl()->SetControlAble(true);
				}
			}
		}
	}
}
// 求两个Shape间的距离（单位：格子）
long CMoveShape::Distance(CShape* pShape)
{
	float x1, y1, x2, y2;
	if (GetAction()==ACT_WALK || GetAction()==ACT_RUN)
	{
		x1 = GetDestX();
		y1 = GetDestY();
	}
	else
	{
		x1 = GetPosX();
		y1 = GetPosY();
	}

	CMoveShape* pMoveShape = dynamic_cast<CMoveShape*>(pShape);
	if (pMoveShape)
	{
		if (pMoveShape->GetAction()==ACT_WALK || pMoveShape->GetAction()==ACT_RUN)
		{
			x2 = pMoveShape->GetDestX();
			y2 = pMoveShape->GetDestY();
		}
		else
		{
			x2 = pMoveShape->GetPosX();
			y2 = pMoveShape->GetPosY();
		}
	}

	return (long)max( abs(x1 - x2) - GetFigure(DIR_RIGHT) - pShape->GetFigure(DIR_RIGHT),
		abs(y1 - y2) - GetFigure(DIR_UP) - pShape->GetFigure(DIR_UP) );
}

// 是否死亡
bool CMoveShape::IsDied()
{
	if (GetState()==STATE_DIED)
	{
		return true;
	}
	return false;
}



/*
* 功能: 增加跳血文本
* 摘要: -
* 参数: -
* 返回值: -
* 作者: -
* 创建日期: -
* 修改日志:
*	2008.12.04 - lpf
*		在添加字符串内容时,判断了如果是主玩家就显示减号,否则不显示减号.
*/
void CMoveShape::AddBloodText(const char* str,char cType, bool bBlastAttack, float fScale)
{
	tagBloodText st;
	st.strText = str;

	if ((unsigned char)str[0] <= 128)
	{
		if (GetGame()->GetMainPlayer() != (CPlayer *)this)
			st.strText.erase(0, 1);
	}

	st.fPosX = GetPosX(); 
	st.fPosY = GetPosY(); 
	st.fPos=2.0f; 
	st.lAlpha = 128;
	st.cType = cType;
	st.bBlastAttack = bBlastAttack;
	st.fScale = fScale;
	st.fCurScale = 0.5f;
	st.dwStartTime = timeGetTime();

	m_listBloodText.push_back(st);
}



// 取得技能列表中某一个正在使用的技能
CSkillXml* CMoveShape::GetCurSkill(long lSkillKey)
{
	return m_mapCurSkill.find(lSkillKey)!=m_mapCurSkill.end()?m_mapCurSkill[lSkillKey]:NULL;
}
// 删除技能列表中某一个技能
void CMoveShape::DeleteCurSkill(long lSkillKey)
{
	CSkillXml* pSkill = m_mapCurSkill.find(lSkillKey)!= m_mapCurSkill.end()? m_mapCurSkill[lSkillKey]:NULL;
	if(pSkill)
	{
		m_mapCurSkill.erase(lSkillKey);
		SAFE_DELETE(pSkill);
	}
}
// 删除技能列表
void CMoveShape::DeleteCurSkillList()
{
	if (m_mapCurSkill.size()>0)
	{
		for (map<long,CSkillXml*>::iterator it= m_mapCurSkill.begin(); it!= m_mapCurSkill.end(); it++)
		{
			CSkillXml* pSkill = (CSkillXml*)it->second;
			if(pSkill)
				SAFE_DELETE(pSkill);
		}
		m_mapCurSkill.clear();
	}
	if(m_vecDecordSkill.size()>0)
		m_vecDecordSkill.clear();
}

// 察看是否存在指定ID的技能
bool CMoveShape::IsHaveSkillByIndex(DWORD dwID)
{
	for (map<long,CSkillXml*>::iterator it= m_mapCurSkill.begin(); it!= m_mapCurSkill.end(); it++)
	{
		CSkillXml* pSkill = (CSkillXml*)it->second;
		if(pSkill && pSkill->GetSkillID() == dwID)
		{
			return true;
		}
	}
	return false;
}

// 显示技能
void CMoveShape::SkillDisplay()
{
	if(m_mapCurSkill.size()>0)
	{
		map<long, CSkillXml*>::iterator it;
		for (it = m_mapCurSkill.begin(); it != m_mapCurSkill.end(); it++)
		{
			it->second->Display();
		}
	}
}


// 即时更新绑定点位置
void CMoveShape::UpdataGetLocator()
{
	if(GetDisplayModel() && GetAnimInfo())
	{
		GetDisplayModel()->ProcessAnimJoint(GetAnimInfo());
		GetDisplayModel()->ProcessLocator(GetAnimInfo());
	}
}

//得到属性值接口
bool  CMoveShape::SetProperty(const char* attrName, long value)
{
	if(NULL == attrName)	return false;
	mapNameValueItr itr = m_mapNameValue.find(attrName);
	if(itr == m_mapNameValue.end())	return false;

	switch (itr->second.lType)
	{
	case 0:	// byte
		*(itr->second.plValue) = (BYTE)value;
		break;
	case 1:	// word
		*(itr->second.pwValue) = (WORD)value;
		break;
	case 2:	// dword
		*(itr->second.pdwValue) = (DWORD)value;
		break;
	case 3:	// float
		*(itr->second.pfValue) = (float)value;
		break;
	case 4:	// long
		*(itr->second.pValue) = (long)value;
		break;
	case 5:	// short
		*(itr->second.psValue) = (short)value;
		break;
	case 6:	// bool
		*(itr->second.pbValue) = value==0?false:true;
		break;
	}

	OnPropertyChanged(attrName);
	return true;
}

//当属性值改变以后出发
bool  CMoveShape::OnPropertyChanged(const char* attrName)
{
	if(NULL == attrName)	return false;

	if( strncmp(attrName,"CollectionID",12) == 0)
	{
		const CCollectionLayout* CollectLayout = CDeputyOccuSystemClient::GetCollectionLayout(m_dwCollectID);
		if (CollectLayout!=NULL)
		{
			m_strCollectionName = CollectLayout->GetCollectionName();
		}
	}
	return true;
}

//设置属性值接口
long  CMoveShape::GetProperty(const char* attrName)
{	
	if(NULL == attrName)	return 0;
	mapNameValueItr itr = m_mapNameValue.find(attrName);
	if(itr == m_mapNameValue.end())	return 0;

	switch (itr->second.lType)
	{
	case 0:	// byte
		return *(itr->second.plValue);

	case 1:	// word
		return *(itr->second.pwValue);

	case 2:	// dword
		return *(itr->second.pdwValue);

	case 3:	// float
		return (long)*(itr->second.pfValue);

	case 4:	// long
		return *(itr->second.pValue);

	case 5:	// short
		return *(itr->second.psValue);

	case 6:	// bool
		return *(itr->second.pbValue);
	}
	return 0;
}

//得到属性值的字符串表示
const char* CMoveShape::GetPropertyStringValue(const char* pszPropertyName)
{
	static char pszString[1024]="";
	strncpy(pszString,"",1024);	
	if( NULL != pszPropertyName)
	{
		//名字
		if( strncmp(pszPropertyName,"Name",4) == 0 )
			return GetName();
		else
		{
			mapNameValueItr itr = m_mapNameValue.find(pszPropertyName);
			if(itr != m_mapNameValue.end())
			{
				switch (itr->second.lType)
				{
				case 0:	// byte
					_snprintf(pszString,1024,"%d", *(itr->second.plValue));
					break;
				case 1:	// word
					_snprintf(pszString,1024,"%d", *(itr->second.pwValue));
					break;
				case 2:	// dword
					_snprintf(pszString,1024,"%u", *(itr->second.pdwValue));
					break;
				case 3:	// float
					_snprintf(pszString,1024,"%f", *(itr->second.pfValue));
					break;
				case 4:	// long
					_snprintf(pszString,1024,"%f", *(itr->second.pValue));
					break;
				case 5:	// short
					_snprintf(pszString,1024,"%f", *(itr->second.psValue));
					break;
				case 6:	// bool
					if( *(itr->second.pbValue) == false)
						_snprintf(pszString,1024,"假");
					else
						_snprintf(pszString,1024,"真");
					break;
				}
			}
		}
	}
	return pszString;
}


void CMoveShape::MoveTo( float xDest, float yDest )
{
	// 寻路
	m_FindPath.SetRegion( GetGame()->GetRegion() );
	m_FindPath.Find( (int)this->GetPosX(), (int)this->GetPosY(), (int)xDest, (int)yDest );
}


//根据对象属性格式化字符串
//用通配符"{}"来格式化字符串 ,属性名字包含在{}中
//例如:"玩家{Name}退出游戏."
void CMoveShape::FormatStringWithProperty(const char* pszIn,char *pszOut,int nOutSize)
{
	if(NULL == pszIn || NULL == pszOut || nOutSize <= 0)	return;
	strncpy(pszOut,"",nOutSize);
	int InPos = 0;
	int OutPos = 0;
	char pszName[256] = "";
	int	nNamePos = 0;
	bool bGetName = false;
	//总长度
	int len = (int)strnlen(pszIn,102400);
	while(InPos < len &&  OutPos < nOutSize)
	{
		char c = pszIn[InPos++];
		if(c == '{')
		{
			nNamePos = 0;
			bGetName = true;
		}
		else if(c == '}')
		{
			bGetName = false;
			pszName[nNamePos++] ='\0';
			//得到值的字符
			const char* pszValueString = GetPropertyStringValue(pszName);
			if(pszValueString)
			{
				int nValueLen = (int)strnlen(pszValueString,1024);
				if(OutPos+nValueLen>=nOutSize)
					break;
				strcat(pszOut,pszValueString);
				OutPos += nValueLen;
			}
		}
		else
		{
			if(bGetName)
				pszName[nNamePos++] =c;
			else
				pszOut[OutPos++] = c;
		}
	}
	if(OutPos == nOutSize)	OutPos--;
	//判断宽字符
	if(OutPos >= 2 && (BYTE)pszOut[OutPos-1] >= 128 && (BYTE)pszOut[OutPos-2] < 128)
		OutPos--;
	pszOut[OutPos] = '\0';
}



/*
* 功能: 设定播放表情动画所需要的信息
* 摘要: 调用该函数来初始化播放表情所需要的状态信息
*		 如果没有表情文件句柄则不处理,否则都要处理
*		 表情动画的结束是有自身完成
* 参数: pFaceAction - 仪态动作指针
* 返回值: -
* 作者: lpf
* 创建日期: 2008.04.10
* 修改日志:
*	2008.06.05 - lpf
*		增加了起始帧的赋值
*	2008.12.24 - lpf
*		增加了对帧间隔时间配置的非零检查,防止不正确的配置导致的当机
*/
void CMoveShape::SetupFaceAnim(tagFaceAction * pFaceAction)
{
	if (pFaceAction->dwFaceFileHandle != 0 && pFaceAction->dwFaceInterval != 0)
	{
		m_stCurrentFaceAnim.dwFaceCount		  = pFaceAction->dwFaceCount;
		m_stCurrentFaceAnim.dwFaceStartIndex  = pFaceAction->dwFaceStartIndex;
		m_stCurrentFaceAnim.dwFaceEndIndex    = pFaceAction->dwFaceEndIndex;
		m_stCurrentFaceAnim.dwFaceInterval    = pFaceAction->dwFaceInterval;
		m_stCurrentFaceAnim.dwFaceFileHandle  = pFaceAction->dwFaceFileHandle;

		m_stCurrentFaceAnim.bPlayState		   = true;
		m_stCurrentFaceAnim.dwFaceLoopCount	   = 0;
		m_stCurrentFaceAnim.dwFaceCurrentIndex = 0;
		m_stCurrentFaceAnim.dwStartTime		   = timeGetTime();
	}
}



/*
* 功能: 设定播放动作动画所需要的信息
* 摘要: 调用该函数来初始化播放动作所需要的状态信息
*		 当玩家或怪物执行一个有“动作“的表情动作时，如果玩家不是和平状态则不能播放动作。
*		 当玩家正播放一个表情动作中的动作时，播放一个“站立”（包括和平和战斗）或“ＩＤＬＥ”以外的动作都会打断现在的表情动作。
* 参数: pFaceAction - 仪态动作指针
* 返回值: -
* 作者: lpf
* 创建日期: 2008.04.10
* 修改日志:
*	2008.11.11 - lpf
*		增加了新的对动作列表的处理
*	2008.12.26 - lpf
*		增加了动作信息中为当前动作是否循环播放的判断,目的在于解决目前客户端一直存在的动作间插值不对的问题
*/
void CMoveShape::SetupActionAnim(tagFaceAction * pFaceAction)
{
	if (GetState() != CShape::STATE_PEACE ||
		pFaceAction->iActionIndex == -1 ||
		pFaceAction->sActionFileName == "0"/* ||
										   GetType() != TYPE_PLAYER*/)
										   return;

	tagActionList * pActionList = GetGame()->GetFaceActionList()->GetActionList(pFaceAction->iActionIndex);

	SetAction((WORD)atoi(pActionList->m_vecActionDecList[0].sAction.c_str()));
	//GetAnimInfo()->m_tActionInfo.bCurActionLooped = true;
	m_iActionCount = (int)pActionList->m_vecActionDecList[0].iLoopCount;
	if (m_iActionCount == -1 || m_iActionCount > 1)
		GetAnimInfo()->m_tActionInfo.bCurActionLooped = TRUE;
	else
		GetAnimInfo()->m_tActionInfo.bCurActionLooped = FALSE;

	m_stCurrentActionAnim.iActionIndex = pFaceAction->iActionIndex;
	m_stCurrentActionAnim.bActionPlayState = true;
	m_stCurrentActionAnim.bEffectPlayState = false;
	m_stCurrentActionAnim.dwActionCurrentIndex = 0;
	m_stCurrentActionAnim.dwStartTime = timeGetTime();
}



/*
* 功能: 播放表情动画
* 摘要: -
* 参数: -
* 返回值: -
* 作者: lpf
* 创建日期: 2008.04.10
* 修改日志:
*	2008.05.21 - lpf
*		修改了表情动画图片的处理宽高尺寸由原来的64像素变为32像素
*	2008.06.05 - lpf
*		修改了目前的图片资源播放规则,增加了起始帧概念
*		由于现在的表情文件纹理尺寸为512x512(单帧的像素尺寸不变),所以最大的纹理索引编号为255
*/
void CMoveShape::PlayFaceAnim()
{
	if (!m_stCurrentFaceAnim.bPlayState)
		return;

	// 头顶坐标
	D3DXMATRIX  matFinal;
	D3DXVECTOR3 pos(GetPosY(), GetPickBoxHeight() + GetHeight(), GetPosX());
	D3DXVECTOR3 pos2;

	GetGame()->GetRegion()->CalculateFinalTransformMatrix(matFinal);
	D3DXVec3TransformCoord(&pos2, &pos, &matFinal);

	if (m_stCurrentFaceAnim.dwFaceLoopCount < m_stCurrentFaceAnim.dwFaceCount)
	{
		RECT  rc;
		DWORD dwCurrentIndex;
		DWORD dwCurrentTime = timeGetTime();
		DWORD dwElapseTime	= dwCurrentTime - m_stCurrentFaceAnim.dwStartTime;

		dwCurrentIndex = dwElapseTime / m_stCurrentFaceAnim.dwFaceInterval + m_stCurrentFaceAnim.dwFaceStartIndex;
		if (dwCurrentIndex > m_stCurrentFaceAnim.dwFaceEndIndex ||
			dwCurrentIndex > 255)
		{
			m_stCurrentFaceAnim.dwStartTime = dwCurrentTime;
			++m_stCurrentFaceAnim.dwFaceLoopCount;
		}else
		{
			rc.left	  = (dwCurrentIndex % 16) * 32;
			rc.top	  = (dwCurrentIndex / 16) * 32;
			rc.right  = rc.left + 32;
			rc.bottom = rc.top + 32;
			GameGraphics2D::GetInstance()->DrawImageSubRect((int)pos2.x - 16, (int)pos2.y - 70, &rc, m_stCurrentFaceAnim.dwFaceFileHandle);
		}
	}else
		m_stCurrentFaceAnim.bPlayState = false;
}



/*
* 功能: 播放动作动画
* 摘要: -
* 参数: -
* 返回值: -
* 作者: lpf
* 创建日期: 2008.11.05
* 修改日志:
*	2008.11.17 - lpf
*		对配置的特效ID为0的情况进行了处理
*	2008.12.26 - lpf
*		增加了动作信息中为当前动作是否循环播放的判断,目的在于解决目前客户端一直存在的动作间插值不对的问题
*/
void CMoveShape::PlayActionAnim()
{
	if (!m_stCurrentActionAnim.bActionPlayState)
	{
		if (m_stCurrentActionAnim.bEffectPlayState)
		{
			// 处理特效取消循环标志,在下次逻辑中会自动删除该对象
			CEffect * pEffect = dynamic_cast<CEffect *>(FindEffectByIndex(m_stCurrentActionAnim.guidEffect));
			if (pEffect)
			{
				m_stCurrentActionAnim.guidEffect = CGUID::GUID_INVALID;
				pEffect->SetIsDeleted(true);
			}

			m_stCurrentActionAnim.bEffectPlayState = false;
		}
		return;
	}

	if (!m_stCurrentActionAnim.bEffectPlayState)
	{
		tagActionList * pActionList = GetGame()->GetFaceActionList()->GetActionList(m_stCurrentActionAnim.iActionIndex);
		if (timeGetTime() - m_stCurrentActionAnim.dwStartTime > pActionList->dwStartTime)
		{
			if (pActionList->dwIdEffect != 0)
			{
				m_stCurrentActionAnim.guidEffect = AddEffectLocator(pActionList->dwIdEffect, this, (eLOCATOR)pActionList->eLocator, pActionList->bIsLoop);
				m_stCurrentActionAnim.bEffectPlayState = true;
			}
		}
	}

	if (m_bDisplayFinish)
	{
		tagActionList * pActionList = GetGame()->GetFaceActionList()->GetActionList(m_stCurrentActionAnim.iActionIndex);
		m_stCurrentActionAnim.dwActionCurrentIndex++;

		if (m_stCurrentActionAnim.dwActionCurrentIndex < pActionList->m_vecActionDecList.size())
		{
			const char * pszAction = pActionList->m_vecActionDecList[m_stCurrentActionAnim.dwActionCurrentIndex].sAction.c_str();

			SetAction((WORD)atoi(pszAction));
			//GetAnimInfo()->m_tActionInfo.bCurActionLooped = true;
			m_iActionCount = (int)pActionList->m_vecActionDecList[m_stCurrentActionAnim.dwActionCurrentIndex].iLoopCount;
			if (m_iActionCount == -1 || m_iActionCount > 1)
				GetAnimInfo()->m_tActionInfo.bCurActionLooped = TRUE;
			else
				GetAnimInfo()->m_tActionInfo.bCurActionLooped = FALSE;
		}else
			m_stCurrentActionAnim.bActionPlayState = false;
	}else
	{
		if (GetAction() < CShape::ACT_FACEACTION || GetAction() > CShape::ACT_FACEACTIONMAX)
			m_stCurrentActionAnim.bActionPlayState = false;
	}
}

//播放表情动作对应音效
void CMoveShape::PlayActionSound()
{
	if(GetType()!=TYPE_PLAYER) return;    //只有玩家的表情动作才有音效

	if(m_stCurrentActionAnim.bActionPlayState&&m_bPlayActionAudio)
	{
		if(!GetGame()->GetAudioList()->IsPlaying(GetActionAudio().c_str(),GetActionChannel()))
		{
			int iChannel = GetGame()->GetAudioList()->Play3DSound(GetActionAudio().c_str(),-1,GetPosY(), GetHeight(), GetPosX());
			SetActionChannel(iChannel);
		}
	}else{
		if(m_bPlayActionAudio==true)
		{
			GetGame()->GetAudioList()->Stop3DSound(GetActionAudio().c_str(),GetActionChannel());
			m_bPlayActionAudio = false;
		}
	}
}

//初始化表情动作的音效
void CMoveShape::InitActionAudio(DWORD dwFaceActionID)
{
	//停止之前正在播放的音效
	GetGame()->GetAudioList()->Stop3DSound(GetActionAudio().c_str(),GetActionChannel());
	if(GetType()!=TYPE_PLAYER) return;    //只有玩家的表情动作才有音效
	tagFaceAction * stpFaceAction = NULL;
	stpFaceAction = GetGame()->GetFaceActionList()->GetPlayerFaceAction(dwFaceActionID);
	if(stpFaceAction->sActionFileName != "0")
	{
		char ActionAudio[256]="";
		sprintf(ActionAudio,"SOUNDS/Action/%d.wav",dwFaceActionID);
		SetActionAudio(ActionAudio);
		m_bPlayActionAudio = true;
	}
}


// 是否达到当前使用技能的规定时间
bool CMoveShape::IsCurSkillActTimeFinished()
{
	if(m_dwCurSkillStartTime > 0 && m_dwCurSkillActTime > 0)
	{
		DWORD dwTime = timeGetTime() - m_dwCurSkillStartTime;
		if(dwTime >= m_dwCurSkillActTime)
		{
			m_dwCurSkillStartTime = 0;
			m_dwCurSkillActTime = 0;
			return true;
		}
	}
	return false;
}


// 添加顺序播放的技能
CEffect* CMoveShape::AddEffectToList(long lGraphicsID, CShape* pHoster, eLOCATOR eLocator, 
									 bool bLoop, float fDir, long lID,bool bAsyn)
{
	CEffect* pEffect = NULL;
	CGUID guid = AddEffectLocator(lGraphicsID,pHoster,eLocator,false,fDir,lID,bAsyn);
	if(guid != CGUID::GUID_INVALID)
	{
		pEffect = FindEffectByIndex(guid);
		if(pEffect)
		{
			RemoveEffectByIndex(guid);
			//该特效已经存在，先删除，然后再添加到最后
			if(FindEffectByOrder(pEffect->GetGraphicsID(),pEffect->GetEffectID()))
				RemoveEffectByOrder(pEffect->GetGraphicsID(),pEffect->GetEffectID());
			m_lsEffectByOrder.push_back(pEffect);
		}
	}
	return pEffect;
}

// 查找指定顺序播放的特效
CEffect * CMoveShape::FindEffectByOrder(long lGraphicsID, long lID)
{
	list<CEffect*>::iterator it = m_lsEffectByOrder.begin();
	for (; it!=m_lsEffectByOrder.end(); it++)
	{
		if ((*it)->GetGraphicsID() == lGraphicsID && (*it)->GetEffectID() == lID)
		{
			CEffect* pEffect = (*it);
			return pEffect;
		}
	}
	return NULL;
}

// 删除指定顺序播放的特效
void CMoveShape::DelEffectByOrder(long lGraphicsID, long lID)
{
	list<CEffect*>::iterator it = m_lsEffectByOrder.begin();
	for (; it!=m_lsEffectByOrder.end(); it++)
	{
		if ((*it)->GetGraphicsID() == lGraphicsID && (*it)->GetEffectID() == lID)
		{
			CEffect* pEffect = (*it);
			SAFE_DELETE(pEffect);
			m_listEffect.erase(it);
			return;
		}
	}
}

// 移出指定顺序播放的特效
void CMoveShape::RemoveEffectByOrder(long lGraphicsID, long lID)
{
	list<CEffect*>::iterator it = m_lsEffectByOrder.begin();
	for (; it!=m_lsEffectByOrder.end(); it++)
	{
		if ((*it)->GetGraphicsID() == lGraphicsID && (*it)->GetEffectID() == lID)
		{
			CEffect* pEffect = (*it);
			m_listEffect.erase(it);
			return;
		}
	}
}

// 删除全部顺序播放的特效
void CMoveShape::DelAllEffectByOrder()
{
	list<CEffect*>::iterator it = m_lsEffectByOrder.begin();
	for (; it!=m_lsEffectByOrder.end();++it)
	{
		SAFE_DELETE((*it));
	}
	m_lsEffectByOrder.clear();
}

// 顺序播放的特效
void CMoveShape::AIEffectByOrder()
{
	if(m_lsEffectByOrder.empty())
		return;
	CEffect* pEffect = m_lsEffectByOrder.front();
	AnimInfo::tagActionInfo *pActionInfo = pEffect->GetAnimInfo()->GetActionInfo();
	if(pActionInfo)
	{
		if (pEffect->GetIsDeleted())
		{
			m_lsEffectByOrder.erase(m_lsEffectByOrder.begin());
			AddEffectToList(pEffect->GetGraphicsID(),this,pEffect->GetHostLocator(),false,pEffect->GetDirEx(),pEffect->GetEffectID());
			pEffect = m_lsEffectByOrder.front();
			if(pEffect)
			{
				pActionInfo = pEffect->GetAnimInfo()->GetActionInfo();
				if(pActionInfo)
					pActionInfo->dwCurActionStartTime = timeGetTime();
			}
		}
		else
		{
			if(m_lsEffectByOrder.size() == 1)
				pEffect->SetLoop(true);
			else
				pEffect->SetLoop(false);
			pEffect->AI();
			//判断结束
			pEffect->m_bDisplayFinish = (pActionInfo->dwCurActionLoopCount > 0);
		}
	}
}

// 绘制顺序播放的特效
void CMoveShape::DisPlayEffectByOrder()
{
	if(m_lsEffectByOrder.empty())
		return;
	static DWORD dwStartTime = timeGetTime();
	DWORD dwLostTime = timeGetTime() - dwStartTime;
	CEffect* pEffect = NULL;
	// 多个特效的时候才进行时间段的顺序显示
	//if(m_lsEffectByOrder.size() > 1)
	//{
	//	// 在1500毫秒内播放第一个特效
	//	if(dwLostTime > 1500)
	//	{
	//		dwStartTime = timeGetTime();
	//		pEffect = m_lsEffectByOrder.front();
	//		m_lsEffectByOrder.erase(m_lsEffectByOrder.begin());
	//		m_lsEffectByOrder.push_back(pEffect);
	//	}
	//}
	pEffect = m_lsEffectByOrder.front();
	if(pEffect)
	{
		if (!pEffect->GetIsDeleted())
			pEffect->Display();
	}
}

// 设置正在吟唱技能的开始时间和吟唱时间
void CMoveShape::SetCurIntonateTime(DWORD dwTime,DWORD dwStartTime)
{
	m_dwIntonateTime = dwTime;
	m_dwIntonateStartTime = dwStartTime;
	if(m_dwIntonateTime == 0)
		m_bIntonateAlpha = 240;
	else
		m_bIntonateAlpha = 40;
}

// 更新吟唱条数据
void CMoveShape::UpdataIntonatebar()
{
	//CPlayer::tagSkill *pSkill = GetSkill(m_wIntonateSkillID);
	if(m_dwIntonateTime)
	{
		//取得吟唱的流逝时间
		DWORD dwTimeElapse = timeGetTime() - m_dwIntonateStartTime;
		// 吟唱中
		if (m_dwIntonateStartTime > 0 && m_dwIntonateStartTime < 0xffffffff)
		{
			if (dwTimeElapse >= m_dwIntonateTime)
			{
				m_dwIntonateStartTime = 0;
				// 吟唱完毕，设置递减的alpha初始值
				m_bIntonateAlpha = 240;
			}
			// 吟唱过程中，计算吟唱条的alpha值
			else
			{
				if(m_bIntonateAlpha > 200)
					m_bIntonateAlpha = 255;
				else
					m_bIntonateAlpha += 40;
			}
		}
		else
		{
			m_dwIntonateTime = 0;
			m_dwIntonateStartTime = 0;
		}
	}
	else
	{
		m_dwIntonateStartTime = 0;
		if(m_bIntonateAlpha > 40)
			m_bIntonateAlpha -= 40;
		else
		{
			m_bIntonateAlpha = 0;
			m_wIntonateSkillID = 0;
		}
	}
}
// 绘制吟唱条
void CMoveShape::DisplayIntonate()
{
	if(m_wIntonateSkillID == 0)
		return;
	//CPlayer::tagSkill *pSkill = GetSkill(m_wIntonateSkillID);
	if(m_dwIntonateTime || m_bIntonateAlpha > 0)
	{
		if ((m_dwIntonateStartTime > 0 && m_dwIntonateStartTime < 0xffffffff) || m_bIntonateAlpha > 0)
		{
			//取得吟唱流逝时间
			DWORD dwTimeElapse = timeGetTime() - m_dwIntonateStartTime;
			float iIndex = (float)dwTimeElapse / m_dwIntonateTime;
			// 超过吟唱时间，立即返回
			if(iIndex > 1.f && m_bIntonateAlpha == 0)
			{
				m_dwIntonateStartTime = 0;
				m_wIntonateSkillID = 0;
				return;
			}
			// 有吟唱条资源
			if(GetGame()->GetIntanateBarBackBitmap() 
				&& GetGame()->GetIntanateBarBitmap()
				&& GetGame()->GetIntanateTimePotBitmap())
			{
				int w = GetGame()->GetIntanateBarBitmap()->GetWidth();
				int h = GetGame()->GetIntanateBarBitmap()->GetHeight();
				int iLen = 0;
				CSkillListXml::tagSkill *pS = CSkillListXml::GetProperty(m_wIntonateSkillID);
				if (pS != NULL)
					iLen = GetGame()->GetFontWidth(pS->strSkillName.c_str());
				static RECT rc;
				rc.left   =  0;
				rc.right  =  (int)(iIndex * w);
				rc.top    =  0;
				rc.bottom = h;
				// 从右至左绘制
				if(!m_bIsLToR)
					rc.right = w - rc.right;
				DWORD dwColor = 0xffffffff;
				if(m_bIntonateAlpha < 255)
				{
					dwColor = m_bIntonateAlpha;
					dwColor = dwColor<<24 | 0xffffff;
				}
				int x = (SCREEN_WIDTH - GetGame()->GetIntanateBarBackBitmap()->GetWidth())/2;
				int y = SCREEN_HEIGHT*4 / 5;
				GetGame()->GetIntanateBarBackBitmap()->Draw(x,y,dwColor);
				GetGame()->GetIntanateBarBitmap()->SetColor(dwColor);
				GetGame()->GetIntanateBarBitmap()->Draw(x+8,y+7,rc);
				GetGame()->GetIntanateTimePotBitmap()->Draw(x+2+rc.right,y,dwColor);
				if(iLen!=0)
				{
					GetGame()->OutPutText((SCREEN_WIDTH-iLen)/2+1,y+5,pS->strSkillName.c_str(),iLen,dwColor & 0xff000000);
					GetGame()->OutPutText((SCREEN_WIDTH-iLen)/2,y+6,pS->strSkillName.c_str(),iLen,dwColor & 0xff000000);
					GetGame()->OutPutText((SCREEN_WIDTH-iLen)/2,y+5,pS->strSkillName.c_str(),iLen,dwColor & 0xfffabe32);
				}
			}
		}
	}
}