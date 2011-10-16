#include "StdAfx.h"
#include "Player.h"

#include "AntiCheat/CPlayerManager.h"
#include "AntiCheat/CConfiger.h"
#include "BuffSkill/NewSkillFactory.h"
#include "BuffSkill/NewSkillBaseProperties.h"
#include "Country/CountryParam.h"
#include "Container/CTestContainer.h"
#include "Country/Country.h"
#include "Country/CountryHandler.h"
#include "FBusiness/ShopManager.h"
#include "GoodsExManage.h"
#include "Goods/CGoods.h"
#include "Goods/CGoodsFactory.h"
#include "Goods/CSuitruleFactory.h"
#include "Linkman/LinkmanSystem.h"
#include "Listener/CGoodsListListener.h"
#include "Listener/CSeekGoodsListener.h"
#include "Listener/CGoodsParticularPropertyListener.h"
#include "LuaScript/Script.h"
#include "LuaScript/ScriptSys.h"
#include "MessagePackage/CS2CContainerObjectAmountChange.h"
#include "Monster.h"
#include "OrganizingSystem/GameOrganizingCtrl.h"
#include "OrganizingSystem/GSTeam.h"
#include "Pet.h"
#include "PetCtrl.h"
#include "PhaseSystem/PhaseManager.h"
#include "PKSys.h"
#include "PlayerMan/PlayerMan.h"
#include "RgnManager.h"
#include "ServerRegion.h"
#include "Session/CSessionFactory.h"
#include "Session/CSession.h"
#include "Session/CPlug.h"
#include "Session/CTrader.h"
#include "Session/CPersonalShopSeller.h"
#include "StateAI/PlayerAI.h"
#include "StateAI/PetAI.h"
#include "MonsterTaunt/TauntedList.h"
#include "StateAI/AIEventSender.h"
#include "../../../Public/Setup/GoodsSetup.h"
#include "../../../Public/Setup/GlobalSetup.h"
#include "../../../Public/Setup/PlayerList.h"
#include "../../../Public/Setup/GMList.h"
#include "../../../Public/Setup/RegionSetup.h"
#include "../../../Public/Setup/ContributeSetup.h"
#include "../../../Public/Setup/QuestSystem.h"
#include "../../../Public/Setup/ReliveGoodsList.h"
#include "../../../Public/Setup/Emotion.h"
#include "../../../Public/Common/QuestDef.h"
#include "../../../Public/ServerPublic/Server/Common/VarListUtils.h"
#include "../../../Public/ServerPublic/Server/Common/VariableList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///
/// 用于ChildObjType_GoodsExTimeOut定时器的子类型。
/// 希望通过GoodsTimerHandler将未来可能出现的各类物品身上的定时器统一。
/// GoodsTimerHandler将具有相同时间触发点的物品定时器统一为一个定时器。
///
struct GoodsExTimerType
{
	enum
	{
		GETT_GUARDIAN_ENERGY,
		// add more...
	};
};

static long lFightPetsCount = 0;
static long lPetsCount = 0;

#pragma warning(disable:4355)

map<eGoodsAddonProperties,string> CPlayer::m_mapFightEnum;
set<string>  CPlayer::m_mapOtherUpdateDirtyAttrData;

CPlayer::CPlayer(void)
: m_lPRgnTimerID(-1)
, m_lTRgnTimerID(-1)
, m_lQueueTimerID(-1)
, m_lRgnKickPlayerTimerID(-1)
, m_CurPRgnNum(0)
, m_CurTRgnNum(0)
, m_bLogin(false)
, m_bIncShopFlag(true)
, m_PlayerSavePartFlag(0)
, m_pFacMemInfo(NULL)
, m_MedalContainer(this)
, m_lCurrTitleType(ePTT_Hide)
, m_pPlayerDepot(this)
, m_pPackage(this)
//, m_pTameTarget(NULL)
, m_bReceiveMail(false)
, m_FocusPet(NULL_GUID)
, m_ActivePet(NULL_GUID)
{	
    m_PhaseRest = ePRest;
    m_GeniusAction = new GeniusAction(this);
    m_GeniusTable.clear();
	b_FirstEnterGame = true;
	////-----------------------------------
	SetType(TYPE_PLAYER);
	SetIsRun(true);

	m_dwMoveValiStartTime=timeGetTime();//移动验证开始的计时
	m_dwLastRecvMsgTime = m_dwMoveValiStartTime;
	m_fMaxPeriodMoveSpeed = 0.0f;
	m_guMoveValiStartRgnID=NULL_GUID;//开始验证的场景ID
	m_lMoveValiStartPosX=0;//开始移动的X位置
	m_lMoveValiStartPosY=0;//开始移动的Y位置
	m_fMoveValiStartSpeed = 0.0f;
	m_fMoveValiTotalDistance=0.0f;//移动的总距离
	m_fMoveValiRecvMsgCount=0.0f;//收到的移动消息个数

	m_lAICount					= 0;
	m_dwLastRestoreHpTimeStamp	= 0;
	m_dwLastRestoreMpTimeStamp	= 0;
	m_dwLastPingTime			= timeGetTime();
	m_fExpScale = 1.0f;
	m_fOccuExpScale = 1.0f;
	m_fSpScale = 1.0f;
	m_fPresentExpScale = 1.0f;

	m_lTicket=0;
	m_lTradePlugId=0;
	m_TradeSessionId=CGUID::GUID_INVALID;

	m_lLastPickGoodsTime=0;
	for(int i=0;i<24;i++)
	{
		SetHotKey(i,0);
	}
	m_mapGoodsIdCoolDown.clear();
	m_mapClassIdCoolDown.clear();
	ClearPersonalShopGoodsData();

	ReleaseNpcShopSellGoods();	

	m_dwYuanbao=0;    

	memset(&m_AppendProperty, 	0,	(long)sizeof(tagPlayerAppendProperty));
	memset(&m_FightProperty,  	0,	(long)sizeof(tagPlayerFightProperty));
	memset(&m_Property,	    	0,	(long)sizeof(tagPlayerProperty));
	memset(&m_BaseFightProperty,0,	(long)sizeof(tagPlayerFightProperty));
	memset(m_dwServerKey,		0,	sizeof(DWORD) * SERVER_KEY_DWORD_SIZE);
	//初始化人物属性
	//InitProperty();


	/*m_pVariableList = NULL;*/
	//m_TeamID = 0;
	m_bCaptain = false;

	//	m_strDepotPassword="";

	InitNameValueMap();
	//m_bInChangingServer			= false;

	m_lFightStateCount			= 0;
	m_dwSinStateTimeStamp		= 0;
	m_dwContinuousKillAmount	= 0;
	m_dwContinuousKillTimeStamp	= 0;
	m_lSilenceTime				= 0;
	m_lSilenceTimeStamp			= 0;
	m_bGMInvincible				= false;
	m_bGMConcealment			= false;
	m_bAlreadyEnterGame			= false;
	m_bNetTransferStatus		= false;
	m_bReliveSkill				= false;

	m_dwLostTimeStamp = 0;
	m_dwMurdererTimeStamp = 0;
	m_lInvestLimitTimeMark=0;

	m_pAI = new StateAI::PlayerAI( this );

	m_dwDisconnectTimeStamp		= 0;
	m_dwDisconnectDelayTime		= 0;

	m_eProgress					= PROGRESS_NONE;
	m_PersonalShopPlugID		= 0;

	m_lEmotionIndex				= 0;
	m_dwEmotonTimeStamp			= 0;

	m_lFightStateTimerID = -1;
	m_dwIntonateStartTime = 0;
	m_dwIntonateTime	  = 0;
	m_dwActStartTime	  = 0;
	m_dwActTime			  = 0;

	//##添加监听器
	m_cEquipment.AddListener( this );	
    m_cEquipmentGod.AddListener(this);
    m_cEquipmentLand.AddListener(this);
    m_cEquipmentHuman.AddListener(this);

	m_cEquipmentGod.SetTrumpType(TT_GOD);
	m_cEquipmentLand.SetTrumpType(TT_GLAND);
	m_cEquipmentHuman.SetTrumpType(TT_HUMAN);

	m_EffectIdSeed = 0;
	strcpy_s(m_szSessionID, 33, "");
	m_mapIncShopCur10.insert(IncShopCur10Map::value_type(CIncrementShopList::TM_YUANBAO,list<DWORD>()));
	m_mapIncShopCur10.insert(IncShopCur10Map::value_type(CIncrementShopList::TM_WEIMIAN,list<DWORD>()));
	m_mapIncShopCur10.insert(IncShopCur10Map::value_type(CIncrementShopList::TM_TICKET,list<DWORD>()));

	m_mapLimitGoodsRecord.clear();
	m_vecTradeList.clear();
	m_lPKTimerID = -1;
	m_lPvPTimerID = -1;
	m_lHpMpTimerID = -1;
	//m_lEnergyTimerID = -1;
	m_lReliveTimerID = -1;

	//提示的计数器
	m_nHintCounter = 0;
	//帮助提示
	m_HelpHints.clear();
	//界面提示
	m_InterfaceHints.clear();
	m_lReceiveMail.clear();

	//已经选出的制作id
	m_dwElectFactureID=0;

	m_lTotalSkillNum = 0;
	m_dwSkillStartTime = timeGetTime();

	m_lTotalNpcScriptsNum = 0;
	m_dwNpcScriptsStartTIme = timeGetTime();

	m_SpriteSystem.SetOwner(this);

	memset(m_szFactionName, 0, MAXSIZE_FactionName);
	memset(m_szUnionName, 0, MAXSIZE_FactionName);

	m_dwBounsTime = timeGetTime();

    m_mover.Init( this );

    m_tauntList = new TauntedList( this );
}

CPlayer::~CPlayer(void)
{
	ReleaseAllPets();
	m_pPlayerDepot.Release();
	b_FirstEnterGame = false;
	m_GoodsAllPurposeOpt.Release();
	m_lReceiveMail.clear();

    SAFE_DELETE(m_GeniusAction);
    m_GeniusTable.clear();
	delete m_pAI;
	m_pPackage.Release();
	map<long,vector<tagSuitGoods>*>::iterator vecIter=  m_mSuitState.begin();
	for(;vecIter!=m_mSuitState.end();vecIter++)
	{		
		SAFE_DELETE(vecIter->second);
	}
	m_mSuitState.clear();
	ClearPersonalShopGoodsData();
	ReleaseNpcShopSellGoods();

	// 删除定时器
	map<CGUID, long>::iterator ite = m_GoodsTimerID.begin();
	for (;ite != m_GoodsTimerID.end(); ++ite)
	{
		void* pVar = NULL;
		GameManager::GetInstance()->Cancel(ite->second, (const void**)&pVar);
	}
	m_GoodsTimerID.clear();


	CancelRgnKickPlayerTimer();
	CancelCreatePRgnTimer();
	CancelCreateTRgnTimer();
	CancelQueueTimer();
	//StopAllSkills();
	ClearActiveSkills();
	ClearPassiveSkill();
	ClearAllBuff();

    delete m_tauntList;
}

// [070608 AHC]
bool CPlayer::SetAttribute(std::string& attrName, long value)
{
#ifdef _DEBUG_SKILL
	/*char strDebug[256];
	_snprintf(strDebug, 256, "玩家状态属性名称%s值%d\n",attrName.c_str(),value);
	OutputDebugStr(strDebug);*/
	//return true;
#endif
	bool bFlag = CMoveShape::SetAttribute(attrName, value);
	if(!bFlag)
	{
		if("Money" == attrName)
		{
			return SetMoney(value,3);
		}
        
        if("Silver" == attrName)
        {
            return SetSilver(value);
        }
		// 脏属性数据标记
		if( SetValue(attrName.c_str(), value) != INVILID_VALUE )
		{

			bFlag = true;
		}
	}
	if(bFlag)
		AddAttributeToDirty(attrName);
	return bFlag;
}
// [070608 AHC]
bool CPlayer::SetAttribute(char* attrName, long value)
{
	if(NULL == attrName)
		return false;

#ifdef _DEBUG_SKILL
	/*char strDebug[256];
	_snprintf(strDebug, 256, "玩家状态属性名称%s值%d\n",attrName.c_str(),value);
	OutputDebugStr(strDebug);*/
	//return true;
#endif
	bool bFlag = CMoveShape::SetAttribute(attrName, value);
	if(!bFlag)
	{
		if( strcmp("Money", attrName) == 0 )
		{
			return SetMoney(value,4);
		}
		// 脏属性数据标记
		if( SetValue(attrName, value) != INVILID_VALUE )
		{			
			bFlag =  true;
		}
	}
	if(bFlag)
		AddAttributeToDirty(attrName);
	return bFlag;
}

long  CPlayer::SetStateAttribute(std::string& attrName,long value)
{
    long ret = CMoveShape::SetStateAttribute( attrName , value );
    
    if( ret < 0 )
        SetAttribute(attrName,value);
    return 0;
}

long  CPlayer::ChangeStateAttribute(std::string& attrName,long value)
{
    long ret = CMoveShape::ChangeStateAttribute( attrName,value );

    if ( ret < 0 )
    {
        ChangeAttribute( attrName , value );
    }
    return 0;
}

//因pk值/pvp值成一定比例触发一个脚本
void CPlayer::OnPKOrPVPValueChange(long lOldValue,long lNewValue)
{
	string strPkVaule="PkValue";
	string strPvPValue="PVPValue";
	long lCurrentPkValue = GetAttribute(strPkVaule);
	long lCurrentPvPValue = GetAttribute(strPvPValue);
	if (lCurrentPkValue!=0 && lCurrentPvPValue/lCurrentPkValue==GlobalSetup::GetSetup()->fPkToPvpValue)
	{
		const char* strScript = GlobalSetup::GetSetup()->pszPKToPvPScript;
		if(strScript)
		{
			Script::RunEnv env( this, NULL, static_cast<CRegion*>( GetFather() ) );
			GetInst( ScriptSys ).RunScript( env, strScript );
		}
	}
	if( (!(lOldValue == 0 && lNewValue == 0) && 
		!(lOldValue>0 && lOldValue <= 20 && lNewValue > 0 && lNewValue <= 20) &&
		!(lOldValue > 20 && lNewValue > 20 )) 
		|| (lOldValue=0 && (lNewValue>0 && lNewValue<ePvp_First_Level))
		|| (lOldValue<ePvp_First_Level && (lNewValue>=ePvp_First_Level && lNewValue<ePvp_Second_Level))
		|| (lOldValue<ePvp_Second_Level && (lNewValue>=ePvp_Second_Level && lNewValue<ePvp_Third_Level))
		|| (lOldValue<ePvp_Third_Level && (lNewValue>=ePvp_Third_Level)))
	{
		CMessage msg(MSG_S2C_PLAYER_SYNC_PKCOUNT);
		msg.Add(GetExID());
		msg.Add(lCurrentPkValue);
		msg.Add(lCurrentPvPValue);
		msg.SendToAround(this,this);
	}
}

long CPlayer::GetAttribute(std::string attrName)
{
	long value = INVILID_VALUE;

	if(attrName.size() < 2)	return value;

	value = CMoveShape::GetAttribute(attrName);
	if(value != INVILID_VALUE)
		return value;

	if("Money" == attrName)
	{
		return GetMoney();
	}

	if(attrName.size() > 2 && attrName[0] == 'C' && attrName[1] == '_') // 取得当前值
	{
		// 取得改变值
		attrName[0] = 'E';
		long evalue = GetValue(attrName.c_str());
		// 取得基本值
		attrName[0] = 'B';
		long bvalue = GetValue(attrName.c_str());

		// 取得当前值的合法值
		map<string, tagPty>::iterator vMoveShapeItr = m_mapShapeNameValue.find(attrName);
		if(vMoveShapeItr == m_mapShapeNameValue.end())
		{
			map<string, tagPty>::iterator vPlayerItr = m_mapNameValue.find(attrName);
			if(vPlayerItr != m_mapNameValue.end())
			{
				tagPty& tagValue = vPlayerItr->second;
				value = GetCurrentValue((char*)vPlayerItr->first.c_str(), tagValue, evalue, bvalue);
			}
		}
		else
		{
			tagPty& tagValue = vMoveShapeItr->second;
			value = GetCurrentValue((char*)vMoveShapeItr->first.c_str(), tagValue, evalue, bvalue);
		}
	}
	else
		value = GetValue(attrName.c_str());



	return value;
}
// 取得当前值的合法值
long CPlayer::GetCurrentValue(char* valueName, const tagPty& tagValue, long evalue/*改变值*/, long tmpValue)
{
	long retValue = evalue+tmpValue;

	//// 9.21.2009 对CP值而言，计算公式不同
	//if( stricmp( valueName, "E_MaxPKCP" ) == 0 ||
	//	stricmp( valueName, "B_MaxPKCP" ) == 0 )
	//{
	//	// (基本CP值+变化CP值)*(1+装备CP倍率总和/100)
	//	retValue = (long)( retValue * ( 1 + GetAllEquipTimesCP() / 100.0f ) );
	//}

	if( retValue > tagValue.maxValue )
		retValue = tagValue.maxValue;

	if( retValue < tagValue.minValue )
		retValue = tagValue.minValue;

	if( retValue < 0 )
		retValue = 0;

	/*if (!stricmp(valueName,"E_MaxHp"))
	{
		retValue =  GetMaxHP();
	}
	if (!stricmp(valueName,"E_MaxMp"))
	{
		retValue =  GetMaxMP();
	}*/

	if (!stricmp(valueName,"Hp"))
	{
		string str="C_MaxHp";
		long lMaxHp = GetMaxHP();
		if (retValue>=lMaxHp)
		{
			retValue = lMaxHp;
		}
	} 
	else if (!stricmp(valueName,"Mp"))
	{
		string str="C_MaxMp";
		long lMaxMp = GetMaxMP();
		if (retValue>=lMaxMp)
		{
			retValue = lMaxMp;
		}
	}	

	return retValue;
}
//设置最大能量值
void	CPlayer::SetBaseMaxEnergy(WORD l)					
{
	m_BaseFightProperty.wMaxEnergy = l; 
	if(GetEnergy()>GetMaxEnergy()) 
		SetEnergy(GetMaxEnergy());
}
bool  CPlayer::ChangeAttribute(std::string& attrName, long value)
{
	bool bFlag = CMoveShape::ChangeAttribute(attrName, value);
	//if(!bFlag)
	//{
	//	// 脏属性数据标记
	//	if("Money" == attrName)
	//	{
	//		SetMoney(GetMoney()+value);
	//		return true;
	//	}
	//	else if (("Hp"==attrName && value>0) && !GetAddHp())
	//	{
	//		return true;
	//	}
	//	long curValue = 0;
	//	long baseValue = 0;
	//	mapNameValueItr itr = m_mapNameValue.begin();
	//	itr = m_mapNameValue.find(attrName);
	//	if (itr != m_mapNameValue.end())
	//	{
	//		baseValue = GetValue((char*)itr->first.c_str());
	//		// 属性值合法处理
	//		//curValue = GetCurrentValue((char*)itr->first.c_str(), itr->second, value, baseValue);
	//		curValue = baseValue + value;
	//		if(SetValue(attrName.c_str(), curValue) != INVILID_VALUE)	
	//		{
	//			bFlag = true;
	//		}
	//	}
	//}
	if(!bFlag)
	{
		// 脏属性数据标记
		if("Money" == attrName)
		{
			SetMoney(GetMoney()+value,5);
			return true;
		}
		else if (("Hp"==attrName && value>0) && !GetAddHp())
		{
			return true;
		}

		long curValue = 0;
		long rightValue = GetValue(attrName.c_str());
		curValue = rightValue + value;
		mapNameValueItr itr = m_mapNameValue.begin();
		itr = m_mapNameValue.find(attrName);
		if (itr != m_mapNameValue.end())
		{
			if(attrName[0] != 'E') // 改变值可以为负数
			{
				if( rightValue + value <  itr->second.minValue ) 
					curValue = itr->second.minValue;
				if( rightValue + value > itr->second.maxValue ) 
					curValue = itr->second.maxValue;
			}

			if (attrName == "Hp")
			{
				string str="C_MaxHp";
				long lMaxHp = GetMaxHP();
				if (curValue>=lMaxHp)
				{
					curValue = lMaxHp;
				}
			} 
			else if (attrName == "Mp")
			{
				string str="C_MaxMp";
				long lMaxMp = GetMaxMP();
				if (curValue>=lMaxMp)
				{
					curValue = lMaxMp;
				}
			}
			else if (attrName == "Energy")
			{
				string str="C_MaxEnergy";
				long lMaxEnergy = GetAttribute(str);
				if (curValue<0)
				{
					curValue = 0;
				}
				else if (curValue>lMaxEnergy)
				{
					curValue = lMaxEnergy;
				}
			}
			// PK CP值范围验证
			else if( attrName == "CurPKCP" )
			{
				long lMaxPKCP = GetAttribute( "C_MaxPKCP" );
				if( curValue > lMaxPKCP )
				{
					curValue = lMaxPKCP;
				}
			}

			if(SetValue(attrName.c_str(), curValue) != INVILID_VALUE)
			{
				bFlag = true;
			}
		}
	}
	if(bFlag)
		AddAttributeToDirty(attrName);
	return bFlag;
}

void CPlayer::AddFightPropertyTODirty()
{
	m_mapDirtyAttrData.insert("C_MaxHp");
	m_mapDirtyAttrData.insert("C_MaxMp");
	m_mapDirtyAttrData.insert("C_MaxStamina");
	m_mapDirtyAttrData.insert("C_MaxEnergy");
	m_mapDirtyAttrData.insert("C_HpRecoverSpeed");
	m_mapDirtyAttrData.insert("C_MpRecoverSpeed");
	m_mapDirtyAttrData.insert("C_StamRecoverSpeed");
    m_mapDirtyAttrData.insert("C_EnerRecoverSpeed");
    m_mapDirtyAttrData.insert("C_FightHpRecoverSpeed");
    m_mapDirtyAttrData.insert("C_FightMpRecoverSpeed");
    m_mapDirtyAttrData.insert("C_FightEnerRecoverSpeed");
    m_mapDirtyAttrData.insert("C_FightStamRecoverSpeed");

    m_mapDirtyAttrData.insert("C_Manly");
    m_mapDirtyAttrData.insert("C_PerverseTrend");
    m_mapDirtyAttrData.insert("C_Complaint");
    m_mapDirtyAttrData.insert("C_MurderousLook");

	m_mapDirtyAttrData.insert("C_Str");
	m_mapDirtyAttrData.insert("C_Dex");
	m_mapDirtyAttrData.insert("C_Con");
	m_mapDirtyAttrData.insert("C_Intellect");
	m_mapDirtyAttrData.insert("C_Spiritualism");
    m_mapDirtyAttrData.insert("C_Point");

    m_mapDirtyAttrData.insert("C_Parry");
    m_mapDirtyAttrData.insert("C_Dodge");
    m_mapDirtyAttrData.insert("C_Def");
    m_mapDirtyAttrData.insert("C_DefSuck");
    m_mapDirtyAttrData.insert("C_Mdef");
    m_mapDirtyAttrData.insert("C_MdefSuck");
    m_mapDirtyAttrData.insert("C_BloodSuck");
    m_mapDirtyAttrData.insert("C_ManaSuck");
    m_mapDirtyAttrData.insert("C_StaminaSuck");

    m_mapDirtyAttrData.insert("C_Hit");
    m_mapDirtyAttrData.insert("C_AtkSpeed");
    m_mapDirtyAttrData.insert("C_MAtkSpeed");
    m_mapDirtyAttrData.insert("C_MinAtk");
    m_mapDirtyAttrData.insert("C_MaxAtk");
    m_mapDirtyAttrData.insert("C_MinMAtk");
    m_mapDirtyAttrData.insert("C_MaxMAtk");
    m_mapDirtyAttrData.insert("C_PhysicImmunity");
    m_mapDirtyAttrData.insert("C_MagicImmunity");
    m_mapDirtyAttrData.insert("C_MoveSpeed");
    m_mapDirtyAttrData.insert("C_MoveAcceleration");
    m_mapDirtyAttrData.insert("C_MoveRadius");
    m_mapDirtyAttrData.insert("C_TurnSpeed");

    m_mapDirtyAttrData.insert("C_ElemDef0");
    m_mapDirtyAttrData.insert("C_ElemDef1");
    m_mapDirtyAttrData.insert("C_ElemDef2");
    m_mapDirtyAttrData.insert("C_ElemDef3");
    m_mapDirtyAttrData.insert("C_ElemDef4");
    m_mapDirtyAttrData.insert("C_ElemDef5");

    m_mapDirtyAttrData.insert("C_DeBuffDef0");
    m_mapDirtyAttrData.insert("C_DeBuffDef1");
    m_mapDirtyAttrData.insert("C_DeBuffDef2");
    m_mapDirtyAttrData.insert("C_DeBuffDef3");
    m_mapDirtyAttrData.insert("C_DeBuffDef4");
    m_mapDirtyAttrData.insert("C_DeBuffDef5");

    m_mapDirtyAttrData.insert("C_ClassDam0");
    m_mapDirtyAttrData.insert("C_ClassDam1");
    m_mapDirtyAttrData.insert("C_ClassDam2");
    m_mapDirtyAttrData.insert("C_ClassDam3");
    m_mapDirtyAttrData.insert("C_ClassDam4");
    m_mapDirtyAttrData.insert("C_ClassDam5");
    m_mapDirtyAttrData.insert("C_ClassDam6");

	m_mapDirtyAttrData.insert("C_Block");
	m_mapDirtyAttrData.insert("C_Charm");
	m_mapDirtyAttrData.insert("C_Absorb");
	m_mapDirtyAttrData.insert("C_Cri");
	m_mapDirtyAttrData.insert("C_BuffPower");
	m_mapDirtyAttrData.insert("C_MCri");
	m_mapDirtyAttrData.insert("C_CriDef");
	m_mapDirtyAttrData.insert("C_MCriDef");
	m_mapDirtyAttrData.insert("C_WillDef");
	m_mapDirtyAttrData.insert("C_ConstiDef");	
	m_mapDirtyAttrData.insert("C_ElemDam0");
	m_mapDirtyAttrData.insert("C_ElemDam1");
	m_mapDirtyAttrData.insert("C_ElemDam2");
	m_mapDirtyAttrData.insert("C_ElemDam3");
	m_mapDirtyAttrData.insert("C_ElemDam4");
	m_mapDirtyAttrData.insert("C_ElemDam5");
	m_mapDirtyAttrData.insert("C_ElemAtk0");
	m_mapDirtyAttrData.insert("C_ElemAtk1");
	m_mapDirtyAttrData.insert("C_ElemAtk2");
	m_mapDirtyAttrData.insert("C_ElemAtk3");
	m_mapDirtyAttrData.insert("C_ElemAtk4");
	m_mapDirtyAttrData.insert("C_ElemAtk5");
	m_mapDirtyAttrData.insert( "C_MaxPKCP" );
}

//添加变量名到胀数据
void  CPlayer::AddAttributeToDirty(string attrName)
{
	if(attrName == "")	return;
	if( (attrName[0] == 'E' && attrName[1] == '_')
		|| (attrName[0] == 'B' && attrName[1] == '_') )
		attrName[0] = 'C';
	m_mapDirtyAttrData.insert(attrName);
	InsertOtherDirtyName(attrName);

	GSTeam *pTeam = (GSTeam*)GetOrganiCtrl()->GetGameTeamByID(GetTeamID());
	if(NULL != pTeam)
	{
		long lChangeType = 0;
		LPCSTR strName = attrName.c_str();

		if (0 == strcmp(strName, "B_MaxHp") || 0 == strcmp(strName, "E_MaxHp") || 0 == strcmp(strName, "C_MaxHp"))
		{
			lChangeType = eMIS_MaxHp;
		}
		else if (0 == strcmp(strName, "B_MaxMp") || 0 == strcmp(strName, "E_MaxMp") || 0 == strcmp(strName, "C_MaxMp"))
		{
			lChangeType = eMIS_MaxMp;
		}
		else
		{
			return;
		}
		
		pTeam->OnPlayerIdioinfoChange(GetExID(), (eUpdateIdioinfo)lChangeType);
	}
}

void CPlayer::CodeChangeAttrToDataBlock(DBWriteSet& setWriteDB)
{
	long lShapeNum = 0;
	long lPlayerNum = 0;
	vector<BYTE>	byShapeDirtyData;byShapeDirtyData.clear();
	vector<BYTE>	byDirtyData;byDirtyData.clear();

#ifdef _DEBUG_SKILL
	/*char str[256];
	_snprintf(str, 256, "\n修改属性个数：%d\n",ShapeAttrNum+PlayerAttrNum);
	if((ShapeAttrNum+PlayerAttrNum)>200)
	{
	long x = 0;
	}
	OutputDebugString(str);*/
#endif
	set<string>::iterator sStrItr = m_mapDirtyAttrData.begin();
	for(; sStrItr != m_mapDirtyAttrData.end(); sStrItr++)
	{
		mapShapeNameValueItr sitr = m_mapShapeNameValue.find(*sStrItr);
		if (sitr != m_mapShapeNameValue.end())
		{
			lShapeNum++;
			// 属性类型
			_AddToByteArray(&byShapeDirtyData, (WORD)sitr->second.attrEnumValue);
			switch (sitr->second.lType)
			{
			case 0:	// byte
				_AddToByteArray(&byShapeDirtyData, (BYTE)ATTR_DATA_BYTE);
				_AddToByteArray(&byShapeDirtyData, (WORD)sizeof(BYTE));
				_AddToByteArray(&byShapeDirtyData, (BYTE)*(BYTE*)(sitr->second.plValue));
				break;
			case 1:	// word
				_AddToByteArray(&byShapeDirtyData, (BYTE)ATTR_DATA_WORD);
				_AddToByteArray(&byShapeDirtyData, (WORD)sizeof(WORD));
				_AddToByteArray(&byShapeDirtyData, (WORD)*(WORD*)(sitr->second.pwValue));
				break;
			case 2:	// dword
				_AddToByteArray(&byShapeDirtyData, (BYTE)ATTR_DATA_DWORD);
				_AddToByteArray(&byShapeDirtyData, (WORD)sizeof(DWORD));
				_AddToByteArray(&byShapeDirtyData, (DWORD)*(DWORD*)(sitr->second.pdwValue));
				break;
			case 3:	// float
				_AddToByteArray(&byShapeDirtyData, (BYTE)ATTR_DATA_FLOAT);
				_AddToByteArray(&byShapeDirtyData, (WORD)sizeof(float));
				_AddToByteArray(&byShapeDirtyData, (float)*(float*)(sitr->second.pfValue));
				break;
			case 4:	// long
				_AddToByteArray(&byShapeDirtyData, (BYTE)ATTR_DATA_LONG);
				_AddToByteArray(&byShapeDirtyData, (WORD)sizeof(long));
				_AddToByteArray(&byShapeDirtyData, (long)*(long*)(sitr->second.pValue));
				break;
			case 5:	// short
				_AddToByteArray(&byShapeDirtyData, (BYTE)ATTR_DATA_SHORT);
				_AddToByteArray(&byShapeDirtyData, (WORD)sizeof(short));
				_AddToByteArray(&byShapeDirtyData, (short)*(short*)(sitr->second.psValue));
				break;
			case 6:	// bool
				_AddToByteArray(&byShapeDirtyData, (BYTE)ATTR_DATA_BOOL);
				_AddToByteArray(&byShapeDirtyData, (WORD)sizeof(bool));
				_AddToByteArray(&byShapeDirtyData, (char)*(bool*)(sitr->second.pbValue));
				break;
			default:
				break;
			}
		}


		long baseValue = 0;
		string  name = *sStrItr;
		if(name[0] == 'C' && name[1] == '_')
		{	
			// 先找出基本值
			name[0] = 'B';
			baseValue = GetValue(name.c_str());
			// 再找出改变值
			name[0] = 'E';
		}

		mapNameValueItr itr = m_mapNameValue.find(name);
		if (itr!=m_mapNameValue.end())
		{
			lPlayerNum++;
			// 属性类型
			_AddToByteArray(&byDirtyData, (WORD)itr->second.attrEnumValue);
			long sendValue = INVILID_VALUE;

			switch (itr->second.lType)
			{
			case 0:	// byte
				::_AddToByteArray(&byDirtyData, (BYTE)ATTR_DATA_BYTE);
				::_AddToByteArray(&byDirtyData, (WORD)sizeof(BYTE));
				if((*sStrItr)[0] == 'C' && (*sStrItr)[1] == '_') // 最终值
				{
					BYTE EValue = *(BYTE*)(itr->second.plValue);
					sendValue = GetCurrentValue((char*)itr->first.c_str(), itr->second, EValue, baseValue);
				}
				else
					sendValue = (long)*(BYTE*)(itr->second.plValue);

				::_AddToByteArray(&byDirtyData, (BYTE)sendValue);

				break;
			case 1:	// word
				::_AddToByteArray(&byDirtyData, (BYTE)ATTR_DATA_WORD);
				::_AddToByteArray(&byDirtyData, (WORD)sizeof(WORD));
				if(((*sStrItr)[0] == 'C' && (*sStrItr)[1] == '_')) // 改变值) // 改变值
				{
					WORD EValue = *(WORD*)(itr->second.pwValue);
					sendValue = GetCurrentValue((char*)itr->first.c_str(), itr->second, EValue, baseValue);
				}
				else
					sendValue = (long)*(WORD*)(itr->second.pwValue);
				::_AddToByteArray(&byDirtyData, (WORD)sendValue);
				break;
			case 2:	// dword
				::_AddToByteArray(&byDirtyData, (BYTE)ATTR_DATA_DWORD);
				::_AddToByteArray(&byDirtyData, (WORD)sizeof(DWORD));
				if(((*sStrItr)[0] == 'C' && (*sStrItr)[1] == '_')) // 改变值) // 改变值
				{
					DWORD EValue = *(DWORD*)(itr->second.pdwValue);
					sendValue = GetCurrentValue((char*)itr->first.c_str(), itr->second, EValue, baseValue);
				}
				else
					sendValue = (long)*(DWORD*)(itr->second.pdwValue);
				::_AddToByteArray(&byDirtyData, (DWORD)sendValue);
				break;
			case 3:	// float
				::_AddToByteArray(&byDirtyData, (BYTE)ATTR_DATA_FLOAT);
				::_AddToByteArray(&byDirtyData, (WORD)sizeof(float));
				if(((*sStrItr)[0] == 'C' && (*sStrItr)[1] == '_')) // 改变值) // 改变值
				{
					float EValue = *(float*)(itr->second.pfValue);
					sendValue = GetCurrentValue((char*)itr->first.c_str(), itr->second, (long)EValue, baseValue);
				}
				else
					sendValue = (long)*(float*)(itr->second.pfValue);
				::_AddToByteArray(&byDirtyData, (float)sendValue);
				break;
			case 4:	// long
				//msg.Add((BYTE)ATTR_DATA_LONG);
				//msg.Add((WORD)sizeof(long));
				::_AddToByteArray(&byDirtyData, (BYTE)ATTR_DATA_LONG);
				::_AddToByteArray(&byDirtyData, (WORD)sizeof(long));
				if(((*sStrItr)[0] == 'C' && (*sStrItr)[1] == '_')) // 改变值) // 改变值
				{
					long EValue = (long)*(long*)(itr->second.pValue);
					sendValue = GetCurrentValue((char*)itr->first.c_str(), itr->second, EValue, baseValue);
				}
				else
					sendValue = (long)*(long*)(itr->second.pValue);
				::_AddToByteArray(&byDirtyData, (long)sendValue);
				break;
			case 5:	// short
				::_AddToByteArray(&byDirtyData, (BYTE)ATTR_DATA_SHORT);
				::_AddToByteArray(&byDirtyData, (WORD)sizeof(short));
				if(((*sStrItr)[0] == 'C' && (*sStrItr)[1] == '_')) // 改变值) // 改变值
				{
					short EValue = *(short*)(itr->second.psValue);
					sendValue = GetCurrentValue((char*)itr->first.c_str(), itr->second, EValue, baseValue);
				}
				else
					sendValue = (long)*(short*)(itr->second.psValue);
				::_AddToByteArray(&byDirtyData, (short)sendValue);
				break;
			case 6:	// bool
				::_AddToByteArray(&byDirtyData, (BYTE)ATTR_DATA_BOOL);
				::_AddToByteArray(&byDirtyData, (WORD)sizeof(bool));
				if(((*sStrItr)[0] == 'C' && (*sStrItr)[1] == '_')) // 改变值) // 改变值
				{
					bool EValue = *(bool*)(itr->second.pbValue);
					sendValue = GetCurrentValue((char*)itr->first.c_str(), itr->second, EValue, baseValue);
				}
				else
					sendValue = (long)*(bool*)(itr->second.pbValue);
				::_AddToByteArray(&byDirtyData, (BYTE)sendValue);
				break;
			default:
				break;
			}
		}
	}

	// Monster属性个数
	setWriteDB.AddToByteArray( (WORD)lShapeNum);
	if(byShapeDirtyData.size() > 0)
		setWriteDB.AddToByteArray(&byShapeDirtyData[0],(long)byShapeDirtyData.size());
	setWriteDB.AddToByteArray( (WORD)lPlayerNum);
	if(byDirtyData.size() > 0)
		setWriteDB.AddToByteArray(&byDirtyData[0],(long)byDirtyData.size());
	m_mapDirtyAttrData.clear();
}
bool CPlayer::CodeOtherUpdateAttrToDataBlock(DBWriteSet& setWriteDB)
{
	vector<BYTE> vecData;vecData.clear();
	vector<BYTE> *bArray = &vecData;
	vector<BYTE> vecPlayerData;vecPlayerData.clear();
	vector<BYTE> *bPlayerArray = &vecPlayerData;
	long lShapeNum =0;
	long lNum =0;	
	mapNameValueItr shapeitr = m_mapShapeNameValue.begin();
	mapNameValueItr playeritr = m_mapNameValue.begin();
	long baseValue = 0;
	string name = "";
	set<string>::iterator pStrItr = m_mapOtherDirtyAttrData.begin();
	for(; pStrItr!= m_mapOtherDirtyAttrData.end(); pStrItr++)
	{
		name = *pStrItr;

		shapeitr = m_mapShapeNameValue.find(name);
		if (shapeitr!=m_mapShapeNameValue.end())
		{
			lShapeNum++;
			// 属性类型
			_AddToByteArray(bArray, (WORD)shapeitr->second.attrEnumValue);

			long sendValue = INVILID_VALUE;
			switch (shapeitr->second.lType)
			{
			case 0:	// byte
				::_AddToByteArray(bArray, (BYTE)ATTR_DATA_BYTE);
				::_AddToByteArray(bArray, (WORD)sizeof(BYTE));
				if((*pStrItr)[0] == 'C' && (*pStrItr)[1] == '_') // 最终值
				{
					BYTE EValue = *(BYTE*)(shapeitr->second.plValue);
					sendValue = GetCurrentValue((char*)shapeitr->first.c_str(), shapeitr->second, EValue, baseValue);
				}
				else
					sendValue = (long)*(BYTE*)(shapeitr->second.plValue);

				::_AddToByteArray(bArray, (BYTE)sendValue);

				break;
			case 1:	// word
				::_AddToByteArray(bArray, (BYTE)ATTR_DATA_WORD);
				::_AddToByteArray(bArray, (WORD)sizeof(WORD));
				if(((*pStrItr)[0] == 'C' && (*pStrItr)[1] == '_')) // 改变值) // 改变值
				{
					WORD EValue = *(WORD*)(shapeitr->second.pwValue);
					sendValue = GetCurrentValue((char*)shapeitr->first.c_str(), shapeitr->second, EValue, baseValue);
				}
				else
					sendValue = (long)*(WORD*)(shapeitr->second.pwValue);
				::_AddToByteArray(bArray, (WORD)sendValue);
				break;
			case 2:	// dword
				::_AddToByteArray(bArray, (BYTE)ATTR_DATA_DWORD);
				::_AddToByteArray(bArray, (WORD)sizeof(DWORD));
				if(((*pStrItr)[0] == 'C' && (*pStrItr)[1] == '_')) // 改变值) // 改变值
				{
					DWORD EValue = *(DWORD*)(shapeitr->second.pdwValue);
					sendValue = GetCurrentValue((char*)shapeitr->first.c_str(), shapeitr->second, EValue, baseValue);
				}
				else
					sendValue = (long)*(DWORD*)(shapeitr->second.pdwValue);
				::_AddToByteArray(bArray, (DWORD)sendValue);
				break;
			case 3:	// float
				::_AddToByteArray(bArray, (BYTE)ATTR_DATA_FLOAT);
				::_AddToByteArray(bArray, (WORD)sizeof(float));
				if(((*pStrItr)[0] == 'C' && (*pStrItr)[1] == '_')) // 改变值) // 改变值
				{
					float EValue = *(float*)(shapeitr->second.pfValue);
					sendValue = GetCurrentValue((char*)shapeitr->first.c_str(), shapeitr->second,(long) EValue, baseValue);
				}
				else
					sendValue = (long)*(float*)(shapeitr->second.pfValue);
				::_AddToByteArray(bArray, (float)sendValue);
				break;
			case 4:	// long
				::_AddToByteArray(bArray, (BYTE)ATTR_DATA_LONG);
				::_AddToByteArray(bArray, (WORD)sizeof(long));
				if(((*pStrItr)[0] == 'C' && (*pStrItr)[1] == '_')) // 改变值) // 改变值
				{
					long EValue = (long)*(long*)(shapeitr->second.pValue);
					sendValue = GetCurrentValue((char*)shapeitr->first.c_str(), shapeitr->second, EValue, baseValue);
				}
				else
					sendValue = (long)*(long*)(shapeitr->second.pValue);
				::_AddToByteArray(bArray, (long)sendValue);
				break;
			case 5:	// short
				::_AddToByteArray(bArray, (BYTE)ATTR_DATA_SHORT);
				::_AddToByteArray(bArray, (WORD)sizeof(short));
				if(((*pStrItr)[0] == 'C' && (*pStrItr)[1] == '_')) // 改变值) // 改变值
				{
					short EValue = *(short*)(shapeitr->second.psValue);
					sendValue = GetCurrentValue((char*)shapeitr->first.c_str(), shapeitr->second, EValue, baseValue);
				}
				else
					sendValue = (long)*(short*)(shapeitr->second.psValue);
				::_AddToByteArray(bArray, (short)sendValue);
				break;
			case 6:	// bool
				::_AddToByteArray(bArray, (BYTE)ATTR_DATA_BOOL);
				::_AddToByteArray(bArray, (WORD)sizeof(bool));
				if((name[0] == 'E' && name[1] == '_')) // 改变值) // 改变值
				{
					bool EValue = *(bool*)(shapeitr->second.pbValue);
					sendValue = GetCurrentValue((char*)shapeitr->first.c_str(), shapeitr->second, EValue, baseValue);
				}
				else
					sendValue = (long)*(bool*)(shapeitr->second.pbValue);
				::_AddToByteArray(bArray, (BYTE)sendValue);
				break;
			default:
				break;
			}
		}

		if(name[0] == 'C' && name[1] == '_')
		{	
			// 先找出基本值
			name[0] = 'B';
			baseValue = GetValue(name.c_str());
			// 再找出改变值
			name[0] = 'E';
		}
		playeritr = m_mapNameValue.find(name);
		if (playeritr!=m_mapNameValue.end())
		{
			lNum++;
			// 属性类型
			::_AddToByteArray(bPlayerArray, (WORD)playeritr->second.attrEnumValue);

			if("Money" == name)
			{
				::_AddToByteArray(bPlayerArray, (BYTE)ATTR_DATA_DWORD);
				::_AddToByteArray(bPlayerArray, (WORD)sizeof(DWORD));
				::_AddToByteArray(bPlayerArray, GetMoney());
				continue;
			}

			long sendValue = INVILID_VALUE;
			switch (playeritr->second.lType)
			{
			case 0:	// byte
				::_AddToByteArray(bPlayerArray, (BYTE)ATTR_DATA_BYTE);
				::_AddToByteArray(bPlayerArray, (WORD)sizeof(BYTE));
				if((*pStrItr)[0] == 'C' && (*pStrItr)[1] == '_') // 最终值
				{
					BYTE EValue = *(BYTE*)(playeritr->second.plValue);
					sendValue = GetCurrentValue((char*)playeritr->first.c_str(), playeritr->second, EValue, baseValue);
				}
				else
					sendValue = (long)*(BYTE*)(playeritr->second.plValue);

				::_AddToByteArray(bPlayerArray, (BYTE)sendValue);

				break;
			case 1:	// word
				::_AddToByteArray(bPlayerArray, (BYTE)ATTR_DATA_WORD);
				::_AddToByteArray(bPlayerArray, (WORD)sizeof(WORD));
				if(((*pStrItr)[0] == 'C' && (*pStrItr)[1] == '_')) // 改变值) // 改变值
				{
					WORD EValue = *(WORD*)(playeritr->second.pwValue);
					sendValue = GetCurrentValue((char*)playeritr->first.c_str(), playeritr->second, EValue, baseValue);
				}
				else
					sendValue = (long)*(WORD*)(playeritr->second.pwValue);
				::_AddToByteArray(bPlayerArray, (WORD)sendValue);
				break;
			case 2:	// dword
				::_AddToByteArray(bPlayerArray, (BYTE)ATTR_DATA_DWORD);
				::_AddToByteArray(bPlayerArray, (WORD)sizeof(DWORD));
				if(((*pStrItr)[0] == 'C' && (*pStrItr)[1] == '_')) // 改变值) // 改变值
				{
					DWORD EValue = *(DWORD*)(playeritr->second.pdwValue);
					sendValue = GetCurrentValue((char*)playeritr->first.c_str(), playeritr->second, EValue, baseValue);
				}
				else
					sendValue = (long)*(DWORD*)(playeritr->second.pdwValue);
				::_AddToByteArray(bPlayerArray, (DWORD)sendValue);
				break;
			case 3:	// float
				::_AddToByteArray(bPlayerArray, (BYTE)ATTR_DATA_FLOAT);
				::_AddToByteArray(bPlayerArray, (WORD)sizeof(float));
				if(((*pStrItr)[0] == 'C' && (*pStrItr)[1] == '_')) // 改变值) // 改变值
				{
					float EValue = *(float*)(playeritr->second.pfValue);
					sendValue = GetCurrentValue((char*)playeritr->first.c_str(), playeritr->second, (long)EValue, baseValue);
				}
				else
					sendValue = (long)*(float*)(playeritr->second.pfValue);
				::_AddToByteArray(bPlayerArray, (float)sendValue);
				break;
			case 4:	// long
				::_AddToByteArray(bPlayerArray, (BYTE)ATTR_DATA_LONG);
				::_AddToByteArray(bPlayerArray, (WORD)sizeof(long));
				if(((*pStrItr)[0] == 'C' && (*pStrItr)[1] == '_')) // 改变值) // 改变值
				{
					long EValue = (long)*(long*)(playeritr->second.pValue);
					sendValue = GetCurrentValue((char*)playeritr->first.c_str(), playeritr->second, EValue, baseValue);
				}
				else
					sendValue = (long)*(long*)(playeritr->second.pValue);
				::_AddToByteArray(bPlayerArray, (long)sendValue);
				break;
			case 5:	// short
				::_AddToByteArray(bPlayerArray, (BYTE)ATTR_DATA_SHORT);
				::_AddToByteArray(bPlayerArray, (WORD)sizeof(short));
				if(((*pStrItr)[0] == 'C' && (*pStrItr)[1] == '_')) // 改变值) // 改变值
				{
					short EValue = *(short*)(playeritr->second.psValue);
					sendValue = GetCurrentValue((char*)playeritr->first.c_str(), playeritr->second, EValue, baseValue);
				}
				else
					sendValue = (long)*(short*)(playeritr->second.psValue);
				::_AddToByteArray(bPlayerArray, (short)sendValue);
				break;
			case 6:	// bool
				::_AddToByteArray(bPlayerArray, (BYTE)ATTR_DATA_BOOL);
				::_AddToByteArray(bPlayerArray, (WORD)sizeof(bool));
				if((name[0] == 'E' && name[1] == '_')) // 改变值) // 改变值
				{
					bool EValue = *(bool*)(playeritr->second.pbValue);
					sendValue = GetCurrentValue((char*)playeritr->first.c_str(), playeritr->second, EValue, baseValue);
				}
				else
					sendValue = (long)*(bool*)(playeritr->second.pbValue);
				::_AddToByteArray(bPlayerArray, (BYTE)sendValue);
				break;
			default:
				break;
			}
		}
	}
	if(lShapeNum == 0 && lNum==0)
		return false;

	// shape属性个数
	setWriteDB.AddToByteArray( (WORD)lShapeNum);
	if(vecData.size() > 0)
		setWriteDB.AddToByteArray(&vecData[0],(long)vecData.size());
	// Player属性个数
	setWriteDB.AddToByteArray( (WORD)lNum);
	if(vecPlayerData.size() > 0)
		setWriteDB.AddToByteArray(&vecPlayerData[0],(long)vecPlayerData.size());
	return true;
}

bool CPlayer::CodeDirtyAttrToDataBlock(set<string>& dirtyAttr, DBWriteSet& setWriteDB)
{
	vector<BYTE> vecData;vecData.clear();
	vector<BYTE> *bArray = &vecData;
	vector<BYTE> vecPlayerData;vecPlayerData.clear();
	vector<BYTE> *bPlayerArray = &vecPlayerData;
	long lShapeNum =0;
	long lNum =0;	
	mapNameValueItr shapeitr;
	mapNameValueItr playeritr;
	long baseValue = 0;
	string name = "";
	set<string>::iterator pStrItr = dirtyAttr.begin();
	for(; pStrItr!= dirtyAttr.end(); pStrItr++)
	{
		name = *pStrItr;

		shapeitr = m_mapShapeNameValue.find(name);
		if (shapeitr!=m_mapShapeNameValue.end())
		{
			lShapeNum++;
			// 属性类型
			_AddToByteArray(bArray, (WORD)shapeitr->second.attrEnumValue);

			long sendValue = INVILID_VALUE;
			switch (shapeitr->second.lType)
			{
			case 0:	// byte
				::_AddToByteArray(bArray, (BYTE)ATTR_DATA_BYTE);
				::_AddToByteArray(bArray, (WORD)sizeof(BYTE));
				if((*pStrItr)[0] == 'C' && (*pStrItr)[1] == '_') // 最终值
				{
					BYTE EValue = *(BYTE*)(shapeitr->second.plValue);
					sendValue = GetCurrentValue((char*)shapeitr->first.c_str(), shapeitr->second, EValue, baseValue);
				}
				else
					sendValue = (long)*(BYTE*)(shapeitr->second.plValue);

				::_AddToByteArray(bArray, (BYTE)sendValue);

				break;
			case 1:	// word
				::_AddToByteArray(bArray, (BYTE)ATTR_DATA_WORD);
				::_AddToByteArray(bArray, (WORD)sizeof(WORD));
				if(((*pStrItr)[0] == 'C' && (*pStrItr)[1] == '_')) // 改变值) // 改变值
				{
					WORD EValue = *(WORD*)(shapeitr->second.pwValue);
					sendValue = GetCurrentValue((char*)shapeitr->first.c_str(), shapeitr->second, EValue, baseValue);
				}
				else
					sendValue = (long)*(WORD*)(shapeitr->second.pwValue);
				::_AddToByteArray(bArray, (WORD)sendValue);
				break;
			case 2:	// dword
				::_AddToByteArray(bArray, (BYTE)ATTR_DATA_DWORD);
				::_AddToByteArray(bArray, (WORD)sizeof(DWORD));
				if(((*pStrItr)[0] == 'C' && (*pStrItr)[1] == '_')) // 改变值) // 改变值
				{
					DWORD EValue = *(DWORD*)(shapeitr->second.pdwValue);
					sendValue = GetCurrentValue((char*)shapeitr->first.c_str(), shapeitr->second, EValue, baseValue);
				}
				else
					sendValue = (long)*(DWORD*)(shapeitr->second.pdwValue);
				::_AddToByteArray(bArray, (DWORD)sendValue);
				break;
			case 3:	// float
				::_AddToByteArray(bArray, (BYTE)ATTR_DATA_FLOAT);
				::_AddToByteArray(bArray, (WORD)sizeof(float));
				if(((*pStrItr)[0] == 'C' && (*pStrItr)[1] == '_')) // 改变值) // 改变值
				{
					float EValue = *(float*)(shapeitr->second.pfValue);
					sendValue = GetCurrentValue((char*)shapeitr->first.c_str(), shapeitr->second, (long)EValue, baseValue);
				}
				else
					sendValue = (long)*(float*)(shapeitr->second.pfValue);
				::_AddToByteArray(bArray, (float)sendValue);
				break;
			case 4:	// long
				::_AddToByteArray(bArray, (BYTE)ATTR_DATA_LONG);
				::_AddToByteArray(bArray, (WORD)sizeof(long));
				if(((*pStrItr)[0] == 'C' && (*pStrItr)[1] == '_')) // 改变值) // 改变值
				{
					long EValue = (long)*(long*)(shapeitr->second.pValue);
					sendValue = GetCurrentValue((char*)shapeitr->first.c_str(), shapeitr->second, EValue, baseValue);
				}
				else
					sendValue = (long)*(long*)(shapeitr->second.pValue);
				::_AddToByteArray(bArray, (long)sendValue);
				break;
			case 5:	// short
				::_AddToByteArray(bArray, (BYTE)ATTR_DATA_SHORT);
				::_AddToByteArray(bArray, (WORD)sizeof(short));
				if(((*pStrItr)[0] == 'C' && (*pStrItr)[1] == '_')) // 改变值) // 改变值
				{
					short EValue = *(short*)(shapeitr->second.psValue);
					sendValue = GetCurrentValue((char*)shapeitr->first.c_str(), shapeitr->second, EValue, baseValue);
				}
				else
					sendValue = (long)*(short*)(shapeitr->second.psValue);
				::_AddToByteArray(bArray, (short)sendValue);
				break;
			case 6:	// bool
				::_AddToByteArray(bArray, (BYTE)ATTR_DATA_BOOL);
				::_AddToByteArray(bArray, (WORD)sizeof(bool));
				if((name[0] == 'E' && name[1] == '_')) // 改变值) // 改变值
				{
					bool EValue = *(bool*)(shapeitr->second.pbValue);
					sendValue = GetCurrentValue((char*)shapeitr->first.c_str(), shapeitr->second, EValue, baseValue);
				}
				else
					sendValue = (long)*(bool*)(shapeitr->second.pbValue);
				::_AddToByteArray(bArray, (BYTE)sendValue);
				break;
			default:
				break;
			}
		}

		if(name[0] == 'C' && name[1] == '_')
		{	
			// 先找出基本值
			name[0] = 'B';
			baseValue = GetValue(name.c_str());
			// 再找出改变值
			name[0] = 'E';
		}
		playeritr = m_mapNameValue.find(name);
		if (playeritr!=m_mapNameValue.end())
		{
			lNum++;
			// 属性类型
			::_AddToByteArray(bPlayerArray, (WORD)playeritr->second.attrEnumValue);

			if("Money" == name)
			{
				::_AddToByteArray(bPlayerArray, (BYTE)ATTR_DATA_DWORD);
				::_AddToByteArray(bPlayerArray, (WORD)sizeof(DWORD));
				::_AddToByteArray(bPlayerArray, GetMoney());
				continue;
			}

			long sendValue = INVILID_VALUE;
			switch (playeritr->second.lType)
			{
			case 0:	// byte
				::_AddToByteArray(bPlayerArray, (BYTE)ATTR_DATA_BYTE);
				::_AddToByteArray(bPlayerArray, (WORD)sizeof(BYTE));
				if((*pStrItr)[0] == 'C' && (*pStrItr)[1] == '_') // 最终值
				{
					BYTE EValue = *(BYTE*)(playeritr->second.plValue);
					sendValue = GetCurrentValue((char*)playeritr->first.c_str(), playeritr->second, EValue, baseValue);
				}
				else
					sendValue = (long)*(BYTE*)(playeritr->second.plValue);

				::_AddToByteArray(bPlayerArray, (BYTE)sendValue);

				break;
			case 1:	// word
				::_AddToByteArray(bPlayerArray, (BYTE)ATTR_DATA_WORD);
				::_AddToByteArray(bPlayerArray, (WORD)sizeof(WORD));
				if(((*pStrItr)[0] == 'C' && (*pStrItr)[1] == '_')) // 改变值) // 改变值
				{
					WORD EValue = *(WORD*)(playeritr->second.pwValue);
					sendValue = GetCurrentValue((char*)playeritr->first.c_str(), playeritr->second, EValue, baseValue);
				}
				else
					sendValue = (long)*(WORD*)(playeritr->second.pwValue);
				::_AddToByteArray(bPlayerArray, (WORD)sendValue);
				break;
			case 2:	// dword
				::_AddToByteArray(bPlayerArray, (BYTE)ATTR_DATA_DWORD);
				::_AddToByteArray(bPlayerArray, (WORD)sizeof(DWORD));
				if(((*pStrItr)[0] == 'C' && (*pStrItr)[1] == '_')) // 改变值) // 改变值
				{
					DWORD EValue = *(DWORD*)(playeritr->second.pdwValue);
					sendValue = GetCurrentValue((char*)playeritr->first.c_str(), playeritr->second, EValue, baseValue);
				}
				else
					sendValue = (long)*(DWORD*)(playeritr->second.pdwValue);
				::_AddToByteArray(bPlayerArray, (DWORD)sendValue);
				break;
			case 3:	// float
				::_AddToByteArray(bPlayerArray, (BYTE)ATTR_DATA_FLOAT);
				::_AddToByteArray(bPlayerArray, (WORD)sizeof(float));
				if(((*pStrItr)[0] == 'C' && (*pStrItr)[1] == '_')) // 改变值) // 改变值
				{
					float EValue = *(float*)(playeritr->second.pfValue);
					sendValue = GetCurrentValue((char*)playeritr->first.c_str(), playeritr->second, (long)EValue, baseValue);
				}
				else
					sendValue = (long)*(float*)(playeritr->second.pfValue);
				::_AddToByteArray(bPlayerArray, (float)sendValue);
				break;
			case 4:	// long
				::_AddToByteArray(bPlayerArray, (BYTE)ATTR_DATA_LONG);
				::_AddToByteArray(bPlayerArray, (WORD)sizeof(long));
				if(((*pStrItr)[0] == 'C' && (*pStrItr)[1] == '_')) // 改变值) // 改变值
				{
					long EValue = (long)*(long*)(playeritr->second.pValue);
					sendValue = GetCurrentValue((char*)playeritr->first.c_str(), playeritr->second, EValue, baseValue);
				}
				else
					sendValue = (long)*(long*)(playeritr->second.pValue);
				::_AddToByteArray(bPlayerArray, (long)sendValue);
				break;
			case 5:	// short
				::_AddToByteArray(bPlayerArray, (BYTE)ATTR_DATA_SHORT);
				::_AddToByteArray(bPlayerArray, (WORD)sizeof(short));
				if(((*pStrItr)[0] == 'C' && (*pStrItr)[1] == '_')) // 改变值) // 改变值
				{
					short EValue = *(short*)(playeritr->second.psValue);
					sendValue = GetCurrentValue((char*)playeritr->first.c_str(), playeritr->second, EValue, baseValue);
				}
				else
					sendValue = (long)*(short*)(playeritr->second.psValue);
				::_AddToByteArray(bPlayerArray, (short)sendValue);
				break;
			case 6:	// bool
				::_AddToByteArray(bPlayerArray, (BYTE)ATTR_DATA_BOOL);
				::_AddToByteArray(bPlayerArray, (WORD)sizeof(bool));
				if((name[0] == 'E' && name[1] == '_')) // 改变值) // 改变值
				{
					bool EValue = *(bool*)(playeritr->second.pbValue);
					sendValue = GetCurrentValue((char*)playeritr->first.c_str(), playeritr->second, EValue, baseValue);
				}
				else
					sendValue = (long)*(bool*)(playeritr->second.pbValue);
				::_AddToByteArray(bPlayerArray, (BYTE)sendValue);
				break;
			default:
				break;
			}
		}
	}
	if(lShapeNum == 0 && lNum==0)
		return false;

	// shape属性个数
	setWriteDB.AddToByteArray( (WORD)lShapeNum);
	if(vecData.size() > 0)
		setWriteDB.AddToByteArray(&vecData[0],(long)vecData.size());
	// Player属性个数
	setWriteDB.AddToByteArray( (WORD)lNum);
	if(vecPlayerData.size() > 0)
		setWriteDB.AddToByteArray(&vecPlayerData[0],(long)vecPlayerData.size());
	return true;
}

long CPlayer::UpdateAttributeToClient()
{
	if(m_mapDirtyAttrData.size() == 0)
		return false;

	CMessage msg( MSG_S2C_SHAPE_CHANGE_PROPERTIES );
	msg.Add(GetType());
	msg.Add(GetExID());
	DBWriteSet setWriteDB;
	msg.GetDBWriteSet(setWriteDB);
	CodeChangeAttrToDataBlock(setWriteDB);
	msg.SendToPlayer(GetExID(),false);

	GSTeam *pTeam = (GSTeam*)GetOrganiCtrl()->GetGameTeamByID(GetTeamID());
	if(NULL != pTeam)
	{
		if(m_mapDirtyAttrData.end() != m_mapDirtyAttrData.find("Hp"))
		{
			pTeam->OnPlayerIdioinfoChange(GetExID(), eMIS_CurrHp);
		}
		if(m_mapDirtyAttrData.end() != m_mapDirtyAttrData.find("Mp"))
		{
			pTeam->OnPlayerIdioinfoChange(GetExID(), eMIS_CurrMp);
		}
		if(m_mapDirtyAttrData.end() != m_mapDirtyAttrData.find("C_MaxHp"))
		{
			pTeam->OnPlayerIdioinfoChange(GetExID(), eMIS_MaxHp);
		}
		if(m_mapDirtyAttrData.end() != m_mapDirtyAttrData.find("C_MaxMp"))
		{
			pTeam->OnPlayerIdioinfoChange(GetExID(), eMIS_MaxMp);
		}
	}
	m_mapDirtyAttrData.clear();
	return true;
}

long  CPlayer::UpdateAttribute(BOOL bImmediately)
{
	if(!IsEnterGame()) return false;

	if(!bImmediately)
	{
		RegisterUpdateAttrTimer();
		return true;
	}

	UpdateAttributeToClient();	
	UpdateOtherAttributeToClient();

	return true;
}

long CPlayer::UpdateOtherAttributeToClient()
{
	//向周围玩家更新属性
	if(m_mapOtherDirtyAttrData.size() > 0)
	{
		set<string>	setDirtyAttr;
		set<string>::iterator it = m_mapOtherDirtyAttrData.find("C_MaxHp");
		if( it != m_mapOtherDirtyAttrData.end() )
		{
			m_mapOtherDirtyAttrData.erase(it);
			setDirtyAttr.insert("C_MaxHp");
		}
		it = m_mapOtherDirtyAttrData.find("Hp");
		if( it != m_mapOtherDirtyAttrData.end() )
		{
			m_mapOtherDirtyAttrData.erase(it);
			setDirtyAttr.insert("Hp");
		}
		// 添加更新PK CP值
		it = m_mapOtherDirtyAttrData.find( "CurPKCP" );
		if( it != m_mapOtherDirtyAttrData.end() )
		{
			m_mapOtherDirtyAttrData.erase( it );
			setDirtyAttr.insert( "CurPKCP" );
		}
		it = m_mapOtherDirtyAttrData.find( "MaxPKCP" );
		if( it != m_mapOtherDirtyAttrData.end() )
		{
			m_mapOtherDirtyAttrData.erase( it );
			setDirtyAttr.insert( "MaxPKCP" );
		}

		if(setDirtyAttr.size() > 0 && GetSelectSelfPlayer().size() > 0)
		{
			UpdateHPMPToClient(setDirtyAttr);
		}

		if(m_mapOtherDirtyAttrData.size() > 0)
		{
			CMessage msgOther( MSG_S2C_SHAPE_CHANGE_PROPERTIES );
			msgOther.Add(GetType());
			msgOther.Add(GetExID());
			DBWriteSet setWriteDBOther;
			msgOther.GetDBWriteSet(setWriteDBOther);
			if( CodeOtherUpdateAttrToDataBlock(setWriteDBOther) )
				msgOther.SendToAround(this,this);
		}

		GSTeam *pTeam = (GSTeam*)GetOrganiCtrl()->GetGameTeamByID(GetTeamID());
		if(NULL != pTeam)
		{
			if(m_mapDirtyAttrData.end() != m_mapDirtyAttrData.find("Hp"))
			{
				pTeam->OnPlayerIdioinfoChange(GetExID(), eMIS_CurrHp);
			}
			if(m_mapDirtyAttrData.end() != m_mapDirtyAttrData.find("Mp"))
			{
				pTeam->OnPlayerIdioinfoChange(GetExID(), eMIS_CurrMp);
			}
			if(m_mapDirtyAttrData.end() != m_mapDirtyAttrData.find("C_MaxHp"))
			{
				pTeam->OnPlayerIdioinfoChange(GetExID(), eMIS_MaxHp);
			}
			if(m_mapDirtyAttrData.end() != m_mapDirtyAttrData.find("C_MaxMp"))
			{
				pTeam->OnPlayerIdioinfoChange(GetExID(), eMIS_MaxMp);
			}
		}

		m_mapOtherDirtyAttrData.clear();
	}
	return true;
}

long  CPlayer::UpdateHPMPToClient(set<string>& dirtyAttr)
{
	CMessage msgOther( MSG_S2C_SHAPE_CHANGE_PROPERTIES );
	msgOther.Add(GetType());
	msgOther.Add(GetExID());
	DBWriteSet setWriteDBOther;
	msgOther.GetDBWriteSet(setWriteDBOther);
	if( CodeDirtyAttrToDataBlock(dirtyAttr,setWriteDBOther) )
		msgOther.SendToPlayers(GetSelectSelfPlayer());
	return true;
}

long  CPlayer::OnUpdateAttributeTimeOut()
{
	UpdateAttributeToClient();
	UpdateOtherAttributeToClient();
	return true;
}


void CPlayer::OnSuitNumChanged(long suit_id,int suit_num)
{	
	map<long,vector<tagSuitGoods>*>::iterator vecIter=m_mSuitState.find(suit_id);
	if(vecIter!=m_mSuitState.end())
	{
		for(long i=0;i< (long) vecIter->second->size();i++)
		{
			CGoods* pGoods=(*(vecIter->second))[i].pGoods;
			pGoods->OnSuitNumChanged(this,suit_num);
		}
	}	
}

void CPlayer::AllEquipmentRefineUpdate(long lVal)
{
	//更新身上所有装备的炼度
	CGoods* pEquip=NULL;
	for(DWORD dwPos=EC_E_HEAD;dwPos<=EC_E_RIDER;dwPos++)
	{
		pEquip=m_cEquipment.GetGoods(dwPos);
		if(pEquip)
		{
			//必须含有炼度这条属性
			if(pEquip->HasAddonProperty(GAP_REFINE))
			{
				DWORD dwOldVal=pEquip->GetAddonPropertyValues(GAP_REFINE,1);
				pEquip->AddRefine(lVal);
				DWORD dwNewVal=pEquip->GetAddonPropertyValues(GAP_REFINE,1);
				DWORD dwMaxVal=pEquip->GetAddonPropertyValues(GAP_REFINE,2);
				if( (dwOldVal / 100 != dwNewVal / 100) || (dwOldVal!=dwMaxVal && dwNewVal==dwMaxVal) )
				{
					CMessage msg(MSG_S2C_EQUIP_REFINE_UPDATE);
					msg.Add((BYTE)dwPos);
					msg.Add(dwNewVal);
					msg.SendToPlayer(GetExID(),false);
				}
			}
		}
	}
}

BOOL CPlayer::DropGoods( CS2CContainerObjectMove::PLAYER_EXTEND_ID pei, const CGUID& guid )
{
	BOOL bResult = FALSE;
	CServerRegion* pRegion = dynamic_cast<CServerRegion*>( GetFather() );
	if( pRegion )
	{
		CGoodsContainer* pContainer = NULL;
		switch( pei )
		{
		case CS2CContainerObjectMove::PEI_PACKET:
			pContainer = getOriginPack();
			break;
		case CS2CContainerObjectMove::PEI_EQUIPMENT:
			pContainer = &m_cEquipment;
			break;
		case CS2CContainerObjectMove::PEI_WALLET:
			pContainer = &m_pPackage.GetWallet();
			break;
		case CS2CContainerObjectMove::PEI_GEM:
			pContainer = &m_pPackage.GetGem();
			break;
        case CS2CContainerObjectMove::PEI_EQUIP_GOD:
            pContainer = &m_cEquipmentGod;
            break;
        case CS2CContainerObjectMove::PEI_EQUIP_LAND:
            pContainer = &m_cEquipmentLand;
            break;
        case CS2CContainerObjectMove::PEI_EQUIP_HUMAN:
            pContainer = &m_cEquipmentHuman;
            break;
		case CS2CContainerObjectMove::PEI_PACK1:
			pContainer = m_pPackage.GetSubpack(0)->pSubpackContainer;
			break;
		case CS2CContainerObjectMove::PEI_PACK2:
			pContainer = m_pPackage.GetSubpack(1)->pSubpackContainer;
			break;
		case CS2CContainerObjectMove::PEI_PACK3:
			pContainer = m_pPackage.GetSubpack(2)->pSubpackContainer;
			break;
		case CS2CContainerObjectMove::PEI_PACK4:
			pContainer = m_pPackage.GetSubpack(3)->pSubpackContainer;
			break;
		case CS2CContainerObjectMove::PEI_PACK5:
			pContainer = m_pPackage.GetSubpack(4)->pSubpackContainer;
			break;
		}
		if( pContainer )
		{
			CS2CContainerObjectMove comMsg;
			CGoods* pGoods = dynamic_cast<CGoods*>( pContainer -> Remove(TYPE_GOODS, guid, &comMsg) );
			if( pGoods )
			{
				LONG lX = GetTileX();
				LONG lY = GetTileY();
				LONG lDropGoodsStartPos = 0;
				lDropGoodsStartPos = pRegion -> GetDropGoodsPos( lX, lY, lX, lY, lDropGoodsStartPos );
				if( lDropGoodsStartPos != -1 )
				{
					DWORD dwDestinationContainerPosition = lX + pRegion -> GetWidth() * lY;
					pGoods -> SetPosXY( lX+0.5f, lY+0.5f );

					pRegion -> AddObject( pGoods );
					CArea* pArea = pRegion->GetArea(lX/AREA_WIDTH, lY/AREA_HEIGHT);
					if(pArea)
					{
						pArea->AddGoodsDelTime(pGoods);
					}
					comMsg.SetOperation( CS2CContainerObjectMove::OT_MOVE_OBJECT );
					comMsg.SetSourceContainerExtendID( pei );
					comMsg.SetDestinationContainer( pRegion -> GetType(), pRegion -> GetExID(), dwDestinationContainerPosition );
					comMsg.Send( GetExID() );

					CMessage MsgSend( MSG_S2C_RGN_ADDSHAPE,pGoods->GetExID());
					MsgSend.Add( pGoods->GetType() );
					MsgSend.Add( pGoods->GetExID() );
					DBWriteSet setWriteDB;
					MsgSend.GetDBWriteSet(setWriteDB);
					pGoods->SerializeForRgnDisplay(setWriteDB);

					MsgSend.SendToAround(pGoods);

					bResult = TRUE;
				}
			}
		}
	}
	return bResult;
}

BOOL CPlayer::AddGoodsWithProtect(CGoods* pGoods,BOOL bProtect, LONG *pOptRerult)
{	
	//! 修改这个接口的时候同时修改下下面的AddAndReturnGoodsWithProtect接口

	if(!pGoods)
		return FALSE;
	DWORD dwGoodsId=pGoods->GetBasePropertiesIndex();
	BOOL bAdded=FALSE;	
	DWORD dwCell = 0;
	if(getOriginPack()->FindPositionForGoods(pGoods, dwCell))
	{
		CGoods* pExistedGoods =getOriginPack()->GetGoods( dwCell );
		if( pExistedGoods == NULL )
		{
			CS2CContainerObjectMove comMsg;
			if(getOriginPack()->Add(dwCell,pGoods, NULL, &comMsg) )
			{
				GetGame()->UpdatePlayerPack(this,CS2CContainerObjectMove::PEI_PACKET,0);
				OnGetGoods(dwGoodsId,pGoods);
				comMsg.SetOperation( CS2CContainerObjectMove::OT_NEW_OBJECT );
				comMsg.SetDestinationContainerExtendID( CS2CContainerObjectMove::PEI_PACKET );
				pGoods-> SerializeForOldClient( &comMsg.GetObjectStream() );								
				comMsg.Send(GetExID() );
				bAdded=TRUE;				
				if(NULL != pOptRerult) *pOptRerult = 1;
			}
		}
		else
		{
			if(getOriginPack()->Add(dwCell,pGoods, NULL) )
			{
				GetGame()->UpdatePlayerPack(this,CS2CContainerObjectMove::PEI_PACKET,0);
				OnGetGoods(dwGoodsId);
				CS2CContainerObjectAmountChange coacMsg;
				coacMsg.SetSourceContainer( TYPE_PLAYER, GetExID(), dwCell );
				coacMsg.SetSourceContainerExtendID( CS2CContainerObjectMove::PEI_PACKET );
				coacMsg.SetObject( pExistedGoods -> GetType(), pExistedGoods -> GetExID() );
				coacMsg.SetObjectAmount( pExistedGoods -> GetAmount() );
				coacMsg.Send(GetExID() );
				bAdded=TRUE;				
				if(NULL != pOptRerult) *pOptRerult = 2;
			}
		}
		GetGame()->UpdatePlayerPack(this,CS2CContainerObjectMove::PEI_PACKET,dwCell);
	}
    else
    {
        for(long l=CS2CContainerObjectMove::PEI_PACK1; l<=CS2CContainerObjectMove::PEI_PACK5; l++)
        {
            int nSub = l - CS2CContainerObjectMove::PEI_PACK1;
            CVolumeLimitGoodsContainer* pContainer = GetPackage()->GetSubpack(nSub)->pSubpackContainer;
            CGoods* pExistedGoods = pContainer->GetGoods(dwCell);
            if( pExistedGoods == NULL )
            {
                CS2CContainerObjectMove comMsg;
                if(pContainer->Add(dwCell, pGoods, NULL, &comMsg))
                {
                    GetGame()->UpdatePlayerPack(this, l, 0);
                    OnGetGoods(dwGoodsId, pGoods);
                    comMsg.SetOperation( CS2CContainerObjectMove::OT_NEW_OBJECT );
                    comMsg.SetDestinationContainerExtendID(l);
                    pGoods->SerializeForOldClient(&comMsg.GetObjectStream());
                    comMsg.Send(GetExID());
                    bAdded = TRUE;
                    if(NULL != pOptRerult)
                        *pOptRerult = 1;
                }
            }
            else
            {
                if(pContainer->Add(dwCell,pGoods, NULL) )
                {
                    GetGame()->UpdatePlayerPack(this, l, 0);
                    OnGetGoods(dwGoodsId);
                    CS2CContainerObjectAmountChange coacMsg;
                    coacMsg.SetSourceContainer( TYPE_PLAYER, GetExID(), dwCell );
                    coacMsg.SetSourceContainerExtendID( l );
                    coacMsg.SetObject( pExistedGoods -> GetType(), pExistedGoods -> GetExID() );
                    coacMsg.SetObjectAmount( pExistedGoods -> GetAmount() );
                    coacMsg.Send(GetExID() );
                    bAdded=TRUE;
                    if(NULL != pOptRerult) 
                        *pOptRerult = 2;
                }
            }
            GetGame()->UpdatePlayerPack(this, nSub, dwCell);
            if(bAdded)
                break;
        }
    }

	if(!bAdded && bProtect)
	{
		//添加到场景并设置保护时间
		CServerRegion* pRegion=dynamic_cast<CServerRegion*>(GetFather());
		if(pRegion)
		{
			long x=0, y=0;
			long lDropGoodsStartPos=0;

			lDropGoodsStartPos= pRegion->GetDropGoodsPos(GetTileX(), GetTileY(), x, y, lDropGoodsStartPos);
			if( lDropGoodsStartPos != -1 )
			{
				pGoods->SetPosXY(x+0.5f, y+0.5f);
				pRegion->AddObject( pGoods);				
				pRegion -> SetGoodsProtection( this, pGoods);				
				// 通知每个玩家
				/*
				CS2CContainerObjectMove comMsg;
				comMsg.SetOperation( CS2CContainerObjectMove::OT_NEW_OBJECT );
				comMsg.SetDestinationContainer( pRegion -> GetType(), 
				pRegion -> GetExID(), y * pRegion -> GetWidth() + x );
				comMsg.SetDestinationObject(pGoods -> GetType(),pGoods-> GetExID() );
				comMsg.SetDestinationObjectAmount(pGoods-> GetAmount() );
				pGoods-> SerializeForOldClient( &( comMsg.GetObjectStream() ) );
				comMsg.SendToAround( this );
				comMsg.Send(GetExID());*/
				CMessage MsgSend( MSG_S2C_RGN_ADDSHAPE,pGoods->GetExID());
				MsgSend.Add( pGoods->GetType() );
				MsgSend.Add( pGoods->GetExID() );
				DBWriteSet setWriteDB;
				MsgSend.GetDBWriteSet(setWriteDB);
				pGoods->SerializeForRgnDisplay(setWriteDB);

				MsgSend.SendToAround(pGoods);
				if(NULL != pOptRerult) *pOptRerult = 1;
			}
		}
	}
	return bAdded;	
}

BOOL CPlayer::AddAndReturnGoodsWithProtect(CGoods **ppGoods,BOOL bProtect, LONG *pOptRerult)
{	
	if(NULL == ppGoods)
		return FALSE;
	CGoods *pGoods = *ppGoods;
	if(!pGoods)
		return FALSE;
	DWORD dwGoodsId=pGoods->GetBasePropertiesIndex();
	BOOL bAdded=FALSE;	
	DWORD dwCell = 0;
	if(getOriginPack()->FindPositionForGoods(pGoods, dwCell))
	{
		CGoods* pExistedGoods =getOriginPack()->GetGoods( dwCell );
		if( pExistedGoods == NULL )
		{
			CS2CContainerObjectMove comMsg;
			if(getOriginPack()->Add(dwCell,pGoods, NULL, &comMsg) )
			{
				OnGetGoods(dwGoodsId,pGoods);
				comMsg.SetOperation( CS2CContainerObjectMove::OT_NEW_OBJECT );
				comMsg.SetDestinationContainerExtendID( CS2CContainerObjectMove::PEI_PACKET );
				pGoods-> SerializeForOldClient( &comMsg.GetObjectStream() );								
				comMsg.Send(GetExID() );
				bAdded=TRUE;				
				if(NULL != pOptRerult) *pOptRerult = 1;
			}
		}
		else
		{
			if(getOriginPack()->Add(dwCell,pGoods, NULL) )
			{
				OnGetGoods(dwGoodsId);
				CS2CContainerObjectAmountChange coacMsg;
				coacMsg.SetSourceContainer( TYPE_PLAYER, GetExID(), dwCell );
				coacMsg.SetSourceContainerExtendID( CS2CContainerObjectMove::PEI_PACKET );
				coacMsg.SetObject( pExistedGoods -> GetType(), pExistedGoods -> GetExID() );
				coacMsg.SetObjectAmount( pExistedGoods -> GetAmount() );
				coacMsg.Send(GetExID() );
				bAdded=TRUE;				
				if(NULL != pOptRerult) *pOptRerult = 2;
				CGoods *pTmpGoods = getOriginPack()->GetGoods(dwCell);
				if(NULL != pTmpGoods)*ppGoods = pTmpGoods;
			}
		}
    }
    else
    {
        for(long l=CS2CContainerObjectMove::PEI_PACK1; l<=CS2CContainerObjectMove::PEI_PACK5; l++)
        {
            int nSub = l - CS2CContainerObjectMove::PEI_PACK1;
            CVolumeLimitGoodsContainer* pContainer=GetPackage()->GetSubpack(nSub)->pSubpackContainer;
            if(pContainer && pContainer->FindPositionForGoods(pGoods, dwCell))
            {
                CGoods* pExistedGoods =pContainer->GetGoods( dwCell );
                if( pExistedGoods == NULL )
                {
                    CS2CContainerObjectMove comMsg;
                    if(pContainer->Add(dwCell,pGoods, NULL, &comMsg) )
                    {
                        OnGetGoods(dwGoodsId,pGoods);
                        comMsg.SetOperation( CS2CContainerObjectMove::OT_NEW_OBJECT );
                        comMsg.SetDestinationContainerExtendID( l );
                        pGoods-> SerializeForOldClient( &comMsg.GetObjectStream() );								
                        comMsg.Send(GetExID() );
                        bAdded=TRUE;
                        //vOut[i] = NULL;
                        if(NULL != pOptRerult) *pOptRerult = 1;
                    }
                }
                else
                {
                    if(pContainer->Add(dwCell,pGoods, NULL) )
                    {
                        OnGetGoods(dwGoodsId);
                        CS2CContainerObjectAmountChange coacMsg;
                        coacMsg.SetSourceContainer( TYPE_PLAYER, GetExID(), dwCell );
                        coacMsg.SetSourceContainerExtendID( l );
                        coacMsg.SetObject( pExistedGoods -> GetType(), pExistedGoods->GetExID() );
                        coacMsg.SetObjectAmount( pExistedGoods -> GetAmount() );
                        coacMsg.Send(GetExID() );
                        bAdded=TRUE;
                        //vOut[i] = NULL;
                        if(NULL != pOptRerult) *pOptRerult = 2;
                        CGoods *pTmpGoods = pContainer->GetGoods(dwCell);
                        if(NULL != pTmpGoods)*ppGoods = pTmpGoods;
                    }
                }
                if(bAdded)
                    break;
            }            
        }
    }

	if(!bAdded && bProtect)
	{
		//添加到场景并设置保护时间
		CServerRegion* pRegion=dynamic_cast<CServerRegion*>(GetFather());
		if(pRegion)
		{
			long x=0, y=0;
			long lDropGoodsStartPos=0;

			lDropGoodsStartPos= pRegion->GetDropGoodsPos(GetTileX(), GetTileY(), x, y, lDropGoodsStartPos);
			if( lDropGoodsStartPos != -1 )
			{
				pGoods->SetPosXY(x+0.5f, y+0.5f);
				pRegion->AddObject( pGoods);				
				pRegion -> SetGoodsProtection( this, pGoods);				
				// 通知每个玩家
				CMessage MsgSend( MSG_S2C_RGN_ADDSHAPE,pGoods->GetExID());
				MsgSend.Add( pGoods->GetType() );
				MsgSend.Add( pGoods->GetExID() );
				DBWriteSet setWriteDB;
				MsgSend.GetDBWriteSet(setWriteDB);
				pGoods->SerializeForRgnDisplay(setWriteDB);

				MsgSend.SendToAround(pGoods);
				if(NULL != pOptRerult) *pOptRerult = 1;
			}
		}
	}
	return bAdded;	
}

BOOL    CPlayer::EquipBackToPack()
{
	CGoods* pGoods=m_cEquipment.GetBackToPackGoods();
	if(!pGoods)
		return FALSE;
	CS2CContainerObjectMove cResult;
	DWORD dwSourcePos=0;
	m_cEquipment.QueryGoodsPosition(pGoods,dwSourcePos);
	m_cEquipment.Remove(pGoods);

	cResult.SetOperation(CS2CContainerObjectMove::OT_MOVE_OBJECT);
	cResult.SetSourceContainerExtendID( CS2CContainerObjectMove::PEI_EQUIPMENT );	

	DWORD dwCell = 0;
	if(getOriginPack()->FindPositionForGoods(pGoods, dwCell))
	{		
		if(getOriginPack()->Add(dwCell,pGoods, NULL) )
		{

			cResult.SetSourceContainer( TYPE_PLAYER, GetExID(), dwSourcePos );					
			cResult.SetSourceObject(pGoods->GetType(),pGoods->GetExID());
			cResult.SetDestinationContainer(TYPE_PLAYER,GetExID(),dwCell);
			cResult.SetDestinationContainerExtendID(CS2CContainerObjectMove::PEI_PACKET);
			cResult.SetDestinationObject(pGoods->GetType(),pGoods->GetExID());
			cResult.SetSourceObjectAmount(pGoods->GetAmount());
			cResult.SetDestinationObjectAmount(pGoods->GetAmount());
			cResult.Send(GetExID());
			m_cEquipment.ClearBackToPackGoods();

			return TRUE;		
		}		
	}
    else
    {
        BOOL bAddSucc = FALSE;
        for(long l=CS2CContainerObjectMove::PEI_PACK1; l<=CS2CContainerObjectMove::PEI_PACK5; l++)
        {
            int nSub = l - CS2CContainerObjectMove::PEI_PACK1;
            CVolumeLimitGoodsContainer* pContainer = GetPackage()->GetSubpack(nSub)->pSubpackContainer;
            if(pContainer && pContainer->FindPositionForGoods(pGoods, dwCell))
            {
                if(pContainer->Add(dwCell, pGoods, NULL))
                {
                    cResult.SetSourceContainer( TYPE_PLAYER, GetExID(), dwSourcePos );					
                    cResult.SetSourceObject(pGoods->GetType(),pGoods->GetExID());
                    cResult.SetDestinationContainer(TYPE_PLAYER,GetExID(),dwCell);
                    cResult.SetDestinationContainerExtendID(l);
                    cResult.SetDestinationObject(pGoods->GetType(),pGoods->GetExID());
                    cResult.SetSourceObjectAmount(pGoods->GetAmount());
                    cResult.SetDestinationObjectAmount(pGoods->GetAmount());
                    cResult.Send(GetExID());
                    m_cEquipment.ClearBackToPackGoods();
                    bAddSucc = TRUE;
                    break;
                }
            }
        }
        if(bAddSucc)
        {
            return TRUE;
        }
    }
   
	m_cEquipment.ClearBackToPackGoods();
	return FALSE;
}

BOOL CPlayer::IsInPersonalShop(CGoods* pGoods)
{
	vector<CGoods*>::iterator it=m_vecInPersonalShopGoods.begin();
	for(;it!=m_vecInPersonalShopGoods.end();it++)
	{
		if(pGoods==*it)
			return TRUE;
	}
	return FALSE;
}

void CPlayer::RemovePersonalShopGoodsData(CGUID& guid)
{
	vector<CGoods*>::iterator it=m_vecInPersonalShopGoods.begin();
	for(;it!=m_vecInPersonalShopGoods.end();it++)
	{
		if((*it)->GetExID()==guid)
		{
			m_vecInPersonalShopGoods.erase(it);
			return;
		}
	}	
}

void CPlayer::SetTicket(long	lTicket)
{
	long lOldTicket=m_lTicket;
	m_lTicket=lTicket;
	if(m_lTicket<0)
		m_lTicket=0;
	else if(m_lTicket>(long)GlobalSetup::GetSetup()->dwTicketLimit)
	{
		m_lTicket=GlobalSetup::GetSetup()->dwTicketLimit;
	}
	long lChangeVal=m_lTicket-lOldTicket;
	if(lChangeVal>0)
	{
		//得到
		CHAR szMsg[256];
		const char* strRes=AppFrame::GetText("40");
		if(strRes)
		{
			_snprintf( szMsg, 256, strRes, lChangeVal );
			//SendNotifyMessage( szMsg,0xffffffff,0,eNOTIFYPOS_LEFT_BOTTOM);
			SendNotifyMessage(szMsg,eNOTIFYPOS_LEFT_BOTTOM);
		}

	}
	else if(lChangeVal<0)
	{
		//失去
		CHAR szMsg[256];
		const char* strRes=AppFrame::GetText("39");
		if(strRes)
		{
			_snprintf( szMsg, 256, strRes, -lChangeVal );
			//SendNotifyMessage( szMsg,0xffffffff,0,eNOTIFYPOS_LEFT_BOTTOM);
			SendNotifyMessage(szMsg,eNOTIFYPOS_LEFT_BOTTOM);
		}
	}

	CMessage msg(MSG_S2C_SET_TICKET);
	msg.Add(m_lTicket);
	msg.SendToPlayer(GetExID(),false);
	// 更新玩家属性
	GetGame()->UpdateSavePlayer(this, SAVE_DETAIL_ATTRPROPERTY);
}

void CPlayer::AddPersonalShopGoodsData(CGoods* pGoods)
{
	if(!IsInPersonalShop(pGoods))
		m_vecInPersonalShopGoods.push_back(pGoods);
}

BOOL CPlayer::CheckChangeGoods(vector<CGoodsBaseProperties::tagGoodsInfo>& vecGoodsIn,vector<CGoodsBaseProperties::tagGoodsInfo>& vecGoodsOut)
{	
	///////////////////////////
	vector<CVolumeLimitGoodsContainer*> vecCloneContainer;//存放所有克隆出来的容器
	CVolumeLimitGoodsContainer* pOriginPackClone=OBJ_CREATE(CVolumeLimitGoodsContainer);
	vector<CGoods*> cloneGoodsVec;
	pOriginPackClone->SetContainerMode( CGoodsContainer::GCM_TEST );
	getOriginPack()->CloneEx(pOriginPackClone,&cloneGoodsVec);
	vecCloneContainer.push_back(pOriginPackClone);
	for(int i=0;i<CSubpackContainer::SUBPACK_NUM;i++)
	{
		CSubpackContainer::tagSubpack* pTagSubpack=GetPackage()->GetSubpack(i);
		if(pTagSubpack && pTagSubpack->pGoods)
		{
			CVolumeLimitGoodsContainer* pSubPackClone=OBJ_CREATE(CVolumeLimitGoodsContainer);
			pSubPackClone->SetContainerMode( CGoodsContainer::GCM_TEST );
			pTagSubpack->pSubpackContainer->CloneEx(pSubPackClone,&cloneGoodsVec);
			vecCloneContainer.push_back(pSubPackClone);
		}
	}
#ifdef _GOODSLOG1_
	char pszLog1[1024]="";							

	_snprintf(pszLog1,1024,"[MSG_C2S_SHOP_BUY][容器克隆完成]");
	CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszLog1);							
#endif
	if(vecCloneContainer.size()<1)
	{
		//没有可以克隆的容器
		return FALSE;
	}
	//尝试在克隆容器中扣除交换需要扣除的物品,如果
	//这个过程失败，说明交换物品并不能满足条件
	for(long i=0 ; i<(long)vecGoodsOut.size();i++)
	{
		DWORD dwGoodsId=vecGoodsOut[i].dwGoodsId;		
		DWORD dwRemoveTotal=vecGoodsOut[i].dwAmount;		
		for(long j=0;j<(long)vecCloneContainer.size();j++)
		{
			//删除物品直到dwRemoveAmount==dwAmount 则说明该物品数量满足条件
			DWORD dwRemoveAmount=vecCloneContainer[j]->DelGoods(dwGoodsId,dwRemoveTotal);
			if(dwRemoveTotal==dwRemoveAmount)
			{
				//说明要求删除的数量全部成功删除了
				dwRemoveTotal=0;
				break;
			}
			else
			{
				dwRemoveTotal-=dwRemoveAmount;
			}
		}
		if(dwRemoveTotal>0)
		{
			//遍历了所有容器也不能把物品扣除完
			for(int n=0;n<(int)vecCloneContainer.size();n++)
			{
				OBJ_RELEASE(CVolumeLimitGoodsContainer, vecCloneContainer[n]);
			}

			for(int n=0;n<(int)cloneGoodsVec.size();n++)
			{
				CGoodsFactory::GarbageCollect(&cloneGoodsVec[n],18);
			}
			return FALSE;
		}
	}
#ifdef _GOODSLOG1_
	char pszLog2[1024]="";							

	_snprintf(pszLog2,1024,"[MSG_C2S_SHOP_BUY][尝试扣除交换需要扣除的物品]");
	CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszLog2);							
#endif

	//进行到这里说明扣除物品的条件是满足的
	//开始尝试添加要交换的物品
	BOOL bSucceed=TRUE;
	vector<CGoods*> vecTempGoods;
	for(long i=0 ; i<(long)vecGoodsIn.size();i++)
	{		
		CGoodsFactory::CreateGoods(vecGoodsIn[i].dwGoodsId,vecGoodsIn[i].dwAmount,vecTempGoods,5);
	}
	for(long i=0 ; i<(long)vecTempGoods.size();i++)
	{
		BOOL bAdded=FALSE;
		for(long j=0;j<(long)vecCloneContainer.size();j++)
		{
			bAdded=vecCloneContainer[j]->Add(vecTempGoods[i],NULL);
			if(bAdded)
				break;
		}
		if(!bAdded)
		{
			//尝试了全部背包都不能放下该物品
			bSucceed=FALSE;
			break;
		}
	}
#ifdef _GOODSLOG1_
	char pszLog3[1024]="";							

	_snprintf(pszLog3,1024,"[MSG_C2S_SHOP_BUY][尝试添加要交换的物品完成]");
	CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszLog3);							
#endif

	for(long i=0 ; i<(long)vecTempGoods.size();i++)
	{
#ifdef _GOODSLOG1_
		if(vecTempGoods[i])
		{
			char pszGoodsLog[1024]="";			
			char srcGoodsGUID[50]="";
			vecTempGoods[i]->GetExID().tostring(srcGoodsGUID);					
			_snprintf(pszGoodsLog,1024,"[物品回收:以物换物回收临时物品][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d]",
				vecTempGoods[i]->GetBasePropertiesIndex(),GoodsSetup::QueryGoodsName(vecTempGoods[i]->GetBasePropertiesIndex()),srcGoodsGUID,vecTempGoods[i]->GetAmount());
			CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);	
		}								
#endif
		CGoodsFactory::GarbageCollect(&vecTempGoods[i],20);
	}
	for(long n=0 ; n<(long)vecCloneContainer.size();n++)
	{
		OBJ_RELEASE(CVolumeLimitGoodsContainer, vecCloneContainer[n]);
	}
	for(long n=0 ; n<(long)cloneGoodsVec.size(); n++)
	{
		CGoodsFactory::GarbageCollect(&cloneGoodsVec[n],18);
	}
	return bSucceed;
}

void CPlayer::GoodsChangeGoods(vector<CGoodsBaseProperties::tagGoodsInfo>& vecGoodsIn,vector<CGoodsBaseProperties::tagGoodsInfo>& vecGoodsOut)
{
	///////////////////////////
	struct tagContainers
	{
		DWORD dwContainerId;
		CVolumeLimitGoodsContainer* pContainer;
	};
	vector<tagContainers*> vecContainer;	
	tagContainers* stOrigin=OBJ_CREATE(tagContainers);
	stOrigin->dwContainerId=CS2CContainerObjectMove::PEI_PACKET;
	stOrigin->pContainer=getOriginPack();
	vecContainer.push_back(stOrigin);
	for(int i=0;i<CSubpackContainer::SUBPACK_NUM;i++)
	{
		CSubpackContainer::tagSubpack* pTagSubpack=GetPackage()->GetSubpack(i);
		if(pTagSubpack && pTagSubpack->pGoods)
		{	
			tagContainers* stSubpack=OBJ_CREATE(tagContainers);
			stSubpack->dwContainerId=CS2CContainerObjectMove::PEI_PACK1+i;
			stSubpack->pContainer=pTagSubpack->pSubpackContainer;
			vecContainer.push_back(stSubpack);
		}
	}	
	if(vecContainer.size()<1)
	{
		//没有可以克隆的容器
		return;
	}
	//尝试在克隆容器中扣除交换需要扣除的物品,如果
	//这个过程失败，说明交换物品并不能满足条件
	CMessage msg(MSG_S2C_SHOP_GOODSCHANGEGOODS);
	//要扣除物品的个数
	msg.Add((DWORD)vecGoodsOut.size());	
	struct tagGoodsOutData
	{
		DWORD dwContainerId;
		vector<CGoodsBaseProperties::tagGoodsDelInfo> vecPosAndNum;
	};

	for(int i=0;i<(int)vecGoodsOut.size();i++)
	{
		DWORD dwGoodsId=vecGoodsOut[i].dwGoodsId;		
		DWORD dwRemoveTotal=vecGoodsOut[i].dwAmount;
		vector<tagGoodsOutData*> vecGoodsOutData;
		vecGoodsOutData.clear();
		for(long j=0; j<(long)vecContainer.size();j++)
		{
			//删除物品直到dwRemoveAmount==dwAmount 则说明该物品数量满足条件
			tagGoodsOutData* stOutData=OBJ_CREATE(tagGoodsOutData);
			stOutData->dwContainerId=vecContainer[j]->dwContainerId;
			DWORD dwRemoveAmount=vecContainer[j]->pContainer->DelGoods(dwGoodsId,dwRemoveTotal,&(stOutData->vecPosAndNum),TRUE);
			vecGoodsOutData.push_back(stOutData);
			if(dwRemoveTotal==dwRemoveAmount)
			{
				//说明要求删除的数量全部成功删除了
				dwRemoveTotal=0;
				break;
			}                 
			else
			{
				dwRemoveTotal-=dwRemoveAmount;
			}			
		}
		if(dwRemoveTotal>0)
		{
			//遍历了所有容器也不能把物品扣除完			
		}
		msg.Add((DWORD)vecGoodsOutData.size());
		for(long j=0;j<(long)vecGoodsOutData.size();j++)
		{
			msg.Add(vecGoodsOutData[j]->dwContainerId);
			msg.Add((DWORD)vecGoodsOutData[j]->vecPosAndNum.size());
			for(long k=0 ; k<(long)vecGoodsOutData[j]->vecPosAndNum.size();k++)
			{
				msg.Add(vecGoodsOutData[j]->vecPosAndNum[k].dwPos);
				msg.Add(vecGoodsOutData[j]->vecPosAndNum[k].dwNum);
			}
		}
		for(long j=0;j<(long)vecGoodsOutData.size();j++)
		{
			OBJ_RELEASE(tagGoodsOutData, vecGoodsOutData[j]);
		}
	}

	//进行到这里说明扣除物品的条件是满足的
	//开始尝试添加要交换的物品

	vector<CGoods*> vecTempGoods;
	for(long i=0 ; i<(long)vecGoodsIn.size();i++)
	{		
		CGoodsFactory::CreateGoods(vecGoodsIn[i].dwGoodsId,vecGoodsIn[i].dwAmount,vecTempGoods,6);
	}
	msg.Add((DWORD)vecTempGoods.size());
	vector<DWORD> vecGoodsId;
	vector<CGoods*> vecGoodsPtr;
	vector<BYTE> btGoodsInData;
	for(long i=0 ; i<(long)vecTempGoods.size();i++)
	{
		BOOL bAdded=FALSE;
		for(int j=0;j<(int)vecContainer.size();j++)
		{
			DWORD dwTempGoodsId=vecTempGoods[i]->GetBasePropertiesIndex();
			DWORD dwTempGoodsNum=vecTempGoods[i]->GetAmount();
			DWORD dwMaxStackNum=vecTempGoods[i]->GetMaxStackNumber();
			bAdded=vecContainer[j]->pContainer->Add(vecTempGoods[i],&btGoodsInData,vecContainer[j]->dwContainerId);
			//玩家NPC商店买物品的日志记录
			if(bAdded)
				GetLogicLogInterface()->logT030_goods_log_npc(this,dwTempGoodsId,vecTempGoods[i]->GetExID(),
					GoodsSetup::QueryGoodsName(dwTempGoodsId),2,0,dwTempGoodsNum);
#ifdef _GOODSLOG1_
			char pszGoodsLog[1024]="";
			char playerGUID[50]="";
			GetExID().tostring(playerGUID);

			_snprintf(pszGoodsLog,1024,"[操作:以物换物得物品][玩家名:%s][玩家GUID:%s][容器ID:%d][物品ID:%d][物品名:%s][物品数量:%d]",
				GetName(),playerGUID,vecContainer[j]->dwContainerId,dwTempGoodsId,GoodsSetup::QueryGoodsName(dwTempGoodsId),dwTempGoodsNum);
			CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
#endif	
			if(bAdded)
			{
				if(dwMaxStackNum>1)
				{
					//OnGetGoods(dwTempGoodsId);
					vecGoodsId.push_back(dwTempGoodsId);
				}	
				else
				{
					//OnGetGoods(dwTempGoodsId,vecTempGoods[i]);
					vecGoodsPtr.push_back(vecTempGoods[i]);
				}
				break;
			}
		}
		if(!bAdded)
		{
			//尝试了全部背包都不能放下该物品			
			break;
		}
	}
	if(btGoodsInData.size()==0)
		return;
	msg.Add((long)btGoodsInData.size());
	msg.AddEx(&btGoodsInData[0],(long)btGoodsInData.size());
	msg.SendToPlayer(GetExID(),false);

	vector<DWORD>::iterator idIter=vecGoodsId.begin();
	for(;idIter!=vecGoodsId.end();idIter++)
	{
		OnGetGoods(*idIter);
	}
	vector<CGoods*>::iterator ptrIter=vecGoodsPtr.begin();
	for(;ptrIter!=vecGoodsPtr.end();ptrIter++)
	{
		OnGetGoods((*ptrIter)->GetBasePropertiesIndex(),*ptrIter);
	}

	for(long i=0 ; i<(long)vecContainer.size();i++)
	{
		OBJ_RELEASE(tagContainers, vecContainer[i]);
	}
	vecContainer.clear();
}

//检查物品是否有该物品，返回物品的个数
int	CPlayer::CheckGoods(const char *goodoriname)
{
	long lRet = 0;
	if( goodoriname )
	{
		CSeekGoodsListener lSeeker;
		lSeeker.SetTarget( (char*)goodoriname );
		getOriginPack()->TraversingContainer( &lSeeker );
		for( size_t i = 0; i < lSeeker.m_vGoodsID.size(); i ++ )
		{
			CGoods* pGoods = dynamic_cast<CGoods*>( getOriginPack()->Find(TYPE_GOODS, lSeeker.m_vGoodsID[i]) );
			if( pGoods )
			{
				lRet += pGoods -> GetAmount();
			}
		}

		lSeeker.m_vGoodsID.clear();
		m_cEquipment.TraversingContainer( &lSeeker );
		for( size_t i = 0; i < lSeeker.m_vGoodsID.size(); i ++ )
		{
			CGoods* pGoods = dynamic_cast<CGoods*>( m_cEquipment.Find(TYPE_GOODS, lSeeker.m_vGoodsID[i]) );
			if( pGoods )
			{
				lRet += pGoods -> GetAmount();
			}
		}
	}
	return lRet;
}

DWORD CPlayer::GetTotalEmptySpaceNum()
{
	DWORD dwEmptyGrid=0;
	dwEmptyGrid+=getOriginPack()->GetEmptySpaceNum();
	for(int i=0;i<CSubpackContainer::SUBPACK_NUM;i++)
	{
		CSubpackContainer::tagSubpack* pTagSubpack=GetPackage()->GetSubpack(i);
		if(pTagSubpack && pTagSubpack->pSubpackContainer)
		{
			dwEmptyGrid+=pTagSubpack->pSubpackContainer->GetEmptySpaceNum();
		}
	}	
	return dwEmptyGrid;
}

DWORD CPlayer::GetTotalSpaceNum()
{
	DWORD allNum=0;
	allNum+=getOriginPack()->GetVolume();
	//子背包
	for (int i=0;i<CSubpackContainer::SUBPACK_NUM;i++)
	{
		CSubpackContainer::tagSubpack* pTagSubpack=GetPackage()->GetSubpack(i);
		if(pTagSubpack && pTagSubpack->pSubpackContainer)
		{
			allNum+=pTagSubpack->pSubpackContainer->GetVolume();
		}
	}
	return allNum;
}


DWORD CPlayer::GetGoodsAmountByOriginName(const char* strOriginName)
{
	DWORD dwAmount=0;
	dwAmount+=getOriginPack()->GetGoodsAmountByOriginName(strOriginName);
	for(int i=0;i<CSubpackContainer::SUBPACK_NUM;i++)
	{
		CSubpackContainer::tagSubpack* pTagSubpack=GetPackage()->GetSubpack(i);
		if(pTagSubpack && pTagSubpack->pSubpackContainer)
		{
			dwAmount+=pTagSubpack->pSubpackContainer->GetGoodsAmountByOriginName(strOriginName);
		}
	}	
	return dwAmount;
}


int	CPlayer::check_item_in_packet(const int item_idx)
{
	long lRet = 0;
	if( item_idx )
	{
		CSeekGoodsListener lSeeker;
		lSeeker.SetTarget( item_idx );
		getOriginPack()->TraversingContainer( &lSeeker );
		for( size_t i = 0; i < lSeeker.m_vGoodsID.size(); i ++ )
		{
			CGoods* pGoods = dynamic_cast<CGoods*>(getOriginPack()->Find(TYPE_GOODS, lSeeker.m_vGoodsID[i]) );
			if( pGoods )
			{
				lRet += pGoods -> GetAmount();
			}
		}
	}
	return lRet;
}

int	CPlayer::remove_item_in_packet(const int item_idx, const int item_num)
{
	long ret = 0;
	if( item_idx )
	{
		long lRemainedGoods;
		CSeekGoodsListener lSeeker;
		lSeeker.SetTarget( item_idx );
		getOriginPack()->TraversingContainer( &lSeeker );

		for( size_t i = 0; i < lSeeker.m_vGoodsID.size(); i ++ )
		{
			lRemainedGoods = 0;
			if( ret < item_num )
				lRemainedGoods = item_num - ret;
			if( lRemainedGoods == 0 )
				break;
			ret += DeleteGoods( CS2CContainerObjectMove::PEI_PACKET, lSeeker.m_vGoodsID[i], lRemainedGoods );
		}
	}
	return ret;
}

BOOL CPlayer::DelGoodsAmountByOriginName( const char* goodsname, int goodsnum)
{
	CSeekGoodsListener lSeeker;
	lSeeker.SetTarget( (char*)goodsname );
	getOriginPack()->TraversingContainer( &lSeeker );
	int  lRet=0;

	for( size_t i = 0; i < lSeeker.m_vGoodsID.size(); i ++ )
	{
		LONG lRemainedGoods = 0;
		if( lRet < goodsnum) lRemainedGoods = goodsnum - lRet;
		if( lRemainedGoods == 0 ) break;
		lRet += DeleteGoods( CS2CContainerObjectMove::PEI_PACKET, lSeeker.m_vGoodsID[i], lRemainedGoods );
	}

	//lSeeker.m_vGoodsID.clear();
	//m_cEquipment.TraversingContainer( &lSeeker );
	//for( size_t i = 0; i < lSeeker.m_vGoodsID.size(); i ++ )
	//{
	//	LONG lRemainedGoods = 0;
	//	if( lRet < goodsnum ) lRemainedGoods = goodsnum - lRet;
	//	if( lRemainedGoods == 0 ) break;
	//	lRet += DeleteGoods( CS2CContainerObjectMove::PEI_EQUIPMENT, lSeeker.m_vGoodsID[i], lRemainedGoods );
	//}

	BOOL bEnough=FALSE;
	for(int i=CS2CContainerObjectMove::PEI_PACK1;i<=CS2CContainerObjectMove::PEI_PACK5;i++)
	{
		if(GetPackage()->GetSubpack(i-CS2CContainerObjectMove::PEI_PACK1)->pGoods)
		{			
			lSeeker.m_vGoodsID.clear();
			GetPackage()->GetSubpack(i-CS2CContainerObjectMove::PEI_PACK1)->pSubpackContainer->TraversingContainer(&lSeeker);
			for(size_t j=0;j<lSeeker.m_vGoodsID.size();j++)
			{
				LONG lRemainedGoods=0;
				if(lRet<goodsnum)
					lRemainedGoods=goodsnum-lRet;
				if(lRemainedGoods==0)
				{
					bEnough=TRUE;
					break;
				}
				lRet+= DeleteGoods(static_cast<CS2CContainerObjectMove::PLAYER_EXTEND_ID>(i),lSeeker.m_vGoodsID[j],lRemainedGoods);
			}
			if(bEnough)
				break;			
		}
	}	
	//SAFE_DELETE_ARRAY(name);
	return lRet;
}

void CPlayer::SetPersonalShopFlag( const CGUID& SessionID, long PlugID )
{
	if( SessionID == NULL_GUID && PlugID == 0 )
	{
		m_PersonalShopSessionID	= NULL_GUID;
		m_PersonalShopPlugID		= 0;
	}
	else
	{
		CSession* pSession = const_cast<CSession*>( CSessionFactory::QuerySession(SessionID) );
		if( pSession )
		{
			CPlug* pPlug = pSession -> QueryPlugByOwner( GetType(), GetExID() );
			if( pPlug && pPlug -> Get32ID() == PlugID )
			{
				m_PersonalShopSessionID	= SessionID;
				m_PersonalShopPlugID		= PlugID;
			}
		}
	}
}

void CPlayer::CodePersonalShopForClient(DBWriteSet& setWriteDB)
{
    CPersonalShopSeller* pSeller = dynamic_cast<CPersonalShopSeller*>( 
        const_cast<CPlug*>( CSessionFactory::QueryPlug(m_PersonalShopPlugID) ) );
    if( m_PersonalShopSessionID != NULL_GUID &&  pSeller )
    {
        setWriteDB.AddToByteArray( m_PersonalShopSessionID );
        setWriteDB.AddToByteArray( m_PersonalShopPlugID );
        setWriteDB.AddToByteArray( pSeller -> GetShopName() );
    }
    else
    {
        setWriteDB.AddToByteArray(NULL_GUID );
    }
}

//根据物品原始名在所有背包中查找第一个物品
CGoods* CPlayer::FindGoodInPack(const char* strGoodsName)
{
	CSeekGoodsListener lSeeker;
	lSeeker.SetTarget((char*)strGoodsName );
	getOriginPack()->TraversingContainer(&lSeeker);
	for( size_t i = 0; i < lSeeker.m_vGoodsID.size(); i ++ )
	{
		CGoods* pGoods = dynamic_cast<CGoods*>(getOriginPack()->Find(
			TYPE_GOODS, lSeeker.m_vGoodsID[i]));
		if( pGoods )
		{
			return pGoods;
		}
	}

	for(int i=CS2CContainerObjectMove::PEI_PACK1;i<=CS2CContainerObjectMove::PEI_PACK5;i++)
	{
		//先检查子背包栏位中的背包物品本身是否唯一
		CGoods* pSubContainer=GetPackage()->GetSubpack(
			i-CS2CContainerObjectMove::PEI_PACK1)->pGoods;
		if(pSubContainer)
		{	
			const char* cName=GoodsSetup::QueryGoodsOriginalName(
				pSubContainer->GetBasePropertiesIndex());
			if (cName && 0==strcmp(strGoodsName,cName))
			{
				return pSubContainer;
			}
			//此背包中的物品是否唯一
			lSeeker.m_vGoodsID.clear();
			GetPackage()->GetSubpack(i-CS2CContainerObjectMove::PEI_PACK1)->
				pSubpackContainer->TraversingContainer(&lSeeker);
			for(size_t j=0;j<lSeeker.m_vGoodsID.size();j++)
			{
				CGoods* pGoods = dynamic_cast<CGoods*>(GetPackage()->GetSubpack(
					i-CS2CContainerObjectMove::PEI_PACK1)->pSubpackContainer->Find(
					TYPE_GOODS, lSeeker.m_vGoodsID[j]));
				if( pGoods )
				{
					return pGoods;
				}
			}	
		}
	}	

	return NULL;
}

CGoods* CPlayer::FindGoodInDepot(const char* strGoodsName)
{
	static DWORD arrContainerType[] = 
	{
		eDCT_Primary,
		eDCT_Secondary1,
		eDCT_Secondary2,
		eDCT_Secondary3,
		eDCT_Secondary4
	};

	CSeekGoodsListener lSeeker;
	lSeeker.SetTarget((char*)strGoodsName );

	for (LONG i = 0; i < sizeof(arrContainerType) / sizeof(DWORD); ++i)
	{
		CAmountLimitGoodsContainer *pContainer = (CAmountLimitGoodsContainer*)m_pPlayerDepot.FindContainer(arrContainerType[i],true);
		if(NULL != pContainer)
		{
			lSeeker.m_vGoodsID.clear();
			pContainer->TraversingContainer(&lSeeker);
			for(size_t j=0;j<lSeeker.m_vGoodsID.size();j++)
			{
				CGoods* pGoods = dynamic_cast<CGoods*>(pContainer->Find(TYPE_GOODS, lSeeker.m_vGoodsID[j]));
				if( pGoods )
				{
					return pGoods;
				}
			}
		}
	}

	return NULL;
}

BOOL CPlayer::IsGoodsUnique(const char* strGoodsName)
{
	DWORD dwGoodsId=GoodsSetup::QueryGoodsIDByOriginalName(strGoodsName);
	if(!dwGoodsId)
		return FALSE;

	CGoods* pExistGoods=FindGoodInPack(strGoodsName);
	if (pExistGoods)	
		return FALSE;
	pExistGoods=FindGoodInDepot(strGoodsName);
	if(pExistGoods)
		return FALSE;
	pExistGoods=m_cEquipment.GetTheFirstGoods(dwGoodsId);
	if(pExistGoods)
		return FALSE;
	return TRUE;
}

//! 在玩家身上（所有背包、装备栏位、仓库）找到一个物品（不忽略锁定物品）
CGoods* CPlayer::FindGoodByGuid(const CGUID &GoodsGUID)
{

	CGoods* pGoods = dynamic_cast<CGoods*>(getOriginPack()->FindIgnoreLock(GoodsGUID));
	if( NULL != pGoods )
	{
		return pGoods;
	}
	pGoods = dynamic_cast<CGoods*>(m_cEquipment.Find(GoodsGUID));
	if( NULL != pGoods )
	{
		return pGoods;
	}	
    pGoods = dynamic_cast<CGoods*>(m_cEquipmentGod.Find(GoodsGUID));
    if(NULL != pGoods)
    {
        return pGoods;
    }
    pGoods = dynamic_cast<CGoods*>(m_cEquipmentLand.Find(GoodsGUID));
    if(NULL != pGoods)
    {
        return pGoods;
    }
    pGoods = dynamic_cast<CGoods*>(m_cEquipmentHuman.Find(GoodsGUID));
    if(NULL != pGoods)
    {
        return pGoods;
    }
	for(int i=CS2CContainerObjectMove::PEI_PACK1;i<=CS2CContainerObjectMove::PEI_PACK5;i++)
	{
		if(GetPackage()->GetSubpack(i-CS2CContainerObjectMove::PEI_PACK1)->pGoods)
		{			
			pGoods = dynamic_cast<CGoods*>(GetPackage()->GetSubpack(i-CS2CContainerObjectMove::PEI_PACK1)->pSubpackContainer->FindIgnoreLock(GoodsGUID));
			if( NULL != pGoods )
			{
				return pGoods;
			}
		}
	}

	static DWORD arrContainerType[] = 
	{
		eDCT_Primary,
		eDCT_Secondary1,
		eDCT_Secondary2,
		eDCT_Secondary3,
		eDCT_Secondary4
	};

	for (LONG i = 0; i < sizeof(arrContainerType) / sizeof(DWORD); ++i)
	{
		CAmountLimitGoodsContainer *pContainer = (CAmountLimitGoodsContainer*)m_pPlayerDepot.FindContainer(arrContainerType[i]);
		if(NULL != pContainer)
		{
			pGoods = (CGoods*)pContainer->FindIgnoreLock(GoodsGUID);
			if(NULL != pGoods)
				return pGoods;
		}
	}


	return NULL;
}


CGoods* CPlayer::FindGoodsAllPlaceRemove(const CGUID& GoodsGUID)
{
	CGoods* pGoods = dynamic_cast<CGoods*>(getOriginPack()->FindIgnoreLock(GoodsGUID));
	if( NULL != pGoods )
	{
		getOriginPack()->Remove(pGoods);
		return pGoods;
	}
	pGoods = dynamic_cast<CGoods*>(m_cEquipment.Find(GoodsGUID));
	if( NULL != pGoods )
	{
		m_cEquipment.Remove(pGoods);
		return pGoods;
	}	
    pGoods = dynamic_cast<CGoods*>(m_cEquipmentGod.Find(GoodsGUID));
    if( NULL != pGoods )
    {
        m_cEquipmentGod.Remove(pGoods);
        return pGoods;
    }	
    pGoods = dynamic_cast<CGoods*>(m_cEquipmentLand.Find(GoodsGUID));
    if( NULL != pGoods )
    {
        m_cEquipmentLand.Remove(pGoods);
        return pGoods;
    }	
    pGoods = dynamic_cast<CGoods*>(m_cEquipmentHuman.Find(GoodsGUID));
    if( NULL != pGoods )
    {
        m_cEquipmentHuman.Remove(pGoods);
        return pGoods;
    }	
	for(int i=CS2CContainerObjectMove::PEI_PACK1;i<=CS2CContainerObjectMove::PEI_PACK5;i++)
	{
		CGoods* pPackGoods=GetPackage()->GetSubpack(i-CS2CContainerObjectMove::PEI_PACK1)->pGoods;
		if(pPackGoods)
		{
			if(pPackGoods->GetExID()==GoodsGUID)
			{
				GetPackage()->GetSubpackContainer()->Remove(pGoods);
				return pPackGoods;
			}
			pGoods = dynamic_cast<CGoods*>(GetPackage()->GetSubpack(i-CS2CContainerObjectMove::PEI_PACK1)->pSubpackContainer->FindIgnoreLock(GoodsGUID));
			if( NULL != pGoods )
			{
				GetPackage()->GetSubpack(i-CS2CContainerObjectMove::PEI_PACK1)->pSubpackContainer->Remove(pGoods);
				return pGoods;
			}
		}
	}

	static DWORD arrContainerType[] = 
	{
		eDCT_Primary,
		eDCT_Secondary1,
		eDCT_Secondary2,
		eDCT_Secondary3,
		eDCT_Secondary4
	};

	for (LONG i = 0; i < sizeof(arrContainerType) / sizeof(DWORD); ++i)
	{
		CAmountLimitGoodsContainer *pContainer = (CAmountLimitGoodsContainer*)m_pPlayerDepot.FindContainer(arrContainerType[i]);
		if(NULL != pContainer)
		{
			pGoods = (CGoods*)pContainer->FindIgnoreLock(GoodsGUID);
			if(NULL != pGoods)
			{
				pContainer->Remove(pGoods);
				return pGoods;
			}
		}
	}


	return NULL;
}

//! 在玩家所有背包（包括子背包）中找到一个物品（不忽略锁定物品）
CGoods* CPlayer::FindGoodsByGuidInPack(const CGUID &GoodsGUID)
{
	CGoods* pGoods = dynamic_cast<CGoods*>(getOriginPack()->FindIgnoreLock(GoodsGUID));
	if( NULL != pGoods )
	{
		return pGoods;
	}
	for(int i=CS2CContainerObjectMove::PEI_PACK1;i<=CS2CContainerObjectMove::PEI_PACK5;i++)
	{
		if(GetPackage()->GetSubpack(i-CS2CContainerObjectMove::PEI_PACK1)->pGoods)
		{			
			pGoods = dynamic_cast<CGoods*>(GetPackage()->GetSubpack(i-CS2CContainerObjectMove::PEI_PACK1)->pSubpackContainer->FindIgnoreLock(GoodsGUID));
			if( NULL != pGoods )
			{
				return pGoods;
			}
		}
	}

	return NULL;
}

BOOL CPlayer::FindGoodsByGuid(const CGUID& guid,DWORD& dwContainer,DWORD& dwPos)
{
	BOOL bFound=FALSE;
	DWORD dwPosition=0;
	bFound = getOriginPack()->QueryGoodsPosition(guid,dwPosition);
	if(bFound)
	{
		dwContainer=CS2CContainerObjectMove::PEI_PACKET;
		dwPos=dwPosition;
		return TRUE;
	}

	/*
	pGoods = dynamic_cast<CGoods*>(m_cEquipment.Find(GoodsGUID));
	if( NULL != pGoods )
	{
	return pGoods;
	}	
	*/

	for(int i=CS2CContainerObjectMove::PEI_PACK1;i<=CS2CContainerObjectMove::PEI_PACK5;i++)
	{
		if(GetPackage()->GetSubpack(i-CS2CContainerObjectMove::PEI_PACK1)->pGoods)
		{			
			bFound =GetPackage()->GetSubpack(i-CS2CContainerObjectMove::PEI_PACK1)->pSubpackContainer->QueryGoodsPosition(guid,dwPosition);
			if(bFound)
			{
				dwContainer=i;
				dwPos=dwPosition;
				return TRUE;
			}
		}
	}	

	return FALSE;
}

CGoods* CPlayer::RemoveGoods(long lContainerId,long lPos)
{
	//! 找到该物品的容器
	CGoodsContainer* pCGoodsContainer = FindContainer(lContainerId);
	if( NULL != pCGoodsContainer )
	{
		CGoods* pGoods = dynamic_cast<CGoods*>( pCGoodsContainer -> GetGoods(lPos) );
		assert(NULL != pGoods);

		//##删除
		CS2CContainerObjectMove comMsg;
		if( pCGoodsContainer -> Remove(pGoods, &comMsg) )
		{
			//OnLoseGoods(pGoods);
			comMsg.SetOperation( CS2CContainerObjectMove::OT_DELETE_OBJECT );
			comMsg.SetSourceContainerExtendID( lContainerId );
			comMsg.Send( GetExID() );

			//##删除该物品?
			//GoodsSetup::GarbageCollect( &pGoods );
			return pGoods;
		}
	}
	return NULL;

	/*CGoods* pGoods=NULL;
	pGoods = dynamic_cast<CGoods*>(getOriginPack()->Find(GoodsGUID));
	if( NULL != pGoods )
	{
	DWORD dwPos=0;
	getOriginPack()->QueryGoodsPosition(pGoods,dwPos);
	getOriginPack()->Remove(dwPos,dwNum);
	return pGoods;
	}

	for(int i=CS2CContainerObjectMove::PEI_PACK1;i<=CS2CContainerObjectMove::PEI_PACK5;i++)
	{
	if(GetPackage()->GetSubpack(i-CS2CContainerObjectMove::PEI_PACK1)->pGoods)
	{			
	pGoods = dynamic_cast<CGoods*>(GetPackage()->GetSubpack(i-CS2CContainerObjectMove::PEI_PACK1)->pSubpackContainer->Find(GoodsGUID));
	if( NULL != pGoods )
	{
	DWORD dwPos=0;
	GetPackage()->GetSubpack(i-CS2CContainerObjectMove::PEI_PACK1)->pSubpackContainer->QueryGoodsPosition(pGoods,dwPos);

	GetPackage()->GetSubpack(i-CS2CContainerObjectMove::PEI_PACK1)->pSubpackContainer->Remove(dwPos,dwNum);
	return pGoods;
	}
	}
	}	*/

}


CGoods* CPlayer::FindGoods(LONG lContainerId,LONG lPos)
{
	CGoods* pGoods=NULL;
	CGoodsContainer* pContainer = FindContainer(lContainerId);
	if(pContainer)
	{
		pGoods=pContainer->GetGoods(lPos);
	}
	return pGoods;
}


BOOL CPlayer::DelGoods(LONG lContainerId,LONG lPos)
{
	CGoodsContainer* pContainer = FindContainer(lContainerId);
	if(pContainer)
	{
		if(pContainer->Remove(lPos,1))
		{
			return TRUE;
		}
	}
	return FALSE;
}

void CPlayer::ClearGoods(LONG lContainerId,LONG lPos)
{
	if(lContainerId==CS2CContainerObjectMove::PEI_PACKET || (lContainerId>=CS2CContainerObjectMove::PEI_PACK1 && 
		lContainerId<=CS2CContainerObjectMove::PEI_PACK5))
	{
		CGoodsContainer* pContainer = FindContainer(lContainerId);
		CVolumeLimitGoodsContainer* pVolumeContainer=dynamic_cast<CVolumeLimitGoodsContainer*>(pContainer);
		if(pVolumeContainer)
		{
			pVolumeContainer->ClearGoods(lPos);
		}
	}
}

//! 找到指定原始名、数量的[未锁定]物品
BOOL CPlayer::FindGoods(char* strOriginName,int num)
{
	CSeekGoodsListener lSeeker;
	lSeeker.SetTarget((char*)strOriginName );
	getOriginPack()->TraversingContainer(&lSeeker);
	for( size_t i = 0; i < lSeeker.m_vGoodsID.size(); i ++ )
	{
		CGoods* pGoods = dynamic_cast<CGoods*>(getOriginPack()->Find(TYPE_GOODS, lSeeker.m_vGoodsID[i]));
		if( pGoods )
		{
			if(!getOriginPack()->IsLocked(pGoods) && !IsInPersonalShop(pGoods) && (int)pGoods->GetAmount() >= num)
			{
				return TRUE;
			}
		}
	}

	for(int i=CS2CContainerObjectMove::PEI_PACK1;i<=CS2CContainerObjectMove::PEI_PACK5;i++)
	{
		if(GetPackage()->GetSubpack(i-CS2CContainerObjectMove::PEI_PACK1)->pGoods)
		{			
			lSeeker.m_vGoodsID.clear();
			CVolumeLimitGoodsContainer* pContainer = GetPackage()->GetSubpack(i-CS2CContainerObjectMove::PEI_PACK1)->pSubpackContainer;
			pContainer->TraversingContainer(&lSeeker);
			for(size_t j=0;j<lSeeker.m_vGoodsID.size();j++)
			{
				CGoods* pGoods = dynamic_cast<CGoods*>(pContainer->Find(TYPE_GOODS, lSeeker.m_vGoodsID[j]));
				if( pGoods )
				{
					if(!pContainer->IsLocked(pGoods)  && !IsInPersonalShop(pGoods) && (int)pGoods->GetAmount() >= num)
					{
						return TRUE;
					}
				}
			}	
		}
	}

	return FALSE;
}
//! 删除指定原始名、数量的[未锁定]物品
BOOL CPlayer::DelGoods(char* strOriginName,int num)
{
	CSeekGoodsListener lSeeker;
	lSeeker.SetTarget((char*)strOriginName );
	getOriginPack()->TraversingContainer(&lSeeker);
	for( size_t i = 0; i < lSeeker.m_vGoodsID.size(); i ++ )
	{
		CGoods* pGoods = dynamic_cast<CGoods*>(getOriginPack()->Find(TYPE_GOODS, lSeeker.m_vGoodsID[i]));
		if( pGoods )
		{
			if(!getOriginPack()->IsLocked(pGoods)  && !IsInPersonalShop(pGoods) && (int)pGoods->GetAmount() >= num)
			{
				DeleteGoods( CS2CContainerObjectMove::PEI_PACKET, lSeeker.m_vGoodsID[i], num);
				return TRUE;
			}
		}
	}

	for(int i=CS2CContainerObjectMove::PEI_PACK1;i<=CS2CContainerObjectMove::PEI_PACK5;i++)
	{
		if(GetPackage()->GetSubpack(i-CS2CContainerObjectMove::PEI_PACK1)->pGoods)
		{			
			lSeeker.m_vGoodsID.clear();
			CVolumeLimitGoodsContainer* pContainer = GetPackage()->GetSubpack(i-CS2CContainerObjectMove::PEI_PACK1)->pSubpackContainer;
			pContainer->TraversingContainer(&lSeeker);
			for(size_t j=0;j<lSeeker.m_vGoodsID.size();j++)
			{
				CGoods* pGoods = dynamic_cast<CGoods*>(pContainer->Find(TYPE_GOODS, lSeeker.m_vGoodsID[j]));
				if( pGoods )
				{
					if(!pContainer->IsLocked(pGoods)  && !IsInPersonalShop(pGoods) && (int)pGoods->GetAmount() >= num)
					{
						DeleteGoods( (CS2CContainerObjectMove::PLAYER_EXTEND_ID)i, lSeeker.m_vGoodsID[j], num);
						return TRUE;
					}
				}
			}	
		}
	}

	return FALSE;
}

//! 删除指定GUID、数量的[未锁定]物品
BOOL CPlayer::DelGoods(const CGUID &GoodsGuid,int num)
{
	CGoods* pGoods = dynamic_cast<CGoods*>(getOriginPack()->Find(GoodsGuid));
	if( pGoods )
	{
		if(!getOriginPack()->IsLocked(pGoods)  && !IsInPersonalShop(pGoods) && (int)pGoods->GetAmount() >= num)
		{	
			return 0 != DeleteGoods( CS2CContainerObjectMove::PEI_PACKET, GoodsGuid, num);
		}
	}

	for(int i=CS2CContainerObjectMove::PEI_PACK1;i<=CS2CContainerObjectMove::PEI_PACK5;i++)
	{
		if(GetPackage()->GetSubpack(i-CS2CContainerObjectMove::PEI_PACK1)->pGoods)
		{			
			CVolumeLimitGoodsContainer* pContainer = GetPackage()->GetSubpack(i-CS2CContainerObjectMove::PEI_PACK1)->pSubpackContainer;

			CGoods* pGoods = dynamic_cast<CGoods*>(pContainer->Find(GoodsGuid));
			if( pGoods )
			{
				if(!pContainer->IsLocked(pGoods)  && !IsInPersonalShop(pGoods) && (int)pGoods->GetAmount() >= num)
				{
					return 0 != DeleteGoods( (CS2CContainerObjectMove::PLAYER_EXTEND_ID)i, GoodsGuid, num);
				}
			}	
		}
	}

	return FALSE;
}

//! 找到指定原始名、数量的[未锁定]物品
BOOL CPlayer::FindGoodsByIndex(LONG lGoodsIdx,  LONG num)
{
	CSeekGoodsListener lSeeker;
	lSeeker.SetTarget(lGoodsIdx);
	getOriginPack()->TraversingContainer(&lSeeker);
	for( size_t i = 0; i < lSeeker.m_vGoodsID.size(); i ++ )
	{
		CGoods* pGoods = dynamic_cast<CGoods*>(getOriginPack()->Find(TYPE_GOODS, lSeeker.m_vGoodsID[i]));
		if( pGoods )
		{
			if(!getOriginPack()->IsLocked(pGoods) && !IsInPersonalShop(pGoods) && (long)pGoods->GetAmount() >= num)
			{
				return TRUE;
			}
			else
			{
				num -= pGoods->GetAmount();
			}
		}
	}

	for(int i=CS2CContainerObjectMove::PEI_PACK1;i<=CS2CContainerObjectMove::PEI_PACK5;i++)
	{
		if(GetPackage()->GetSubpack(i-CS2CContainerObjectMove::PEI_PACK1)->pGoods)
		{			
			lSeeker.m_vGoodsID.clear();
			CVolumeLimitGoodsContainer* pContainer = GetPackage()->GetSubpack(i-CS2CContainerObjectMove::PEI_PACK1)->pSubpackContainer;
			pContainer->TraversingContainer(&lSeeker);
			for(size_t j=0;j<lSeeker.m_vGoodsID.size();j++)
			{
				CGoods* pGoods = dynamic_cast<CGoods*>(pContainer->Find(TYPE_GOODS, lSeeker.m_vGoodsID[j]));
				if( pGoods )
				{
					if(!pContainer->IsLocked(pGoods)  && !IsInPersonalShop(pGoods) && (long)pGoods->GetAmount() >= num)
					{
						return TRUE;
					}
					else
					{
						num -= pGoods->GetAmount();
					}
				}
			}	
		}
	}

	return FALSE;
}
//! 删除指定原始名、数量的[未锁定]物品
BOOL CPlayer::DelGoodsByIndex(LONG lGoodsIdx,  LONG num)
{
	CSeekGoodsListener lSeeker;
	lSeeker.SetTarget(lGoodsIdx);
	getOriginPack()->TraversingContainer(&lSeeker);
	for( size_t i = 0; i < lSeeker.m_vGoodsID.size(); i ++ )
	{
		CGoods* pGoods = dynamic_cast<CGoods*>(getOriginPack()->Find(TYPE_GOODS, lSeeker.m_vGoodsID[i]));
		if( pGoods )
		{
			if(!getOriginPack()->IsLocked(pGoods)  && !IsInPersonalShop(pGoods) && (long)pGoods->GetAmount() >= num)
			{
				DeleteGoods( CS2CContainerObjectMove::PEI_PACKET, lSeeker.m_vGoodsID[i], num);
				return TRUE;
			}
		}
	}

	for(int i=CS2CContainerObjectMove::PEI_PACK1;i<=CS2CContainerObjectMove::PEI_PACK5;i++)
	{
		if(GetPackage()->GetSubpack(i-CS2CContainerObjectMove::PEI_PACK1)->pGoods)
		{			
			lSeeker.m_vGoodsID.clear();
			CVolumeLimitGoodsContainer* pContainer = GetPackage()->GetSubpack(i-CS2CContainerObjectMove::PEI_PACK1)->pSubpackContainer;
			pContainer->TraversingContainer(&lSeeker);
			for(size_t j=0;j<lSeeker.m_vGoodsID.size();j++)
			{
				CGoods* pGoods = dynamic_cast<CGoods*>(pContainer->Find(TYPE_GOODS, lSeeker.m_vGoodsID[j]));
				if( pGoods )
				{
					if(!pContainer->IsLocked(pGoods)  && !IsInPersonalShop(pGoods) && (long)pGoods->GetAmount() >= num)
					{
						DeleteGoods( (CS2CContainerObjectMove::PLAYER_EXTEND_ID)i, lSeeker.m_vGoodsID[j], num);
						return TRUE;
					}
				}
			}	
		}
	}

	return FALSE;
}

//! 按测试结果删除物品
void CPlayer::DelGoodsByTestResult(vector<tagDelTestResult> &vecDelResult)
{
	for (long i=0; i<(long)vecDelResult.size(); i++)
	{
		DelGoodsByTest(&vecDelResult[i]);
	}
}

DWORD CPlayer::FindGoods(LONG lContainerId,LONG lPos,const char* strGoodsName)
{
	if(NULL == strGoodsName)	return 0;
	DWORD dwNum=0;
	if(lContainerId==0) 
	{
		return CheckGoods(strGoodsName);
	}
	else
	{
		CGoods* pGoods=FindGoods(lContainerId,lPos);
		if(pGoods)
		{
			char *strName =(char*)(GoodsSetup::QueryGoodsOriginalName(pGoods->GetBasePropertiesIndex()));
			if(strName && strcmp(strGoodsName,strName) == 0)
				dwNum=pGoods->GetAmount();
		}
	}
	return dwNum;
}

DWORD CPlayer::FindGoodsByName(LONG lContainerId,const char* strGoodsName)
{
	CGoodsContainer* pContainer = FindContainer(lContainerId);
	if(pContainer)
	{
		return pContainer->GetGoodsAmountByName(strGoodsName);		
	}
	return 0;
}

DWORD CPlayer::FindGoodsByType(LONG lContainerId,int nType)
{
	CGoodsContainer* pContainer = FindContainer(lContainerId);
	if(pContainer)
	{
		return pContainer->GetGoodsAmountByType(nType);
	}
	return 0;
}

//玩家属性初始化
void CPlayer::InitProperty(void)
{
	PlayerList::SetBaseFightProperty(&m_Property,&m_BaseFightProperty);
	PlayerList::SetFightProperty(&m_Property,&m_BaseFightProperty,&m_FightProperty);
	if (GetState()==STATE_DIED)
	{
		SetHP(0);
	}
	UpdateHpMpRecoverSpeed();

	if(!GetIsRun())
		SetSpeed(GlobalSetup::GetSetup()->fPlayerWalkSpeed);
	else
		SetSpeed(GlobalSetup::GetSetup()->fPlayerRunSpeed);
}

//初始化赠送经验值
void CPlayer::InitPresentExp(void)
{
	CServerRegion* pServerRegion = dynamic_cast<CServerRegion*>(GetFather());
	if(NULL == pServerRegion)	return;

	time_t curTime;
	time(&curTime);
	//时间差分钟
	long lDiffMinute = (long)(curTime-m_Property.dwLastExitGameTime)/60;
	long lCooldown = GlobalSetup::GetSetup()->dwPresentExpCooldown;
	if(lDiffMinute > 0 && lDiffMinute > lCooldown)
	{
		//减去冷却时间
		lDiffMinute = lDiffMinute - lCooldown;
		DWORD dwPresentExpPerMinute = PlayerList::GetPresentExpPerMinute((uchar)GetLevel());
		DWORD dwMaxPresentExp = PlayerList::GetMaxPresentExp((uchar)GetLevel());
		DWORD dwCurPresentExp = lDiffMinute*dwPresentExpPerMinute;

		float fPresentExpScaleWithPlace = 1.0f;
		//个人副本
		if( GetCurRgnType() == ::RGN_PERSONAL_HOUSE )
			fPresentExpScaleWithPlace = GlobalSetup::GetSetup()->fPresentExpScaleInPersonalHouse;
		//安全区系数
		else if(pServerRegion->GetSecurity(GetTileX(),GetTileY() ) == CRegion::SECURTIY_SAFE)
			fPresentExpScaleWithPlace = GlobalSetup::GetSetup()->fPresentExpScaleInSafeArea;
		else
			fPresentExpScaleWithPlace = GlobalSetup::GetSetup()->fPresentExpScale;

		dwCurPresentExp = (DWORD)(dwCurPresentExp*fPresentExpScaleWithPlace*GetPresentExpScale());

		if(m_Property.dwPresentExp+dwCurPresentExp > dwMaxPresentExp)
			m_Property.dwPresentExp = dwMaxPresentExp;
		else
			m_Property.dwPresentExp += dwCurPresentExp;

		//赠送经验
		m_mapDirtyAttrData.insert("PresentExp");
		UpdateAttribute();		
	}
	//如果当前有赠送经验值,则加一个buff显示
	if(m_Property.dwPresentExp > 0)
	{
		AddNewState(this,GlobalSetup::GetSetup()->lPresentExpBuffId,GlobalSetup::GetSetup()->lPresentExpBuffLvl,14);
	}
	//重设下线时间
	SetLastExitGameTime((DWORD)curTime);
}

//重设战斗属性中的力量、敏捷、体质、智利、智慧、精神、生命值、法力值等基本属性
void CPlayer::ResetBaseFightProperty(void)
{
	PlayerList::SetBaseFightProperty(&m_Property,&m_BaseFightProperty);

	//力量
	AddAttributeToDirty("C_Strenth");
	//敏捷
	AddAttributeToDirty("C_Agility");
	//体质
	AddAttributeToDirty("C_Consti");
	//智利
	AddAttributeToDirty("C_Intell");
	//智慧
	AddAttributeToDirty("C_Wisdom");
	//精神
	AddAttributeToDirty("C_Spirit");
	//最大生命值
	AddAttributeToDirty("C_MaxHp");
	//当前生命值
	AddAttributeToDirty("Hp");
	//最大法力值
	AddAttributeToDirty("C_MaxMp");
	//当前法力值
	AddAttributeToDirty("Mp");
	//最大能量值
	AddAttributeToDirty("C_MaxEnergy");
}


//当力量,敏捷,体质,智利,智慧,精神的基本属性和变化属性发生改变的时候，
//都要重新计算格挡,招架,闪避等基本属性
void CPlayer::ResetFightProperty(void)
{
	//重新计算
	PlayerList::SetFightProperty(&m_Property,&m_BaseFightProperty,&m_FightProperty);
	if (GetState()==STATE_DIED)
	{
		SetHP(0);
	}
	UpdateHpMpRecoverSpeed();
	ProduceEvent(ET_ResetBaseFightProperty);
	//添加脏属性名字
	//格挡
	m_mapDirtyAttrData.insert("C_Block");
	//招架
	m_mapDirtyAttrData.insert("C_Parry");
	//闪避
	m_mapDirtyAttrData.insert("C_Dodge");
	//基本物理最小攻击
	m_mapDirtyAttrData.insert("C_MinAtk");
	//基本物理最大攻击
	m_mapDirtyAttrData.insert("C_MaxAtk");

	m_mapDirtyAttrData.insert("Hp");
	m_mapDirtyAttrData.insert("Mp");

	AddAttributeToDirty("C_MaxHp");
	AddAttributeToDirty("C_MaxMp");

	m_mapDirtyAttrData.insert( "CurPKCP" );
	AddAttributeToDirty( "C_MaxPKCP" );

	//物理防御
	m_mapDirtyAttrData.insert("C_Def");
	//物理爆击值
	m_mapDirtyAttrData.insert("C_Cri");
	//命中值
	m_mapDirtyAttrData.insert("C_Hit");
	//物爆伤害值
	m_mapDirtyAttrData.insert("C_CriDamage");;
	//物爆抗性值
	m_mapDirtyAttrData.insert("C_CriDef");
	//体质抗性值
	m_mapDirtyAttrData.insert("C_ConstiDef");
	//魔法攻击
	m_mapDirtyAttrData.insert("C_MAtk");
	//魔法爆击伤害
	m_mapDirtyAttrData.insert("C_MCriDamage");
	//意志抗性值
	m_mapDirtyAttrData.insert("C_WillDef");
	//魔法爆击抗性值
	m_mapDirtyAttrData.insert("C_MCriDef");
	//魔法抗性值
	m_mapDirtyAttrData.insert("C_Mdef");
	//魔法爆击值
	m_mapDirtyAttrData.insert("C_MCri");
	//升级经验
	m_mapDirtyAttrData.insert("UpgradeExp");
	//职业升级经验
	m_mapDirtyAttrData.insert("OccuUpgradeExp");
	//基本能量恢复值
	m_mapDirtyAttrData.insert("B_EnerRecoverSpeed");
	//战斗能量恢复值
	m_mapDirtyAttrData.insert("B_FightEnerRecoverSpeed");
}

void CPlayer::StudySkill(DWORD dwID,DWORD dwLv)
{
	AddSkill(dwID,dwLv);
	CNewSkill *pSkillProperty = CNewSkillFactory::QueryNewSkillProperty(dwID);

	if (NULL!=pSkillProperty)
	{
		if(pSkillProperty->GetSkillType()==ModuType_Skill_Passive)
		{
			OnBeginSkill(dwID,dwLv);				
		}
	}
	StudyKillScript(dwID,dwLv);	
}
bool CPlayer::CheckStudySkill(DWORD dwID,DWORD &dwSkillLv)
{
	//DWORD dwLv=0;	
	if (this->IsDied())
	{
		//死亡状态下不能升级技能
		return false;
	}
	if (dwSkillLv==0)
	{
		map<long,tagSkill>::iterator it = m_Skills.find(dwID);
		if (it!=m_Skills.end())
		{
			dwSkillLv = it->second.wLevel + 1;
		}
		else
		{
			dwSkillLv = 1;
		} 
	}


	//职业等级不符合条件
	if(!OccuLvJudgeStudy(dwID,dwSkillLv))
	{
		return false;
	}

	//职业不符合学习条件
	if (!OccuJudgeStudy(dwID,dwSkillLv))
	{
		return false;
	}

	//等级不符合学习条件	
	if (!LevelJudgeStudy(dwID,dwSkillLv))
	{
		return false;
	}



	//前置技能不符合学习条件	
	if (!SkillJudgeStudy(dwID,dwSkillLv))
	{
		return false;
	}
	//前置限制技能不符合条件
	if (!LimitSkillJudgeStudy(dwID,dwSkillLv))
	{
		return false;
	}

	//SP不符合学习条件		
	if (!SPJudgeStudy(dwID,dwSkillLv))
	{
		return false;
	}

	//符合所有学习技能条件
	CNewSkill *pSkillProperty = CNewSkillFactory::QueryNewSkillProperty(dwID);
	if (NULL!=pSkillProperty)
	{
		if (!GetInst(SkillAttribute).IsExist(dwID , dwSkillLv))
		{			
			return false;			
		}
	}
	else
	{
		return false;
	}
	//物品条件
	if (!GoodJudgeStudy(dwID,dwSkillLv))
	{
		return false;
	}
	return true;//可以学习
}


//学习技能触发脚本
void CPlayer::StudyKillScript(DWORD dwID,DWORD dwLv)
{
	CStudySkillCondition::tagCondition *ptgCondition = CStudySkillCondition::GetCondition(dwID,(WORD)dwLv);	
	if (ptgCondition!=NULL)
	{
		for (vector<CStudySkillCondition::tagConditionEx*>::iterator it = ptgCondition->vec_ConditionEx.begin();
			it!=ptgCondition->vec_ConditionEx.end();++it)
		{
			if ((*it)->wConditionExType==CStudySkillCondition::CONDITIONEX_SCRIPT)
			{
				PlayerRunScript((char*)(*it)->strValue.c_str());
			}
		}
	}	
}
//职业等级判断
bool CPlayer::OccuLvJudgeStudy(DWORD dwID,DWORD dwLv)
{
	char strTemp[256];
	string str="Occu";
	_snprintf(strTemp, 256, "OccuLvl%d", GetAttribute(str));
	string strOccuLv = strTemp;

	CStudySkillCondition::tagCondition *ptgCondition = CStudySkillCondition::GetCondition(dwID,(WORD)dwLv);
	if (ptgCondition!=NULL)
	{
		if((long)ptgCondition->dwOccuLv >	GetAttribute(strOccuLv))
		{
			StudySkillFailed(SKILL_STUDY_FAILED_OCCULV);
			return false;
		}
		return true;
	}	
	return false;
}

//职业学习条件
bool CPlayer::OccuJudgeStudy(DWORD dwID,DWORD dwLv)
{	
	string str="Occu";
	COccupSkill* pOccupSkill = COccupSkillList::GetOccupSkill(GetAttribute(str));
	if(pOccupSkill!=NULL)
	{
		vector<COccupSkill::tagOccupSkill*> vcOccupSkill = pOccupSkill->GetOccupSkill();
		vector<COccupSkill::tagOccupSkill*>::iterator it=vcOccupSkill.begin();
		for (;it!=vcOccupSkill.end();++it)
		{
			if (((*it)->lSkillID==(long)dwID)&&((*it)->lLeve>=(long)dwLv))
			{
				return true;
			}
		}
	}

	StudySkillFailed(SKILL_STUDY_FAILED_OCCUPATION);
	return false;
}
//等级学习条件	
bool CPlayer::LevelJudgeStudy(DWORD dwID,DWORD dwLv)
{
	string str="Level";
	CStudySkillCondition::tagCondition *ptgCondition = CStudySkillCondition::GetCondition(dwID,(WORD)dwLv);
	if (ptgCondition!=NULL)
	{
		eOccupation Occu = (eOccupation)GetOccupation();
		long lOccuLvlCoe = (long)PlayerList::GetOccuLvlCoe(Occu,GetOccuLvl(Occu));
		if(ptgCondition->wPlayerLev >	lOccuLvlCoe )
		{
			StudySkillFailed(SKILL_STUDY_FAILED_RANK);
			return false;
		}
		return true;
	}	

	return false;
}
//SP学习条件		
bool CPlayer::SPJudgeStudy(DWORD dwID,DWORD dwLv)
{

	CStudySkillCondition::tagCondition *ptgCondition = CStudySkillCondition::GetCondition(dwID,(WORD)dwLv);
	if (ptgCondition!=NULL)
	{
		if (ptgCondition->dwSp > GetOccuSp((eOccupation)GetOccupation()) )
		{
			StudySkillFailed(SKILL_STUDY_FAILED_SP);
			return false;
		}
		DecOccuSp((eOccupation)GetOccupation(),ptgCondition->dwSp);
		return true;
	}
	return false;


}
//前置技能学习条件	
bool CPlayer::SkillJudgeStudy(DWORD dwID,DWORD dwLv)
{
	//CONDITIONEX_PRESKILL,			// 附加前置技能
	CStudySkillCondition::tagCondition *ptgCondition = CStudySkillCondition::GetCondition(dwID,(WORD)dwLv);
	if (ptgCondition)
	{
		vector<CStudySkillCondition::tagConditionEx*>::iterator	it=ptgCondition->vec_ConditionEx.begin();
		for (;it!=ptgCondition->vec_ConditionEx.end();++it)
		{
			if ((CStudySkillCondition::eConditionEx)(*it)->wConditionExType==CStudySkillCondition::CONDITIONEX_PRESKILL)
			{
				DWORD dwSkillID = atol((*it)->strValue.c_str());//)
				DWORD dwSkillLv = (*it)->lValue;
				itSkill sit=m_Skills.find(dwSkillID);
				if (sit!=m_Skills.end())
				{
					if (sit->second.wLevel < dwSkillLv)
					{
						//当前技能的等级小于限制等级
						StudySkillFailed(SKILL_STUDY_FAILED_SKILL);
						return false;
					}					
				}
				else
				{
					//没有找到前置技能
					StudySkillFailed(SKILL_STUDY_FAILED_SKILL);
					return false;
				}

			}
		}
		return true;
	}

	return false;
}
//前置限制技能学习条件
bool CPlayer::LimitSkillJudgeStudy(DWORD dwID,DWORD dwLv)
{
	//CONDITIONEX_CONFLICTSKILL		// 附加限制技能
	CStudySkillCondition::tagCondition *ptgCondition = CStudySkillCondition::GetCondition(dwID,(WORD)dwLv);
	if (ptgCondition)
	{
		vector<CStudySkillCondition::tagConditionEx*>::iterator	it=ptgCondition->vec_ConditionEx.begin();
		for (;it!=ptgCondition->vec_ConditionEx.end();++it)
		{
			if ((CStudySkillCondition::eConditionEx)(*it)->wConditionExType==CStudySkillCondition::CONDITIONEX_CONFLICTSKILL)
			{
				DWORD dwSkillID = atol((*it)->strValue.c_str()); 			
				itSkill sit=m_Skills.find(dwSkillID);
				if (sit!=m_Skills.end())
				{
					//存在前置限制技能
					StudySkillFailed(SKILL_STUDY_FAILED_LIMIT_SKILL);
					return false;			
				}

			}
		}
		return true;
	}

	return false;
}

//物品条件判断
bool CPlayer::GoodJudgeStudy(DWORD dwID, DWORD dwLv)
{
	CStudySkillCondition::tagCondition *ptgCondition = CStudySkillCondition::GetCondition(dwID,(WORD)dwLv);
	if (ptgCondition)
	{
		vector<CStudySkillCondition::tagConditionEx*>::iterator	it=ptgCondition->vec_ConditionEx.begin();
		for (;it!=ptgCondition->vec_ConditionEx.end();++it)
		{
			if ((CStudySkillCondition::eConditionEx)(*it)->wConditionExType==CStudySkillCondition::CONDITIONEX_GOODS)
			{
				string strName = (*it)->strValue; 		

				if (strName == AppFrame::GetText("GS_GOODS_GOLD"))
				{
					//检查金币条件
					if((long)GetMoney()<(*it)->lValue)
					{						
						StudySkillFailed(SKILL_STUDY_FAILED_GOLD);
						return false;	
					}
					else
					{
						SetMoney(GetMoney()-(*it)->lValue,6);
					}
				} 
				else if(strName == AppFrame::GetText("GS_GOODS_SILVER"))
				{
					//检查银币条件
					if ((long)GetSilver()<(*it)->lValue)
					{
						StudySkillFailed(SKILL_STUDY_FAILED_SILVER);
						return false;
					} 
					else
					{
						SetSilver(GetSilver()-(*it)->lValue);
					}
				}
				else
				{
					//普通物品
					if((long)GetGoodsAmountByOriginName(strName.c_str()) < (*it)->lValue)
					{
						//物品
						StudySkillFailed(SKILL_STUDY_FAILED_GOOD);
						return false;			
					}
					else
					{
						DelGoods((char*)strName.c_str(),(*it)->lValue);
					}
				}

			}
		}
		return true;
	}	
	return false;

}
//学习技能失败
void CPlayer::StudySkillFailed(long lValue)
{

	CMessage msg(MSG_S2C_SKILL_STUDY_FAILED);
	msg.Add(lValue);
	msg.SendToPlayer(this -> GetExID(),false);
}
//返回技能学习列表
void CPlayer::SendBackSkill(string strName,CGUID guidNpc)
{
	map<string,CStudySkillCondition::tagTeachSkill*> mapSkillList = CStudySkillCondition::GetTeacherSkill(); 
	map<string,CStudySkillCondition::tagTeachSkill*>::iterator it=mapSkillList.find(strName);
	if (it!=mapSkillList.end())
	{
		CMessage msg(MSG_S2C_SKILL_STUDY_BEGIN);
		msg.Add(guidNpc);
		if (CareerJudge(strName))
		{
			msg.Add((long)(it->second->lSkillList.size()));
			for(list<DWORD>::iterator sit=it->second->lSkillList.begin();sit!=it->second->lSkillList.end();++sit)
			{
				CNewSkillProperties *pNewSkillProperties = CNewSkillFactory::QueryNewSkillProperty(*sit);
				if (pNewSkillProperties)
				{
					msg.Add(pNewSkillProperties->GetUseType());
				}
				else
				{
					msg.Add((BYTE)0);	
				}				
				msg.Add(*sit);
			}
		}
		else
		{
			msg.Add((long)0);
		}

		msg.SendToPlayer(this -> GetExID(),false);
	}


}
//显示学习技能的职业判断
bool CPlayer::CareerJudge(string strName)
{
	string str="Occu";
	long lOccuID = GetAttribute(str);
	map<string,CStudySkillCondition::tagTeachSkill*> mapSkillList = CStudySkillCondition::GetTeacherSkill(); 
	map<string,CStudySkillCondition::tagTeachSkill*>::iterator it=mapSkillList.find(strName);
	if (it!=mapSkillList.end())
	{

		for(list<DWORD>::iterator sit=it->second->lOccList.begin();sit!=it->second->lOccList.end();++sit)
		{
			if (lOccuID==(*sit))
			{
				return true;
			}
		}
	}
	return false;
}
//添加技能判断
bool CPlayer::CanAddSkill(DWORD dwID, DWORD dwLv)
{
	map<long,tagSkill>::iterator it = m_Skills.find(dwID);
	if (it!=m_Skills.end())
	{
		if (it->second.wLevel<dwLv)
		{
			//新加技能的等级高于原有技能
			m_Skills.erase(it);
			return true;
		} 
		else
		{
			return false;
		}
	}
	else
	{
		return true;
	}
}
//添加技能
void CPlayer::AddSkill(long id,long lv,long lCoolDown,time_t t_EndTime)
{	
	if (!CanAddSkill(id,lv))
	{
		return;
	}
	tagSkill tgSklll;

    if ( GetInst(SkillAttribute).IsExist(id,lv) )
    {
        tgSklll.dwID = id;
        tgSklll.wLevel = (WORD)lv;

        if (lCoolDown>0)
        {
            tgSklll.wCooldown = lCoolDown;
            tgSklll.dwCooldownStartTime = timeGetTime();
        } 
        else
        {
            tgSklll.wCooldown = 0;
            tgSklll.dwCooldownStartTime = 0;
        }

        tgSklll.tCooldownEndtime = t_EndTime;
        tgSklll.dwOldCooldown = GetSkillValue( id , lv, "CoolDown" );


        m_Skills[id] = tgSklll;	

        CMessage msg(MSG_S2C_PLAYER_ADDSKILL);
        msg.Add(this->GetExID());
        msg.Add((DWORD)id);
        msg.Add((WORD)lv);
        msg.Add((DWORD)tgSklll.dwOldCooldown);
        msg.Add((float)GetSkillValue( id , lv, "MinAtkDistance" ));
        msg.Add((float)GetSkillValue( id , lv, "MaxAtkDistance" ));
        msg.SendToAround(this);

        GetGame()->UpdateSavePlayer(this, SAVE_DETAIL_ATTRSKILLLIST);

       Log4c::Trace(ROOT_MODULE,"Add Skllid %d lvl %d successful!", id , lv  );

        /// @Expter 技能升级
        if( m_GeniusTable.size() > 0) 
            m_GeniusAction->AddSkill(id,lv);
    }
}



LONG CPlayer::CheckSkill(long id)
{
	itSkill  it = m_Skills.find(id);
	if (it != m_Skills.end())
	{
		return it->second.wLevel;
	}
	return 0;
}

//玩家的技能初始化
void CPlayer::InitSkills()
{

	BYTE eOcc = GetOccupation();
	switch(eOcc)
	{

	case OCC_Novice://初心者
		break;
	case OCC_Fighter://战士
		break;
	case OCC_Mystic://法师(人类法师)
		break;
	case OCC_Rogue:	//行者
		break;
	case OCC_Knight://骑士 
		break;
	case OCC_Warrior://斗士 
		break;
	case OCC_Scout://斥候 
		break;
	case OCC_Craftsman://艺人
		break;
	case OCC_Warlock://术士
		break;
	case OCC_Cleric://牧师
		break;
	case OCC_Paladin://圣骑士
		break;
	case OCC_DarkKnight://暗骑士
		break;
	case OCC_Gladiator://角斗士 
		break;
	case OCC_Berserker://狂战士 
		break;
	case OCC_Ranger://巡守 
		break;
	case OCC_Assassin://刺客 
		break;
	case OCC_BladeDancer://剑舞者 
		break;
	case OCC_Bard://吟游诗人 
		break;
	case OCC_Sorcerer://巫师 
		break;
	case OCC_Necromancer://亡灵巫师 
		break;
	case OCC_Bishop://主教 
		break;
	case OCC_Prophet://贤者(先知)
		break;
	}
}

////技能的选择逻辑
tagSkillID CPlayer::GetDefaultAttackSkillID()
{
	//PlayerList::eOccupation eOcc = (PlayerList::eOccupation)GetOccupation();
	BYTE eOcc = GetOccupation();

	switch(eOcc)
	{
	case OCC_Novice://初心者
		return SKILL_BASE_ATTACK;
		break;
	case OCC_Fighter://战士
		break;
	case OCC_Mystic://法师(人类法师)
		break;
	case OCC_Rogue:	//行者
		break;
	case OCC_Knight://骑士 
		break;
	case OCC_Warrior://斗士 
		break;
	case OCC_Scout://斥候 
		break;
	case OCC_Craftsman://艺人
		break;
	case OCC_Warlock://术士
		break;
	case OCC_Cleric://牧师
		break;
	case OCC_Paladin://圣骑士
		break;
	case OCC_DarkKnight://暗骑士
		break;
	case OCC_Gladiator://角斗士 
		break;
	case OCC_Berserker://狂战士 
		break;
	case OCC_Ranger://巡守 
		break;
	case OCC_Assassin://刺客 
		break;
	case OCC_BladeDancer://剑舞者 
		break;
	case OCC_Bard://吟游诗人 
		break;
	case OCC_Sorcerer://巫师 
		break;
	case OCC_Necromancer://亡灵巫师 
		break;
	case OCC_Bishop://主教 
		break;
	case OCC_Prophet://贤者(先知)
		break;

	}

	return SKILL_BASE_ATTACK;
}

//int CPlayer::GetSubpackCanPlace()
//{
//	for(int i=0;i<CSubpackContainer::SUBPACK_NUM;i++)
//	{
//		if(!m_pSubpackContainer->GetSubpack(i)->pSubpackContainer->IsFull())
//		{
//			return i;
//		}
//	}
//	return -1;
//}

CVolumeLimitGoodsContainer* CPlayer::GetPackContainerById(DWORD dwId)
{
	if(dwId==CS2CContainerObjectMove::PEI_PACKET)
	{
		return getOriginPack();
	}
	else if(dwId>=CS2CContainerObjectMove::PEI_PACK1 && dwId<=CS2CContainerObjectMove::PEI_PACK5)
	{
		return GetPackage()->GetSubpack(dwId-CS2CContainerObjectMove::PEI_PACK1)->pSubpackContainer;
	}
	return NULL;
}

//当速度发生改变以后触发
void CPlayer::OnSpeedChanged()
{
	//对移动进行验证
	ResetMoveValiData();
}
//响应位置改变
void CPlayer::OnPosChanged()
{
	GSTeam *pTeam = (GSTeam*)GetOrganiCtrl()->GetGameTeamByID(GetTeamID());
	if(NULL != pTeam)
	{
		//pTeam->OnPlayerIdioinfoChange(GetExID(), GSTeam::eMIS_MaxEnergy);
		pTeam->MarkPlayerPosChange(GetExID());
	}
}

void CPlayer::OnGetGoods(DWORD dwGoodsId,CGoods* pGoods)
{

	CGoodsBaseProperties* pProperty=GoodsSetup::QueryGoodsBaseProperties(dwGoodsId);
	if(!pProperty)
		return;	

	CGoodsFactory::RunScriptOnGet(dwGoodsId,this);

	//非叠加物品执行
	if(GoodsSetup::QueryGoodsMaxStackNumber(dwGoodsId)==1 && pGoods)
	{
		//! 得到即绑定的物品操作
		LONG lBindType = pGoods->GetAddonPropertyValues(GAP_BIND, 1);

		if(eBT_GetAndCantUntie == lBindType)//! 得到即绑定（且不能解绑）
		{
			pGoods->ChangeAttribute(GAP_BIND, eBT_CantUntie, 1, true);
			pGoods->UpdateAttribute(GetExID());
		}
		else if (eBT_GetAndBind == lBindType)//! 得到即绑定（可以解绑）
		{
			pGoods->ChangeAttribute(GAP_BIND, eBT_Bind, 1, true);
			pGoods->UpdateAttribute(GetExID());
		}

		//! 关于限制时间
		if(TestGoodsTimeout(pGoods))
		{
			//! 删除物品
			RegisterLimitGoodsTimerID(pGoods->GetExID(), 1);
		}
	}	
}

void CPlayer::OnLoseGoods(CGoods* pGoods)
{
	if(NULL == pGoods) return;

	pGoods->RunScriptOnLose(this);

	//! 删除与该物品有关的限时物品Timer
	CancelLimitGoodsTimer(pGoods->GetExID());

	m_GoodsTimerHandler.UnRegister( this, pGoods->GetExID() );
}

//! 响应仓库物品获得
void CPlayer::OnDepotGetGoods(CGoods* pGoods)
{

}
//! 响应仓库物品失去
void CPlayer::OnDepotLoseGoods(CGoods* pGoods)
{

}

BOOL CPlayer::IsEquipBusinessPack()
{
	for(int i=CS2CContainerObjectMove::PEI_PACK1;i<=CS2CContainerObjectMove::PEI_PACK5;i++)
	{

		CGoods* pGoods=GetPackage()->GetSubpack(i-CS2CContainerObjectMove::PEI_PACK1)->pGoods;
		if(pGoods)
		{
			LONG lVal=pGoods->GetAddonPropertyValues(GAP_PACK_TYPE,1);
			if(lVal==SPT_BUSINESS)
				return TRUE;
		}
	}
	return FALSE;
}

//! 删除物品并通知玩家
void CPlayer::DelGoodsAndSendMsg(long lContainerId, long lPos)
{
	//! 找到该物品的容器
	CGoodsContainer* pCGoodsContainer = FindContainer(lContainerId);
	if( NULL != pCGoodsContainer )
	{
		CGoods* pGoods = dynamic_cast<CGoods*>( pCGoodsContainer -> GetGoods(lPos) );
		assert(NULL != pGoods);

		//##删除
		CS2CContainerObjectMove comMsg;
		if( pCGoodsContainer -> Remove(pGoods, &comMsg) )
		{
			OnLoseGoods(pGoods);
			comMsg.SetOperation( CS2CContainerObjectMove::OT_DELETE_OBJECT );
			comMsg.SetSourceContainerExtendID( lContainerId );
			comMsg.Send( GetExID() );

			//##删除该物品?
#ifdef _GOODSLOG1_
			if(pGoods)
			{
				char pszGoodsLog[1024]="";			
				char srcGoodsGUID[50]="";
				pGoods->GetExID().tostring(srcGoodsGUID);					
				_snprintf(pszGoodsLog,1024,"[物品回收:CPlayer::DelGoodsAndSendMsg][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d]",
					pGoods->GetBasePropertiesIndex(),GoodsSetup::QueryGoodsName(pGoods->GetBasePropertiesIndex()),srcGoodsGUID,pGoods->GetAmount());
				CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
			}			
#endif
			CGoodsFactory::GarbageCollect( &pGoods ,21);
		}
	}
}

long	CPlayer::RegisterInvestTimer()
{
	tagTimerVar* pVar= OBJ_CREATE(tagTimerVar);
	pVar->TimerType=ChildObjType_InvestLimit;
	time_t t;
	tm* pTm=NULL;
	t=time(NULL);
	pTm=localtime(&t);
	pTm->tm_hour=0;
	pTm->tm_min=0;
	pTm->tm_sec=0;
	time_t t0=mktime(pTm);
	long lFuture=(24*60*60)-(long)(t-t0);
	m_lInvestLimitTimeMark= (long)t+lFuture;
	long lTimerId=GameManager::GetInstance()->Schedule(this,pVar,timeGetTime()+lFuture*1000);
	return lTimerId;
}

//! 为物品注册一个定时器事件ID，并返回该ID
long	CPlayer::RegisterLimitGoodsTimerID(const CGUID &GoodsGUID, ULONG dwFutureTime)
{
	//! m_PlayerTimer对象会自动释放pVar
	void* pVar =  OBJ_CREATE(tagTimerVar);
	/*assert(NULL != pVar);*/
	((tagTimerVar*)pVar)->TimerType = ChildObjType_GoodsTimeout;

	long lTimerID = GameManager::GetInstance()->Schedule(this, pVar, dwFutureTime + timeGetTime());
	m_GoodsTimerID[GoodsGUID] = lTimerID;
	return lTimerID;
}

// 注册‘星座能量’递减定时器
long CPlayer::RegisterGuardianEnergyDecTimer( const CGUID &goodsID )
{
	unsigned long lRestSec = GetRestSeconds();
	// 将秒数粗略为整数分钟，主要是为了让玩家背包内的所有该类物品采用一个定时器
	lRestSec = 60 - lRestSec % 60 + lRestSec;
 
	tagTimerVar *pVar = OBJ_CREATE(tagTimerVar);
	pVar->TimerType = ChildObjType_GoodsExTimeOut;
	pVar->lvar = GoodsExTimerType::GETT_GUARDIAN_ENERGY;
	pVar->lvar1 = lRestSec * 1000;

	long lTimerID = m_GoodsTimerHandler.Register( this, goodsID, pVar->lvar1, 24 * 60 * 60 * 1000, pVar ) ;
	if( lTimerID == 0 )
	{
		OBJ_RELEASE(tagTimerVar, pVar );
	}
	return lTimerID;
}

//! 取消一个物品的Timer
void CPlayer::CancelLimitGoodsTimer(const CGUID &GoodsGUID)
{
	map<CGUID, long>::iterator ite = m_GoodsTimerID.find(GoodsGUID);
	if (ite != m_GoodsTimerID.end())
	{
		void* pVar = NULL;
		GameManager::GetInstance()->Cancel(ite->second, (const void**)&pVar);
		/*assert(NULL != pVar);
		GameManager::GetInstance()->FreeBlock(pVar);*/
		m_GoodsTimerID.erase(ite);
	}
}

//! 响应定时器触发的物品删除事件
void CPlayer::OnLimitGoodsTimeout(long lTimerID)
{
	//! 找到对应的物品ID
	map<CGUID, long>::iterator ite = m_GoodsTimerID.begin();
	for (; ite != m_GoodsTimerID.end(); ++ite)
	{
		if (ite->second == lTimerID) break;
	}

	//! 如果找到
	if (ite != m_GoodsTimerID.end())
	{
		pair<long, long> Pos;
		if(FindGoodsByGuid(ite->first, Pos))
		{
			CGoods *pGoods = FindGoodByGuid(ite->first);
			if(NULL != pGoods)
			{
				char szInfo[128] = {0};
				_snprintf(szInfo, 128, "%s[%s]%s", AppFrame::GetText("52"), pGoods->GetName(), AppFrame::GetText("53"));
				//SendNotifyMessage( szInfo,0xffff0000,0,eNOTIFYPOS_LEFT_BOTTOM);	
				SendNotifyMessage(szInfo,eNOTIFYPOS_LEFT_BOTTOM);
			}

			//! 删除物品
			pair<long, long> GoodsPosInfo;
			if(FindGoodsByGuid(ite->first, GoodsPosInfo))
			{
				DelGoodsAndSendMsg(GoodsPosInfo.first, GoodsPosInfo.second);
			}
		}
	}
}

//void CPlayer::OnGuardianEnergyTimeOut( const CGUID &goodsID )
//{ 
//	CGoods *pGoods = FindGoodByGuid( goodsID );
//	if( pGoods != NULL )
//	{
//		long lOldValue = pGoods->GetGuardianEnergy();
//		// 降低‘星座能量’值
//		long lNewValue = pGoods->DecGuardianEnergy();
//		pGoods->UpdateAttribute( GetExID() );
//		// 检查此物品是否被玩家装备，需要改变属性
//		const CGoods *pEquip = m_cEquipment.GetGoods( EC_GUARDIAN );
//		if( pGoods == pEquip &&
//			lOldValue > 0 && lNewValue == 0 )
//		{
//			MountEquipEx( pGoods, false, true );	
//			UpdateAttribute( TRUE );
//		}
//	}
//}

PlayerDepot* CPlayer::GetPlayerDepot(void)
{
	return &m_pPlayerDepot;
}

//! 物品限制时间处理
//!	若未超时设置定时器，若超时，返回true，应当再接口外部删除物品
bool CPlayer::TestGoodsTimeout(CGoods* pGoods)
{
	//! 得到物品的限制时间属性
	vector<long> vOutProperty;
	pGoods->GetAddonPropertyValues(GAP_TIME_LIMIT, vOutProperty);

	//! 检测
	if (2 == vOutProperty.size())
	{
		DWORD dwValue1 = vOutProperty[0];
		DWORD dwValue2 = vOutProperty[1];

		switch(dwValue1)
		{
		case eGTL_Minute:
			{
				if (0 < dwValue2)
				{
					//! 修改物品属性
					pGoods->ChangeAttribute(GAP_TIME_LIMIT, (long)eGTL_Date, 1, true);
					pGoods->ChangeAttribute(GAP_TIME_LIMIT, dwValue2 * 60 + time(NULL), 2, true);
					

					//! 大于15天，不注册删除定时器
					if(15 > (dwValue2 / 24 / 60))
					{
						RegisterLimitGoodsTimerID(pGoods->GetExID(), dwValue2 * 60 * 1000);
					}
				}
				else
				{
					return true;
				}
			}
			break;
		case eGTL_Date:
			{
				DWORD dwCurrTime = (DWORD)time(NULL);
				if (dwCurrTime < dwValue2)
				{
					//! 大于15天，不注册删除定时器
					if(15 > ((dwValue2 - dwCurrTime) / 24 / 60))
					{
						RegisterLimitGoodsTimerID(pGoods->GetExID(), (dwValue2 - dwCurrTime) * 1000);
					}
				}
				else
				{
					return true;
				}
			}
			break;
		}
	}

	return false;
}

void CPlayer::CheckGoodsTimeOut(CGoods* pGoods)
{
    if (NULL != pGoods)
    {
        //! 关于限制时间
        if(TestGoodsTimeout(pGoods))
        {
            //! 删除物品
            pair<long, long> GoodsPosInfo;
            if(FindGoodsByGuid(pGoods->GetExID(), GoodsPosInfo))
            {
                DelGoodsAndSendMsg(GoodsPosInfo.first, GoodsPosInfo.second);
            }
        }

        // ’星座能量‘
    //    CheckGuardianEnergy( pGoods, true );
    }
}

//bool CPlayer::CheckGuardianEnergy( CGoods *pGoods, bool bLogin )
//{
//	if( pGoods->GetGoodsBaseType() != GT_GUARDIAN || 
//		!pGoods->HasAddonProperty( CGoodsBaseProperties::GAP_GUARDIAN_ENERGY ) )
//	{
//		return false;
//	} 
//
//	if( bLogin )
//	{
//		// 刚进入游戏需要检查能量值是否衰减
//		unsigned long uElapsedDays = GetElapsedDay( time( NULL ), (time_t) m_Property.dwLastExitGameTime );
//		if( uElapsedDays > 0 )
//		{
//			pGoods->DecGuardianEnergy( uElapsedDays );
//		} 
//	}
//
//	// 注册衰减定时器
//	RegisterGuardianEnergyDecTimer( pGoods->GetExID() );
//
//	return true;
//}

// 区域
void CPlayer::SetArea(CArea* p)
{
	m_pArea=p;

	// Fox@20081112 刷新角色场景和坐标
	GetInst(CPlayerMan).RefreshElem(PET_AREA, this);
}

void CPlayer::SetCredit(DWORD dwAreaId,DWORD dwCredit)
{
	m_mapCredit[dwAreaId]=dwCredit;
}

void CPlayer::AddCredit(DWORD dwAreaId,DWORD dwCredit)
{	
	map<DWORD,DWORD>::iterator it=m_mapCredit.find(dwAreaId);
	//09-9-14 增加最大上限声望的判定 by tanglei
	long lMaxCredit=GetInst(CCreditSystem).MaxAreaCredit();
	if(it!=m_mapCredit.end())
	{
		if ((long)(it->second+dwCredit)>lMaxCredit)
		{
			it->second=lMaxCredit;
		}
		else
			it->second+=dwCredit;
	}
	else
	{		
		m_mapCredit[dwAreaId]=dwCredit;
	}
}

void CPlayer::ChangeCredit(DWORD dwAreaId,long lCredit)
{
	map<DWORD,DWORD>::iterator it=m_mapCredit.find(dwAreaId);
	//09-9-14 增加最大上限声望的判定,不增加而是修改声望值 by tanglei
	long lMaxCredit=GetInst(CCreditSystem).MaxAreaCredit();
	if(it!=m_mapCredit.end())
	{
		if (lCredit>lMaxCredit)
		{
			it->second=lMaxCredit;
		}
		else
			it->second=lCredit;
		if(it->second<0)
			it->second=0;		
	}
	else
	{
		if(lCredit<0)
			lCredit=0;
		m_mapCredit[dwAreaId]=lCredit;
	}
}

DWORD CPlayer::GetCredit(DWORD dwAreaId)
{
	map<DWORD,DWORD>::iterator it=m_mapCredit.find(dwAreaId);
	if(it!=m_mapCredit.end())
		return it->second;
	return 0;
}

long CPlayer::GetCreditLevel(DWORD dwAreaID)
{
	return GetInst(CCreditSystem).CreditLevel( GetCredit(dwAreaID) );
}

long CPlayer::GetOrganLevel(const char* strName)
{
	long lValue=INVILID_VALUE;
	lValue=GetAttribute(string(strName));
	if (lValue==INVILID_VALUE)
		return 0;
	return GetInst(CCreditSystem).CreditOrganLeveyByDirtyName(strName,lValue);
}

void CPlayer::AddOrganCredit(const char* strName,DWORD dwCredit)
{	
	long lValue=INVILID_VALUE;
	lValue=GetAttribute(string(strName));
	if (lValue==INVILID_VALUE)
		return;
	//上限判定
	long MaxValue=GetInst(CCreditSystem).MaxOrganValByDirtyName(strName);
	if ((long)(lValue+dwCredit)>MaxValue)
	{
		SetAttribute(string(strName),MaxValue);
	}
	else
		SetAttribute(string(strName),lValue+dwCredit);
}

void CPlayer::ChangeOrganCredit(const char* strName,DWORD dwCredit)
{
	//上限判定
	long MaxValue=GetInst(CCreditSystem).MaxOrganValByDirtyName(strName);
	if ( (long)dwCredit>MaxValue)
	{
		SetAttribute(string(strName),MaxValue);
	}
	else
		SetAttribute(string(strName),dwCredit);
}

long CPlayer::GetOrganCredit(DWORD dwOrganID)
{
	//脏数据字段名
	const char* strDirtyName=GetInst(CCreditSystem).GetDirtyName(dwOrganID);
	long lValue=INVILID_VALUE;
	if (strDirtyName!=NULL)
	{
		lValue=GetAttribute(strDirtyName); 
	}
	if (lValue==INVILID_VALUE)
		return 0;
	else
		return lValue;
}

//声望
void CPlayer::CodeCreditToDataBlock(DBWriteSet& setWriteDB)
{
    setWriteDB.AddToByteArray((DWORD)m_mapCredit.size());
    map<DWORD,DWORD>::iterator creditIter=m_mapCredit.begin();
    for(;creditIter!=m_mapCredit.end();creditIter++)
    {
        setWriteDB.AddToByteArray(creditIter->first);
        setWriteDB.AddToByteArray(creditIter->second);
    }
}

void CPlayer::DecodeCreditFromDataBlock(DBReadSet& setReadDB)
{
    m_mapCredit.clear();
    DWORD dwCreditSize=setReadDB.GetDwordFromByteArray();
#ifdef _RUNSTACKINFO1_
	char pszStatckInfo[1024]="";
	_snprintf(pszStatckInfo,1024,"Decode Credit(size:%d) start.",dwCreditSize);
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStatckInfo);
#endif
    for(DWORD i=0;i<dwCreditSize;i++)
    {
        DWORD dwAreaId=setReadDB.GetDwordFromByteArray();
        DWORD dwCredit=setReadDB.GetDwordFromByteArray();
        m_mapCredit[dwAreaId]=dwCredit;
    }
}

//添加记录
void CPlayer::AddLimitGoodsRecord(DWORD dwGoodsId,DWORD dwGoodsNum)
{
	map<DWORD,DWORD>::iterator recordIt=m_mapLimitGoodsRecord.find(dwGoodsId);
	if(recordIt==m_mapLimitGoodsRecord.end())
		m_mapLimitGoodsRecord[dwGoodsId]=dwGoodsNum;
	else	
		recordIt->second+=dwGoodsNum;

	// 更新玩家属性
	GetGame()->UpdateSavePlayer(this, SAVE_DETAIL_ATTRPROPERTY);
}

//获取购买数量
DWORD CPlayer::GetLimitGoodsBuyNum(DWORD dwGoodsId)
{
	DWORD dwNum=0;
	map<DWORD,DWORD>::iterator recordIt=m_mapLimitGoodsRecord.find(dwGoodsId);
	if(recordIt!=m_mapLimitGoodsRecord.end())
		dwNum=recordIt->second;
	return dwNum;
}
BOOL CPlayer::SetInvestRecord(DWORD dwSpotId,DWORD dwInvestVal)
{	
	map<DWORD,DWORD>::iterator it=m_mapInvestRecord.find(dwSpotId);
	if(it!=m_mapInvestRecord.end())
		return FALSE;
	if(m_mapInvestRecord.size()==0)
	{
		RegisterInvestTimer();
	}
	m_mapInvestRecord[dwSpotId]=dwInvestVal;
	return TRUE;
}

DWORD CPlayer::GetInvestRecord(DWORD dwSpotId)
{
	map<DWORD,DWORD>::iterator it=m_mapInvestRecord.find(dwSpotId);
	if(it!=m_mapInvestRecord.end())
		return it->second;
	return 0;
}

void CPlayer::PushbakNpcShopSellGoods(CGoods* pGoods,DWORD dwPrice)
{
	if(!pGoods)
		return;
	CPlayer::tagShopSelledGoods stSelledGoods;
	stSelledGoods.pGoods=pGoods;
	stSelledGoods.dwSelledPrice=dwPrice;
	if(m_listNpcShopSellGoods.size()>=10)
	{
		list<CPlayer::tagShopSelledGoods>::iterator it=m_listNpcShopSellGoods.begin();		
#ifdef _GOODSLOG1_
		if((*it).pGoods)
		{
			char pszGoodsLog[1024]="";			
			char srcGoodsGUID[50]="";
			(*it).pGoods->GetExID().tostring(srcGoodsGUID);					
			_snprintf(pszGoodsLog,1024,"[物品回收:CPlayer::PushbakNpcShopSellGoods][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d]",
				(*it).pGoods->GetBasePropertiesIndex(),GoodsSetup::QueryGoodsName((*it).pGoods->GetBasePropertiesIndex()),srcGoodsGUID,(*it).pGoods->GetAmount());
			CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
		}

#endif
		CGoodsFactory::GarbageCollect(&((*it).pGoods),22);
		m_listNpcShopSellGoods.erase(it);

		m_listNpcShopSellGoods.push_back(stSelledGoods);
	}
	else
	{
		m_listNpcShopSellGoods.push_back(stSelledGoods);
	}
}

CGoods* CPlayer::FetchNpcShopSellGoods(const CGUID& guid)
{
	CGoods* pGoods=NULL;
	list<CPlayer::tagShopSelledGoods>::iterator it=m_listNpcShopSellGoods.begin();
	for(;it!=m_listNpcShopSellGoods.end();it++)
	{
		if(guid==((*it).pGoods)->GetExID())
		{
			pGoods=(*it).pGoods;
			m_listNpcShopSellGoods.erase(it);
			break;
		}
	}
	return pGoods;
}

CGoods* CPlayer::FindNpcShopSellGoods(CGUID& guid,DWORD* dwPrice)
{
	CGoods* pGoods=NULL;
	*dwPrice=0;
	list<CPlayer::tagShopSelledGoods>::iterator it=m_listNpcShopSellGoods.begin();
	for(;it!=m_listNpcShopSellGoods.end();it++)
	{
		if(guid==((*it).pGoods)->GetExID())
		{
			pGoods=(*it).pGoods;
			*dwPrice=(*it).dwSelledPrice;			
			break;
		}
	}
	return pGoods;
}

void CPlayer::ReleaseNpcShopSellGoods()
{
	list<CPlayer::tagShopSelledGoods>::iterator it=m_listNpcShopSellGoods.begin();
	for(;it!=m_listNpcShopSellGoods.end();it++)
	{
		CGoods* pGoods=(*it).pGoods;
		if(pGoods)
		{
#ifdef _GOODSLOG1_			
			char pszGoodsLog[1024]="";			
			char srcGoodsGUID[50]="";
			pGoods->GetExID().tostring(srcGoodsGUID);					
			_snprintf(pszGoodsLog,1024,"[物品回收:CPlayer::ReleaseNpcShopSellGoods][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d]",
				pGoods->GetBasePropertiesIndex(),GoodsSetup::QueryGoodsName(pGoods->GetBasePropertiesIndex()),srcGoodsGUID,pGoods->GetAmount());
			CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
#endif
			CGoodsFactory::GarbageCollect(&((*it).pGoods),23);
		}
	}
	m_listNpcShopSellGoods.clear();
}

BOOL CPlayer::UpdateCooldown(DWORD dwGoodsId)
{
	CGoodsBaseProperties* pProperty=GoodsSetup::QueryGoodsBaseProperties(dwGoodsId);
	if(pProperty)
	{
		//冷却时间		
		DWORD dwCoolDownTime=pProperty->GetAddonPropertyValue(GAP_COOLDOWN,1);
		DWORD dwCoolDownId=pProperty->GetAddonPropertyValue(GAP_COOLDOWN,2);
		if(dwCoolDownId!=0)
		{						
			tagGoodsCommonCooldown stCooldown;
			ZeroMemory(&stCooldown,sizeof(stCooldown));
			stCooldown.dwCooldownTime=dwCoolDownTime;
			stCooldown.dwCooldownStartTime=timeGetTime();
			m_mapClassIdCoolDown[dwCoolDownId]=stCooldown;
		}
		else if(dwCoolDownTime!=0)
		{
			tagGoodsCommonCooldown stCooldown;
			ZeroMemory(&stCooldown,sizeof(stCooldown));
			stCooldown.dwCooldownTime=dwCoolDownTime;
			stCooldown.dwCooldownStartTime=timeGetTime();
			m_mapGoodsIdCoolDown[dwGoodsId]=stCooldown;
		}
		return TRUE;
	}
	return FALSE;
}

//回购
void CPlayer::CodeNpcShopSellGoodsToDataBlock(DBWriteSet& setWriteDB)
{
#ifdef _RUNSTACKINFO1_
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"Code NpcShopSell Start");
#endif

    setWriteDB.AddToByteArray((DWORD)m_listNpcShopSellGoods.size());
    list<tagShopSelledGoods>::iterator sellGoodsIter= m_listNpcShopSellGoods.begin();
    for(;sellGoodsIter!=m_listNpcShopSellGoods.end();sellGoodsIter++)
    {
        CGoods* pGoods=(*sellGoodsIter).pGoods;
        if(pGoods)			
        {
            pGoods->Serialize(setWriteDB);
            setWriteDB.AddToByteArray((*sellGoodsIter).dwSelledPrice);
        }
    }
}

void CPlayer::DecodeNpcShopSellGoodsFromDataBlock(DBReadSet& setReadDB)
{
    DWORD dwSellGoodsSize=setReadDB.GetDwordFromByteArray();
#ifdef _RUNSTACKINFO1_
    char pszStatckInfo[1024];
    _snprintf(pszStatckInfo,1024,"Decode dwSellGoodsSize(Size:%d) start.",dwSellGoodsSize);
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStatckInfo);
#endif
    for(DWORD i=0;i<dwSellGoodsSize;i++)
    {
        CGoods* pGoods = CGoodsFactory::UnserializeGoods( setReadDB,7 );
        DWORD dwSellPrice=setReadDB.GetDwordFromByteArray();
        if( pGoods )
        {
            PushbakNpcShopSellGoods(pGoods,dwSellPrice);
        }
    }
}

//! 找到容器对象
CGoodsContainer*	CPlayer::FindContainer(long lContainerId)
{
	switch( lContainerId )
	{
	case CS2CContainerObjectMove::PEI_PACKET:
		return getOriginPack();
	case CS2CContainerObjectMove::PEI_EQUIPMENT:
		return &m_cEquipment;	
    case CS2CContainerObjectMove::PEI_EQUIP_GOD:
        return &m_cEquipmentGod;
    case CS2CContainerObjectMove::PEI_EQUIP_LAND:
        return &m_cEquipmentLand;
    case CS2CContainerObjectMove::PEI_EQUIP_HUMAN:
        return &m_cEquipmentHuman;
	case CS2CContainerObjectMove::PEI_PACK:
		return GetPackage()->GetSubpackContainer();
	case CS2CContainerObjectMove::PEI_PACK1:
		return GetPackage()->GetSubpack(0)->pSubpackContainer;
	case CS2CContainerObjectMove::PEI_PACK2:
		return GetPackage()->GetSubpack(1)->pSubpackContainer;
	case CS2CContainerObjectMove::PEI_PACK3:
		return GetPackage()->GetSubpack(2)->pSubpackContainer;
	case CS2CContainerObjectMove::PEI_PACK4:
		return GetPackage()->GetSubpack(3)->pSubpackContainer;
	case CS2CContainerObjectMove::PEI_PACK5:
		return GetPackage()->GetSubpack(4)->pSubpackContainer;
	}

	return NULL;
}

//! 通过物品索引，在背包和子背包中找到第一个一个物品的信息
BOOL CPlayer::FindFirstGoodsByIndex(LONG lGoodsIndex, tagGoodsParam &GoodsParam)
{
	//! 要搜索的容器列表
	static LONG arrContainerID[] = 
	{
		CS2CContainerObjectMove::PEI_PACKET,
		CS2CContainerObjectMove::PEI_PACK1,
		CS2CContainerObjectMove::PEI_PACK2,
		CS2CContainerObjectMove::PEI_PACK3,
		CS2CContainerObjectMove::PEI_PACK4,
		CS2CContainerObjectMove::PEI_PACK5,
	};

	//! 要搜索的容器个数
	static long lContainerNum = sizeof(arrContainerID) / sizeof(LONG);

	CGoods						*pGoods		= NULL;
	DWORD						dwPos		= 0;
	CVolumeLimitGoodsContainer	*pContainer = NULL;

	for (LONG i = 0; i < lContainerNum; ++i)
	{
		CVolumeLimitGoodsContainer *pContainer = (CVolumeLimitGoodsContainer*)FindContainer(arrContainerID[i]);
		if (NULL != pContainer)
		{
			pGoods = pContainer->GetTheFirstGoods(lGoodsIndex, m_vecInPersonalShopGoods);
			if(NULL != pGoods)
			{
				pContainer->QueryGoodsPosition(pGoods, dwPos);
				GoodsParam.wPlace		= (WORD)arrContainerID[i];
				GoodsParam.lPos			= (BYTE)dwPos;
				GoodsParam.TargetGuid	= pGoods->GetExID();
				GoodsParam.lTargetType	= pGoods->GetGoodsBaseType();

				GoodsParam.lCoolingTime	= pGoods->GetAddonPropertyValues(GAP_COOLDOWN, 1);
				LONG lCoolingType = pGoods->GetAddonPropertyValues(GAP_COOLDOWN, 2);
				if(GoodsParam.lCoolingType != lCoolingType)
				{
					GoodsParam.lLastTime	= 0;
					GoodsParam.lCoolingType = lCoolingType;
				}

				return TRUE;
			}
		}
	}

	return FALSE;
}

//! 通过物品guid找到物品所在位置
//! 返回值：pair<容器ID，物品位置>
bool CPlayer::FindGoodsByGuid(const CGUID &GoodsGUID,  pair<long, long> &ReValue)
{
	//! 要搜索的容器列表
	static long arr_lContainerID[] = {
		CS2CContainerObjectMove::PEI_PACKET,	//! 普通背包
		CS2CContainerObjectMove::PEI_EQUIPMENT,	//! 装备栏
		CS2CContainerObjectMove::PEI_PACK,		//! 子背包容器
		CS2CContainerObjectMove::PEI_PACK1,		//! 子背包栏位1
		CS2CContainerObjectMove::PEI_PACK2,		//! 子背包栏位2
		CS2CContainerObjectMove::PEI_PACK3,		//! 子背包栏位3
		CS2CContainerObjectMove::PEI_PACK4,		//! 子背包栏位4
		CS2CContainerObjectMove::PEI_PACK5,		//! 子背包栏位5
	};
	//! 要搜索的容器个数
	static long lContainerNum = sizeof(arr_lContainerID) / sizeof(long);

	//! 开始查找
	for (long i = 0; i < lContainerNum; ++i)
	{
		CGoodsContainer *pContainer = FindContainer(arr_lContainerID[i]);
		if (NULL != pContainer)
		{
			DWORD dPos = 0;
			if(pContainer->QueryGoodsPosition(GoodsGUID, dPos))
			{
				ReValue.first	= arr_lContainerID[i];
				ReValue.second	= dPos;
				return true;
			}
		}
	}

	return false;
}


bool CPlayer::CodeToDataBlock(DBWriteSet& setWriteDB, bool bExData)
{
#ifdef _RUNSTACKINFO1_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CPlayer::CodeToDataBlock() Start");
#endif
	//! 开始编码
	CMoveShape::CodeToDataBlock(setWriteDB, bExData);

	if( bExData )
	{
		setWriteDB.AddToByteArray((BYTE*)&m_Property, (long)sizeof(tagPlayerProperty));
		//setWriteDB.AddToByteArray(&m_arrElementProperty, sizeof(tagPlayerElementProperty) * MT_Max);
		setWriteDB.AddToByteArray( (char*)GetAccount());
		setWriteDB.AddToByteArray( GetTeamID());

        CodeSkillsToDataBlock(setWriteDB);
        //天赋编码Ws
        CodeGeniusToDataBlock(setWriteDB);
        //结义金兰
        CodeBrotherToDataBlock(setWriteDB);

#ifdef _RUNSTACKINFO1_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"Code Buff Start");
#endif
		//##添加异常状态
		CodeBuffToDataBlock( setWriteDB );


		setWriteDB.AddToByteArray(m_lInvestLimitTimeMark);
		setWriteDB.AddToByteArray((DWORD)m_mapInvestRecord.size());
		map<DWORD,DWORD>::iterator recordIter=m_mapInvestRecord.begin();
		for(;recordIter!=m_mapInvestRecord.end();recordIter++)
		{
			setWriteDB.AddToByteArray(recordIter->first);
			setWriteDB.AddToByteArray(recordIter->second);
		}

#ifdef _RUNSTACKINFO1_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"Code Equipment Start");
#endif
		m_cEquipment.Serialize( setWriteDB, bExData );
        m_cEquipmentGod.Serialize(setWriteDB, bExData);
        m_cEquipmentLand.Serialize(setWriteDB, bExData);
        m_cEquipmentHuman.Serialize(setWriteDB, bExData);

#ifdef _RUNSTACKINFO1_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"Code Medal Start");
#endif
		m_MedalContainer.CodeToDataBlock(setWriteDB);

        // 背包
        m_pPackage.CodeToDataBlockForServer(setWriteDB);
        // 声望
        CodeCreditToDataBlock(setWriteDB);
		//回购
        CodeNpcShopSellGoodsToDataBlock(setWriteDB);

#ifdef _RUNSTACKINFO1_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"Code GoodsCooldown Start");
#endif
		CodeGoodsCooldown(setWriteDB);

		setWriteDB.AddToByteArray(m_lTicket);

#ifdef _RUNSTACKINFO1_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"Code Depot Start");
#endif
		//! 仓库
		m_pPlayerDepot.CodeToDataBlock_ForServer(setWriteDB);

#ifdef _RUNSTACKINFO1_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"Code SpriteSystem Start");
#endif
		//! 小精灵系统
		m_SpriteSystem.CodeToDataBlock(setWriteDB);
		//! 玩家需要保存在服务器的设置
		m_SetupOnServer.CodeToDataBlock(setWriteDB);


		//VarialbeList
		//if(m_pVariableList)
		//	m_pVariableList->CodeToDataBlock(setWriteDB);
		VarList::Encode( m_VarList, setWriteDB );

		//SilenceTime
		setWriteDB.AddToByteArray(GetSilence());

		//GodMode
        setWriteDB.AddToByteArray((static_cast<char>(IsGod() ? 1:0)));

		//##杀人时间
		if( m_dwMurdererTimeStamp )
		{
			DWORD dwMurdererTimeLeft = GlobalSetup::GetSetup()->dwOnePkCountTime - ( timeGetTime() - m_dwMurdererTimeStamp );
			if (dwMurdererTimeLeft > GlobalSetup::GetSetup()->dwOnePkCountTime)
				dwMurdererTimeLeft = GlobalSetup::GetSetup()->dwOnePkCountTime;

			setWriteDB.AddToByteArray( dwMurdererTimeLeft );
		}
		else 
		{
			setWriteDB.AddToByteArray( static_cast<DWORD>(0) );
		}

		//##战斗状态的剩余时间
		setWriteDB.AddToByteArray( m_lFightStateCount );

		//添加是否登陆
		setWriteDB.AddToByteArray( (char)m_bLogin);

		//添加城战死亡状态时间
		//setWriteDB.AddToByteArray( m_lCityWarDiedStateTime );

#ifdef _RUNSTACKINFO1_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"Code Quest Start");
#endif
		//添加任务数据
		CodeQuestDataToDataBlock(setWriteDB);
		//添加俑兵任务数据
		CodeMerQuestDataToDataBlock(setWriteDB);

#ifdef _RUNSTACKINFO1_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"Code OccuRecord Start");
#endif
		//添加副职业数据
		m_DOccuRecord.CodeToDataBlock(setWriteDB);

#ifdef _RUNSTACKINFO1_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"Code PersonalRgn Start");
#endif
		// 个人副本
		setWriteDB.AddToByteArray( GetPersonalRgnGUID());
		// 个人房屋GUID
		setWriteDB.AddToByteArray( GetPersonalHouseRgnGUID());
		// 组队副本
		//GameManager::GetInstance()->GetRgnManager()->RgnGuidCode(GetExID(), setWriteDB);
		// 当前个人副本数
		setWriteDB.AddToByteArray( this->GetCurPRgnNum());
		// 当前队伍副本数
		setWriteDB.AddToByteArray( this->GetCurTRgnNum());

		//-------------------------------------------------------------------------------
		// 最近购买的10种商品列表. by Fox.		2008-02-21
        CodeIncShopToDataBlock(setWriteDB);

		//! 当前称号类型
		setWriteDB.AddToByteArray( GetTitleType());
		// 是否第一次登录
		setWriteDB.AddToByteArray( (BYTE)GetFirstLogin());

#ifdef _RUNSTACKINFO1_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"Code SysMail Start");
#endif
		//系统邮件列表
		setWriteDB.AddToByteArray((long)m_mapSysMail.size());
		map<long,long>::iterator mit=m_mapSysMail.begin();
		for (; mit!=m_mapSysMail.end(); ++mit)
		{
			setWriteDB.AddToByteArray((*mit).first);
			setWriteDB.AddToByteArray((*mit).second);			
		}

		// 宠物列表
		GetInst(CPetCtrl).CodeToDataBlock(setWriteDB, this);

		// 商业背包
		m_BusinessContainer.EncodeToSave( setWriteDB );

		// 保存状态
		setWriteDB.AddToByteArray((long)m_wState);

		setWriteDB.AddToByteArray((BYTE)GetShowFashion());
		
		/// 添加反外挂模块,退出进行玩家编码  @Expter20090602 
		GetInst(AntiCheat::CPlayerManager).PlayerEncode(this,setWriteDB);
	}

#ifdef _RUNSTACKINFO1_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CPlayer::CodeToDataBlock() End");
#endif
	return true;
}

bool CPlayer::CodeToDataBlock_ForClient(DBWriteSet& setWriteDB, bool bExData)
{
#ifdef _RUNSTACKINFO1_
	char pszStatckInfo[1024]="";
	_snprintf(pszStatckInfo,1024,"CPlayer::CodeToDataBlock_ForClient(name:%s) Start",GetName());
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStatckInfo);
#endif
	CMoveShape::CodeToDataBlock_ForClient( setWriteDB, bExData );

	if( bExData )
	{
#ifdef _RUNSTACKINFO1_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"Code Property Start");
#endif
		setWriteDB.AddToByteArray( (BYTE*)&m_Property, (long)sizeof(tagPlayerProperty));//-sizeof(string)*2
		//基本战斗属性
		CodeChangeAttrToDataBlock(setWriteDB);
		setWriteDB.AddToByteArray(&m_arrElementProperty, sizeof(tagPlayerElementProperty) * MT_Max);
		m_mapDirtyAttrData.clear();
#ifdef _RUNSTACKINFO1_
		_snprintf(pszStatckInfo,1024,"Code Skill(Num:%d) Start",GetNumSkills());
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStatckInfo);
#endif

		SetShowFashion(GetShowFashion());
		setWriteDB.AddToByteArray(GetShowFashion());

		//##添加,技能个数		
		CodeSkillsToDataBlock_ForClient(setWriteDB, TRUE);
        //添加天赋
        CodeGeniusToDataBlock_ForClient(setWriteDB,TRUE);
        // 结义金兰
        // __FOX__: 好友列表可缓发, 因为当打开好友界面时需刷新状态。
        CodeBrotherToDataBlock(setWriteDB);
        // 声望
        CodeCreditToDataBlock(setWriteDB);
		//共用冷却
        CodeGoodsColldownForClient(setWriteDB);

#ifdef _RUNSTACKINFO1_
		_snprintf(pszStatckInfo,1024,"Code Equipment(Num:%d) Start",m_cEquipment.GetEquipAmount());
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStatckInfo);
#endif
		////add equipments
		setWriteDB.AddToByteArray((WORD)(m_cEquipment.GetEquipAmount()));
		for(long i=0;i< (long)CEquipmentContainer::s_dwEquipmentLimit;i++)
		{
			if(m_cEquipment.GetGoods(DWORD(i)))
			{				
				m_cEquipment.GetGoods(DWORD(i))->SerializeForOldClient(setWriteDB);
				setWriteDB.AddToByteArray((WORD)i);
			}
		}

        m_cEquipmentGod.SerializeForClient(setWriteDB);
        m_cEquipmentLand.SerializeForClient(setWriteDB);
        m_cEquipmentHuman.SerializeForClient(setWriteDB);

		m_MedalContainer.CodeToDataBlock(setWriteDB);

		//背包
        m_pPackage.CodeToDataBlockForClient(setWriteDB);

		setWriteDB.AddToByteArray(GetTicket());	
#ifdef _RUNSTACKINFO1_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"Code Depot Start");
#endif
		//! 仓库数据
        // __FOX__: 编解码可缓
		m_pPlayerDepot.CodeHeadToDataBlock_ForClient(setWriteDB);

#ifdef _RUNSTACKINFO1_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"Code OrgSys Start");
#endif
		//! 组织系统
		CodeOrgSysToDataBlock(setWriteDB, bExData);
		//! 当前使用的称号类型
		setWriteDB.AddToByteArray( m_lCurrTitleType);
		//! 当前称号
		setWriteDB.AddToByteArray( m_AppendProperty.strTitle);
		//GM隐蔽
		setWriteDB.AddToByteArray((BYTE)m_bGMConcealment);
		//! 小精灵系统
		/////////////////////////////////////////////////////////
		////! -------------系统配置-------------
		SpriteSystem::CodeSetupToDataBlock(setWriteDB);

		////! -------------玩家数据-------------
		m_SpriteSystem.CodeToDataBlock(setWriteDB);
		/////////////////////////////////////////////////////////

		//! 玩家需要保存在服务器的设置
		m_SetupOnServer.CodeToDataBlock(setWriteDB);

		CodeQuestDataToDataBlock_ForClient(setWriteDB);
		//添加副职业数据
		m_DOccuRecord.CodeToDataBlockForClient(setWriteDB);
		//////////////////////////////////////////////////////////////////////////

#ifdef _RUNSTACKINFO1_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"Code GSTeam Start");
#endif
		//! 发送组队配置到客户端
		GSTeam::SendAboutSkillToC(setWriteDB);
		//! 招募信息
		CodeRecruitToDataBlock_ForClient(setWriteDB);

		// Fox@20081226 宠物列表
		GetInst(CPetCtrl).CodeToDataBlockForClient(setWriteDB, this);
		// 商业背包数据
        m_BusinessContainer.CodeForClient(setWriteDB);

		if(m_Property.lMarriageStep >= GlobalSetup::GetSetup()->lShowSpouseNameStep)
			setWriteDB.AddToByteArray( (BYTE) true );
		else
			setWriteDB.AddToByteArray( (BYTE) false );
	}
	else
	{
        // __FOX__: 针对玩家本人及他人的编解码可以合并
		/*
		EC_HEAD,				//	1：头盔
		EC_NECKLACE,			//	2：项链
		EC_WING,				//	3：翅膀
		EC_BODY,				//	4：盔甲
		EC_BACK,				//	5：腰带
		EC_GLOVE,			//6：手套	
		EC_BOOT,			//7：鞋子
		EC_HEADGEAR,			//	8：头饰
		EC_FROCK,			//	9：外套		
		EC_LRING,			//	10：左戒指
		EC_RRING,			//	11：右戒指		
		EC_MEDAL1,			//	12：勋章1
		EC_MEDAL2,			//	13	勋章2
		EC_MEDAL3,			//	14	勋章3
		EC_FAIRY,			//	15：小精灵
		EC_WEAPON,			//	16：武器
		EC_WEAPON2,			//	17：副手武器
		*/
		setWriteDB.AddToByteArray( GetHeadPic());
		setWriteDB.AddToByteArray( GetFacePic() );
		setWriteDB.AddToByteArray( GetShowFashion());
        for(long l=EC_E_HEAD; l<=EC_P_LEG; l++)
        {
            CGoods* tGoods = m_cEquipment.GetGoods(l);
            setWriteDB.AddToByteArray(tGoods?tGoods->GetBasePropertiesIndex():0 );
            setWriteDB.AddToByteArray(tGoods?tGoods->GetAddonPropertyValues(GAP_WEAPON_LEVEL, 1):0 );
        }
		

		setWriteDB.AddToByteArray( (BYTE)m_Property.bShowCountry);
		//////////////////////////////////////////////////////////////////////////
		//  [4/18/2007 chenxianjin]
		setWriteDB.AddToByteArray( GetHP());
		setWriteDB.AddToByteArray( GetMaxHP());
		setWriteDB.AddToByteArray( GetPkValue());//pk值
		setWriteDB.AddToByteArray( GetPVPValue());//恶名值
		//////////////////////////////////////////////////////////////////////////

		//! 组织系统
		CodeOrgSysToDataBlock(setWriteDB, bExData);

		//! 当前使用的称号类型
		setWriteDB.AddToByteArray( m_lCurrTitleType);
		//! 当前称号
		setWriteDB.AddToByteArray( m_AppendProperty.strTitle);
		//GM隐蔽
		setWriteDB.AddToByteArray((BYTE)m_bGMConcealment);

		// 职业+性别
		//  [4/18/2007 chenxianjin]
		setWriteDB.AddToByteArray( GetOccupation());
		setWriteDB.AddToByteArray( GetSex());
		setWriteDB.AddToByteArray( GetDOccupation());

		//个人商店的信息
        CodePersonalShopForClient(setWriteDB);
		//##表情信息
		setWriteDB.AddToByteArray( m_lEmotionIndex );
		setWriteDB.AddToByteArray( timeGetTime() - m_dwEmotonTimeStamp );
		// 国家
		setWriteDB.AddToByteArray( m_Property.byCountry );
		// 等级
		setWriteDB.AddToByteArray( GetLevel() );
		//! 招募信息
		CodeRecruitToDataBlock_ForClient(setWriteDB);
		// PK CP值
		setWriteDB.AddToByteArray( GetPKCP() );
		setWriteDB.AddToByteArray( GetMaxPKCP() );
        // 移动目标点
        setWriteDB.AddToByteArray( m_mover.GetDestX() );
        setWriteDB.AddToByteArray( m_mover.GetDestY() );
	}

#ifdef _RUNSTACKINFO1_
	_snprintf(pszStatckInfo,1024,"CPlayer::CodeToDataBlock_ForClient(name:%s) End",GetName());
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStatckInfo);
#endif
	return true;
}

void CPlayer::CodeRecruitToDataBlock_ForClient(DBWriteSet& setWriteDB)
{
	//! 添加招募状态
	setWriteDB.AddToByteArray( (BYTE)((m_RecruitState.bIsRecruiting) ? 1 : 0) );
	//! 添加招募名称
	if (m_RecruitState.bIsRecruiting)
	{
		if (NULL_GUID == GetTeamID())
		{
			setWriteDB.AddToByteArray( (BYTE)1);
		}
		else
		{
			GSTeam *pTeam = (GSTeam*)(GetOrganiCtrl()->GetGameTeamByID(GetTeamID()));
			if (NULL == pTeam)
			{
				setWriteDB.AddToByteArray( (BYTE)1);
			}
			else
			{
				setWriteDB.AddToByteArray( (BYTE)pTeam->GetMemberNum());
			}
		}
		setWriteDB.AddToByteArray( m_RecruitState.szRecruitName);
		if (0 != m_RecruitState.szPassword[0])
		{
			setWriteDB.AddToByteArray( (BYTE)1);
		}
		else
		{
			setWriteDB.AddToByteArray( (BYTE)0);
		}
	}
}

// 解码
bool CPlayer::DecodeFromDataBlock(DBReadSet& setReadDB,bool bExData)
{
#ifdef _RUNSTACKINFO1_
	char pszStatckInfo[1024]="";
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CPlayer::DecodeFromDataBlock() Start");
#endif
	ClearSkills();
	ClearAllStates();

	CMoveShape::DecodeFromDataBlock(setReadDB, bExData);

	//##设置容器的拥有者
	m_cEquipment.SetOwner( GetType(), GetExID() );
    m_cEquipmentGod.SetOwner(GetType(), GetExID());
    m_cEquipmentLand.SetOwner(GetType(), GetExID());
    m_cEquipmentHuman.SetOwner(GetType(), GetExID());
	m_pPackage.SetOwner(this);	

	//! 设置仓库的拥有者
	m_pPlayerDepot.SetOwner(this);

	//m_cEquipmentUpgrading.SetOwner( GetType(), GetExID() );

	if( bExData )
	{
#ifdef _RUNSTACKINFO1_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CPlayer::DecodeFromDataBlock(),Decode property start.");
#endif
		char strTemp[256];
		setReadDB.GetBufferFromByteArray( &m_Property, (long)sizeof(tagPlayerProperty));
		//setReadDB.GetBufferFromByteArray( &m_arrElementProperty, sizeof(tagPlayerElementProperty) * MT_Max);
		setReadDB.GetStringFromByteArray( strTemp,256);
		SetAccount(strTemp);		
		setReadDB.GetBufferFromByteArray(m_TeamID);
        setReadDB.GetBufferFromByteArray(m_PhaseID);
        m_PhaseRest = (ePhaseRest)setReadDB.GetLongFromByteArray();

#ifdef _RUNSTACKINFO1_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"Decode Skill start.");
#endif
		DecodeSkillsFromDataBlock( setReadDB );

        //编码天赋
        DecodeGeniusFromDataBlock( setReadDB );

        // 结义金兰
        DecodeBrotherFromDataBlock(setReadDB);

#ifdef _RUNSTACKINFO1_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"Decode Buff start.");
#endif
		//##获取异常状态数据
		DecodeBuffFromDataBlock( setReadDB );

		m_lInvestLimitTimeMark=setReadDB.GetLongFromByteArray();
		DWORD dwRecordSize=setReadDB.GetDwordFromByteArray();
		m_mapInvestRecord.clear();
		for(DWORD i=0;i<dwRecordSize;i++)
		{
			DWORD dwSpotId=setReadDB.GetDwordFromByteArray();
			DWORD dwInvestValue=setReadDB.GetDwordFromByteArray();
			m_mapInvestRecord[dwSpotId]=dwInvestValue;
		}
		time_t t;
		t=time(NULL);
		if(t>m_lInvestLimitTimeMark)
		{
			m_lInvestLimitTimeMark=0;
			ClearInvestRecord();
		}		

		m_cEquipment.Unserialize( setReadDB );
        m_cEquipmentGod.Unserialize( setReadDB );
        m_cEquipmentLand.Unserialize( setReadDB );
        m_cEquipmentHuman.Unserialize( setReadDB );

		m_MedalContainer.DecodeFromDataBlock(setReadDB);

#ifdef _RUNSTACKINFO1_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"Decode Equipment start.");
#endif
		for (long i = EC_E_HEAD; i <= EC_MEDAL3; ++i)
		{
			CGoods *pTmpGoods = m_cEquipment.GetGoods( i );
			CheckGoodsTimeOut(pTmpGoods);
		}

        m_pPackage.DecordeDataBlockFromServer(setReadDB);

        // 声望
		DecodeCreditFromDataBlock(setReadDB);

        DecodeNpcShopSellGoodsFromDataBlock(setReadDB);		

        DecodeGoodsCooldown(setReadDB);

		m_lTicket=setReadDB.GetDwordFromByteArray();

#ifdef _RUNSTACKINFO1_
		_snprintf(pszStatckInfo,1024,"Decode Depot(Ticket:%d) start.",m_lTicket);
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStatckInfo);
#endif
		//! 仓库
		m_pPlayerDepot.DecodeFromDataBlock_FromServer(setReadDB);

#ifdef _RUNSTACKINFO1_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"Decode SpriteSystem start.");
#endif
		//! 小精灵系统
		m_SpriteSystem.DecodeFromDataBlock(setReadDB);

#ifdef _RUNSTACKINFO1_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"Decode SetupOnServer start.");
#endif

		//! 玩家需要保存在服务器的设置
		m_SetupOnServer.DecodeFromDataBlock(setReadDB);

#ifdef _RUNSTACKINFO1_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"Decode VariableList start.");
#endif
		//VariableList
		/*OBJ_RELEASE(CVariableList, m_pVariableList);*/
		/*m_pVariableList = OBJ_CREATE(CVariableList);
		m_pVariableList->DecodeFromDataBlock( setReadDB,true);*/
		VarList::Decode( m_VarList, setReadDB );

		// Silence
		SetSilence( setReadDB.GetLongFromByteArray() );

		// GodMode
		if( setReadDB.GetCharFromByteArray() )
			God();
		else 
			God( false );

		OnUpdateMurdererSign();

		//##时间
		DWORD dwMurdererTimeLeft	= setReadDB.GetDwordFromByteArray(  );
		if( dwMurdererTimeLeft && GetPkValue())
		{
			if (dwMurdererTimeLeft > GlobalSetup::GetSetup()->dwOnePkCountTime)
				dwMurdererTimeLeft = GlobalSetup::GetSetup()->dwOnePkCountTime;
			m_dwMurdererTimeStamp	= timeGetTime() - (GlobalSetup::GetSetup()->dwOnePkCountTime - dwMurdererTimeLeft);
		}

		//##战斗状态
		m_lFightStateCount			= setReadDB.GetLongFromByteArray(  );

		////##宠物相关
		//char strName[256] = {0};
		//DWORD dwPetsAmount = setReadDB.GetDwordFromByteArray(  );
		//for( DWORD i = 0; i < dwPetsAmount; i ++ )
		//{
		//	tagPetInformation tPet;
		//	tPet.strOriginalName	= setReadDB.GetStringFromByteArray(  strName );
		//	tPet.dwHp				= setReadDB.GetDwordFromByteArray(  );
		//	tPet.dwLevel			= setReadDB.GetDwordFromByteArray(  );
		//	tPet.dwExperience		= setReadDB.GetDwordFromByteArray(  );
		//	m_vUncreatedPets.push_back( tPet );
		//}


		//得到城战死亡状态时间
		m_bLogin = setReadDB.GetCharFromByteArray(  )==0?false:true;

		//得到任务数据
		DecodeQuestDataFromDataBlock(setReadDB);
		//得到俑兵任务数据
		DecodeMerQuestDataFromDataBlock(setReadDB);

		//得到副职业数据
		m_DOccuRecord.DecodeFromDataBlock(setReadDB);

		// 国家
		//m_Property.byCountry = setReadDB.GetByteFromByteArray(  );
		//m_lContribute = setReadDB.GetLongFromByteArray(  );
		//-------------------------------------------------------------------------------
		// 最近购买的10种商品列表. by Fox.		2008-02-21
        DecodeIncShopFromDataBlock(setReadDB);
		
		//系统邮件列表
		long lNum = setReadDB.GetLongFromByteArray();
#ifdef _RUNSTACKINFO1_
		_snprintf(pszStatckInfo,1024,"Decode SysMai(Size:%d) start.",lNum);
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStatckInfo);
#endif
		for (int i=0;i<lNum;++i)
		{
			long lID = setReadDB.GetLongFromByteArray();
			long lEndTime = setReadDB.GetLongFromByteArray();
			InsertSysMail(lID,lEndTime);
		}

		// 是否第一次登录
		SetFirstLogin( (0 != setReadDB.GetByteFromByteArray()) );

#ifdef _RUNSTACKINFO1_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"Decode OrgSys start.");
#endif
		//组织系统
		DecodeOrgSysFromDataBlock(setReadDB,bExData);

		//! 当前称号类型
		m_lCurrTitleType = setReadDB.GetLongFromByteArray();
		UpdateTitle(FALSE);

		CGUID tGUID;
		CGUID PHGUID;
#ifdef _RUNSTACKINFO1_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"Decode PersonalRgn start.");
#endif
		// 个人副本GUID
		setReadDB.GetBufferFromByteArray( tGUID);
		SetPersonalRgnGUID(tGUID);
		// 个人房屋GUID
		setReadDB.GetBufferFromByteArray( PHGUID);
		SetPersonalHouseRgnGUID(PHGUID);
		// 组队副本GUID
		//GameManager::GetInstance()->GetRgnManager()->RgnGuidDecode(GetExID(), setReadDB);

		// 当前个人副本数
		SetCurPRgnNum(setReadDB.GetLongFromByteArray());
		// 当前队伍副本数
		SetCurTRgnNum(setReadDB.GetLongFromByteArray());

		GetInst(CPetCtrl).DecodeFromDataBlock(setReadDB, this);

		// 解码商业背包
		m_BusinessContainer.DecodeFromDB( setReadDB );

		// 保存状态
		m_wState = (WORD)setReadDB.GetLongFromByteArray();

		SetShowFashion(setReadDB.GetByteFromByteArray());

		/// 反外挂解码玩家属性模块,Expter 20090602
		GetInst(AntiCheat::CPlayerManager).PlayerDecode(this,setReadDB);

	}
#ifdef _RUNSTACKINFO1_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CPlayer::DecodeFromDataBlock() End");
#endif
	return true;
}

bool CPlayer::DecodeOrgSysFromDataBlock(DBReadSet& setReadDB,bool bExData)
{
	setReadDB.GetBufferFromByteArray( m_TeamID);
	setReadDB.GetBufferFromByteArray( m_FactionID);
	GetOrganiCtrl()->UpdateFacInfo(this);
	setReadDB.GetBufferFromByteArray( m_UnionID);
	m_lFacDemiseWarning = setReadDB.GetLongFromByteArray();
	m_lFacDisbandWarning = setReadDB.GetLongFromByteArray();

	return true;
}

bool CPlayer::CodeOrgSysToDataBlock(DBWriteSet& setWriteDB,bool bExData)
{
	if(bExData)
		GetOrganiCtrl()->SendInitFacDataToPlayer(GetExID());
	setWriteDB.AddToByteArray(GetFactionID());
	setWriteDB.AddToByteArray(GetUnionID());
	if (NULL != m_pFacMemInfo)
	{
		setWriteDB.AddToByteArray( (BYTE)TRUE);
		if(bExData)
		{
			setWriteDB.AddToByteArray(m_lFacDemiseWarning);
			setWriteDB.AddToByteArray(m_lFacDisbandWarning);
		}
		setWriteDB.AddToByteArray( m_pFacMemInfo->szTitle);
		GameFaction *pFaction = GetOrganiCtrl()->GetFactionOrganizing(GetFactionID());
		assert(NULL != pFaction);
		if (NULL != pFaction)
		{
			setWriteDB.AddToByteArray(pFaction->GetIconCRC32());
			if(bExData)
			{
				pFaction->AddPublicData_ToDataBlock(setWriteDB);
				setWriteDB.AddToByteArray( pFaction->GetMemberJobLvl(GetExID()));
				setWriteDB.AddToByteArray( pFaction->GetPurview(GetExID()));
				const tagOrgPronounceWord& PronounceWord = pFaction->GetPronounceWord();
				setWriteDB.AddToByteArray( PronounceWord.szBody);
			}	
			else
			{
				setWriteDB.AddToByteArray(pFaction->GetName());
			}
		}	
	}
	else
	{
		setWriteDB.AddToByteArray( (BYTE)FALSE);
	}

	if(bExData)
		GetOrganiCtrl()->SendPlayerUnionData(this, GetUnionID());

	return true;
}

void CPlayer::CodeGoodsCooldown(DBWriteSet& setWriteDB, bool bExData/* =true */)
{
    map<DWORD,DWORD> mapCooldownData;
    mapCooldownData.clear();

    map<DWORD,tagGoodsCommonCooldown>::iterator classIdIter=m_mapClassIdCoolDown.begin();
    //公用冷却
    DWORD dwNowTime=timeGetTime();
    for(;classIdIter!=m_mapClassIdCoolDown.end();classIdIter++)
    {
        DWORD dwElapse=dwNowTime-classIdIter->second.dwCooldownStartTime;
        if(dwElapse < classIdIter->second.dwCooldownTime)
        {
            mapCooldownData[classIdIter->first]=classIdIter->second.dwCooldownTime - dwElapse;
        }
    }

    setWriteDB.AddToByteArray((DWORD)mapCooldownData.size());
    map<DWORD,DWORD>::iterator dataIter=mapCooldownData.begin();
    for(;dataIter!=mapCooldownData.end();dataIter++)
    {
        setWriteDB.AddToByteArray(dataIter->first);
        setWriteDB.AddToByteArray(dataIter->second);
    }
    

	//GOODS ID
	mapCooldownData.clear();
	map<DWORD,tagGoodsCommonCooldown>::iterator goodsIdIter=m_mapGoodsIdCoolDown.begin();
	for(;goodsIdIter!=m_mapGoodsIdCoolDown.end();goodsIdIter++)
	{
		DWORD dwElapse=dwNowTime-goodsIdIter->second.dwCooldownStartTime;
		if(dwElapse < goodsIdIter->second.dwCooldownTime)
		{
			mapCooldownData[goodsIdIter->first]=goodsIdIter->second.dwCooldownTime - dwElapse;
		}
	}
	setWriteDB.AddToByteArray((DWORD)mapCooldownData.size());
	dataIter=mapCooldownData.begin();
	for(;dataIter!=mapCooldownData.end();dataIter++)
	{
		setWriteDB.AddToByteArray(dataIter->first);
		setWriteDB.AddToByteArray(dataIter->second);
	}
}

void CPlayer::DecodeGoodsCooldown(DBReadSet& setReadDB)
{
    m_mapClassIdCoolDown.clear();
    DWORD dwCooldownSize=setReadDB.GetDwordFromByteArray();
    DWORD dwCooldownId=0,dwCooldownTime=0;
    tagGoodsCommonCooldown stCooldown;
    DWORD dwNowTime=timeGetTime();
#ifdef _RUNSTACKINFO1_
    char pszStatckInfo[1024];
    _snprintf(pszStatckInfo,1024,"Decode dwCooldownSize(Size:%d) start.",dwCooldownSize);
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStatckInfo);
#endif
    for(DWORD i=0;i<dwCooldownSize;i++)
    {
        dwCooldownId=setReadDB.GetDwordFromByteArray();
        dwCooldownTime=setReadDB.GetDwordFromByteArray();
        ZeroMemory(&stCooldown,sizeof(tagGoodsCommonCooldown));
        stCooldown.dwCooldownStartTime=dwNowTime;
        stCooldown.dwCooldownTime=dwCooldownTime;
        m_mapClassIdCoolDown[dwCooldownId]=stCooldown;
    }

    m_mapGoodsIdCoolDown.clear();
    dwCooldownSize=setReadDB.GetDwordFromByteArray();
#ifdef _RUNSTACKINFO1_
    _snprintf(pszStatckInfo,1024,"Decode CooldownSize(Size:%d) start.",dwCooldownSize);
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStatckInfo);
#endif
    for(DWORD i=0;i<dwCooldownSize;i++)
    {
        dwCooldownId=setReadDB.GetDwordFromByteArray();
        dwCooldownTime=setReadDB.GetDwordFromByteArray();
        ZeroMemory(&stCooldown,sizeof(tagGoodsCommonCooldown));
        stCooldown.dwCooldownStartTime=dwNowTime;
        stCooldown.dwCooldownTime=dwCooldownTime;
        m_mapGoodsIdCoolDown[dwCooldownId]=stCooldown;
    }
}
//共用冷却
void CPlayer::CodeGoodsColldownForClient(DBWriteSet& setWriteDB)
{
    map<DWORD,tagGoodsCommonCooldown>::iterator cooldownClassIt=m_mapClassIdCoolDown.begin();
    setWriteDB.AddToByteArray((DWORD)m_mapClassIdCoolDown.size());
    for(;cooldownClassIt!=m_mapClassIdCoolDown.end();cooldownClassIt++)
    {
        //共用ID
        setWriteDB.AddToByteArray((DWORD)cooldownClassIt->first);
        //剩余毫秒数
        setWriteDB.AddToByteArray((DWORD)cooldownClassIt->second.dwCooldownTime);
    }		
    //GOODS ID 冷却
    map<DWORD,tagGoodsCommonCooldown>::iterator cooldownGoodsIt=m_mapGoodsIdCoolDown.begin();
    setWriteDB.AddToByteArray((DWORD)m_mapGoodsIdCoolDown.size());
    for(;cooldownGoodsIt!=m_mapGoodsIdCoolDown.end();cooldownGoodsIt++)
    {
        //GOODS ID
        setWriteDB.AddToByteArray((DWORD)cooldownGoodsIt->first);
        //剩余毫秒数
        setWriteDB.AddToByteArray((DWORD)cooldownGoodsIt->second.dwCooldownTime);
    }
}

//移动时触发
void CPlayer::OnMove(float fNewX, float fNewY)
{
#ifdef _RUNSTACKINFO1_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CPlayer::OnMove() Start.");
#endif
#ifdef _DEBUG

	//char str1[128]="";
	//_snprintf(str1, 128, "----------------------当前速度值:%f----------------x坐标:%f,y坐标:%f-----------\n",GetSpeed(),fNewX,fNewY);
	//OutputDebugString(str1);
#endif
	CMoveShape::OnMove(fNewX,fNewY);
	//if( !m_FightPets.empty() )
	//{
	//	CheckPetsMove();	// 检查宠物移动
	//}

	//检查切换点
	CheckSwitchPoint();
#ifdef _RUNSTACKINFO1_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CPlayer::OnMove() End.");
#endif
}
// 请求复活(0:安全回城 1:原地复活)
void CPlayer::OnRelive(long lType,long lRecoverRatio)
{
	CServerRegion* pRegion = dynamic_cast<CServerRegion*>(GetFather());
	if(NULL == pRegion || lRecoverRatio<=0)
		return;				
	//如果角色死亡
	if( IsDied() )
	{
		long lHp = GetMaxHP()*lRecoverRatio/100;
		long lMp = GetMaxMP()*lRecoverRatio/100;
		if (lHp<1)
		{
			lHp = 1;
		}
		if (lMp<1)
		{
			lMp = 1;
		}
		
        /// 转换为和平状态
        AI_EVENT_SENDER( m_pAI ).Relive();      

		ChangeStateTo(STATE_PEACE);
		SetAttribute("Hp",lHp);
		SetAttribute("Mp",lMp);
		UpdateAttribute();
		SetAction(CShape::ACT_STAND);
		
		//StartUpPassiveSkill();
		SetPos(0);

		UnRegisterReliveTimerID();
		FreeReliveScript();

		// 复活时触发脚本
		PlayerRunScript(GlobalSetup::GetSetup()->szReliveScriptName);

		long region, l, t, r, b, d;
		CServerRegion* pRegion = (CServerRegion*)GetFather();
		CServerRegion* pReturnRegion = NULL;

		//流放岛复活就在流放岛 && 
		//  [9/7/2009 chenxianj]
		//玩家死亡复活在pk值大于18000时回到流放岛
		long lJailLandID = GetCountryParam()->GetCountryJailIsLandRegionID(GetCountry());
		if ( ( (long)GetPkValue()>=GlobalSetup::GetSetup()->lPrisonIslandPkValue && lType!=1) 
			|| (lJailLandID==pRegion->GetID() && 0 != lJailLandID) )
		{
			//先复活			
			CMessage msg(MSG_S2C_PLAYER_ANSWER_RELIVE);
			msg.Add(GetAction());
			msg.Add(GetState());
			msg.Add(GetHP());
			msg.Add(GetMP());
			msg.SendToPlayer(GetExID());

			// 通知周围玩家
			CMessage msg2(MSG_S2C_SHAPE_RELIVE);
			msg2.Add(GetType());
			msg2.Add(GetExID());
			msg2.Add(GetAction());
			msg2.Add(GetState());
			msg2.SendToAround(this, this);		

			ChangeRgnToIsland();
			return;
		}


		if (lType==1) // 原地复活
		{
			region = pRegion->GetID();
			l = r = GetTileX();
			t = b = GetTileY();
			d = GetDir();
			//先复活			
			CMessage msg(MSG_S2C_PLAYER_ANSWER_RELIVE);
			msg.Add(GetAction());
			msg.Add(GetState());
			msg.Add(GetHP());
			msg.Add(GetMP());
			msg.SendToPlayer(GetExID());

			// 通知周围玩家
			CMessage msg2(MSG_S2C_SHAPE_RELIVE);
			msg2.Add(GetType());
			msg2.Add(GetExID());
			msg2.Add(GetAction());
			msg2.Add(GetState());
			msg2.SendToAround(this, this);					
		}
		else //复活回城
		{
			if(pRegion)
			{
				if(ChangeRgnToVillage())
				{
					//先复活			
					CMessage msg(MSG_S2C_PLAYER_ANSWER_RELIVE);
					msg.Add(GetAction());
					msg.Add(GetState());
					msg.Add(GetHP());
					msg.Add(GetMP());
					msg.SendToPlayer(GetExID());

					// 通知周围玩家
					CMessage msg2(MSG_S2C_SHAPE_RELIVE);
					msg2.Add(GetType());
					msg2.Add(GetExID());
					msg2.Add(GetAction());
					msg2.Add(GetState());
					msg2.SendToAround(this, this);	
					return;
				}
				// 回城新规则
				region = l = t = r = b = d = -1;
				// 指定地图复活
				if(pRegion->GetRgnType() == RGN_NORMAL)
				{
					if(pRegion->GetSetup().bReLiveIsUsed)
						pRegion->GetReLiveReturnPoint(this, region, 1, l, t, r, b, d);

					// 判断各自区域是否为普通区域
					if(pRegion->GetSecurity((int)GetPosX(), (int)GetPosY()) == CRegion::SECURITY_FREE) // 普通区域则加一个配置的Debuff
					{
						AddNewState(this, GlobalSetup::GetSetup()->lReliveDebuffId, GlobalSetup::GetSetup()->lReliveDebuffLevel,15);
					}
				}
				else // 副本场景
				{
					if(pRegion->GetSetup().bDupRgnIsUsed)
						pRegion->GetDupRgnReturnPoint(this, region, l, t, r, b, d);
				}

				if(region == 0) // 原场景复活
				{
					SetRegionID(pRegion->GetID());
					SetRegionGUID(pRegion->GetExID());
					SetCurRgnType((eRgnType)RGN_NORMAL);
				}
				else if(region == -1) // 回国都复活
				{
					BYTE byCountry = GetCountry();
					long lCityRegion = GetCountryParam()->GetCountryMainRegionID(byCountry);
					l = GetCountryParam()->GetCountryMainRectLeft(byCountry);
					t = GetCountryParam()->GetCountryMainRectTop(byCountry);
					r = GetCountryParam()->GetCountryMainRectRight(byCountry);
					b = GetCountryParam()->GetCountryMainRectBottom(byCountry);

					pReturnRegion = GameManager::GetInstance()->GetRgnManager()->FindRgnByTemplateID(RGN_NORMAL, lCityRegion);
					if(!pReturnRegion)
					{
						ChangeRegion(RGN_NORMAL, NULL_GUID, l, t, GetDir(), lCityRegion);
						return;
					}

					SetRegionID(lCityRegion);
					SetRegionGUID(pReturnRegion->GetExID());
					SetCurRgnType((eRgnType)RGN_NORMAL);
				}
				else if(region > 0) // 制定场景复活
				{
					pReturnRegion = GameManager::GetInstance()->GetRgnManager()->FindRgnByTemplateID(RGN_NORMAL, region);
					if(!pReturnRegion)
					{
						ChangeRegion(RGN_NORMAL, NULL_GUID, l, t, GetDir(), region);
						return;
					}
					SetRegionID(region);
					SetRegionGUID(pReturnRegion->GetExID());
					SetCurRgnType((eRgnType)RGN_NORMAL);
				}
			}
		}

		if (pReturnRegion==NULL) // 是原地复活 或 未找到配置的复活场景
		{
			return;
		}

		//##恢复状态
		EnterResidentState();

		//切换场景
		if( ChangeRegion((eRgnType)pReturnRegion->GetRgnType(),pReturnRegion->GetExID(), l, t, GetDir()) == true)
		{
			CMessage msg(MSG_S2C_PLAYER_ANSWER_RELIVE);
			msg.Add(GetAction());
			msg.Add(GetState());
			msg.Add(GetHP());
			msg.Add(GetMP());
			msg.SendToPlayer(GetExID());
		}
	}
	else
	{
		//错误
	}
}

// 宠物切换场景
void CPlayer::PetsChangeRegionLocal(INIT_TYPE lInitType)
{
	//if( !m_FightPets.empty() )
	//{
	//	ITR_ACT_PET itrActPet = m_FightPets.begin();
	//	CPet* pPet = NULL;
	//	while( itrActPet != m_FightPets.end() )
	//	{
	//		pPet = GetActivePet(itrActPet->first);
	//		if( pPet != NULL )
	//		{
	//			++itrActPet;
	//			pPet->SetTarget(NULL);
	//			pPet->ModifyPos(lInitType);
	//		}
	//		else
	//		{
	//			m_FightPets.erase(itrActPet++);
	//		}
	//	}
	//}
}

// 切换场景
bool CPlayer::ChangeRegion(eRgnType type, const CGUID& lRegionID, long x, long y, long dir, long templateID, long lUseGoods, long lRange, long lCarDistance)
{
#ifdef _RUNSTACKINFO1_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CPlayer::ChangeRegion() Start.");
#endif
    // 切换场景时停止移动逻辑
    m_mover.OnSetPos( x, y );

	/*
	if (GetCurrentProgress()!=PROGRESS_NONE)
		return false;*/
	if(GetCurrentProgress()==PROGRESS_INCREMENT)
	{
		SetCurrentProgress( CPlayer::PROGRESS_NONE );
		SetMoveable();
		CMessage msg( MSG_S2C_INCREMENTSHOP_CLOSE );
		msg.SendToPlayer( GetExID() );
	}
	else if(GetCurrentProgress()==PROGRESS_TRADING)
	{		
		CSession* pSession = const_cast<CSession*>( CSessionFactory::QuerySession(m_TradeSessionId) );
		if(pSession)
		{
			CTrader* pPlug = dynamic_cast<CTrader*>( pSession -> QueryPlugByOwner( 
				GetType(), GetExID() ) );
			if( pPlug && pPlug -> Get32ID() == m_lTradePlugId )
			{
				if( pSession -> IsSessionAvailable() )
				{
					pSession -> Abort();
				}
			}
		}
	}
	else if( GetCurrentProgress() == PROGRESS_FBUSINESS )
	{
		SetCurrentProgress( PROGRESS_NONE );
		SetMoveable();
		GetInst( FBusiness::CShopManager ).PlayerCloseShop( this );
	}
    /// 处于相位不能切换场景
    else if ( GetPhaseID() != NULL_GUID )
    {
        return false;
    }

	CServerRegion* pRegion = dynamic_cast<CServerRegion*>( GetFather() );
	if( pRegion == NULL || !this->CanMove())
	{
		return false;
	}

	//切换场景时候,进行一次速度验证
	ResetMoveValiData();

	LONG lDir	= dir;
	LONG lX		= x;
	LONG lY		= y;
	LONG lOldX,lOldY;
	lOldX = GetTileX();
	lOldY = GetTileY();

	if( lDir < DIR_UP || lDir > DIR_LEFTUP )
	{
		lDir = random( DIR_LEFTUP - DIR_UP + 1 );
	}

	// 结束场景定时器
	CancelRgnKickPlayerTimer();

	//结束所有主动技能
	EndAllActiveSkill();

	//##如果是在同一个场景就只改变玩家位置
	if( lRegionID == pRegion -> GetExID() )
	{
		//##产生随机点
		if( lX == -1 && lY == -1 )
		{
			pRegion -> GetRandomPos( lX, lY );
		}
		else if( lRange > 0 )
		{
			pRegion -> GetRandomPosInRange( lX, lY, lX - lRange, lY - lRange, 
				lRange * 2 + 1, lRange * 2 + 1 );
		}

		if( lX != GetTileX() || lY != GetTileY() )
		{
			CMessage Msg( MSG_S2C_SHAPE_SETPOS );
			Msg.Add( GetType() );
			Msg.Add( GetExID() );
			Msg.Add( (float)lX );
			Msg.Add( (float)lY );
			Msg.Add( lUseGoods );
			Msg.SendToAround( this );

			SetPosXY( lX+0.5f, lY+0.5f );
		}

		if( lDir != GetDir() )
		{
			SetDir( lDir );

			CMessage Msg( MSG_S2C_SHAPE_CHANGEDIR );
			Msg.Add( static_cast<BYTE>(lDir) );
			Msg.Add( GetDirEx());
			Msg.Add( GetType() );
			Msg.Add( GetExID() );

			Msg.SendToAround( this );
		}
#ifdef _RUNSTACKINFO1_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CPlayer::ChangeRegion() End1.");
#endif
		GetLogicLogInterface()->logT440_change_map_log(this, pRegion->GetID(), templateID, 1);
		PetsChangeRegionLocal(IT_SAME_RGN);
		return true;
	}

	//##切换场景 此场景在此服务器上
	// test [071127 AHC]
	CServerRegion * pNewRegion = GameManager::GetInstance()->GetRgnManager()->FindRgnByRgnGUID(type, lRegionID);
	if( pNewRegion != NULL )
	{
		// 产生随机点
		if( lX == -1 && lY == -1 )
		{
			pNewRegion -> GetRandomPos( lX, lY );
		}
		else if( lRange > 0 )
		{
			pNewRegion -> GetRandomPosInRange( lX, lY, lX - lRange, lY - lRange, lRange * 2, lRange * 2 );
		}

		// 服务器要求客户端进行切换
		CMessage Msg( MSG_S2C_RGN_CHANGE );
		Msg.Add( GetType());
		Msg.Add( GetExID() );
		Msg.Add( lUseGoods );
		Msg.Add( pNewRegion->GetExID() ); // 普通场景发送RgnID
		Msg.Add( pNewRegion->GetID() ); // 副本场景发送OwnerID
		Msg.Add( lX );
		Msg.Add( lY );
		Msg.Add( lDir );
		Msg.Add( (char*)pNewRegion->GetName() );
		Msg.Add(pNewRegion->GetTaxRate());
		Msg.Add((BYTE)pNewRegion->GetWarType());
		Msg.Add((BYTE)pNewRegion->GetSpaceType());
		Msg.Add((BYTE)pNewRegion->IsChangeEquip());
		Msg.Add(pNewRegion->GetResourceID());
		Msg.Add(pNewRegion->GetExpScale());
		Msg.Add((BYTE)((CServerRegion*)pNewRegion)->IsNoPk());
		DBWriteSet setWriteDB;
		Msg.GetDBWriteSet(setWriteDB);
		GameManager::GetInstance()->GetRgnManager()->AddLinedRgnInfoToByteArray(pNewRegion->GetLinedIdFlag(), setWriteDB);
		// 发送当前场景的据点信息
		Msg.Add((BYTE)pNewRegion->GetStrongPointFlag()); // 据点战是否开启
		if(pNewRegion->GetStrongPointFlag() == false) // 未开启据点战
		{
			Msg.Add((long)0); // 据点个数
		}
		else
		{
			Msg.Add((long)pNewRegion->GetStronPointNum()); // 据点个数
			std::vector<CServerRegion::tagStrongPoint>::iterator spItr = pNewRegion->GetStrongPointVec().begin();
			for(; spItr != pNewRegion->GetStrongPointVec().end(); spItr++)
			{
				Msg.Add((*spItr).szName);//据点名字
				Msg.Add((long)(*spItr).lPosX); //据点X坐标
				Msg.Add((long)(*spItr).lPosY); //据点Y坐标
				Msg.Add((long)(*spItr).lState); //据点状态
			}
		}
		Msg.SendToAround( this );
		//触发该事件
		OnExitRegion();
		//Player退出场景处理
		pRegion->PlayerExit(this);

		// 设置父亲场景
		SetFather(pNewRegion);
		SetRegionGUID(pNewRegion->GetExID());
		SetRegionID(pNewRegion->GetID());
		SetNextRegionID(pNewRegion->GetID());
		SetCurRgnType((eRgnType)pNewRegion->GetRgnType());
		//设置坐标
		CShape::SetPosXY(lX+0.5f,lY+0.5f);

		//Player进入场景处理
		pNewRegion->PlayerEnter(this,lUseGoods);

		// Fox@20081110 刷新角色场景和坐标
		GetInst(CPlayerMan).RefreshElem(PET_REGION, this);
#ifdef _RUNSTACKINFO1_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CPlayer::ChangeRegion() End2.");
#endif
		GetLogicLogInterface()->logT440_change_map_log(this, pRegion->GetID(), templateID, 2);
		PetsChangeRegionLocal(IT_SAME_GS);
		return true;
	}

	// 反外挂：如果是切服，则检查玩家是否拒绝了答题
	GetInst( AntiCheat::CPlayerManager ).CheckRefuseAnswer( GetExID(), true );

	////场景不在此服务器上，交换服务器
	//SetAction(ACT_STAND);
	if(GetState() != STATE_DIED)
	SetState(STATE_PEACE);

	tagPCR ChangePlayer = {type,templateID,lRegionID,lX,lY,lDir,lUseGoods,lRange};
	GetGame()->PushChangeRgnPlayer( GetExID(),ChangePlayer);
	GetLogicLogInterface()->logT440_change_map_log(this, pRegion->GetID(), templateID, 3);
#ifdef _RUNSTACKINFO1_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CPlayer::ChangeRegion() End3.");
#endif
	return false;
}

void CPlayer::SetPosition( LONG lX, LONG lY )
{
	CMoveShape::SetPosition(lX,lY);

	ResetMoveValiData();
}

void CPlayer::SetSlipPos( LONG lX, LONG lY,float fSpeed ,float fDir)
{
	CMoveShape::SetSlipPos(lX,lY,fSpeed,fDir);

	ResetMoveValiData();
}


BOOL CPlayer::CheckSwitchPoint()
{
#ifdef _RUNSTACKINFO3_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CPlayer::CheckSwitchPoint() Start.");
#endif
	CRegion* pRegion = dynamic_cast<CRegion*>( GetFather() );
	if( pRegion )
	{
		CRegion::tagSwitch* pSwitchPoint = pRegion -> GetSwitch( GetTileX() , GetTileY() );
		if( pSwitchPoint )
		{
			switch( pSwitchPoint -> lState )
			{
				//##直接切换
			case CRegion::CPT_NORMAL:
				{

					// 判断等级是否足够 soft 2006.9.19
					long lEenterLvl = CRegionSetup::GetEnterRegionLvl(pSwitchPoint->lRegionID);
					if (GetLevel() < lEenterLvl)
					{
						// 后退三步
						//long x = GetTileX();
						//long y = GetTileY();
						//long dir = GetRearDir();
						//for (int i=0; i<3; i++)
						//{
						//	x += _dir[dir][0];
						//	y += _dir[dir][1];
						//}
						//// [071127 AHC REGION]
						//CGUID returnRgn = NULL_GUID;
						//CServerRegion* pRetRgn = GameManager::GetInstance()->GetRgnManager()->FindRgnByTemplateID(RGN_NORMAL, GetRegionID());
						//if(pRetRgn)
						//	returnRgn = pRetRgn->GetExID();
						//ChangeRegion(RGN_NORMAL, returnRgn, x, y, GetDir(), 0, 1);
						// 给提示
						CHAR szMsg[256];
						_snprintf( szMsg, 256, "[Colour 'ffff0000']你还不足 %d 级，暂时无法进入该区域。", lEenterLvl);
						//SendNotifyMessage( szMsg, 0xffff0000);
						SendNotifyMessage(szMsg);
						return FALSE;
					}
					// [071127 AHC REGION]
					CGUID returnRgn = NULL_GUID;
					CServerRegion* pRetRgn = GameManager::GetInstance()->GetRgnManager()->FindRgnByTemplateID(RGN_NORMAL, pSwitchPoint->lRegionID);
					if(pRetRgn)
						returnRgn = pRetRgn->GetExID();
					ChangeRegion(RGN_NORMAL, returnRgn, pSwitchPoint -> lCoordX, 
						pSwitchPoint -> lCoordY, pSwitchPoint -> lDir,pSwitchPoint->lRegionID);
				}
				break;

				//##随机切换
			case CRegion::CPT_RANDOM:
				break;

				//##脚本触发
			case CRegion::CPT_SCRIPT:
				{
					//stRunScript sScript;
					//sScript.desShape = NULL;
					//sScript.pRegion = pRegion;
					//sScript.srcShape	= this;

					char strScript[256];
					_snprintf( strScript, 256, "scripts/regions/%d.lua", pSwitchPoint -> lRegionID );
					//RunScript( &sScript, const_cast<CHAR*>( GetGame() -> GetScriptFileData(strScript) ) );
					Script::RunEnv env( this, NULL, pRegion );
					GetInst( ScriptSys ).RunScript( env, strScript );
				}
				break;
			}
			return TRUE;
		}
	}
#ifdef _RUNSTACKINFO3_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CPlayer::CheckSwitchPoint() End.");
#endif
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
//	战斗
//////////////////////////////////////////////////////////////////////////

void CPlayer::OnEquipmentWaste( eEquipType ec, CGoods* pEquipment )
{
	if( pEquipment )
	{
		if( m_cEquipment.GetGoods(ec) == pEquipment )
		{
			LONG lPreviousDurability = pEquipment -> GetAddonPropertyValues( GAP_GOODS_MAX_DURABILITY, 2 );
			//##还有特殊损耗，暂时不处理
			if( lPreviousDurability > 0 && CGoodsFactory::EquipmentWaste(pEquipment) )
			{
				LONG lCurrentDurability = pEquipment -> GetAddonPropertyValues( GAP_GOODS_MAX_DURABILITY, 2 );
				if( lCurrentDurability > 0 )
				{
					if( lPreviousDurability / 100 != lCurrentDurability / 100 )
					{
						CMessage msg( MSG_S2C_EQUIPDURUPDATE );
						msg.Add( static_cast<BYTE>(ec) );
						msg.Add( static_cast<DWORD>(lCurrentDurability) );
						msg.SendToPlayer( GetExID() );
					}
				}
				else
				{
                    /** 
                     *  新的装备耐久调整方案
                     *  不在计算和叠加装备的属性
                     */
                    pEquipment->RunScriptOnRemoveEquip(this);
                    MountEquipEx(pEquipment,false);
                    return ;


					//##首先移出装备
					CS2CContainerObjectMove comMsg;
					if( m_cEquipment.Remove(pEquipment, &comMsg) )
					{
						CHAR szMsg[256];
						const char* strRes=AppFrame::GetText("7");
						_snprintf( szMsg, 256, strRes, pEquipment -> GetName() );
						//SendNotifyMessage( szMsg,NOTIFY_COLOR_RED);
						SendNotifyMessage(szMsg);

						//##如果成功，就通知玩家。
						comMsg.SetOperation( CS2CContainerObjectMove::OT_DELETE_OBJECT );
						comMsg.SetSourceContainerExtendID( CS2CContainerObjectMove::PEI_EQUIPMENT );
						comMsg.Send( GetExID() );

						//##然后通知周围玩家。
						CMessage msg( MSG_S2C_PLAYER_CHANGE_EQUIPMENT );
						msg.Add( GetExID() );
						msg.Add( static_cast<DWORD>(ec) );
						msg.Add( BYTE(0) /*表示卸下装备*/ );
						msg.Add( pEquipment -> GetBasePropertiesIndex() );
						msg.Add( pEquipment -> GetAddonPropertyValues(GAP_WEAPON_LEVEL, 1) );
						msg.SendToAround( this );
						//装备消失日志
						GetLogicLogInterface()->logT040_goods_log_disappear(this,pEquipment,0);
#ifdef _GOODSLOG1_
						char pszGoodsLog[1024]="";			
						char srcGoodsGUID[50]="";
						pEquipment->GetExID().tostring(srcGoodsGUID);					
						_snprintf(pszGoodsLog,1024,"[物品回收:CPlayer::OnEquipmentWaste][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d]",
							pEquipment->GetBasePropertiesIndex(),GoodsSetup::QueryGoodsName(pEquipment->GetBasePropertiesIndex()),srcGoodsGUID,pEquipment->GetAmount());
						CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
#endif

						//##删除该物品
						CGoodsFactory::GarbageCollect( &pEquipment,27 );
						


					}
				}
			}
		}
	}
}

//##护具的损耗
void CPlayer::OnArmorDamaged()
{
	LONG lHeadWastingProbability	= random( 100 );
	LONG lBodyWastingProbability	= random( 100 );
	LONG lGloveWastingProbability	= random( 100 );
	LONG lBootWastingProbability	= random( 100 );

	LONG	lBackWaste = random( 100 );
	LONG	lHeadgearWaste = random( 100 );
	LONG	lFrockWaste = random( 100 );
	LONG	lWingWaste = random( 100 );
	LONG	lManteauWaste = random( 100 );
	LONG	lFairyWaste = random( 100 );
	LONG	lNeckWaste=random(100);
	LONG	lRingWaste=random(100);
	LONG	lShieldWaste=random(100);
	/*
	enum eEquipType 
	{	
	EC_HEAD,				//	0：头盔
	EC_NECKLACE,			//	1：项链
	EC_WING,				//	2：翅膀
	EC_BODY,				//	3：盔甲
	EC_BACK,				//	4：腰带
	EC_GLOVE,			//5：手套	
	EC_BOOT,			//6：鞋子
	EC_HEADGEAR,			//	7：头饰
	EC_FROCK,			//	8：外套		
	EC_LRING,			//	9：左戒指
	EC_RRING,			//	10：右戒指		
	EC_MEDAL1,			//	11：勋章1
	EC_MEDAL2,			//	12	勋章2
	EC_MEDAL3,			//	13	勋章3
	EC_FAIRY,			//	14：小精灵
	EC_WEAPON,			//	15：武器
	EC_WEAPON2,			//	16：副手武器
	EC_CLOAK,			//17披风
	EC_HORSE,			//18坐骑
	EC_TOTAL
	};
	*/

	//项链
	if( lNeckWaste < GlobalSetup::GetSetup() ->lNeckWaste )
	{
		CGoods* pNeck = m_cEquipment.GetGoods( EC_E_NECKLACE );
		if( pNeck )
		{
			OnEquipmentWaste( EC_E_NECKLACE, pNeck );
		}
	}

	//头盔
	if( lHeadWastingProbability < GlobalSetup::GetSetup() -> lHeadWaste )
	{
		CGoods* pHelmet = m_cEquipment.GetGoods( EC_E_HEAD );
		if( pHelmet )
		{
			OnEquipmentWaste( EC_E_HEAD, pHelmet );
		}
	}

	//左戒指
	//右戒指
	if(lRingWaste<GlobalSetup::GetSetup() ->lRingWaste)
	{
		CGoods* pLRing = m_cEquipment.GetGoods( EC_E_LRING);
		if( pLRing )
		{
			OnEquipmentWaste( EC_E_LRING, pLRing );
		}
		CGoods* pRRING = m_cEquipment.GetGoods( EC_E_RRING);
		if( pRRING )
		{
			OnEquipmentWaste( EC_E_RRING, pRRING );
		}
	}


	if( lBodyWastingProbability < GlobalSetup::GetSetup() -> lBodyWaste )
	{
		CGoods* pMail = m_cEquipment.GetGoods( EC_E_BODY );
		if( pMail )
		{
			OnEquipmentWaste( EC_E_BODY, pMail );
		}
	}

	if( lGloveWastingProbability < GlobalSetup::GetSetup() -> lGloveWaste )
	{
		CGoods* pGlove = m_cEquipment.GetGoods( EC_E_HAND );
		if( pGlove )
		{
			OnEquipmentWaste( EC_E_HAND, pGlove );
		}
	}

	if( lBootWastingProbability < GlobalSetup::GetSetup() -> lBootWaste )
	{
		CGoods* pBoot = m_cEquipment.GetGoods( EC_E_BOOT );
		if( pBoot )
		{
			OnEquipmentWaste( EC_E_BOOT, pBoot );
		}
	}

	if( lWingWaste < GlobalSetup::GetSetup() -> lWingWaste )
	{
		CGoods* pGoods = m_cEquipment.GetGoods( EC_E_WING );
		if( pGoods )
		{
			OnEquipmentWaste( EC_E_WING, pGoods );
		}
	}
}

//##武器被损坏
void CPlayer::OnWeaponDamaged()
{
	CGoods* pWeapon = m_cEquipment.GetGoods( EC_E_WEAPON );
	if( pWeapon )
	{
		OnEquipmentWaste( EC_E_WEAPON, pWeapon );
	}
}
//减pk值
void	CPlayer::OnDecreasePKValue()
{
	if( IsDied() ) return;
	DWORD wMurdererCounts = GetPkValue();
	// [9/7/2009 chenxianj]
	//pk值小于等于20，如果当前状态是战斗状态，pk值不再减少
	if (wMurdererCounts<=eUnDecPKPVP_Value && GetState()==STATE_FIGHT)
	{
		return;
	}

	if (wMurdererCounts>0)
	{
		SetPkValue(GetPkValue() - 1);
	} 

}
//设置PK值
void	CPlayer::SetPkValue(DWORD l)
{
	SetAttribute("PkValue",l);
	UpdateAttribute(TRUE);
}

//流放流亡岛
void    CPlayer::ChangeRgnToIsland()
{
	BYTE	byCountry = GetCountry();
	long	lRgnID = GetCountryParam()->GetCountryJailIsLandRegionID(byCountry);
	long 	l = GetCountryParam()->GetCountryJailIsLandRectLeft(byCountry);
	long	t = GetCountryParam()->GetCountryJailIsLandRectTop(byCountry);
	long	lDir = GetCountryParam()->GetCountryJailIsLandDir(byCountry);


	ChangeRegion(RGN_NORMAL, NULL_GUID, l, t,lDir, lRgnID);
}

//发送国家公告
void	CPlayer::PostCountryInfo(const char *strName)
{
	DWORD dwColor = 0xFFFF0000;
	DWORD dwBkColor = 0;
	char  strInfo[256];
	_snprintf(strInfo, 256, AppFrame::GetText("STR_GS_RGNFUNC_COUNTRYINFO"),strName);

	CMessage msg( MSG_S2W_GM_COUNTRY_MESSAGE );
	msg.Add(GetExID());
	msg.Add((char*)strInfo);	
	msg.Add( (long)GetCountry());
	msg.Add( dwColor );
	msg.Add( dwBkColor );
	msg.Send();
}

//流放流亡村
bool	CPlayer::ChangeRgnToVillage()
{
	long lPkValue = GetPkValue(); 
	if (lPkValue<= (long)GlobalSetup::GetSetup()->dwPkCountPerAttack || lPkValue>=GlobalSetup::GetSetup()->lPrisonIslandPkValue)
	{
		return false;
	}

	BYTE	byCountry = GetCountry();
	long	lRgnID = GetCountryParam()->GetCountryJailVillageRegionID(byCountry);
	long 	l = GetCountryParam()->GetCountryJailVillageRectLeft(byCountry);
	long	t = GetCountryParam()->GetCountryJailVillageRectTop(byCountry);
	long	lDir = GetCountryParam()->GetCountryJailVillageDir(byCountry);

	/*CServerRegion* pReliveRgn = GameManager::GetInstance()->GetRgnManager()->FindRgnByTemplateID(RGN_NORMAL, lRgnID);
	CGUID retRgnGuid;
	if(pReliveRgn)
	{
	retRgnGuid = pReliveRgn->GetExID();*/
	ChangeRegion(RGN_NORMAL, NULL_GUID, l, t,lDir, lRgnID);
	return true;
	/*}
	return false;*/
}

//减恶名值
void	CPlayer::OnDecreasePvPValue()
{
	if( IsDied() ) return;

	DWORD dwPvPValue = GetPVPValue();

	if (dwPvPValue<=eUnDecPKPVP_Value && GetState()==STATE_FIGHT)
	{
		return;
	}
	if (dwPvPValue>0)
	{
		SetAttribute("PVPValue",GetPVPValue()-1);
		UpdateAttribute(TRUE);
	} 

}
//自动恢复hp,mp
void	CPlayer::OnIncreaseHpMpValue()
{
	string strHp = "Hp";
	string strMp = "Mp";
	string strBHpRecover = "B_HpRecoverSpeed";
	string strBMpRecover = "B_MpRecoverSpeed";
	string strEHpRecover = "E_HpRecoverSpeed";
	string strEMpRecover = "E_MpRecoverSpeed";
	long lCurretnHpRecoverSpeed = GetAttribute(strBHpRecover) + GetAttribute(strEHpRecover);
	long lCurretnMpRecoverSpeed = GetAttribute(strBMpRecover) + GetAttribute(strEMpRecover);

	if (!IsDied())
	{
		if (GetAttribute(strHp)< (long)GetMaxHP() && lCurretnHpRecoverSpeed>0)
		{
			long lCurretnHp = GetAttribute(strHp) + lCurretnHpRecoverSpeed;
			if (lCurretnHp> (long)GetMaxHP())
			{
				lCurretnHp = (long) GetMaxHP();
			}
			SetAttribute("Hp",lCurretnHp);
			UpdateAttribute(1);			
		}
		if (GetAttribute(strMp)< (long)GetMaxMP() && lCurretnMpRecoverSpeed>0)
		{
			long lCurretnMp = GetAttribute(strMp) + lCurretnMpRecoverSpeed;
			if (lCurretnMp>GetMaxMP())
			{
				lCurretnMp = GetMaxMP();
			}
			SetAttribute("Mp",lCurretnMp);
			UpdateAttribute(1);			
		}			
	}

	//检查客户端是否发过定时Ping消息
	DWORD dwCurTime = timeGetTime();
	if(  (long)(dwCurTime - GetLastPingTime()) > 60*1000*GlobalSetup::GetSetup()->lPingChekTime )
	{
		//如果客户端超时未发ping消息,则直接踢掉。
		GetGame()->KickPlayer(GetExID());
		SetLastPingTime(dwCurTime);

		GetLogicLogInterface()->logT900_anticheat_player_log(this,"PINGERROR", 0, 0, 0 );
		//封号
		long lBanSecs = GlobalSetup::GetSetup()->lPingErrorBanTime * 60;
		CMessage msg( MSG_S2W_GM_BANPLAYER );
		msg.Add( NULL_GUID );
		msg.Add( GetName() );
		msg.Add( (long) lBanSecs );
		msg.Send();
	}
}
//更新能量值
//void	CPlayer::OnUpdateEnergy()
//{
//	long lNum = 0;
//	string strEnergy	= "Energy";
//	string strMaxEnergy = "B_MaxEnergy";
//	string strEMaxEnergy = "E_MaxEnergy";
//
//	string strBBaseRecover = "B_EnerRecoverSpeed";
//	string strBFightRecover = "B_FightEnerRecoverSpeed";
//	string strEBaseRecover = "E_EnerRecoverSpeed";
//
//	string strEFightRecover = "E_FightEnerRecoverSpeed";
//	long lMaxEnergy = GetAttribute(strMaxEnergy) + GetAttribute(strEMaxEnergy);
//
//
//	if (this->GetState()==STATE_FIGHT)
//	{
//		lNum = GetAttribute(strBFightRecover) + GetAttribute(strEFightRecover);
//	} 
//	else if(this->GetState()==STATE_PEACE)
//	{
//		lNum = GetAttribute(strBBaseRecover) + GetAttribute(strEBaseRecover);
//	}
//
//	long lCurrentValue = lNum + GetAttribute(strEnergy);
//
//	if (lCurrentValue<0)
//	{	
//		lCurrentValue = 0;		
//	}
//	else if (lCurrentValue>=lMaxEnergy)
//	{
//		lCurrentValue = lMaxEnergy;
//	}			
//	if (lCurrentValue==GetAttribute(strEnergy))
//	{
//		return;
//	}
//	SetAttribute(strEnergy,lCurrentValue);
//	UpdateAttribute(1);
//}
//pk值注册定时器
void	CPlayer::RegisterPKValue()
{
	if(m_lPKTimerID!=-1)// || GetPkCount()==0
		return;
	tagTimerVar* pTimvar = OBJ_CREATE(tagTimerVar);
	pTimvar->TimerType = ChildObjType_PKValue;
	pTimvar->lvar = 0;
	m_lPKTimerID = TIMER_SCHEDULE(this,(void*)pTimvar,timeGetTime(),GlobalSetup::GetSetup()->dwOnePkCountTime);
}
//pvp值注册定时器
void	CPlayer::RegisterPvPValue()
{
	if(m_lPvPTimerID!=-1)// || GetPVPValue()==0
		return;
	tagTimerVar* pTimvar =  OBJ_CREATE(tagTimerVar);
	pTimvar->TimerType = ChildObjType_PvPValue;
	pTimvar->lvar = 0;
	m_lPvPTimerID = TIMER_SCHEDULE(this,(void*)pTimvar,timeGetTime(),GlobalSetup::GetSetup()->dwOnePkCountTime);
}
//pk值注消定时器
void	CPlayer::UnRegisterPKValue()
{
	if(m_lPKTimerID != -1)
		TIMER_CANCEL(m_lPKTimerID);
	m_lPKTimerID = -1;
}
//pvp值注消定时器
void	CPlayer::UnRegisterPvPValue()
{
	if(m_lPvPTimerID != -1)
		TIMER_CANCEL(m_lPvPTimerID);
	m_lPvPTimerID = -1;
}
//注册自动恢复HP,MP定时器ID
void CPlayer::RegisterAutoReNewHpMp()
{
	if (m_lHpMpTimerID!=-1 )
		return;
	tagTimerVar *pTimType = OBJ_CREATE(tagTimerVar);
	pTimType->TimerType = ChildObjType_AutoRecoverHpMp;
	pTimType->lvar = 0;
	m_lHpMpTimerID = TIMER_SCHEDULE(this,(void*)pTimType,timeGetTime(),GlobalSetup::GetSetup()->dwHpMpRestoreTime);

}
//注消自动恢复HP,MP定时器
void CPlayer::UnRegisterAutoReNewHpMp()
{
	if (m_lHpMpTimerID != -1)
	{
		TIMER_CANCEL(m_lHpMpTimerID);
	}
	m_lHpMpTimerID = -1;
}
//注册能量值恢复
//void	CPlayer::RegisterEnergyUpdate(long lUpdateTime)
//{
//	if (m_lEnergyTimerID!=-1 || lUpdateTime==0)
//		return;
//	tagTimerVar *pTimType = OBJ_CREATE(tagTimerVar);
//	pTimType->TimerType = ChildObjType_AutoRecoverEnergy;
//	pTimType->lvar = 0;
//	m_lEnergyTimerID = TIMER_SCHEDULE(this,(void*)pTimType,timeGetTime(),lUpdateTime);
//}

//注销能量值恢复
//void	CPlayer::UnRegisterEnergyUpdate		()
//{
//	if (m_lEnergyTimerID != -1)
//	{
//		TIMER_CANCEL(m_lEnergyTimerID);
//	}
//	m_lEnergyTimerID = -1;
//}

//死亡注册回城复活定时器
void	CPlayer::RegisterReliveTimerID()
{
	if (m_lReliveTimerID != -1)
		return;
	
	tagTimerVar *pTimType = OBJ_CREATE(tagTimerVar);
	pTimType->TimerType = ChildObjType_ReliveTimer;
	pTimType->lvar = 0;
	m_lReliveTimerID = TIMER_SCHEDULE(this,(void*)pTimType,timeGetTime()+GlobalSetup::GetSetup()->lReLiveTime,0);
	
}

//复活注销回城复活定时器
void	CPlayer::UnRegisterReliveTimerID()
{
	if (m_lReliveTimerID != -1)
	{
		TIMER_CANCEL(m_lReliveTimerID);
	}
	m_lReliveTimerID = -1;
}


void CPlayer::OnDecreaseMurdererSign()
{
	//##死亡状态下，不能减免杀人数。
	if( IsDied() ) return;

	WORD wMurdererCounts = (WORD)GetPkValue();
	DWORD dwPvPValue = GetPVPValue();
	if (wMurdererCounts>0)
	{
		SetPkValue(GetPkValue() - 1);

		if (GetPkCount()==0)
			m_dwMurdererTimeStamp = 0;
		else
			m_dwMurdererTimeStamp = timeGetTime();
	}
	if (dwPvPValue>0)
	{
		SetAttribute("PVPValue",GetPVPValue()-1);
		UpdateAttribute(TRUE);
		if (GetPVPValue()==0)
		{
			m_dwMurdererTimeStamp = 0;
		}
		else
		{
			m_dwMurdererTimeStamp = timeGetTime();
		}
	}


}

//##被伤害后的处理
void CPlayer::OnBeenHurted( LONG lAttackerType, const CGUID& AttackerID )
{
	//EnterCombatState();
	OnArmorDamaged();

	CServerRegion* pRegion = dynamic_cast<CServerRegion*>( GetFather() );
	if( pRegion )
	{
		CPlayer* pAttacker = NULL;
		if( TYPE_PET == lAttackerType )
		{
			if( lAttackerType == TYPE_PET )
			{
				CMoveShape* pPet = GetGame()->FindShape(TYPE_PET, AttackerID);
				if( pPet != NULL )
				{
					pAttacker = (CPlayer*)(pPet->GetHost());
				}
			}
		}
		else if( TYPE_PLAYER == lAttackerType )
		{
			CPlayer* pAttacker = GetGame() -> FindPlayer( AttackerID );
		}

		if( pAttacker != NULL )
		{
			// 通知
			// 允许通知 不同国家 非国战时间 在非竞技区
			if( pAttacker && pRegion->GetNotify() &&
				GetCountry() != pAttacker->GetCountry() &&
				//!CPKSys::getInstance()->IsCountryWarState(this, pMudder) && 
				pRegion->GetSecurity(GetTileX(), GetTileY()) != CRegion::SECURTIY_FIGHT )
			{

				if(  (long)(timeGetTime() - pRegion->GetLastNotifyHurtTime()) > pRegion->GetNotify() )
				{
					char szInfo[1024];
					pRegion->SetLastNotifyHurtTime( timeGetTime() );

					// 通知本国在线
					// 有他国国民在<场景>，坐标x，y袭击本国国民，请速去支援。
					_snprintf(szInfo, 1024, "敌人在<%s>[%u,%u]袭击本国国民，请速去支援。",
						pRegion->GetName(), GetTileX(), GetTileY() );
					CMessage msg(MSG_S2W_OTHER_TALK_NOTIFY);
					msg.Add(BYTE(0));
					msg.Add(GetCountry());	// 国家ID
					msg.Add(szInfo);
					msg.Send();

					//// 死在他国 或者中立区域
					//if( GetCountry() != pRegion->GetCountry() || !pRegion->GetCountry() ) 
					//{
					//	// 通知他国在线
					//	// [玩家名]在[场景]成功击杀了敌人！
					//	_snprintf(szInfo, "[%s]场景<%s>成功击杀了敌人。",
					//		pAttacker->GetName(), pRegion->GetName());

					//	CMessage msg(MSG_S2W_OTHER_TALK_NOTIFY);
					//	msg.Add(pMudder->GetCountry());	// 国家ID
					//	msg.Add(szInfo);
					//	msg.Send();
					//}
				}
			}
		}
	}
}

void CPlayer::OnUpdateMurdererSign()
{
	if (GetPkValue()==0)
	{
		m_dwMurdererTimeStamp = 0;
	}
	else if( m_dwMurdererTimeStamp == 0 )
	{
		m_dwMurdererTimeStamp = timeGetTime();
	}
}

void CPlayer::OnClearMurdererSign()
{
	if( GetPkValue() == 0 )
	{
		m_dwMurdererTimeStamp	= 0;
	}
}

//##报告凶手
void CPlayer::ReportMurderer( const CGUID& MurdererID )
{
	CPlayer* pMurderer = GetGame() -> FindPlayer( MurdererID );
	if( pMurderer )
	{
		long lNum = pMurderer -> GetPkValue() + GlobalSetup::GetSetup()->dwPkCountPerKill;
		if (lNum > 65535)
			lNum = 65535;

		//pMurderer->SetAttribute("PkValue",lNum);
		//pMurderer->SetPkValue(lNum);
		pMurderer->SetPkValue(lNum);

		pMurderer -> OnUpdateMurdererSign();
	}
	//##交给WorldServer处理杀人数
	else
	{
		CMessage msg( MSG_S2W_SERVER_REPORT_MURDERER );
		msg.Add( GetType() );
		msg.Add( GetExID() );
		msg.Add( GetKilledMeType() );
		msg.Add( GetKilledMeID() );
		//##发送给世界服务器处理
		msg.Send();
	}
}

bool	CPlayer::IsBadman()
{
	return GetPkValue() > GlobalSetup::GetSetup()->dwPkCountPerKill || m_dwSinStateTimeStamp > 0;
}


//##被杀死后的处理
void CPlayer::OnBeenMurdered( LONG lMurdererType, const CGUID& MurdererGuid )
{

	CServerRegion* pRegion = dynamic_cast<CServerRegion*>( GetFather() );
	if( !pRegion ) return;

	//##保存数据
	CGUID MurdererID	= MurdererGuid;
	if( lMurdererType == TYPE_PLAYER )
	{
		CPlayer* pMurderer = GetGame() -> FindPlayer( MurdererID );
		if( pMurderer )
		{
			GetLogicLogInterface()->logT140_player_pkdie_log(this, pMurderer);
			//! 自动记录PK名单
			LinkmanSystem::GetInstance().NotePkPlayer(this, pMurderer);
			//##增加杀人总数
			pMurderer -> SetPkCount( pMurderer -> GetPkCount() + 1 );
		}
		//进入Pk系统判断
		GetPKSys()->OnKill(pMurderer,this,pRegion);
	}
}

// 死亡处理
void CPlayer::OnDied()
{
	// 玩家死亡，收回侍从
	CPet* activePet = GetActivePet();
	if (activePet!=NULL)
	{
		activePet->ChangeStateType(PET_STATE_FIGHT);
	}

	CServerRegion* pRegion = dynamic_cast<CServerRegion*>(GetFather());
	if (!pRegion)
		return;

	//##清除阻挡
	CServerRegion::tagCell *pCell = pRegion->GetCell(GetTileX(),GetTileY());
	if (!pCell)
		return;

    if( GetPhaseID()!=NULL_GUID)
        PHASEMAN.OnPhaseEvent( GetPhaseID(),this, eDie );

	CRegion::eSecurity Security = pRegion->GetSecurity(GetTileX(), GetTileY());

	// 爆物品 --------------------------------------------------------------------------------------------
	DropParticularGoodsWhenDead();

	// 掉物品开始
	// 只有在大于14级，或者是坏人才掉东西
	//if( GetLevel() > GlobalSetup::GetSetup()->m_btNewbieLevelLimit || IsBadman() )
	//{
	//	LONG64 lCurLevelExp = PlayerList::GetPlayerTotalExp(GetLevel()-1) - PlayerList::GetPlayerTotalExp(GetLevel()-2);

	//	LONG64 lLossExp = GetPKSys()->GetDiedLostExp(this,pRegion,Security);

	//	LONG64 lTotalExp = GetTotalExp();
	//	LONG64 lLvlExp = PlayerList::GetPlayerTotalExp(GetLevel()-2);
	//	if (lTotalExp-lLossExp<lLvlExp)
	//	{
	//		lLossExp = lTotalExp - lLvlExp;
	//	}

	//	if (lLossExp>0)
	//	{
	//		SetExp(lTotalExp-lLossExp);

	//		CMessage msg(MSG_S2C_PLAYER_ADDEXP);

	//		msg.Add(GetExp());
	//		msg.Add(GetExpMultiple());
	//		//			msg.Add(GetVigour());
	//		msg.SendToPlayer(GetExID());
	//	}

	//	long lNameColor = 0;
	//	if(m_dwSinStateTimeStamp>0)
	//		lNameColor = 3;
	//	else if (GetPkValue() > 0 && GetPkValue() <= GlobalSetup::GetSetup()->dwPkCountPerAttack)
	//		lNameColor = 1;
	//	else if(GetPkValue() > GlobalSetup::GetSetup()->dwPkCountPerAttack)
	//		lNameColor = 2;

	//	long lDropGoodsStartPos = 0;		// 爆出物品的起始位置
	//	LONG lX, lY;

		//得到是否掉落的参数
//		if(GetPKSys()->GetDiedLostGoods(this,GetKilledMeType(),GetKilledMeID(),pRegion,Security,lNameColor))
//		{
//			// 只有在大于40级，或者是坏人才掉东西
//			if( GetLevel() > GlobalSetup::GetSetup()->m_btNewSoldierLevel || IsBadman() )
//			{
//				//------------------
//				// 装备区域
//				//------------------
//				/*
//				enum eEquipType 
//				{	
//				EC_HEAD,				//	0：头盔
//				EC_NECKLACE,			//	1：项链
//				EC_WING,				//	2：翅膀
//				EC_BODY,				//	3：盔甲
//				EC_BACK,				//	4：腰带
//				EC_GLOVE,			//5：手套	
//				EC_BOOT,			//6：鞋子
//				EC_HEADGEAR,			//	7：头饰
//				EC_FROCK,			//	8：外套		
//				EC_LRING,			//	9：左戒指
//				EC_RRING,			//	10：右戒指		
//				EC_MEDAL1,			//	11：勋章1
//				EC_MEDAL2,			//	12	勋章2
//				EC_MEDAL3,			//	13	勋章3
//				EC_FAIRY,			//	14：小精灵
//				EC_WEAPON,			//	15：武器
//				EC_WEAPON2,			//	16：副手武器
//				EC_CLOAK,			//17披风
//				EC_HORSE,			//18坐骑
//				EC_TOTAL
//				};
//				*/
//				// 头
//				if(m_cEquipment.GetGoods(EC_HEAD) && random(10000) < DiedLost.fDropEquip_Head*10000)
//				{
//					pRegion -> GetDropGoodsPos( GetTileX(), GetTileY(), lX, lY, lDropGoodsStartPos );
//					if( lDropGoodsStartPos != -1 )
//
//					{
//						CGoods* pGoods = dynamic_cast<CGoods*>( m_cEquipment.GetGoods(EC_HEAD) );
//						if( pGoods && ( pGoods -> GetAddonPropertyValues(CGoodsBaseProperties::GAP_PARTICULAR_ATTRIBUTE, 1) &
//							CGoodsBaseProperties::PA_NEVER_DROP_DOWN ) != CGoodsBaseProperties::PA_NEVER_DROP_DOWN )
//						{
//							if (!GoodsExManage::GetInstance().IsFrost(pGoods) && !GoodsExManage::GetInstance().IsBind(pGoods)) //! 如果冻结
//							{								
//
//							CS2CContainerObjectMove comMsg;
//							pGoods = dynamic_cast<CGoods*>( m_cEquipment.Remove( static_cast<DWORD>(EC_HEAD), static_cast<DWORD>(1), &comMsg) );
//							if( pGoods )
//							{
//								pGoods -> SetPosXY( lX+0.5f, lY+0.5f );
//								pRegion -> AddObject( pGoods );
//
//								//##发消息给自己
//								comMsg.SetSourceContainerExtendID( CS2CContainerObjectMove::PEI_EQUIPMENT );
//								comMsg.SetOperation( CS2CContainerObjectMove::OT_MOVE_OBJECT );
//								comMsg.SetDestinationContainer( pRegion -> GetType(), pRegion -> GetExID(), 
//									lY * pRegion -> GetWidth() + lX );
//								comMsg.SetDestinationContainerExtendID( 0 );
//								comMsg.SetDestinationObject( pGoods -> GetType(), pGoods -> GetExID() );
//								comMsg.SetDestinationObjectAmount( pGoods -> GetAmount() );
//								comMsg.Send( GetExID() );
//
//								//##给周围玩家发送消息
//								CMessage MsgSend( MSG_S2C_RGN_ADDSHAPE,pGoods->GetExID());
//								MsgSend.Add( pGoods->GetType() );
//								MsgSend.Add( pGoods->GetExID() );
//								DBWriteSet setWriteDB;
//								MsgSend.GetDBWriteSet(setWriteDB);
//								pGoods->SerializeForRgnDisplay(setWriteDB);
//
//								MsgSend.SendToAround(pGoods);
//
//								lDropGoodsStartPos ++;
//#ifdef _GOODSLOG1_
//								char pszGoodsLog[1024]="";
//								char playerGUID[50]="";
//								GetExID().tostring(playerGUID);
//								char goodsGUID[50]="";
//								pGoods->GetExID().tostring(goodsGUID);
//								_snprintf(pszGoodsLog,1024,"[操作:玩家死亡掉装备][玩家名:%s][玩家GUID:%s][场景ID:%d][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d][X坐标:%d][Y坐标:%d]",
//									GetName(),playerGUID,pRegion->GetID(),pGoods->GetBasePropertiesIndex(),GoodsSetup::QueryGoodsName(pGoods->GetBasePropertiesIndex()),
//									goodsGUID,pGoods->GetAmount(),GetTileX(),GetTileY());
//								CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
//#endif
//
//								GetLogicLogInterface()->logT050_goods_log_fall(this,pGoods);
//
//							}
//							}
//						}
//					}					
//				}
//
//				//项链
//				if(m_cEquipment.GetGoods(EC_NECKLACE) && random(10000) < DiedLost.fDropEquip_Necklace*10000)
//				{
//					pRegion -> GetDropGoodsPos( GetTileX(), GetTileY(), lX, lY, lDropGoodsStartPos );
//					if( lDropGoodsStartPos != -1 )
//
//					{
//						CGoods* pGoods = dynamic_cast<CGoods*>( m_cEquipment.GetGoods(EC_NECKLACE) );
//						if( pGoods && ( pGoods -> GetAddonPropertyValues(CGoodsBaseProperties::GAP_PARTICULAR_ATTRIBUTE, 1) &
//							CGoodsBaseProperties::PA_NEVER_DROP_DOWN ) != CGoodsBaseProperties::PA_NEVER_DROP_DOWN )
//						{
//							if (!GoodsExManage::GetInstance().IsFrost(pGoods) && !GoodsExManage::GetInstance().IsBind(pGoods)) //! 如果冻结
//							{
//								
//							CS2CContainerObjectMove comMsg;
//							pGoods = dynamic_cast<CGoods*>( m_cEquipment.Remove( static_cast<DWORD>(EC_NECKLACE), static_cast<DWORD>(1), &comMsg) );
//							if( pGoods )
//							{
//								pGoods -> SetPosXY( lX+0.5f, lY+0.5f );
//								pRegion -> AddObject( pGoods );
//
//								//##发消息给自己
//								comMsg.SetSourceContainerExtendID( CS2CContainerObjectMove::PEI_EQUIPMENT );
//								comMsg.SetOperation( CS2CContainerObjectMove::OT_MOVE_OBJECT );
//								comMsg.SetDestinationContainer( pRegion -> GetType(), pRegion -> GetExID(), 
//									lY * pRegion -> GetWidth() + lX );
//								comMsg.SetDestinationContainerExtendID( 0 );
//								comMsg.SetDestinationObject( pGoods -> GetType(), pGoods -> GetExID() );
//								comMsg.SetDestinationObjectAmount( pGoods -> GetAmount() );
//								comMsg.Send( GetExID() );
//
//								//##给周围玩家发送消息
//								CMessage MsgSend( MSG_S2C_RGN_ADDSHAPE,pGoods->GetExID());
//								MsgSend.Add( pGoods->GetType() );
//								MsgSend.Add( pGoods->GetExID() );
//								DBWriteSet setWriteDB;
//								MsgSend.GetDBWriteSet(setWriteDB);
//								pGoods->SerializeForRgnDisplay(setWriteDB);
//
//								MsgSend.SendToAround(pGoods);
//
//								lDropGoodsStartPos ++;
//#ifdef _GOODSLOG1_
//								char pszGoodsLog[1024]="";
//								char playerGUID[50]="";
//								GetExID().tostring(playerGUID);
//								char goodsGUID[50]="";
//								pGoods->GetExID().tostring(goodsGUID);
//								_snprintf(pszGoodsLog,1024,"[操作:玩家死亡掉装备][玩家名:%s][玩家GUID:%s][场景ID:%d][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d][X坐标:%d][Y坐标:%d]",
//									GetName(),playerGUID,pRegion->GetID(),pGoods->GetBasePropertiesIndex(),GoodsSetup::QueryGoodsName(pGoods->GetBasePropertiesIndex()),
//									goodsGUID,pGoods->GetAmount(),GetTileX(),GetTileY());
//								CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
//#endif
//
//								GetLogicLogInterface()->logT050_goods_log_fall(this,pGoods);
//							}
//							}
//						}
//					}					
//				}
//
//				//翅膀
//				if(m_cEquipment.GetGoods(EC_WING) && random(10000) < DiedLost.fDropEquip_Wing*10000)
//				{
//					pRegion -> GetDropGoodsPos( GetTileX(), GetTileY(), lX, lY, lDropGoodsStartPos );
//					if( lDropGoodsStartPos != -1 )
//
//					{
//						CGoods* pGoods = dynamic_cast<CGoods*>( m_cEquipment.GetGoods(EC_WING) );
//						if( pGoods && ( pGoods -> GetAddonPropertyValues(CGoodsBaseProperties::GAP_PARTICULAR_ATTRIBUTE, 1) &
//							CGoodsBaseProperties::PA_NEVER_DROP_DOWN ) != CGoodsBaseProperties::PA_NEVER_DROP_DOWN )
//						{
//							if (!GoodsExManage::GetInstance().IsFrost(pGoods) && !GoodsExManage::GetInstance().IsBind(pGoods)) //! 如果冻结
//							{
//								
//							CS2CContainerObjectMove comMsg;
//							pGoods = dynamic_cast<CGoods*>( m_cEquipment.Remove( static_cast<DWORD>(EC_WING), static_cast<DWORD>(1), &comMsg) );
//							if( pGoods )
//							{
//								pGoods -> SetPosXY( lX+0.5f, lY+0.5f );
//								pRegion -> AddObject( pGoods );
//
//								//##发消息给自己
//								comMsg.SetSourceContainerExtendID( CS2CContainerObjectMove::PEI_EQUIPMENT );
//								comMsg.SetOperation( CS2CContainerObjectMove::OT_MOVE_OBJECT );
//								comMsg.SetDestinationContainer( pRegion -> GetType(), pRegion -> GetExID(), 
//									lY * pRegion -> GetWidth() + lX );
//								comMsg.SetDestinationContainerExtendID( 0 );
//								comMsg.SetDestinationObject( pGoods -> GetType(), pGoods -> GetExID() );
//								comMsg.SetDestinationObjectAmount( pGoods -> GetAmount() );
//								comMsg.Send( GetExID() );
//
//								//##给周围玩家发送消息
//								CMessage MsgSend( MSG_S2C_RGN_ADDSHAPE,pGoods->GetExID());
//								MsgSend.Add( pGoods->GetType() );
//								MsgSend.Add( pGoods->GetExID() );
//								DBWriteSet setWriteDB;
//								MsgSend.GetDBWriteSet(setWriteDB);
//								pGoods->SerializeForRgnDisplay(setWriteDB);
//
//								MsgSend.SendToAround(pGoods);
//
//								lDropGoodsStartPos ++;
//#ifdef _GOODSLOG1_
//								char pszGoodsLog[1024]="";
//								char playerGUID[50]="";
//								GetExID().tostring(playerGUID);
//								char goodsGUID[50]="";
//								pGoods->GetExID().tostring(goodsGUID);
//								_snprintf(pszGoodsLog,1024,"[操作:玩家死亡掉装备][玩家名:%s][玩家GUID:%s][场景ID:%d][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d][X坐标:%d][Y坐标:%d]",
//									GetName(),playerGUID,pRegion->GetID(),pGoods->GetBasePropertiesIndex(),GoodsSetup::QueryGoodsName(pGoods->GetBasePropertiesIndex()),
//									goodsGUID,pGoods->GetAmount(),GetTileX(),GetTileY());
//								CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
//#endif
//
//								GetLogicLogInterface()->logT050_goods_log_fall(this,pGoods);
//							}
//							}
//						}
//					}					
//				}
//
//				// 盔甲
//				if (m_cEquipment.GetGoods(EC_BODY) && random(10000) < DiedLost.fDropEquip_Body*10000)
//				{
//					pRegion -> GetDropGoodsPos( GetTileX(), GetTileY(), lX, lY, lDropGoodsStartPos );
//					if( lDropGoodsStartPos != -1 )
//
//					{
//						CGoods* pGoods = dynamic_cast<CGoods*>( m_cEquipment.GetGoods(EC_BODY) );
//						if( pGoods && ( pGoods -> GetAddonPropertyValues(CGoodsBaseProperties::GAP_PARTICULAR_ATTRIBUTE, 1) &
//							CGoodsBaseProperties::PA_NEVER_DROP_DOWN ) != CGoodsBaseProperties::PA_NEVER_DROP_DOWN )
//						{
//							if (!GoodsExManage::GetInstance().IsFrost(pGoods) && !GoodsExManage::GetInstance().IsBind(pGoods)) //! 如果冻结
//							{
//								
//							CS2CContainerObjectMove comMsg;
//							pGoods = dynamic_cast<CGoods*>( m_cEquipment.Remove( static_cast<DWORD>(EC_BODY), static_cast<DWORD>(1), &comMsg) );
//							if( pGoods )
//							{
//								pGoods -> SetPosXY( lX+0.5f, lY+0.5f );
//								pRegion -> AddObject( pGoods );
//
//								//##发消息给自己
//								comMsg.SetSourceContainerExtendID( CS2CContainerObjectMove::PEI_EQUIPMENT );
//								comMsg.SetOperation( CS2CContainerObjectMove::OT_MOVE_OBJECT );
//								comMsg.SetDestinationContainer( pRegion -> GetType(), pRegion -> GetExID(), 
//									lY * pRegion -> GetWidth() + lX );
//								comMsg.SetDestinationContainerExtendID( 0 );
//								comMsg.SetDestinationObject( pGoods -> GetType(), pGoods -> GetExID() );
//								comMsg.SetDestinationObjectAmount( pGoods -> GetAmount() );
//								comMsg.Send( GetExID() );
//
//								//##给周围玩家发送消息
//								CMessage MsgSend( MSG_S2C_RGN_ADDSHAPE,pGoods->GetExID());
//								MsgSend.Add( pGoods->GetType() );
//								MsgSend.Add( pGoods->GetExID() );
//								DBWriteSet setWriteDB;
//								MsgSend.GetDBWriteSet(setWriteDB);
//								pGoods->SerializeForRgnDisplay(setWriteDB);
//
//								MsgSend.SendToAround(pGoods);
//
//								lDropGoodsStartPos ++;
//#ifdef _GOODSLOG1_
//								char pszGoodsLog[1024]="";
//								char playerGUID[50]="";
//								GetExID().tostring(playerGUID);
//								char goodsGUID[50]="";
//								pGoods->GetExID().tostring(goodsGUID);
//								_snprintf(pszGoodsLog,1024,"[操作:玩家死亡掉装备][玩家名:%s][玩家GUID:%s][场景ID:%d][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d][X坐标:%d][Y坐标:%d]",
//									GetName(),playerGUID,pRegion->GetID(),pGoods->GetBasePropertiesIndex(),GoodsSetup::QueryGoodsName(pGoods->GetBasePropertiesIndex()),
//									goodsGUID,pGoods->GetAmount(),GetTileX(),GetTileY());
//								CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
//#endif
//
//								GetLogicLogInterface()->logT050_goods_log_fall(this,pGoods);
//							}
//							}
//						}
//					}					
//				}
//
//				//腰带
//				if(m_cEquipment.GetGoods(EC_BACK) && random(10000) < DiedLost.fDropEquip_Back*10000)
//				{
//					pRegion -> GetDropGoodsPos( GetTileX(), GetTileY(), lX, lY, lDropGoodsStartPos );
//					if( lDropGoodsStartPos != -1 )
//
//					{
//						CGoods* pGoods = dynamic_cast<CGoods*>( m_cEquipment.GetGoods(EC_BACK) );
//						if( pGoods && ( pGoods -> GetAddonPropertyValues(CGoodsBaseProperties::GAP_PARTICULAR_ATTRIBUTE, 1) &
//							CGoodsBaseProperties::PA_NEVER_DROP_DOWN ) != CGoodsBaseProperties::PA_NEVER_DROP_DOWN )
//						{
//							if (!GoodsExManage::GetInstance().IsFrost(pGoods) && !GoodsExManage::GetInstance().IsBind(pGoods)) //! 如果冻结
//							{
//								
//							CS2CContainerObjectMove comMsg;
//							pGoods = dynamic_cast<CGoods*>( m_cEquipment.Remove( static_cast<DWORD>(EC_BACK), static_cast<DWORD>(1), &comMsg) );
//							if( pGoods )
//							{
//								pGoods -> SetPosXY( lX+0.5f, lY+0.5f );
//								pRegion -> AddObject( pGoods );
//
//								//##发消息给自己
//								comMsg.SetSourceContainerExtendID( CS2CContainerObjectMove::PEI_EQUIPMENT );
//								comMsg.SetOperation( CS2CContainerObjectMove::OT_MOVE_OBJECT );
//								comMsg.SetDestinationContainer( pRegion -> GetType(), pRegion -> GetExID(), 
//									lY * pRegion -> GetWidth() + lX );
//								comMsg.SetDestinationContainerExtendID( 0 );
//								comMsg.SetDestinationObject( pGoods -> GetType(), pGoods -> GetExID() );
//								comMsg.SetDestinationObjectAmount( pGoods -> GetAmount() );
//								comMsg.Send( GetExID() );
//
//								//##给周围玩家发送消息
//								CMessage MsgSend( MSG_S2C_RGN_ADDSHAPE,pGoods->GetExID());
//								MsgSend.Add( pGoods->GetType() );
//								MsgSend.Add( pGoods->GetExID() );
//								DBWriteSet setWriteDB;
//								MsgSend.GetDBWriteSet(setWriteDB);
//								pGoods->SerializeForRgnDisplay(setWriteDB);
//
//								MsgSend.SendToAround(pGoods);
//
//								lDropGoodsStartPos ++;
//#ifdef _GOODSLOG1_
//								char pszGoodsLog[1024]="";
//								char playerGUID[50]="";
//								GetExID().tostring(playerGUID);
//								char goodsGUID[50]="";
//								pGoods->GetExID().tostring(goodsGUID);
//								_snprintf(pszGoodsLog,1024,"[操作:玩家死亡掉装备][玩家名:%s][玩家GUID:%s][场景ID:%d][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d][X坐标:%d][Y坐标:%d]",
//									GetName(),playerGUID,pRegion->GetID(),pGoods->GetBasePropertiesIndex(),GoodsSetup::QueryGoodsName(pGoods->GetBasePropertiesIndex()),
//									goodsGUID,pGoods->GetAmount(),GetTileX(),GetTileY());
//								CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
//#endif
//
//								GetLogicLogInterface()->logT050_goods_log_fall(this,pGoods);
//							}
//							}
//						}
//					}					
//				}
//
//				//手套
//				if(m_cEquipment.GetGoods(EC_GLOVE) && random(10000) < DiedLost.fDropEquip_Glove*10000)
//				{
//					pRegion -> GetDropGoodsPos( GetTileX(), GetTileY(), lX, lY, lDropGoodsStartPos );
//					if( lDropGoodsStartPos != -1 )
//
//					{
//						CGoods* pGoods = dynamic_cast<CGoods*>( m_cEquipment.GetGoods(EC_GLOVE) );
//						if( pGoods && ( pGoods -> GetAddonPropertyValues(CGoodsBaseProperties::GAP_PARTICULAR_ATTRIBUTE, 1) &
//							CGoodsBaseProperties::PA_NEVER_DROP_DOWN ) != CGoodsBaseProperties::PA_NEVER_DROP_DOWN )
//						{
//							if (!GoodsExManage::GetInstance().IsFrost(pGoods) && !GoodsExManage::GetInstance().IsBind(pGoods)) //! 如果冻结
//							{
//								
//							CS2CContainerObjectMove comMsg;
//							pGoods = dynamic_cast<CGoods*>( m_cEquipment.Remove( static_cast<DWORD>(EC_GLOVE), static_cast<DWORD>(1), &comMsg) );
//							if( pGoods )
//							{
//								pGoods -> SetPosXY( lX+0.5f, lY+0.5f );
//								pRegion -> AddObject( pGoods );
//
//								//##发消息给自己
//								comMsg.SetSourceContainerExtendID( CS2CContainerObjectMove::PEI_EQUIPMENT );
//								comMsg.SetOperation( CS2CContainerObjectMove::OT_MOVE_OBJECT );
//								comMsg.SetDestinationContainer( pRegion -> GetType(), pRegion -> GetExID(), 
//									lY * pRegion -> GetWidth() + lX );
//								comMsg.SetDestinationContainerExtendID( 0 );
//								comMsg.SetDestinationObject( pGoods -> GetType(), pGoods -> GetExID() );
//								comMsg.SetDestinationObjectAmount( pGoods -> GetAmount() );
//								comMsg.Send( GetExID() );
//
//								//##给周围玩家发送消息
//								CMessage MsgSend( MSG_S2C_RGN_ADDSHAPE,pGoods->GetExID());
//								MsgSend.Add( pGoods->GetType() );
//								MsgSend.Add( pGoods->GetExID() );
//								DBWriteSet setWriteDB;
//								MsgSend.GetDBWriteSet(setWriteDB);
//								pGoods->SerializeForRgnDisplay(setWriteDB);
//
//								MsgSend.SendToAround(pGoods);
//
//								lDropGoodsStartPos ++;
//#ifdef _GOODSLOG1_
//								char pszGoodsLog[1024]="";
//								char playerGUID[50]="";
//								GetExID().tostring(playerGUID);
//								char goodsGUID[50]="";
//								pGoods->GetExID().tostring(goodsGUID);
//								_snprintf(pszGoodsLog,1024,"[操作:玩家死亡掉装备][玩家名:%s][玩家GUID:%s][场景ID:%d][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d][X坐标:%d][Y坐标:%d]",
//									GetName(),playerGUID,pRegion->GetID(),pGoods->GetBasePropertiesIndex(),GoodsSetup::QueryGoodsName(pGoods->GetBasePropertiesIndex()),
//									goodsGUID,pGoods->GetAmount(),GetTileX(),GetTileY());
//								CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
//#endif
//
//								GetLogicLogInterface()->logT050_goods_log_fall(this,pGoods);
//							}
//							}
//						}
//					}					
//				}
//
//				//鞋子
//				if(m_cEquipment.GetGoods(EC_BOOT) && random(10000) < DiedLost.fDropEquip_Boot*10000)
//				{
//					pRegion -> GetDropGoodsPos( GetTileX(), GetTileY(), lX, lY, lDropGoodsStartPos );
//					if( lDropGoodsStartPos != -1 )
//
//					{
//						CGoods* pGoods = dynamic_cast<CGoods*>( m_cEquipment.GetGoods(EC_BOOT) );
//						if( pGoods && ( pGoods -> GetAddonPropertyValues(CGoodsBaseProperties::GAP_PARTICULAR_ATTRIBUTE, 1) &
//							CGoodsBaseProperties::PA_NEVER_DROP_DOWN ) != CGoodsBaseProperties::PA_NEVER_DROP_DOWN )
//						{
//							if (!GoodsExManage::GetInstance().IsFrost(pGoods) && !GoodsExManage::GetInstance().IsBind(pGoods)) //! 如果冻结
//							{
//								
//							CS2CContainerObjectMove comMsg;
//							pGoods = dynamic_cast<CGoods*>( m_cEquipment.Remove( static_cast<DWORD>(EC_BOOT), static_cast<DWORD>(1), &comMsg) );
//							if( pGoods )
//							{
//								pGoods -> SetPosXY( lX+0.5f, lY+0.5f );
//								pRegion -> AddObject( pGoods );
//
//								//##发消息给自己
//								comMsg.SetSourceContainerExtendID( CS2CContainerObjectMove::PEI_EQUIPMENT );
//								comMsg.SetOperation( CS2CContainerObjectMove::OT_MOVE_OBJECT );
//								comMsg.SetDestinationContainer( pRegion -> GetType(), pRegion -> GetExID(), 
//									lY * pRegion -> GetWidth() + lX );
//								comMsg.SetDestinationContainerExtendID( 0 );
//								comMsg.SetDestinationObject( pGoods -> GetType(), pGoods -> GetExID() );
//								comMsg.SetDestinationObjectAmount( pGoods -> GetAmount() );
//								comMsg.Send( GetExID() );
//
//								//##给周围玩家发送消息
//								CMessage MsgSend( MSG_S2C_RGN_ADDSHAPE,pGoods->GetExID());
//								MsgSend.Add( pGoods->GetType() );
//								MsgSend.Add( pGoods->GetExID() );
//								DBWriteSet setWriteDB;
//								MsgSend.GetDBWriteSet(setWriteDB);
//								pGoods->SerializeForRgnDisplay(setWriteDB);
//
//								MsgSend.SendToAround(pGoods);
//
//								lDropGoodsStartPos ++;
//#ifdef _GOODSLOG1_
//								char pszGoodsLog[1024]="";
//								char playerGUID[50]="";
//								GetExID().tostring(playerGUID);
//								char goodsGUID[50]="";
//								pGoods->GetExID().tostring(goodsGUID);
//								_snprintf(pszGoodsLog,1024,"[操作:玩家死亡掉装备][玩家名:%s][玩家GUID:%s][场景ID:%d][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d][X坐标:%d][Y坐标:%d]",
//									GetName(),playerGUID,pRegion->GetID(),pGoods->GetBasePropertiesIndex(),GoodsSetup::QueryGoodsName(pGoods->GetBasePropertiesIndex()),
//									goodsGUID,pGoods->GetAmount(),GetTileX(),GetTileY());
//								CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
//#endif
//
//								GetLogicLogInterface()->logT050_goods_log_fall(this,pGoods);
//							}
//							}
//						}
//					}					
//				}
//
//				//头饰
//				if(m_cEquipment.GetGoods(EC_HEADGEAR) && random(10000) < DiedLost.fDropEquip_Headgear*10000)
//				{
//					pRegion -> GetDropGoodsPos( GetTileX(), GetTileY(), lX, lY, lDropGoodsStartPos );
//					if( lDropGoodsStartPos != -1 )
//
//					{
//						CGoods* pGoods = dynamic_cast<CGoods*>( m_cEquipment.GetGoods(EC_HEADGEAR) );
//						if( pGoods && ( pGoods -> GetAddonPropertyValues(CGoodsBaseProperties::GAP_PARTICULAR_ATTRIBUTE, 1) &
//							CGoodsBaseProperties::PA_NEVER_DROP_DOWN ) != CGoodsBaseProperties::PA_NEVER_DROP_DOWN )
//						{
//							if (!GoodsExManage::GetInstance().IsFrost(pGoods) && !GoodsExManage::GetInstance().IsBind(pGoods)) //! 如果冻结
//							{
//								
//							CS2CContainerObjectMove comMsg;
//							pGoods = dynamic_cast<CGoods*>( m_cEquipment.Remove( static_cast<DWORD>(EC_HEADGEAR), static_cast<DWORD>(1), &comMsg) );
//							if( pGoods )
//							{
//								pGoods -> SetPosXY( lX+0.5f, lY+0.5f );
//								pRegion -> AddObject( pGoods );
//
//								//##发消息给自己
//								comMsg.SetSourceContainerExtendID( CS2CContainerObjectMove::PEI_EQUIPMENT );
//								comMsg.SetOperation( CS2CContainerObjectMove::OT_MOVE_OBJECT );
//								comMsg.SetDestinationContainer( pRegion -> GetType(), pRegion -> GetExID(), 
//									lY * pRegion -> GetWidth() + lX );
//								comMsg.SetDestinationContainerExtendID( 0 );
//								comMsg.SetDestinationObject( pGoods -> GetType(), pGoods -> GetExID() );
//								comMsg.SetDestinationObjectAmount( pGoods -> GetAmount() );
//					L:			comMsg.Send( GetExID() );
//
//								//##给周围玩家发送消息
//								CMessage MsgSend( MSG_S2C_RGN_ADDSHAPE,pGoods->GetExID());
//								MsgSend.Add( pGoods->GetType() );
//								MsgSend.Add( pGoods->GetExID() );
//								DBWriteSet setWriteDB;
//								MsgSend.GetDBWriteSet(setWriteDB);
//								pGoods->SerializeForRgnDisplay(setWriteDB);
//
//								MsgSend.SendToAround(pGoods);
//
//								lDropGoodsStartPos ++;
//#ifdef _GOODSLOG1_
//								char pszGoodsLog[1024]="";
//								char playerGUID[50]="";
//								GetExID().tostring(playerGUID);
//								char goodsGUID[50]="";
//								pGoods->GetExID().tostring(goodsGUID);
//								_snprintf(pszGoodsLog,1024,"[操作:玩家死亡掉装备][玩家名:%s][玩家GUID:%s][场景ID:%d][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d][X坐标:%d][Y坐标:%d]",
//									GetName(),playerGUID,pRegion->GetID(),pGoods->GetBasePropertiesIndex(),GoodsSetup::QueryGoodsName(pGoods->GetBasePropertiesIndex()),
//									goodsGUID,pGoods->GetAmount(),GetTileX(),GetTileY());
//								CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
//#endif
//
//								GetLogicLogInterface()->logT050_goods_log_fall(this,pGoods);
//							}
//							}
//						}
//					}					
//				}
//
//				//外套
//				if(m_cEquipment.GetGoods(EC_FROCK) && random(10000) < DiedLost.fDropEquip_Frock*10000)
//				{
//					pRegion -> GetDropGoodsPos( GetTileX(), GetTileY(), lX, lY, lDropGoodsStartPos );
//					if( lDropGoodsStartPos != -1 )
//
//					{
//						CGoods* pGoods = dynamic_cast<CGoods*>( m_cEquipment.GetGoods(EC_FROCK) );
//						if( pGoods && ( pGoods -> GetAddonPropertyValues(CGoodsBaseProperties::GAP_PARTICULAR_ATTRIBUTE, 1) &
//							CGoodsBaseProperties::PA_NEVER_DROP_DOWN ) != CGoodsBaseProperties::PA_NEVER_DROP_DOWN )
//						{
//							if (!GoodsExManage::GetInstance().IsFrost(pGoods) && !GoodsExManage::GetInstance().IsBind(pGoods)) //! 如果冻结
//							{
//								
//							
//							CS2CContainerObjectMove comMsg;
//							pGoods = dynamic_cast<CGoods*>( m_cEquipment.Remove( static_cast<DWORD>(EC_FROCK), static_cast<DWORD>(1), &comMsg) );
//							if( pGoods )
//							{
//								pGoods -> SetPosXY( lX+0.5f, lY+0.5f );
//								pRegion -> AddObject( pGoods );
//
//								//##发消息给自己
//								comMsg.SetSourceContainerExtendID( CS2CContainerObjectMove::PEI_EQUIPMENT );
//								comMsg.SetOperation( CS2CContainerObjectMove::OT_MOVE_OBJECT );
//								comMsg.SetDestinationContainer( pRegion -> GetType(), pRegion -> GetExID(), 
//									lY * pRegion -> GetWidth() + lX );
//								comMsg.SetDestinationContainerExtendID( 0 );
//								comMsg.SetDestinationObject( pGoods -> GetType(), pGoods -> GetExID() );
//								comMsg.SetDestinationObjectAmount( pGoods -> GetAmount() );
//								comMsg.Send( GetExID() );
//
//								//##给周围玩家发送消息
//								CMessage MsgSend( MSG_S2C_RGN_ADDSHAPE,pGoods->GetExID());
//								MsgSend.Add( pGoods->GetType() );
//								MsgSend.Add( pGoods->GetExID() );
//								DBWriteSet setWriteDB;
//								MsgSend.GetDBWriteSet(setWriteDB);
//								pGoods->SerializeForRgnDisplay(setWriteDB);
//
//								MsgSend.SendToAround(pGoods);
//
//								lDropGoodsStartPos ++;
//#ifdef _GOODSLOG1_
//								char pszGoodsLog[1024]="";
//								char playerGUID[50]="";
//								GetExID().tostring(playerGUID);
//								char goodsGUID[50]="";
//								pGoods->GetExID().tostring(goodsGUID);
//								_snprintf(pszGoodsLog,1024,"[操作:玩家死亡掉装备][玩家名:%s][玩家GUID:%s][场景ID:%d][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d][X坐标:%d][Y坐标:%d]",
//									GetName(),playerGUID,pRegion->GetID(),pGoods->GetBasePropertiesIndex(),GoodsSetup::QueryGoodsName(pGoods->GetBasePropertiesIndex()),
//									goodsGUID,pGoods->GetAmount(),GetTileX(),GetTileY());
//								CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
//#endif
//
//								GetLogicLogInterface()->logT050_goods_log_fall(this,pGoods);
//							}
//							}
//						}
//					}					
//				}
//
//				//LRING
//				if(m_cEquipment.GetGoods(EC_LRING) && random(10000) < DiedLost.fDropEquip_LRing*10000)
//				{
//					pRegion -> GetDropGoodsPos( GetTileX(), GetTileY(), lX, lY, lDropGoodsStartPos );
//					if( lDropGoodsStartPos != -1 )
//
//					{
//						CGoods* pGoods = dynamic_cast<CGoods*>( m_cEquipment.GetGoods(EC_LRING) );
//						if( pGoods && ( pGoods -> GetAddonPropertyValues(CGoodsBaseProperties::GAP_PARTICULAR_ATTRIBUTE, 1) &
//							CGoodsBaseProperties::PA_NEVER_DROP_DOWN ) != CGoodsBaseProperties::PA_NEVER_DROP_DOWN )
//						{
//							if (!GoodsExManage::GetInstance().IsFrost(pGoods) && !GoodsExManage::GetInstance().IsBind(pGoods)) //! 如果冻结
//							{
//								
//							CS2CContainerObjectMove comMsg;
//							pGoods = dynamic_cast<CGoods*>( m_cEquipment.Remove( static_cast<DWORD>(EC_LRING), static_cast<DWORD>(1), &comMsg) );
//							if( pGoods )
//							{
//								pGoods -> SetPosXY( lX+0.5f, lY+0.5f );
//								pRegion -> AddObject( pGoods );
//
//								//##发消息给自己
//								comMsg.SetSourceContainerExtendID( CS2CContainerObjectMove::PEI_EQUIPMENT );
//								comMsg.SetOperation( CS2CContainerObjectMove::OT_MOVE_OBJECT );
//								comMsg.SetDestinationContainer( pRegion -> GetType(), pRegion -> GetExID(), 
//									lY * pRegion -> GetWidth() + lX );
//								comMsg.SetDestinationContainerExtendID( 0 );
//								comMsg.SetDestinationObject( pGoods -> GetType(), pGoods -> GetExID() );
//								comMsg.SetDestinationObjectAmount( pGoods -> GetAmount() );
//								comMsg.Send( GetExID() );
//
//								//##给周围玩家发送消息
//								CMessage MsgSend( MSG_S2C_RGN_ADDSHAPE,pGoods->GetExID());
//								MsgSend.Add( pGoods->GetType() );
//								MsgSend.Add( pGoods->GetExID() );
//								DBWriteSet setWriteDB;
//								MsgSend.GetDBWriteSet(setWriteDB);
//								pGoods->SerializeForRgnDisplay(setWriteDB);
//
//								MsgSend.SendToAround(pGoods);
//
//								lDropGoodsStartPos ++;
//#ifdef _GOODSLOG1_
//								char pszGoodsLog[1024]="";
//								char playerGUID[50]="";
//								GetExID().tostring(playerGUID);
//								char goodsGUID[50]="";
//								pGoods->GetExID().tostring(goodsGUID);
//								_snprintf(pszGoodsLog,1024,"[操作:玩家死亡掉装备][玩家名:%s][玩家GUID:%s][场景ID:%d][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d][X坐标:%d][Y坐标:%d]",
//									GetName(),playerGUID,pRegion->GetID(),pGoods->GetBasePropertiesIndex(),GoodsSetup::QueryGoodsName(pGoods->GetBasePropertiesIndex()),
//									goodsGUID,pGoods->GetAmount(),GetTileX(),GetTileY());
//								CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
//#endif
//
//								GetLogicLogInterface()->logT050_goods_log_fall(this,pGoods);
//							}
//							}
//						}
//					}					
//				}
//
//				//RRING
//				if(m_cEquipment.GetGoods(EC_RRING) && random(10000) < DiedLost.fDropEquip_RRing*10000)
//				{
//					pRegion -> GetDropGoodsPos( GetTileX(), GetTileY(), lX, lY, lDropGoodsStartPos );
//					if( lDropGoodsStartPos != -1 )
//
//					{
//						CGoods* pGoods = dynamic_cast<CGoods*>( m_cEquipment.GetGoods(EC_RRING) );
//						if( pGoods && ( pGoods -> GetAddonPropertyValues(CGoodsBaseProperties::GAP_PARTICULAR_ATTRIBUTE, 1) &
//							CGoodsBaseProperties::PA_NEVER_DROP_DOWN ) != CGoodsBaseProperties::PA_NEVER_DROP_DOWN )
//						{
//							if (!GoodsExManage::GetInstance().IsFrost(pGoods) && !GoodsExManage::GetInstance().IsBind(pGoods)) //! 如果冻结
//							{
//								
//							CS2CContainerObjectMove comMsg;
//							pGoods = dynamic_cast<CGoods*>( m_cEquipment.Remove( static_cast<DWORD>(EC_RRING), static_cast<DWORD>(1), &comMsg) );
//							if( pGoods )
//							{
//								pGoods -> SetPosXY( lX+0.5f, lY+0.5f );
//								pRegion -> AddObject( pGoods );
//
//								//##发消息给自己
//								comMsg.SetSourceContainerExtendID( CS2CContainerObjectMove::PEI_EQUIPMENT );
//								comMsg.SetOperation( CS2CContainerObjectMove::OT_MOVE_OBJECT );
//								comMsg.SetDestinationContainer( pRegion -> GetType(), pRegion -> GetExID(), 
//									lY * pRegion -> GetWidth() + lX );
//								comMsg.SetDestinationContainerExtendID( 0 );
//								comMsg.SetDestinationObject( pGoods -> GetType(), pGoods -> GetExID() );
//								comMsg.SetDestinationObjectAmount( pGoods -> GetAmount() );
//								comMsg.Send( GetExID() );
//
//								//##给周围玩家发送消息
//								CMessage MsgSend( MSG_S2C_RGN_ADDSHAPE,pGoods->GetExID());
//								MsgSend.Add( pGoods->GetType() );
//								MsgSend.Add( pGoods->GetExID() );
//								DBWriteSet setWriteDB;
//								MsgSend.GetDBWriteSet(setWriteDB);
//								pGoods->SerializeForRgnDisplay(setWriteDB);
//
//								MsgSend.SendToAround(pGoods);
//
//								lDropGoodsStartPos ++;
//#ifdef _GOODSLOG1_
//								char pszGoodsLog[1024]="";
//								char playerGUID[50]="";
//								GetExID().tostring(playerGUID);
//								char goodsGUID[50]="";
//								pGoods->GetExID().tostring(goodsGUID);
//								_snprintf(pszGoodsLog,1024,"[操作:玩家死亡掉装备][玩家名:%s][玩家GUID:%s][场景ID:%d][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d][X坐标:%d][Y坐标:%d]",
//									GetName(),playerGUID,pRegion->GetID(),pGoods->GetBasePropertiesIndex(),GoodsSetup::QueryGoodsName(pGoods->GetBasePropertiesIndex()),
//									goodsGUID,pGoods->GetAmount(),GetTileX(),GetTileY());
//								CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
//#endif
//
//								GetLogicLogInterface()->logT050_goods_log_fall(this,pGoods);
//							}
//							}
//						}
//					}					
//				}
//
//				//精灵
//				if(m_cEquipment.GetGoods(EC_FAIRY) && random(10000) < DiedLost.fDropEquip_Fairy*10000)
//				{
//					pRegion -> GetDropGoodsPos( GetTileX(), GetTileY(), lX, lY, lDropGoodsStartPos );
//					if( lDropGoodsStartPos != -1 )
//
//					{
//						CGoods* pGoods = dynamic_cast<CGoods*>( m_cEquipment.GetGoods(EC_FAIRY) );
//						if( pGoods && ( pGoods -> GetAddonPropertyValues(CGoodsBaseProperties::GAP_PARTICULAR_ATTRIBUTE, 1) &
//							CGoodsBaseProperties::PA_NEVER_DROP_DOWN ) != CGoodsBaseProperties::PA_NEVER_DROP_DOWN )
//						{
//							if (!GoodsExManage::GetInstance().IsFrost(pGoods) && !GoodsExManage::GetInstance().IsBind(pGoods)) //! 如果冻结
//							{
//								
//							CS2CContainerObjectMove comMsg;
//							pGoods = dynamic_cast<CGoods*>( m_cEquipment.Remove( static_cast<DWORD>(EC_FAIRY), static_cast<DWORD>(1), &comMsg) );
//							if( pGoods )
//							{
//								pGoods -> SetPosXY( lX+0.5f, lY+0.5f );
//								pRegion -> AddObject( pGoods );
//
//								//##发消息给自己
//								comMsg.SetSourceContainerExtendID( CS2CContainerObjectMove::PEI_EQUIPMENT );
//								comMsg.SetOperation( CS2CContainerObjectMove::OT_MOVE_OBJECT );
//								comMsg.SetDestinationContainer( pRegion -> GetType(), pRegion -> GetExID(), 
//									lY * pRegion -> GetWidth() + lX );
//								comMsg.SetDestinationContainerExtendID( 0 );
//								comMsg.SetDestinationObject( pGoods -> GetType(), pGoods -> GetExID() );
//								comMsg.SetDestinationObjectAmount( pGoods -> GetAmount() );
//								comMsg.Send( GetExID() );
//
//								//##给周围玩家发送消息
//								CMessage MsgSend( MSG_S2C_RGN_ADDSHAPE,pGoods->GetExID());
//								MsgSend.Add( pGoods->GetType() );
//								MsgSend.Add( pGoods->GetExID() );
//								DBWriteSet setWriteDB;
//								MsgSend.GetDBWriteSet(setWriteDB);
//								pGoods->SerializeForRgnDisplay(setWriteDB);
//
//								MsgSend.SendToAround(pGoods);
//
//								lDropGoodsStartPos ++;
//#ifdef _GOODSLOG1_
//								char pszGoodsLog[1024]="";
//								char playerGUID[50]="";
//								GetExID().tostring(playerGUID);
//								char goodsGUID[50]="";
//								pGoods->GetExID().tostring(goodsGUID);
//								_snprintf(pszGoodsLog,1024,"[操作:玩家死亡掉装备][玩家名:%s][玩家GUID:%s][场景ID:%d][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d][X坐标:%d][Y坐标:%d]",
//									GetName(),playerGUID,pRegion->GetID(),pGoods->GetBasePropertiesIndex(),GoodsSetup::QueryGoodsName(pGoods->GetBasePropertiesIndex()),
//									goodsGUID,pGoods->GetAmount(),GetTileX(),GetTileY());
//								CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
//#endif
//
//								GetLogicLogInterface()->logT050_goods_log_fall(this,pGoods);
//							}
//							}
//						}
//					}					
//				}
//
//				// 武器
//				if (m_cEquipment.GetGoods(EC_WEAPON) && random(10000) < DiedLost.fDropEquip_Weapon*10000)
//				{
//					pRegion -> GetDropGoodsPos( GetTileX(), GetTileY(), lX, lY, lDropGoodsStartPos );
//					if( lDropGoodsStartPos != -1 )
//
//					{
//						CGoods* pGoods = dynamic_cast<CGoods*>( m_cEquipment.GetGoods(EC_WEAPON) );
//						if( pGoods && ( pGoods -> GetAddonPropertyValues(CGoodsBaseProperties::GAP_PARTICULAR_ATTRIBUTE, 1) &
//							CGoodsBaseProperties::PA_NEVER_DROP_DOWN ) != CGoodsBaseProperties::PA_NEVER_DROP_DOWN )
//						{
//							if (!GoodsExManage::GetInstance().IsFrost(pGoods) && !GoodsExManage::GetInstance().IsBind(pGoods)) //! 如果冻结
//							{
//								
//							CS2CContainerObjectMove comMsg;
//							pGoods = dynamic_cast<CGoods*>( m_cEquipment.Remove( static_cast<DWORD>(EC_WEAPON), static_cast<DWORD>(1), &comMsg) );
//							if( pGoods )
//							{
//								pGoods -> SetPosXY( lX+0.5f, lY+0.5f );
//								pRegion -> AddObject( pGoods );
//
//								//##发消息给自己
//								comMsg.SetSourceContainerExtendID( CS2CContainerObjectMove::PEI_EQUIPMENT );
//								comMsg.SetOperation( CS2CContainerObjectMove::OT_MOVE_OBJECT );
//								comMsg.SetDestinationContainer( pRegion -> GetType(), pRegion -> GetExID(), 
//									lY * pRegion -> GetWidth() + lX );
//								comMsg.SetDestinationContainerExtendID( 0 );
//								comMsg.SetDestinationObject( pGoods -> GetType(), pGoods -> GetExID() );
//								comMsg.SetDestinationObjectAmount( pGoods -> GetAmount() );
//								comMsg.Send( GetExID() );
//
//								//##给周围玩家发送消息
//								CMessage MsgSend( MSG_S2C_RGN_ADDSHAPE,pGoods->GetExID());
//								MsgSend.Add( pGoods->GetType() );
//								MsgSend.Add( pGoods->GetExID() );
//								DBWriteSet setWriteDB;
//								MsgSend.GetDBWriteSet(setWriteDB);
//								pGoods->SerializeForRgnDisplay(setWriteDB);
//
//								MsgSend.SendToAround(pGoods);
//
//								lDropGoodsStartPos ++;
//#ifdef _GOODSLOG1_
//								char pszGoodsLog[1024]="";
//								char playerGUID[50]="";
//								GetExID().tostring(playerGUID);
//								char goodsGUID[50]="";
//								pGoods->GetExID().tostring(goodsGUID);
//								_snprintf(pszGoodsLog,1024,"[操作:玩家死亡掉装备][玩家名:%s][玩家GUID:%s][场景ID:%d][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d][X坐标:%d][Y坐标:%d]",
//									GetName(),playerGUID,pRegion->GetID(),pGoods->GetBasePropertiesIndex(),GoodsSetup::QueryGoodsName(pGoods->GetBasePropertiesIndex()),
//									goodsGUID,pGoods->GetAmount(),GetTileX(),GetTileY());
//								CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
//#endif
//
//								GetLogicLogInterface()->logT050_goods_log_fall(this,pGoods);
//							}
//							}
//						}
//					}					
//				}
//
//				//副手武器
//				if(m_cEquipment.GetGoods(EC_WEAPON2) && random(10000) < DiedLost.fDropEquip_Weapon2*10000)
//				{
//					pRegion -> GetDropGoodsPos( GetTileX(), GetTileY(), lX, lY, lDropGoodsStartPos );
//					if( lDropGoodsStartPos != -1 )
//
//					{
//						CGoods* pGoods = dynamic_cast<CGoods*>( m_cEquipment.GetGoods(EC_WEAPON2) );
//						if( pGoods && ( pGoods -> GetAddonPropertyValues(CGoodsBaseProperties::GAP_PARTICULAR_ATTRIBUTE, 1) &
//							CGoodsBaseProperties::PA_NEVER_DROP_DOWN ) != CGoodsBaseProperties::PA_NEVER_DROP_DOWN )
//						{
//							if (!GoodsExManage::GetInstance().IsFrost(pGoods) && !GoodsExManage::GetInstance().IsBind(pGoods)) //! 如果冻结
//							{
//								
//							CS2CContainerObjectMove comMsg;
//							pGoods = dynamic_cast<CGoods*>( m_cEquipment.Remove( static_cast<DWORD>(EC_WEAPON2), static_cast<DWORD>(1), &comMsg) );
//							if( pGoods )
//							{
//								pGoods -> SetPosXY( lX+0.5f, lY+0.5f );
//								pRegion -> AddObject( pGoods );
//
//								//##发消息给自己
//								comMsg.SetSourceContainerExtendID( CS2CContainerObjectMove::PEI_EQUIPMENT );
//								comMsg.SetOperation( CS2CContainerObjectMove::OT_MOVE_OBJECT );
//								comMsg.SetDestinationContainer( pRegion -> GetType(), pRegion -> GetExID(), 
//									lY * pRegion -> GetWidth() + lX );
//								comMsg.SetDestinationContainerExtendID( 0 );
//								comMsg.SetDestinationObject( pGoods -> GetType(), pGoods -> GetExID() );
//								comMsg.SetDestinationObjectAmount( pGoods -> GetAmount() );
//								comMsg.Send( GetExID() );
//
//								//##给周围玩家发送消息
//								CMessage MsgSend( MSG_S2C_RGN_ADDSHAPE,pGoods->GetExID());
//								MsgSend.Add( pGoods->GetType() );
//								MsgSend.Add( pGoods->GetExID() );
//								DBWriteSet setWriteDB;
//								MsgSend.GetDBWriteSet(setWriteDB);
//								pGoods->SerializeForRgnDisplay(setWriteDB);
//
//								MsgSend.SendToAround(pGoods);
//								lDropGoodsStartPos ++;
//#ifdef _GOODSLOG1_
//								char pszGoodsLog[1024]="";
//								char playerGUID[50]="";
//								GetExID().tostring(playerGUID);
//								char goodsGUID[50]="";
//								pGoods->GetExID().tostring(goodsGUID);
//								_snprintf(pszGoodsLog,1024,"[操作:玩家死亡掉装备][玩家名:%s][玩家GUID:%s][场景ID:%d][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d][X坐标:%d][Y坐标:%d]",
//									GetName(),playerGUID,pRegion->GetID(),pGoods->GetBasePropertiesIndex(),GoodsSetup::QueryGoodsName(pGoods->GetBasePropertiesIndex()),
//									goodsGUID,pGoods->GetAmount(),GetTileX(),GetTileY());
//								CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
//#endif
//
//								GetLogicLogInterface()->logT050_goods_log_fall(this,pGoods);
//							}
//							}
//						}
//					}					
//				}
//
//				//披风
//				if(m_cEquipment.GetGoods(EC_CLOAK) && random(10000) < DiedLost.fDropEquip_Cloak*10000)
//				{
//					pRegion -> GetDropGoodsPos( GetTileX(), GetTileY(), lX, lY, lDropGoodsStartPos );
//					if( lDropGoodsStartPos != -1 )
//
//					{
//						CGoods* pGoods = dynamic_cast<CGoods*>( m_cEquipment.GetGoods(EC_CLOAK) );
//						if( pGoods && ( pGoods -> GetAddonPropertyValues(CGoodsBaseProperties::GAP_PARTICULAR_ATTRIBUTE, 1) &
//							CGoodsBaseProperties::PA_NEVER_DROP_DOWN ) != CGoodsBaseProperties::PA_NEVER_DROP_DOWN )
//						{
//							if (!GoodsExManage::GetInstance().IsFrost(pGoods) && !GoodsExManage::GetInstance().IsBind(pGoods)) //! 如果冻结
//							{
//								
//							CS2CContainerObjectMove comMsg;
//							pGoods = dynamic_cast<CGoods*>( m_cEquipment.Remove( static_cast<DWORD>(EC_CLOAK), static_cast<DWORD>(1), &comMsg) );
//							if( pGoods )
//							{
//								pGoods -> SetPosXY( lX+0.5f, lY+0.5f );
//								pRegion -> AddObject( pGoods );
//
//								//##发消息给自己
//								comMsg.SetSourceContainerExtendID( CS2CContainerObjectMove::PEI_EQUIPMENT );
//								comMsg.SetOperation( CS2CContainerObjectMove::OT_MOVE_OBJECT );
//								comMsg.SetDestinationContainer( pRegion -> GetType(), pRegion -> GetExID(), 
//									lY * pRegion -> GetWidth() + lX );
//								comMsg.SetDestinationContainerExtendID( 0 );
//								comMsg.SetDestinationObject( pGoods -> GetType(), pGoods -> GetExID() );
//								comMsg.SetDestinationObjectAmount( pGoods -> GetAmount() );
//								comMsg.Send( GetExID() );
//
//								//##给周围玩家发送消息
//								CMessage MsgSend( MSG_S2C_RGN_ADDSHAPE,pGoods->GetExID());
//								MsgSend.Add( pGoods->GetType() );
//								MsgSend.Add( pGoods->GetExID() );
//								DBWriteSet setWriteDB;
//								MsgSend.GetDBWriteSet(setWriteDB);
//								pGoods->SerializeForRgnDisplay(setWriteDB);
//
//								MsgSend.SendToAround(pGoods);
//
//								lDropGoodsStartPos ++;
//#ifdef _GOODSLOG1_
//								char pszGoodsLog[1024]="";
//								char playerGUID[50]="";
//								GetExID().tostring(playerGUID);
//								char goodsGUID[50]="";
//								pGoods->GetExID().tostring(goodsGUID);
//								_snprintf(pszGoodsLog,1024,"[操作:玩家死亡掉装备][玩家名:%s][玩家GUID:%s][场景ID:%d][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d][X坐标:%d][Y坐标:%d]",
//									GetName(),playerGUID,pRegion->GetID(),pGoods->GetBasePropertiesIndex(),GoodsSetup::QueryGoodsName(pGoods->GetBasePropertiesIndex()),
//									goodsGUID,pGoods->GetAmount(),GetTileX(),GetTileY());
//								CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
//#endif
//
//								GetLogicLogInterface()->logT050_goods_log_fall(this,pGoods);
//							}
//							}
//						}
//					}					
//				}	
//
//
//				//坐骑
//				if(m_cEquipment.GetGoods(EC_HORSE) && random(10000) < DiedLost.fDropEquip_Horse*10000)
//				{
//					pRegion -> GetDropGoodsPos( GetTileX(), GetTileY(), lX, lY, lDropGoodsStartPos );
//					if( lDropGoodsStartPos != -1 )
//
//					{
//						CGoods* pGoods = dynamic_cast<CGoods*>( m_cEquipment.GetGoods(EC_HORSE) );
//						if( pGoods && ( pGoods -> GetAddonPropertyValues(CGoodsBaseProperties::GAP_PARTICULAR_ATTRIBUTE, 1) &
//							CGoodsBaseProperties::PA_NEVER_DROP_DOWN ) != CGoodsBaseProperties::PA_NEVER_DROP_DOWN )
//						{
//							if (!GoodsExManage::GetInstance().IsFrost(pGoods) && !GoodsExManage::GetInstance().IsBind(pGoods)) //! 如果冻结
//							{
//								
//							CS2CContainerObjectMove comMsg;
//							pGoods = dynamic_cast<CGoods*>( m_cEquipment.Remove( static_cast<DWORD>(EC_HORSE), static_cast<DWORD>(1), &comMsg) );
//							if( pGoods )
//							{
//								pGoods -> SetPosXY( lX+0.5f, lY+0.5f );
//								pRegion -> AddObject( pGoods );
//
//								//##发消息给自己
//								comMsg.SetSourceContainerExtendID( CS2CContainerObjectMove::PEI_EQUIPMENT );
//								comMsg.SetOperation( CS2CContainerObjectMove::OT_MOVE_OBJECT );
//								comMsg.SetDestinationContainer( pRegion -> GetType(), pRegion -> GetExID(), 
//									lY * pRegion -> GetWidth() + lX );
//								comMsg.SetDestinationContainerExtendID( 0 );
//								comMsg.SetDestinationObject( pGoods -> GetType(), pGoods -> GetExID() );
//								comMsg.SetDestinationObjectAmount( pGoods -> GetAmount() );
//								comMsg.Send( GetExID() );
//
//								//##给周围玩家发送消息
//								CMessage MsgSend( MSG_S2C_RGN_ADDSHAPE,pGoods->GetExID());
//								MsgSend.Add( pGoods->GetType() );
//								MsgSend.Add( pGoods->GetExID() );
//								DBWriteSet setWriteDB;
//								MsgSend.GetDBWriteSet(setWriteDB);
//								pGoods->SerializeForRgnDisplay(setWriteDB);
//
//								MsgSend.SendToAround(pGoods);
//
//								lDropGoodsStartPos ++;
//#ifdef _GOODSLOG1_
//								char pszGoodsLog[1024]="";
//								char playerGUID[50]="";
//								GetExID().tostring(playerGUID);
//								char goodsGUID[50]="";
//								pGoods->GetExID().tostring(goodsGUID);
//								_snprintf(pszGoodsLog,1024,"[操作:玩家死亡掉装备][玩家名:%s][玩家GUID:%s][场景ID:%d][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d][X坐标:%d][Y坐标:%d]",
//									GetName(),playerGUID,pRegion->GetID(),pGoods->GetBasePropertiesIndex(),GoodsSetup::QueryGoodsName(pGoods->GetBasePropertiesIndex()),
//									goodsGUID,pGoods->GetAmount(),GetTileX(),GetTileY());
//								CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
//#endif
//
//								GetLogicLogInterface()->logT050_goods_log_fall(this,pGoods);
//							}
//							}
//						}
//					}					
//				}	
//
//
//			}
//			else
//			{
//				// 新兵不会凋落装备
//			}
//			//------------------
//			// 背包区域
//			//------------------
//			//##背包
//			CGoodsListListener lGoodsList;
//			m_cOriginPack->TraversingContainer( &lGoodsList );
//			for( size_t i = 0; i < lGoodsList.m_vGoodsID.size(); i ++ )
//			{
//				if( random(10000) < DiedLost.fDropEquip_Bag * 10000 )
//				{
//					pRegion -> GetDropGoodsPos( GetTileX(), GetTileY(), lX, lY, lDropGoodsStartPos );
//					if( lDropGoodsStartPos != -1 )
//					{
//						CGoods* pGoods = dynamic_cast<CGoods*>( m_cOriginPack->Find(TYPE_GOODS, lGoodsList.m_vGoodsID[i]) );
//
//						if(pGoods)
//						{
//							if (GoodsExManage::GetInstance().IsFrost(pGoods) || GoodsExManage::GetInstance().IsBind(pGoods)) //! 如果冻结
//							{
//								continue;
//							}
//						}
//
//						if( pGoods && ( pGoods -> GetAddonPropertyValues(CGoodsBaseProperties::GAP_PARTICULAR_ATTRIBUTE, 1) &
//							CGoodsBaseProperties::PA_NEVER_DROP_DOWN ) == CGoodsBaseProperties::PA_NEVER_DROP_DOWN )
//						{
//							continue;
//						}
//
//						CS2CContainerObjectMove comMsg;
//						pGoods = dynamic_cast<CGoods*>( m_cOriginPack->Remove(TYPE_GOODS, lGoodsList.m_vGoodsID[i], &comMsg) );
//						if( pGoods )
//						{
//							pGoods -> SetPosXY( lX+0.5f, lY+0.5f );
//							pRegion -> AddObject( pGoods );
//
//							//##给自己发消息
//							comMsg.SetSourceContainerExtendID( CS2CContainerObjectMove::PEI_PACKET );
//							comMsg.SetOperation( CS2CContainerObjectMove::OT_MOVE_OBJECT );
//							comMsg.SetDestinationContainer( pRegion -> GetType(), pRegion -> GetExID(), 
//								lY * pRegion -> GetWidth() + lX );
//							comMsg.SetDestinationContainerExtendID( 0 );
//							comMsg.SetDestinationObject( pGoods -> GetType(), pGoods -> GetExID() );
//							comMsg.SetDestinationObjectAmount( pGoods -> GetAmount() );
//							comMsg.Send( GetExID() );
//
//							//##给周围玩家发送消息
//							CMessage MsgSend( MSG_S2C_RGN_ADDSHAPE,pGoods->GetExID());
//							MsgSend.Add( pGoods->GetType() );
//							MsgSend.Add( pGoods->GetExID() );
//							DBWriteSet setWriteDB;
//							MsgSend.GetDBWriteSet(setWriteDB);
//							pGoods->SerializeForRgnDisplay(setWriteDB);
//
//							MsgSend.SendToAround(pGoods);
//
//							lDropGoodsStartPos ++;
//#ifdef _GOODSLOG1_
//							char pszGoodsLog[1024]="";
//							char playerGUID[50]="";
//							GetExID().tostring(playerGUID);
//							char goodsGUID[50]="";
//							pGoods->GetExID().tostring(goodsGUID);
//							_snprintf(pszGoodsLog,1024,"[操作:玩家死亡掉背包物品][玩家名:%s][玩家GUID:%s][背包ID:%d][场景ID:%d][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d][X坐标:%d][Y坐标:%d]",
//								GetName(),playerGUID,1,pRegion->GetID(),pGoods->GetBasePropertiesIndex(),GoodsSetup::QueryGoodsName(pGoods->GetBasePropertiesIndex()),
//								goodsGUID,pGoods->GetAmount(),GetTileX(),GetTileY());
//							CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
//#endif
//
//							GetLogicLogInterface()->logT050_goods_log_fall(this,pGoods);
//						}
//					}
//				}//if end
//			}
//
//
//			for(int i=CS2CContainerObjectMove::PEI_PACK1;i<=CS2CContainerObjectMove::PEI_PACK5;i++)
//			{
//				if(getSubpackContainer()->GetSubpack(i-CS2CContainerObjectMove::PEI_PACK1)->pGoods)
//				{		
//					CVolumeLimitGoodsContainer* pSubContainer=getSubpackContainer()->GetSubpack(i-CS2CContainerObjectMove::PEI_PACK1)->pSubpackContainer;
//					lGoodsList.m_vGoodsID.clear();
//					pSubContainer->TraversingContainer(&lGoodsList);
//					for(size_t j=0;j<lGoodsList.m_vGoodsID.size();j++)
//					{
//						if( random(10000) < DiedLost.fDropEquip_Bag * 10000 )
//						{
//							pRegion -> GetDropGoodsPos( GetTileX(), GetTileY(), lX, lY, lDropGoodsStartPos );
//							if( lDropGoodsStartPos != -1 )
//							{
//								CGoods* pGoods = dynamic_cast<CGoods*>( pSubContainer->Find(TYPE_GOODS, lGoodsList.m_vGoodsID[j]) );
//
//								if(pGoods)
//								{
//									if (GoodsExManage::GetInstance().IsFrost(pGoods) || GoodsExManage::GetInstance().IsBind(pGoods)) //! 如果冻结
//									{
//										continue;
//									}
//								}
//
//								if( pGoods && ( pGoods -> GetAddonPropertyValues(CGoodsBaseProperties::GAP_PARTICULAR_ATTRIBUTE, 1) &
//									CGoodsBaseProperties::PA_NEVER_DROP_DOWN ) == CGoodsBaseProperties::PA_NEVER_DROP_DOWN )
//								{
//									continue;
//								}
//
//								CS2CContainerObjectMove comMsg;
//								pGoods = dynamic_cast<CGoods*>( pSubContainer->Remove(TYPE_GOODS, lGoodsList.m_vGoodsID[j], &comMsg) );
//								if( pGoods )
//								{
//									pGoods -> SetPosXY( lX+0.5f, lY+0.5f );
//									pRegion -> AddObject( pGoods );
//
//									//##给自己发消息
//									comMsg.SetSourceContainerExtendID( i );
//									comMsg.SetOperation( CS2CContainerObjectMove::OT_MOVE_OBJECT );
//									comMsg.SetDestinationContainer( pRegion -> GetType(), pRegion -> GetExID(), 
//										lY * pRegion -> GetWidth() + lX );
//									comMsg.SetDestinationContainerExtendID( 0 );
//									comMsg.SetDestinationObject( pGoods -> GetType(), pGoods -> GetExID() );
//									comMsg.SetDestinationObjectAmount( pGoods -> GetAmount() );
//									comMsg.Send( GetExID() );
//
//									//##给周围玩家发送消息
//									CMessage MsgSend( MSG_S2C_RGN_ADDSHAPE,pGoods->GetExID());
//									MsgSend.Add( pGoods->GetType() );
//									MsgSend.Add( pGoods->GetExID() );
//									DBWriteSet setWriteDB;
//									MsgSend.GetDBWriteSet(setWriteDB);
//									pGoods->SerializeForRgnDisplay(setWriteDB);
//
//									MsgSend.SendToAround(pGoods);
//
//									lDropGoodsStartPos ++;
//#ifdef _GOODSLOG1_
//									char pszGoodsLog[1024]="";
//									char playerGUID[50]="";
//									GetExID().tostring(playerGUID);
//									char goodsGUID[50]="";
//									pGoods->GetExID().tostring(goodsGUID);
//									_snprintf(pszGoodsLog,1024,"[操作:玩家死亡掉背包物品][玩家名:%s][玩家GUID:%s][背包ID:%d][场景ID:%d][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d][X坐标:%d][Y坐标:%d]",
//										GetName(),playerGUID,i,pRegion->GetID(),pGoods->GetBasePropertiesIndex(),GoodsSetup::QueryGoodsName(pGoods->GetBasePropertiesIndex()),
//										goodsGUID,pGoods->GetAmount(),GetTileX(),GetTileY());
//									CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
//#endif
//
//									GetLogicLogInterface()->logT050_goods_log_fall(this,pGoods);
//								}
//							}
//						}//if end
//					}	
//				}
//			}	
//
//
//			//------------------
//			// 其他区域
//			//------------------
//			// 金钱
//			if (GetMoney() && random(10000) < DiedLost.fDropEquip_Money*10000)
//			{
//				DWORD dwDropMoney = static_cast<DWORD>( GetMoney() * DiedLost.fDropEquip_MoneyPercent );
//				if( dwDropMoney>0 && GetMoney() - dwDropMoney >= 0 )
//				{
//					lDropGoodsStartPos = pRegion->GetDropGoodsPos(GetTileX(), GetTileY(), lX, lY, lDropGoodsStartPos);
//					if( lDropGoodsStartPos != -1 )
//					{
//						//extern bool _AddMoneyToRegion(CRegion* p_region, long dx, long dy, DWORD dw_money, long lProtectPlayerID=0, long lProtectTeamID=0);
//						//_AddMoneyToRegion(pRegion, lX, lY, dwDropMoney);
//						SetMoney(GetMoney()-dwDropMoney,7);
//						CGoods* pGoldCoins = CGoodsFactory::CreateGoods( GoodsSetup::GetGoldCoinIndex(),8 );
//						if( pGoldCoins )
//						{
//							pGoldCoins -> SetAmount( dwDropMoney );
//							pGoldCoins -> SetPosXY( lX+0.5f, lY+0.5f );
//							pRegion -> AddObject( pGoldCoins );
//
//							CMessage MsgSend( MSG_S2C_RGN_ADDSHAPE,pGoldCoins->GetExID());
//							MsgSend.Add( pGoldCoins->GetType() );
//							MsgSend.Add( pGoldCoins->GetExID() );
//							DBWriteSet setWriteDB;
//							MsgSend.GetDBWriteSet(setWriteDB);
//							pGoldCoins->SerializeForRgnDisplay(setWriteDB);
//
//							MsgSend.SendToAround(pGoldCoins);
//#ifdef _GOODSLOG1_
//							char pszGoodsLog[1024]="";
//							char playerGUID[50]="";
//							GetExID().tostring(playerGUID);
//							char goodsGUID[50]="";
//							pGoldCoins->GetExID().tostring(goodsGUID);
//							_snprintf(pszGoodsLog,1024,"[操作:玩家死亡掉金币][玩家名:%s][玩家GUID:%s][场景ID:%d][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d][X坐标:%d][Y坐标:%d]",
//								GetName(),playerGUID,pRegion->GetID(),pGoldCoins->GetBasePropertiesIndex(),GoodsSetup::QueryGoodsName(pGoldCoins->GetBasePropertiesIndex()),
//								goodsGUID,pGoldCoins->GetAmount(),GetTileX(),GetTileY());
//							CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
//#endif
//
//							GetLogicLogInterface()->logT050_goods_log_fall(this,pGoldCoins);
//						}
//					}			
//				}
//			}
//		}
		//if(GetPKSys()->GetDiedLostGoods(this,GetKilledMeType(),GetKilledMeID(),pRegion,Security,lNameColor,DiedLost))
		//{
			// 只有在大于40级，或者是坏人才掉东西
			//if( GetLevel() > GlobalSetup::GetSetup()->m_btNewSoldierLevel || IsBadman() )
			//{
				//------------------
				// 装备区域
				//------------------
				/*
				enum EQUIPMENT_COLUMN 
				{	
				EC_HEAD,				//	0：头盔
				EC_NECKLACE,			//	1：项链
				EC_WING,				//	2：翅膀
				EC_BODY,				//	3：盔甲
				EC_BACK,				//	4：腰带
				EC_GLOVE,			//5：手套	
				EC_BOOT,			//6：鞋子
				EC_HEADGEAR,			//	7：头饰
				EC_FROCK,			//	8：外套		
				EC_LRING,			//	9：左戒指
				EC_RRING,			//	10：右戒指		
				EC_MEDAL1,			//	11：勋章1
				EC_MEDAL2,			//	12	勋章2
				EC_MEDAL3,			//	13	勋章3
				EC_FAIRY,			//	14：小精灵
				EC_WEAPON,			//	15：武器
				EC_WEAPON2,			//	16：副手武器
				EC_CLOAK,			//17披风
				EC_HORSE,			//18坐骑
				EC_TOTAL
				};
				*/
				// 头
//				if(m_cEquipment.GetGoods(CEquipmentContainer::EC_HEAD) && random(10000) < DiedLost.fDropEquip_Head*10000)
//				{
//					pRegion -> GetDropGoodsPos( GetTileX(), GetTileY(), lX, lY, lDropGoodsStartPos );
//					if( lDropGoodsStartPos != -1 )
//
//					{
//						CGoods* pGoods = dynamic_cast<CGoods*>( m_cEquipment.GetGoods(CEquipmentContainer::EC_HEAD) );
//						if( pGoods && ( pGoods -> GetAddonPropertyValues(CGoodsBaseProperties::GAP_PARTICULAR_ATTRIBUTE, 1) &
//							CGoodsBaseProperties::PA_NEVER_DROP_DOWN ) != CGoodsBaseProperties::PA_NEVER_DROP_DOWN )
//						{
//							if (!GoodsExManage::GetInstance().IsFrost(pGoods) && !GoodsExManage::GetInstance().IsBind(pGoods)) //! 如果冻结
//							{								
//
//							CS2CContainerObjectMove comMsg;
//							pGoods = dynamic_cast<CGoods*>( m_cEquipment.Remove( static_cast<DWORD>(CEquipmentContainer::EC_HEAD), static_cast<DWORD>(1), &comMsg) );
//							if( pGoods )
//							{
//								pGoods -> SetPosXY( lX+0.5f, lY+0.5f );
//								pRegion -> AddObject( pGoods );
//
//								//##发消息给自己
//								comMsg.SetSourceContainerExtendID( CS2CContainerObjectMove::PEI_EQUIPMENT );
//								comMsg.SetOperation( CS2CContainerObjectMove::OT_MOVE_OBJECT );
//								comMsg.SetDestinationContainer( pRegion -> GetType(), pRegion -> GetExID(), 
//									lY * pRegion -> GetWidth() + lX );
//								comMsg.SetDestinationContainerExtendID( 0 );
//								comMsg.SetDestinationObject( pGoods -> GetType(), pGoods -> GetExID() );
//								comMsg.SetDestinationObjectAmount( pGoods -> GetAmount() );
//								comMsg.Send( GetExID() );
//
//								//##给周围玩家发送消息
//								CMessage MsgSend( MSG_S2C_RGN_ADDSHAPE,pGoods->GetExID());
//								MsgSend.Add( pGoods->GetType() );
//								MsgSend.Add( pGoods->GetExID() );
//								DBWriteSet setWriteDB;
//								MsgSend.GetDBWriteSet(setWriteDB);
//								pGoods->SerializeForRgnDisplay(setWriteDB);
//
//								MsgSend.SendToAround(pGoods);
//
//								lDropGoodsStartPos ++;
//#ifdef _GOODSLOG1_
//								char pszGoodsLog[1024]="";
//								char playerGUID[50]="";
//								GetExID().tostring(playerGUID);
//								char goodsGUID[50]="";
//								pGoods->GetExID().tostring(goodsGUID);
//								_snprintf(pszGoodsLog,1024,"[操作:玩家死亡掉装备][玩家名:%s][玩家GUID:%s][场景ID:%d][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d][X坐标:%d][Y坐标:%d]",
//									GetName(),playerGUID,pRegion->GetID(),pGoods->GetBasePropertiesIndex(),GoodsSetup::QueryGoodsName(pGoods->GetBasePropertiesIndex()),
//									goodsGUID,pGoods->GetAmount(),GetTileX(),GetTileY());
//								CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
//#endif
//
//								GetLogicLogInterface()->logT050_goods_log_fall(this,pGoods);
//
//							}
//							}
//						}
//					}					
//				}
//
//				//项链
//				if(m_cEquipment.GetGoods(CEquipmentContainer::EC_NECKLACE) && random(10000) < DiedLost.fDropEquip_Necklace*10000)
//				{
//					pRegion -> GetDropGoodsPos( GetTileX(), GetTileY(), lX, lY, lDropGoodsStartPos );
//					if( lDropGoodsStartPos != -1 )
//
//					{
//						CGoods* pGoods = dynamic_cast<CGoods*>( m_cEquipment.GetGoods(CEquipmentContainer::EC_NECKLACE) );
//						if( pGoods && ( pGoods -> GetAddonPropertyValues(CGoodsBaseProperties::GAP_PARTICULAR_ATTRIBUTE, 1) &
//							CGoodsBaseProperties::PA_NEVER_DROP_DOWN ) != CGoodsBaseProperties::PA_NEVER_DROP_DOWN )
//						{
//							if (!GoodsExManage::GetInstance().IsFrost(pGoods) && !GoodsExManage::GetInstance().IsBind(pGoods)) //! 如果冻结
//							{
//								
//							CS2CContainerObjectMove comMsg;
//							pGoods = dynamic_cast<CGoods*>( m_cEquipment.Remove( static_cast<DWORD>(CEquipmentContainer::EC_NECKLACE), static_cast<DWORD>(1), &comMsg) );
//							if( pGoods )
//							{
//								pGoods -> SetPosXY( lX+0.5f, lY+0.5f );
//								pRegion -> AddObject( pGoods );
//
//								//##发消息给自己
//								comMsg.SetSourceContainerExtendID( CS2CContainerObjectMove::PEI_EQUIPMENT );
//								comMsg.SetOperation( CS2CContainerObjectMove::OT_MOVE_OBJECT );
//								comMsg.SetDestinationContainer( pRegion -> GetType(), pRegion -> GetExID(), 
//									lY * pRegion -> GetWidth() + lX );
//								comMsg.SetDestinationContainerExtendID( 0 );
//								comMsg.SetDestinationObject( pGoods -> GetType(), pGoods -> GetExID() );
//								comMsg.SetDestinationObjectAmount( pGoods -> GetAmount() );
//								comMsg.Send( GetExID() );
//
//								//##给周围玩家发送消息
//								CMessage MsgSend( MSG_S2C_RGN_ADDSHAPE,pGoods->GetExID());
//								MsgSend.Add( pGoods->GetType() );
//								MsgSend.Add( pGoods->GetExID() );
//								DBWriteSet setWriteDB;
//								MsgSend.GetDBWriteSet(setWriteDB);
//								pGoods->SerializeForRgnDisplay(setWriteDB);
//
//								MsgSend.SendToAround(pGoods);
//
//								lDropGoodsStartPos ++;
//#ifdef _GOODSLOG1_
//								char pszGoodsLog[1024]="";
//								char playerGUID[50]="";
//								GetExID().tostring(playerGUID);
//								char goodsGUID[50]="";
//								pGoods->GetExID().tostring(goodsGUID);
//								_snprintf(pszGoodsLog,1024,"[操作:玩家死亡掉装备][玩家名:%s][玩家GUID:%s][场景ID:%d][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d][X坐标:%d][Y坐标:%d]",
//									GetName(),playerGUID,pRegion->GetID(),pGoods->GetBasePropertiesIndex(),GoodsSetup::QueryGoodsName(pGoods->GetBasePropertiesIndex()),
//									goodsGUID,pGoods->GetAmount(),GetTileX(),GetTileY());
//								CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
//#endif
//
//								GetLogicLogInterface()->logT050_goods_log_fall(this,pGoods);
//							}
//							}
//						}
//					}					
//				}
//
//				//翅膀
//				if(m_cEquipment.GetGoods(CEquipmentContainer::EC_WING) && random(10000) < DiedLost.fDropEquip_Wing*10000)
//				{
//					pRegion -> GetDropGoodsPos( GetTileX(), GetTileY(), lX, lY, lDropGoodsStartPos );
//					if( lDropGoodsStartPos != -1 )
//
//					{
//						CGoods* pGoods = dynamic_cast<CGoods*>( m_cEquipment.GetGoods(CEquipmentContainer::EC_WING) );
//						if( pGoods && ( pGoods -> GetAddonPropertyValues(CGoodsBaseProperties::GAP_PARTICULAR_ATTRIBUTE, 1) &
//							CGoodsBaseProperties::PA_NEVER_DROP_DOWN ) != CGoodsBaseProperties::PA_NEVER_DROP_DOWN )
//						{
//							if (!GoodsExManage::GetInstance().IsFrost(pGoods) && !GoodsExManage::GetInstance().IsBind(pGoods)) //! 如果冻结
//							{
//								
//							CS2CContainerObjectMove comMsg;
//							pGoods = dynamic_cast<CGoods*>( m_cEquipment.Remove( static_cast<DWORD>(CEquipmentContainer::EC_WING), static_cast<DWORD>(1), &comMsg) );
//							if( pGoods )
//							{
//								pGoods -> SetPosXY( lX+0.5f, lY+0.5f );
//								pRegion -> AddObject( pGoods );
//
//								//##发消息给自己
//								comMsg.SetSourceContainerExtendID( CS2CContainerObjectMove::PEI_EQUIPMENT );
//								comMsg.SetOperation( CS2CContainerObjectMove::OT_MOVE_OBJECT );
//								comMsg.SetDestinationContainer( pRegion -> GetType(), pRegion -> GetExID(), 
//									lY * pRegion -> GetWidth() + lX );
//								comMsg.SetDestinationContainerExtendID( 0 );
//								comMsg.SetDestinationObject( pGoods -> GetType(), pGoods -> GetExID() );
//								comMsg.SetDestinationObjectAmount( pGoods -> GetAmount() );
//								comMsg.Send( GetExID() );
//
//								//##给周围玩家发送消息
//								CMessage MsgSend( MSG_S2C_RGN_ADDSHAPE,pGoods->GetExID());
//								MsgSend.Add( pGoods->GetType() );
//								MsgSend.Add( pGoods->GetExID() );
//								DBWriteSet setWriteDB;
//								MsgSend.GetDBWriteSet(setWriteDB);
//								pGoods->SerializeForRgnDisplay(setWriteDB);
//
//								MsgSend.SendToAround(pGoods);
//
//								lDropGoodsStartPos ++;
//#ifdef _GOODSLOG1_
//								char pszGoodsLog[1024]="";
//								char playerGUID[50]="";
//								GetExID().tostring(playerGUID);
//								char goodsGUID[50]="";
//								pGoods->GetExID().tostring(goodsGUID);
//								_snprintf(pszGoodsLog,1024,"[操作:玩家死亡掉装备][玩家名:%s][玩家GUID:%s][场景ID:%d][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d][X坐标:%d][Y坐标:%d]",
//									GetName(),playerGUID,pRegion->GetID(),pGoods->GetBasePropertiesIndex(),GoodsSetup::QueryGoodsName(pGoods->GetBasePropertiesIndex()),
//									goodsGUID,pGoods->GetAmount(),GetTileX(),GetTileY());
//								CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
//#endif
//
//								GetLogicLogInterface()->logT050_goods_log_fall(this,pGoods);
//							}
//							}
//						}
//					}					
//				}
//
//				// 盔甲
//				if (m_cEquipment.GetGoods(CEquipmentContainer::EC_BODY) && random(10000) < DiedLost.fDropEquip_Body*10000)
//				{
//					pRegion -> GetDropGoodsPos( GetTileX(), GetTileY(), lX, lY, lDropGoodsStartPos );
//					if( lDropGoodsStartPos != -1 )
//
//					{
//						CGoods* pGoods = dynamic_cast<CGoods*>( m_cEquipment.GetGoods(CEquipmentContainer::EC_BODY) );
//						if( pGoods && ( pGoods -> GetAddonPropertyValues(CGoodsBaseProperties::GAP_PARTICULAR_ATTRIBUTE, 1) &
//							CGoodsBaseProperties::PA_NEVER_DROP_DOWN ) != CGoodsBaseProperties::PA_NEVER_DROP_DOWN )
//						{
//							if (!GoodsExManage::GetInstance().IsFrost(pGoods) && !GoodsExManage::GetInstance().IsBind(pGoods)) //! 如果冻结
//							{
//								
//							CS2CContainerObjectMove comMsg;
//							pGoods = dynamic_cast<CGoods*>( m_cEquipment.Remove( static_cast<DWORD>(CEquipmentContainer::EC_BODY), static_cast<DWORD>(1), &comMsg) );
//							if( pGoods )
//							{
//								pGoods -> SetPosXY( lX+0.5f, lY+0.5f );
//								pRegion -> AddObject( pGoods );
//
//								//##发消息给自己
//								comMsg.SetSourceContainerExtendID( CS2CContainerObjectMove::PEI_EQUIPMENT );
//								comMsg.SetOperation( CS2CContainerObjectMove::OT_MOVE_OBJECT );
//								comMsg.SetDestinationContainer( pRegion -> GetType(), pRegion -> GetExID(), 
//									lY * pRegion -> GetWidth() + lX );
//								comMsg.SetDestinationContainerExtendID( 0 );
//								comMsg.SetDestinationObject( pGoods -> GetType(), pGoods -> GetExID() );
//								comMsg.SetDestinationObjectAmount( pGoods -> GetAmount() );
//								comMsg.Send( GetExID() );
//
//								//##给周围玩家发送消息
//								CMessage MsgSend( MSG_S2C_RGN_ADDSHAPE,pGoods->GetExID());
//								MsgSend.Add( pGoods->GetType() );
//								MsgSend.Add( pGoods->GetExID() );
//								DBWriteSet setWriteDB;
//								MsgSend.GetDBWriteSet(setWriteDB);
//								pGoods->SerializeForRgnDisplay(setWriteDB);
//
//								MsgSend.SendToAround(pGoods);
//
//								lDropGoodsStartPos ++;
//#ifdef _GOODSLOG1_
//								char pszGoodsLog[1024]="";
//								char playerGUID[50]="";
//								GetExID().tostring(playerGUID);
//								char goodsGUID[50]="";
//								pGoods->GetExID().tostring(goodsGUID);
//								_snprintf(pszGoodsLog,1024,"[操作:玩家死亡掉装备][玩家名:%s][玩家GUID:%s][场景ID:%d][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d][X坐标:%d][Y坐标:%d]",
//									GetName(),playerGUID,pRegion->GetID(),pGoods->GetBasePropertiesIndex(),GoodsSetup::QueryGoodsName(pGoods->GetBasePropertiesIndex()),
//									goodsGUID,pGoods->GetAmount(),GetTileX(),GetTileY());
//								CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
//#endif
//
//								GetLogicLogInterface()->logT050_goods_log_fall(this,pGoods);
//							}
//							}
//						}
//					}					
//				}
//
//				//腰带
//				if(m_cEquipment.GetGoods(CEquipmentContainer::EC_BACK) && random(10000) < DiedLost.fDropEquip_Back*10000)
//				{
//					pRegion -> GetDropGoodsPos( GetTileX(), GetTileY(), lX, lY, lDropGoodsStartPos );
//					if( lDropGoodsStartPos != -1 )
//
//					{
//						CGoods* pGoods = dynamic_cast<CGoods*>( m_cEquipment.GetGoods(CEquipmentContainer::EC_BACK) );
//						if( pGoods && ( pGoods -> GetAddonPropertyValues(CGoodsBaseProperties::GAP_PARTICULAR_ATTRIBUTE, 1) &
//							CGoodsBaseProperties::PA_NEVER_DROP_DOWN ) != CGoodsBaseProperties::PA_NEVER_DROP_DOWN )
//						{
//							if (!GoodsExManage::GetInstance().IsFrost(pGoods) && !GoodsExManage::GetInstance().IsBind(pGoods)) //! 如果冻结
//							{
//								
//							CS2CContainerObjectMove comMsg;
//							pGoods = dynamic_cast<CGoods*>( m_cEquipment.Remove( static_cast<DWORD>(CEquipmentContainer::EC_BACK), static_cast<DWORD>(1), &comMsg) );
//							if( pGoods )
//							{
//								pGoods -> SetPosXY( lX+0.5f, lY+0.5f );
//								pRegion -> AddObject( pGoods );
//
//								//##发消息给自己
//								comMsg.SetSourceContainerExtendID( CS2CContainerObjectMove::PEI_EQUIPMENT );
//								comMsg.SetOperation( CS2CContainerObjectMove::OT_MOVE_OBJECT );
//								comMsg.SetDestinationContainer( pRegion -> GetType(), pRegion -> GetExID(), 
//									lY * pRegion -> GetWidth() + lX );
//								comMsg.SetDestinationContainerExtendID( 0 );
//								comMsg.SetDestinationObject( pGoods -> GetType(), pGoods -> GetExID() );
//								comMsg.SetDestinationObjectAmount( pGoods -> GetAmount() );
//								comMsg.Send( GetExID() );
//
//								//##给周围玩家发送消息
//								CMessage MsgSend( MSG_S2C_RGN_ADDSHAPE,pGoods->GetExID());
//								MsgSend.Add( pGoods->GetType() );
//								MsgSend.Add( pGoods->GetExID() );
//								DBWriteSet setWriteDB;
//								MsgSend.GetDBWriteSet(setWriteDB);
//								pGoods->SerializeForRgnDisplay(setWriteDB);
//
//								MsgSend.SendToAround(pGoods);
//
//								lDropGoodsStartPos ++;
//#ifdef _GOODSLOG1_
//								char pszGoodsLog[1024]="";
//								char playerGUID[50]="";
//								GetExID().tostring(playerGUID);
//								char goodsGUID[50]="";
//								pGoods->GetExID().tostring(goodsGUID);
//								_snprintf(pszGoodsLog,1024,"[操作:玩家死亡掉装备][玩家名:%s][玩家GUID:%s][场景ID:%d][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d][X坐标:%d][Y坐标:%d]",
//									GetName(),playerGUID,pRegion->GetID(),pGoods->GetBasePropertiesIndex(),GoodsSetup::QueryGoodsName(pGoods->GetBasePropertiesIndex()),
//									goodsGUID,pGoods->GetAmount(),GetTileX(),GetTileY());
//								CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
//#endif
//
//								GetLogicLogInterface()->logT050_goods_log_fall(this,pGoods);
//							}
//							}
//						}
//					}					
//				}
//
//				//手套
//				if(m_cEquipment.GetGoods(CEquipmentContainer::EC_GLOVE) && random(10000) < DiedLost.fDropEquip_Glove*10000)
//				{
//					pRegion -> GetDropGoodsPos( GetTileX(), GetTileY(), lX, lY, lDropGoodsStartPos );
//					if( lDropGoodsStartPos != -1 )
//
//					{
//						CGoods* pGoods = dynamic_cast<CGoods*>( m_cEquipment.GetGoods(CEquipmentContainer::EC_GLOVE) );
//						if( pGoods && ( pGoods -> GetAddonPropertyValues(CGoodsBaseProperties::GAP_PARTICULAR_ATTRIBUTE, 1) &
//							CGoodsBaseProperties::PA_NEVER_DROP_DOWN ) != CGoodsBaseProperties::PA_NEVER_DROP_DOWN )
//						{
//							if (!GoodsExManage::GetInstance().IsFrost(pGoods) && !GoodsExManage::GetInstance().IsBind(pGoods)) //! 如果冻结
//							{
//								
//							CS2CContainerObjectMove comMsg;
//							pGoods = dynamic_cast<CGoods*>( m_cEquipment.Remove( static_cast<DWORD>(CEquipmentContainer::EC_GLOVE), static_cast<DWORD>(1), &comMsg) );
//							if( pGoods )
//							{
//								pGoods -> SetPosXY( lX+0.5f, lY+0.5f );
//								pRegion -> AddObject( pGoods );
//
//								//##发消息给自己
//								comMsg.SetSourceContainerExtendID( CS2CContainerObjectMove::PEI_EQUIPMENT );
//								comMsg.SetOperation( CS2CContainerObjectMove::OT_MOVE_OBJECT );
//								comMsg.SetDestinationContainer( pRegion -> GetType(), pRegion -> GetExID(), 
//									lY * pRegion -> GetWidth() + lX );
//								comMsg.SetDestinationContainerExtendID( 0 );
//								comMsg.SetDestinationObject( pGoods -> GetType(), pGoods -> GetExID() );
//								comMsg.SetDestinationObjectAmount( pGoods -> GetAmount() );
//								comMsg.Send( GetExID() );
//
//								//##给周围玩家发送消息
//								CMessage MsgSend( MSG_S2C_RGN_ADDSHAPE,pGoods->GetExID());
//								MsgSend.Add( pGoods->GetType() );
//								MsgSend.Add( pGoods->GetExID() );
//								DBWriteSet setWriteDB;
//								MsgSend.GetDBWriteSet(setWriteDB);
//								pGoods->SerializeForRgnDisplay(setWriteDB);
//
//								MsgSend.SendToAround(pGoods);
//
//								lDropGoodsStartPos ++;
//#ifdef _GOODSLOG1_
//								char pszGoodsLog[1024]="";
//								char playerGUID[50]="";
//								GetExID().tostring(playerGUID);
//								char goodsGUID[50]="";
//								pGoods->GetExID().tostring(goodsGUID);
//								_snprintf(pszGoodsLog,1024,"[操作:玩家死亡掉装备][玩家名:%s][玩家GUID:%s][场景ID:%d][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d][X坐标:%d][Y坐标:%d]",
//									GetName(),playerGUID,pRegion->GetID(),pGoods->GetBasePropertiesIndex(),GoodsSetup::QueryGoodsName(pGoods->GetBasePropertiesIndex()),
//									goodsGUID,pGoods->GetAmount(),GetTileX(),GetTileY());
//								CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
//#endif
//
//								GetLogicLogInterface()->logT050_goods_log_fall(this,pGoods);
//							}
//							}
//						}
//					}					
//				}
//
//				//鞋子
//				if(m_cEquipment.GetGoods(CEquipmentContainer::EC_BOOT) && random(10000) < DiedLost.fDropEquip_Boot*10000)
//				{
//					pRegion -> GetDropGoodsPos( GetTileX(), GetTileY(), lX, lY, lDropGoodsStartPos );
//					if( lDropGoodsStartPos != -1 )
//
//					{
//						CGoods* pGoods = dynamic_cast<CGoods*>( m_cEquipment.GetGoods(CEquipmentContainer::EC_BOOT) );
//						if( pGoods && ( pGoods -> GetAddonPropertyValues(CGoodsBaseProperties::GAP_PARTICULAR_ATTRIBUTE, 1) &
//							CGoodsBaseProperties::PA_NEVER_DROP_DOWN ) != CGoodsBaseProperties::PA_NEVER_DROP_DOWN )
//						{
//							if (!GoodsExManage::GetInstance().IsFrost(pGoods) && !GoodsExManage::GetInstance().IsBind(pGoods)) //! 如果冻结
//							{
//								
//							CS2CContainerObjectMove comMsg;
//							pGoods = dynamic_cast<CGoods*>( m_cEquipment.Remove( static_cast<DWORD>(CEquipmentContainer::EC_BOOT), static_cast<DWORD>(1), &comMsg) );
//							if( pGoods )
//							{
//								pGoods -> SetPosXY( lX+0.5f, lY+0.5f );
//								pRegion -> AddObject( pGoods );
//
//								//##发消息给自己
//								comMsg.SetSourceContainerExtendID( CS2CContainerObjectMove::PEI_EQUIPMENT );
//								comMsg.SetOperation( CS2CContainerObjectMove::OT_MOVE_OBJECT );
//								comMsg.SetDestinationContainer( pRegion -> GetType(), pRegion -> GetExID(), 
//									lY * pRegion -> GetWidth() + lX );
//								comMsg.SetDestinationContainerExtendID( 0 );
//								comMsg.SetDestinationObject( pGoods -> GetType(), pGoods -> GetExID() );
//								comMsg.SetDestinationObjectAmount( pGoods -> GetAmount() );
//								comMsg.Send( GetExID() );
//
//								//##给周围玩家发送消息
//								CMessage MsgSend( MSG_S2C_RGN_ADDSHAPE,pGoods->GetExID());
//								MsgSend.Add( pGoods->GetType() );
//								MsgSend.Add( pGoods->GetExID() );
//								DBWriteSet setWriteDB;
//								MsgSend.GetDBWriteSet(setWriteDB);
//								pGoods->SerializeForRgnDisplay(setWriteDB);
//
//								MsgSend.SendToAround(pGoods);
//
//								lDropGoodsStartPos ++;
//#ifdef _GOODSLOG1_
//								char pszGoodsLog[1024]="";
//								char playerGUID[50]="";
//								GetExID().tostring(playerGUID);
//								char goodsGUID[50]="";
//								pGoods->GetExID().tostring(goodsGUID);
//								_snprintf(pszGoodsLog,1024,"[操作:玩家死亡掉装备][玩家名:%s][玩家GUID:%s][场景ID:%d][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d][X坐标:%d][Y坐标:%d]",
//									GetName(),playerGUID,pRegion->GetID(),pGoods->GetBasePropertiesIndex(),GoodsSetup::QueryGoodsName(pGoods->GetBasePropertiesIndex()),
//									goodsGUID,pGoods->GetAmount(),GetTileX(),GetTileY());
//								CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
//#endif
//
//								GetLogicLogInterface()->logT050_goods_log_fall(this,pGoods);
//							}
//							}
//						}
//					}					
//				}
//
//				//头饰
//				if(m_cEquipment.GetGoods(CEquipmentContainer::EC_HEADGEAR) && random(10000) < DiedLost.fDropEquip_Headgear*10000)
//				{
//					pRegion -> GetDropGoodsPos( GetTileX(), GetTileY(), lX, lY, lDropGoodsStartPos );
//					if( lDropGoodsStartPos != -1 )
//
//					{
//						CGoods* pGoods = dynamic_cast<CGoods*>( m_cEquipment.GetGoods(CEquipmentContainer::EC_HEADGEAR) );
//						if( pGoods && ( pGoods -> GetAddonPropertyValues(CGoodsBaseProperties::GAP_PARTICULAR_ATTRIBUTE, 1) &
//							CGoodsBaseProperties::PA_NEVER_DROP_DOWN ) != CGoodsBaseProperties::PA_NEVER_DROP_DOWN )
//						{
//							if (!GoodsExManage::GetInstance().IsFrost(pGoods) && !GoodsExManage::GetInstance().IsBind(pGoods)) //! 如果冻结
//							{
//								
//							CS2CContainerObjectMove comMsg;
//							pGoods = dynamic_cast<CGoods*>( m_cEquipment.Remove( static_cast<DWORD>(CEquipmentContainer::EC_HEADGEAR), static_cast<DWORD>(1), &comMsg) );
//							if( pGoods )
//							{
//								pGoods -> SetPosXY( lX+0.5f, lY+0.5f );
//								pRegion -> AddObject( pGoods );
//
//								//##发消息给自己
//								comMsg.SetSourceContainerExtendID( CS2CContainerObjectMove::PEI_EQUIPMENT );
//								comMsg.SetOperation( CS2CContainerObjectMove::OT_MOVE_OBJECT );
//								comMsg.SetDestinationContainer( pRegion -> GetType(), pRegion -> GetExID(), 
//									lY * pRegion -> GetWidth() + lX );
//								comMsg.SetDestinationContainerExtendID( 0 );
//								comMsg.SetDestinationObject( pGoods -> GetType(), pGoods -> GetExID() );
//								comMsg.SetDestinationObjectAmount( pGoods -> GetAmount() );
//								comMsg.Send( GetExID() );
//
//								//##给周围玩家发送消息
//								CMessage MsgSend( MSG_S2C_RGN_ADDSHAPE,pGoods->GetExID());
//								MsgSend.Add( pGoods->GetType() );
//								MsgSend.Add( pGoods->GetExID() );
//								DBWriteSet setWriteDB;
//								MsgSend.GetDBWriteSet(setWriteDB);
//								pGoods->SerializeForRgnDisplay(setWriteDB);
//
//								MsgSend.SendToAround(pGoods);
//
//								lDropGoodsStartPos ++;
//#ifdef _GOODSLOG1_
//								char pszGoodsLog[1024]="";
//								char playerGUID[50]="";
//								GetExID().tostring(playerGUID);
//								char goodsGUID[50]="";
//								pGoods->GetExID().tostring(goodsGUID);
//								_snprintf(pszGoodsLog,1024,"[操作:玩家死亡掉装备][玩家名:%s][玩家GUID:%s][场景ID:%d][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d][X坐标:%d][Y坐标:%d]",
//									GetName(),playerGUID,pRegion->GetID(),pGoods->GetBasePropertiesIndex(),GoodsSetup::QueryGoodsName(pGoods->GetBasePropertiesIndex()),
//									goodsGUID,pGoods->GetAmount(),GetTileX(),GetTileY());
//								CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
//#endif
//
//								GetLogicLogInterface()->logT050_goods_log_fall(this,pGoods);
//							}
//							}
//						}
//					}					
//				}
//
//				//外套
//				if(m_cEquipment.GetGoods(CEquipmentContainer::EC_FROCK) && random(10000) < DiedLost.fDropEquip_Frock*10000)
//				{
//					pRegion -> GetDropGoodsPos( GetTileX(), GetTileY(), lX, lY, lDropGoodsStartPos );
//					if( lDropGoodsStartPos != -1 )
//
//					{
//						CGoods* pGoods = dynamic_cast<CGoods*>( m_cEquipment.GetGoods(CEquipmentContainer::EC_FROCK) );
//						if( pGoods && ( pGoods -> GetAddonPropertyValues(CGoodsBaseProperties::GAP_PARTICULAR_ATTRIBUTE, 1) &
//							CGoodsBaseProperties::PA_NEVER_DROP_DOWN ) != CGoodsBaseProperties::PA_NEVER_DROP_DOWN )
//						{
//							if (!GoodsExManage::GetInstance().IsFrost(pGoods) && !GoodsExManage::GetInstance().IsBind(pGoods)) //! 如果冻结
//							{
//								
//							
//							CS2CContainerObjectMove comMsg;
//							pGoods = dynamic_cast<CGoods*>( m_cEquipment.Remove( static_cast<DWORD>(CEquipmentContainer::EC_FROCK), static_cast<DWORD>(1), &comMsg) );
//							if( pGoods )
//							{
//								pGoods -> SetPosXY( lX+0.5f, lY+0.5f );
//								pRegion -> AddObject( pGoods );
//
//								//##发消息给自己
//								comMsg.SetSourceContainerExtendID( CS2CContainerObjectMove::PEI_EQUIPMENT );
//								comMsg.SetOperation( CS2CContainerObjectMove::OT_MOVE_OBJECT );
//								comMsg.SetDestinationContainer( pRegion -> GetType(), pRegion -> GetExID(), 
//									lY * pRegion -> GetWidth() + lX );
//								comMsg.SetDestinationContainerExtendID( 0 );
//								comMsg.SetDestinationObject( pGoods -> GetType(), pGoods -> GetExID() );
//								comMsg.SetDestinationObjectAmount( pGoods -> GetAmount() );
//								comMsg.Send( GetExID() );
//
//								//##给周围玩家发送消息
//								CMessage MsgSend( MSG_S2C_RGN_ADDSHAPE,pGoods->GetExID());
//								MsgSend.Add( pGoods->GetType() );
//								MsgSend.Add( pGoods->GetExID() );
//								DBWriteSet setWriteDB;
//								MsgSend.GetDBWriteSet(setWriteDB);
//								pGoods->SerializeForRgnDisplay(setWriteDB);
//
//								MsgSend.SendToAround(pGoods);
//
//								lDropGoodsStartPos ++;
//#ifdef _GOODSLOG1_
//								char pszGoodsLog[1024]="";
//								char playerGUID[50]="";
//								GetExID().tostring(playerGUID);
//								char goodsGUID[50]="";
//								pGoods->GetExID().tostring(goodsGUID);
//								_snprintf(pszGoodsLog,1024,"[操作:玩家死亡掉装备][玩家名:%s][玩家GUID:%s][场景ID:%d][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d][X坐标:%d][Y坐标:%d]",
//									GetName(),playerGUID,pRegion->GetID(),pGoods->GetBasePropertiesIndex(),GoodsSetup::QueryGoodsName(pGoods->GetBasePropertiesIndex()),
//									goodsGUID,pGoods->GetAmount(),GetTileX(),GetTileY());
//								CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
//#endif
//
//								GetLogicLogInterface()->logT050_goods_log_fall(this,pGoods);
//							}
//							}
//						}
//					}					
//				}
//
//				//LRING
//				if(m_cEquipment.GetGoods(CEquipmentContainer::EC_LRING) && random(10000) < DiedLost.fDropEquip_LRing*10000)
//				{
//					pRegion -> GetDropGoodsPos( GetTileX(), GetTileY(), lX, lY, lDropGoodsStartPos );
//					if( lDropGoodsStartPos != -1 )
//
//					{
//						CGoods* pGoods = dynamic_cast<CGoods*>( m_cEquipment.GetGoods(CEquipmentContainer::EC_LRING) );
//						if( pGoods && ( pGoods -> GetAddonPropertyValues(CGoodsBaseProperties::GAP_PARTICULAR_ATTRIBUTE, 1) &
//							CGoodsBaseProperties::PA_NEVER_DROP_DOWN ) != CGoodsBaseProperties::PA_NEVER_DROP_DOWN )
//						{
//							if (!GoodsExManage::GetInstance().IsFrost(pGoods) && !GoodsExManage::GetInstance().IsBind(pGoods)) //! 如果冻结
//							{
//								
//							CS2CContainerObjectMove comMsg;
//							pGoods = dynamic_cast<CGoods*>( m_cEquipment.Remove( static_cast<DWORD>(CEquipmentContainer::EC_LRING), static_cast<DWORD>(1), &comMsg) );
//							if( pGoods )
//							{
//								pGoods -> SetPosXY( lX+0.5f, lY+0.5f );
//								pRegion -> AddObject( pGoods );
//
//								//##发消息给自己
//								comMsg.SetSourceContainerExtendID( CS2CContainerObjectMove::PEI_EQUIPMENT );
//								comMsg.SetOperation( CS2CContainerObjectMove::OT_MOVE_OBJECT );
//								comMsg.SetDestinationContainer( pRegion -> GetType(), pRegion -> GetExID(), 
//									lY * pRegion -> GetWidth() + lX );
//								comMsg.SetDestinationContainerExtendID( 0 );
//								comMsg.SetDestinationObject( pGoods -> GetType(), pGoods -> GetExID() );
//								comMsg.SetDestinationObjectAmount( pGoods -> GetAmount() );
//								comMsg.Send( GetExID() );
//
//								//##给周围玩家发送消息
//								CMessage MsgSend( MSG_S2C_RGN_ADDSHAPE,pGoods->GetExID());
//								MsgSend.Add( pGoods->GetType() );
//								MsgSend.Add( pGoods->GetExID() );
//								DBWriteSet setWriteDB;
//								MsgSend.GetDBWriteSet(setWriteDB);
//								pGoods->SerializeForRgnDisplay(setWriteDB);
//
//								MsgSend.SendToAround(pGoods);
//
//								lDropGoodsStartPos ++;
//#ifdef _GOODSLOG1_
//								char pszGoodsLog[1024]="";
//								char playerGUID[50]="";
//								GetExID().tostring(playerGUID);
//								char goodsGUID[50]="";
//								pGoods->GetExID().tostring(goodsGUID);
//								_snprintf(pszGoodsLog,1024,"[操作:玩家死亡掉装备][玩家名:%s][玩家GUID:%s][场景ID:%d][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d][X坐标:%d][Y坐标:%d]",
//									GetName(),playerGUID,pRegion->GetID(),pGoods->GetBasePropertiesIndex(),GoodsSetup::QueryGoodsName(pGoods->GetBasePropertiesIndex()),
//									goodsGUID,pGoods->GetAmount(),GetTileX(),GetTileY());
//								CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
//#endif
//
//								GetLogicLogInterface()->logT050_goods_log_fall(this,pGoods);
//							}
//							}
//						}
//					}					
//				}
//
//				//RRING
//				if(m_cEquipment.GetGoods(CEquipmentContainer::EC_RRING) && random(10000) < DiedLost.fDropEquip_RRing*10000)
//				{
//					pRegion -> GetDropGoodsPos( GetTileX(), GetTileY(), lX, lY, lDropGoodsStartPos );
//					if( lDropGoodsStartPos != -1 )
//
//					{
//						CGoods* pGoods = dynamic_cast<CGoods*>( m_cEquipment.GetGoods(CEquipmentContainer::EC_RRING) );
//						if( pGoods && ( pGoods -> GetAddonPropertyValues(CGoodsBaseProperties::GAP_PARTICULAR_ATTRIBUTE, 1) &
//							CGoodsBaseProperties::PA_NEVER_DROP_DOWN ) != CGoodsBaseProperties::PA_NEVER_DROP_DOWN )
//						{
//							if (!GoodsExManage::GetInstance().IsFrost(pGoods) && !GoodsExManage::GetInstance().IsBind(pGoods)) //! 如果冻结
//							{
//								
//							CS2CContainerObjectMove comMsg;
//							pGoods = dynamic_cast<CGoods*>( m_cEquipment.Remove( static_cast<DWORD>(CEquipmentContainer::EC_RRING), static_cast<DWORD>(1), &comMsg) );
//							if( pGoods )
//							{
//								pGoods -> SetPosXY( lX+0.5f, lY+0.5f );
//								pRegion -> AddObject( pGoods );
//
//								//##发消息给自己
//								comMsg.SetSourceContainerExtendID( CS2CContainerObjectMove::PEI_EQUIPMENT );
//								comMsg.SetOperation( CS2CContainerObjectMove::OT_MOVE_OBJECT );
//								comMsg.SetDestinationContainer( pRegion -> GetType(), pRegion -> GetExID(), 
//									lY * pRegion -> GetWidth() + lX );
//								comMsg.SetDestinationContainerExtendID( 0 );
//								comMsg.SetDestinationObject( pGoods -> GetType(), pGoods -> GetExID() );
//								comMsg.SetDestinationObjectAmount( pGoods -> GetAmount() );
//								comMsg.Send( GetExID() );
//
//								//##给周围玩家发送消息
//								CMessage MsgSend( MSG_S2C_RGN_ADDSHAPE,pGoods->GetExID());
//								MsgSend.Add( pGoods->GetType() );
//								MsgSend.Add( pGoods->GetExID() );
//								DBWriteSet setWriteDB;
//								MsgSend.GetDBWriteSet(setWriteDB);
//								pGoods->SerializeForRgnDisplay(setWriteDB);
//
//								MsgSend.SendToAround(pGoods);
//
//								lDropGoodsStartPos ++;
//#ifdef _GOODSLOG1_
//								char pszGoodsLog[1024]="";
//								char playerGUID[50]="";
//								GetExID().tostring(playerGUID);
//								char goodsGUID[50]="";
//								pGoods->GetExID().tostring(goodsGUID);
//								_snprintf(pszGoodsLog,1024,"[操作:玩家死亡掉装备][玩家名:%s][玩家GUID:%s][场景ID:%d][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d][X坐标:%d][Y坐标:%d]",
//									GetName(),playerGUID,pRegion->GetID(),pGoods->GetBasePropertiesIndex(),GoodsSetup::QueryGoodsName(pGoods->GetBasePropertiesIndex()),
//									goodsGUID,pGoods->GetAmount(),GetTileX(),GetTileY());
//								CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
//#endif
//
//								GetLogicLogInterface()->logT050_goods_log_fall(this,pGoods);
//							}
//							}
//						}
//					}					
//				}
//
//				//精灵
//				if(m_cEquipment.GetGoods(CEquipmentContainer::EC_FAIRY) && random(10000) < DiedLost.fDropEquip_Fairy*10000)
//				{
//					pRegion -> GetDropGoodsPos( GetTileX(), GetTileY(), lX, lY, lDropGoodsStartPos );
//					if( lDropGoodsStartPos != -1 )
//
//					{
//						CGoods* pGoods = dynamic_cast<CGoods*>( m_cEquipment.GetGoods(CEquipmentContainer::EC_FAIRY) );
//						if( pGoods && ( pGoods -> GetAddonPropertyValues(CGoodsBaseProperties::GAP_PARTICULAR_ATTRIBUTE, 1) &
//							CGoodsBaseProperties::PA_NEVER_DROP_DOWN ) != CGoodsBaseProperties::PA_NEVER_DROP_DOWN )
//						{
//							if (!GoodsExManage::GetInstance().IsFrost(pGoods) && !GoodsExManage::GetInstance().IsBind(pGoods)) //! 如果冻结
//							{
//								
//							CS2CContainerObjectMove comMsg;
//							pGoods = dynamic_cast<CGoods*>( m_cEquipment.Remove( static_cast<DWORD>(CEquipmentContainer::EC_FAIRY), static_cast<DWORD>(1), &comMsg) );
//							if( pGoods )
//							{
//								pGoods -> SetPosXY( lX+0.5f, lY+0.5f );
//								pRegion -> AddObject( pGoods );
//
//								//##发消息给自己
//								comMsg.SetSourceContainerExtendID( CS2CContainerObjectMove::PEI_EQUIPMENT );
//								comMsg.SetOperation( CS2CContainerObjectMove::OT_MOVE_OBJECT );
//								comMsg.SetDestinationContainer( pRegion -> GetType(), pRegion -> GetExID(), 
//									lY * pRegion -> GetWidth() + lX );
//								comMsg.SetDestinationContainerExtendID( 0 );
//								comMsg.SetDestinationObject( pGoods -> GetType(), pGoods -> GetExID() );
//								comMsg.SetDestinationObjectAmount( pGoods -> GetAmount() );
//								comMsg.Send( GetExID() );
//
//								//##给周围玩家发送消息
//								CMessage MsgSend( MSG_S2C_RGN_ADDSHAPE,pGoods->GetExID());
//								MsgSend.Add( pGoods->GetType() );
//								MsgSend.Add( pGoods->GetExID() );
//								DBWriteSet setWriteDB;
//								MsgSend.GetDBWriteSet(setWriteDB);
//								pGoods->SerializeForRgnDisplay(setWriteDB);
//
//								MsgSend.SendToAround(pGoods);
//
//								lDropGoodsStartPos ++;
//#ifdef _GOODSLOG1_
//								char pszGoodsLog[1024]="";
//								char playerGUID[50]="";
//								GetExID().tostring(playerGUID);
//								char goodsGUID[50]="";
//								pGoods->GetExID().tostring(goodsGUID);
//								_snprintf(pszGoodsLog,1024,"[操作:玩家死亡掉装备][玩家名:%s][玩家GUID:%s][场景ID:%d][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d][X坐标:%d][Y坐标:%d]",
//									GetName(),playerGUID,pRegion->GetID(),pGoods->GetBasePropertiesIndex(),GoodsSetup::QueryGoodsName(pGoods->GetBasePropertiesIndex()),
//									goodsGUID,pGoods->GetAmount(),GetTileX(),GetTileY());
//								CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
//#endif
//
//								GetLogicLogInterface()->logT050_goods_log_fall(this,pGoods);
//							}
//							}
//						}
//					}					
//				}
//
//				// 武器
//				if (m_cEquipment.GetGoods(CEquipmentContainer::EC_WEAPON) && random(10000) < DiedLost.fDropEquip_Weapon*10000)
//				{
//					pRegion -> GetDropGoodsPos( GetTileX(), GetTileY(), lX, lY, lDropGoodsStartPos );
//					if( lDropGoodsStartPos != -1 )
//
//					{
//						CGoods* pGoods = dynamic_cast<CGoods*>( m_cEquipment.GetGoods(CEquipmentContainer::EC_WEAPON) );
//						if( pGoods && ( pGoods -> GetAddonPropertyValues(CGoodsBaseProperties::GAP_PARTICULAR_ATTRIBUTE, 1) &
//							CGoodsBaseProperties::PA_NEVER_DROP_DOWN ) != CGoodsBaseProperties::PA_NEVER_DROP_DOWN )
//						{
//							if (!GoodsExManage::GetInstance().IsFrost(pGoods) && !GoodsExManage::GetInstance().IsBind(pGoods)) //! 如果冻结
//							{
//								
//							CS2CContainerObjectMove comMsg;
//							pGoods = dynamic_cast<CGoods*>( m_cEquipment.Remove( static_cast<DWORD>(CEquipmentContainer::EC_WEAPON), static_cast<DWORD>(1), &comMsg) );
//							if( pGoods )
//							{
//								pGoods -> SetPosXY( lX+0.5f, lY+0.5f );
//								pRegion -> AddObject( pGoods );
//
//								//##发消息给自己
//								comMsg.SetSourceContainerExtendID( CS2CContainerObjectMove::PEI_EQUIPMENT );
//								comMsg.SetOperation( CS2CContainerObjectMove::OT_MOVE_OBJECT );
//								comMsg.SetDestinationContainer( pRegion -> GetType(), pRegion -> GetExID(), 
//									lY * pRegion -> GetWidth() + lX );
//								comMsg.SetDestinationContainerExtendID( 0 );
//								comMsg.SetDestinationObject( pGoods -> GetType(), pGoods -> GetExID() );
//								comMsg.SetDestinationObjectAmount( pGoods -> GetAmount() );
//								comMsg.Send( GetExID() );
//
//								//##给周围玩家发送消息
//								CMessage MsgSend( MSG_S2C_RGN_ADDSHAPE,pGoods->GetExID());
//								MsgSend.Add( pGoods->GetType() );
//								MsgSend.Add( pGoods->GetExID() );
//								DBWriteSet setWriteDB;
//								MsgSend.GetDBWriteSet(setWriteDB);
//								pGoods->SerializeForRgnDisplay(setWriteDB);
//
//								MsgSend.SendToAround(pGoods);
//
//								lDropGoodsStartPos ++;
//#ifdef _GOODSLOG1_
//								char pszGoodsLog[1024]="";
//								char playerGUID[50]="";
//								GetExID().tostring(playerGUID);
//								char goodsGUID[50]="";
//								pGoods->GetExID().tostring(goodsGUID);
//								_snprintf(pszGoodsLog,1024,"[操作:玩家死亡掉装备][玩家名:%s][玩家GUID:%s][场景ID:%d][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d][X坐标:%d][Y坐标:%d]",
//									GetName(),playerGUID,pRegion->GetID(),pGoods->GetBasePropertiesIndex(),GoodsSetup::QueryGoodsName(pGoods->GetBasePropertiesIndex()),
//									goodsGUID,pGoods->GetAmount(),GetTileX(),GetTileY());
//								CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
//#endif
//
//								GetLogicLogInterface()->logT050_goods_log_fall(this,pGoods);
//							}
//							}
//						}
//					}					
//				}
//
//				//副手武器
//				if(m_cEquipment.GetGoods(CEquipmentContainer::EC_WEAPON2) && random(10000) < DiedLost.fDropEquip_Weapon2*10000)
//				{
//					pRegion -> GetDropGoodsPos( GetTileX(), GetTileY(), lX, lY, lDropGoodsStartPos );
//					if( lDropGoodsStartPos != -1 )
//
//					{
//						CGoods* pGoods = dynamic_cast<CGoods*>( m_cEquipment.GetGoods(CEquipmentContainer::EC_WEAPON2) );
//						if( pGoods && ( pGoods -> GetAddonPropertyValues(CGoodsBaseProperties::GAP_PARTICULAR_ATTRIBUTE, 1) &
//							CGoodsBaseProperties::PA_NEVER_DROP_DOWN ) != CGoodsBaseProperties::PA_NEVER_DROP_DOWN )
//						{
//							if (!GoodsExManage::GetInstance().IsFrost(pGoods) && !GoodsExManage::GetInstance().IsBind(pGoods)) //! 如果冻结
//							{
//								
//							CS2CContainerObjectMove comMsg;
//							pGoods = dynamic_cast<CGoods*>( m_cEquipment.Remove( static_cast<DWORD>(CEquipmentContainer::EC_WEAPON2), static_cast<DWORD>(1), &comMsg) );
//							if( pGoods )
//							{
//								pGoods -> SetPosXY( lX+0.5f, lY+0.5f );
//								pRegion -> AddObject( pGoods );
//
//								//##发消息给自己
//								comMsg.SetSourceContainerExtendID( CS2CContainerObjectMove::PEI_EQUIPMENT );
//								comMsg.SetOperation( CS2CContainerObjectMove::OT_MOVE_OBJECT );
//								comMsg.SetDestinationContainer( pRegion -> GetType(), pRegion -> GetExID(), 
//									lY * pRegion -> GetWidth() + lX );
//								comMsg.SetDestinationContainerExtendID( 0 );
//								comMsg.SetDestinationObject( pGoods -> GetType(), pGoods -> GetExID() );
//								comMsg.SetDestinationObjectAmount( pGoods -> GetAmount() );
//								comMsg.Send( GetExID() );
//
//								//##给周围玩家发送消息
//								CMessage MsgSend( MSG_S2C_RGN_ADDSHAPE,pGoods->GetExID());
//								MsgSend.Add( pGoods->GetType() );
//								MsgSend.Add( pGoods->GetExID() );
//								DBWriteSet setWriteDB;
//								MsgSend.GetDBWriteSet(setWriteDB);
//								pGoods->SerializeForRgnDisplay(setWriteDB);
//
//								MsgSend.SendToAround(pGoods);
//								lDropGoodsStartPos ++;
//#ifdef _GOODSLOG1_
//								char pszGoodsLog[1024]="";
//								char playerGUID[50]="";
//								GetExID().tostring(playerGUID);
//								char goodsGUID[50]="";
//								pGoods->GetExID().tostring(goodsGUID);
//								_snprintf(pszGoodsLog,1024,"[操作:玩家死亡掉装备][玩家名:%s][玩家GUID:%s][场景ID:%d][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d][X坐标:%d][Y坐标:%d]",
//									GetName(),playerGUID,pRegion->GetID(),pGoods->GetBasePropertiesIndex(),GoodsSetup::QueryGoodsName(pGoods->GetBasePropertiesIndex()),
//									goodsGUID,pGoods->GetAmount(),GetTileX(),GetTileY());
//								CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
//#endif
//
//								GetLogicLogInterface()->logT050_goods_log_fall(this,pGoods);
//							}
//							}
//						}
//					}					
//				}
//
//				//披风
//				if(m_cEquipment.GetGoods(CEquipmentContainer::EC_CLOAK) && random(10000) < DiedLost.fDropEquip_Cloak*10000)
//				{
//					pRegion -> GetDropGoodsPos( GetTileX(), GetTileY(), lX, lY, lDropGoodsStartPos );
//					if( lDropGoodsStartPos != -1 )
//
//					{
//						CGoods* pGoods = dynamic_cast<CGoods*>( m_cEquipment.GetGoods(CEquipmentContainer::EC_CLOAK) );
//						if( pGoods && ( pGoods -> GetAddonPropertyValues(CGoodsBaseProperties::GAP_PARTICULAR_ATTRIBUTE, 1) &
//							CGoodsBaseProperties::PA_NEVER_DROP_DOWN ) != CGoodsBaseProperties::PA_NEVER_DROP_DOWN )
//						{
//							if (!GoodsExManage::GetInstance().IsFrost(pGoods) && !GoodsExManage::GetInstance().IsBind(pGoods)) //! 如果冻结
//							{
//								
//							CS2CContainerObjectMove comMsg;
//							pGoods = dynamic_cast<CGoods*>( m_cEquipment.Remove( static_cast<DWORD>(CEquipmentContainer::EC_CLOAK), static_cast<DWORD>(1), &comMsg) );
//							if( pGoods )
//							{
//								pGoods -> SetPosXY( lX+0.5f, lY+0.5f );
//								pRegion -> AddObject( pGoods );
//
//								//##发消息给自己
//								comMsg.SetSourceContainerExtendID( CS2CContainerObjectMove::PEI_EQUIPMENT );
//								comMsg.SetOperation( CS2CContainerObjectMove::OT_MOVE_OBJECT );
//								comMsg.SetDestinationContainer( pRegion -> GetType(), pRegion -> GetExID(), 
//									lY * pRegion -> GetWidth() + lX );
//								comMsg.SetDestinationContainerExtendID( 0 );
//								comMsg.SetDestinationObject( pGoods -> GetType(), pGoods -> GetExID() );
//								comMsg.SetDestinationObjectAmount( pGoods -> GetAmount() );
//								comMsg.Send( GetExID() );
//
//								//##给周围玩家发送消息
//								CMessage MsgSend( MSG_S2C_RGN_ADDSHAPE,pGoods->GetExID());
//								MsgSend.Add( pGoods->GetType() );
//								MsgSend.Add( pGoods->GetExID() );
//								DBWriteSet setWriteDB;
//								MsgSend.GetDBWriteSet(setWriteDB);
//								pGoods->SerializeForRgnDisplay(setWriteDB);
//
//								MsgSend.SendToAround(pGoods);
//
//								lDropGoodsStartPos ++;
//#ifdef _GOODSLOG1_
//								char pszGoodsLog[1024]="";
//								char playerGUID[50]="";
//								GetExID().tostring(playerGUID);
//								char goodsGUID[50]="";
//								pGoods->GetExID().tostring(goodsGUID);
//								_snprintf(pszGoodsLog,1024,"[操作:玩家死亡掉装备][玩家名:%s][玩家GUID:%s][场景ID:%d][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d][X坐标:%d][Y坐标:%d]",
//									GetName(),playerGUID,pRegion->GetID(),pGoods->GetBasePropertiesIndex(),GoodsSetup::QueryGoodsName(pGoods->GetBasePropertiesIndex()),
//									goodsGUID,pGoods->GetAmount(),GetTileX(),GetTileY());
//								CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
//#endif
//
//								GetLogicLogInterface()->logT050_goods_log_fall(this,pGoods);
//							}
//							}
//						}
//					}					
//				}	
//
//
//				//坐骑
//				if(m_cEquipment.GetGoods(CEquipmentContainer::EC_HORSE) && random(10000) < DiedLost.fDropEquip_Horse*10000)
//				{
//					pRegion -> GetDropGoodsPos( GetTileX(), GetTileY(), lX, lY, lDropGoodsStartPos );
//					if( lDropGoodsStartPos != -1 )
//
//					{
//						CGoods* pGoods = dynamic_cast<CGoods*>( m_cEquipment.GetGoods(CEquipmentContainer::EC_HORSE) );
//						if( pGoods && ( pGoods -> GetAddonPropertyValues(CGoodsBaseProperties::GAP_PARTICULAR_ATTRIBUTE, 1) &
//							CGoodsBaseProperties::PA_NEVER_DROP_DOWN ) != CGoodsBaseProperties::PA_NEVER_DROP_DOWN )
//						{
//							if (!GoodsExManage::GetInstance().IsFrost(pGoods) && !GoodsExManage::GetInstance().IsBind(pGoods)) //! 如果冻结
//							{
//								
//							CS2CContainerObjectMove comMsg;
//							pGoods = dynamic_cast<CGoods*>( m_cEquipment.Remove( static_cast<DWORD>(CEquipmentContainer::EC_HORSE), static_cast<DWORD>(1), &comMsg) );
//							if( pGoods )
//							{
//								pGoods -> SetPosXY( lX+0.5f, lY+0.5f );
//								pRegion -> AddObject( pGoods );
//
//								//##发消息给自己
//								comMsg.SetSourceContainerExtendID( CS2CContainerObjectMove::PEI_EQUIPMENT );
//								comMsg.SetOperation( CS2CContainerObjectMove::OT_MOVE_OBJECT );
//								comMsg.SetDestinationContainer( pRegion -> GetType(), pRegion -> GetExID(), 
//									lY * pRegion -> GetWidth() + lX );
//								comMsg.SetDestinationContainerExtendID( 0 );
//								comMsg.SetDestinationObject( pGoods -> GetType(), pGoods -> GetExID() );
//								comMsg.SetDestinationObjectAmount( pGoods -> GetAmount() );
//								comMsg.Send( GetExID() );
//
//								//##给周围玩家发送消息
//								CMessage MsgSend( MSG_S2C_RGN_ADDSHAPE,pGoods->GetExID());
//								MsgSend.Add( pGoods->GetType() );
//								MsgSend.Add( pGoods->GetExID() );
//								DBWriteSet setWriteDB;
//								MsgSend.GetDBWriteSet(setWriteDB);
//								pGoods->SerializeForRgnDisplay(setWriteDB);
//
//								MsgSend.SendToAround(pGoods);
//
//								lDropGoodsStartPos ++;
//#ifdef _GOODSLOG1_
//								char pszGoodsLog[1024]="";
//								char playerGUID[50]="";
//								GetExID().tostring(playerGUID);
//								char goodsGUID[50]="";
//								pGoods->GetExID().tostring(goodsGUID);
//								_snprintf(pszGoodsLog,1024,"[操作:玩家死亡掉装备][玩家名:%s][玩家GUID:%s][场景ID:%d][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d][X坐标:%d][Y坐标:%d]",
//									GetName(),playerGUID,pRegion->GetID(),pGoods->GetBasePropertiesIndex(),GoodsSetup::QueryGoodsName(pGoods->GetBasePropertiesIndex()),
//									goodsGUID,pGoods->GetAmount(),GetTileX(),GetTileY());
//								CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
//#endif
//
//								GetLogicLogInterface()->logT050_goods_log_fall(this,pGoods);
//							}
//							}
//						}
//					}					
//				}	
//
//
//			}
//			else
//			{
//				// 新兵不会凋落装备
//			}
//			//------------------
//			// 背包区域
//			//------------------
//			//##背包
//			CGoodsListListener lGoodsList;
//			getOriginPack()->TraversingContainer( &lGoodsList );
//			for( size_t i = 0; i < lGoodsList.m_vGoodsID.size(); i ++ )
//			{
//				if( random(10000) < DiedLost.fDropEquip_Bag * 10000 )
//				{
//					pRegion -> GetDropGoodsPos( GetTileX(), GetTileY(), lX, lY, lDropGoodsStartPos );
//					if( lDropGoodsStartPos != -1 )
//					{
//						CGoods* pGoods = dynamic_cast<CGoods*>( getOriginPack()->Find(TYPE_GOODS, lGoodsList.m_vGoodsID[i]) );
//
//						if(pGoods)
//						{
//							if (GoodsExManage::GetInstance().IsFrost(pGoods) || GoodsExManage::GetInstance().IsBind(pGoods)) //! 如果冻结
//							{
//								continue;
//							}
//						}
//
//						if( pGoods && ( pGoods -> GetAddonPropertyValues(CGoodsBaseProperties::GAP_PARTICULAR_ATTRIBUTE, 1) &
//							CGoodsBaseProperties::PA_NEVER_DROP_DOWN ) == CGoodsBaseProperties::PA_NEVER_DROP_DOWN )
//						{
//							continue;
//						}
//
//						CS2CContainerObjectMove comMsg;
//						pGoods = dynamic_cast<CGoods*>( getOriginPack()->Remove(TYPE_GOODS, lGoodsList.m_vGoodsID[i], &comMsg) );
//						if( pGoods )
//						{
//							pGoods -> SetPosXY( lX+0.5f, lY+0.5f );
//							pRegion -> AddObject( pGoods );
//
//							//##给自己发消息
//							comMsg.SetSourceContainerExtendID( CS2CContainerObjectMove::PEI_PACKET );
//							comMsg.SetOperation( CS2CContainerObjectMove::OT_MOVE_OBJECT );
//							comMsg.SetDestinationContainer( pRegion -> GetType(), pRegion -> GetExID(), 
//								lY * pRegion -> GetWidth() + lX );
//							comMsg.SetDestinationContainerExtendID( 0 );
//							comMsg.SetDestinationObject( pGoods -> GetType(), pGoods -> GetExID() );
//							comMsg.SetDestinationObjectAmount( pGoods -> GetAmount() );
//							comMsg.Send( GetExID() );
//
//							//##给周围玩家发送消息
//							CMessage MsgSend( MSG_S2C_RGN_ADDSHAPE,pGoods->GetExID());
//							MsgSend.Add( pGoods->GetType() );
//							MsgSend.Add( pGoods->GetExID() );
//							DBWriteSet setWriteDB;
//							MsgSend.GetDBWriteSet(setWriteDB);
//							pGoods->SerializeForRgnDisplay(setWriteDB);
//
//							MsgSend.SendToAround(pGoods);
//
//							lDropGoodsStartPos ++;
//#ifdef _GOODSLOG1_
//							char pszGoodsLog[1024]="";
//							char playerGUID[50]="";
//							GetExID().tostring(playerGUID);
//							char goodsGUID[50]="";
//							pGoods->GetExID().tostring(goodsGUID);
//							_snprintf(pszGoodsLog,1024,"[操作:玩家死亡掉背包物品][玩家名:%s][玩家GUID:%s][背包ID:%d][场景ID:%d][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d][X坐标:%d][Y坐标:%d]",
//								GetName(),playerGUID,1,pRegion->GetID(),pGoods->GetBasePropertiesIndex(),GoodsSetup::QueryGoodsName(pGoods->GetBasePropertiesIndex()),
//								goodsGUID,pGoods->GetAmount(),GetTileX(),GetTileY());
//							CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
//#endif
//
//							GetLogicLogInterface()->logT050_goods_log_fall(this,pGoods);
//						}
//					}
//				}//if end
//			}
//
//
//			for(int i=CS2CContainerObjectMove::PEI_PACK1;i<=CS2CContainerObjectMove::PEI_PACK5;i++)
//			{
//				if(GetPackage()->GetSubpack(i-CS2CContainerObjectMove::PEI_PACK1)->pGoods)
//				{		
//					CVolumeLimitGoodsContainer* pSubContainer=GetPackage()->GetSubpack(i-CS2CContainerObjectMove::PEI_PACK1)->pSubpackContainer;
//					lGoodsList.m_vGoodsID.clear();
//					pSubContainer->TraversingContainer(&lGoodsList);
//					for(size_t j=0;j<lGoodsList.m_vGoodsID.size();j++)
//					{
//						if( random(10000) < DiedLost.fDropEquip_Bag * 10000 )
//						{
//							pRegion -> GetDropGoodsPos( GetTileX(), GetTileY(), lX, lY, lDropGoodsStartPos );
//							if( lDropGoodsStartPos != -1 )
//							{
//								CGoods* pGoods = dynamic_cast<CGoods*>( pSubContainer->Find(TYPE_GOODS, lGoodsList.m_vGoodsID[j]) );
//
//								if(pGoods)
//								{
//									if (GoodsExManage::GetInstance().IsFrost(pGoods) || GoodsExManage::GetInstance().IsBind(pGoods)) //! 如果冻结
//									{
//										continue;
//									}
//								}
//
//								if( pGoods && ( pGoods -> GetAddonPropertyValues(CGoodsBaseProperties::GAP_PARTICULAR_ATTRIBUTE, 1) &
//									CGoodsBaseProperties::PA_NEVER_DROP_DOWN ) == CGoodsBaseProperties::PA_NEVER_DROP_DOWN )
//								{
//									continue;
//								}
//
//								CS2CContainerObjectMove comMsg;
//								pGoods = dynamic_cast<CGoods*>( pSubContainer->Remove(TYPE_GOODS, lGoodsList.m_vGoodsID[j], &comMsg) );
//								if( pGoods )
//								{
//									pGoods -> SetPosXY( lX+0.5f, lY+0.5f );
//									pRegion -> AddObject( pGoods );
//
//									//##给自己发消息
//									comMsg.SetSourceContainerExtendID( i );
//									comMsg.SetOperation( CS2CContainerObjectMove::OT_MOVE_OBJECT );
//									comMsg.SetDestinationContainer( pRegion -> GetType(), pRegion -> GetExID(), 
//										lY * pRegion -> GetWidth() + lX );
//									comMsg.SetDestinationContainerExtendID( 0 );
//									comMsg.SetDestinationObject( pGoods -> GetType(), pGoods -> GetExID() );
//									comMsg.SetDestinationObjectAmount( pGoods -> GetAmount() );
//									comMsg.Send( GetExID() );
//
//									//##给周围玩家发送消息
//									CMessage MsgSend( MSG_S2C_RGN_ADDSHAPE,pGoods->GetExID());
//									MsgSend.Add( pGoods->GetType() );
//									MsgSend.Add( pGoods->GetExID() );
//									DBWriteSet setWriteDB;
//									MsgSend.GetDBWriteSet(setWriteDB);
//									pGoods->SerializeForRgnDisplay(setWriteDB);
//
//									MsgSend.SendToAround(pGoods);
//
//									lDropGoodsStartPos ++;
//#ifdef _GOODSLOG1_
//									char pszGoodsLog[1024]="";
//									char playerGUID[50]="";
//									GetExID().tostring(playerGUID);
//									char goodsGUID[50]="";
//									pGoods->GetExID().tostring(goodsGUID);
//									_snprintf(pszGoodsLog,1024,"[操作:玩家死亡掉背包物品][玩家名:%s][玩家GUID:%s][背包ID:%d][场景ID:%d][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d][X坐标:%d][Y坐标:%d]",
//										GetName(),playerGUID,i,pRegion->GetID(),pGoods->GetBasePropertiesIndex(),GoodsSetup::QueryGoodsName(pGoods->GetBasePropertiesIndex()),
//										goodsGUID,pGoods->GetAmount(),GetTileX(),GetTileY());
//									CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
//#endif
//
//									GetLogicLogInterface()->logT050_goods_log_fall(this,pGoods);
//								}
//							}
//						}//if end
//					}	
//				}
//			}	
//
//
//			//------------------
//			// 其他区域
//			//------------------
//			// 金钱
//			if (GetMoney() && random(10000) < DiedLost.fDropEquip_Money*10000)
//			{
//				DWORD dwDropMoney = static_cast<DWORD>( GetMoney() * DiedLost.fDropEquip_MoneyPercent );
//				if( dwDropMoney>0 && GetMoney() - dwDropMoney >= 0 )
//				{
//					lDropGoodsStartPos = pRegion->GetDropGoodsPos(GetTileX(), GetTileY(), lX, lY, lDropGoodsStartPos);
//					if( lDropGoodsStartPos != -1 )
//					{
//						//extern bool _AddMoneyToRegion(CRegion* p_region, long dx, long dy, DWORD dw_money, long lProtectPlayerID=0, long lProtectTeamID=0);
//						//_AddMoneyToRegion(pRegion, lX, lY, dwDropMoney);
//						SetMoney(GetMoney()-dwDropMoney,7);
//						CGoods* pGoldCoins = CGoodsFactory::CreateGoods( GoodsSetup::GetGoldCoinIndex(),8 );
//						if( pGoldCoins )
//						{
//							pGoldCoins -> SetAmount( dwDropMoney );
//							pGoldCoins -> SetPosXY( lX+0.5f, lY+0.5f );
//							pRegion -> AddObject( pGoldCoins );
//
//							CMessage MsgSend( MSG_S2C_RGN_ADDSHAPE,pGoldCoins->GetExID());
//							MsgSend.Add( pGoldCoins->GetType() );
//							MsgSend.Add( pGoldCoins->GetExID() );
//							DBWriteSet setWriteDB;
//							MsgSend.GetDBWriteSet(setWriteDB);
//							pGoldCoins->SerializeForRgnDisplay(setWriteDB);
//
//							MsgSend.SendToAround(pGoldCoins);
//#ifdef _GOODSLOG1_
//							char pszGoodsLog[1024]="";
//							char playerGUID[50]="";
//							GetExID().tostring(playerGUID);
//							char goodsGUID[50]="";
//							pGoldCoins->GetExID().tostring(goodsGUID);
//							_snprintf(pszGoodsLog,1024,"[操作:玩家死亡掉金币][玩家名:%s][玩家GUID:%s][场景ID:%d][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d][X坐标:%d][Y坐标:%d]",
//								GetName(),playerGUID,pRegion->GetID(),pGoldCoins->GetBasePropertiesIndex(),GoodsSetup::QueryGoodsName(pGoldCoins->GetBasePropertiesIndex()),
//								goodsGUID,pGoldCoins->GetAmount(),GetTileX(),GetTileY());
//							CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
//#endif
//
//							GetLogicLogInterface()->logT050_goods_log_fall(this,pGoldCoins);
//						}
//					}			
//				}
//			}
//		}
//	}
//	else { //
//		const char* strRes=AppFrame::GetText("8");
//		SendNotifyMessage( strRes );
//
//	}

	//死亡注册回城复活定时器
	RegisterReliveTimerID();

	//死亡时触发脚本
	const char* strScript = GlobalSetup::GetSetup()->pszPlayerDiedScript;
	if(strScript)
	{
		//stRunScript sScript;
		//sScript.pszFileName = (char*)strScript;
		//sScript.desShape = NULL;
		//sScript.pRegion =  dynamic_cast<CRegion*>(GetFather());
		//sScript.srcShape	= this;
		//RunScript( &sScript, const_cast<CHAR*>( GetGame() -> GetScriptFileData(strScript) ) );
		Script::RunEnv env( this, NULL, static_cast<CRegion*>( GetFather() ) );
		GetInst( ScriptSys ).RunScript( env, strScript );
	}

	char szMsg[1024]="";
	CRegion* pLocalRgn = dynamic_cast<CRegion*>(GetFather());
	// 报告凶手
	switch(GetKilledMeType())
	{
	case TYPE_PLAYER:
		{
			CPlayer *pMudder = GetGame()->FindPlayer(GetKilledMeID());
			if (pMudder)
			{
				if( pMudder->GetHideFlag() == 1 && pLocalRgn && pLocalRgn->GetRgnHideFlag() != 2
					&& (GetTeamID()!=pMudder->GetTeamID() || GetTeamID()==NULL_GUID || pMudder->GetTeamID()==NULL_GUID) )
					_snprintf(szMsg, 1024, "[Colour 'ffff0000']你被玩家<%s>杀死了。", GlobalSetup::GetSetup()->szHideName);
				else
					_snprintf(szMsg, 1024, "你被玩家<%s>杀死了。", pMudder->GetName());
				//SendNotifyMessage(szMsg, 0xffff0000);
				SendNotifyMessage(szMsg);


				// 通知
				// 允许通知 不同国家 非国战时间 在非竞技区
				if( pRegion->GetNotify() &&
					GetCountry() != pMudder->GetCountry() &&
					pRegion->GetSecurity(GetTileX(), GetTileY()) != CRegion::SECURTIY_FIGHT )
				{
					char szInfo[1024];
					if( pRegion->GetLastNotifyKillTime()+pRegion->GetNotify() < timeGetTime() )
					{
						pRegion->SetLastNotifyKillTime( timeGetTime() );

						// 通知他国在线
						// [玩家名]在[场景]成功击杀了敌人！
						if( pMudder->GetFactionID() != NULL_GUID)
						{
							//							_snprintf(szInfo, "<%s>帮会 %s玩家在<%s>成功击杀敌国人。", pMudder->GetFactionName().data(), pMudder->GetName(), pRegion->GetName());
						}
						else
						{
							if(pMudder->GetHideFlag() == 1 && pRegion && pRegion->GetRgnHideFlag() != 2)
								_snprintf(szInfo, 1024, "%s玩家在<%s>成功击杀敌国人。",	 GlobalSetup::GetSetup()->szHideName, pRegion->GetName());
							else
								_snprintf(szInfo, 1024, "%s玩家在<%s>成功击杀敌国人。",	 pMudder->GetName(), pRegion->GetName());
						}
						CMessage msg2(MSG_S2W_OTHER_TALK_NOTIFY);
						msg2.Add(BYTE(2));
						msg2.Add(pMudder->GetCountry());	// 国家ID
						msg2.Add(szInfo);
						msg2.Send();
					}

				}
			}
		}
		break;

	case TYPE_MONSTER:
		{
			CMonster *pMudder = pRegion->FindMonsterByID(GetKilledMeID());
			if (pMudder)
			{
				_snprintf(szMsg, 1024, "[Colour 'ffff0000']你被怪物<%s>杀死了。", pMudder->GetName());
				SendNotifyMessage(szMsg);
			}
		}
		break;

	case TYPE_PET:
		{
			CPet *pMudder = GetPet(GetKilledMeID());
			if( pMudder != NULL )
			{
				CPlayer* pPlayer = (CPlayer*)pMudder->GetHost();
				_snprintf(szMsg, 1024, "[Colour 'ffff0000']你被玩家%s的宠物<%s>杀死了。",
					pPlayer->GetName(),
					pMudder->GetName());
				SendNotifyMessage(szMsg);

				// 通知
				// 允许通知 不同国家 非国战时间 在非竞技区
				if( pRegion->GetNotify() &&
					GetCountry() != pMudder->GetCountry() &&
					pRegion->GetSecurity(GetTileX(), GetTileY()) != CRegion::SECURTIY_FIGHT )
				{
					char szInfo[1024];
					if( pRegion->GetLastNotifyKillTime()+pRegion->GetNotify() < timeGetTime() )
					{
						pRegion->SetLastNotifyKillTime( timeGetTime() );



						// 通知他国在线
						// [玩家名]在[场景]成功击杀了敌人！
						if( pPlayer->GetFactionID() != NULL_GUID)
						{
							//_snprintf(szInfo, "<%s>帮会 %s玩家在<%s>成功击杀敌国人。",
							//	pMudder->GetFactionName().data(),
							//	pMudder->GetName(),
							//	pRegion->GetName());
						}
						else
						{
							if(pPlayer->GetHideFlag() == 1 && pRegion && pRegion->GetRgnHideFlag() != 2)
								_snprintf(szInfo, 1024, "%s玩家在<%s>成功击杀敌国人。",
								GlobalSetup::GetSetup()->szHideName,
								pRegion->GetName());
							else
								_snprintf(szInfo, 1024, "%s玩家在<%s>成功击杀敌国人。",
									pPlayer->GetName(),
									pRegion->GetName());
						}
						CMessage msg2(MSG_S2W_OTHER_TALK_NOTIFY);
						msg2.Add(BYTE(2));
						msg2.Add(pMudder->GetCountry());	// 国家ID
						msg2.Add(szInfo);
						msg2.Send();
					}
				}
			}
		}
		break;
	}
}

// 升级
long CPlayer::CheckLevel()
{
	WORD lOldLevel = GetLevel();
	string strHp = "Hp";
	string strMp = "Mp";
	string strExp = "Exp";
	string strExpMultiple = "ExpMultiple";

	LONG64 llExp = GetTotalExp();
	LONG64 llNextExp = GetNextTotalExp();
	while( llExp >= llNextExp )
	{
		if (GetLevel() >= PlayerList::GetLevelNum())	// 已经是最大等级
		{
			SetExp(GetNextTotalExp());
			break;
		}
		else	
		{
			// 升级
			SetLevel(GetLevel()+1, FALSE);		
			//设置Hp
			SetAttribute(strHp,GetMaxHP());
			//设置Mp
			SetAttribute(strMp,GetMaxMP());
			LONG64 llRet = llExp-llNextExp;
			//设置Exp
			SetAttribute(strExp, (long)llRet%dwExpMutipleValue);
			//设置ExpMultiple 
			SetAttribute(strExpMultiple, (long)llRet/dwExpMutipleValue);
			//触发等级变化接口
			OnLevelChanged();
			UpdateAttribute();

			llExp = llRet;
			llNextExp = GetNextTotalExp();
		}
	}

	WORD lChangeLevel = GetLevel() - lOldLevel;
	//##检查是否升级
	if( lChangeLevel > 0 )
	{
		GetLogicLogInterface()->logT100_player_level_log(this, lOldLevel, "普通升级");
		// 更新属性数据
		GetGame()->UpdateSavePlayer(this, SAVE_DETAIL_ATTRPROPERTY);
		//##发送升级消息给周围玩家
		CMessage msg( MSG_S2C_PLAYER_LEVELUP );
		msg.Add( GetExID() );
		msg.Add(GetMaxHP());
		msg.Add(GetMaxMP());
		msg.Add(GetLevel());
		msg.Add( GetExp() );
		msg.Add(GetExpMultiple());
		msg.Add(GetNextExp());
		msg.Add(GetNextExpMultiple());
		//得到当前赠送经验
		msg.Add(GetPresentExp());
		msg.SendToAround(this);

		// Fox@20081110 刷新角色等级
		GetInst(CPlayerMan).RefreshElem(PET_LEVEL, this);
	}

	return lChangeLevel;
}

// 检查职业等级变化
long CPlayer::CheckOccuLevel(eOccupation byOccu)
{
	WORD lOldOccuLevel = GetOccuLvl(byOccu);
	string strHp = "Hp";
	string strMp = "Mp";
	string strExp = "Exp";

	while( GetOccuExp(byOccu) >= GetNextOccuExp() )
	{
		if (GetOccuLvl(byOccu) >= PlayerList::GetOccuLevelNum(byOccu))	// 已经是最大等级
		{
			SetOccuExp(byOccu,GetNextExp());
			break;
		}
		else	
		{
			// 升级
			SetOccuLvl(byOccu,GetOccuLvl(byOccu)+1, FALSE);		
			//经验
			SetOccuExp(byOccu,GetOccuExp(byOccu)-GetNextOccuExp());
			//触发等级变化接口
			OnOccuLevelChanged();
			UpdateAttribute();
		}
	}

	WORD lChangeLevel = GetOccuLvl(byOccu) - lOldOccuLevel;
	//##检查是否升级
	if( lChangeLevel > 0 )
	{
		GetLogicLogInterface()->logT101_player_Occulevel_log(this, lOldOccuLevel, "普通升级");
		// 更新属性数据
		GetGame()->UpdateSavePlayer(this, SAVE_DETAIL_ATTRPROPERTY);
		//##发送升级消息给周围玩家
		CMessage msg( MSG_S2C_PLAYER_OCCULEVELUP );
		msg.Add( GetExID() );
		msg.Add(GetMaxHP());
		msg.Add(GetMaxMP());
		msg.Add(BYTE(byOccu));
		msg.Add(GetOccuLvl(byOccu));
		msg.Add(GetOccuExp(byOccu) );
		msg.Add(GetNextOccuExp());
		msg.SendToAround(this);
	}

	return lChangeLevel;
}

/**## Added by Jar @ 2004-4-6 */
bool CPlayer::IsGM()
{
	if( NULL == GMList::GetInfoByName( GetName() ) ) 
		return false;
	return true;
}

long CPlayer::GetGMLevel()
{
	tagGMInfo* pInfo = GMList::GetInfoByName( GetName() );
	if( pInfo )
	{
		return pInfo -> lLevel;
	}
	else return GM_PLAYER;
}

// 检测玩家的TileX,Y是否在指定范围内 (一个正方行内)
// x,y中心坐标坐标，range跨度，
//
//	■■■■■■■
//	■p1■■■■■	r
//	■■■■■■■	a
//	■■■xy■■■	n = 3
//	■■■■■■■	g
//	■■■■■■■	e
//	■■■■■■■
//		p2
//
// p1 在区域内， p2 在区域外
bool CPlayer::IsInArea(/*long regionid,*/ long x, long y, long range)
{
	long lX,lY;

	// 检测坐标减去跨度小于0的情况 和 大与WH的情况
	CServerRegion* pRegion=dynamic_cast<CServerRegion*>(GetFather());

	lX=lY=0;


	if( x - range < 0 )
	{
		lX = 0;
	}
	else if(x - range > pRegion->GetWidth() )
	{
		lX = pRegion->GetWidth();
	}
	else
	{
		lX = x;
	}

	if( y - range < 0 ) 
	{
		lY = 0;
	}
	else if( y-range > pRegion->GetHeight() )
	{
		lY = pRegion->GetHeight();
	}
	else
	{
		lY = y;
	}

	if( GetTileX() >= lX - range && 
		GetTileX() <= lX + range &&
		GetTileY() >= lY - range &&
		GetTileY() <= lY + range )
		return true;
	return false;
}

// 检测场景
bool CPlayer::IsInRegion(long regionid)
{
	// 检测是否在该场景
	if( GetRegionID() != regionid ) return false;

	return true;
}
//改变职业,用来专门包装转职的接口
//职业变换以后要重新设置相关属性
void CPlayer::ChangeOccu(eOccupation eNewOccu)
{
	//如果等于现在的职业，则直接返回
	if(GetOccupation() == eNewOccu)	return;
	ProduceEvent(ET_ChangeOccu);
	int nOldOccu = GetOccupation();
	int nNewOccu = eNewOccu;
	SetOccupation(eNewOccu);
	//如果从来没转到该职业,则初始化该职业等级为1
	if(m_Property.byOccuLvl[m_Property.byOccu] == 0)
		m_Property.byOccuLvl[m_Property.byOccu] = 1;
	AddAttributeToDirty(string("Occu"));
	char pszTempt[256]="";
	_snprintf(pszTempt,256,"OccuLvl%d",m_Property.byOccu);
	AddAttributeToDirty(string(pszTempt));

	list<long> lSkill = CNewSkillFactory::GetInitOccuSkill(GetOccupation());
	for (list<long>::iterator it=lSkill.begin(); it!=lSkill.end(); ++it)
	{

		DWORD dwID = (*it);
		DWORD dwLevel = 1;		
		AddSkill(dwID,dwLevel);		
		OnBeginSkill(dwID,dwLevel);		
	}	

	//重设战斗属性的基本属性的属性
	ResetBaseFightProperty();
	//重设其他战斗属性
	ResetFightProperty();
	////更新能量定时器
	//UpdateEnergyTimer();//

	UpdateAttribute(1);

	// Fox@20081111 刷新角色等级
	GetInst(CPlayerMan).RefreshElem(PET_OCCUPATION, this);

	//! 通知WS改变
	if(nOldOccu != nNewOccu)
	{
		CMessage msg(MSG_S2W_PLAYER_OCCU_CHANGE);
		msg.Add(GetExID());
		msg.Add((LONG)eNewOccu);
		msg.Send(false);
	}
}

void CPlayer::ChangePropertyByGoods(CGoods* pGoods,eGoodsAddonProperties addon,int whichVal,string strAttrName,bool bMount)
{
	if(pGoods==NULL)
		return;
	if(whichVal!=1 && whichVal!=2)
		return;
	long lval=pGoods -> GetAddonPropertyValues( addon, whichVal );	
	if(bMount)
	{					
		ChangeAttribute(strAttrName,lval);
	}
	else
	{
		ChangeAttribute(strAttrName,-lval);
	}	
}

void CPlayer::UpdatePropertyByGoodsTrigger(CGoods* pGoods,bool bMount)
{
	if(!pGoods)
		return;
	//物品是否有‘属性激发’的附加属性
	if(!pGoods->HasAddonProperty(GAP_PROPERTY_TRIGGER))
		return;
	LONG lCurTriggerLevel=pGoods->GetAddonPropertyValues(GAP_PROPERTY_TRIGGER,1);
	if(lCurTriggerLevel<=0)
		return;
	CGoodsBaseProperties* pGoodsBaseProperty=GoodsSetup::QueryGoodsBaseProperties(pGoods->GetBasePropertiesIndex());
	if(!pGoodsBaseProperty)
		return;
	for(size_t i=0;i<pGoodsBaseProperty->GetTriggerProperties().size();i++)
	{			
		if(lCurTriggerLevel>=pGoodsBaseProperty->GetTriggerProperties()[i].wTriggerLevel)
		{
			string strPropertyName=CPlayer::GetFightPropertyNameByEnum(pGoodsBaseProperty->GetTriggerProperties()[i].gapType);
			if(strPropertyName!="")
			{
				if(bMount)
					ChangeAttribute(strPropertyName,pGoodsBaseProperty->GetTriggerProperties()[i].lVal1);
				else
					ChangeAttribute(strPropertyName,-(pGoodsBaseProperty->GetTriggerProperties()[i].lVal1));
			}
		}		
	}	
}

void CPlayer::MountAllEquip()
{
	CGoods* pGoods=NULL;
	for(size_t i = 0; i<EC_TOTAL; i++)
	{		
		pGoods = m_cEquipment.GetGoods((DWORD)i);
		if(pGoods!=NULL)
		{   
            if( !pGoods->HasAddonProperty( GAP_GOODS_MAX_DURABILITY) || 
                pGoods->GetAddonPropertyValues( GAP_GOODS_MAX_DURABILITY, 2 ) > 0)
			    MountEquipEx(pGoods,true);
		}
	}
}

void CPlayer::MountEquipEx(CGoods *pGoods, bool bMount, bool bGuardianFlag )
{
	if(pGoods==NULL)
		return;
	vector<eGoodsAddonProperties> vOut;
	pGoods -> GetEnabledAddonProperties( vOut );
	BOOL bHasAddSkillProperty=pGoods->HasAddonProperty(GAP_ADD_SKILL);
	if(bHasAddSkillProperty)
	{
		long lSkillId=pGoods->GetAddonPropertyValues(GAP_ADD_SKILL,1);
		DWORD lSkillLev=pGoods->GetAddonPropertyValues(GAP_ADD_SKILL,2);
		if(bMount)
		{
			//添加技能
			if(CheckStudySkill(lSkillId,lSkillLev))
			{
				StudySkill(lSkillId,lSkillLev);
			}
		}
		else
		{
			//删除技能
			DelSkill(lSkillId,lSkillLev);
		}
	}

	// 如果是‘星座守护石’类物品，检查其‘星座能量’是否有效，无效则屏蔽各类属性
	//if( !bGuardianFlag )
	//{
	//	long lCurEnergy = pGoods->GetGuardianEnergy();
	//	if( lCurEnergy == 0 )
	//	{
	//		// 屏蔽属性
	//		return ;
	//	}
	//}

	//for( size_t i = 0; i < vOut.size(); i++ )
	//{
	//	switch( vOut[i] ) 
	//	{			
	//	case CGoodsBaseProperties::GAP_ADD_CON://体质追加
	//		{
	//			long lret=pGoods -> GetAddonPropertyValues( vOut[i], 1 );
	//			string strAttrName="";
	//			GlobalSetup::tagSetup *pSetup = GlobalSetup::GetSetup();					
	//			long lVal=lret * (long)pSetup->fCon2MaxHp[GetOccupation()];
	//			if(bMount)
	//			{	
	//				strAttrName="E_Consti";
	//				ChangeAttribute(strAttrName,lret);
	//			}
	//			else
	//			{					
	//				strAttrName="E_Consti";
	//				DWORD dwCurHp=GetHP();
	//				ChangeAttribute(strAttrName,-lret);		
	//				if(dwCurHp>GetMaxHP())
	//				{
	//					strAttrName="Hp";
	//					ChangeAttribute(strAttrName,GetMaxHP());
	//				}
	//				//strAttrName="Hp";
	//				//ChangeAttribute(strAttrName,GetMaxHP());
	//			}						
	//		}
	//		break;
	//	case CGoodsBaseProperties::GAP_ADD_MAXHP_LEVEL:	//HP冲等生命上限
	//		{
	//			long lVal=pGoods->GetAddonPropertyValues(vOut[i],1);
	//			string strAttrName="E_MaxHp";
	//			if(bMount)
	//			{
	//				ChangeAttribute(strAttrName,lVal);
	//			}
	//			else
	//			{
	//				ChangeAttribute(strAttrName,-lVal);
	//				/*
	//				if(GetHP()>GetMaxHP())
	//				{
	//				strAttrName="Hp";
	//				ChangeAttribute(strAttrName,GetMaxHP());
	//				}*/
	//			}
	//		}
	//		break;		
	//	case CGoodsBaseProperties::GAP_MAXHP_CORRECT:	//生命上限修正	
	//		{
	//			ChangePropertyByGoods(pGoods,CGoodsBaseProperties::GAP_MAXHP_CORRECT,1,"E_MaxHp",bMount);	
	//			if(!bMount)
	//			{
	//				// 卸装
	//				if(GetHP() > GetMaxHP())
	//				{
	//					string strAttrName="Hp";
	//					ChangeAttribute(strAttrName,GetMaxHP());						
	//				}
	//			}
	//			else
	//			{

	//			}
	//		}
	//		break;
	//	case CGoodsBaseProperties::GAP_RANDOM_ADDON_MAXHP:	//随机生命上限修正	
	//		{
	//			ChangePropertyByGoods(pGoods,CGoodsBaseProperties::GAP_RANDOM_ADDON_MAXHP,1,"E_MaxHp",bMount);	
	//			if(!bMount)
	//			{
	//				// 卸装
	//				if(GetHP() > GetMaxHP())
	//				{
	//					string strAttrName="Hp";
	//					ChangeAttribute(strAttrName,GetMaxHP());						
	//				}
	//			}
	//			else
	//			{

	//			}
	//		}
	//		break;
	//	case CGoodsBaseProperties::GAP_ADD_MAX_PKCP: // PK CP值上限修正(随机)
	//		{
	//			ChangePropertyByGoods( pGoods, CGoodsBaseProperties::GAP_ADD_MAX_PKCP, 1, "E_MaxPKCP", bMount );
	//			if( !bMount )
	//			{
	//				// 卸装需要适当调整当前值
	//				long maxcp = GetMaxPKCP();
	//				if( GetPKCP() > maxcp )
	//				{
	//					string strAttrName = "CurPKCP";
	//					SetAttribute( strAttrName, maxcp );
	//				}
	//			}
	//		}
	//		break;
	//	case CGoodsBaseProperties::GAP_TIMES_MAX_PKCP: // PK CP值上限倍率变化
	//		{
	//			// 倍率发生变化，需要更新属性
	//			AddAttributeToDirty( "C_MaxPKCP" );
	//			if( !bMount )
	//			{
	//				// 卸装需要适当调整当前值
	//				long maxcp = GetMaxPKCP();
	//				if( GetPKCP() > maxcp )
	//				{
	//					string strAttrName = "CurPKCP";
	//					SetAttribute( strAttrName, maxcp );
	//				}
	//			}
	//		}
	//		break;
	//	case CGoodsBaseProperties::GAP_MAXMP_CORRECT://法力上限修正
	//		{
	//			ChangePropertyByGoods(pGoods,CGoodsBaseProperties::GAP_MAXMP_CORRECT,1,"E_MaxMp",bMount);	
	//			if(!bMount)
	//			{
	//				// 卸装
	//				if(GetMP() > GetMaxMP())
	//				{
	//					string strAttrName="Mp";
	//					ChangeAttribute(strAttrName,GetMaxMP());						
	//				}
	//			}
	//			else
	//			{

	//			}
	//		}
	//		break;
	//	case CGoodsBaseProperties::GAP_MOUNT_MAXHP://坐骑生命上限追加
	//		{
	//			ChangePropertyByGoods(pGoods,CGoodsBaseProperties::GAP_MOUNT_MAXHP,1,"E_MaxHp",bMount);	
	//			if(!bMount)
	//			{
	//				// 卸装
	//				if(GetHP() > GetMaxHP())
	//				{
	//					string strAttrName="Hp";
	//					ChangeAttribute(strAttrName,GetMaxHP());						
	//				}
	//			}
	//		}
	//		break;
	//	case CGoodsBaseProperties::GAP_ELEMENT_1://! 物品魔化属性1，值1代表魔化属性，值2表示魔化等级
	//	case CGoodsBaseProperties::GAP_ELEMENT_2://! 物品魔化属性2，值1代表魔化属性，值2表示魔化等级
	//	case CGoodsBaseProperties::GAP_ELEMENT_3://! 物品魔化属性3，值1代表魔化属性，值2表示魔化等级
	//		{
	//			vector<long> vPro;
	//			pGoods->GetAddonPropertyValues(vOut[i], vPro);
	//			if (2 > vPro.size() || eGET_Disabled == vPro[0] || eGET_Enabled == vPro[0])
	//				break;

	//			GoodsExSetup::tagElementValue RetagElementValue;
	//			if(!GoodsExManage::GetInstance().GetGoodsExSetup().GetElementValue(pGoods->GetAddonPropertyValues(CGoodsBaseProperties::GAP_ROLE_MINIMUM_LEVEL_LIMIT, 1), vPro[1], RetagElementValue))
	//				break;

	//			DWORD dwElementType = vPro[0] % 10;
	//			if(CGoodsFactory::IsAttEquipment(pGoods))
	//			{
	//				AddElementProperty(dwElementType, AD_Attack, RetagElementValue.lAtkValue * ((bMount)? 1 : -1));
	//			}
	//			else
	//			{
	//				AddElementProperty(dwElementType, AD_Defend, RetagElementValue.lDefValue * ((bMount)? 1 : -1));
	//				for (long i = 0; i < ET_Max; ++i)
	//				{
	//					if(i != dwElementType)
	//						AddElementProperty(i, AD_Defend, RetagElementValue.lAffixValue * ((bMount)? 1 : -1));
	//				}
	//			}
	//		}
	//		break;
	//	case CGoodsBaseProperties::GAP_ELEMENT_XIANG_XING_1://! 魔化相性1
	//	case CGoodsBaseProperties::GAP_ELEMENT_XIANG_XING_2://! 魔化相性2
	//	case CGoodsBaseProperties::GAP_ELEMENT_XIANG_XING_3://! 魔化相性3
	//		{
	//			vector<long> vPro;
	//			pGoods->GetAddonPropertyValues(vOut[i], vPro);
	//			if (2 > vPro.size() || eGET_Disabled == vPro[0] || eGET_Enabled == vPro[0])
	//				break;

	//			DWORD dwElementType = vPro[0] % 10;
	//			if(CGoodsFactory::IsAttEquipment(pGoods))
	//				AddElementProperty(dwElementType, AD_AttackXiang, vPro[1] * ((bMount)? 1 : -1));
	//			else
	//				AddElementProperty(dwElementType, AD_DefendXiang, vPro[1] * ((bMount)? 1 : -1));
	//		}
	//		break;
	//	default:
 //           {
 //               string strAttrName = CPlayer::GetFightPropertyNameByEnum(vOut[i]);
 //               if(strAttrName != "")
 //               {
 //                   ChangePropertyByGoods(pGoods, vOut[i], 1, strAttrName, bMount);
 //               }
 //           }
	//		break;
	//	}
	//}

	UpdatePropertyByGoodsTrigger(pGoods,bMount);


	if(pGoods->IsSuitGoods())
	{
		LONG lSuitId=pGoods->GetAddonPropertyValues(GAP_SUITID,1);		

		map<long,vector<tagSuitGoods>*>::iterator suitIter=m_mSuitState.find(lSuitId);
		if(suitIter!=m_mSuitState.end())//已经有对应ID的套装状态
		{				
			if(bMount)
			{
				//vector中是否存在相同原始名的物品
				BOOL bSameOriginName=FALSE;
				vector<tagSuitGoods>::iterator goodsIter=suitIter->second->begin();
				const CHAR* pOriginName=GoodsSetup::QueryGoodsOriginalName(pGoods->GetBasePropertiesIndex());
				for(;goodsIter!=suitIter->second->end();goodsIter++)
				{
					const CHAR* pName=GoodsSetup::QueryGoodsOriginalName(goodsIter->pGoods->GetBasePropertiesIndex());
					if(strcmp(pName,pOriginName)==0)
					{
						bSameOriginName=TRUE;
						break;
					}
				}

				int nSize=(int)suitIter->second->size();

				if(!bSameOriginName)
				{
					tagSuitGoods stSuitGoods;
					stSuitGoods.bActived=FALSE;
					stSuitGoods.pGoods=pGoods;
					suitIter->second->push_back(stSuitGoods);	

					if(CSuitRuleFactory::SuitActive(this,nSize,pGoods))
					{							
						SendNotifyMessage("套装效果被激发");
					}
					OnSuitNumChanged(lSuitId,(int)suitIter->second->size());
				}																
			}
			else
			{	
				//目前装备中是否有相同原始名物品
				vector<CGoods*> goodsVec;
				m_cEquipment.GetGoodsByID(pGoods->GetBasePropertiesIndex(),goodsVec);
				CGoods* pNextGoods=NULL;
				if(goodsVec.size()>=1)
					pNextGoods=goodsVec[0];
				//在SUITMAP中是否存在
				BOOL bInMap=FALSE;
				vector<tagSuitGoods>::iterator goodsIter=suitIter->second->begin();				
				for(;goodsIter!=suitIter->second->end();goodsIter++)
				{					
					if(pGoods==goodsIter->pGoods)
					{
						bInMap=TRUE;
						break;
					}
				}

				if(bInMap)
				{				
					//goodsVec不为空说明还装备有同样的套装物品	
					if(!pNextGoods)
					{
						if(CSuitRuleFactory::SuitInvalidate(this,(int)suitIter->second->size(),pGoods))
						{
						}
						pGoods->InvalidSuitProperties(this);					
						suitIter->second->erase(goodsIter);					
						OnSuitNumChanged(lSuitId,(int)suitIter->second->size());
					}
					else
					{
						pGoods->InvalidSuitProperties(this);
						suitIter->second->erase(goodsIter);
						tagSuitGoods stVal;
						stVal.bActived=FALSE;
						stVal.pGoods=pNextGoods;
						suitIter->second->push_back(stVal);
						OnSuitNumChanged(lSuitId,(int)suitIter->second->size());
					}					
				}				
			}
		}
		else
		{
			vector<tagSuitGoods>* pVec=new vector<tagSuitGoods>;
			tagSuitGoods stSuitGoods;
			stSuitGoods.bActived=FALSE;
			stSuitGoods.pGoods=pGoods;
			pVec->push_back(stSuitGoods);
			m_mSuitState[lSuitId]=pVec;
		}
	}
}

int CPlayer::MountFuMoProperty( eGoodsAddonProperties type, int value )
{
	// 如果附魔值为0，直接返回。属性不做改变。
	if(!value) return 0;

	switch( type ) 
	{			
	case 14:	//最小攻击力
		{
			long lret = GetMinAtk() + value;
			if(value > 0)
			{
				SetMinAtk((WORD)lret);
			}
			else
			{
				if(lret>=0)
					SetMinAtk((WORD)lret);
				else
					SetMinAtk(0);
			}					
		}
		break;
	case 15:	//最大攻击力
		{
			long lret = (long)GetMaxAtk() + value;
			if(value > 0)
			{
				SetMaxAtk((WORD)lret);
			}
			else
			{
				if(lret>=0)
					SetMaxAtk((WORD)lret);
				else
					SetMaxAtk(0);
			}					
		}
		break;
		//case 16:	//法术攻击系数
		//	{	
		//		long lret = GetElementModify() + value;
		//		if(value > 0)
		//		{
		//			SetElementModify(lret);
		//		}
		//		else
		//		{
		//			if(lret>=0)
		//				SetElementModify(lret);
		//			else
		//				SetElementModify(0);
		//		}					
		//	}
		//	break;
	case 17:	//物理防御力
		{
			long lret = GetDef() + value;
			if(value > 0)
			{
				SetDef((WORD)lret);
			}
			else
			{
				if(lret>=0)
					SetDef((WORD)lret);
				else
					SetDef(0);
			}
		}
		break;
		//case 18:	//攻击速度修正
		//	{
		//		long lret  =GetAtcSpeed() + value;
		//		if(value > 0)
		//		{
		//			SetAtcSpeed(lret);
		//		}
		//		else
		//		{
		//			if(lret>=0)
		//				SetAtcSpeed(lret);
		//			else
		//				SetAtcSpeed(0);
		//		}					
		//	}
		//	break;
	case 19:	//命中能力
		{
			long lret=GetHit() + value;
			if(value > 0)
			{
				SetHit((WORD)lret);
			}
			else
			{
				if(lret>=0)
					SetHit((WORD)lret);
				else
					SetHit(0);
			}
		}
		break;
		//case 20:	//致命一击
		//	{
		//		long lret=GetCCH() + value;
		//		if(value > 0)
		//		{
		//			SetCCH(lret);
		//		}
		//		else
		//		{
		//			if(lret>=0)
		//				SetCCH(lret);
		//			else
		//				SetCCH(0);
		//		}					
		//	}
		//	break;
	case 21:	//闪避能力
		{
			long lret=GetDodge() + value;
			if(value > 0)
			{
				SetDodge((WORD)lret);
			}
			else
			{
				if(lret>=0)
					SetDodge((WORD)lret);
				else
					SetDodge(0);
			}					
		}
		break;
	case 22:	
		{					
		}
		break;
		//case 23:	//法术抗性修正
		//	{
		//		long lret=GetElementResistant() + value;
		//		if(value > 0)
		//		{
		//			SetElementResistant(lret);
		//		}
		//		else
		//		{
		//			if(lret>=0)
		//				SetElementResistant(lret);
		//			else
		//				SetElementResistant(0);
		//		}					
		//	}
		//	break;
	case 24:	
		{
		}
		break;
	case 25:	//生命恢复速度
		{
			long lret=GetHpRecoverSpeed() + value;
			if(value > 0)
			{
				SetHpRecoverSpeed((WORD)lret);
			}
			else
			{
				if(lret>=0)
					SetHpRecoverSpeed((WORD)lret);
				else
					SetHpRecoverSpeed(0);							
			}					
		}
		break;
	case 26:	//法力恢复速度
		{
			long lret=GetMpRecoverSpeed() + value;
			if(value > 0)
			{
				SetMpRecoverSpeed((WORD)lret);
			}
			else
			{
				if(lret>=0)
					SetMpRecoverSpeed((WORD)lret);
				else
					SetMpRecoverSpeed(0);							
			}					
		}
		break;
		//case 27:	//力量追加
		//	{
		//		long lret=GetStr() + value;
		//		if(value > 0)
		//		{
		//			SetStr(lret);
		//			GlobalSetup::tagSetup *pSetup = GlobalSetup::GetSetup();
		//			short sStrChange = short( value );
		//			SetMaxAtk(WORD(m_Property.wMaxAtk + sStrChange * pSetup->fStr2MaxAtk[GetOccupation()]));
		//			SetBurden(WORD(m_Property.wBurden + sStrChange * pSetup->fStr2Burden[GetOccupation()]));
		//		}
		//		else
		//		{
		//			if(lret>=0)							
		//				SetStr(lret);							
		//			else							
		//				SetStr(0);
		//			GlobalSetup::tagSetup *pSetup = GlobalSetup::GetSetup();
		//			short sStrChange = short( value );

		//			float fVal=m_Property.wMaxAtk + sStrChange * pSetup->fStr2MaxAtk[GetOccupation()];
		//			if(fVal>=0)
		//				SetMaxAtk(fVal);
		//			else
		//				SetMaxAtk(0);

		//			fVal=m_Property.wBurden + sStrChange * pSetup->fStr2Burden[GetOccupation()];
		//			if(fVal>=0)
		//				SetBurden(fVal);
		//			else
		//				SetBurden(0);
		//		}					
		//	}
		//	break;
		//case 28://敏捷追加
		//	{
		//		long lret=GetDex() + value;
		//		if(value > 0)
		//		{
		//			SetDex(lret);
		//			GlobalSetup::tagSetup *pSetup = GlobalSetup::GetSetup();
		//			short sDexChange = short( value );
		//			SetMinAtk(WORD(m_Property.wMinAtk + sDexChange * pSetup->fDex2MinAtk[GetOccupation()]));
		//			SetReAnk(WORD(m_Property.wReAnk + sDexChange * pSetup->fDex2Stiff[GetOccupation()]));

		//		}
		//		else
		//		{
		//			if(lret>0)
		//				SetDex(lret);
		//			else
		//				SetDex(0);

		//			GlobalSetup::tagSetup *pSetup = GlobalSetup::GetSetup();
		//			short sDexChange = short( value );

		//			float fVal=m_Property.wMinAtk + sDexChange * pSetup->fDex2MinAtk[GetOccupation()];
		//			if(fVal>=0)
		//				SetMinAtk(fVal);
		//			else
		//				SetMinAtk(0);

		//			fVal=m_Property.wReAnk + sDexChange * pSetup->fDex2Stiff[GetOccupation()];
		//			if(fVal>=0)
		//				SetReAnk(fVal);
		//			else
		//				SetReAnk(0);
		//		}				
		//	}
		//	break;
	case 29://体质追加
		{
			long lret=GetCon() + value;
			if(value > 0)
			{
				SetCon((WORD)lret);
				GlobalSetup::tagSetup *pSetup = GlobalSetup::GetSetup();
				short sConChange = short( value );

				/*SetMaxHP(WORD(m_Property.wMaxHp + sConChange * pSetup->fCon2MaxHp[GetOccupation()]));
				SetDef(WORD(m_Property.wDef + sConChange * pSetup->fCon2Def[GetOccupation()]));*/
				SetMaxHP(DWORD(m_FightProperty.dwMaxHp + sConChange * pSetup->fCon2MaxHp[GetOccupation()]));
				SetDef(WORD(m_FightProperty.wDef + sConChange * pSetup->fCon2Def[GetOccupation()]));
			}
			else
			{
				if(lret>=0)
					SetCon((WORD)lret);
				else
					SetCon(0);

				GlobalSetup::tagSetup *pSetup = NULL;// GlobalSetup::SetConsti();
				short sConChange = short( value );
				float fVal=m_FightProperty.dwMaxHp + sConChange * pSetup->fCon2MaxHp[GetOccupation()];
				if(fVal>=0)
					SetMaxHP((DWORD)fVal);
				else
					SetMaxHP(0);
				fVal=m_FightProperty.wDef + sConChange * pSetup->fCon2Def[GetOccupation()];
				if(fVal>=0)
					SetDef((WORD)fVal);
				else
					SetDef(0);
			}									
		}
		break;
		//case 30://灵力追加
		//	{
		//		long lret=GetInt() + value;
		//		if(value > 0)
		//		{
		//			SetInt(lret);
		//			GlobalSetup::tagSetup *pSetup = GlobalSetup::GetSetup();
		//			short sIntChange =short( value );

		//			SetElementModify(WORD(m_Property.sElementModify + sIntChange * pSetup->fInt2Element[GetOccupation()]));
		//			SetMaxMP(WORD(m_Property.wMaxMp + sIntChange * pSetup->fInt2MaxMp[GetOccupation()]));
		//			SetElementResistant(WORD(m_Property.wElementResistant + sIntChange * pSetup->fInt2Resistant[GetOccupation()]));
		//		}
		//		else
		//		{
		//			if(lret>=0)
		//				SetInt(lret);
		//			else
		//				SetInt(0);
		//			GlobalSetup::tagSetup *pSetup = GlobalSetup::GetSetup();
		//			short sIntChange =short( value );

		//			float fVal=m_Property.sElementModify + sIntChange * pSetup->fInt2Element[GetOccupation()];
		//			if(fVal>=0)
		//				SetElementModify(fVal);
		//			else
		//				SetElementModify(0);

		//			fVal=m_Property.wMaxMp + sIntChange * pSetup->fInt2MaxMp[GetOccupation()];
		//			if(fVal>=0)
		//				SetMaxMP(fVal);
		//			else
		//				SetMaxMP(0);

		//			fVal=m_Property.wElementResistant + sIntChange * pSetup->fInt2Resistant[GetOccupation()];
		//			if(fVal>=0)
		//				SetElementResistant(fVal);
		//			else
		//				SetElementResistant(0);
		//		}					
		//	}
		//	break;
	case 31:	//生命上限修正	
		{
			long lret=GetMaxHP() + value;
			if(value > 0)
			{
				SetMaxHP((DWORD)lret);
			}
			else
			{
				if(lret>=0)
					SetMaxHP((DWORD)lret);
				else
					SetMaxHP(0);
			}				
		}
		break;
	case 32://法力上限修正
		{
			long lret=GetMaxMP() + value;
			if(value > 0)
			{
				SetMaxMP((WORD)lret);
			}
			else
			{
				if(lret>=0)
					SetMaxMP((WORD)lret);
				else
					SetMaxMP(0);
			}					
		}
		break;
	case 33:
		{
		}
		break;
	case 34:
		{
		}
		break;
	case 35:
		{
		}
		break;
	case 36:
		{
		}
		break;
	case 37:
		{
		}
		break;
	case 38:
		{
		}
		break;
	case 39:
		{
		}
		break;
	case 40:
		{
		}
		break;
	case 41:
		{
		}
		break;
	case 42:
		{
		}
		break;
	case 43:
		{
		}
		break;
	case 44:
		{
		}
		break;
	case 45:
		{
		}
		break;
	case 46:
		{
		}
		break;
	case 47:
		{
		}
		break;
		//case 51:	//硬直修正
		//	{
		//		long lret=GetReAnk() + value;
		//		if(value > 0)
		//		{
		//			SetReAnk(lret);
		//		}
		//		else
		//		{
		//			if(lret>=0)
		//				SetReAnk(lret);
		//			else
		//				SetReAnk(0);
		//		}
		//	}
		//	break;
		//case 52:	//负重修正
		//	{
		//		long lret=GetBurden() + value;
		//		if(value > 0)
		//		{
		//			SetBurden(lret);
		//		}
		//		else
		//		{
		//			if(lret>=0)
		//				SetBurden(lret);
		//			else
		//				SetBurden(0);
		//		}
		//	}
		//	break;
		//case 91:	// 物免比例
		//	{
		//		long lret=GetAttackAvoid() + value;
		//		if(value > 0)
		//		{
		//			SetAttackAvoid(lret);
		//		}
		//		else
		//		{
		//			if(lret>=0)
		//				SetAttackAvoid(lret);
		//			else
		//				SetAttackAvoid(0);
		//		}
		//	}
		//	break;
		//case 92:	// 魔免比例
		//	{
		//		long lret=GetElementAvoid() + value;
		//		if(value > 0)
		//		{
		//			SetElementAvoid(lret);
		//		}
		//		else
		//		{
		//			if(lret>=0)
		//				SetElementAvoid(lret);
		//			else
		//				SetElementAvoid(0);
		//		}
		//	}
		//	break;
		//case 93:	// 闪避几率
		//	{
		//		long lret=GetFullMiss() + value;
		//		if(value > 0)
		//		{
		//			SetFullMiss(lret);
		//		}
		//		else
		//		{
		//			if(lret>=0)
		//				SetFullMiss(lret);
		//			else
		//				SetFullMiss(0);
		//		}
		//	}
		//	break;
		//case 95:	// 物理爆击几率
		//	{
		//		long lret=GetBlastAttack() + value;
		//		if(value > 0)
		//		{
		//			SetBlastAttack(lret);
		//		}
		//		else
		//		{
		//			if(lret>=0)
		//				SetBlastAttack(lret);
		//			else
		//				SetBlastAttack(0);
		//		}
		//	}
		//	break;
		//case 96:	// 法术爆击几率
		//	{
		//		long lret=GetBlastAttack() + value;
		//		if(value > 0)
		//		{
		//			SetBlastElementAttack(lret);
		//		}
		//		else
		//		{
		//			if(lret>=0)
		//				SetBlastElementAttack(lret);
		//			else
		//				SetBlastElementAttack(0);
		//		}
		//	}
		//	break;

	case 97:	// 付魔
		{
			break;
		}
		break;
	default:
		break;
	}
	return value;
}

void CPlayer::InitFightEnumMap()
{
	//m_mapFightEnum[GAP_MAXHP_CORRECT]="E_MaxHp";//生命上限			
	//m_mapFightEnum[GAP_MAXMP_CORRECT]="E_MaxMp";//法力上限			
	//m_mapFightEnum[GAP_HP_RESUME_CORRECT]="E_HpRecoverSpeed";//生命恢复速度			
	//m_mapFightEnum[GAP_MP_RESUME_CORRECT]="E_MpRecoverSpeed";//法力恢复速度			
	//m_mapFightEnum[GAP_ADD_STR]="E_Str";//力量			
	//m_mapFightEnum[GAP_ADD_DEX]="E_Dex";//敏捷			
	//m_mapFightEnum[GAP_ADD_CON]="E_Con";//体质		
	//m_mapFightEnum[GAP_ADD_INT]="E_Intellect";//智力
	//m_mapFightEnum[GAP_ADD_SPI]="E_Spiritualism";//精神			
	//m_mapFightEnum[GAP_PARRY_CORRECT]="E_Block";//格挡			
	//m_mapFightEnum[GAP_BLOCK_CORRECT]="E_Parry";//招架			
	//m_mapFightEnum[GAP_FLEE_CORRECT]="E_Dodge";//闪避			
	//m_mapFightEnum[GAP_HIT_RATE_CORRECT]="E_Hit";//命中
	//m_mapFightEnum[GAP_DEDICANT_CORRECT]="E_Absorb";//专注			
	//m_mapFightEnum[GAP_NOR_DEF]="E_Def";//物理防御	
	//m_mapFightEnum[GAP_MAGIC_DEF]="E_Mdef";//魔法抗性			
	//m_mapFightEnum[GAP_NORATTACK_KILLRATE]="E_Cri";//物理爆击			
	//m_mapFightEnum[GAP_MAGIC_SPEED]="E_MAtkSpeed";//施法速度			
	//m_mapFightEnum[GAP_MIN_ATTACK]="E_MinAtk";//最小攻击力
	//m_mapFightEnum[GAP_MAX_ATTACK]="E_MaxAtk";//最大攻击力
	//m_mapFightEnum[GAP_FUMO_PROPERTY]="E_MAtk";//魔法攻击力			
	//m_mapFightEnum[GAP_MAGIC_KILLRATE]="E_MCri";//魔法爆击率			
	//m_mapFightEnum[GAP_LEECH_HP]="E_BloodSuk";//生命吸收率			
	//m_mapFightEnum[GAP_LEECH_MP]="E_ManaSuck";//法力吸收率		
	//m_mapFightEnum[GAP_WILL_DEF]="E_WillDef";//意志抗性		
	//m_mapFightEnum[GAP_CON_DEF]="E_ConstiDef";//体质抗性
	//m_mapFightEnum[GAP_BLED_DEF]="E_DeBuffDef0";//流血抗性
	//m_mapFightEnum[GAP_DIZZY_DEF]="E_DeBuffDef1";//击昏抗性
	//m_mapFightEnum[GAP_LULL_DEF]="E_DeBuffDef2";//麻痹抗性
	//m_mapFightEnum[GAP_HYPNOSIS_DEF]="E_DeBuffDef3";//催眠抗性
	//m_mapFightEnum[GAP_POISON_DEF]="E_DeBuffDef4";//中毒抗性
	//m_mapFightEnum[GAP_FIX_DEF]="E_DeBuffDef5";//定身抗性
	//m_mapFightEnum[GAP_SILENCE_DEF]="E_DeBuffDef6";//沉默抗性
	//m_mapFightEnum[GAP_FIREDEF_CORRECT]="E_ElemDef0";//火抗		
	//m_mapFightEnum[GAP_WATERDEF_CORRECT]="E_ElemDef1";//水抗
	//m_mapFightEnum[GAP_EARTHDEF_CORRECT]="E_ElemDef2";//地抗	
	//m_mapFightEnum[GAP_WINDDEF_CORRECT]="E_ElemDef3";//风抗
	//m_mapFightEnum[GAP_DARKDEF_CORRECT]="E_ElemDef4";//黑暗抗
	//m_mapFightEnum[GAP_HOLYDEF_CORRECT]="E_ElemDef5";//神圣抗	
	//m_mapFightEnum[GAP_PENETRATE]="E_Pierce";//穿透
	//m_mapFightEnum[GAP_EXCUSE_HURT]="E_Immunity";//免伤
	//m_mapFightEnum[GAP_LUCKY]="E_Luck";//幸运
}

string CPlayer::GetFightPropertyNameByEnum(eGoodsAddonProperties enumVal)
{
	map<eGoodsAddonProperties,string>::iterator nameIter=m_mapFightEnum.find(enumVal);
	if(nameIter!=m_mapFightEnum.end())
	{
		return nameIter->second;
	}
	return "";
}



BOOL CPlayer::RestoreHp( DWORD dwHp, DWORD dwPersistTime, DWORD  dwFrequency)
{
	//##如果当前时间还没有到达最小使用间隔.
	if( timeGetTime() < m_dwLastRestoreHpTimeStamp + GlobalSetup::GetSetup() -> dwHealTimeInterval )
	{
		return FALSE;
	}

	m_dwLastRestoreHpTimeStamp = timeGetTime();

	return FALSE;
}

BOOL CPlayer::RestoreMp( DWORD dwMp, DWORD dwPersistTime, DWORD dwFrequency )
{
	//##如果当前时间还没有到达最小使用间隔.
	if( timeGetTime() < m_dwLastRestoreMpTimeStamp + GlobalSetup::GetSetup() -> 
		dwMpTimeInterval )
	{
		return FALSE;
	}

	m_dwLastRestoreMpTimeStamp = timeGetTime();
	return FALSE;
}

void CPlayer::UseItem(WORD wPlace,BYTE lPos,long lTargetType,CGUID& lTargetGuid)
{	
	//  [7/19/2007 chenxianjin]
	//有不能使用装备的状态
	if (!GetUseItem() || IsLockPlayerState() )
	{
		return;
	}
	CGoods* pGoods=NULL;
	CS2CContainerObjectMove::PLAYER_EXTEND_ID place=static_cast<CS2CContainerObjectMove::PLAYER_EXTEND_ID>(wPlace);

	CVolumeLimitGoodsContainer* pVolumeContainer=NULL;

	if(place==CS2CContainerObjectMove::PEI_PACKET)
	{
		pVolumeContainer=getOriginPack();
		pGoods=getOriginPack()->GetGoods(lPos);
	}
	else if(place>=CS2CContainerObjectMove::PEI_PACK1 && place<=CS2CContainerObjectMove::PEI_PACK5)
	{
		pVolumeContainer=m_pPackage.GetSubpack(place-CS2CContainerObjectMove::PEI_PACK1)->pSubpackContainer;
		pGoods=pVolumeContainer->GetGoods(lPos);
	}	
	CGoodsBaseProperties* pGoodsProperty=GoodsSetup::QueryGoodsBaseProperties(pGoods->GetBasePropertiesIndex());
	if(!pGoodsProperty)
		return;
	if(GetPkValue()>20)
	{
		if(strcmp(pGoodsProperty->GetOriginalName(),"GZ0203")==0 || 
			strcmp(pGoodsProperty->GetOriginalName(),"GZ0204")==0 || 
			strcmp(pGoodsProperty->GetOriginalName(),"GZ0205")==0 || 
			strcmp(pGoodsProperty->GetOriginalName(),"GZ0206")==0)
		{
			const char* strMsg=AppFrame::GetText("49");
			if(strMsg)
				SendSystemInfo(strMsg,0xFFFFFFFF);	
			return;
		}
	}


	CServerRegion* pRegion = dynamic_cast<CServerRegion*>( GetFather() );
	if( pGoods && pRegion )
	{
		if(IsInPersonalShop(pGoods))
		{
			const char* strMsg=AppFrame::GetText("46");
			if(strMsg)
				SendSystemInfo(strMsg,0xFFFFFFFF);	
			return;
		}
		CGoodsBaseProperties* pProperties = GoodsSetup::QueryGoodsBaseProperties(
			pGoods -> GetBasePropertiesIndex() );
		if( pProperties && pProperties -> GetGoodsBaseType() == GBT_CONSUMABLE )
		{
			BOOL bCanUse = TRUE;

			//判断该场景是否支持使用该物品
			EquipRestrict eRet = EQUIPFIT;
			if( pRegion->FindForbidGood(pProperties->GetOriginalName()) == true)
			{
				eRet = REGION_FORBID;
			}

			if( eRet==EQUIPFIT )
			{
				eRet = (EquipRestrict)CanUseItem( pGoods );
			}

			if(eRet!=EQUIPFIT)
			{
				bCanUse = FALSE;
			}

			if( bCanUse && pGoods -> GetAmount() > 0 )
			{
				if(strcmp(pGoodsProperty->GetOriginalName(),"GZ0201")==0 || 
					strcmp(pGoodsProperty->GetOriginalName(),"GZ0201P")==0)
				{
					if(!pRegion->GetSetup().bRuneStoneIsUsed)
					{
						const char* strMsg=AppFrame::GetText("42");
						if(strMsg)
							SendSystemInfo(strMsg,0xFFFFFFFF);
						return;
					}
				}
				
				BOOL bItemUsed		= TRUE;
				BOOL bBackToCity	= FALSE;
				vector<eGoodsAddonProperties> vOut;
				pGoods -> GetEnabledAddonProperties( vOut );

				//如果是使用坐骑的物品,则不进入循环
				if( pGoods -> GetAddonPropertyValues(GAP_MOUNT_TYPE, 1) )
				{			
						//##摆摊，战斗状态下不能骑坐骑
						if( GetCurrentProgress() != PROGRESS_OPEN_STALL && m_lFightStateCount == 0 )
						{
							const char *strGoodOriName = GoodsSetup::QueryGoodsOriginalName(pGoods->GetBasePropertiesIndex());
							if( Mount( pGoods -> GetAddonPropertyValues(GAP_MOUNT_TYPE, 1), 
								pGoods -> GetAddonPropertyValues(GAP_MOUNT_LEVEL, 1), 
								pGoods -> GetAddonPropertyValues(GAP_MOUNT_ROLE_LIMIT, 1),
								strGoodOriName) )
							{
								//##如果是无限制使用,则不减少数量
								if( pGoods -> GetAddonPropertyValues(GAP_UNLIMITED_ACCESS, 1) == 1 )
								{
									bItemUsed = FALSE;
								}
							}
							else
							{
								bItemUsed = FALSE;
							}
						}
						else
						{
							bItemUsed = FALSE;
						}
					
				}
				else
				{
					//! pGoods -> GetEnabledAddonProperties( vOut );
				}

				for( size_t i = 0; i < vOut.size(); i++ )
				{
					switch( vOut[i] )
					{						
					case GAP_ADD_HP:
						{
							if( pGoods -> GetAddonPropertyValues(vOut[i], 2) == 0 )
							{
								bItemUsed = RestoreHp( pGoods -> GetAddonPropertyValues(vOut[i], 1), 0, 400 );

							}
							else
							{
								bItemUsed = RestoreHp( pGoods -> GetAddonPropertyValues(vOut[i], 1), 
									pGoods -> GetAddonPropertyValues(vOut[i], 2) , 400 );
							}
						}
						break;
					case GAP_ADD_PERCENT_HP:	//生命百分比增加
						{
							bItemUsed = RestoreHp( static_cast<DWORD>( GetMaxHP() * ( pGoods -> GetAddonPropertyValues(vOut[i], 1) / 100.0f) ),
								0, 400 );
						}
						break;
					case GAP_ADD_MP:	//法力增加
						{
							if(pGoods -> GetAddonPropertyValues( vOut[i], 2 )==0)
							{
								bItemUsed = RestoreMp( pGoods -> GetAddonPropertyValues(vOut[i], 1), 0, 400 );
							}
							else
							{
								bItemUsed = RestoreMp( pGoods -> GetAddonPropertyValues(vOut[i], 1), (pGoods -> GetAddonPropertyValues(vOut[i], 2) - 1) * 400, 400 );
							}
						}
						break;
					case GAP_ADD_PERCENT_MP:	//法力百分比增加
						{
							bItemUsed = RestoreMp( static_cast<DWORD>( GetMaxMP() * (pGoods -> GetAddonPropertyValues(vOut[i], 1) / 100.0f) ),
								0, 400 );
						}
						break;
					case GAP_HOME:	//回城
						{
							// 在他国领土不能使用回城
							if( bCanUse )
							{
								if(GetCurrentProgress()==CPlayer::PROGRESS_OPEN_STALL 
									|| GetCurrentProgress()==CPlayer::PROGRESS_TRADING)
								{
									bItemUsed=FALSE;
								}
								else if( pRegion->GetCountry() && pRegion->GetCountry() != GetCountry() )
								{
									bItemUsed = FALSE;
									const char* strMsg=AppFrame::GetText("41");
									if(strMsg)
										SendSystemInfo(strMsg,0xFFFFFFFF);									
								}
								else if(!pRegion->GetSetup().bRuneStoneIsUsed)
								{
									if(pRegion->GetSetup().lRuneStoneRegionID == 0) // 原地
									{
									}
									else if(pRegion->GetSetup().lRuneStoneRegionID == -1) // 回国都									{
									{
									}
									else if(pRegion->GetSetup().lRuneStoneRegionID>0) // > 0 切换到该ID场景
									{

									}
									bItemUsed=FALSE;
									const char* strMsg=AppFrame::GetText("42");
									if(strMsg)
										SendSystemInfo(strMsg,0xFFFFFFFF);
								}
								else
								{
									bBackToCity = TRUE;
								}
							}
							else
								bItemUsed = FALSE;

						}
						break;
					case GAP_RAN_TRANS:	//随机传送
						{
							// 只能在本国内使用随机传送
							if(bCanUse)
							{
								if(GetCurrentProgress()==CPlayer::PROGRESS_OPEN_STALL 
									|| GetCurrentProgress()==CPlayer::PROGRESS_TRADING)
								{
									bItemUsed=FALSE;
								}
								else if( pRegion->GetCountry() != GetCountry() )
								{
									bItemUsed = FALSE;
									const char* strMsg=AppFrame::GetText("43");
									if(strMsg)
										SendSystemInfo(strMsg,0xFFFFFFFF);
								}
								else
								{
									CServerRegion* pRegion=((CServerRegion*)GetFather());
									if(pRegion)
									{
										//##在使用回程卷，随机卷，npc传送的时候掉落物品
										DropParticularGoodsWhenRecall();

										long lX = -1, lY = -1;
										pRegion->GetRandomPos(lX, lY);

										// [071127 AHC REGION]
										ChangeRegion(GetCurRgnType(), GetRegionGUID(),lX, lY,GetDir());
									}
								}
							}
							else 
								bItemUsed = FALSE;
						}
						break;
					case GAP_SCRIPT:	//执行脚本
						{
							char strFile[50];
							_snprintf(strFile, 50, "scripts/Goods/%d.lua",pGoods -> GetAddonPropertyValues(vOut[i], 1));
							if( !m_VarList->HasVar( "#m_strUseItem" ) )
							{
								m_VarList->AddVar( "#m_strUseItem", tString("") );
							}
							m_VarList->SetVarValue( "#m_strUseItem", tString( pProperties->GetOriginalName() ) );
							
							Script::RunEnv env( this, NULL, static_cast<CRegion*>( GetFather() ), m_VarList );
							GetInst( ScriptSys ).RunScript( env, strFile );
						}
						break;
					case GAP_ADD_STATE:
						{
							DWORD dwStateId=pGoods -> GetAddonPropertyValues(vOut[i],1);
							DWORD dwLevel=pGoods -> GetAddonPropertyValues(vOut[i],2);	
							AddNewState(this,dwStateId,dwLevel,16);
						}
						break;
					case GAP_POWER_INTENSITY:
						{
							CGoods *pArm = m_cEquipment.GetGoods(EC_E_WEAPON);
							bItemUsed = GoodsExManage::GetInstance().ReinforcePower(pArm, pGoods, this);				
						}
						break;						
					}
				}

				BOOL bHasRecordRegionAttr=pGoods->HasAddonProperty(GAP_RECORD_REGIONID);
				BOOL bHasRecordCoorAttr=pGoods->HasAddonProperty(GAP_RECORD_COOR);
				BOOL bReduce=TRUE;
				if(bHasRecordCoorAttr && bHasRecordRegionAttr)
				{
					//物品是土地符
					LONG lRegionId=pGoods->GetAddonPropertyValues(GAP_RECORD_REGIONID,1);
					LONG lXPos=pGoods->GetAddonPropertyValues(GAP_RECORD_COOR,1);
					LONG lYPos=pGoods->GetAddonPropertyValues(GAP_RECORD_COOR,2);
					if(lRegionId==0)
					{
						//未记录
						CServerRegion* pRegion=((CServerRegion*)GetFather());
						if(pRegion)
						{
							if(pRegion->GetSetup().bRuneStoneCanRecord)
							{	
								//判断阵营
								CCountry *pCountry = GetCountryHandler()->GetCountry(GetCountry());
								CCountry* pRgnCountry=GetCountryHandler()->GetCountry(pRegion->GetCountry());
								if(!pCountry || !pRgnCountry)
									return;
								if(pCountry->GetCampID(GetCountry())==pRgnCountry->GetCampID(pRegion->GetCountry()))
								{
									//同一阵营
									LONG lRgnId=pRegion->GetID();
									LONG lPlayerPosX=(LONG)GetPosX();
									LONG lPlayerPosY=(LONG)GetPosY();
									pGoods->ChangeAttribute(GAP_RECORD_REGIONID,lRgnId,1);
									pGoods->ChangeAttribute(GAP_RECORD_COOR,lPlayerPosX,1);
									pGoods->ChangeAttribute(GAP_RECORD_COOR,lPlayerPosY,2);
									pGoods->UpdateAttribute(GetExID());
								}
								else
								{
									const char* strMsg=AppFrame::GetText("47");
									if(strMsg)
									{
										//SendNotifyMessage(strMsg,0xffffffff,0,eNOTIFYPOS_CENTER);
										//SendNotifyMessage(strMsg,0xffffffff,0,eNOTIFYPOS_SYSTEM);
										SendNotifyMessage(strMsg,eNOTIFYPOS_CENTER);
										SendNotifyMessage(strMsg,eNOTIFYPOS_SYSTEM);
									}
								}
							}
							else
							{
								const char* strMsg=AppFrame::GetText("44");
								if(strMsg)
								{
									SendNotifyMessage(strMsg,eNOTIFYPOS_CENTER);
									SendNotifyMessage(strMsg,eNOTIFYPOS_SYSTEM);
								}
							}

							bReduce=FALSE;
						}
					}
					else if(lRegionId && lXPos>=0 && lYPos>=0)
					{
						//查找ServerRegion  GUID
						CGUID rgnGUID = NULL_GUID;
						CServerRegion* pRgn = GameManager::GetInstance()->GetRgnManager()->FindRgnByTemplateID(RGN_NORMAL, lRegionId);
						if(pRgn)
						{
							if(pRegion->GetSetup().bRuneStoneIsUsed)
							{
								//判断阵营
								CCountry *pCountry = GetCountryHandler()->GetCountry(GetCountry());
								CCountry* pRgnCountry=GetCountryHandler()->GetCountry(pRegion->GetCountry());
								if(!pCountry || !pRgnCountry)
									return;
								if(pCountry->GetCampID(GetCountry())==pRgnCountry->GetCampID(pRegion->GetCountry()))
								{
									//同一阵营
									rgnGUID = pRgn->GetExID();						
									ChangeRegion(RGN_NORMAL, rgnGUID, lXPos, lYPos, GetDir());
								}
								else
								{
									const char* strMsg=AppFrame::GetText("47");
									if(strMsg)
									{
										SendNotifyMessage(strMsg,eNOTIFYPOS_CENTER);
										SendNotifyMessage(strMsg,eNOTIFYPOS_SYSTEM);
									}
									bReduce=FALSE;
								}
							}
							else
							{
								const char* strMsg=AppFrame::GetText("45");
								if(strMsg)
								{
									SendNotifyMessage(strMsg,eNOTIFYPOS_CENTER);
									SendNotifyMessage(strMsg,eNOTIFYPOS_SYSTEM);
								}
								bReduce=FALSE;
							}
						}
					}
				}

				if( bItemUsed )
				{
					//冷却时间
					BOOL bAddSkill=pGoods->HasAddonProperty(GAP_ADD_SKILL);
					if(!bAddSkill)
					{
						DWORD dwGoodsId=pGoods->GetBasePropertiesIndex();
						UpdateCooldown(dwGoodsId);

						CMessage msg(MSG_S2C_PLAYER_USEITEM);
						msg.Add((char)PLAYER_USEITEM_SUCCEED);
						msg.Add(GetExID());
						msg.Add(pGoods->GetBasePropertiesIndex());
						msg.SendToAround( this );
					}


					if(pGoods -> GetAddonPropertyValues(GAP_UNLIMITED_ACCESS, 1) ==0 && !bAddSkill && bReduce)
					{


						pGoods -> SetAmount( pGoods -> GetAmount() - 1 );

						//Goods Consume Log
						GetLogicLogInterface()->logT022_goods_log_consume(this,pGoods->GetBasePropertiesIndex(),pGoods->GetExID(),
							GoodsSetup::QueryGoodsName(pGoods->GetBasePropertiesIndex()),pGoods->GetAmount(),1,wPlace,lPos);

						if( pGoods -> GetAmount() == 0 )
						{
							//消耗道具数量用完 删除							
							if(pVolumeContainer)
							{							
								CS2CContainerObjectMove comMsg;
								if(pVolumeContainer->Remove( pGoods, &comMsg ) )
								{
									comMsg.SetOperation( CS2CContainerObjectMove::OT_DELETE_OBJECT );
									comMsg.SetSourceContainerExtendID( wPlace);
									comMsg.Send( GetExID() );
#ifdef _GOODSLOG1_
									char pszGoodsLog[1024]="";			
									char srcGoodsGUID[50]="";
									pGoods->GetExID().tostring(srcGoodsGUID);					
									_snprintf(pszGoodsLog,1024,"[物品回收:UseItem物品用完][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d]",
										pGoods->GetBasePropertiesIndex(),GoodsSetup::QueryGoodsName(pGoods->GetBasePropertiesIndex()),srcGoodsGUID,pGoods->GetAmount());
									CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
#endif
									CGoodsFactory::GarbageCollect( &pGoods,28 );
								}
								else
								{
									OutputDebugString("Some error occur, goods can't remove from m_cPacket.\n");
								}
							}
						}
						else
						{
							CS2CContainerObjectAmountChange coacMsg;
							coacMsg.SetSourceContainer( GetType(), GetExID(), lPos );
							coacMsg.SetSourceContainerExtendID(wPlace);
							coacMsg.SetObject( pGoods -> GetType(), pGoods -> GetExID() );
							coacMsg.SetObjectAmount( pGoods -> GetAmount() );
							coacMsg.Send( GetExID() );
						}
					}
				}
				else
				{
					//CMessage msg(MSG_S2C_PLAYER_USEITEM);
					//msg.Add((char)PLAYER_USEITEM_FAILED);
					//msg.Add((char)REGION_FORBID);
					//msg.SendToPlayer(GetExID());
				}

				if( bBackToCity )
				{
					//##在使用回程卷，随机卷，npc传送的时候掉落物品
					DropParticularGoodsWhenRecall();
					if (!ChangeRgnToVillage())
					{
						((CServerRegion*)GetFather())->BackToCity(this);
					}					
				}
			}
			else
			{
				if ( eRet == REGION_FORBID )
				{
					const char* strMsg=AppFrame::GetText("83");
					if(strMsg)
						SendNotifyMessage( strMsg, eNOTIFYPOS_CENTER );
				}

				CMessage msg(MSG_S2C_PLAYER_USEITEM);
				msg.Add((char)PLAYER_USEITEM_FAILED);
				msg.Add((char)eRet);
				msg.SendToPlayer(GetExID());
			}
		}
	}
}

void CPlayer::UseItem(DWORD dwContainerId,DWORD dwPos)
{
	CGoods* pGoods=FindGoods(dwContainerId,dwPos);
	if(!pGoods)
		return;
	if(pGoods -> GetAddonPropertyValues(GAP_UNLIMITED_ACCESS, 1) !=0)
		return;
	pGoods -> SetAmount( pGoods -> GetAmount() - 1 );

	//Goods Consume Log
	GetLogicLogInterface()->logT022_goods_log_consume(this,pGoods->GetBasePropertiesIndex(),pGoods->GetExID(),
		GoodsSetup::QueryGoodsName(pGoods->GetBasePropertiesIndex()),pGoods->GetAmount(),1,dwContainerId,dwPos);

	if( pGoods -> GetAmount() == 0 )
	{
		//消耗道具数量用完 删除
		CVolumeLimitGoodsContainer* pVolContainer=NULL;
		pVolContainer=this->GetPackContainerById(dwContainerId);
		if(pVolContainer)
		{							
			CS2CContainerObjectMove comMsg;
			if(pVolContainer->Remove( pGoods, &comMsg ) )
			{
				comMsg.SetOperation( CS2CContainerObjectMove::OT_DELETE_OBJECT );
				comMsg.SetSourceContainerExtendID( dwContainerId);
				comMsg.Send( GetExID() );
#ifdef _GOODSLOG1_
				char pszGoodsLog[1024]="";			
				char srcGoodsGUID[50]="";
				pGoods->GetExID().tostring(srcGoodsGUID);					
				_snprintf(pszGoodsLog,1024,"[物品回收:CPlayer::UseItem(DWORD dwContainerId,DWORD dwPos)][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d]",
					pGoods->GetBasePropertiesIndex(),GoodsSetup::QueryGoodsName(pGoods->GetBasePropertiesIndex()),srcGoodsGUID,pGoods->GetAmount());
				CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
#endif
				CGoodsFactory::GarbageCollect( &pGoods ,29);
			}
			else
			{
				OutputDebugString("Some error occur, goods can't remove from m_cPacket.\n");
			}
		}
	}
	else
	{
		CS2CContainerObjectAmountChange coacMsg;
		coacMsg.SetSourceContainer( GetType(), GetExID(), dwPos );
		coacMsg.SetSourceContainerExtendID(dwContainerId);
		coacMsg.SetObject( pGoods -> GetType(), pGoods -> GetExID() );
		coacMsg.SetObjectAmount( pGoods -> GetAmount() );
		coacMsg.Send( GetExID() );
	}	
}

BOOL CPlayer::AutoUseItem(WORD wPlace,BYTE lPos,long lTargetType,CGUID& lTargetGuid)
{
	BOOL bReturnValue = FALSE;

	//有不能使用装备的状态
	if (!GetUseItem() || IsDied())
	{
		return bReturnValue;
	}
	CGoods* pGoods=NULL;
	CS2CContainerObjectMove::PLAYER_EXTEND_ID place=static_cast<CS2CContainerObjectMove::PLAYER_EXTEND_ID>(wPlace);
	CVolumeLimitGoodsContainer* pVolumeContainer=NULL;
	if(place==CS2CContainerObjectMove::PEI_PACKET)
	{
		pGoods=getOriginPack()->GetGoods(lPos);
		pVolumeContainer=getOriginPack();
		if(NULL != pGoods)
			if(pVolumeContainer->IsLocked(pGoods))
				return FALSE;
	}
	else if(place>=CS2CContainerObjectMove::PEI_PACK1 && place<=CS2CContainerObjectMove::PEI_PACK5)
	{
		pVolumeContainer=m_pPackage.GetSubpack(place-CS2CContainerObjectMove::PEI_PACK1)->pSubpackContainer;
		pGoods=pVolumeContainer->GetGoods(lPos);
		if(NULL != pGoods)
			if(pVolumeContainer->IsLocked(pGoods))
				return FALSE;
	}

	CServerRegion* pRegion = dynamic_cast<CServerRegion*>( GetFather() );
	if( pGoods && pRegion )
	{
		if(lTargetGuid != pGoods->GetExID())
		{
			return FALSE;
		}

		if(IsInPersonalShop(pGoods))
		{
			return FALSE;
		}

		CGoodsBaseProperties* pProperties = GoodsSetup::QueryGoodsBaseProperties(
			pGoods -> GetBasePropertiesIndex() );
		if( pProperties && pProperties -> GetGoodsBaseType() == GBT_CONSUMABLE )
		{
			BOOL bCanUse = TRUE;

			//判断该场景是否支持使用该物品
			EquipRestrict eRet = EQUIPFIT;
			if( pRegion->FindForbidGood(pProperties->GetOriginalName()) == true)
			{
				eRet = REGION_FORBID;
			}

			if( eRet==EQUIPFIT )
			{
				eRet = (EquipRestrict)CanUseItem( pGoods );
			}

			if(eRet!=EQUIPFIT)
			{
				bCanUse = FALSE;
			}

			if( bCanUse && pGoods -> GetAmount() > 0 )
			{
				BOOL bItemUsed		= TRUE;
				BOOL bBackToCity	= FALSE;
				vector<eGoodsAddonProperties> vOut;
				pGoods -> GetEnabledAddonProperties( vOut );

				//如果是使用坐骑的物品,则不进入循环
				if( pGoods -> GetAddonPropertyValues(GAP_MOUNT_TYPE, 1) )
				{				
						//##摆摊，战斗状态下不能骑坐骑
						if( GetCurrentProgress() != PROGRESS_OPEN_STALL && m_lFightStateCount == 0 )
						{
							const char *strGoodOriName = GoodsSetup::QueryGoodsOriginalName(pGoods->GetBasePropertiesIndex());
							if( Mount( pGoods -> GetAddonPropertyValues(GAP_MOUNT_TYPE, 1), 
								pGoods -> GetAddonPropertyValues(GAP_MOUNT_LEVEL, 1), 
								pGoods -> GetAddonPropertyValues(GAP_MOUNT_ROLE_LIMIT, 1),
								strGoodOriName) )
							{
								//##如果是无限制使用,则不减少数量
								if( pGoods -> GetAddonPropertyValues(GAP_UNLIMITED_ACCESS, 1) == 1 )
								{
									bItemUsed = FALSE;
								}
							}
							else
							{
								bItemUsed = FALSE;
							}
						}
						else
						{
							bItemUsed = FALSE;
						}
					
				}
				else
				{
					//! pGoods -> GetEnabledAddonProperties( vOut );
				}

				for( size_t i = 0; i < vOut.size(); i++ )
				{
					switch( vOut[i] )
					{
					case GAP_ADD_HP:
						{
							if( pGoods -> GetAddonPropertyValues(vOut[i], 2) == 0 )
							{
								bItemUsed = RestoreHp( pGoods -> GetAddonPropertyValues(vOut[i], 1), 0, 400 );

							}
							else
							{
								bItemUsed = RestoreHp( pGoods -> GetAddonPropertyValues(vOut[i], 1), 
									pGoods -> GetAddonPropertyValues(vOut[i], 2) , 400 );
							}
						}
						break;
					case GAP_ADD_PERCENT_HP:	//生命百分比增加
						{
							bItemUsed = RestoreHp( static_cast<DWORD>( GetMaxHP() * ( pGoods -> GetAddonPropertyValues(vOut[i], 1) / 100.0f) ),
								0, 400 );
						}
						break;
					case GAP_ADD_MP:	//法力增加
						{
							if(pGoods -> GetAddonPropertyValues( vOut[i], 2 )==0)
							{
								bItemUsed = RestoreMp( pGoods -> GetAddonPropertyValues(vOut[i], 1), 0, 400 );
							}
							else
							{
								bItemUsed = RestoreMp( pGoods -> GetAddonPropertyValues(vOut[i], 1), (pGoods -> GetAddonPropertyValues(vOut[i], 2) - 1) * 400, 400 );
							}
						}
						break;
					case GAP_ADD_PERCENT_MP:	//法力百分比增加
						{
							bItemUsed = RestoreMp( static_cast<DWORD>( GetMaxMP() * (pGoods -> GetAddonPropertyValues(vOut[i], 1) / 100.0f) ),
								0, 400 );
						}
						break;
					case GAP_HOME:	//回城
						{
							// 在他国领土不能使用回城
							if( bCanUse )
							{
								if( pRegion->GetCountry() && pRegion->GetCountry() != GetCountry() )
								{
									bItemUsed = FALSE;
									const char* strMsg=AppFrame::GetText("41");
									if(strMsg)
										SendSystemInfo(strMsg,0xFFFFFFFF);
								}
								else
								{
									bBackToCity = TRUE;
								}
							}
							else
								bItemUsed = FALSE;

						}
						break;
					case GAP_RAN_TRANS:	//随机传送
						{
							// 只能在本国内使用随机传送
							if( bCanUse )
							{
								if( pRegion->GetCountry() != GetCountry() )
								{
									bItemUsed = FALSE;
									const char* strMsg=AppFrame::GetText("43");
									if(strMsg)
										SendSystemInfo(strMsg,0xFFFFFFFF);
								}
								else
								{
									CServerRegion* pRegion=((CServerRegion*)GetFather());
									if(pRegion)
									{
										//##在使用回程卷，随机卷，npc传送的时候掉落物品
										DropParticularGoodsWhenRecall();

										long lX = -1, lY = -1;
										pRegion->GetRandomPos(lX, lY);

										// [071127 AHC REGION]
										ChangeRegion(GetCurRgnType(), GetRegionGUID(),lX, lY,GetDir());
									}
								}
							}
							else 
								bItemUsed = FALSE;
						}
						break;
					case GAP_SCRIPT:	//执行脚本
						{
							char strFile[50];
							_snprintf(strFile, 50, "scripts/Goods/%d.lua",pGoods -> GetAddonPropertyValues(vOut[i], 1));
							if( !m_VarList->HasVar( "#m_strUseItem" ) )
							{
								m_VarList->AddVar( "#m_strUseItem", tString("") );
							}
							m_VarList->SetVarValue( "#m_strUseItem", tString( pProperties->GetOriginalName() ) );

							Script::RunEnv env( this, NULL, static_cast<CRegion*>( GetFather() ), m_VarList );
							GetInst( ScriptSys ).RunScript( env, strFile );
						}
						break;
					case GAP_ADD_STATE:
						{
							DWORD dwStateId=pGoods -> GetAddonPropertyValues(vOut[i],1);
							DWORD dwLevel=pGoods -> GetAddonPropertyValues(vOut[i],2);	
							AddNewState(this,dwStateId,dwLevel,17);
						}
						break;
					case GAP_POWER_INTENSITY:
						{
							CGoods *pArm = m_cEquipment.GetGoods(EC_E_WEAPON);
							bItemUsed = GoodsExManage::GetInstance().ReinforcePower(pArm, pGoods, this);				
						}
						break;						
					}
				}

				if( bItemUsed )
				{
					//冷却时间
					DWORD dwGoodsId=pGoods->GetBasePropertiesIndex();
					UpdateCooldown(dwGoodsId);

					////////////////////////////////

					CMessage msg(MSG_S2C_PLAYER_USEITEM);
					msg.Add((char)PLAYER_USEITEM_SUCCEED);
					msg.Add(GetExID());
					msg.Add(pGoods->GetBasePropertiesIndex());
					msg.SendToAround( this );
					BOOL bAddSkill=pGoods->HasAddonProperty(GAP_ADD_SKILL);
					if(pGoods -> GetAddonPropertyValues(GAP_UNLIMITED_ACCESS, 1) ==0 && !bAddSkill)
					{


						pGoods -> SetAmount( pGoods -> GetAmount() - 1 );

						if( pGoods -> GetAmount() == 0 )
						{
							//消耗道具数量用完 删除							
							if(pVolumeContainer)
							{							
								CS2CContainerObjectMove comMsg;
								if(pVolumeContainer->Remove( pGoods, &comMsg ) )
								{
									comMsg.SetOperation( CS2CContainerObjectMove::OT_DELETE_OBJECT );
									comMsg.SetSourceContainerExtendID( wPlace);
									comMsg.Send( GetExID() );
#ifdef _GOODSLOG1_
									char pszGoodsLog[1024]="";			
									char srcGoodsGUID[50]="";
									pGoods->GetExID().tostring(srcGoodsGUID);					
									_snprintf(pszGoodsLog,1024,"[物品回收:AutoUseItem][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d]",
										pGoods->GetBasePropertiesIndex(),GoodsSetup::QueryGoodsName(pGoods->GetBasePropertiesIndex()),srcGoodsGUID,pGoods->GetAmount());
									CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
#endif
									CGoodsFactory::GarbageCollect( &pGoods,28 );
								}
								else
								{
									OutputDebugString("Some error occur, goods can't remove from m_cPacket.\n");
								}
							}
						}
						else
						{
							CS2CContainerObjectAmountChange coacMsg;
							coacMsg.SetSourceContainer( GetType(), GetExID(), lPos );
							coacMsg.SetSourceContainerExtendID(wPlace);
							coacMsg.SetObject( pGoods -> GetType(), pGoods -> GetExID() );
							coacMsg.SetObjectAmount( pGoods -> GetAmount() );
							coacMsg.Send( GetExID() );
						}
					}

					bReturnValue = TRUE;
				}
				else
				{
					//CMessage msg(MSG_S2C_PLAYER_USEITEM);
					//msg.Add((char)PLAYER_USEITEM_FAILED);
					//msg.Add((char)REGION_FORBID);
					//msg.SendToPlayer(GetExID());
				}

				if( bBackToCity )
				{
					//##在使用回程卷，随机卷，npc传送的时候掉落物品
					DropParticularGoodsWhenRecall();

					((CServerRegion*)GetFather())->BackToCity(this);
				}
			}
			else
			{
				if ( eRet == REGION_FORBID )
				{
					const char* strMsg=AppFrame::GetText("83");
					if(strMsg)
						SendNotifyMessage( strMsg, eNOTIFYPOS_CENTER );
				}

				CMessage msg(MSG_S2C_PLAYER_USEITEM);
				msg.Add((char)PLAYER_USEITEM_FAILED);
				msg.Add((char)eRet);
				msg.SendToPlayer(GetExID());
			}
		}
	}

	return bReturnValue;
}

BOOL CPlayer::Mount( DWORD dwMountType, DWORD dwMountLevel, DWORD dwRoleLimit,const char* strGoodName  )
{
	if(strGoodName == NULL)
		return FALSE;
	BOOL bResult = TRUE;

	if( m_lFightStateCount == 0 )
	{
		if( dwRoleLimit )
		{
			//##判断玩家的角色
			if( dwRoleLimit | PR_GUILD_MASTER )
			{
				//bResult = FALSE;
			}

			if( dwRoleLimit | PR_UNION_MASTER )
			{
				//bResult = FALSE;
			}

			if( dwRoleLimit | PR_CITY_MASTER )
			{
				//bResult = FALSE;
			}
		}

		if( bResult )
		{
			//  [6/15/2007 chenxianjin]
			//CNewState *pState = new CNewState(STATE_RIDE_HORSE);
			/*CRideState* pState = new CRideState( dwMountType, dwMountLevel, dwRoleLimit );
			pState->SetGoodName(strGoodName);
			if( pState && pState -> Begin( this, this ) )
			{
			m_vStates.push_back( pState );
			}
			else
			{
			SAFE_DELETE( pState );
			bResult = FALSE;
			}*/
		}
	}
	else 
	{
		const char* strRes=AppFrame::GetText("10");
		SendNotifyMessage( strRes);
	}

	return bResult;
}

//long CPlayer::GetCurBurden()
//{
//	long lWeight=0;
//	//lWeight += m_cEquipment.GetContentsWeight();
//	//lWeight += m_cPacket.GetContentsWeight();
//	//lWeight += m_cHand.GetContentsWeight();
//	return lWeight;
//}
//
//bool CPlayer::CheckBurden(CGoods* pGoods)
//{
//	/*if( pGoods -> GetWeight() + GetCurBurden() > GetBurden() )
//	{
//	return false;
//	}*/
//	return true;
//}

bool CPlayer::CanMountSubpack(CGoods* pGoods)
{
	if(!pGoods)
		return false;
	DWORD dwLevLimit=pGoods->GetAddonPropertyValues(GAP_ROLE_LEVEL_LIMIT,1);
	if(dwLevLimit>0 && GetLevel()<dwLevLimit)
	{
		//等级不足
		const char* pStrMsg = AppFrame::GetText("11");
		if(pStrMsg)
			SendNotifyMessage(pStrMsg);		
		return false;
	}
	return true;
}

long CPlayer::CanMountEquip(CGoods *pGoods)
{	
	if( pGoods )
	{
        if( pGoods->HasAddonProperty(GAP_GOODS_MAX_DURABILITY) )
        {
           LONG lCurrentDurability	= pGoods -> GetAddonPropertyValues( GAP_GOODS_MAX_DURABILITY, 2 );
           if ( lCurrentDurability <= 0 )
           {
                /// 装备耐久度不足，无法装备
               return  UNKNOW_NOTFIT;
           }
        }
        
		CGoodsBaseProperties* pProperty=GoodsSetup::QueryGoodsBaseProperties(pGoods->GetBasePropertiesIndex());
		if(!pProperty)
			return UNKNOW_NOTFIT;
		if(pProperty->GetGoodsBaseType()!=GBT_EQUIPMENT)
			return UNKNOW_NOTFIT;
		//装备各种限制判定
		DWORD dwVal=0;
		//等级
		dwVal=pGoods -> GetAddonPropertyValues( GAP_ROLE_LEVEL_LIMIT, 1 );
		if(dwVal!=0 && dwVal>GetLevel())
			return LEVEL_NOTFIT;

		dwVal=pGoods->GetAddonPropertyValues(GAP_SKILL_LIMIT,1);
		if(dwVal!=0)
		{
			DWORD dwLev=GetSkillLv(dwVal);
			if(dwLev<=0)
				return SKILL_NOTFIT;
		}

		//防具类型限制
		dwVal=pGoods->GetAddonPropertyValues(GAP_GUARD_EQUIP_CATEGORY,1);
		if(dwVal!=0)
		{
			eEquipLimitType elt=CGoodsFactory::QueryPlayerEquipLimitType(this);
			if(elt!=dwVal)
				return EQUIP_TYPE_NOTFIT;
		}

		//职业限制
		if(pGoods->HasAddonProperty(GAP_REQUIRE_OCCUPATION))
		{
			dwVal=pGoods -> GetAddonPropertyValues( GAP_REQUIRE_OCCUPATION, 1 );
			//if(dwVal!=0 && dwVal!=GetOccupation())
			//return OCCUPATION_NOTFIT;
			BYTE nPlayerOccu=GetOccupation();
			DWORD dwOccuMask=1;
			dwOccuMask=dwOccuMask<<nPlayerOccu;
			if(!(dwOccuMask & dwVal))
			{
				return OCCUPATION_NOTFIT;
			}
		}


		if(pGoods->HasAddonProperty(GAP_REQUIRE_GENDER))
		{
			dwVal=pGoods -> GetAddonPropertyValues( GAP_REQUIRE_GENDER, 1 );
			dwVal--;
			if(dwVal!=GetSex())
				return SEX_NOTFIT;
		}
		return EQUIPFIT;
	}
	return UNKNOW_NOTFIT;
}


void    CPlayer::SendEquipNotifyMsg(long lRet)
{
	const char* pStrMsg=NULL;
	if(lRet==CPlayer::LEVEL_NOTFIT)
	{
		pStrMsg=AppFrame::GetText("11");
		if(pStrMsg)
			//SendNotifyMessage(pStrMsg,NOTIFY_COLOR_RED);
			SendNotifyMessage(pStrMsg);
		//等级不够
	}
	else if(lRet==CPlayer::STR_NOTFIT)
	{
		pStrMsg=AppFrame::GetText("12");
		if(pStrMsg)
			//SendNotifyMessage(pStrMsg,NOTIFY_COLOR_RED);
			SendNotifyMessage(pStrMsg);
		//力量不够
	}
	else if(lRet==CPlayer::DEX_NOTFIT)
	{
		pStrMsg=AppFrame::GetText("13");
		if(pStrMsg)
			//SendNotifyMessage(pStrMsg,NOTIFY_COLOR_RED);
			SendNotifyMessage(pStrMsg);
		//敏捷不够
	}
	else if(lRet==CPlayer::CON_NOTFIT)
	{
		pStrMsg=AppFrame::GetText("14");
		if(pStrMsg)
			//SendNotifyMessage(pStrMsg,NOTIFY_COLOR_RED);
			SendNotifyMessage(pStrMsg);
		//体质不够
	}
	else if(lRet==CPlayer::INT_NOTFIT)
	{
		pStrMsg=AppFrame::GetText("15");
		if(pStrMsg)
			//SendNotifyMessage(pStrMsg,NOTIFY_COLOR_RED);
			SendNotifyMessage(pStrMsg);
		//灵力不够
	}
	else if(lRet==CPlayer::OCCUPATION_NOTFIT)
	{
		pStrMsg=AppFrame::GetText("16");
		if(pStrMsg)
			//SendNotifyMessage(pStrMsg,NOTIFY_COLOR_RED);
			SendNotifyMessage(pStrMsg);
		//职业不对
	}
	else if(lRet==CPlayer::SEX_NOTFIT)
	{
		pStrMsg=AppFrame::GetText("17");
		if(pStrMsg)
			//SendNotifyMessage(pStrMsg,NOTIFY_COLOR_RED);
			SendNotifyMessage(pStrMsg);
		//性别不对
	}
	else if(lRet==CPlayer::SKILL_NOTFIT)
	{
		pStrMsg=AppFrame::GetText("18");
		if(pStrMsg)
			//SendNotifyMessage(pStrMsg,NOTIFY_COLOR_RED);
			SendNotifyMessage(pStrMsg);
		//技能不对
	}
	else if(lRet==CPlayer::REGION_FORBID)
	{
		//场景禁止
		pStrMsg=AppFrame::GetText("19");
		if(pStrMsg)
			/*SendNotifyMessage(pStrMsg,NOTIFY_COLOR_RED);*/
			SendNotifyMessage(pStrMsg);
	}
	else if(lRet==CPlayer::COOLDOWN_NOTFIT)
	{
		//冷却不对
		pStrMsg=AppFrame::GetText("21");
		if(pStrMsg)
			//SendNotifyMessage(pStrMsg,NOTIFY_COLOR_RED);
			SendNotifyMessage(pStrMsg);
	}
	else if(lRet==CPlayer::EQUIP_TYPE_NOTFIT)
	{
		//防具类型不对
		pStrMsg=AppFrame::GetText("22");
		if(pStrMsg)
			//SendNotifyMessage(pStrMsg,NOTIFY_COLOR_RED);
			SendNotifyMessage(pStrMsg);
	}
}

long CPlayer::CanMountEquip(CGoods *pGoods,DWORD dwPos,BOOL bMsgNotify)
{
	long lRet=CanMountEquip(pGoods);
	if(lRet==EQUIPFIT)
	{
		//对栏位进行检测
		eEquipType ecColumn = static_cast<eEquipType>( dwPos);
		DWORD dwEcVal = pGoods->GetEquipType();

		if(ecColumn==dwEcVal)
		{
			return EQUIPFIT;
		}
		else
		{
			//栏位不对
			return UNKNOW_NOTFIT;
		}
	}
	else
	{			
		if(bMsgNotify)
		{
			SendEquipNotifyMsg(lRet);
		}
	}
	return lRet;
}

long CPlayer::CanUseItem(CGoods* pGoods)
{
	if( pGoods )
	{
		DWORD dwVal=0;
		dwVal= pGoods -> GetAddonPropertyValues( GAP_ROLE_LEVEL_LIMIT, 1 );
		if(dwVal!=0 && dwVal>GetLevel())
			return LEVEL_NOTFIT;

		dwVal= pGoods -> GetAddonPropertyValues( GAP_REQUIRE_OCCUPATION, 1 );
		if(dwVal!=0 && dwVal!=GetOccupation()+1)
			return OCCUPATION_NOTFIT;

		dwVal= pGoods -> GetAddonPropertyValues( GAP_REQUIRE_GENDER, 1 );
		if(dwVal!=0 && dwVal!=GetSex())
			return SEX_NOTFIT;

		//冷却检测
		DWORD dwGoodsId=pGoods->GetBasePropertiesIndex();
		DWORD dwCoolDownTime=pGoods->GetAddonPropertyValues(GAP_COOLDOWN,1);
		DWORD dwCoolDownId=pGoods->GetAddonPropertyValues(GAP_COOLDOWN,2);
		if(dwCoolDownId!=0)
		{
			//有公用冷却限制
			map<DWORD,tagGoodsCommonCooldown>::iterator iter=m_mapClassIdCoolDown.find(dwCoolDownId);
			if(iter!=m_mapClassIdCoolDown.end())
			{
				if(timeGetTime() - iter->second.dwCooldownStartTime<=iter->second.dwCooldownTime/*dwCoolDownTime*/)				
				{
					return COOLDOWN_NOTFIT;
				}
				else
				{
				}
			}
		}
		else if(dwCoolDownTime!=0)
		{
			//GOODSID冷却限制
			map<DWORD,tagGoodsCommonCooldown>::iterator iter=m_mapGoodsIdCoolDown.find(dwGoodsId);
			if(iter!=m_mapGoodsIdCoolDown.end())
			{
				if(timeGetTime()-iter->second.dwCooldownStartTime<=iter->second.dwCooldownTime)
				{
					return COOLDOWN_NOTFIT;
				}
			}
		}


		return EQUIPFIT;
	}
	return UNKNOW_NOTFIT;
}

//更新HPMP恢复速度
void CPlayer::UpdateHpMpRecoverSpeed()
{
	string strBHpSpeed = "B_HpRecoverSpeed";
	string strBMpSpeed = "B_MpRecoverSpeed";
	if (GetState()==STATE_FIGHT)
	{
		SetAttribute(strBHpSpeed,0);
		SetAttribute(strBMpSpeed,0);
	}

}
//改变状态
void CPlayer::ChangeStateTo(CShape::eSTATE state)
{
	if (state==STATE_FIGHT || state==STATE_PEACE)
	{
		string strConsti = "C_Consti";
		string strSpirit = "C_Spirit";
		string strOccu	 = "Occu";
		string strBHpSpeed = "B_HpRecoverSpeed";
		string strBMpSpeed = "B_MpRecoverSpeed";
		string strLv       = "Level";

		long lTotalConsti = GetAttribute(strConsti);
		long lTotalSpiri = GetAttribute(strSpirit);
		long lLv		 = GetAttribute(strLv);

		float fTemp = pow((float)(1.0/lLv),(float)0.1);
		//Hp恢复速度值
		double dbHpRecoverSpeed =  (double)((((lTotalConsti*(1-lLv/220) + lTotalConsti)*(3-lLv/200))/5)*2)*fTemp;//GetAttribute(strBHpSpeed);//
		//Mp恢复速度值
		double dbMpRecoverSpeed =  (double)((((lTotalSpiri*(1-lLv/220) + lTotalSpiri)*(3-lLv/200))/5)*2)*fTemp;//GetAttribute(strBMpSpeed); //

		if (state==STATE_FIGHT)
		{
			dbHpRecoverSpeed = 0;//dbHpRecoverSpeed/5;
			dbMpRecoverSpeed = 0;//dbMpRecoverSpeed/5;
			SetAttribute(strBHpSpeed, (long)dbHpRecoverSpeed);
			SetAttribute(strBMpSpeed, (long)dbMpRecoverSpeed);
		}
		else if (state==STATE_PEACE)
		{			
			SetAttribute(strBHpSpeed, (long)dbHpRecoverSpeed);
			SetAttribute(strBMpSpeed, (long)dbMpRecoverSpeed);

		}
		
		UpdateAttribute(1);
	}


	CMoveShape::ChangeStateTo(state);

	//if (state==STATE_FIGHT || state==STATE_PEACE)
	//{
	//	UpdateEnergyTimer();
	//}
	
	ChangeActivePetState(state);
	
}
//////////////////////////////////////////////////////////////////////////
//	变量名和地址的映射表
//////////////////////////////////////////////////////////////////////////

// 初始化
void CPlayer::InitNameValueMap()
{
	CMoveShape::InitNameValueMap();

	m_mapNameValue.clear();

	tagPty st;// 0: byte 1:WORD 2:DWORD	3:float 4:long 5:short 6:bool
	char tmp[128];
	st.SetType(4);  st.pValue   = &m_lType;				st.attrEnumValue=P_ATTR_TYPE;       m_mapNameValue["Type"]=st;			
	st.SetType(4);  st.pValue   = &m_lGraphicsID;		st.attrEnumValue=P_ATTR_GRAPHICSID;	m_mapNameValue["GraphicsID"]=st;	
	st.SetType(4);  st.pValue   = &m_lRegionID;			st.attrEnumValue=P_ATTR_REGIONID;	m_mapNameValue["RegionID"]=st;
	st.SetType(3);	st.pfValue  = &m_fPosX;			    st.attrEnumValue=P_ATTR_TILEX;		m_mapNameValue["TileX"]=st;		
	st.SetType(3);	st.pfValue  = &m_fPosY;			    st.attrEnumValue=P_ATTR_TILEY;		m_mapNameValue["TileY"]=st;
	st.SetType(4);	st.pfValue  = &m_fDir;			    st.attrEnumValue=P_ATTR_DIR;		m_mapNameValue["Dir"]=st;			
	st.SetType(4);	st.pValue   = &m_lPos;				st.attrEnumValue=P_ATTR_POS;		m_mapNameValue["Pos"]=st;			
	st.SetType(1);	st.pwValue  = &m_wState;			st.attrEnumValue=P_ATTR_STATE;		m_mapNameValue["State"]=st;			
	st.SetType(1);	st.pwValue  = &m_wAction;			st.attrEnumValue=P_ATTR_ACTION;		m_mapNameValue["Action"]=st;		


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*人物属性*/
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	st.SetType(0);	st.plValue  = (BYTE*)(&m_Property.byOccu);				st.attrEnumValue=P_ATTR_OCCU;				m_mapNameValue["Occu"]=st;		
	st.SetType(0);	st.plValue  = (BYTE*)(&m_Property.byAssOccu);			st.attrEnumValue=P_ATTR_DOCCU;				m_mapNameValue["DOccu"]=st;
	st.SetType(0);	st.plValue	= &m_Property.bySex;						st.attrEnumValue=P_ATTR_SEX;				m_mapNameValue["Sex"]=st;					
	st.SetType(0);	st.plValue	= &m_Property.byHairPic;					st.attrEnumValue=P_ATTR_HEADPIC;			m_mapNameValue["HeadPic"]=st;				
	st.SetType(0);	st.plValue	= &m_Property.byFacePic;					st.attrEnumValue=P_ATTR_FACEPIC;			m_mapNameValue["FacePic"]=st;				
	st.SetType(1);	st.pdwValue	= &m_Property.dwHp;							st.attrEnumValue=P_ATTR_HP;					m_mapNameValue["Hp"]=st;				
	st.SetType(1);	st.pwValue	= &m_Property.wMp;							st.attrEnumValue=P_ATTR_MP;					m_mapNameValue["Mp"]=st;				
	st.SetType(1);	st.pwValue  = &m_Property.wEnergy;						st.attrEnumValue=P_ATTR_ENERGY;				m_mapNameValue["Energy"]=st;			
	st.SetType(1);  st.pwValue  = &m_Property.wStamina;						st.attrEnumValue=P_ATTR_STAMINA;			m_mapNameValue["Stamina"]=st;				
	st.SetType(1);	st.pwValue	= &m_Property.wLevel;						st.attrEnumValue=P_ATTR_LEVEL;				m_mapNameValue["Level"]=st;				
	st.SetType(2);	st.pdwValue	= &m_Property.dwExp;						st.attrEnumValue=P_ATTR_EXP;				m_mapNameValue["Exp"]=st;
	st.SetType(2);	st.pdwValue	= &m_Property.dwExpMultiple;				st.attrEnumValue=P_ATTR_EXPMULTIPLE;		m_mapNameValue["ExpMultiple"]=st;
	st.SetType(2);	st.pdwValue	= &m_Property.dwPresentExp;					st.attrEnumValue=P_ATTR_PRESENTEXP;			m_mapNameValue["PresentExp"]=st;
    st.SetType(4);  st.pValue   = &m_Buddhism.lDistract;                    st.attrEnumValue=P_ATTR_BUDDHISM;           m_mapNameValue["lDistract"]=st;
    st.SetType(4);  st.pValue   = &m_Buddhism.lDryNum;                      st.attrEnumValue=P_ATTR_DRYNUM;             m_mapNameValue["lDryNum"]=st;
    st.SetType(4);  st.pValue   = &m_Buddhism.lDryEndTime;                  st.attrEnumValue=P_ATTR_DRYENDTIME;         m_mapNameValue["lDryEndTime"]=st;

	//职业等级数据
	for(int i=0;i<OCC_Max;i++)
	{
		_snprintf(tmp, 128, "OccuLvl%d", i);
		st.SetType(0);	st.plValue	= (BYTE*)(&m_Property.byOccuLvl[i]);	st.attrEnumValue=P_ATTRIBUTE_TYPE(P_ATTR_OCCULVL0+i);	m_mapNameValue[tmp]=st;
	}
	//职业经验数据
	for(int i=0;i<OCC_Max;i++)
	{
		_snprintf(tmp, 128, "OccuExp%d", i);
		st.SetType(2);	st.pdwValue	= &m_Property.dwOccuExp[i];				st.attrEnumValue=P_ATTRIBUTE_TYPE(P_ATTR_OCCUEXP0+i);	m_mapNameValue[tmp]=st;
	}
	st.SetType(2);	st.pdwValue	= &m_Property.dwCurOccuUpgradeExp;			st.attrEnumValue=P_ATTR_CUROCCUUPGRADEEXP;	m_mapNameValue["OccuUpgradeExp"]=st;

    st.SetType(1); st.pwValue  = &m_Property.wTaoismLvl;                     st.attrEnumValue=P_ATTR_TAOISMLVL;          m_mapNameValue["TaoismLvl"]=st;
    st.SetType(2); st.pdwValue = &m_Property.dwTaoismExp;                    st.attrEnumValue=P_ATTR_TAOISMEXP;          m_mapNameValue["TaoismExp"]=st;
    st.SetType(2); st.pdwValue = &m_Property.dwRelationVal;                  st.attrEnumValue=P_ATTR_RELATIONVAL;        m_mapNameValue["RelationVal"]=st;
    st.SetType(2); st.pdwValue = &m_Property.dwHiddenVirtue;                 st.attrEnumValue=P_ATTR_HIDDENVIRTUE;       m_mapNameValue["HiddenVirtue"]=st;
    st.SetType(1); st.pwValue  = &m_Property.wVigour;                        st.attrEnumValue=P_ATTR_VIGOUR;             m_mapNameValue["Vigour"]=st;
    st.SetType(1); st.pwValue  = &m_Property.wThew;                          st.attrEnumValue=P_ATTR_THEW;               m_mapNameValue["Thew"]=st;

	st.SetType(0);	st.plValue  = &m_Property.byCountry;					st.attrEnumValue=P_ATTR_COUNTRY;			m_mapNameValue["Country"]=st;				
	st.SetType(4);	st.pValue   = &m_Property.lCountryContribute;				st.attrEnumValue=P_ATTR_COUNTRYCONTRIBUTE;	m_mapNameValue["CountryContribute"]=st;	
	st.SetType(2);	st.pdwValue = &m_Property.dwSpouseID;					st.attrEnumValue=P_ATTR_SPOUSEID;			m_mapNameValue["SpouseId"]=st;			
	st.SetType(4);	st.pdwValue = &m_Property.dwSpouseParam;				st.attrEnumValue=P_ATTR_SPOUSEPARAM;		m_mapNameValue["SpouseParam"]=st;		
	st.SetType(0); st.plValue   = &m_Property.byBusinessLevel;				st.attrEnumValue=P_ATTR_BUSINESSLEVEL;		m_mapNameValue["BusinessLevel"]=st;		
	st.SetType(2); st.pdwValue  = &m_Property.dwBusinessExp;				st.attrEnumValue=P_ATTR_BUSINESSEXP;		m_mapNameValue["BusinessExp"]=st;
	st.SetType(2);	st.pdwValue	= &m_Property.dwPkValue;					st.attrEnumValue=P_ATTR_PKVALUE;			m_mapNameValue["PkValue"]=st;				
	st.SetType(2);	st.pdwValue	= &m_Property.dwPkCount;					st.attrEnumValue=P_ATTR_PKCOUNT;			m_mapNameValue["PkCount"]=st;			
	st.SetType(2);	st.pdwValue	= &m_Property.dwPVPCount;					st.attrEnumValue=P_ATTR_PVPCOUNT;			m_mapNameValue["PVPCount"]=st;			
	st.SetType(2);	st.pdwValue	= &m_Property.dwPVPValue;					st.attrEnumValue=P_ATTR_PVPVALUE;			m_mapNameValue["PVPValue"]=st;			
	st.SetType(0);	st.plValue  = &m_Property.byPKOnOff;  					st.attrEnumValue=P_ATTR_PKONOFF;			m_mapNameValue["PkOnOff"]=st;			
	st.SetType(4);	st.pValue   = &m_DOccuRecord.m_lFactureExp ;			st.attrEnumValue=P_ATTR_FACTUREEXP;			m_mapNameValue["dwFactureExp"]=st;						//	制作经验
	st.SetType(4);	st.pValue   = &m_DOccuRecord.m_lCollectionExp;			st.attrEnumValue=P_ATTR_COLLECTIONEXP;		m_mapNameValue["dwCollectionExp"]=st;					//	采集经验
	st.SetType(4);	st.pValue   = &m_DOccuRecord.m_lFactureGrade ;			st.attrEnumValue=P_ATTR_FACTUREGRADE;		m_mapNameValue["dwFactureGrade"]=st;					//	制作等级
	st.SetType(4);	st.pValue   = &m_DOccuRecord.m_lCollectionGrade;		st.attrEnumValue=P_ATTR_COLLECTIONGRADE;	m_mapNameValue["dwCollectionGrade"]=st;				//	采集等级
	//职业SP数据
	for(int i=0;i<OCC_Max;i++)
	{
		_snprintf(tmp, 128, "OccuSP%d", i);
		st.SetType(2);	st.pdwValue	= &m_Property.dwOccuSP[i];				st.attrEnumValue=P_ATTRIBUTE_TYPE(P_ATTR_OCCUSP0+i);	m_mapNameValue[tmp]=st;
	}
	st.SetType(2); st.pdwValue = &m_Property.dwUpgradeExp;					st.attrEnumValue=P_ATTR_UPGRADEEXP;			m_mapNameValue["UpgradeExp"]=st;		
	st.SetType(6); st.pdwValue = (DWORD*)&m_Property.bShowCountry;					st.attrEnumValue=P_ATTR_SHOWCOUNTRY;		m_mapNameValue["ShowCountry"]=st;
	//	st.SetType(0); st.plValue  = &m_Property.byVisibility;				m_mapNameValue["byVisibility"]=st;				m_mapAttrType["byVisibility"]=P_ATTR_VISIBILITY;			
	for(int i=0; i<24; ++i)
	{
		_snprintf(tmp, 128, "dwHotKey%d", i);
		st.SetType(2);	st.pdwValue	= &m_Property.dwHotKey[i];		st.attrEnumValue=P_ATTRIBUTE_TYPE(P_ATTR_HOTKEY0+i);	m_mapNameValue[tmp]=st;			

	}

	st.SetType(6);	st.pbValue  = &m_bGMInvincible;		m_mapNameValue["GMInvincible"]=st;;	//GM无敌
	st.SetType(6);	st.pbValue  = &m_bGMConcealment;	st.attrEnumValue=P_ATTR_CONCEALMENT;	m_mapNameValue["GMConcealment"]=st;	//GM隐蔽

	st.lType = 4; st.pValue = &m_Property.lCurPKCP;     st.attrEnumValue = P_ATTR_CURPKCP; m_mapNameValue["CurPKCP"] = st;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*战斗属性*/ // B_:基本值, C_:当前值, E_:变化值
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////							

	st.SetType(4);	st.pValue	= &m_FightProperty.dwMaxHp;					st.attrEnumValue=P_ATTR_MAXHP;			   	m_mapNameValue["E_MaxHp"]=st;			
	st.SetType(5);	st.psValue	= &m_FightProperty.wMaxMp;					st.attrEnumValue=P_ATTR_MAXMP;				m_mapNameValue["E_MaxMp"]=st;			
	st.SetType(5);	st.psValue	= &m_FightProperty.wMaxStamina;				st.attrEnumValue=P_ATTR_MAXSTAMINA;			m_mapNameValue["E_MaxStamina"]=st;		
	st.SetType(5);	st.psValue	= &m_FightProperty.wMaxEnergy;				st.attrEnumValue=P_ATTR_MAXENERGY;			m_mapNameValue["E_MaxEnergy"]=st;		
	st.SetType(5);	st.psValue	= &m_FightProperty.wHpRecoverSpeed;			st.attrEnumValue=P_ATTR_HPRECOVERSPEED;		m_mapNameValue["E_HpRecoverSpeed"]=st;	
	st.SetType(5);	st.psValue	= &m_FightProperty.wMpRecoverSpeed;			st.attrEnumValue=P_ATTR_MPRECOVERSPEED;		m_mapNameValue["E_MpRecoverSpeed"]=st;	
    st.SetType(5);	st.psValue	= &m_FightProperty.wHpRecoverRatio;			st.attrEnumValue=P_ATTR_HPRECOVERRATIO;     m_mapNameValue["E_HpRecoverRatio"]=st;							
    st.SetType(5);	st.psValue	= &m_FightProperty.wEnerRecoverSpeed;		st.attrEnumValue=P_ATTR_ENERRECOVERSPEED;   m_mapNameValue["E_EnerRecoverSpeed"]=st;							
	st.SetType(5);	st.psValue	= &m_FightProperty.wStamRecoverSpeed;		st.attrEnumValue=P_ATTR_STAMRECOVERSPEED;	m_mapNameValue["E_StamRecoverSpeed"]=st;	
    st.SetType(5);  st.psValue  = &m_FightProperty.wFightHpRecoverSpeed;    st.attrEnumValue=P_ATTR_FIGHTHPRECOVERSPEED;m_mapNameValue["E_FightHpRecoverSpeed"]=st;
    st.SetType(5);  st.psValue  = &m_FightProperty.wFightMpRecoverSpeed;    st.attrEnumValue=P_ATTR_FIGHTMPRECOVERSPEED;m_mapNameValue["E_FightMpRecoverSpeed"]=st;
    st.SetType(5);	st.psValue	= &m_FightProperty.wFightEnerRecoverSpeed;	st.attrEnumValue=P_ATTR_FIGHTENERRECOVERSPEED;	m_mapNameValue["E_FightEnerRecoverSpeed"]=st;				
    st.SetType(5);  st.psValue  = &m_FightProperty.wFightStamRecoverSpeed;  st.attrEnumValue=P_ATTR_FIGHTSTAMRECOVERSPEED;m_mapNameValue["E_FightStamRecoverSpeed"]=st;
    
    st.SetType(5);  st.psValue  = &m_FightProperty.wManly;                  st.attrEnumValue=P_ATTR_MANLY;                  m_mapNameValue["E_Manly"]=st;
    st.SetType(5);  st.psValue  = &m_FightProperty.wPerverseTrend;          st.attrEnumValue=P_ATTR_PERVERSETREND;          m_mapNameValue["E_PerverseTrend"]=st;
    st.SetType(5);  st.psValue  = &m_FightProperty.wComplaint;              st.attrEnumValue=P_ATTR_COMPLAINT;              m_mapNameValue["E_Complaint"]=st;
    st.SetType(5);  st.psValue  = &m_FightProperty.wMurderousLook;          st.attrEnumValue=P_ATTR_MURDEROUSLOOK;          m_mapNameValue["E_MurderousLook"]=st;

    st.SetType(5);  st.psValue = &m_FightProperty.wStr;                     st.attrEnumValue=P_ATTR_STR;                    m_mapNameValue["E_Str"]=st;
    st.SetType(5);  st.psValue = &m_FightProperty.wDex;                     st.attrEnumValue=P_ATTR_DEX;                    m_mapNameValue["E_DEX"]=st;
    st.SetType(5);  st.psValue = &m_FightProperty.wCon;                     st.attrEnumValue=P_ATTR_CON;                    m_mapNameValue["E_Con"]=st;
    st.SetType(5);  st.psValue = &m_FightProperty.wIntellect;               st.attrEnumValue=P_ATTR_INTELLECT;              m_mapNameValue["E_Intellect"]=st;
    st.SetType(5);  st.psValue = &m_FightProperty.wSpiritualism;            st.attrEnumValue=P_ATTR_SPIRITUALISM;           m_mapNameValue["E_Spiritualism"]=st;
    st.SetType(5);  st.psValue = &m_FightProperty.wPoint;                   st.attrEnumValue=P_ATTR_POINT;                  m_mapNameValue["E_Point"]=st;

    st.SetType(5);	st.psValue = &m_FightProperty.wParry;					st.attrEnumValue=P_ATTR_PARRY;				m_mapNameValue["E_Parry"]=st;			
    st.SetType(5);	st.psValue = &m_FightProperty.wDodge;					st.attrEnumValue=P_ATTR_DODGE;				m_mapNameValue["E_Dodge"]=st;			
    st.SetType(5);	st.psValue = &m_FightProperty.wDef;					    st.attrEnumValue=P_ATTR_DEF;			    m_mapNameValue["E_Def"]=st;	
    st.SetType(5);  st.psValue = &m_FightProperty.HurtFactor;               st.attrEnumValue=P_ATTR_HURTFACTOR;         m_mapNameValue["E_HurtFactor"]=st;
//    st.SetType(5);	st.psValue = &m_FightProperty.wDefSuck;                 st.attrEnumValue=P_ATTR_DEFSUCK;            m_mapNameValue["E_DefSuck"]=st;
    st.SetType(5);	st.psValue = &m_FightProperty.wMdef;					st.attrEnumValue=P_ATTR_MDEF;			    m_mapNameValue["E_Mdef"]=st;	
    st.SetType(5);  st.psValue = &m_FightProperty.wMdefSuck;                st.attrEnumValue=P_ATTR_MDEFSUCK;           m_mapNameValue["E_MdefSuck"]=st;
    st.SetType(5);	st.psValue = &m_FightProperty.wBloodSuck;				st.attrEnumValue=P_ATTR_BLOODSUCK;		    m_mapNameValue["E_BloodSuck"]=st;			
    st.SetType(5);	st.psValue = &m_FightProperty.wManaSuck;				st.attrEnumValue=P_ATTR_MANASUCK;		    m_mapNameValue["E_ManaSuck"]=st;			
 //   st.SetType(5);	st.psValue = &m_FightProperty.wEnergySuck;              st.attrEnumValue=P_ATTR_ENERGYSUCK;         m_mapNameValue["E_EnergySuck"]=st;
    st.SetType(5);	st.psValue = &m_FightProperty.wStaminaSuck;             st.attrEnumValue=P_ATTR_STAMINASUCK;        m_mapNameValue["E_StaminaSuck"]=st;

    st.SetType(5);	st.psValue = &m_FightProperty.wHit;						st.attrEnumValue=P_ATTR_HIT;			    m_mapNameValue["E_Hit"]=st;					
    st.SetType(5);  st.psValue = &m_FightProperty.wAtkSpeed;                st.attrEnumValue=P_ATTR_ATKSPEED;           m_mapNameValue["E_AtkSpeed"]=st;
    st.SetType(5);	st.psValue = &m_FightProperty.wMAtkSpeed;				st.attrEnumValue=P_ATTR_MATKSPEED;		    m_mapNameValue["E_MAtkSpeed"]=st;		
    st.SetType(5);	st.psValue = &m_FightProperty.wMinAtk;					st.attrEnumValue=P_ATTR_MINATK;			    m_mapNameValue["E_MinAtk"]=st;			
    st.SetType(5);	st.psValue = &m_FightProperty.wMaxAtk;					st.attrEnumValue=P_ATTR_MAXATK;			    m_mapNameValue["E_MaxAtk"]=st;			
    st.SetType(5);	st.psValue = &m_FightProperty.wMinMAtk;				st.attrEnumValue=P_ATTR_MINMATK;			m_mapNameValue["E_MinMAtk"]=st;			
    st.SetType(5);	st.psValue = &m_FightProperty.wMaxMAtk;				st.attrEnumValue=P_ATTR_MAXMATK;			m_mapNameValue["E_MaxMAtk"]=st;			
    st.SetType(5);  st.psValue = &m_FightProperty.wPhysicImmunity;      st.attrEnumValue=P_ATTR_PHYSICIMMUNITY;     m_mapNameValue["E_PhysicImmunity"]=st;
    st.SetType(5);  st.psValue = &m_FightProperty.wMagicImmunity;       st.attrEnumValue=P_ATTR_MAGICIMMUNITY;      m_mapNameValue["E_MagicImmunity"]=st;


	st.SetType(5);	st.psValue	= &m_FightProperty.wFightBlock;				st.attrEnumValue=P_ATTR_BLOCK;				m_mapNameValue["E_Block"]=st;			

	st.SetType(5);	st.psValue	= &m_FightProperty.wCharm;						st.attrEnumValue=P_ATTR_CHARM;			m_mapNameValue["E_Charm"]=st;			
	st.SetType(5);	st.psValue	= &m_FightProperty.wAbsorb;						st.attrEnumValue=P_ATTR_ABSORB;			m_mapNameValue["E_Absorb"]=st;			
	st.SetType(5);	st.psValue	= &m_FightProperty.wShieldDef;					st.attrEnumValue=P_ATTR_SHIELD_DEF;		m_mapNameValue["E_ShieldDef"]=st;
	st.SetType(4);	st.psValue	= &m_FightProperty.wCri;						st.attrEnumValue=P_ATTR_CRI;			m_mapNameValue["E_Cri"]=st;					
	st.SetType(5);	st.psValue	= &m_FightProperty.wBuffPower;					st.attrEnumValue=P_ATTR_BUFFPOWER;		m_mapNameValue["E_BuffPower"]=st;		
	st.SetType(4);	st.pValue	= &m_FightProperty.wMAtk;						st.attrEnumValue=P_ATTR_MATK;			m_mapNameValue["E_MAtk"]=st;				
	st.SetType(4);	st.pValue	= &m_FightProperty.wMCri;						st.attrEnumValue=P_ATTR_MCRI;			m_mapNameValue["E_MCri"]=st;				
	st.SetType(5);	st.psValue	= &m_FightProperty.wWillDef;					st.attrEnumValue=P_ATTR_WILLDEF;		m_mapNameValue["E_WillDef"]=st;				
	st.SetType(5);	st.psValue	= &m_FightProperty.wConstiDef;					st.attrEnumValue=P_ATTR_CONSTIDEF;		m_mapNameValue["E_ConstiDef"]=st;		

	st.SetType(5);st.psValue = &m_FightProperty.wDeBuffDef[DFT_Bled];			st.attrEnumValue=P_ATTR_DEBUFFDEF_BLED;		 m_mapNameValue["E_DeBuffDef0"]=st;		
	st.SetType(5);st.psValue = &m_FightProperty.wDeBuffDef[DET_Dizzy];			st.attrEnumValue=P_ATTR_DEBUFFDEF_DIZZY;	 m_mapNameValue["E_DeBuffDef1"]=st;			
	st.SetType(5);st.psValue = &m_FightProperty.wDeBuffDef[DET_Lull];			st.attrEnumValue=P_ATTR_DEBUFFDEF_LULL;		 m_mapNameValue["E_DeBuffDef2"]=st;		
	st.SetType(5);st.psValue = &m_FightProperty.wDeBuffDef[DET_Hypnosis];		st.attrEnumValue=P_ATTR_DEBUFFDEF_HYPNOSIS;	 m_mapNameValue["E_DeBuffDef3"]=st;	
	st.SetType(5);st.psValue = &m_FightProperty.wDeBuffDef[DET_Poison];		st.attrEnumValue=P_ATTR_DEBUFFDEF_POSION;	 m_mapNameValue["E_DeBuffDef4"]=st;	
	st.SetType(5);st.psValue = &m_FightProperty.wDeBuffDef[DET_Fix];			st.attrEnumValue=P_ATTR_DEBUFFDEF_FIX;		 m_mapNameValue["E_DeBuffDef5"]=st;
	st.SetType(5);st.psValue = &m_FightProperty.wDeBuffDef[DET_Silence];		st.attrEnumValue=P_ATTR_DEBUFFDEF_SILENCE;	 m_mapNameValue["E_DeBuffDef6"]=st;

	st.SetType(5);st.psValue = &m_FightProperty.wElemDef[ET_Fire];			 st.attrEnumValue=P_ATTR_ELEMDEF_FIRE;    m_mapNameValue["E_ElemDef0"]=st;	
	st.SetType(5);st.psValue = &m_FightProperty.wElemDef[ET_Water];		 st.attrEnumValue=P_ATTR_ELEMDEF_WATER;	  m_mapNameValue["E_ElemDef1"]=st;	
	st.SetType(5);st.psValue = &m_FightProperty.wElemDef[ET_Earth];		 st.attrEnumValue=P_ATTR_ELEMDEF_EARTH;	  m_mapNameValue["E_ElemDef2"]=st;	
	st.SetType(5);st.psValue = &m_FightProperty.wElemDef[ET_Wind];			 st.attrEnumValue=P_ATTR_ELEMDEF_WIND;	  m_mapNameValue["E_ElemDef3"]=st;	
	st.SetType(5);st.psValue = &m_FightProperty.wElemDef[ET_Dark];			 st.attrEnumValue=P_ATTR_ELEMDEF_DARK;	  m_mapNameValue["E_ElemDef4"]=st;	
	st.SetType(5);st.psValue = &m_FightProperty.wElemDef[ET_Holy];			 st.attrEnumValue=P_ATTR_ELEMDEF_HOLY;	  m_mapNameValue["E_ElemDef5"]=st;	


	st.SetType(5);st.psValue = &m_FightProperty.wClassDam[C_Human];			st.attrEnumValue=P_ATTR_CLASSDAM_HUMAN;    m_mapNameValue["E_ClassDam0"]=st;		
	st.SetType(5);st.psValue = &m_FightProperty.wClassDam[C_Element];			st.attrEnumValue=P_ATTR_CLASSDAM_ELEMENT;   m_mapNameValue["E_ClassDam1"]=st;	
	st.SetType(5);st.psValue = &m_FightProperty.wClassDam[C_Alien];			st.attrEnumValue=P_ATTR_CLASSDAM_ALIEN;	   m_mapNameValue["E_ClassDam2"]=st;		
	st.SetType(5);st.psValue = &m_FightProperty.wClassDam[C_Animal];			st.attrEnumValue=P_ATTR_CLASSDAM_ANIMAL;   m_mapNameValue["E_ClassDam3"]=st;		
	st.SetType(5);st.psValue = &m_FightProperty.wClassDam[C_Undead];			st.attrEnumValue=P_ATTR_CLASSDAM_UNDEAD;   m_mapNameValue["E_ClassDam4"]=st;		
	st.SetType(5);st.psValue = &m_FightProperty.wClassDam[C_Evil];			    st.attrEnumValue=P_ATTR_CLASSDAM_EVIL;	   m_mapNameValue["E_ClassDam5"]=st;		
	st.SetType(5);st.psValue = &m_FightProperty.wClassDam[C_Hexapods];			st.attrEnumValue=P_ATTR_CLASSDAM_HEXAPODS;   m_mapNameValue["E_ClassDam6"]=st;	

	st.SetType(5);st.psValue = &m_FightProperty.wElemDam[ET_Fire];			    		st.attrEnumValue=P_ATTR_ELEMDAM_FIRE;	m_mapNameValue["E_ElemDam0"]=st;		
	st.SetType(5);st.psValue = &m_FightProperty.wElemDam[ET_Water];					st.attrEnumValue=P_ATTR_ELEMDAM_WATER;	m_mapNameValue["E_ElemDam1"]=st;	
	st.SetType(5);st.psValue = &m_FightProperty.wElemDam[ET_Earth];					st.attrEnumValue=P_ATTR_ELEMDAM_EARTH;	m_mapNameValue["E_ElemDam2"]=st;	
	st.SetType(5);st.psValue = &m_FightProperty.wElemDam[ET_Wind];			    		st.attrEnumValue=P_ATTR_ELEMDAM_WIND;	m_mapNameValue["E_ElemDam3"]=st;	
	st.SetType(5);st.psValue = &m_FightProperty.wElemDam[ET_Dark];			    		st.attrEnumValue=P_ATTR_ELEMDAM_DARK;	m_mapNameValue["E_ElemDam4"]=st;	
	st.SetType(5);st.psValue = &m_FightProperty.wElemDam[ET_Holy];			    		st.attrEnumValue=P_ATTR_ELEMDAM_HOLY;	m_mapNameValue["E_ElemDam5"]=st;	

	st.SetType(5);st.psValue = &m_FightProperty.wElemAtk[ET_Fire];			    		st.attrEnumValue=P_ATTR_ELEMATK_FIRE;	m_mapNameValue["E_ElemAtk0"]=st;		
	st.SetType(5);st.psValue = &m_FightProperty.wElemAtk[ET_Water];					st.attrEnumValue=P_ATTR_ELEMATK_WATER;	m_mapNameValue["E_ElemAtk1"]=st;	
	st.SetType(5);st.psValue = &m_FightProperty.wElemAtk[ET_Earth];					st.attrEnumValue=P_ATTR_ELEMATK_EARTH;	m_mapNameValue["E_ElemAtk2"]=st;	
	st.SetType(5);st.psValue = &m_FightProperty.wElemAtk[ET_Wind];			    		st.attrEnumValue=P_ATTR_ELEMATK_WIND;	m_mapNameValue["E_ElemAtk3"]=st;	
	st.SetType(5);st.psValue = &m_FightProperty.wElemAtk[ET_Dark];			    		st.attrEnumValue=P_ATTR_ELEMATK_DARK;	m_mapNameValue["E_ElemAtk4"]=st;	
	st.SetType(5);st.psValue = &m_FightProperty.wElemAtk[ET_Holy];			    		st.attrEnumValue=P_ATTR_ELEMATK_HOLY;	m_mapNameValue["E_ElemAtk5"]=st;	

	st.SetType(5);st.psValue = &m_FightProperty.wCriDef;		    st.attrEnumValue=P_ATTR_CRIDEF;	m_mapNameValue["E_CriDef"]=st;	
	st.SetType(5);st.psValue = &m_FightProperty.wMCriDef;		    st.attrEnumValue=P_ATTR_MCRIDEF;	m_mapNameValue["E_MCriDef"]=st;	
	st.SetType(5);st.psValue = &m_FightProperty.wCriDamage;		    st.attrEnumValue=P_ATTR_CRIDAMAGE;	m_mapNameValue["E_CriDamage"]=st;	
	st.SetType(5);st.psValue = &m_FightProperty.wMCriDamage;		st.attrEnumValue=P_ATTR_MCRIDAMAGE;	m_mapNameValue["E_MCriDamage"]=st;	

	st.SetType(5);	st.psValue	= &m_FightProperty.wImmunity;		st.attrEnumValue=P_ATTR_IMMUNITY;		m_mapNameValue["E_Immunity"]=st;				
	st.SetType(5);	st.psValue	= &m_FightProperty.wPierce;			st.attrEnumValue=P_ATTR_PIERCE;			m_mapNameValue["E_Pierce"]=st;
	st.SetType(5);	st.psValue	= &m_FightProperty.wLuck;			st.attrEnumValue=P_ATTR_LUCK;			m_mapNameValue["E_Luck"]=st;
	st.SetType(4);  st.pValue   = &m_FightProperty.lMaxPKCP;        st.attrEnumValue = P_ATTR_MAXPKCP;      m_mapNameValue["E_MaxPKCP"]=st;


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*基本战斗属性*/
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	st.attrEnumValue = 0;
    st.SetType(4);	st.pValue	= &m_BaseFightProperty.dwMaxHp;					m_mapNameValue["B_MaxHp"]=st;			
    st.SetType(5);	st.psValue	= &m_BaseFightProperty.wMaxMp;					m_mapNameValue["B_MaxMp"]=st;			
    st.SetType(5);	st.psValue	= &m_BaseFightProperty.wMaxStamina;				m_mapNameValue["B_MaxStamina"]=st;		
    st.SetType(5);	st.psValue	= &m_BaseFightProperty.wMaxEnergy;				m_mapNameValue["B_MaxEnergy"]=st;		
    st.SetType(5);	st.psValue	= &m_BaseFightProperty.wHpRecoverSpeed;			m_mapNameValue["B_HpRecoverSpeed"]=st;	
    st.SetType(5);	st.psValue	= &m_BaseFightProperty.wMpRecoverSpeed;			m_mapNameValue["B_MpRecoverSpeed"]=st;	
    st.SetType(5);	st.psValue	= &m_BaseFightProperty.wHpRecoverRatio;			m_mapNameValue["B_HpRecoverRatio"]=st;							
    st.SetType(5);	st.psValue	= &m_BaseFightProperty.wEnerRecoverSpeed;		m_mapNameValue["B_EnerRecoverSpeed"]=st;							
    st.SetType(5);	st.psValue	= &m_BaseFightProperty.wStamRecoverSpeed;		m_mapNameValue["B_StamRecoverSpeed"]=st;	
    st.SetType(5);  st.psValue  = &m_BaseFightProperty.wFightHpRecoverSpeed;    m_mapNameValue["B_FightHpRecoverSpeed"]=st;
    st.SetType(5);  st.psValue  = &m_BaseFightProperty.wFightMpRecoverSpeed;    m_mapNameValue["B_FightMpRecoverSpeed"]=st;
    st.SetType(5);	st.psValue	= &m_BaseFightProperty.wFightEnerRecoverSpeed;	m_mapNameValue["B_FightEnerRecoverSpeed"]=st;				
    st.SetType(5);  st.psValue  = &m_BaseFightProperty.wFightStamRecoverSpeed;  m_mapNameValue["B_FightStamRecoverSpeed"]=st;

    st.SetType(5);  st.psValue  = &m_BaseFightProperty.wManly;                  m_mapNameValue["B_Manly"]=st;
    st.SetType(5);  st.psValue  = &m_BaseFightProperty.wPerverseTrend;          m_mapNameValue["B_PerverseTrend"]=st;
    st.SetType(5);  st.psValue  = &m_BaseFightProperty.wComplaint;              m_mapNameValue["B_Complaint"]=st;
    st.SetType(5);  st.psValue  = &m_BaseFightProperty.wMurderousLook;          m_mapNameValue["B_MurderousLook"]=st;

    st.SetType(5);  st.psValue = &m_BaseFightProperty.wStr;                     m_mapNameValue["B_Str"]=st;
    st.SetType(5);  st.psValue = &m_BaseFightProperty.wDex;                     m_mapNameValue["B_DEX"]=st;
    st.SetType(5);  st.psValue = &m_BaseFightProperty.wCon;                     m_mapNameValue["B_Con"]=st;
    st.SetType(5);  st.psValue = &m_BaseFightProperty.wIntellect;               m_mapNameValue["B_Intellect"]=st;
    st.SetType(5);  st.psValue = &m_BaseFightProperty.wSpiritualism;            m_mapNameValue["B_Spiritualism"]=st;
    st.SetType(5);  st.psValue = &m_BaseFightProperty.wPoint;                   m_mapNameValue["B_Point"]=st;

    st.SetType(5);	st.psValue = &m_BaseFightProperty.wParry;				m_mapNameValue["B_Parry"]=st;			
    st.SetType(5);	st.psValue = &m_BaseFightProperty.wDodge;				m_mapNameValue["B_Dodge"]=st;			
    st.SetType(5);	st.psValue = &m_BaseFightProperty.wDef;					m_mapNameValue["B_Def"]=st;	
 //   st.SetType(5);	st.psValue = &m_BaseFightProperty.wDefSuck;             m_mapNameValue["B_DefSuck"]=st;
    st.SetType(5);	st.psValue = &m_BaseFightProperty.wMdef;				m_mapNameValue["B_Mdef"]=st;	
    st.SetType(5);  st.psValue = &m_BaseFightProperty.wMdefSuck;            m_mapNameValue["B_MdefSuck"]=st;
    st.SetType(5);	st.psValue = &m_BaseFightProperty.wBloodSuck;			m_mapNameValue["B_BloodSuck"]=st;			
    st.SetType(5);	st.psValue = &m_BaseFightProperty.wManaSuck;			m_mapNameValue["B_ManaSuck"]=st;			
 //   st.SetType(5);	st.psValue = &m_BaseFightProperty.wEnergySuck;          m_mapNameValue["B_EnergySuck"]=st;
    st.SetType(5);	st.psValue = &m_BaseFightProperty.wStaminaSuck;         m_mapNameValue["B_StaminaSuck"]=st;

    st.SetType(5);	st.psValue = &m_BaseFightProperty.wHit;					m_mapNameValue["B_Hit"]=st;					
    st.SetType(5);  st.psValue = &m_BaseFightProperty.wAtkSpeed;            m_mapNameValue["B_AtkSpeed"]=st;
    st.SetType(5);	st.psValue = &m_BaseFightProperty.wMAtkSpeed;			m_mapNameValue["B_MAtkSpeed"]=st;		
    st.SetType(5);	st.psValue = &m_BaseFightProperty.wMinAtk;				m_mapNameValue["B_MinAtk"]=st;			
    st.SetType(5);	st.psValue = &m_BaseFightProperty.wMaxAtk;				m_mapNameValue["B_MaxAtk"]=st;			
    st.SetType(5);	st.psValue = &m_BaseFightProperty.wMinMAtk;				m_mapNameValue["B_MinMAtk"]=st;			
    st.SetType(5);	st.psValue = &m_BaseFightProperty.wMaxMAtk;				m_mapNameValue["B_MaxMAtk"]=st;			
    st.SetType(5);  st.psValue = &m_BaseFightProperty.wPhysicImmunity;      m_mapNameValue["B_PhysicImmunity"]=st;
    st.SetType(5);  st.psValue = &m_BaseFightProperty.wMagicImmunity;       m_mapNameValue["B_MagicImmunity"]=st;

	st.SetType(5);	st.psValue	= &m_BaseFightProperty.wShieldDef;				m_mapNameValue["B_ShieldDef"]=st;							
	st.SetType(5);	st.psValue	= &m_BaseFightProperty.wImmunity;				m_mapNameValue["B_Immunity"]=st;				
	st.SetType(5);	st.psValue	= &m_BaseFightProperty.wPierce;					m_mapNameValue["B_Pierce"]=st;
	st.SetType(5);	st.psValue	= &m_BaseFightProperty.wLuck;					m_mapNameValue["B_Luck"]=st;

	st.SetType(5);	st.psValue	= &m_BaseFightProperty.wFightBlock;					m_mapNameValue["B_Block"]=st;				//格挡			
	st.SetType(5);	st.psValue	= &m_BaseFightProperty.wAbsorb;					m_mapNameValue["B_Absorb"]=st;				//专注			
	st.SetType(4);	st.psValue	= &m_BaseFightProperty.wCri;						m_mapNameValue["B_Cri"]=st;					//物理爆击			
	st.SetType(5);	st.psValue	= &m_BaseFightProperty.wBuffPower;				m_mapNameValue["B_BuffPower"]=st;			//Buff攻击强度		
	st.SetType(4);	st.pValue	= &m_BaseFightProperty.wMAtk;						m_mapNameValue["B_MAtk"]=st;					//魔法攻击力			
	st.SetType(4);	st.pValue	= &m_BaseFightProperty.wMCri;						m_mapNameValue["B_MCri"]=st;					//魔法爆击率			
	st.SetType(5);	st.psValue	= &m_BaseFightProperty.wWillDef;					m_mapNameValue["B_WillDef"]=st;				//意志抗性		
	st.SetType(5);	st.psValue	= &m_BaseFightProperty.wConstiDef;				m_mapNameValue["B_ConstiDef"]=st;			//体质抗性

	st.SetType(5);st.psValue = &m_BaseFightProperty.wDeBuffDef[DFT_Bled];		m_mapNameValue["B_DeBuffDef0"]=st;			//Debuff抗性
	st.SetType(5);st.psValue = &m_BaseFightProperty.wDeBuffDef[DET_Dizzy];		m_mapNameValue["B_DeBuffDef1"]=st;			//Debuff抗性
	st.SetType(5);st.psValue = &m_BaseFightProperty.wDeBuffDef[DET_Lull];		m_mapNameValue["B_DeBuffDef2"]=st;			//Debuff抗性
	st.SetType(5);st.psValue = &m_BaseFightProperty.wDeBuffDef[DET_Hypnosis];		m_mapNameValue["B_DeBuffDef3"]=st;			//Debuff抗性
	st.SetType(5);st.psValue = &m_BaseFightProperty.wDeBuffDef[DET_Poison];		m_mapNameValue["B_DeBuffDef4"]=st;			//Debuff抗性
	st.SetType(5);st.psValue = &m_BaseFightProperty.wDeBuffDef[DET_Fix];		m_mapNameValue["B_DeBuffDef5"]=st;			//Debuff抗性
	st.SetType(5);st.psValue = &m_BaseFightProperty.wDeBuffDef[DET_Silence];	m_mapNameValue["B_DeBuffDef6"]=st;			

	st.SetType(5);st.psValue = &m_BaseFightProperty.wElemDef[ET_Fire];			m_mapNameValue["B_ElemDef0"]=st;
	st.SetType(5);st.psValue = &m_BaseFightProperty.wElemDef[ET_Water];			m_mapNameValue["B_ElemDef1"]=st;
	st.SetType(5);st.psValue = &m_BaseFightProperty.wElemDef[ET_Earth];			m_mapNameValue["B_ElemDef2"]=st;
	st.SetType(5);st.psValue = &m_BaseFightProperty.wElemDef[ET_Wind];			m_mapNameValue["B_ElemDef3"]=st;
	st.SetType(5);st.psValue = &m_BaseFightProperty.wElemDef[ET_Dark];			m_mapNameValue["B_ElemDef4"]=st;
	st.SetType(5);st.psValue = &m_BaseFightProperty.wElemDef[ET_Holy];			m_mapNameValue["B_ElemDef5"]=st; //各个元素抗性

	{
		st.SetType(4);st.pValue = &m_arrElementProperty[ME_Earth].uArrPro[AD_Attack];		st.attrEnumValue=P_EARTH_ATTACK;	m_mapNameValue["Earth_Attack"]=st;
		st.SetType(4);st.pValue = &m_arrElementProperty[ME_Earth].uArrPro[AD_Defend];		st.attrEnumValue=P_EARTH_DEFEND;	m_mapNameValue["Earth_Defend"]=st;
		st.SetType(4);st.pValue = &m_arrElementProperty[ME_Water].uArrPro[AD_Attack];		st.attrEnumValue=P_WATER_ATTACK;	m_mapNameValue["Water_Attack"]=st;
		st.SetType(4);st.pValue = &m_arrElementProperty[ME_Water].uArrPro[AD_Defend];		st.attrEnumValue=P_WATER_DEFEND;	m_mapNameValue["Water_Defend"]=st;
		st.SetType(4);st.pValue = &m_arrElementProperty[ME_Fire].uArrPro[AD_Attack];		st.attrEnumValue=P_FIRE_ATTACK;		m_mapNameValue["Fire_Attack"]=st;
		st.SetType(4);st.pValue = &m_arrElementProperty[ME_Fire].uArrPro[AD_Defend];		st.attrEnumValue=P_FIRE_DEFEND;		m_mapNameValue["Fire_Defend"]=st;
		st.SetType(4);st.pValue = &m_arrElementProperty[ME_Wind].uArrPro[AD_Attack];		st.attrEnumValue=P_WIND_ATTACK;		m_mapNameValue["Wind_Attack"]=st;
		st.SetType(4);st.pValue = &m_arrElementProperty[ME_Wind].uArrPro[AD_Defend];		st.attrEnumValue=P_WIND_DEFEND;		m_mapNameValue["Wind_Defend"]=st;
		st.SetType(4);st.pValue = &m_arrElementProperty[ME_Bright].uArrPro[AD_Attack];		st.attrEnumValue=P_BRIGHT_ATTACK;	m_mapNameValue["Bright_Attack"]=st;
		st.SetType(4);st.pValue = &m_arrElementProperty[ME_Bright].uArrPro[AD_Defend];		st.attrEnumValue=P_BRIGHT_DEFEND;	m_mapNameValue["Bright_Defend"]=st;
		st.SetType(4);st.pValue = &m_arrElementProperty[ME_Dark].uArrPro[AD_Attack];		st.attrEnumValue=P_DARK_ATTACK;		m_mapNameValue["Dark_Attack"]=st;
		st.SetType(4);st.pValue = &m_arrElementProperty[ME_Dark].uArrPro[AD_Defend];		st.attrEnumValue=P_DARK_DEFEND;		m_mapNameValue["Dark_Defend"]=st;

		st.SetType(4);st.pValue = &m_arrElementProperty[ME_Earth].uArrPro[AD_AttackXiang];			st.attrEnumValue=P_EARTH_ATTACKXIANG;	m_mapNameValue["Earth_AttackXiang"]=st;
		st.SetType(4);st.pValue = &m_arrElementProperty[ME_Earth].uArrPro[AD_DefendXiang];			st.attrEnumValue=P_EARTH_DEFENDXIANG;	m_mapNameValue["Earth_DefendXiang"]=st;
		st.SetType(4);st.pValue = &m_arrElementProperty[ME_Water].uArrPro[AD_AttackXiang];			st.attrEnumValue=P_WATER_ATTACKXIANG;	m_mapNameValue["Water_AttackXiang"]=st;
		st.SetType(4);st.pValue = &m_arrElementProperty[ME_Water].uArrPro[AD_DefendXiang];			st.attrEnumValue=P_WATER_DEFENDXIANG;	m_mapNameValue["Water_DefendXiang"]=st;
		st.SetType(4);st.pValue = &m_arrElementProperty[ME_Fire].uArrPro[AD_AttackXiang];			st.attrEnumValue=P_FIRE_ATTACKXIANG;	m_mapNameValue["Fire_AttackXiang"]=st;
		st.SetType(4);st.pValue = &m_arrElementProperty[ME_Fire].uArrPro[AD_DefendXiang];			st.attrEnumValue=P_FIRE_DEFENDXIANG;	m_mapNameValue["Fire_DefendXiang"]=st;
		st.SetType(4);st.pValue = &m_arrElementProperty[ME_Wind].uArrPro[AD_AttackXiang];			st.attrEnumValue=P_WIND_ATTACKXIANG;	m_mapNameValue["Wind_AttackXiang"]=st;
		st.SetType(4);st.pValue = &m_arrElementProperty[ME_Wind].uArrPro[AD_DefendXiang];			st.attrEnumValue=P_WIND_DEFENDXIANG;	m_mapNameValue["Wind_DefendXiang"]=st;
		st.SetType(4);st.pValue = &m_arrElementProperty[ME_Bright].uArrPro[AD_AttackXiang];		st.attrEnumValue=P_BRIGHT_ATTACKXIANG;	m_mapNameValue["Bright_AttackXiang"]=st;
		st.SetType(4);st.pValue = &m_arrElementProperty[ME_Bright].uArrPro[AD_DefendXiang];		st.attrEnumValue=P_BRIGHT_DEFENDXIANG;	m_mapNameValue["Bright_DefendXiang"]=st;
		st.SetType(4);st.pValue = &m_arrElementProperty[ME_Dark].uArrPro[AD_AttackXiang];			st.attrEnumValue=P_DARK_ATTACKXIANG;	m_mapNameValue["Dark_AttackXiang"]=st;
		st.SetType(4);st.pValue = &m_arrElementProperty[ME_Dark].uArrPro[AD_DefendXiang];			st.attrEnumValue=P_DARK_DEFENDXIANG;	m_mapNameValue["Dark_DefendXiang"]=st;
	}


	st.SetType(5);st.psValue = &m_BaseFightProperty.wClassDam[C_Human];			m_mapNameValue["B_ClassDam0"]=st;
	st.SetType(5);st.psValue = &m_BaseFightProperty.wClassDam[C_Element];	m_mapNameValue["B_ClassDam1"]=st;
	st.SetType(5);st.psValue = &m_BaseFightProperty.wClassDam[C_Alien];			m_mapNameValue["B_ClassDam2"]=st;
	st.SetType(5);st.psValue = &m_BaseFightProperty.wClassDam[C_Animal];			m_mapNameValue["B_ClassDam3"]=st;
	st.SetType(5);st.psValue = &m_BaseFightProperty.wClassDam[C_Undead];			m_mapNameValue["B_ClassDam4"]=st;
	st.SetType(5);st.psValue = &m_BaseFightProperty.wClassDam[C_Evil];			    m_mapNameValue["B_ClassDam5"]=st;
	st.SetType(5);st.psValue = &m_BaseFightProperty.wClassDam[C_Hexapods];			m_mapNameValue["B_ClassDam6"]=st;//对各个种族伤害

	st.SetType(5);st.psValue = &m_BaseFightProperty.wElemDam[ET_Fire];			    m_mapNameValue["B_ElemDam0"]=st;			//对各个元素伤害
	st.SetType(5);st.psValue = &m_BaseFightProperty.wElemDam[ET_Water];			m_mapNameValue["B_ElemDam1"]=st;			//对各个元素伤害
	st.SetType(5);st.psValue = &m_BaseFightProperty.wElemDam[ET_Earth];			m_mapNameValue["B_ElemDam2"]=st;			//对各个元素伤害
	st.SetType(5);st.psValue = &m_BaseFightProperty.wElemDam[ET_Wind];			    m_mapNameValue["B_ElemDam3"]=st;			//对各个元素伤害
	st.SetType(5);st.psValue = &m_BaseFightProperty.wElemDam[ET_Dark];			    m_mapNameValue["B_ElemDam4"]=st;			//对各个元素伤害
	st.SetType(5);st.psValue = &m_BaseFightProperty.wElemDam[ET_Holy];			    m_mapNameValue["B_ElemDam5"]=st;			//对各个元素伤害

	st.SetType(5);st.psValue = &m_BaseFightProperty.wElemAtk[ET_Fire];				m_mapNameValue["B_ElemAtk0"]=st;		
	st.SetType(5);st.psValue = &m_BaseFightProperty.wElemAtk[ET_Water];			m_mapNameValue["B_ElemAtk1"]=st;	
	st.SetType(5);st.psValue = &m_BaseFightProperty.wElemAtk[ET_Earth];			m_mapNameValue["B_ElemAtk2"]=st;	
	st.SetType(5);st.psValue = &m_BaseFightProperty.wElemAtk[ET_Wind];				m_mapNameValue["B_ElemAtk3"]=st;	
	st.SetType(5);st.psValue = &m_BaseFightProperty.wElemAtk[ET_Dark];				m_mapNameValue["B_ElemAtk4"]=st;	
	st.SetType(5);st.psValue = &m_BaseFightProperty.wElemAtk[ET_Holy];				m_mapNameValue["B_ElemAtk5"]=st;

	st.SetType(5);st.psValue = &m_BaseFightProperty.wCriDef;		m_mapNameValue["B_CriDef"]=st;	
	st.SetType(5);st.psValue = &m_BaseFightProperty.wMCriDef;		m_mapNameValue["B_MCriDef"]=st;	
	st.SetType(5);st.psValue = &m_BaseFightProperty.wCriDamage;		m_mapNameValue["B_CriDamage"]=st;	
	st.SetType(5);st.psValue = &m_BaseFightProperty.wMCriDamage;		m_mapNameValue["B_MCriDamage"]=st;	

	st.SetType(4); st.pValue = &m_BaseFightProperty.lMaxPKCP;   m_mapNameValue["B_MaxPKCP"] = st;


	st.SetType(0);	st.plValue  = &m_AppendProperty.bShowFashion;			m_mapNameValue["ShowFashion"]=st;	m_mapAttrType["ShowFashion"]=P_ATTR_DISPLAYHEADPIECE;	//	是显示头盔

	st.SetType(0);	st.plValue=&m_Property.byCountry;		m_mapNameValue["btCountry"]=st;		m_mapAttrType["btCountry"]=P_ATTR_COUNTRY;					//	国家编号

	st.SetType(3);	st.pfValue=&m_fExpScale;                m_mapNameValue["ExpScale"]=st;
	st.SetType(3);	st.pfValue=&m_fOccuExpScale; 	        m_mapNameValue["OccuExpScale"]=st;
	st.SetType(3);	st.pfValue=&m_fSpScale;             	m_mapNameValue["SpScale"]=st;
	st.SetType(3);	st.pfValue=&m_fPresentExpScale;	        m_mapNameValue["PresentExpScale"]=st;

	// 商人身份
	st.SetType(0); st.plValue = &m_Property.byIsBusinessMan; m_mapNameValue["BusinessMan"] = st;

	m_mapDirtyAttrData.clear();	
	AddFightPropertyTODirty();
}

//初始客户端需要更新的数据类型
void CPlayer::InitUpdateDirtyAttrData()
{
	/*m_mapSelfUpdateDirtyAttrData.insert("Occu");
	m_mapSelfUpdateDirtyAttrData.insert("DOccu");
	m_mapSelfUpdateDirtyAttrData.insert("Class");
	m_mapSelfUpdateDirtyAttrData.insert("Sex");
	m_mapSelfUpdateDirtyAttrData.insert("HeadPic");
	m_mapSelfUpdateDirtyAttrData.insert("Hp");
	m_mapSelfUpdateDirtyAttrData.insert("Mp");
	m_mapSelfUpdateDirtyAttrData.insert("Energy");
	m_mapSelfUpdateDirtyAttrData.insert("Stamina");
	m_mapSelfUpdateDirtyAttrData.insert("Level");
	m_mapSelfUpdateDirtyAttrData.insert("Exp");
	m_mapSelfUpdateDirtyAttrData.insert("PresentExp");
	m_mapSelfUpdateDirtyAttrData.insert("RankOfNobility");
	m_mapSelfUpdateDirtyAttrData.insert("RankOfNobCredit");
	m_mapSelfUpdateDirtyAttrData.insert("RankOfMercenary");
	m_mapSelfUpdateDirtyAttrData.insert("MercenaryCredit");
	m_mapSelfUpdateDirtyAttrData.insert("Country");
	m_mapSelfUpdateDirtyAttrData.insert("CountryContribute");
	m_mapSelfUpdateDirtyAttrData.insert("BusinessLevel");
	m_mapSelfUpdateDirtyAttrData.insert("BusinessExp");
	m_mapSelfUpdateDirtyAttrData.insert("ArtisanCredit");
	m_mapSelfUpdateDirtyAttrData.insert("ArtisanLevel");
	m_mapSelfUpdateDirtyAttrData.insert("ArtisanExp");
	m_mapSelfUpdateDirtyAttrData.insert("PkValue");
	m_mapSelfUpdateDirtyAttrData.insert("PkCount");
	m_mapSelfUpdateDirtyAttrData.insert("PVPCount");
	m_mapSelfUpdateDirtyAttrData.insert("PVPValue");
	m_mapSelfUpdateDirtyAttrData.insert("UpgradeExp");
	m_mapSelfUpdateDirtyAttrData.insert("ShowCountry");


	m_mapSelfUpdateDirtyAttrData.insert("C_MaxHp");
	m_mapSelfUpdateDirtyAttrData.insert("C_MaxMp");
	m_mapSelfUpdateDirtyAttrData("C_MaxStamina");
	m_mapSelfUpdateDirtyAttrData.insert("C_MaxEnergy");
	m_mapDirtyAttrData.insert("C_Strenth");
	m_mapDirtyAttrData.insert("C_Agility");
	m_mapDirtyAttrData.insert("C_Consti");
	m_mapDirtyAttrData.insert("C_Intell");
	m_mapDirtyAttrData.insert("C_Wisdom");
	m_mapDirtyAttrData.insert("C_Spirit");
	m_mapDirtyAttrData.insert("C_Block");
	m_mapDirtyAttrData.insert("C_Parry");
	m_mapDirtyAttrData.insert("C_Dodge");
	m_mapDirtyAttrData.insert("C_Charm");
	m_mapDirtyAttrData.insert("C_Hit");
	m_mapDirtyAttrData.insert("C_Absorb");
	m_mapDirtyAttrData.insert("C_Def");
	m_mapDirtyAttrData.insert("C_Mdef");
	m_mapDirtyAttrData.insert("C_Cri");
	m_mapDirtyAttrData.insert("C_MAtkSpeed");
	m_mapDirtyAttrData.insert("C_BuffPower");
	m_mapDirtyAttrData.insert("C_MinAtk");
	m_mapDirtyAttrData.insert("C_MaxAtk");
	m_mapDirtyAttrData.insert("C_MAtk");
	m_mapDirtyAttrData.insert("C_MCri");
	m_mapDirtyAttrData.insert("C_CriDef");
	m_mapDirtyAttrData.insert("C_MCriDef");
	m_mapDirtyAttrData.insert("C_BloodSuck");
	m_mapDirtyAttrData.insert("C_ManaSuck");
	m_mapDirtyAttrData.insert("C_WillDef");
	m_mapDirtyAttrData.insert("C_ConstiDef");
	m_mapDirtyAttrData.insert("C_DeBuffDef0");
	m_mapDirtyAttrData.insert("C_DeBuffDef1");
	m_mapDirtyAttrData.insert("C_DeBuffDef2");
	m_mapDirtyAttrData.insert("C_DeBuffDef3");
	m_mapDirtyAttrData.insert("C_DeBuffDef4");
	m_mapDirtyAttrData.insert("C_DeBuffDef5");
	m_mapDirtyAttrData.insert("C_ElemDef0");
	m_mapDirtyAttrData.insert("C_ElemDef1");
	m_mapDirtyAttrData.insert("C_ElemDef2");
	m_mapDirtyAttrData.insert("C_ElemDef3");
	m_mapDirtyAttrData.insert("C_ElemDef4");
	m_mapDirtyAttrData.insert("C_ElemDef5");
	m_mapDirtyAttrData.insert("C_ClassDam0");
	m_mapDirtyAttrData.insert("C_ClassDam1");
	m_mapDirtyAttrData.insert("C_ClassDam2");
	m_mapDirtyAttrData.insert("C_ClassDam3");
	m_mapDirtyAttrData.insert("C_ClassDam4");
	m_mapDirtyAttrData.insert("C_ClassDam5");
	m_mapDirtyAttrData.insert("C_ClassDam6");
	m_mapDirtyAttrData.insert("C_ElemDam0");
	m_mapDirtyAttrData.insert("C_ElemDam1");
	m_mapDirtyAttrData.insert("C_ElemDam2");
	m_mapDirtyAttrData.insert("C_ElemDam3");
	m_mapDirtyAttrData.insert("C_ElemDam4");
	m_mapDirtyAttrData.insert("C_ElemDam5");
	m_mapDirtyAttrData.insert("C_ElemAtk0");
	m_mapDirtyAttrData.insert("C_ElemAtk1");
	m_mapDirtyAttrData.insert("C_ElemAtk2");
	m_mapDirtyAttrData.insert("C_ElemAtk3");
	m_mapDirtyAttrData.insert("C_ElemAtk4");
	m_mapDirtyAttrData.insert("C_ElemAtk5");*/

	// 初始化通知的simple数据
	m_mapOtherUpdateDirtyAttrData.insert(string("Hp"));
	m_mapOtherUpdateDirtyAttrData.insert(string("C_MaxHp"));
	m_mapOtherUpdateDirtyAttrData.insert(string("ShowCountry"));
	m_mapOtherUpdateDirtyAttrData.insert(string("bInvisible"));
	m_mapOtherUpdateDirtyAttrData.insert(string("bRide"));
	m_mapOtherUpdateDirtyAttrData.insert(string("GMConcealment"));
	// PK CP 值
	m_mapOtherUpdateDirtyAttrData.insert( "CurPKCP" );
	m_mapOtherUpdateDirtyAttrData.insert( "MaxPKCP" );

	/*m_mapOtherUpdateDirtyAttrData.insert(string("PkValue"));
	m_mapOtherUpdateDirtyAttrData.insert(string("PVPValue"));*/
}

long CPlayer::GetValue(const char* strName)
{
	long value = INVILID_VALUE;
	value = CMoveShape::GetValue(strName);
	if(INVILID_VALUE != value)
		return value;

	mapNameValueItr itr = m_mapNameValue.begin();
	itr = m_mapNameValue.find(strName);
	if (itr != m_mapNameValue.end())
	{
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
	}
	return value;
}

long CPlayer::SetValue(const char* strName, long dwValue)
{
#ifdef _DEBUG_SKILL
	/*char strDebug[256];
	_snprintf(strDebug, 256, "玩家状态属性名称%s值%d\n",strName,dwValue);
	OutputDebugStr(strDebug);	*/
#endif


	long value = INVILID_VALUE;

	if (GetState()==STATE_DIED && strcmp(strName,"Hp")==0 && dwValue>0)
	{
		return 0;
	}
	value = CMoveShape::SetValue(strName, dwValue);
	if(INVILID_VALUE != value)
		return value;

	mapNameValueItr itr = m_mapNameValue.begin();
	itr = m_mapNameValue.find(strName);
	if (itr != m_mapNameValue.end())
	{
		long lOldValue = GetValue(strName);;
		/*	if(dwValue < itr->second.minValue)
		dwValue = itr->second.minValue;
		if(dwValue > itr->second.maxValue)
		dwValue = itr->second.maxValue;*/

		switch (itr->second.lType)
		{
		case 0:	// byte
			*(itr->second.plValue) = (BYTE)dwValue;
			break;
		case 1:	// word
			*(itr->second.pwValue) = (WORD)dwValue;
			break;
		case 2:	// dword
			*(itr->second.pdwValue) = (DWORD)dwValue;
			break;
		case 3:	// float
			*(itr->second.pfValue) = (float)dwValue;
			break;
		case 4:	// long
			*(itr->second.pValue) = (long)dwValue;
			break;
		case 5:	// short
			*(itr->second.psValue) = (short)dwValue;
			break;
		case 6:	// bool
			*(itr->second.pbValue) = dwValue==0?false:true;
			break;
		}

		OnPropertyValueChanged(strName, dwValue);

		if (strcmp("PkValue", strName) == 0 || strcmp("PVPValue", strName) == 0)
		{
			OnPKOrPVPValueChange(lOldValue,dwValue);
		}
		else if (strcmp("E_MaxHp",strName) == 0)
		{
			string strHp = "Hp";
			string strMaxHp = "C_MaxHp";
			if (GetAttribute(strHp)>(long)GetMaxHP())
			{
				SetAttribute(strHp,(long)GetMaxHP());
			}
		}
		else if (strcmp("E_MaxMp",strName) == 0)
		{
			string strMp = "Mp";
			string strMaxMp = "C_MaxMp";
			if (GetAttribute(strMp)>(long)GetMaxMP())
			{
				SetAttribute(strMp,(long)GetMaxMP());
			}
		}
		// 调整当前PK CP值
		else if( strcmp( "E_MaxPKCP", strName ) == 0 )
		{
			std::string strPKCP = "CurPKCP";
			long lMaxPKCP = GetMaxPKCP();
			if( GetAttribute( strPKCP ) > (long)lMaxPKCP )
			{
				SetAttribute( strPKCP, lMaxPKCP );
			}
		}

		return dwValue;
	}


	return value;
}

void  CPlayer::OnPropertyValueChanged(const char* strName, LONG lValue)
{
	//如果力量、敏捷、体质、智利、智慧、精神之一的基本值或变化值发生改变
	//要重新计算相关其他战斗属性
	if(NULL == strName)	return;
	if(0 == strncmp(strName,"B_Str",9) || 0 == strncmp(strName,"E_Str",9) ||
		0 == strncmp(strName,"B_Dex",9) || 0 == strncmp(strName,"E_Dex",9) ||
		0 == strncmp(strName,"B_Con",8) || 0 == strncmp(strName,"E_Con",8) ||
		0 == strncmp(strName,"B_Intellect",8) || 0 == strncmp(strName,"E_Intellect",8) ||
//		0 == strncmp(strName,"B_Wisdom",8) || 0 == strncmp(strName,"E_Wisdom",8) ||
		0 == strncmp(strName,"B_Spiritualism",8) || 0 == strncmp(strName,"E_Spiritualism",8))
	{
		ResetFightProperty();
	}
	//如果是职业等级发生变化,则触发相应接口
	else if( 0 == strncmp(strName,"OccuLvl",7) )
	{
		OnOccuLevelChanged();
		GetGame()->UpdateSavePlayer(this, SAVE_DETAIL_ATTRPROPERTY);
	}

	//如果是职业发生变化,则触发相应接口
	else if( 0 == strcmp(strName,"Occu") )
	{
		GetGame()->UpdateSavePlayer(this, SAVE_DETAIL_ATTRPROPERTY);
	}
	else if( 0 == strcmp(strName,"RankOfNobility") )
	{
		//更新爵位称号
		if(GetTitleType() == ePTT_Knight)
			UpdateTitle(true);
	}

	//! [2007-8-7 add]
	if (NULL_GUID == GetTeamID())
	{
		return;
	}

	GSTeam *pTeam = (GSTeam*)GetOrganiCtrl()->GetGameTeamByID(GetTeamID());
	if(NULL == pTeam)
	{
		return;
	}

	long lChangeType = 0;

	if (0 == strcmp(strName, "lTileX") || 0 == strcmp(strName, "lTileY") )
	{
		lChangeType = eMIS_Pos;
	}
	else if (0 == strcmp(strName, "Hp"))
	{
		lChangeType = eMIS_CurrHp;
	}
	else if (0 == strcmp(strName, "Mp"))
	{
		lChangeType = eMIS_CurrMp;
	}
	else if (0 == strcmp(strName, "CurPKCP"))
	{
		lChangeType = eMIS_CurrEnergy;
	}
	else if (0 == strcmp(strName, "B_MaxHp") || 0 == strcmp(strName, "E_MaxHp") || 0 == strcmp(strName, "C_MaxHp"))
	{
		lChangeType = eMIS_MaxHp;
	}
	else if (0 == strcmp(strName, "B_MaxMp") || 0 == strcmp(strName, "E_MaxMp") || 0 == strcmp(strName, "C_MaxMp"))
	{
		lChangeType = eMIS_MaxMp;
	}
	else if (0 == strcmp(strName, "MaxPKCP"))
	{
		lChangeType = eMIS_MaxEnergy;
	}
	else if (0 == strcmp(strName, "Level"))
	{
		lChangeType = eMIS_Level;
	}
	else if (0 == strcmp(strName, "Occu"))
	{
		lChangeType = eMIS_Occupation;
	}
	else if (0 == strcmp(strName, "Sex"))
	{
		lChangeType = eMIS_Sex;
	}
	else
	{
		return;
	}

	pTeam->OnPlayerIdioinfoChange(GetExID(), (eUpdateIdioinfo)lChangeType, lValue);

	//! [add end]
}

//////////////////////////////////////////////////////////////////////////
//	技能
//////////////////////////////////////////////////////////////////////////

//判断cooldown是否超时
bool CPlayer::CooldownPast(long lSkillID)
{
	DWORD lEndTime = 0;
	DWORD dwCurTime = timeGetTime();
	itSkill it = m_Skills.find(lSkillID);
	if(it== m_Skills.end())	
		return true;
#ifdef _DEBUG
	/*char str[256];
	_snprintf(str, 256, "\n技能ID:%d,冷却当前时间:%d,冷却结束时间%d,冷却开始时间:%d,冷却时间%d\n",lSkillID,dwCurTime,(*it).second.dwCooldownStartTime + (*it).second.wCooldown,(*it).second.dwCooldownStartTime, (*it).second.wCooldown);
	OutputDebugString(str);*/
#endif
	lEndTime = (*it).second.dwCooldownStartTime+ (*it).second.wCooldown;
 	return 	dwCurTime >= lEndTime;//(*it).second.dwCooldownStartTime+ (*it).second.wCooldown;
}
//根据ID结束状态
void CPlayer::EndBuff(DWORD dwStateID)
{
	CMoveShape::EndBuff(dwStateID);
}
//设置公共冷却时间
void CPlayer::SetCommonCooldown(long lID, DWORD dwTime)
{
	//给所有受公共冷却时间影响的技能家冷却时间
	DWORD dwCurTime = timeGetTime();
	itSkill it = m_Skills.begin();
	for(;it!=m_Skills.end();it++)
	{
		time_t t_StartTime;
		t_StartTime = time(NULL); 

		tagSkill &skill = (*it).second;
		if((*it).first == lID)
		{
			DWORD dwRemainTime = 0;
			if(dwCurTime < skill.dwCooldownStartTime+skill.wCooldown)
				dwRemainTime = skill.wCooldown-(dwCurTime-skill.dwCooldownStartTime);
			if (dwRemainTime<dwTime)
			{
				dwRemainTime = dwTime;
			}		
			skill.dwCooldownStartTime=dwCurTime;
			skill.wCooldown=dwRemainTime;
			skill.tCooldownEndtime = t_StartTime + time_t(dwRemainTime/1000);
		}
		else
		{
			DWORD dwRemainTime = 0;			
			if(dwCurTime < skill.dwCooldownStartTime+skill.wCooldown)
				dwRemainTime = skill.wCooldown-(dwCurTime-skill.dwCooldownStartTime);
			if (dwRemainTime<dwTime)
			{
				dwRemainTime = dwTime;
			}

				skill.dwCooldownStartTime = dwCurTime;
			skill.wCooldown = dwRemainTime;
		}
	}
	return;
}
void CPlayer::SendUseItemSucceed(long lGoodsIndex)
{
	CMessage msg(MSG_S2C_PLAYER_USEITEM);
	msg.Add((char)PLAYER_USEITEM_SUCCEED);
	msg.Add(GetExID());
	msg.Add(lGoodsIndex);
	msg.SendToAround( this );
}

bool CPlayer::GoodsCoolDown(long lGoodsIndex)
{
	CGoodsBaseProperties* pProperty=GoodsSetup::QueryGoodsBaseProperties(lGoodsIndex);
	if(pProperty)
	{
		//冷却检测
		DWORD dwCoolDownTime = pProperty->GetAddonPropertyValue(GAP_COOLDOWN,1);
		DWORD dwCoolDownId = pProperty->GetAddonPropertyValue(GAP_COOLDOWN,2);

		if(dwCoolDownId!=0)
		{
			//有公用冷却限制
			map<DWORD,tagGoodsCommonCooldown>::iterator iter=m_mapClassIdCoolDown.find(dwCoolDownId);
			if(iter!=m_mapClassIdCoolDown.end())
			{
				if(timeGetTime() - iter->second.dwCooldownStartTime<=dwCoolDownTime)				
				{
					return false;
				}			
			}
		}
		else if(dwCoolDownTime!=0)
		{
			//GOODSID冷却限制
			map<DWORD,tagGoodsCommonCooldown>::iterator iter=m_mapGoodsIdCoolDown.find(lGoodsIndex);
			if(iter!=m_mapGoodsIdCoolDown.end())
			{
				if(timeGetTime()-iter->second.dwCooldownStartTime<=dwCoolDownTime)
				{
					return false;
				}
			}
		}
		return true;
	}
	return false;
}

int CPlayer::JudgeSkillBegining(DWORD dwSkillID, DWORD dwLevel,CMoveShape *pTarget,long lGoodsIndex)
{
    DWORD dwCurrTime = timeGetTime();

    if ( !IsCanUseAllSkill() )
    {
        return SKILL_USE_FAILED_INVALID_NOTUSESKILL;
    }

	if (lGoodsIndex==0 && !CooldownPast(dwSkillID))
	{	
		//冷却时间
		return SKILL_USE_FAILED_INVALID_TIME;
	}

	if (lGoodsIndex!=0 && !GoodsCoolDown(lGoodsIndex))
	{
		//物品冷却时间
		return SKILL_USE_FAILED_INVALID_TIME;
	}

	if (dwCurrTime<m_dwIntonateStartTime + m_dwIntonateTime)
	{
		//吟唱时间
		return SKILL_USE_FAILED_INVALID_INTONATE;		
	}

    if (dwCurrTime < (m_dwCDStartTime+m_dwCDControlTime) )
    {
        //关于全局的CD时间
        return SKILL_USE_FAILED_INVALID_CDTIME;
    }

    if ( m_Buddhism.IsFull() )
    {
#ifdef  _DEBUG
        char  szText[256]="[Colour 'ffffffff'] 杂念值过高不能使用技能";
        SendNotifyMessage( szText, eNOTIFYPOS_LEFT_BOTTOM );
#endif
        /// 杂念值满了不能使用技能
        return SKILL_USE_FAILED_INVALID_FULLDISTRACT;
    }

	CNewSkill *pSkillProperty = CNewSkillFactory::QueryNewSkillProperty(dwSkillID);
	if (!pSkillProperty)
	{
		return SKILL_USE_FAILED_INVALID_UNKNOWN;
	}
	if (pSkillProperty)
	{	
		long lActTime = GetSkillValue( dwSkillID , dwLevel ,"ActTime" );
		if (lActTime>0)
		{
			if (IsInAct())
			{
				//动作时间
				return SKILL_USE_FAILED_INVALID_ACT;
			}
		}
	}
	if (pSkillProperty && pTarget)
	{
		if (pSkillProperty->GetTarget()==0 && pSkillProperty->GetDeadUse()==0 && pTarget->GetState()==STATE_DIED)
		{
			return SKILL_USE_FAILED_INVALID_STATE;
		}
        if( PHASEMAN.PhasePlayerFilter( this , pTarget) )
        {
            /// 不同的相位
            return SKILL_USE_FAILED_INVALID_NOTSAMEPHASE;
        }
	}

	if (this->IsDied())
	{
		//死亡
		return SKILL_USE_FAILED_INVALID_STATE;
	}

	if (GetCurrentProgress()==CPlayer::PROGRESS_OPEN_STALL 
		|| GetCurrentProgress()==CPlayer::PROGRESS_TRADING 
        || IsLockPlayerState() )
	{
		/// 摆摊/交易/濒死/跟随 状态下不能使用技能
		return SKILL_USE_FAILED_INVALID_STATE;
	}

	if (GetUseFightSkill() == FALSE)
	{
		if (pSkillProperty->GetUseType()==1)
		{
			//不能使用战士类技能
			return SKILL_USE_FAILED_INVALID_STATE;
		}
	}
	
	if (GetUseRabbSkill() == FALSE)
	{
		if (pSkillProperty->GetUseType()==2)
		{
			//不能使法师类技能
			return SKILL_USE_FAILED_INVALID_STATE;
		}
	}

	return -1;
}
long CPlayer::AddActiveSkill(DWORD dwSkillID, DWORD dwLevel,CMoveShape *pTarget,long lGoodsIndex,long lContainerID,long lPos,CGUID GoodsGuid)
{
	SetInstanceSkill(NULL);
	int nRet = JudgeSkillBegining(dwSkillID,dwLevel,pTarget,lGoodsIndex);

	if (nRet!=-1)
	{
#ifdef _SKILL_DEBUG
		Log4c::Trace(ROOT_MODULE,"发送技能失败消息:技能ID:%6d\n",dwSkillID);
#endif
		CMessage msg(MSG_S2C_SKILL_USE);
		msg.Add((CHAR)SKILL_USE_RESULT_FAILED);
		msg.Add( this -> GetExID() );
		//id和等级
		msg.Add((long)dwSkillID);
		msg.Add((BYTE)dwLevel );
		msg.Add((DWORD)0);
		//错误类型
		msg.Add((BYTE)nRet);
		msg.SendToPlayer(GetExID());
		return 0;
	}
	else
	{
		CMoveShape::InitSkillInstance(dwSkillID,dwLevel,lGoodsIndex,lContainerID,lPos,GoodsGuid,4);
		return 1;
	}	

}
//
void CPlayer::SetSkillCooldown(long lID,DWORD dwTime)
{
	itSkill it = m_Skills.find(lID);
	if(it== m_Skills.end())	return;

	(*it).second.dwCooldownStartTime=timeGetTime();
	(*it).second.wCooldown = dwTime;
}

LONG CPlayer::GetNumSkills()
{
	LONG lNumSkills = 0;
	lNumSkills = (LONG)m_Skills.size();

	return lNumSkills;
}
LONG CPlayer::GetSkillLv(long lSkillID)
{
	long lSkillLv=0;
	itSkill it = m_Skills.find(lSkillID);

	if(it!=m_Skills.end())
	{
		lSkillLv =(*it).second.wLevel; 
	}
	return lSkillLv;
}


void CPlayer::CodeSkillsToDataBlock_ForClient(DBWriteSet& setWriteDB, BOOL bAddRestoreTime )
{
    setWriteDB.AddToByteArray( GetNumSkills());
	//公共冷却时间
	setWriteDB.AddToByteArray((WORD)GlobalSetup::GetSetup()->lCommoCoolDownTime);
	tagSkill tSkill;
	itSkill it=m_Skills.begin(); 
    for (;it!=m_Skills.end();++it)
    {
        tSkill.dwID = it->second.dwID;
        tSkill.wLevel = it->second.wLevel;
        tSkill.wCooldown = it->second.wCooldown;
        DWORD dwCooldown = 0;
        if (it->second.dwOldCooldown - tSkill.wCooldown>0 && tSkill.wCooldown>0)
        {
            dwCooldown = it->second.dwOldCooldown - tSkill.wCooldown;
            it->second.dwCooldownStartTime = timeGetTime();
#ifdef _DEBUG
            char str[256];
            _snprintf(str, 256, "\n---------------------------技能ID:%d,冷却开始时间:%d,冷却时间:%d\n",tSkill.dwID ,(*it).second.dwCooldownStartTime,tSkill.wCooldown);
            OutputDebugString(str);
#endif
        }

        setWriteDB.AddToByteArray(tSkill.dwID);
        setWriteDB.AddToByteArray(tSkill.wLevel);
        setWriteDB.AddToByteArray(tSkill.wCooldown);

        if ( bAddRestoreTime && GetInst(SkillAttribute).IsExist(it->second.dwID,it->second.wLevel) )
        {
            setWriteDB.AddToByteArray((float)GetSkillValue( it->second.dwID , it->second.wLevel , "MinAtkDistance" ));
            setWriteDB.AddToByteArray((float)GetSkillValue( it->second.dwID , it->second.wLevel , "MaxAtkDistance" ));
            setWriteDB.AddToByteArray((WORD)GetSkillValue( it->second.dwID , it->second.wLevel , "SMp" ));
        }
        setWriteDB.AddToByteArray(dwCooldown);
    }
}

void CPlayer::CodeSkillsToDataBlock(DBWriteSet& setWriteDB)
{
    setWriteDB.AddToByteArray( GetNumSkills());
    
#ifdef _RUNSTACKINFO1_
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CPlayer::CodeToDataBlock() m_Skills Start");
#endif
    itSkill it=m_Skills.begin();
    for(;it!=m_Skills.end();++it)
    {
        setWriteDB.AddToByteArray( static_cast<DWORD>(it->second.dwID));
        setWriteDB.AddToByteArray( static_cast<DWORD>(it->second.wLevel));

        CNewSkillProperties *pSkillProperties = CNewSkillFactory::QueryNewSkillProperty(it->second.dwID);
        if (pSkillProperties && pSkillProperties->GetCoolDown()==1)
        {
            time_t t_CurrentTime;
            t_CurrentTime = time(NULL);
            if (t_CurrentTime-it->second.tCooldownEndtime < 0)
            {
                setWriteDB.AddToByteArray( (DWORD)(it->second.tCooldownEndtime));
            }
            else
            {
                setWriteDB.AddToByteArray( (DWORD)(0));
            }

        }
        else
        {
            setWriteDB.AddToByteArray( (DWORD)(0));
        }
    }
}

void CPlayer::DecodeSkillsFromDataBlock(DBReadSet& setReadDB)
{
	LONG lNumSkills = setReadDB.GetLongFromByteArray(  );

	tagSkill tSkill;
	time_t t_CurrentTime;
	for( LONG i = 0; i < lNumSkills; i++ )
	{
		t_CurrentTime = time(NULL);
		tSkill.dwID = setReadDB.GetDwordFromByteArray();
		tSkill.wLevel = (WORD)setReadDB.GetDwordFromByteArray();


		tSkill.tCooldownEndtime =time_t(setReadDB.GetDwordFromByteArray( ));
		long lDiffTime =  (long)(tSkill.tCooldownEndtime - t_CurrentTime);

		if (lDiffTime>0)
		{
			tSkill.wCooldown = lDiffTime*1000;
		}
		else
		{
			tSkill.wCooldown = 0;
		}
		//##添加技能	
		AddSkill(tSkill.dwID,tSkill.wLevel,tSkill.wCooldown,tSkill.tCooldownEndtime);	
	}
}

void CPlayer::DecodeGeniusFromDataBlock(DBReadSet& setReadDB)
{
    long  lNumGenius = setReadDB.GetLongFromByteArray();

    for ( int i = 0 ; i < lNumGenius ; i++ )
    {
        long genid = setReadDB.GetLongFromByteArray();
        long lvl   = setReadDB.GetLongFromByteArray();

        m_GeniusTable[ genid ] = lvl ;
    }
    m_GeniusAction->Update();
}

void CPlayer::CodeGeniusToDataBlock(DBWriteSet& setWriteDB)
{
    setWriteDB.AddToByteArray( (long) m_GeniusTable.size() );

    for ( std::map<long,long>::iterator itr = m_GeniusTable.begin(); itr != m_GeniusTable.end() ; itr ++ )
    {
        setWriteDB.AddToByteArray( itr->first );
        setWriteDB.AddToByteArray( itr->second);
    }
}

void CPlayer::CodeGeniusToDataBlock_ForClient(DBWriteSet& setWriteDB, BOOL bAddRestoreTime)
{
      CodeGeniusToDataBlock(setWriteDB);

      m_GeniusAction->CodeGeniusToDataBlock( setWriteDB ,bAddRestoreTime );
}

// 义结金兰
void CPlayer::CodeBrotherToDataBlock(DBWriteSet& setWriteDB)
{
    setWriteDB.AddToByteArray((long)m_mapBrotherInfo.size());
    map<CGUID, tagBrotherInfo>::iterator itor = m_mapBrotherInfo.begin();
    for(; itor != m_mapBrotherInfo.end(); ++itor)
    {
        setWriteDB.AddToByteArray((*itor).second.guidBrother);
        setWriteDB.AddToByteArray((*itor).second.lFriendVal);
    }

}

void CPlayer::DecodeBrotherFromDataBlock(DBReadSet& setReadDB)
{
    m_mapBrotherInfo.clear();
    long lBrotherNum = setReadDB.GetLongFromByteArray();
    tagBrotherInfo stBrother;
    for(long l=0; l<lBrotherNum; l++)
    {
        setReadDB.GetBufferFromByteArray(stBrother.guidBrother);
        setReadDB.GetLongFromByteArray();
        m_mapBrotherInfo[stBrother.guidBrother] = stBrother;
    }

}

void CPlayer::OnChangeStates()
{
	//##在发送HP/MP/RP/YP给自身后，将HP发送给队友

	//////////////////////////////////////////////////////////////////////////
	//  [4/18/2007 chenxianjin]
	CMessage msg( MSG_S2C_SKILL_STATE_CHANGE );
	msg.Add( GetType() );
	msg.Add( GetExID() );
	msg.Add( GetHP() );
	msg.Add( GetMP() );
	msg.Add( GetEnergy() );
	//	msg.Add( GetYP() );
	msg.SendToPlayer(GetExID());
	//////////////////////////////////////////////////////////////////////////

	if( GetTeamID() != NULL_GUID )
	{
		//##发送给队友
		//assert(false);
		//LONG lSessionID = GetGame() -> GetTeamSessionID( GetTeamID() );
		/*	CTeam* pTeam = dynamic_cast<CTeam*>( const_cast<CSession*>( CSessionFactory::QuerySession(lSessionID) ) );
		if( pTeam )
		{
		CPlug* pPlug = pTeam -> QueryPlugByOwner( GetType(), GetExID() );
		if( pPlug )
		{
		FLOAT fHp = static_cast<FLOAT>( GetHP() ) / static_cast<FLOAT>( GetMaxHP() );
		pPlug -> ChangeState( CTeamate::TS_CHANGE_STATE, reinterpret_cast<UCHAR*>(&fHp) );
		}
		}*/
	}
}



void CPlayer::SetSilence( long lTime )
{ 
	m_lSilenceTimeStamp	= timeGetTime() ;
	m_lSilenceTime		= lTime; 
}

bool CPlayer::IsInSilence()
{ 
	if(m_lSilenceTime == 0)	return false;

	DWORD dwCurTime = timeGetTime();
	if( (m_lSilenceTimeStamp + m_lSilenceTime) >= (long)dwCurTime)
	{
		return true;
	}
	m_lSilenceTime = 0;
	return false;
}

void CPlayer::UpdateCurrentState()
{
	//##战斗状态的减少
	if( m_lFightStateCount > 0 )
	{
		-- m_lFightStateCount;
		if( m_lFightStateCount <= 0 )
		{
			//EnterPeaceState();
		}
	}
	//##犯罪状态的减少
	if( m_dwSinStateTimeStamp > 0 )
	{
		//##恢复一般状态
		if( timeGetTime() >= m_dwSinStateTimeStamp + GlobalSetup::GetSetup() -> dwCriminalTime ||
			GetPkValue() > GlobalSetup::GetSetup()->dwPkCountPerKill )
		{
			EnterResidentState();
		}
	}
}

void CPlayer::EnterCriminalState()
{
	if( GetPkValue() <= GlobalSetup::GetSetup()->dwPkCountPerKill )
	{
		if( m_dwSinStateTimeStamp == 0 )
		{
			CMessage msg( MSG_S2C_SHAPE_SINSTATE );
			msg.Add( GetExID() );
			msg.Add( static_cast<BYTE>(true) );
			msg.SendToAround( this );
		}
		m_dwSinStateTimeStamp = timeGetTime();
	}
}

void CPlayer::EnterResidentState()
{
	//##无论是否杀人，这个函数只用于清除犯罪状态
	m_dwSinStateTimeStamp = 0;
	CMessage msg( MSG_S2C_SHAPE_SINSTATE );
	msg.Add( GetExID() );
	msg.Add( static_cast<BYTE>(false) );
	msg.SendToAround( this );
}

//void CPlayer::EnterPeaceState()
//{
//	SetState( STATE_PEACE );
//	m_lFightStateCount = 0;
//
//	//##发送消息给客户端
//	CMessage msg( MSG_S2C_SHAPE_CHANGESTATE );
//	msg.Add( GetExID() );
//	msg.Add( static_cast<LONG>(STATE_PEACE) );
//	msg.SendToAround( this );
//}

//void CPlayer::EnterCombatState()
//{
//	SetState( STATE_FIGHT );
//	m_lFightStateCount = GlobalSetup::GetSetup() -> lFightStateTimer;
//	if(m_lFightStateTimerID!=-1)
//	{
//		TIMER_CANCEL(m_lFightStateTimerID);
//	}
//	//注册定时器
//	m_lFightStateTimerID = TIMER_SCHEDULE(this,NULL,timeGetTime()+m_lFightStateCount,0);
//
//	//##发送消息给客户端
//	CMessage msg( MSG_S2C_SHAPE_CHANGESTATE );
//	msg.Add( GetExID() );
//	msg.Add( static_cast<LONG>(STATE_FIGHT) );
//	msg.SendToAround( this );
//}



//##怒的增加，（只对武士有用）
void CPlayer::IncreaseRp( BOOL bAttack/*##标志是否是攻击，如果为false则是被攻击*/, WORD wLoseHP )
{
	//
	//	if( GetOccupation() != PlayerList::OC_FIGHTER ) return;

	//##获取全局设置
	GlobalSetup::tagSetup* pSetup = GlobalSetup::GetSetup();
	if( !pSetup ) return;

	//##如果等级不够，则直接退出
	if( GetLevel() < pSetup -> wBaseRp_Lv1 ) return;

	LONG lMaxRp = 0;
	//##否则获取怒值上限
	if( GetLevel() < pSetup -> wBaseRp_Lv2 )
	{
		lMaxRp = pSetup -> wBaseMaxRp_Lv1;
	}
	else 
	{
		lMaxRp = pSetup -> wBaseMaxRp_Lv2;
	}

	//##攻击的时候，增加怒值
	if( bAttack )
	{
		LONG lRp = GetEnergy();
		lRp = lRp + pSetup -> wBaseAtcRp_Sword;
		if( lRp > lMaxRp ) lRp = lMaxRp;
		SetEnergy( static_cast<WORD>(lRp) );
	}
	else 
		//##受伤，增加怒值
	{
		FLOAT	fPercent	= static_cast<FLOAT>( wLoseHP ) / GetMaxHP();
		WORD	wRpAdded	= 0;
		for( LONG i = 5; i >= 0; i -- )
		{
			if( fPercent <= pSetup -> fRpAdd_Lv1[i] )
			{
				wRpAdded = pSetup -> wRpAdd_Lv1[i];
			}
			else
			{
				break;
			}
		}
		//##增加怒值，然后返回
		LONG lRp	= GetEnergy();
		lRp			= lRp + wRpAdded;
		if( lRp > lMaxRp ) 
		{
			lRp = lMaxRp;
		}
		SetEnergy( static_cast<WORD>(lRp) );
	}
}


//根据ID得到物品
CGoods* CPlayer::GetGoodsById(const CGUID& guid)
{


	CGoods* pGoods=NULL;
	pGoods = (CGoods*)getOriginPack()->Find(TYPE_GOODS,guid);
	if(pGoods != NULL)
		return pGoods;
	pGoods = (CGoods*)m_cEquipment.Find(TYPE_GOODS,guid);

	return pGoods;
}

//根据ID得到背包物品
CGoods* CPlayer::GetGoodsById_FromPackage(const CGUID& guid)
{
	CGoods* pGoods = (CGoods*)getOriginPack()->Find(TYPE_GOODS,guid);
	if(pGoods)
		return pGoods;
	CVolumeLimitGoodsContainer* pSubPack=NULL;
	for(int i=CS2CContainerObjectMove::PEI_PACK1;i<=CS2CContainerObjectMove::PEI_PACK5;i++)
	{
		if(GetPackage()->GetSubpack(i-CS2CContainerObjectMove::PEI_PACK1)->pGoods)
		{
			pSubPack=GetPackage()->GetSubpack(i-CS2CContainerObjectMove::PEI_PACK1)->pSubpackContainer;
			if(pSubPack)
			{
				pGoods=(CGoods*)pSubPack->Find(TYPE_GOODS,guid);
				if(pGoods)
					return pGoods;
			}
		}		
	}	
	return NULL;

}


void CPlayer::Disconnect( DWORD dwDelayTime )
{
	// 如果没有进入到场景中，那么直接踢掉即可
	CServerRegion* pRegion = dynamic_cast<CServerRegion*>(GetFather());

	if ( pRegion==NULL || pRegion->FindChildObject(TYPE_PLAYER, GetExID()) == NULL )
	{
		GetGame()->KickPlayer(GetExID());
		return;
	}

	//##如果之前已经设置了断开连接的标记,则不再设置断开标记.
	if( m_dwDisconnectTimeStamp ) return;
	m_dwDisconnectTimeStamp = timeGetTime();
	m_dwDisconnectDelayTime = dwDelayTime;
}

void        CPlayer::UnCancelBuddhismTimer()
{
     if ( m_Buddhism.lBudhismTimer > 0  )
     {
         TIMER_CANCEL( m_Buddhism.lBudhismTimer );
     }
}

void        CPlayer::RegisterBuddhismTimer()
{
    /// 注册定时器
    tagTimerVar* pTimvar =  OBJ_CREATE(tagTimerVar);
    pTimvar->TimerType = (eChildObjType)ChildObjType_Budhism;
    m_Buddhism.lBudhismTimer= TIMER_SCHEDULE(this,(void*)pTimvar,timeGetTime() + GlobalSetup::GetSetup()->lDistractTime ,GlobalSetup::GetSetup()->lDistractTime);
}

void        CPlayer::RegisterDryTimer()
{
    /// 注册定时器
    tagTimerVar* pTimvar =  OBJ_CREATE(tagTimerVar);
    pTimvar->TimerType = (eChildObjType)ChildObjType_ClearDry;
    m_Buddhism.lDryEndTime = timeGetTime() + GlobalSetup::GetSetup()->lDryTime;
    m_Buddhism.lDryTimer= TIMER_SCHEDULE(this,(void*)pTimvar,m_Buddhism.lDryEndTime, 0 );

}

void        CPlayer::UseSkillUpdateBuddhismDistract()
{
    /// 增加杂念值
    m_Buddhism.UpdateDistract( GlobalSetup::GetSetup()->lAddDistractValue );
    
    /// 增加燥影响的杂念值
    m_Buddhism.UpdateDistract( GlobalSetup::GetSetup()->lDryValue * m_Buddhism.RetDryNum() );
    
    /// 增加一个燥
    m_Buddhism.UpdateDryNum( 1 , GlobalSetup::GetSetup()->lDryCount );

    /// Cancel m_Buddhism.lDryTimer
    if( m_Buddhism.lDryTimer > 0 )
         TIMER_CANCEL( m_Buddhism.lDryTimer );

    /// 注册定时器
    RegisterDryTimer();

#ifdef  _DEBUG
    char  szText[256];
    _snprintf(szText,256,"[Colour 'ffffffff'] 杂念值=%d,燥的数量=%d,结束时间=%d",m_Buddhism.lDistract,m_Buddhism.lDryNum,m_Buddhism.lDryEndTime);
    SendNotifyMessage( szText, eNOTIFYPOS_LEFT_BOTTOM );
#endif

    /// 添加杂念值
    AddAttributeToDirty("lDistract");
    /// 添加燥的数量 
    AddAttributeToDirty("lDryNum");
    /// 结束时间
    AddAttributeToDirty("lDryEndTime");

    UpdateAttribute(1);
}

CPlayer::eProgress CPlayer::GetCurrentProgress()
{
	return m_eProgress;
}

void CPlayer::SetCurrentProgress( CPlayer::eProgress ep )
{
	int nOldProgress=m_eProgress;
	m_eProgress = ep;
	GetLogicLogInterface()->logT902_player_progress_chg_log(this,nOldProgress,ep);
}

bool CPlayer::IsLockPlayerState()  
{
    if ( GetPhaseID() == NULL_GUID && GetCurrentProgress() == CPlayer::PROGRESS_FOLLOW_LEADER )
        return true;
    if ( GetPhaseID() != NULL_GUID && GetState() == CShape::STATE_AGONAL )
        return true;
    return false;
}


void CPlayer::SendNotifyMessage(const char* szText,eNOTIFY_POS ePos)
{
	if( szText )
	{
		/*CMessage msg( MSG_S2C_OTHER_ADDINFO );
		msg.Add( dwColor );
		msg.Add( dwBkColor );
		msg.Add((BYTE)ePos);
		msg.Add( szText );
		msg.SendToPlayer(GetExID(),false);*/
		/***********************************************************************/
		/*  zhaohang 2010-11-8 fix 直接剔除颜色，颜色加入字符串中，客户端能自动解析*/
		CMessage msg( MSG_S2C_OTHER_ADDINFO );
		//msg.Add( dwColor );
		//msg.Add( dwBkColor );
		msg.Add((BYTE)ePos);
		msg.Add( szText );
		msg.SendToPlayer(GetExID(),false);
		/***********************************************************************/
	}
}
void CPlayer::SendPlayerInfo(const char* szText, DWORD dwColor /* = 0xffffffff  */,DWORD dwBkColor)
{
	if (szText)
	{
		CMessage msg(MSG_S2C_OTHER_ADDPLAYERINFO);
		msg.Add((LONG)0);
		msg.Add(dwColor);
		msg.Add(dwBkColor);
		msg.Add(szText);
		msg.SendToPlayer(GetExID(),false);

	}	
}

void CPlayer::SendSystemInfo(const char* szText, DWORD dwColor )
{
	if( szText )
	{
		CMessage msg( MSG_S2C_OTHER_ADDSYSTEMINFO );
		msg.Add( dwColor );
		msg.Add( szText );
		msg.SendToPlayer(GetExID());
	}
}


void CPlayer::SendOtherInfo(const char* szText )
{
	if( szText )
	{
		CMessage msg(MSG_S2C_OTHER_TALK_NOTIFY);
		msg.Add(BYTE(0));
		msg.Add(szText);
		msg.SendToPlayer(GetExID(),false);
	}
}
BOOL CPlayer::DelSkill( long lSkillID,long lSkillLv )
{	
	itSkill it = m_Skills.find(lSkillID);
	if (it!=m_Skills.end())
	{
		m_Skills.erase(it);		
	}
	else
	{
		return FALSE;
	}
	EndPassiveSkill(lSkillID,lSkillLv);

	char tmp[32];
	for(int i=0; i<24; ++i)
	{
		_snprintf(tmp, 32, "dwHotKey%d", i);
		DWORD dwHotKey = GetAttribute((string)tmp);
		if (dwHotKey!=0)
		{
			if ((dwHotKey&0x80000000)!=0)	// 物品
			{
				DWORD dwSkillID = dwHotKey & 0x7fffffff;
				if (dwSkillID==lSkillID)
				{
					SetAttribute((string)tmp,0);
					UpdateAttribute();
					//向客户端发送清除快捷键消息
					CMessage msg(MSG_S2C_SETHOTKEY);
					msg.Add((char)HOTKEY_CLEAR_SUCCEED);
					msg.Add((BYTE)i);
					msg.Add((DWORD)0);
					msg.SendToPlayer(GetExID());
				}
			}				
		}
	}

	CMessage msg(MSG_S2C_PLAYER_DELSKILL);
	msg.Add((DWORD)lSkillID);
	msg.SendToPlayer(GetExID());

    /// 删除技能的时候，清空天赋配置表
    m_GeniusAction->Remove( Skill_Remove , lSkillID , 1);
	return TRUE;
}

long  CPlayer::GetGeniusLvl(long Geniusid)
{
    std::map<long,long>::iterator itr = m_GeniusTable.find( Geniusid );
    if ( itr != m_GeniusTable.end() )
    {
        return itr->second;
    }
    return -1;
}

/// 移除天赋
void  CPlayer::RemoveGenius(long Geniusid)
{
    std::map<long,long>::iterator itr = m_GeniusTable.find( Geniusid );
    if ( itr != m_GeniusTable.end() )
    {
        long   lvl = itr->second ;
        m_GeniusTable.erase( itr );
        m_GeniusAction->Remove( Genius_Remove , Geniusid , lvl );
    }
}

void   CPlayer::StudyGenius(long Geniusid,long lvl)
{
    std::map<long,long>::iterator itr = m_GeniusTable.find( Geniusid );
    if ( itr != m_GeniusTable.end() )
    {
        itr->second = lvl;
        m_GeniusAction->Update(Genius_Update,Geniusid,lvl);
        return;
    }
    m_GeniusAction->AddGenius(Geniusid,lvl);
}

void CPlayer::IncreaseContinuousKill()
{
	//##获取全局的设置
	GlobalSetup::tagSetup* pSetup = GlobalSetup::GetSetup();
	if( pSetup == NULL ) return;

	//##如果在设定时间内杀死怪物
	if(  (long)(timeGetTime() - m_dwContinuousKillTimeStamp) < pSetup -> lHitTime )
	{
		++ m_dwContinuousKillAmount;
		//##连击等级的检查
		//if( m_dwContinuousKillAmount > GetHitTopLog() )
		//{
		//	vector<HitLevelSetup::tagHitLevel>::iterator iter = 
		//		HitLevelSetup::GetHitLevelVec() -> begin();
		//	for( ; iter != HitLevelSetup::GetHitLevelVec() -> end(); iter ++ )
		//	{
		//		if( m_dwContinuousKillAmount == iter -> dwHit )
		//		{
		//			//##设置新的连击等级数和经验
		//			SetHitTopLog( static_cast<WORD>(iter -> dwHit) );
		//			//SetExp( GetExp() + iter -> dwExp );
		//			IncExp(iter -> dwExp );

		//			//##检查是否升级
		//			CheckLevel();

		//			//##发送连击奖励消息
		//			CMessage msg( MSG_S2C_PLAYER_HITLEVELUPGRADE );
		//			msg.Add( GetHitTopLog() );
		//			msg.SendToPlayer(GetExID());
		//			break;
		//		}
		//	}
		//}
	}
	else
	{
		m_dwContinuousKillAmount = 0;
	}

	m_dwContinuousKillTimeStamp = timeGetTime();

	//##发送连击数给客户端
	CMessage msg( MSG_S2C_PLAYER_SYNC_HITS );
	msg.Add( m_dwContinuousKillAmount );
	msg.SendToPlayer(GetExID());
}

//添加一个移动消息
void CPlayer::AddMoveMsg(float fNewX,float fNewY)
{
	float fXOffset = fNewX - GetPosX();
	float fYOffset = fNewY - GetPosY();
	float fDis = sqrt( fYOffset * fYOffset + fXOffset * fXOffset );

	m_fMoveValiTotalDistance += fDis;
	//如果总共收到一定个数消息以后，进行验证。
	++m_fMoveValiRecvMsgCount;

	DWORD dwCurTime = timeGetTime();
	if( (dwCurTime-m_dwLastRecvMsgTime) == 0)	return;

	float fMoveSpeed = fDis/(float)(dwCurTime-m_dwLastRecvMsgTime);
	if(fMoveSpeed > m_fMaxPeriodMoveSpeed)
		m_fMaxPeriodMoveSpeed = fMoveSpeed;
		
		

//#ifdef _DEBUG	
//	char pszStackInfo1[256]="";
//	_snprintf(pszStackInfo1,256,"总距离:%f,距离:%f,m_fMaxPeriodMoveSpeed:%f,fMoveSpeed:%f,时间:%d,标志:%d \n",m_fMoveValiTotalDistance,fDis,m_fMaxPeriodMoveSpeed,fMoveSpeed,dwCurTime-m_dwLastRecvMsgTime,lFlag);
//	OutputDebugString(pszStackInfo1);
//#endif
	m_dwLastRecvMsgTime = dwCurTime;
}
//移动验证,包括移动非法处理
bool CPlayer::CheckMoveSpeedValidate()
{
	//为了验证的有效性,时间大于等于秒才验证
	float fTimeDif = (float)(timeGetTime()-m_dwMoveValiStartTime);

	//float fTotalMoveDis = (float)dwTimeDif*m_fMoveValiStartSpeed;
	//估计应该接受消息的个数
	float fTotalMsgCount = GlobalSetup::GetSetup()->fMaxMsgCountPerSecond*(fTimeDif/1000.00f+0.5f);
	if(fTotalMsgCount < m_fMoveValiRecvMsgCount )
	{
		char pszGuid[256]="";
		GetExID().tostring(pszGuid);
		Log4c::Trace(ROOT_MODULE,"(PlayerID:%s)Move Msg Count Error,fTimeDif:%f,MoveValiRecvMsgCount:%f,MaxRecvMsgCount:%f",pszGuid,fTimeDif,m_fMoveValiRecvMsgCount,fTotalMsgCount);
		//把玩家踢下线
		GetGame()->KickPlayer(GetExID());
		return false;
	}
	//超过误差
	float fAverSpeed = m_fMoveValiStartSpeed;
	if(fTimeDif != 0.0f)
		fAverSpeed = m_fMoveValiTotalDistance/fTimeDif;
	if( (fAverSpeed-m_fMoveValiStartSpeed) >= 0.0005f &&
		(m_fMaxPeriodMoveSpeed-m_fMoveValiStartSpeed) > 0.0008)
	{
		if(m_guMoveValiStartRgnID != GetRegionGUID())
		{
			char pszGuid[256]="";
			GetExID().tostring(pszGuid);
			Log4c::Warn(ROOT_MODULE, FormatText("GS_PLAYER_0",pszGuid));//Move Region Error,Kick Player PlayerID[%s].
			GetGame()->KickPlayer(GetExID());	
		}
		else if( (fAverSpeed-m_fMoveValiStartSpeed) > GlobalSetup::GetSetup()->fMaxMistakeAverSpeed ||
			(m_fMaxPeriodMoveSpeed-m_fMoveValiStartSpeed) > GlobalSetup::GetSetup()->fMaxMistakeMaxSpeed )
		{
			char pszGuid[256]="";
			GetExID().tostring(pszGuid);
			Log4c::Trace(ROOT_MODULE,"PlayerID[%s] Move Speed Error,TimeDif:%f,RealMoveDis:%f,AverSpeed:%f,MaxPeriodMoveSpeed:%f,MoveValiStartSpeed:%f",
				pszGuid,fTimeDif,m_fMoveValiTotalDistance,fAverSpeed,m_fMaxPeriodMoveSpeed,m_fMoveValiStartSpeed);		
			GetGame()->KickPlayer(GetExID());	
		}
		else
			SetPosition(m_lMoveValiStartPosX,m_lMoveValiStartPosY);
			return false;
		}

	ResetMoveValiData();	
	return true;
}

//重设相关数据
void CPlayer::ResetMoveValiData()
{
	m_dwMoveValiStartTime=timeGetTime();
	//m_dwLastRecvMsgTime = m_dwMoveValiStartTime;
	m_fMaxPeriodMoveSpeed = 0.0f;
	m_guMoveValiStartRgnID = GetRegionGUID();
	m_lMoveValiStartPosX=GetTileX();//开始移动的X位置
	m_lMoveValiStartPosY=GetTileY();//开始移动的Y位置
	m_fMoveValiStartSpeed = GetSpeed();
	m_fMoveValiTotalDistance=0.0f;//移动的总距离
	m_fMoveValiRecvMsgCount=0;//收到的移动消息个数
}

//验证每一步的移动
bool CPlayer::CheckMoveStep(float fCurX,float fCurY)
{
	CServerRegion *pRegion = (CServerRegion*)GetFather();
	if(NULL == pRegion || pRegion->GetType() != TYPE_REGION)
		return false;
	long lSX = GetTileX();
	long lSY = GetTileY();
	int nMaxDis = max((abs(lSX-(long)fCurX)),(abs(lSY-(long)fCurY)));
	if(nMaxDis > 2)
		return false;
	//long lDir = GetLineDir(lSX,lSY,fCurX,fCurY);
	//判断阻挡
	BYTE byBlock = pRegion->GetBlock((int)fCurX,(int)fCurY);	
	if(byBlock != CRegion::BLOCK_NO && byBlock != CRegion::BLOCK_AIM)
	{
		return false;
	}
	/*nMaxDis--;
	while(nMaxDis>0)
	{
	long lDX = lSX + nMaxDis*_dir[lDir][0];
	long lDY = lSY + nMaxDis*_dir[lDir][1];
	byBlock = pRegion->GetBlock(lDX,lDY);	
	if(byBlock != CRegion::BLOCK_NO && byBlock != CRegion::BLOCK_AIM)
	{
	return false;
	}
	nMaxDis--;
	}*/


	/*CServerRegion *pRegion = (CServerRegion*)GetFather();
	if(NULL == pRegion || pRegion->GetType() != TYPE_REGION)
	return false;
	float fSX = GetPosX();
	float fSY = GetPosY();
	float fDistance = Distance(fSX,fSY,fCurX,fCurY);
	if(0.0f == fDistance)	return true;
	float fRadianDir = GetRadianDir(fSX,fSY,fCurX,fCurY);	
	float fcos= cosf(fRadianDir);
	float fsin= sinf(fRadianDir);

	float fCount = 0.0f;
	int nBlockNum = 0;
	while(fCount <= fDistance)
	{
	float fx = fSX + fCount * fcos;			
	float fy = fSY + fCount * fsin;
	BYTE byBlock = pRegion->GetBlock(fx,fy);
	if(byBlock != CRegion::BLOCK_NO && byBlock != CRegion::BLOCK_AIM)
	nBlockNum++;
	else
	nBlockNum = 0;
	if(nBlockNum >= 2)
	break;
	fCount += 0.5f;
	}
	if(nBlockNum >= 2)
	return false;*/
	return true;
}
// 处理行走请求
void CPlayer::OnQuestMoveStep(float fCurX,float fCurY,float fDestX,float fDestY)
{
#ifdef _RUNSTACKINFO3_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CPlayer::OnQuestMoveStep() Start.");
#endif

	if (!CanMove())
	{
		OnCannotMove();
		return;
	}

	if (!GetInstruction())
	{
		//不能使用玩家使用指令时直返回
		return;
	}

	if(!CheckMoveStep(fCurX,fCurY) )
	{
		OnCannotMove();
		return;
	}
	StateAI::BaseAI* pAI = GetAI();
	if( NULL == pAI )
	{
		OnCannotMove();
		return;
	}
	//添加一个验证消息数据
	AddMoveMsg(fCurX,fCurY);

	long lDir = GetLineDir(GetTileX(),GetTileY(),(int)fCurX,(int)fCurY);
	SetDir(lDir);
#ifdef _RUNSTACKINFO3_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CPlayer::OnQuestMoveStep() SetPosXY.");
#endif
	SetPosXY(fCurX,fCurY);
#ifdef _RUNSTACKINFO3_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CPlayer::OnQuestMoveStep() MoveTo.");
#endif
	pAI->OnMove( fDestX,fDestY);		

	if(m_fMoveValiRecvMsgCount > MoveValiMaxMsgCount)
	{
		CheckMoveSpeedValidate();
	}
	//char strDebug[256];
	//_snprintf( strDebug, "Client move to: %d, %d \n", GetTileX(), GetTileY() );
	//OutputDebugString( strDebug );
#ifdef _RUNSTACKINFO3_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CPlayer::OnQuestMoveStep() End.");
#endif

}

void CPlayer::OnQuestStopMove(float fCurX,float fCurY,short lDir)
{
#ifdef _RUNSTACKINFO3_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CPlayer::OnQuestStopMove() CheckMoveStep.");
#endif
	if(!CheckMoveStep(fCurX,fCurY) )
	{
		OnCannotMove();
		return;
	}
	//添加一个验证消息数据
	AddMoveMsg(fCurX,fCurY);

	SetDir(lDir);
	SetPosXY(fCurX,fCurY);	
	OnStopMove();

	CheckMoveSpeedValidate();

#ifdef _RUNSTACKINFO3_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CPlayer::OnQuestStopMove() CheckSwitchPoint.");
#endif
	//检查切换点
	CheckSwitchPoint();

#ifdef _RUNSTACKINFO3_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CPlayer::OnQuestStopMove() end.");
#endif
}


//是否能移动
bool  CPlayer::CanMove()
{
	/*if(m_lCityWarDiedState == true)
	return false;*/
	return CMoveShape::CanMove() > 0 ;
}

void CPlayer::OnCannotMove()
{
	CMessage msg( MSG_S2C_SHAPE_SETPOS );
	msg.Add( GetType() );
	msg.Add( GetExID());
	msg.Add( (GetPosX()) );
	msg.Add( (GetPosY()) );
	msg.Add( static_cast<LONG>(0) );
	msg.SendToPlayer(GetExID());
}

void CPlayer::OnExitRegion()
{
	CServerRegion* pRegion = dynamic_cast<CServerRegion*>( GetFather() );
	if( pRegion )
	{
		// 如果是副本则检查副本当前人数 == 0,开启删除副本定时器
		if((eRgnType)pRegion->GetRgnType() != RGN_NORMAL && (eRgnType)pRegion->GetRgnType() != RGN_PERSONAL_HOUSE)
		{
			// 人数--
			//			pRegion->SetCurDupRgnPlayerNum(pRegion->GetCurDupRgnPlayerNum()-1);
			if(pRegion->GetPlayerAmout() == 1)
			{
				//	pRegion->SetRgnDelTimerID(GameManager::GetInstance()->GetRgnManager()->GetRegionTimer()->Schedule(pRegion, NULL, timeGetTime()+GetGame()->GetSetup()->lPDupRgnDelTime));
				pRegion->RegisterDeleteRgnTimerID(GlobalSetup::GetSetup()->lPDupRgnDelTime);
			}
		}
		//执行退出场景脚本
		const char* pszExitRegionSctipt = CRegionSetup::GetExitRegionScript(pRegion->GetID());
		PlayerRunScript((char*)pszExitRegionSctipt);
	}

	DropParticularGoodsWhenRecall();
	ProduceEvent(ET_ExitRegion);
	ProduceEvent(ET_EventBreak);

	//清除所有提示
	ClearAllHint();
	//清除脚本定时器
	ClearScriptTimer();

}


//当玩家等级发生变化的时候触发，主要指的是升级
void CPlayer::OnLevelChanged()
{
	//重设战斗属性的基本属性的属性
	ResetBaseFightProperty();
	//重设其他战斗属性
	ResetFightProperty();
	//升级时触发脚本
	const char* strScript = GlobalSetup::GetSetup()->pszPlayerUpLevelScript;
	if(strScript)
	{
		//stRunScript sScript;
		//sScript.pszFileName = (char*)strScript;
		//sScript.desShape = this;
		//sScript.pRegion =  dynamic_cast<CRegion*>(GetFather());
		//sScript.srcShape	= this;
		//RunScript( &sScript, const_cast<CHAR*>( GetGame() -> GetScriptFileData(strScript) ) );
		Script::RunEnv env( this, this, static_cast<CRegion*>( GetFather() ) );
		GetInst( ScriptSys ).RunScript( env, strScript );
	}
	GSTeam *pTeam = (GSTeam*)GetOrganiCtrl()->GetGameTeamByID(GetTeamID());
	if(NULL != pTeam)
	{
		pTeam->OnPlayerIdioinfoChange(GetExID(), eMIS_MaxHp);
		pTeam->OnPlayerIdioinfoChange(GetExID(), eMIS_MaxMp);
	}
	SetHP(GetMaxHP());
	SetMP(GetMaxMP());
	SetPKCP( GetMaxPKCP() ); // 重置PK CP值

	if(NULL != pTeam)
	{
		pTeam->OnPlayerIdioinfoChange(GetExID(), eMIS_CurrHp);
		pTeam->OnPlayerIdioinfoChange(GetExID(), eMIS_CurrMp);	
	}



	GetGame()->UpdateSavePlayer(this, SAVE_DETAIL_ATTRPROPERTY);
	//AddPetsLev();

	if( GetLevel() == GlobalSetup::GetSetup()->m_btNewbieLevelLimit + 1 )
	{
		SendNotifyMessage( "从现在开始，你的等级已经超过新手保护规则规定的等级，死亡后将不再受到保护。" );
	}
	else if( GetLevel() == GlobalSetup::GetSetup()->m_btNewSoldierLevel + 1 )
	{
		SendNotifyMessage( "从现在开始，你的等级已经超过新兵保护规则规定的等级。" );
	}
}

//当玩家职业等级增发生变化的时候，主要指职业等级增加的时候
void CPlayer::OnOccuLevelChanged()
{
	//重设战斗属性的基本属性的属性
	ResetBaseFightProperty();
	//UpdateAttribute(1);
	//重设其他战斗属性
	ResetFightProperty();
	//UpdateAttribute(1);
}


void CPlayer::OnEnterRegion(bool bIsRelive)
{
	CMoveShape::OnEnterRegion(bIsRelive);

	AddNewState(this,STATE_BASE_GOD,1,18);
	CServerRegion* pRegion = dynamic_cast<CServerRegion*>( GetFather() );
	if( pRegion )
	{
        //通知chat server该玩家切换场景
        CMessage csmsg(MSG_S2CS_PLAYER_CHANGE_REGION);
        csmsg.Add(GetExID());
        csmsg.Add(pRegion->GetID());
        csmsg.SendToChatServer();

		// 设置隐匿状态
		if(pRegion->GetRgnHideFlag() == 2)
		{
			SetHideFlag(0);
		}
		else if(pRegion->GetRgnHideFlag() == 0 && GetCanHide() == 0)
		{
			SetHideFlag(0);
		}
		else if( pRegion->GetRgnHideFlag() != 2 && (pRegion->GetRgnHideFlag() == 1 || GetCanHide() == 1) )
		{
			SetHideFlag(1);
		}

		// 设置据点战功能PK规则
		if(pRegion->GetStrongPointFlag() == true) // 开启据点战功能
		{
			SetPk_Camp(true);
			SetPk_Country(true);
			SetPk_Team(true);
			SetPk_Union(true);
			SetPk_Badman(false);
			SetPk_Pirate(false);
			SetPk_Normal(false);
		}

		ProduceEvent(ET_EnterRegion);
		SetCurRgnType((eRgnType)pRegion->GetRgnType());
		//执行进入该场景的脚本
		const char* pszEnterRegionSctipt = CRegionSetup::GetEnterRegionScript(pRegion->GetID());
		PlayerRunScript((char*)pszEnterRegionSctipt);

		//不对GM做判断
		if(!IsGM())
		{
			//! 队伍相关
			GSTeam *pTeam = GetOrganiCtrl()->GetGameTeamByID(GetTeamID());
			if(NULL != pTeam) // 玩家在队伍中
			{
				pTeam->MemberMapChange(GetExID());

				if(pRegion->GetRgnType() == RGN_TEAM)
				{
					// 判断队长的副本RgnID,是否清人
					if(this->GetExID() != pTeam->GetMasterID())
					{
						if( pRegion->GetExID() != GameManager::GetInstance()->GetRgnManager()->GetTeamRgnGUID(pTeam->GetMasterID(), pRegion->GetID()) )
						{
							RegisterRgnKickPlayerTimerID(GlobalSetup::GetSetup()->lRgnKickPlayerDeltaTime);
							CMessage msg(MSG_S2C_OTHER_TIME_INFO_BOX);
							msg.Add((BYTE)RGN_CLEARDUPRGNPLAYER);
							msg.Add((long)GlobalSetup::GetSetup()->lRgnKickPlayerDeltaTime);
							msg.SendToPlayer(GetExID());
							return;
						}
					}
				}
				if(pRegion->GetRgnType() == RGN_NORMAL && pRegion->GetStrongPointFlag() == true) // 是开启据点的普通场景
				{
					// 取消组队
					CMessage msg(MSG_S2W_TEAM_KICK_PLAYER);
					msg.Add(GetExID());
					msg.Add(GetExID());
					msg.Send(false);
				}
			}
			else
			{
				// 判断自己的副本RgnID,是否清人
				if( pRegion->GetRgnType() == RGN_TEAM && (GameManager::GetInstance()->GetRgnManager()->GetTeamRgnGUID(GetExID(), pRegion->GetID()) != pRegion->GetExID()) )
				{
					RegisterRgnKickPlayerTimerID(GlobalSetup::GetSetup()->lRgnKickPlayerDeltaTime);
					CMessage msg(MSG_S2C_OTHER_TIME_INFO_BOX);
					msg.Add((BYTE)RGN_CLEARDUPRGNPLAYER);
					msg.Add((long)GlobalSetup::GetSetup()->lRgnKickPlayerDeltaTime);
					msg.SendToPlayer(GetExID());
				}
			}
		}
	}
}



//当进入GS触发,包括第一次进入GS，或者以后的切换服务器
bool CPlayer::OnEnter()
{
#ifdef _RUNSTACKINFO_
	char pszStatckInfo[1024]="";
	_snprintf(pszStatckInfo,1024,"CPlayer::OnEnter(name:%s,RegionID:%d,x:%d,y:%d) Start",GetName(),GetRegionID(),GetTileX(),GetTileY());
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStatckInfo);
#endif
	string strRegionName = "未知场景";
	string strOccu = "Occu";

	CGUID tGUID;
	CServerRegion* pRegion = NULL;
	char strRegionGUID[50]="";
	GetRegionGUID().tostring(strRegionGUID);

	pRegion = GameManager::GetInstance()->GetRgnManager()->FindRgnByRgnGUID(GetRegionGUID());
	long lTaxRate = 0;
	eRWT lWarRegionType = RWT_Normal;
	long lResourceID = 0;
	float fExpScale = 1.0;
	if( pRegion )
	{
		lTaxRate = pRegion->GetTaxRate();
		lWarRegionType = pRegion->GetWarType();
		lResourceID = pRegion->GetResourceID();
		fExpScale = pRegion->GetExpScale();
		long x, y;

		//得到当前HP和MP,在装备和状态初始化之后,重新设置这两个值
		DWORD dwHp = GetHP();
		WORD wMp = GetMP();
#ifdef _RUNSTACKINFO3_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"SetPosXY Start");
#endif
		// 产生随机点
		if( GetTileX() == 0 && GetTileY() == 0 )
		{
			pRegion->GetRandomPos(x, y);
			CShape::SetPosXY(x+0.5f,y+0.5f);
		}

		//如果出生的地方有阻挡,那么在周围找寻空格子
		if (pRegion->GetBlock(GetTileX(), GetTileY()) != CRegion::BLOCK_NO)
		{
			pRegion->GetRandomPosInRange(x, y, GetTileX(), GetTileY(), 3,3);
			CShape::SetPosXY(x+0.5f,y+0.5f);
		}

		//设置城战上线的正确位置 
		pRegion->SetEnterPosXY(this);

		SetFather(pRegion);
		strRegionName = pRegion->GetName();	

#ifdef _RUNSTACKINFO3_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"InitProperty Start");
#endif
		InitProperty();
		//InitSkills();
		MountAllEquip();

#ifdef _RUNSTACKINFO3_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"ActivePets Start");
#endif

#ifdef _RUNSTACKINFO3_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"Send MSG_S2C_LOG_ANSWER_PLAYERDATA Start");
#endif
		CMessage msg(MSG_S2C_LOG_ANSWER_PLAYERDATA);
		msg.Add(GetSignCode());
		msg.Add(1L);
		msg.Add(GetExID());
		// 全局参数
		msg.AddEx(GlobalSetup::GetSetup(), sizeof(float)*33);
		msg.Add(GlobalSetup::GetSetup()->lHitTime);
		msg.Add(GlobalSetup::GetSetup()->wBaseRp_Lv1);
		msg.Add(GlobalSetup::GetSetup()->wBaseRp_Lv2);
		msg.Add((WORD)GlobalSetup::GetSetup()->dwPkCountPerKill);
		msg.Add((float)(GlobalSetup::GetSetup()->fBasePriceRate));
		msg.Add((float)(GlobalSetup::GetSetup()->fTradeInRate));
		msg.Add((float)(GlobalSetup::GetSetup()->fRepairFactor));

		msg.Add(GlobalSetup::GetSetup()->szTalkCountryGoodsName);
		msg.Add((long)GlobalSetup::GetSetup()->nTalkCountryGoodsNum);
		msg.Add(GlobalSetup::GetSetup()->lTalkCountryGold);
		//msg.Add(GlobalSetup::GetSetup()->lTalkCountrySilver);

		msg.Add(GlobalSetup::GetSetup()->szTalkWorldGoodsName);
		msg.Add((long)GlobalSetup::GetSetup()->nTalkWorldGoodsNum);
		msg.Add(GlobalSetup::GetSetup()->lTalkWorldGold);

		ContributeSetup::tagContributePara* ContrbutePara = ContributeSetup::GetContributeParaList();
		msg.Add((BYTE)ContrbutePara->lCombatLevel1);
		msg.Add((BYTE)ContrbutePara->lCombatLevel2);

		msg.Add(AREA_WIDTH);
		msg.Add(AREA_HEIGHT);
		AddFightPropertyTODirty();
		DBWriteSet setWriteDB;
		msg.GetDBWriteSet(setWriteDB);
		CodeToDataBlock_ForClient(setWriteDB);
		msg.Add(const_cast<char*>(strRegionName.c_str()));
		msg.Add(lTaxRate);
		msg.Add((BYTE)lWarRegionType);
		msg.Add((BYTE)pRegion->GetSpaceType());
		msg.Add((BYTE)pRegion->IsChangeEquip());
		msg.Add(pRegion->GetExID());
		msg.Add(pRegion->GetID());
		msg.Add(lResourceID);
		msg.Add(fExpScale);
		msg.Add((BYTE)(pRegion)->IsNoPk());
		GameManager::GetInstance()->GetRgnManager()->AddLinedRgnInfoToByteArray(pRegion->GetLinedIdFlag(), setWriteDB);
		//发送服务器时间 by tanglei
		time_t t = time(NULL);
		msg.Add((DWORD)t);//end
		// 发送当前场景的据点信息
		msg.Add((BYTE)pRegion->GetStrongPointFlag()); // 据点战是否开启
		if(pRegion->GetStrongPointFlag() == false) // 未开启据点战
		{
			msg.Add((long)0); // 据点个数
		}
		else
		{
			msg.Add((long)pRegion->GetStronPointNum()); // 据点个数
			std::vector<CServerRegion::tagStrongPoint>::iterator spItr = pRegion->GetStrongPointVec().begin();
			for(; spItr != pRegion->GetStrongPointVec().end(); spItr++)
			{
				msg.Add((*spItr).szName);//据点名字
				msg.Add((long)(*spItr).lPosX); //据点X坐标
				msg.Add((long)(*spItr).lPosY); //据点Y坐标
				msg.Add((long)(*spItr).lState); //据点状态
			}
		}
#ifdef _DEBUG
        ulong lTotalSize = msg.GetSize();
        char szOut[128];
        sprintf(szOut, "the player %s msg totalsize %u.\n", GetName(),lTotalSize);
        OutputDebugString(szOut);
#endif
		msg.SendToPlayer(GetExID(),false);


        /// 需要进行详细指定与客户交互形势
        if ( GetPhaseID() != NULL_GUID )
        {
            PHASEMAN.OnPhaseOper( GetPhaseID() , this , eReJoin );
        }

#ifdef _RUNSTACKINFO3_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"pRegion->PlayerEnter() Start");
#endif
		//处理玩家进入场景
		pRegion->PlayerEnter(this,0);
		// 宠物进入场景
		if( GetActivePetGUID() != NULL_GUID && !IsDied() )
		{
			ActivatePet();
		}

#ifdef _RUNSTACKINFO3_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"StartAllBuff() Start");
#endif
		StartAllBuff();

		StartUpPassiveSkill();

		ProduceEvent(ET_EnterRegion);

		RegisterPKValue();
		RegisterPvPValue();
		RegisterAutoReNewHpMp();
		//RegisterEnergyUpdate(PlayerList::GetOccuValueRatio("baseenergyrecovertime",GetAttribute(strOccu)));
		//初始化俑兵任务数据
		IniMercQuest();
		//发送系统邮件
		//GameManager::GetInstance()->GetMailManager()->SendSysMailToReciver(this);
#ifdef _RUNSTACKINFO3_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"SendSysMailMsgToReciver() Start");
#endif
		GameManager::GetInstance()->GetMailManager()->SendSysMailMsgToReciver(this);
		//发送系统邮件
		GameManager::GetInstance()->GetMailManager()->SendSysMailToReciver(this);

		//判断是否第一次登录给该玩家出发登陆脚本
		if(GetFirstLogin())
		{
			SetFirstLogin(false);
#ifdef _RUNSTACKINFO3_
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"OnEnterGame() Start");
#endif
			OnEnterGame();
		}

		// 判断时候有副本数据,开启副本创建定时器
		if(GetCurPRgnNum() > 0)
			RegisterCreatePRgnTimerID(GlobalSetup::GetSetup()->lCreateDupRgnDeltaTime);
		if(GetCurTRgnNum() > 0)
			RegisterCreateTRgnTimerID(GlobalSetup::GetSetup()->lCreateDupRgnDeltaTime);

		//重设HP和MP
		if(GetState() != STATE_DIED && dwHp == 0)
		{
			Log4c::Warn(ROOT_MODULE, FormatText("GS_PLAYER_1",GetName(),GetState(),dwHp));//玩家状态错误(Name:%s,State:%d,Hp:%d)
			SetState(STATE_PEACE);
			dwHp = GetMaxHP()/2;
		}
		SetHP(dwHp);AddAttributeToDirty("Hp");
		SetMP(wMp);AddAttributeToDirty("Mp");
		//表示进入游戏
		SetEnterGame(true);
		UpdateAttribute(1);

		/// 添加反外挂模块,玩家进入游戏启动定时器 @Expter20090602
		if (!GetInst(AntiCheat::CPlayerManager).PlayerEnter(this))
		{
			return false;
		}
        RegisterBuddhismTimer();
	}
	else
	{
		CMessage retmsg(MSG_S2C_LOG_ANSWER_PLAYERDATA);
		retmsg.Add(GetSignCode());
		retmsg.Add(0L);
		retmsg.Add(GetExID());
		retmsg.SendToPlayer(GetExID(), false);
		Log4c::Trace(ROOT_MODULE,"玩家登陆,进入到GS,未找到场景Name:%s,RegionID:%d,regionguid:%s,PosX:%d,PosY:%d",
			GetName(),GetRegionID(),strRegionGUID,GetTileX(),GetTileY());
		return false;
	}
#ifdef _RUNSTACKINFO_
	_snprintf(pszStatckInfo,1024,"CPlayer::OnEnter(name:%s) End",GetName());
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStatckInfo);
#endif
	return true;
}
//启动被动技能
void CPlayer::StartUpPassiveSkill()
{
	map<long,tagSkill>::iterator it = m_Skills.begin();
	for(;it!=m_Skills.end();++it)
	{
		OnBeginSkill(it->second.dwID,it->second.wLevel);
	}
}

//使用物品触发技能
bool CPlayer::ItemBeginSkill(DWORD dwID, DWORD dwLevel, CMoveShape *pDestShape)
{

	if(!AddActiveSkill(dwID,dwLevel))
	{
		return false;
	}

	if (NULL == m_pInstanceSkill || !GetAI()->BeginSkill(m_pInstanceSkill,pDestShape->GetTileX(),pDestShape->GetTileY(),pDestShape))
	{
		return false;
	}
	else
	{
		return true;
	}	
}
//当进入游戏时触发,第一次登陆的时候触发，以后切换服务器不会触发
void CPlayer::OnEnterGame(void)
{
	SetPlayerLogin(true);
	const char* strScript =GlobalSetup::GetSetup()->pszPlayerLoginScript;
	if(strScript)
	{
		PlayerRunScript((char*)strScript);
	}
	//初始化赠送经验
	InitPresentExp();

	ProduceEvent(ET_EnterGame);

    // validate player for chat server
    SendValidateMsgForCs();
}

//玩家选择退出游戏时退出GS触发，包括切换法服务器的退出GS也触发
void CPlayer::OnExit( BOOL bOnChangeServer )
{
#ifdef _RUNSTACKINFO_
	char pszStatckInfo[1024]="";
	char pszGUID[50]="";
	m_guid.tostring(pszGUID);
	_snprintf(pszStatckInfo,1024,"CPlayer::OnExit(PlayerID:%s) Start",pszGUID);
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStatckInfo);
#endif
	// 清楚时间效验
	GetGame()->RemoveValidateTime(GetExID());

#ifdef _RUNSTACKINFO1_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(), "OnExit() step 1");
#endif
	//清除所有的脚本
	//DelectAllScripByShape(GetExID());
	GetInst( ScriptSys ).PushDelScript( this );

#ifdef _RUNSTACKINFO1_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(), "OnExit() step 2");
#endif
	// 爆出下线必爆的物品
	DropParticularGoodsWhenLost();

	//释放俑兵任务数据 
	//ReleaseMercQuest();

	/**## 禁言的处理 */
	if( IsInSilence() )
	{
		m_lSilenceTime = ( m_lSilenceTimeStamp + m_lSilenceTime ) - timeGetTime();
	}

#ifdef _RUNSTACKINFO1_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(), "OnExit() step 3");
#endif
	CServerRegion* pRegion = dynamic_cast<CServerRegion*>(GetFather());
	// 玩家退出场景的消息
	/*CMessage Msg( MSG_S2C_RGN_DELSHAPE );
	Msg.Add( GetType() );
	Msg.Add( GetExID() );
	Msg.Add( (long)0 );
	Msg.Add( GetTileX() );
	Msg.Add( GetTileY() );
	Msg.SendToAround( this, this );*/
	if(pRegion)
	{
		// 玩家下线时，通知周围玩家清除Shape对象（包括玩家激活宠物对象）
		pRegion->DelShapeToAround(this);
		CPet* activePet = GetActivePet();
		if (activePet!=NULL)
		{
			pRegion->DelShapeToAround(activePet);
		}
	}

	////取消在攻城,据点战中的争夺状态
	//if(GetIsContendState() == true && pRegion)
	//{
	//	pRegion->CancelContendByPlayerID(this);
	//}

	// 不是在切换场景服务器,是离开游戏
	if( bOnChangeServer == FALSE )
	{
		//产生退出游戏事件事件
		ProduceEvent(ET_ExitGame);		
		ProduceEvent(ET_EventBreak);
	}

#ifdef _RUNSTACKINFO1_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(), "OnExit() step 4");
#endif
	//清除状态
	//ClearAllBuff();	
	//EndBuffAndDebuff();
	//清除技能
	ClearSkills();

#ifdef _RUNSTACKINFO1_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(), "OnExit() step 5");
#endif
	//如果角色死亡
	if( IsDied() || (pRegion && pRegion -> DoesRecallWhenLost(this)) )
	{
		if( IsDied() )
		{
			// 2009.01.07 ahc 修改 死亡回城的判断
			//SetHP(GetMaxHP()/2);	//设置其HP为满血的一半
			SetAction(CShape::ACT_STAND);
			SetPos(0);
		}
	}

#ifdef _RUNSTACKINFO1_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(), "OnExit() step 6");
#endif
	//！ 删除物品扩展操作会话
	GoodsExManage::GetInstance().CloseGoodsExWindow(this);

#ifdef _RUNSTACKINFO1_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(), "OnExit() step 7");
#endif
	//! 联系人系统响应
	LinkmanSystem::GetInstance().OnPlayerExit(GetExID());
#ifdef _RUNSTACKINFO1_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(), "OnExit() step 8");
#endif
	//释放邮件
	GameManager::GetInstance()->GetMailManager()->ReleaseMail(GetReceiveMailList());

	// 反外挂：如果是退出游戏，需要为玩家增加随机的活跃度，并且检查是否有拒绝答题行为
	if( !bOnChangeServer )
	{
		const AntiCheat::Range &r = GetInst( AntiCheat::CConfiger ).GetSystem().QuitActivityRange;
		GetInst( AntiCheat::CPlayerManager ).PlayerChangeActivity( GetExID(), 
			random( r.second - r.first + 1 ) + r.first, false );
		GetInst( AntiCheat::CPlayerManager ).CheckRefuseAnswer( GetExID(), false );
	}

    /// 离开相位触发离开操作.
    if( GetPhaseID() != NULL_GUID )
        PHASEMAN.OnPhaseOper( GetPhaseID(),this,eLeave );

#ifdef _RUNSTACKINFO1_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(), "OnExit() step 9");
#endif
	// 退出处理
	OnExitGame(bOnChangeServer);

	/// 添加反外挂模块,清除玩家当前信息 @Expter20090602
	//GetInst(AntiCheat::CPlayerManager).PlayerExit(this, bOnChangeServer == TRUE );

#ifdef _RUNSTACKINFO1_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(), "OnExit() step 10");
#endif


    UnCancelBuddhismTimer();
	UnRegisterAutoReNewHpMp();
	UnRegisterPKValue();
	UnRegisterPvPValue();
	//UnRegisterEnergyUpdate();
	UnRegisterReliveTimerID();

#ifdef _RUNSTACKINFO1_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(), "OnExit() step 11");
#endif
	CancelCreatePRgnTimer();
	CancelCreateTRgnTimer();
	CancelQueueTimer();
	CancelUpdateAttrTimer();
	CancelRgnKickPlayerTimer();
	
	m_GoodsTimerHandler.UnRegisterAll( this );
#ifdef _RUNSTACKINFO_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CPlayer::OnExit() End");
#endif
}



//PK开关(b0:全体保护)
BOOL	CPlayer::IsPk_Normal()	
{
	return (BOOL)(m_Property.byPKOnOff & 0x01);
}
void	CPlayer::SetPk_Normal(bool b)		
{
	if(b==true)
	{
		m_Property.byPKOnOff = m_Property.byPKOnOff | 0x01;
	}
	else
	{
		m_Property.byPKOnOff = m_Property.byPKOnOff & 0xfe;
	}
}
//b1:组队保护
BOOL	CPlayer::IsPk_Team()					
{
	return (BOOL)(m_Property.byPKOnOff & 0x02);
}
void	CPlayer::SetPk_Team(bool b)
{
	if(b==true)
		m_Property.byPKOnOff = m_Property.byPKOnOff | 0x02;
	else 
		m_Property.byPKOnOff = m_Property.byPKOnOff & 0xfd; 
}
//b2:工会保护,
BOOL	CPlayer::IsPk_Union()
{
	return (BOOL)(m_Property.byPKOnOff & 0x04);
}
void	CPlayer::SetPk_Union(bool b)
{
	if(b==true)
		m_Property.byPKOnOff = m_Property.byPKOnOff | 0x04;
	else 
		m_Property.byPKOnOff = m_Property.byPKOnOff & 0xfb; 
}
//b3:红名保护
BOOL	CPlayer::IsPk_Badman()				
{
	return (BOOL)(m_Property.byPKOnOff & 0x08);
}
void	CPlayer::SetPk_Badman(bool b)
{
	if(b==true)
		m_Property.byPKOnOff = m_Property.byPKOnOff | 0x08;
	else m_Property.byPKOnOff = m_Property.byPKOnOff & 0xf7;
}
//b4:本国保护
BOOL	CPlayer::IsPk_Country()
{
	return (BOOL)(m_Property.byPKOnOff & 0x10);
}
void	CPlayer::SetPk_Country(bool b)	
{
	if(b==true)
		m_Property.byPKOnOff = m_Property.byPKOnOff | 0x10;
	else
		m_Property.byPKOnOff = m_Property.byPKOnOff & 0xef; 
}
//b5:阵营保护
BOOL	CPlayer::IsPk_Camp()	
{
	return (BOOL)(m_Property.byPKOnOff & 0x20);
}
void	CPlayer::SetPk_Camp(bool b)			
{
	if (b==true)
		m_Property.byPKOnOff = m_Property.byPKOnOff | 0x20;
	else
		m_Property.byPKOnOff = m_Property.byPKOnOff & 0xdf;
}
//b6.海盗保护
BOOL	CPlayer::IsPk_Pirate()				
{
	return (BOOL)(m_Property.byPKOnOff & 0x40);
}
void	CPlayer::SetPk_Pirate(bool b)
{
	if (b==true)
		m_Property.byPKOnOff = m_Property.byPKOnOff | 0x40;
	else
		m_Property.byPKOnOff = m_Property.byPKOnOff & 0xbf;
}
//当退出游戏时触发,切换服务器的时候不会触发
void CPlayer::OnExitGame(BOOL bOnChangeServer)
{
#ifdef _RUNSTACKINFO3_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CPlayer::OnExitGame() Start");
#endif
	if(bOnChangeServer == FALSE)
	{
        //通知chat server该玩家下线
        CMessage csmsg(MSG_S2CS_PLAYER_EXIT_GAME);
        csmsg.Add(GetExID());
        csmsg.SendToChatServer();

		// 下线回城点配置
		CServerRegion* pRegion = static_cast<CServerRegion*>(GetFather());
		if(pRegion)
		{
			long lRetRgnTemplateID = 0;
			long l = 0;
			long t = 0;
			long r = 0;
			long b = 0;
			long d = 0;

			if(pRegion->GetRgnType() == RGN_NORMAL)
			{	
				BYTE byCountry = GetCountry();

				if(GetState()==STATE_DIED  && (long)GetPkValue()<GlobalSetup::GetSetup()->lPrisonIslandPkValue
										   && GetPkValue()>GlobalSetup::GetSetup()->dwPkCountPerAttack)
				{
					//死亡下线，pk值大干20小于18000,被放到红名村
					lRetRgnTemplateID = GetCountryParam()->GetCountryJailVillageRegionID(byCountry);
					l = GetCountryParam()->GetCountryJailVillageRectLeft(byCountry);
					t = GetCountryParam()->GetCountryJailVillageRectTop(byCountry);
					r = GetCountryParam()->GetCountryJailVillageRectRight(byCountry);
					b = GetCountryParam()->GetCountryJailVillageRectBottom(byCountry);
					d = GetCountryParam()->GetCountryJailVillageDir(byCountry);	
				}
				else if(GetState()==STATE_DIED  && (long)GetPkValue()>=GlobalSetup::GetSetup()->lPrisonIslandPkValue)
				{
					//死亡下线，pk值大干lPrisonIslandPkValue,被放到流亡岛
					lRetRgnTemplateID = GetCountryParam()->GetCountryJailIsLandRegionID(byCountry);
					l = GetCountryParam()->GetCountryJailIsLandRectLeft(byCountry);
					t = GetCountryParam()->GetCountryJailIsLandRectTop(byCountry);
					r = GetCountryParam()->GetCountryJailIsLandRectRight(byCountry);
					b = GetCountryParam()->GetCountryJailIsLandRectBottom(byCountry);
					d = GetCountryParam()->GetCountryJailIsLandDir(byCountry);
				}
				else if((pRegion->GetSetup().bLogoutIsUsed && GetState() == STATE_DIED  ) 
					|| (pRegion->GetSetup().bLogoutIsUsed && GetState() != STATE_DIED  &&  pRegion->GetDeadReturn()))
				{					
					pRegion->GetLogoutReturnPoint(this, lRetRgnTemplateID, l, t, r, b, d);
				}
				else
				{
					//保持当前位置
					lRetRgnTemplateID = GetRegionID();
					l = GetTileX();
					t = GetTileY();
					r = GetTileX();
					b = GetTileY();
					d = GetDir();
				}
			}
			else // 是副本场景
			{
				if(pRegion->GetSetup().bDupRgnIsUsed)
					pRegion->GetDupRgnReturnPoint(this, lRetRgnTemplateID, l, t, r, b, d);
			}


			

			

			// 如果是死亡状态
			if(GetState() == STATE_DIED)
			{
				SetState(STATE_PEACE);
				SetHP(GetMaxHP()/2);
				PlayerRunScript(GlobalSetup::GetSetup()->szReliveScriptName);
			}

			if(lRetRgnTemplateID > 0) // 不是返回当前地图
			{
				SetRegionGUID(NULL_GUID);
				SetRegionID(lRetRgnTemplateID);
				CShape::SetPosXY((float)l, (float)t);
			}
			else if(lRetRgnTemplateID == -1) // 回国都
			{
				BYTE byCountry = GetCountry();
				long lCityRegion = GetCountryParam()->GetCountryMainRegionID(byCountry);
				l = GetCountryParam()->GetCountryMainRectLeft(byCountry);
				t = GetCountryParam()->GetCountryMainRectTop(byCountry);
				r = GetCountryParam()->GetCountryMainRectRight(byCountry);
				b = GetCountryParam()->GetCountryMainRectBottom(byCountry);
	

				SetRegionGUID(NULL_GUID);
				SetRegionID(lCityRegion);
				CShape::SetPosXY((float)l, (float)t);
			}
		}

		//设置下线时间
		time_t curTime;
		time(&curTime);
		SetLastExitGameTime((DWORD)curTime);
		
		StopAllSkills();
		StopPassiveSkills();
		

		// 修改状态
		if(GetState() != STATE_DIED)
			SetState(STATE_PEACE);

	
		//##发送消息
		CMessage msg( MSG_S2W_LOG_PLAYERQUIT );
		msg.Add( GetExID() );
		msg.Add((long)1); // 正常退出
		for (LONG i = 0; i < SERVER_KEY_DWORD_SIZE; ++i)
		{
			msg.Add(GetServerKey(i));
		}
		DBWriteSet setWriteDB;
		msg.GetDBWriteSet(setWriteDB);
		CodeToDataBlock(setWriteDB);
		msg.Send();

		// 发送保存PHRgn消息
		if(GetPersonalHouseRgnGUID() != NULL_GUID)
		{
			GetGame()->SendSavePHDupRgn(GetExID());
		}
	}
#ifdef _RUNSTACKINFO3_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CPlayer::OnExitGame() End");
#endif
}


DWORD CPlayer::GetMoney()
{
	return m_pPackage.GetWallet().GetGoldCoinsAmount();
}
DWORD	CPlayer::GetGem(void)
{
	return m_pPackage.GetGem().GetGoldCoinsAmount();
}

//!		得到可装的最大钱数
DWORD	CPlayer::GetMaxMoney(void)
{
	return m_pPackage.GetWallet().GetMaxCapacity();
}

DWORD   CPlayer::GetSilver()
{
	return m_pPackage.GetSilverWallet().GetGoldCoinsAmount();
}

DWORD   CPlayer::GetMaxSilver()
{
	return m_pPackage.GetSilverWallet().GetMaxCapacity();
}

bool CPlayer::SetMoney( DWORD dwCoins,int nFlag )
{
	long lVal=dwCoins;
	if(lVal<0)
	{
		Log4c::Warn(ROOT_MODULE,FormatText("GS_PLAYER_2",lVal));//SetMoney传入了负数%d
		return false;
	}
	bool bResult = false;
	CS2CContainerObjectMove comMsg;
	if( dwCoins > GetMoney() )
	{	
		GetLogicLogInterface()->logT503_Gold_gain_log(this,GetMoney(),dwCoins,nFlag);		
		bResult = m_pPackage.GetWallet().AddGoldCoins( dwCoins - GetMoney(), &comMsg )?true:false;
	}
	else if( dwCoins < GetMoney() )
	{				
		GetLogicLogInterface()->logT503_Gold_gain_log(this,GetMoney(),dwCoins,nFlag);		
		bResult = m_pPackage.GetWallet().DecreaseGoldCoins( GetMoney() - dwCoins, &comMsg )?true:false;
	}
	if( bResult )
	{
		if( comMsg.GetOperation() == CS2CContainerObjectMove::OT_NEW_OBJECT )
		{
			//##新增钱的操作。
			comMsg.SetDestinationContainerExtendID( CS2CContainerObjectMove::PEI_WALLET );
			comMsg.Send( GetExID() );
		}
		else if( comMsg.GetOperation() == CS2CContainerObjectMove::OT_DELETE_OBJECT )
		{
			//##删除钱的操作
			comMsg.SetSourceContainerExtendID( CS2CContainerObjectMove::PEI_WALLET );
			comMsg.Send( GetExID() );
		}
		else 
		{
			//##增加减少
			CGoods* pGoldCoins = m_pPackage.GetWallet().GetGoods( 0 );
			if( pGoldCoins )
			{
				CS2CContainerObjectAmountChange coacMsg;
				coacMsg.SetSourceContainer( GetType(), GetExID(), 0 );
				coacMsg.SetSourceContainerExtendID( CS2CContainerObjectMove::PEI_WALLET );
				coacMsg.SetObject( pGoldCoins -> GetType(), pGoldCoins -> GetExID() );
				coacMsg.SetObjectAmount( pGoldCoins -> GetAmount() );
				coacMsg.Send( GetExID() );
			}
		}
		// 更新钱包物品标志
		GetGame()->UpdateSavePlayer(this, SAVE_DETAIL_ATTRWALLETGOODS);
	}
	return bResult;
}

bool CPlayer::SetSilver( DWORD dwCoins )
{
	long lVal=dwCoins;
	if(lVal<0)
	{
		Log4c::Warn(ROOT_MODULE,FormatText("GS_PLAYER_3",lVal));//SetSilver传入了负数%d
		return false;
	}
	bool bResult = false;
	CS2CContainerObjectMove comMsg;
	if( dwCoins > GetSilver() )
	{
		bResult = m_pPackage.GetSilverWallet().AddGoldCoins( dwCoins - GetSilver(), &comMsg )?true:false;
	}
	else if( dwCoins < GetSilver() )
	{
		bResult = m_pPackage.GetSilverWallet().DecreaseGoldCoins( GetSilver() - dwCoins, &comMsg )?true:false;
	}
	if( bResult )
	{
		if( comMsg.GetOperation() == CS2CContainerObjectMove::OT_NEW_OBJECT )
		{
			//##新增钱的操作。
			comMsg.SetDestinationContainerExtendID( CS2CContainerObjectMove::PEI_SILVERWALLET );
			comMsg.Send( GetExID() );
		}
		else if( comMsg.GetOperation() == CS2CContainerObjectMove::OT_DELETE_OBJECT )
		{
			//##删除钱的操作
			comMsg.SetSourceContainerExtendID( CS2CContainerObjectMove::PEI_SILVERWALLET );
			comMsg.Send( GetExID() );
		}
		else 
		{
			//##增加减少
			CGoods* pSilverCoins = m_pPackage.GetSilverWallet().GetGoods( 0 );
			if( pSilverCoins )
			{
				CS2CContainerObjectAmountChange coacMsg;
				coacMsg.SetSourceContainer( GetType(), GetExID(), 0 );
				coacMsg.SetSourceContainerExtendID( CS2CContainerObjectMove::PEI_SILVERWALLET );
				coacMsg.SetObject( pSilverCoins -> GetType(), pSilverCoins -> GetExID() );
				coacMsg.SetObjectAmount( pSilverCoins -> GetAmount() );
				coacMsg.Send( GetExID() );
			}
		}
		// 更新钱包物品标志
		GetGame()->UpdateSavePlayer(this, SAVE_DETAIL_ATTRSILVERGOODS);
	}
	return bResult;
}

bool CPlayer::SetGem( DWORD lNum )
{
	bool bResult = false;
	CS2CContainerObjectMove comMsg;
	if( lNum > GetGem() )
	{
		bResult = m_pPackage.GetGem().AddGoldCoins( lNum - GetGem(), &comMsg )?true:false;
	}
	else if( lNum < GetGem() )
	{
		bResult = m_pPackage.GetGem().DecreaseGoldCoins( GetGem() - lNum, &comMsg )?true:false;
	}
	if( bResult )
	{
		if( comMsg.GetOperation() == CS2CContainerObjectMove::OT_NEW_OBJECT )
		{
			//##新增钱的操作。
			comMsg.SetDestinationContainerExtendID( CS2CContainerObjectMove::PEI_GEM );
			comMsg.Send( GetExID() );
		}
		else if( comMsg.GetOperation() == CS2CContainerObjectMove::OT_DELETE_OBJECT )
		{
			//##删除钱的操作
			comMsg.SetSourceContainerExtendID( CS2CContainerObjectMove::PEI_GEM );
			comMsg.Send( GetExID() );
		}
		else 
		{
			//##增加减少
			CGoods* pGem = m_pPackage.GetGem().GetGoods( 0 );
			if( pGem )
			{
				CS2CContainerObjectAmountChange coacMsg;
				coacMsg.SetSourceContainer( GetType(), GetExID(), 0 );
				coacMsg.SetSourceContainerExtendID( CS2CContainerObjectMove::PEI_GEM );
				coacMsg.SetObject( pGem -> GetType(), pGem -> GetExID() );
				coacMsg.SetObjectAmount( pGem -> GetAmount() );
				coacMsg.Send( GetExID() );
			}
		}
	}
	return bResult;
}

void CPlayer::SetYuanbao(DWORD dwVal)
{
	m_dwYuanbao=dwVal;
	CMessage msg(MSG_S2C_SET_YUANBAO);
	msg.Add(m_dwYuanbao);
	msg.SendToPlayer(GetExID());
}

CPacketListener::CPacketListener()
{
	m_pDBWriteData = NULL;
}

CPacketListener::~CPacketListener()
{
	m_pDBWriteData = NULL;
}

bool CPacketListener::OnTraversingContainer( CContainer* pContainer, CBaseObject* pObject )
{
	bool bResult	= false;
	CGoods* pGoods	= dynamic_cast<CGoods*>( pObject );

	CVolumeLimitGoodsContainer* pVolumeLimitGoodsContainer	= 
		dynamic_cast<CVolumeLimitGoodsContainer*>( pContainer );

	if( pVolumeLimitGoodsContainer && pGoods )
	{
		CGoodsBaseProperties* pProperties = GoodsSetup::QueryGoodsBaseProperties(
			pGoods -> GetBasePropertiesIndex() );
		if( pProperties )
		{
			DWORD dwPosition = 0;
			if( pVolumeLimitGoodsContainer -> QueryGoodsPosition(pGoods, dwPosition) )
			{

				if( pProperties -> GetGoodsBaseType() == GBT_EQUIPMENT ||
                    pProperties->GetGoodsBaseType() == GBT_TRUMP /*||
                    pProperties->GetGoodsType() == GT_EQUIP_LAND ||
                    pProperties->GetGoodsType() == GT_EQUIP_HUMAN*/)
				{
					m_pDBWriteData->AddToByteArray( static_cast<BYTE>(true) );
				}
				else
				{
					m_pDBWriteData->AddToByteArray( static_cast<BYTE>(false) );
				}
				m_pDBWriteData->AddToByteArray( static_cast<WORD>( pGoods -> GetAmount() ) );
				m_pDBWriteData->AddToByteArray( static_cast<BYTE>(dwPosition) );
				pGoods -> SerializeForOldClient( *m_pDBWriteData );
				bResult = true;
			}
		}
	}

	return bResult;
}

//CHAR* CPlayer::GetDepotPassword()
//{
//	return const_cast<CHAR*>( m_strDepotPassword.c_str() );
//}
//
//void CPlayer::SetDepotPassword( CHAR* strPassword )
//{
//	if( strPassword )
//	{
//		m_strDepotPassword = strPassword;
//	}
//}


// 攻击间隔
WORD CPlayer::GetAtcInterval()
{
	/*
	switch(GetOccupation())
	{
	case PlayerList::OC_FIGHTER:
	return 700;

	case PlayerList::OC_HUNTER:
	break;

	case PlayerList::OC_TAOIST:
	break;
	}*/

	short sAI = 1500 - GetAtcSpeed() * 100;
	if( sAI<500 )
		sAI = 500;
	else if (sAI>2500)
		sAI = 2500;

	return sAI;
}

//##获取挥拳的时间
DWORD CPlayer::GetStrikeOutTime()
{/*
 switch(GetOccupation()) 
 {
 case PlayerList::OC_FIGHTER:
 return 500;

 case PlayerList::OC_HUNTER:
 break;

 case PlayerList::OC_TAOIST:
 break;
 }*/

	return 600;
}

bool CPlayer::IsVisible(CMoveShape* pTarget)
{
	if(GetIsGMConcealment())
		return false;
	return CMoveShape::IsVisible(pTarget);
}

bool CPlayer::IsAttackAble(CMoveShape* pAttacker)
{
	bool bResult = false;
	//if( m_lCityWarDiedState == true )
	//	return bResult;
	//如果是GM,并且出于隐身和无敌状态则不能被攻击
	if( GetIsGMInvincible() || GetIsGMConcealment() || 
        PHASEMAN.PhasePlayerFilter(this,pAttacker) || 
        IsCanBeenAttack()==0 )
		return bResult;

	//察看是否是城战保护模式
	CServerRegion* pRegion = dynamic_cast<CServerRegion*>( GetFather() );

	if( pRegion )
	{
		switch( pAttacker -> GetType() )
		{
		case TYPE_PLAYER:
			{
				CPlayer* pPlayer = dynamic_cast<CPlayer*>( pAttacker );
				if( pPlayer )
				{
					// 如果在nopk场景，玩家不能PK的
					if(0 == pRegion->IsNoPk())
						return false;

					// 如果攻击者或者受害者在安全区内，则攻击无效。
					if( pRegion->GetSecurity(GetTileX(),GetTileY()) ==  CRegion::SECURTIY_SAFE ||
						pRegion->GetSecurity(pPlayer->GetTileX(),pPlayer->GetTileY()) == CRegion::SECURTIY_SAFE )
						return false;

					bResult = true;
					//##对自己不能杀死
					if( GetExID() == pPlayer -> GetExID() )
					{
						bResult = false;
					}

					//if( pPlayer->IsEnemyFactionMember(GetFactionID()) > 0|| pPlayer->IsCityWarEneymyFactionMemeber(GetFactionID()) > 0 )
					//{
					//	break;
					//}
					//##如果攻击者是玩家，那么玩家应该遵循PK开关限制。
					if((pPlayer->IsPk_Normal())
						&& IsBadman()==false)
					{
						if( GetCountry() == pPlayer->GetCountry())
						{
							bResult = false;
						}
					}

					if( (pPlayer->IsPk_Team()) && 
						NULL_GUID != pPlayer -> GetTeamID() &&
						pPlayer -> GetTeamID() == GetTeamID() )
					{
						bResult = false;
					}
					if( (pPlayer-> IsPk_Union()) &&
						(( NULL_GUID != pPlayer -> GetFactionID() && 
						pPlayer -> GetFactionID() == GetFactionID()) || 
						(NULL_GUID != pPlayer -> GetUnionID() &&
						pPlayer -> GetUnionID() == GetUnionID() )) )
					{
						bResult = false;
					}
					if( (pPlayer->IsPk_Badman()) &&	IsBadman() )
					{
						bResult = false;
					}

					if((pPlayer->IsPk_Country()) && 
						IsBadman() == false )
					{
						if( GetCountry() != pPlayer->GetCountry() )
						{
							bResult = false;
						}
					}
				}
			}
			break;

		case TYPE_PET:
			{
				CPet* pPet = dynamic_cast<CPet*>( pAttacker );
				if( pPet != NULL )
				{
					// 如果在nopk场景，玩家不能PK的
					if(0 == pRegion->IsNoPk())
						return false;

					CPlayer* pPlayer = (CPlayer*)pPet->GetHost();
					if( NULL == pPlayer || this == pPlayer )
					{
						return false;
					}

					// 如果攻击者(主人)或者受害者在安全区内，则攻击无效。
					if( pRegion->GetSecurity(GetTileX(),GetTileY()) ==  CRegion::SECURTIY_SAFE ||
						pRegion->GetSecurity(pPet->GetTileX(),pPet->GetTileY()) == CRegion::SECURTIY_SAFE ||
						pRegion->GetSecurity(pPlayer->GetTileX(),pPlayer->GetTileY()) == CRegion::SECURTIY_SAFE )
						return false;

					bResult = true;
					//##对自己不能杀死
					if( GetExID() == pPet->GetExID() )
					{
						bResult = false;
					}

					//##如果攻击者是玩家，那么玩家应该遵循PK开关限制。
					if((pPlayer->IsPk_Normal())
						&& IsBadman()==false)
					{
						if( GetCountry() == pPlayer->GetCountry())
						{
							bResult = false;
						}
					}

					if( (pPlayer->IsPk_Team()) && 
						NULL_GUID != pPlayer -> GetTeamID() &&
						pPlayer -> GetTeamID() == GetTeamID() )
					{
						bResult = false;
					}
					if( (pPlayer-> IsPk_Union()) &&
						(( NULL_GUID != pPlayer -> GetFactionID() && 
						pPlayer -> GetFactionID() == GetFactionID()) || 
						(NULL_GUID != pPlayer -> GetUnionID() &&
						pPlayer -> GetUnionID() == GetUnionID() )) )
					{
						bResult = false;
					}
					if( (pPlayer->IsPk_Badman()) &&	IsBadman() )
					{
						bResult = false;
					}

					if((pPlayer->IsPk_Country()) && 
						IsBadman() == false )
					{
						if( GetCountry() != pPlayer->GetCountry() )
						{
							bResult = false;
						}
					}
				}
			}
			break;

		case TYPE_MONSTER:
			//##如果攻击者是怪物，那么普通的怪物应该可以攻击。而守卫则需要判定是否是坏人
			{
				CMonster* pMonster = dynamic_cast<CMonster*>( pAttacker );
				if( pMonster )
				{
					bResult = true;
					////##如果是宠物
					//if( pMonster -> DoesCreatureBeenTamed() && pMonster -> m_pPetAI )
					//{
					//	CMoveShape* pMaster = pMonster -> m_pPetAI -> GetPetMaster();
					//	if( pMaster )
					//	{
					//		if( pMaster == this || IsAttackAble(pMaster) == FALSE )
					//		{
					//			bResult = false;
					//		}
					//	}
					//	//##如果找不到主人，则有可能主人已经下线，这个时候，宠物不再攻击任何玩家！
					//	//else 
					//	//{
					//	//	bResult = false;
					//	//}
					//}
					//
					//else

					//##如果是守卫
					if( pMonster -> GetNpcType()  == NT_Guard )
					{

						// commented by kevin lynx . 不需要守卫规则判定。
						//if( pAttacker -> GetAI() &&( pAttacker -> GetAI()->GetAIType() == CAIFactory::CITY_GUARD_WITH_SWORD ||pAttacker -> GetAI()->GetAIType() == CAIFactory::CITY_GUARD_WITH_BOW) )
						//{
						//	//##是守城卫
						//	if( NULL_GUID != GetFactionID() && GetFactionID() == pRegion->GetOwnedCityFaction() )
						//	{
						//		bResult = false;
						//	}
						//	else
						//	{
						//		if( NULL_GUID != GetUnionID() &&  GetUnionID() == pRegion->GetOwnedCityUnion() )
						//		{
						//			bResult = false;
						//		}
						//	}
						//}
						//else
						//{
						//	//##是一般的守卫
						//	if( IsBadman() == false )
						//	{
						//		bResult = false;
						//	}
						//}
					}
				}
			}

			break;
		}
	}
	return bResult;
}


long CPlayer::OnBeenAttacked( CMoveShape* pAttacker,long lHp,long lDamageResult)
{
	//如果是GM,并且出于隐身和无敌状态则不能被攻击
	if(GetIsGMInvincible() || GetIsGMConcealment() )
		return 0;
	return CMoveShape::OnBeenAttacked(pAttacker,lHp,lDamageResult);
}

void CPlayer::OnBeenKilled(CMoveShape* pAttacker)
{
	CMoveShape::OnBeenKilled(pAttacker);
	//玩家杀死玩家的时候，触发脚本
	if(pAttacker && pAttacker->GetType() == TYPE_PLAYER)
	{
		//PVP杀人时触发脚本
		const char* strScript = GlobalSetup::GetSetup()->pszPVPKilled;
		if(strScript)
		{
			//stRunScript sScript;
			//sScript.pszFileName = (char*)strScript;
			//sScript.desShape = this;
			//sScript.pRegion =  dynamic_cast<CRegion*>(GetFather());
			//sScript.srcShape	= pAttacker;
			//RunScript( &sScript, const_cast<CHAR*>( GetGame() -> GetScriptFileData(strScript) ) );
			Script::RunEnv env( pAttacker, this, static_cast<CRegion*>( GetFather() ) );
			GetInst( ScriptSys ).RunScript( env, strScript );
		}

		//死亡时触发脚本
		strScript = GlobalSetup::GetSetup()->pszPVPBeKilled;
		if(strScript)
		{
			//stRunScript sScript;
			//sScript.pszFileName = (char*)strScript;
			//sScript.desShape = pAttacker;
			//sScript.pRegion =  dynamic_cast<CRegion*>(GetFather());
			//sScript.srcShape	= this;
			//RunScript( &sScript, const_cast<CHAR*>( GetGame() -> GetScriptFileData(strScript) ) );
			Script::RunEnv env( this, pAttacker, static_cast<CRegion*>( GetFather() ) );
			GetInst( ScriptSys ).RunScript( env, strScript );
		}
	}
}


DWORD  CPlayer::DeleteGoods(CS2CContainerObjectMove::PLAYER_EXTEND_ID pei,eGoodsBaseType baseType,DWORD dwWeaponType,DWORD dwNum)
{
	DWORD dwDelNum=0;
	CVolumeLimitGoodsContainer* pContainer = NULL;
	switch( pei )
	{
	case CS2CContainerObjectMove::PEI_PACKET:
		pContainer = getOriginPack();
		break;
		/*
		case CS2CContainerObjectMove::PEI_EQUIPMENT:
		pContainer = &m_cEquipment;
		break;*/	
	case CS2CContainerObjectMove::PEI_PACK1:
		pContainer=GetPackage()->GetSubpack(0)->pSubpackContainer;
		break;
	case CS2CContainerObjectMove::PEI_PACK2:
		pContainer=GetPackage()->GetSubpack(1)->pSubpackContainer;
		break;
	case CS2CContainerObjectMove::PEI_PACK3:
		pContainer=GetPackage()->GetSubpack(2)->pSubpackContainer;
		break;
	case CS2CContainerObjectMove::PEI_PACK4:
		pContainer=GetPackage()->GetSubpack(3)->pSubpackContainer;
		break;
	case CS2CContainerObjectMove::PEI_PACK5:
		pContainer=GetPackage()->GetSubpack(4)->pSubpackContainer;
		break;
	}
	if( pContainer )
	{
		dwDelNum=pContainer->DelGoods(baseType,dwWeaponType,dwNum,pei,GetExID());
	}
	return dwDelNum;
}


DWORD CPlayer::DeleteGoods( CS2CContainerObjectMove::PLAYER_EXTEND_ID pei, const CGUID& guid, DWORD dwAmount )
{
	DWORD dwResult = 0;
	CGoodsContainer* pContainer = NULL;
	switch( pei )
	{
	case CS2CContainerObjectMove::PEI_PACKET:
		pContainer = getOriginPack();
		break;
	case CS2CContainerObjectMove::PEI_EQUIPMENT:
		pContainer = &m_cEquipment;
		break;
	case CS2CContainerObjectMove::PEI_GEM:
		pContainer = &m_pPackage.GetGem();
		break;
	case CS2CContainerObjectMove::PEI_WALLET:
		pContainer = &m_pPackage.GetWallet();
		break;
    case CS2CContainerObjectMove::PEI_EQUIP_GOD:
        pContainer = &m_cEquipmentGod;
        break;
    case CS2CContainerObjectMove::PEI_EQUIP_LAND:
        pContainer = &m_cEquipmentLand;
        break;
    case CS2CContainerObjectMove::PEI_EQUIP_HUMAN:
        pContainer = &m_cEquipmentHuman;
        break;
	case CS2CContainerObjectMove::PEI_PACK1:
		pContainer=GetPackage()->GetSubpack(0)->pSubpackContainer;
		break;
	case CS2CContainerObjectMove::PEI_PACK2:
		pContainer=GetPackage()->GetSubpack(1)->pSubpackContainer;
		break;
	case CS2CContainerObjectMove::PEI_PACK3:
		pContainer=GetPackage()->GetSubpack(2)->pSubpackContainer;
		break;
	case CS2CContainerObjectMove::PEI_PACK4:
		pContainer=GetPackage()->GetSubpack(3)->pSubpackContainer;
		break;
	case CS2CContainerObjectMove::PEI_PACK5:
		pContainer=GetPackage()->GetSubpack(4)->pSubpackContainer;
		break;		
	}
	if( pContainer )
	{
		CGoods* pGoods = dynamic_cast<CGoods*>( pContainer -> Find(TYPE_GOODS, guid) );
		if( pGoods )
		{
			DWORD dwPosition = 0;
			if( pContainer -> QueryGoodsPosition(pGoods, dwPosition) )
			{
				if( pGoods -> GetAmount() > dwAmount )
				{
					//##改变数量
					dwResult = dwAmount;
					pGoods -> SetAmount( pGoods -> GetAmount() - dwAmount );

					CS2CContainerObjectAmountChange coacMsg;
					coacMsg.SetSourceContainer( GetType(), GetExID(), dwPosition );
					coacMsg.SetSourceContainerExtendID( pei );
					coacMsg.SetObject( pGoods -> GetType(), pGoods -> GetExID() );
					coacMsg.SetObjectAmount( pGoods -> GetAmount() );
					coacMsg.Send( GetExID() );

					OnLoseGoods(pGoods);

					//Goods Consume Log
					GetLogicLogInterface()->logT022_goods_log_consume(this,pGoods->GetBasePropertiesIndex(),pGoods->GetExID(),
						GoodsSetup::QueryGoodsName(pGoods->GetBasePropertiesIndex()),pGoods->GetAmount(),dwAmount,pei,dwPosition);
				}
				else
				{
					//##删除
					CS2CContainerObjectMove comMsg;
					if( pContainer -> Remove(pGoods, &comMsg) )
					{
						dwResult = pGoods -> GetAmount();

						comMsg.SetOperation( CS2CContainerObjectMove::OT_DELETE_OBJECT );
						comMsg.SetSourceContainerExtendID( pei );						
						comMsg.Send( GetExID() );

						OnLoseGoods(pGoods);

						//Goods Consume Log
						GetLogicLogInterface()->logT022_goods_log_consume(this,pGoods->GetBasePropertiesIndex(),pGoods->GetExID(),
							GoodsSetup::QueryGoodsName(pGoods->GetBasePropertiesIndex()),pGoods->GetAmount(),dwAmount,pei,dwPosition);
#ifdef _GOODSLOG1_
						char pszGoodsLog[1024]="";			
						char srcGoodsGUID[50]="";
						pGoods->GetExID().tostring(srcGoodsGUID);					
						_snprintf(pszGoodsLog,1024,"[物品回收:CPlayer::DeleteGoods][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d]",
							pGoods->GetBasePropertiesIndex(),GoodsSetup::QueryGoodsName(pGoods->GetBasePropertiesIndex()),srcGoodsGUID,pGoods->GetAmount());
						CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
#endif

						//##删除该物品?
						//GoodsSetup::GarbageCollect( &pGoods ,31);
						CGoodsFactory::AddGoodsToGCSet(pGoods);

					}
				}
			}
		}
	}
	return dwResult;
}

DWORD CPlayer::DeleteDepotGoods( DWORD dwContainerType, const CGUID& guid, DWORD dwAmount )
{
	DWORD dwResult = 0;
	CGoodsContainer* pContainer = m_pPlayerDepot.FindContainer(dwContainerType, true);

	if( pContainer )
	{
		CGoods* pGoods = dynamic_cast<CGoods*>( pContainer -> Find(TYPE_GOODS, guid) );
		if( pGoods )
		{
			DWORD dwPosition = 0;
			if( pContainer -> QueryGoodsPosition(pGoods, dwPosition) )
			{
				if( pGoods -> GetAmount() > dwAmount )
				{
					//##改变数量
					dwResult = dwAmount;
					pGoods -> SetAmount( pGoods -> GetAmount() - dwAmount );

					CS2CContainerObjectAmountChange coacMsg;
					coacMsg.SetSourceContainer( GetType(), GetExID(), dwPosition );
					coacMsg.SetSourceContainerExtendID( dwContainerType );
					coacMsg.SetObject( pGoods -> GetType(), pGoods -> GetExID() );
					coacMsg.SetObjectAmount( pGoods -> GetAmount() );
					coacMsg.Send( GetExID() );
				}
				else
				{
					//##删除
					CS2CContainerObjectMove comMsg;
					if( pContainer -> Remove(pGoods, &comMsg) )
					{
						dwResult = pGoods -> GetAmount();

						comMsg.SetOperation( CS2CContainerObjectMove::OT_DELETE_OBJECT );
						comMsg.SetSourceContainerExtendID( dwContainerType );						
						comMsg.Send( GetExID() );
#ifdef _GOODSLOG1_
						char pszGoodsLog[1024]="";			
						char srcGoodsGUID[50]="";
						pGoods->GetExID().tostring(srcGoodsGUID);					
						_snprintf(pszGoodsLog,1024,"[物品回收:CPlayer::DeleteGoods][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d]",
							pGoods->GetBasePropertiesIndex(),GoodsSetup::QueryGoodsName(pGoods->GetBasePropertiesIndex()),srcGoodsGUID,pGoods->GetAmount());
						CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
#endif

						//##删除该物品?
						//GoodsSetup::GarbageCollect( &pGoods ,31);
						CGoodsFactory::AddGoodsToGCSet(pGoods);

					}
				}
			}
		}
	}
	return dwResult;
}

void CPlayer::DropParticularGoodsWhenDead()
{
	if( IsDied() )
	{
		CGoodsListListener lGoodsList;
		getOriginPack()->TraversingContainer( &lGoodsList );
		for( size_t i = 0; i < lGoodsList.m_vGoodsID.size(); i ++ )
		{
			CGoods* pGoods = dynamic_cast<CGoods*>(getOriginPack()->Find(TYPE_GOODS, lGoodsList.m_vGoodsID[i]) );
			if( pGoods && ( pGoods -> GetAddonPropertyValues(GAP_PARTICULAR_ATTRIBUTE, 1) &
				PA_DROP_DOWN_WHEN_DEAD ) == PA_DROP_DOWN_WHEN_DEAD )
			{
				DropGoods( CS2CContainerObjectMove::PEI_PACKET, lGoodsList.m_vGoodsID[i] );
			}
		}

		lGoodsList.m_vGoodsID.clear();
		m_cEquipment.TraversingContainer( &lGoodsList );
		for( size_t i = 0; i < lGoodsList.m_vGoodsID.size(); i ++ )
		{
			CGoods* pGoods = dynamic_cast<CGoods*>( m_cEquipment.Find(TYPE_GOODS, lGoodsList.m_vGoodsID[i]) );
			if( pGoods && ( pGoods -> GetAddonPropertyValues(GAP_PARTICULAR_ATTRIBUTE, 1) &
				PA_DROP_DOWN_WHEN_DEAD ) == PA_DROP_DOWN_WHEN_DEAD )
			{
				DropGoods( CS2CContainerObjectMove::PEI_EQUIPMENT, lGoodsList.m_vGoodsID[i] );
			}
		}	

        lGoodsList.m_vGoodsID.clear();
        m_cEquipmentGod.TraversingContainer( &lGoodsList );
        for( size_t i = 0; i < lGoodsList.m_vGoodsID.size(); i ++ )
        {
            CGoods* pGoods = dynamic_cast<CGoods*>( m_cEquipmentGod.Find(lGoodsList.m_vGoodsID[i]) );
            if( pGoods && ( pGoods -> GetAddonPropertyValues(GAP_PARTICULAR_ATTRIBUTE, 1) &
                PA_DROP_DOWN_WHEN_DEAD ) == PA_DROP_DOWN_WHEN_DEAD )
            {
                DropGoods( CS2CContainerObjectMove::PEI_EQUIP_GOD, lGoodsList.m_vGoodsID[i] );
            }
        }	

        lGoodsList.m_vGoodsID.clear();
        m_cEquipmentLand.TraversingContainer( &lGoodsList );
        for( size_t i = 0; i < lGoodsList.m_vGoodsID.size(); i ++ )
        {
            CGoods* pGoods = dynamic_cast<CGoods*>( m_cEquipmentLand.Find(lGoodsList.m_vGoodsID[i]) );
            if( pGoods && ( pGoods -> GetAddonPropertyValues(GAP_PARTICULAR_ATTRIBUTE, 1) &
                PA_DROP_DOWN_WHEN_DEAD ) == PA_DROP_DOWN_WHEN_DEAD )
            {
                DropGoods( CS2CContainerObjectMove::PEI_EQUIP_LAND, lGoodsList.m_vGoodsID[i] );
            }
        }	

        lGoodsList.m_vGoodsID.clear();
        m_cEquipmentHuman.TraversingContainer( &lGoodsList );
        for( size_t i = 0; i < lGoodsList.m_vGoodsID.size(); i ++ )
        {
            CGoods* pGoods = dynamic_cast<CGoods*>( m_cEquipmentHuman.Find(lGoodsList.m_vGoodsID[i]) );
            if( pGoods && ( pGoods -> GetAddonPropertyValues(GAP_PARTICULAR_ATTRIBUTE, 1) &
                PA_DROP_DOWN_WHEN_DEAD ) == PA_DROP_DOWN_WHEN_DEAD )
            {
                DropGoods( CS2CContainerObjectMove::PEI_EQUIP_HUMAN, lGoodsList.m_vGoodsID[i] );
            }
        }	

		for(int i=CS2CContainerObjectMove::PEI_PACK1;i<=CS2CContainerObjectMove::PEI_PACK5;i++)
		{
			if(GetPackage()->GetSubpack(i-CS2CContainerObjectMove::PEI_PACK1)->pGoods)
			{			
				lGoodsList.m_vGoodsID.clear();
				GetPackage()->GetSubpack(i-CS2CContainerObjectMove::PEI_PACK1)->pSubpackContainer->TraversingContainer(&lGoodsList);
				for(size_t j=0;j<lGoodsList.m_vGoodsID.size();j++)
				{
					CGoods* pGoods = dynamic_cast<CGoods*>(GetPackage()->GetSubpack(i-CS2CContainerObjectMove::PEI_PACK1)->pSubpackContainer->Find(TYPE_GOODS, lGoodsList.m_vGoodsID[j]));
					if( pGoods && ( pGoods -> GetAddonPropertyValues(GAP_PARTICULAR_ATTRIBUTE, 1) &
						PA_DROP_DOWN_WHEN_DEAD ) == PA_DROP_DOWN_WHEN_DEAD )
					{
						DropGoods( static_cast<CS2CContainerObjectMove::PLAYER_EXTEND_ID>(i), lGoodsList.m_vGoodsID[j] );
					}
				}	
			}
		}	

	}
}

void CPlayer::DropParticularGoodsWhenLost()
{
	//if( IsDied() )
	{
		CGoodsListListener lGoodsList;
		getOriginPack()->TraversingContainer( &lGoodsList );
		for( size_t i = 0; i < lGoodsList.m_vGoodsID.size(); i ++ )
		{
			CGoods* pGoods = dynamic_cast<CGoods*>(getOriginPack()->Find(TYPE_GOODS, lGoodsList.m_vGoodsID[i]) );
			if( pGoods && ( pGoods -> GetAddonPropertyValues(GAP_PARTICULAR_ATTRIBUTE, 1) &
				PA_DROP_DOWN_WHEN_LOGOUT ) == PA_DROP_DOWN_WHEN_LOGOUT )
			{
				DropGoods( CS2CContainerObjectMove::PEI_PACKET, lGoodsList.m_vGoodsID[i] );
			}
		}

		lGoodsList.m_vGoodsID.clear();
		m_cEquipment.TraversingContainer( &lGoodsList );
		for( size_t i = 0; i < lGoodsList.m_vGoodsID.size(); i ++ )
		{
			CGoods* pGoods = dynamic_cast<CGoods*>( m_cEquipment.Find(TYPE_GOODS, lGoodsList.m_vGoodsID[i]) );
			if( pGoods && ( pGoods -> GetAddonPropertyValues(GAP_PARTICULAR_ATTRIBUTE, 1) &
				PA_DROP_DOWN_WHEN_LOGOUT ) == PA_DROP_DOWN_WHEN_LOGOUT )
			{
				DropGoods( CS2CContainerObjectMove::PEI_EQUIPMENT, lGoodsList.m_vGoodsID[i] );
			}
		}

        lGoodsList.m_vGoodsID.clear();
        m_cEquipmentGod.TraversingContainer( &lGoodsList );
        for( size_t i = 0; i < lGoodsList.m_vGoodsID.size(); i ++ )
        {
            CGoods* pGoods = dynamic_cast<CGoods*>( m_cEquipmentGod.Find(lGoodsList.m_vGoodsID[i]) );
            if( pGoods && ( pGoods -> GetAddonPropertyValues(GAP_PARTICULAR_ATTRIBUTE, 1) &
                PA_DROP_DOWN_WHEN_LOGOUT ) == PA_DROP_DOWN_WHEN_LOGOUT )
            {
                DropGoods( CS2CContainerObjectMove::PEI_EQUIP_GOD, lGoodsList.m_vGoodsID[i] );
            }
        }	

        lGoodsList.m_vGoodsID.clear();
        m_cEquipmentLand.TraversingContainer( &lGoodsList );
        for( size_t i = 0; i < lGoodsList.m_vGoodsID.size(); i ++ )
        {
            CGoods* pGoods = dynamic_cast<CGoods*>( m_cEquipmentLand.Find(lGoodsList.m_vGoodsID[i]) );
            if( pGoods && ( pGoods -> GetAddonPropertyValues(GAP_PARTICULAR_ATTRIBUTE, 1) &
                PA_DROP_DOWN_WHEN_LOGOUT ) == PA_DROP_DOWN_WHEN_LOGOUT )
            {
                DropGoods( CS2CContainerObjectMove::PEI_EQUIP_LAND, lGoodsList.m_vGoodsID[i] );
            }
        }	

        lGoodsList.m_vGoodsID.clear();
        m_cEquipmentHuman.TraversingContainer( &lGoodsList );
        for( size_t i = 0; i < lGoodsList.m_vGoodsID.size(); i ++ )
        {
            CGoods* pGoods = dynamic_cast<CGoods*>( m_cEquipmentHuman.Find(lGoodsList.m_vGoodsID[i]) );
            if( pGoods && ( pGoods -> GetAddonPropertyValues(GAP_PARTICULAR_ATTRIBUTE, 1) &
                PA_DROP_DOWN_WHEN_LOGOUT ) == PA_DROP_DOWN_WHEN_LOGOUT )
            {
                DropGoods( CS2CContainerObjectMove::PEI_EQUIP_HUMAN, lGoodsList.m_vGoodsID[i] );
            }
        }	

		for(int i=CS2CContainerObjectMove::PEI_PACK1;i<=CS2CContainerObjectMove::PEI_PACK5;i++)
		{
			if(GetPackage()->GetSubpack(i-CS2CContainerObjectMove::PEI_PACK1)->pGoods)
			{			
				lGoodsList.m_vGoodsID.clear();
				GetPackage()->GetSubpack(i-CS2CContainerObjectMove::PEI_PACK1)->pSubpackContainer->TraversingContainer(&lGoodsList);
				for(size_t j=0;j<lGoodsList.m_vGoodsID.size();j++)
				{
					CGoods* pGoods = dynamic_cast<CGoods*>(GetPackage()->GetSubpack(i-CS2CContainerObjectMove::PEI_PACK1)->pSubpackContainer->Find(TYPE_GOODS, lGoodsList.m_vGoodsID[j]));
					if( pGoods && ( pGoods -> GetAddonPropertyValues(GAP_PARTICULAR_ATTRIBUTE, 1) &
						PA_DROP_DOWN_WHEN_LOGOUT ) == PA_DROP_DOWN_WHEN_LOGOUT )
					{
						DropGoods( static_cast<CS2CContainerObjectMove::PLAYER_EXTEND_ID>(i), lGoodsList.m_vGoodsID[j] );
					}
				}	
			}
		}	

	}
}

void CPlayer::DropParticularGoodsWhenRecall()
{
	CGoodsListListener lGoodsList;
	getOriginPack()->TraversingContainer( &lGoodsList );
	for( size_t i = 0; i < lGoodsList.m_vGoodsID.size(); i ++ )
	{
		CGoods* pGoods = dynamic_cast<CGoods*>(getOriginPack()->Find(TYPE_GOODS, lGoodsList.m_vGoodsID[i]) );
		if( pGoods && ( pGoods -> GetAddonPropertyValues(GAP_PARTICULAR_ATTRIBUTE, 1) &
			PA_DROP_DOWN_WHEN_RECALL ) == PA_DROP_DOWN_WHEN_RECALL )
		{
			DropGoods( CS2CContainerObjectMove::PEI_PACKET, lGoodsList.m_vGoodsID[i] );
		}
	}

	lGoodsList.m_vGoodsID.clear();
	m_cEquipment.TraversingContainer( &lGoodsList );
	for( size_t i = 0; i < lGoodsList.m_vGoodsID.size(); i ++ )
	{
		CGoods* pGoods = dynamic_cast<CGoods*>( m_cEquipment.Find(TYPE_GOODS, lGoodsList.m_vGoodsID[i]) );
		if( pGoods && ( pGoods -> GetAddonPropertyValues(GAP_PARTICULAR_ATTRIBUTE, 1) &
			PA_DROP_DOWN_WHEN_RECALL ) == PA_DROP_DOWN_WHEN_RECALL )
		{
			DropGoods( CS2CContainerObjectMove::PEI_EQUIPMENT, lGoodsList.m_vGoodsID[i] );
		}
	}

    lGoodsList.m_vGoodsID.clear();
    m_cEquipmentGod.TraversingContainer( &lGoodsList );
    for( size_t i = 0; i < lGoodsList.m_vGoodsID.size(); i ++ )
    {
        CGoods* pGoods = dynamic_cast<CGoods*>( m_cEquipmentGod.Find(lGoodsList.m_vGoodsID[i]) );
        if( pGoods && ( pGoods -> GetAddonPropertyValues(GAP_PARTICULAR_ATTRIBUTE, 1) &
            PA_DROP_DOWN_WHEN_RECALL ) == PA_DROP_DOWN_WHEN_RECALL )
        {
            DropGoods( CS2CContainerObjectMove::PEI_EQUIP_GOD, lGoodsList.m_vGoodsID[i] );
        }
    }

    lGoodsList.m_vGoodsID.clear();
    m_cEquipmentLand.TraversingContainer( &lGoodsList );
    for( size_t i = 0; i < lGoodsList.m_vGoodsID.size(); i ++ )
    {
        CGoods* pGoods = dynamic_cast<CGoods*>( m_cEquipmentLand.Find(lGoodsList.m_vGoodsID[i]) );
        if( pGoods && ( pGoods -> GetAddonPropertyValues(GAP_PARTICULAR_ATTRIBUTE, 1) &
           PA_DROP_DOWN_WHEN_RECALL ) == PA_DROP_DOWN_WHEN_RECALL )
        {
            DropGoods( CS2CContainerObjectMove::PEI_EQUIP_LAND, lGoodsList.m_vGoodsID[i] );
        }
    }

    lGoodsList.m_vGoodsID.clear();
    m_cEquipmentHuman.TraversingContainer( &lGoodsList );
    for( size_t i = 0; i < lGoodsList.m_vGoodsID.size(); i ++ )
    {
        CGoods* pGoods = dynamic_cast<CGoods*>( m_cEquipmentHuman.Find(lGoodsList.m_vGoodsID[i]) );
        if( pGoods && ( pGoods -> GetAddonPropertyValues(GAP_PARTICULAR_ATTRIBUTE, 1) &
            PA_DROP_DOWN_WHEN_RECALL ) == PA_DROP_DOWN_WHEN_RECALL )
        {
            DropGoods( CS2CContainerObjectMove::PEI_EQUIP_HUMAN, lGoodsList.m_vGoodsID[i] );
        }
    }

	for(int i=CS2CContainerObjectMove::PEI_PACK1;i<=CS2CContainerObjectMove::PEI_PACK5;i++)
	{
		if(GetPackage()->GetSubpack(i-CS2CContainerObjectMove::PEI_PACK1)->pGoods)
		{			
			lGoodsList.m_vGoodsID.clear();
			GetPackage()->GetSubpack(i-CS2CContainerObjectMove::PEI_PACK1)->pSubpackContainer->TraversingContainer(&lGoodsList);
			for(size_t j=0;j<lGoodsList.m_vGoodsID.size();j++)
			{
				CVolumeLimitGoodsContainer* pSubContainer=GetPackage()->GetSubpack(i-CS2CContainerObjectMove::PEI_PACK1)->pSubpackContainer;
				CGoods* pGoods = dynamic_cast<CGoods*>(pSubContainer->Find(TYPE_GOODS, lGoodsList.m_vGoodsID[j]));
				if( pGoods && ( pGoods -> GetAddonPropertyValues(GAP_PARTICULAR_ATTRIBUTE, 1) &
					PA_DROP_DOWN_WHEN_RECALL ) == PA_DROP_DOWN_WHEN_RECALL )
				{
					DropGoods( static_cast<CS2CContainerObjectMove::PLAYER_EXTEND_ID>(i), lGoodsList.m_vGoodsID[j] );
				}
			}	
		}
	}	

}

void CPlayer::DeleteGoodsInPacket(char* strGoodName)
{
	if(strGoodName==NULL)
		return;

	CSeekGoodsListener lSeeker;
	lSeeker.SetTarget( strGoodName );
	getOriginPack()->TraversingContainer( &lSeeker );
	if(lSeeker.m_vGoodsID.size()<=0)
		return;

	LONG lRemainedGoods = 1;
	DeleteGoods( CS2CContainerObjectMove::PEI_PACKET, lSeeker.m_vGoodsID[0], lRemainedGoods );
}

//经验值
void CPlayer::SetExp(LONG64 l)
{
	DWORD lExpMultiple = (DWORD)( l / dwExpMutipleValue );
	DWORD lExp = (DWORD)(  l % dwExpMutipleValue );
	m_Property.dwExp = lExp;
	m_Property.dwExpMultiple = lExpMultiple;
}

//!，给玩家增加经验
void CPlayer::IncExp(LONG64 llExp, DWORD dwAddType)
{
	LONG64 llResultExp = llExp+GetTotalExp();
	if(0 > llResultExp)
	{
		Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,FormatText("GS_PLAYER_11", GetName(), GetExp(), llExp, dwAddType));//玩家【%s】得到一个负的经验【当前经验 = %ld，增加经验 = %ld，增加类型 = %d】，执行返回！
		return;
	}

	LONG64 llMaxExp = PlayerList::GetMaxExp();
	// 当前经验已经到达最大值
	if(GetTotalExp() >= llMaxExp)
	{
		return;
	}

	if(llResultExp > llMaxExp)
		llResultExp = llMaxExp;

	SetExp(llResultExp);

	//if( !m_FightPets.empty() ) 
	//{
	//	AddPetsExp((long)llExp, (long)dwAddType);
	//	GetGame()->UpdateSavePlayer(this, SAVE_DETAIL_ATTRPROPERTY);
	//}

	CMessage msg( MSG_S2C_PLAYER_ADDEXP );
	msg.Add(dwAddType);
	msg.Add( GetExp() );
	msg.Add( GetExpMultiple() );
	//得到当前赠送经验
	msg.Add(GetPresentExp());
	//		msg.Add( GetVigour() );
	msg.SendToPlayer( GetExID());
}

//!		添加经验日志
void CPlayer::AddIncExpLog(DWORD dwOldExp, DWORD dwOldExpMultiple, DWORD dwAddExp, DWORD dwAddExpMultiple, DWORD dwAddType, const char *pSrcName)
{
	//粗略计算经验
	LONG64 llGetExp = GetLevel()*40;
	LONG64 llAddExp = (LONG64)dwExpMutipleValue*dwAddExpMultiple+dwAddExp;
	if(llAddExp > llGetExp)
		GetLogicLogInterface()->logT080_player_getexp_log(this, dwAddType, dwAddExp, dwAddExpMultiple, dwOldExp, dwOldExpMultiple, GetExp(), GetExpMultiple(), pSrcName);
}

//!		添加职业经验日志
void CPlayer::AddIncOccuExpLog(BYTE byOccu,DWORD dwOldExp, DWORD dwAddExp, DWORD dwAddType, const char *pSrcName)
{
	if(dwAddExp > 100)
		GetLogicLogInterface()->logT081_player_getoccuexp_log(this, dwAddType, byOccu, dwAddExp, dwOldExp, GetOccuExp((eOccupation)byOccu), pSrcName);
}

//扣除SP
void CPlayer::DecOccuSp(eOccupation eOccu,long lSp)
{
	long lCurrentSp = GetOccuSp(eOccu) - lSp;
	if( 0 > lCurrentSp)
		lCurrentSp = 0;
	SetOccuSp(eOccu,lCurrentSp);	
	//##同步当前技能点数值
	CMessage msg( MSG_S2C_PLAYER_ADDSP );
	msg.Add((BYTE)eOccu);
	msg.Add( GetOccuSp(eOccu) );
	msg.SendToPlayer( GetExID());
}

//给玩家增加技能点数
void CPlayer::IncOccuSp(eOccupation eOccu,long lSp)
{
	long lResultSp = GetOccuSp(eOccu)+lSp;
	if(0 > lResultSp)
	{
		Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,FormatText("GS_PLAYER_12", lSp));//得到一个负的技能点[%d]"/>
		return;
	}

	SetOccuSp(eOccu,lResultSp);
	//##同步当前技能点数值
	CMessage msg( MSG_S2C_PLAYER_ADDSP );
	msg.Add((BYTE)eOccu);
	msg.Add( GetOccuSp(eOccu) );
	msg.SendToPlayer( GetExID(),false);
}

//增加职业的职业经验
void CPlayer::IncOccuExp(eOccupation byOccu,long lExp, DWORD dwAddType)
{
	long lResultExp = GetOccuExp(byOccu)+lExp;
	if(0 > lResultExp)
	{
		Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,FormatText("GS_PLAYER_13",byOccu,lExp,dwAddType));//得到一个负的职业经验[Occu:%d,Exp:%d,addType:%d]
		return;
	}

	SetOccuExp(byOccu,lResultExp);
	GetGame()->UpdateSavePlayer(this, SAVE_DETAIL_ATTRPROPERTY);

	CMessage msg( MSG_S2C_PLAYER_ADDOCCUEXP );
	msg.Add(dwAddType);
	msg.Add(BYTE(byOccu));
	msg.Add( GetOccuExp(byOccu) );
	msg.SendToPlayer( GetExID());
}

//获取赠送经验
DWORD CPlayer::AcquirePresentExp(DWORD dwMaxAcqExp)
{
	DWORD dwOldPresentExp = GetPresentExp();
	DWORD dwResult = 0;
	if(GetPresentExp() >= dwMaxAcqExp)
		dwResult = dwMaxAcqExp;
	else
		dwResult = GetPresentExp();	
	SetPresentExp(GetPresentExp()-dwResult);
	if(dwOldPresentExp > 0 && GetPresentExp()== 0)
	{
		//停止显示赠送经验的buff
		DelNewState(GlobalSetup::GetSetup()->lPresentExpBuffId,GlobalSetup::GetSetup()->lPresentExpBuffLvl);
		//更新赠送经验值
		//赠送经验
		m_mapDirtyAttrData.insert("PresentExp");
		UpdateAttribute();		
	}
	return dwResult;
}

void CPlayer::PerformEmotion( LONG lID )
{
	//##首先清除表情
	m_lEmotionIndex		= 0;
	m_dwEmotonTimeStamp	= 0;

	if( IsDied() ) return;
	if( GetAI() == FALSE ) return;
	if( GetAI() -> HasTarget() ) return;

	//##判定是否需要存储
	if( EmotionSetup::IsEmotionRepeated(lID) )
	{
		m_lEmotionIndex		= lID;
		m_dwEmotonTimeStamp	= timeGetTime();
	}

	CMessage msg( MSG_S2C_SHAPE_EMOTION );
	msg.Add( GetType() );
	msg.Add( GetExID() );
	msg.Add( lID );
	msg.SendToAround( this, this );
}

void CPlayer::ClearEmotion()
{
	m_lEmotionIndex		= 0;
	m_dwEmotonTimeStamp	= 0;

	CMessage msg( MSG_S2C_SHAPE_EMOTION );
	msg.Add( GetType() );
	msg.Add( GetExID() );
	msg.Add( static_cast<LONG>(0) );
	msg.SendToAround( this, this );
}

void CPlayer::PackGlint()
{
	CMessage msg( MSG_S2C_OTHER_PackGlint );
	msg.SendToPlayer( this->GetExID());
}

//添加任务数据到ByteArray
bool CPlayer::CodeQuestDataToDataBlock(DBWriteSet& setWriteDB)
{
	return m_RoleQueue.CodeToDataBlock(setWriteDB)?true:false;
}

//添加任务数据到ByteArray
bool CPlayer::CodeQuestDataToDataBlock_ForClient(DBWriteSet& setWriteDB)
{
	return m_RoleQueue.CodeToDataBlock(setWriteDB)?true:false;
}
bool CPlayer::DecodeQuestDataFromDataBlock(DBReadSet& setReadDB)
{
	return m_RoleQueue.DecodeFromDataBlock(setReadDB)?true:false;
}

//得到一个任务的状态(0:存在但没完成,1:存在且完成,2:没有)
eQuestState CPlayer::GetQuestState(LONG lQuestID)
{
	if(m_RoleQueue.GetQuestFinishedRecord(lQuestID))
	{
		return eQS_Complete;
	}
	else if (m_RoleQueue.GetRecordByID(lQuestID)!=0)
	{
		return eQS_NotComplete;
	}
	return eQS_No;
}

//得到有效的任务数量
long CPlayer::GetValidQuestNum()
{
	return m_RoleQueue.GetQuestSum();
}
//给该玩家添加一个任务

BOOL CPlayer::AddQuest(CQuestRecord*& p)								//添加一个任务
{
	if(!m_RoleQueue.AddOneRecord(p))
		return FALSE;
	GetGame()->UpdateSavePlayer(GetExID(), SAVE_DETAIL_ATTRQUESTLIST);
	return TRUE;
}
BOOL CPlayer::AddQuest(LONG lQuestID)
{
	if (!AddOneRecord(lQuestID))
	{
		return FALSE;
	}
	CQuestRecord* record =m_RoleQueue.GetRecordByID(lQuestID);
	if (record==NULL)
	{
		return FALSE;
	}
	GetLogicLogInterface()->logT460_quest_log(this,lQuestID,1);



	//给客户端发送消息
	CMessage msg(MSG_S2C_QUEST);//(MSG_S2C_Quest_Add);
    msg.Add((BYTE)QS::QS_ADD);
	msg.Add(record->GetQuestID());
	msg.Add(record->GetQuestStep());
	msg.SendToPlayer(GetExID());
	return TRUE ;
}
//该玩家完成一个任务
bool CPlayer::CompleteQuest(DWORD dwQuestID)
{
	//清空队长共享的任务,用于判断队员不可重复接
	if (GetTeamCurrQuest() == dwQuestID)
	{
		m_MerQuestSys.SetMasterQuestID(0);
	}
	m_MerQuestSys.OnDelQuest(dwQuestID);
	m_MerQuestSys.SetLastQuestID(dwQuestID);
	OnCompleteQuest( (long)dwQuestID );
	//执行完成一个任务
	if (!m_RoleQueue.DelOneRecord(dwQuestID))
	{
		return false;
	}
	long lListID=m_MerQuestSys.IsExistQuest(dwQuestID);
	if (0 == lListID)
	{
		lListID=CQuestSystem::GetQuestListID( (long) dwQuestID);
	}
	//清空刷新时间
	if (m_MerQuestSys.ClearTime(lListID))
	{
		FreshMercQuest(lListID);
	}
	if( GetAttributeBeRecord(dwQuestID) ) 
        AddFinishedRoleRecord(dwQuestID);
	GetLogicLogInterface()->logT460_quest_log(this,dwQuestID,2);
    CMessage msg(MSG_S2C_QUEST);//(MSG_S2C_Quest_Complete);
    msg.Add((BYTE)QS::QS_COMPLETE);
	msg.Add(dwQuestID);
	msg.SendToPlayer(GetExID());
	return true;
}
//移出一个任务
void  CPlayer::RemoveQuest(LONG lQuestID)
{
	if(m_RoleQueue.DelOneRecord(lQuestID))
	{
		//string script=CQuestSystem::GetQuest(lQuestID)->GetAttribute().GetDeleteScript();
		////执行删除任务脚本
		//char *str=new char[script.size()+1];
		//memcpy(str,script.c_str(),script.size()+1);
		//PlayerRunScript(str);
		//delete []str;
		GetLogicLogInterface()->logT460_quest_log(this,lQuestID,3);
        CMessage msg(MSG_S2C_QUEST);//(MSG_S2C_Quest_Remove);
        msg.Add((BYTE)QS::QS_REMOVE);
		msg.Add(lQuestID);
		msg.SendToPlayer(GetExID());
	}	
	//更新佣兵任务数据
	if(m_MerQuestSys.OnDelQuest(lQuestID))
	{
		m_MerQuestSys.DisbandQuestRecord(lQuestID);
	}
	SendMercQuestDataToClient(0);
	SetSavePartFlag(GetSavePartFlag()|SAVE_DETAIL_ATTRQUESTLIST);

}

//更新一个任务的步骤
void CPlayer::UpdateQuestStep(DWORD dwQuestID,long lStep)
{
	CQuestRecord* record=m_RoleQueue.GetRecordByID(dwQuestID);
	if (record==NULL)
	{
		return ;
	}
	if (record->GetQuestStep()==lStep)
	{
		return ;
	}

	if(CQuestSystem::GetStep(dwQuestID,lStep)==0)
	{
		return;
	}
	m_RoleQueue.JumpStepTo(dwQuestID,(uchar)lStep);//GetRecordByID(dwQuestID)->JumpStepTo(lStep);
	CMessage msg(MSG_S2C_QUEST);//(MSG_S2C_Quest_UpdateStep);
    msg.Add((BYTE)QS::QS_UPDATE_STEP);
	msg.Add((long)dwQuestID);
	msg.Add(lStep);
	msg.SendToPlayer(GetExID());	
}


void CPlayer::UpdataQuestParamData(DWORD dwQuestID,long lIndex,long lParam, long lRegionID, long lRegionX, long lRegionY)
{
	CQuestRecord* record=m_RoleQueue.GetRecordByID(dwQuestID);
	if (record==NULL)
	{
		return ;
	}

	//m_RoleQueue.GetRecordByID(dwQuestID)->SetParam(lIndex,lParam);
	record->SetParam(lIndex,lParam);
	m_RoleQueue.SetRegionID(record->GetQuestID(),record->GetQuestStep(),lIndex,lRegionID);
	m_RoleQueue.SetRegionX(record->GetQuestID(),record->GetQuestStep(),lIndex,lRegionX);
	m_RoleQueue.SetRegionY(record->GetQuestID(),record->GetQuestStep(),lIndex,lRegionY);
	CMessage msg(MSG_S2C_QUEST);//(MSG_S2C_Quest_UpdateParamData);
    msg.Add((BYTE)QS::QS_UPDATE_PARAM);
	msg.Add((LONG)dwQuestID);
	msg.Add(lIndex);
	msg.Add(lParam);
	msg.Add(lRegionID);
	msg.Add(lRegionX);
	msg.Add(lRegionY);
	msg.SendToPlayer(GetExID());	
	GetGame()->UpdateSavePlayer(GetExID(), SAVE_DETAIL_ATTRQUESTLIST);
}

void CPlayer::UpdataQuestData(DWORD dwQuestID,long lIndex,long lVal)
{
	CQuestRecord* record=m_RoleQueue.GetRecordByID(dwQuestID);
	if (record==NULL)
	{
		return ;
	}

	record->SetParam(lIndex,lVal);
	CMessage msg(MSG_S2C_QUEST);//(MSG_S2C_Quest_UpdateData);
    msg.Add((BYTE)QS::QS_UPDATE_DATA);
	msg.Add((LONG)dwQuestID);
	msg.Add(lIndex);
	msg.Add(lVal);
	msg.SendToPlayer(GetExID());	
	GetGame()->UpdateSavePlayer(GetExID(), SAVE_DETAIL_ATTRQUESTLIST);
}

//添加俑兵任务数据到ByteArray
bool CPlayer::CodeMerQuestDataToDataBlock(DBWriteSet& setWriteDB)
{
	return m_MerQuestSys.CodeToDataBlock(setWriteDB);
}

//解俑兵任务数据
bool CPlayer::DecodeMerQuestDataFromDataBlock(DBReadSet& setReadDB)
{
	return m_MerQuestSys.DecodeFromDataBlock(setReadDB);
}

//获得某个列表ID的俑兵任务
void CPlayer::AcquireMercQuestList(long lMercListID,bool usegoods)
{
//	CMercenaryQuestSys::tagMerQuest MerQuests;
//	bool bResult = false;
//	//删除相应列表中的任务
//	set<long>	DisbandQuests;
//	m_MerQuestSys.GetDisbandQuest(DisbandQuests);
//	for (set<long>::iterator it=DisbandQuests.begin();it!=DisbandQuests.end();it++)
//	{
//		if(lMercListID == CQuestSystem::GetQuestListID( *it ))
//			m_MerQuestSys.DisbandQuestRecord( *it ,true);
//	}
//	//排出在外的quest
//	set<long>	BesidesQuests;
//	m_MerQuestSys.GetMercListQuests(BesidesQuests);
//	BesidesQuests.insert(m_MerQuestSys.GetCurQuestID());
//	//道具刷新
//	if (usegoods)
//	{
//		bResult = CQuestSystem::GetMercQuests(lMercListID,GetLevel(),
//			GetOccupation(),GetRankOfMercenary(),
//			BesidesQuests,MerQuests.Quests,MAX_MERC_QUEST_NUM,
//			usegoods);
//	}
//	//正常刷新
//	else
//	{
//		//排除最近一次完成的任务
//		long lastquestID=m_MerQuestSys.GetLastQuestID();
//		if (lastquestID!=0)
//		{
//			BesidesQuests.insert(lastquestID);
//			m_MerQuestSys.SetLastQuestID(0);
//		}
//		bResult = CQuestSystem::GetMercQuests(lMercListID,GetLevel(),
//			GetOccupation(),GetRankOfMercenary(),
//			BesidesQuests,MerQuests.Quests,MAX_MERC_QUEST_NUM);
//	}
//	if(bResult)
//	{
//		if (MerQuests.Quests.size()==MAX_MERC_QUEST_NUM)
//		{
//			GetLogicLogInterface()->logT461_mercquest_log(this,usegoods?1:0,MerQuests.Quests[0],
//			MerQuests.Quests[1],
//			MerQuests.Quests[2],
//			MerQuests.Quests[3]);
//		}
//		
//		time_t curTime;
//		time(&curTime);
//		MerQuests.dwLastUpdateTime=DWORD(curTime);
//		m_MerQuestSys.SetMercQuests(lMercListID,MerQuests);
//		SetSavePartFlag(GetSavePartFlag()|SAVE_DETAIL_ATTRMERQUESTLIST);
//	}
//	//}
//
}

bool CPlayer::IsMerQuestInit(long lMercListID)
{
	return m_MerQuestSys.IsExistQuestList(lMercListID);
}

//发送俑兵任务数据到客户端
void CPlayer::SendMercQuestDataToClient(long lMercListID)
{		
	//发送消息到客户端
	CMessage msg(MSG_S2C_MercQuest_UpdateData);
	msg.Add(m_MerQuestSys.GetCurQuestID());
	//获取当前列表放弃的任务
	set<long>quests;
	m_MerQuestSys.GetDisbandQuest(quests);
	long lsize=(long)quests.size();
	set<long>::iterator it=quests.begin();
	msg.Add(lsize);
	for (;it!=quests.end();it++)
	{
		msg.Add( (*it) );
	}
	//当前俑兵任务
	if (lMercListID==0)
	{
		msg.Add((long)0);
		msg.SendToPlayer(GetExID());
		return;
	}
	else
		msg.Add((long)1);

	msg.Add(lMercListID);
	if(lMercListID)
	{	
		CMercenaryQuestSys::tagMerQuest MerQuests;
		bool bResult = m_MerQuestSys.GetMercQuests(lMercListID,MerQuests);
		if(bResult)
		{
			time_t curTime;
			time(&curTime);
			long lUpdateTime = (long)(CQuestSystem::GetQuestListFreshTime(lMercListID)-(curTime-MerQuests.dwLastUpdateTime));
			msg.Add(lUpdateTime);
			msg.Add((WORD)MerQuests.Quests.size());
			vector<long>::iterator it = MerQuests.Quests.begin();
			for(;it !=  MerQuests.Quests.end();it++)
			{
				msg.Add((*it));
				if (m_RoleQueue.GetQuestFinishedRecord(*it) )
				{
					msg.Add((long)1);
				}
				else
				{
					msg.Add((long)0);
				}
			}
		}
		else
			msg.Add((long)0);
	}
	msg.SendToPlayer(GetExID());
}

//打开俑兵任务界面
void CPlayer::OpenMercQuestInterface(long lMercListID)
{
	CMessage msg(MSG_S2C_MercQuest_OpenInterface);
	msg.Add(lMercListID);
	msg.Add(CQuestSystem::GetQuestListType(lMercListID));
	msg.SendToPlayer(GetExID());
}
void CPlayer::OpenShareQuestInterface()
{
	CMessage msg(MSG_S2C_SHARE_QUEST_OPEN);
	msg.SendToPlayer(GetExID());
}
//发送当前正在进行的任务ID
long CPlayer::GetTeamCurrQuest()
{
	CGUID guid=this->GetTeamID();
	GSTeam* pteam=GetOrganiCtrl()->GetGameTeamByID(guid);
	if (pteam)
	{
		if (NULL_GUID!=pteam->IsMember(m_guid))
		{
			return pteam->GetCurrShareQuest();
			/*CMessage msg(MSG_S2C_SHARE_CURRQUEST);
			msg.Add(pteam->GetCurrShareQuest());
			msg.SendToPlayer(GetExID());*/
		}	
	}	
	return 0;
}
void CPlayer::SetMasterQuest(long lQuestID)
{
	m_MerQuestSys.SetMasterQuestID(lQuestID);
	return;
}
//队长设置队伍当前任务ID
void CPlayer::SetTeamCurrQuest(long lQuestID)
{
	CGUID guid=this->GetTeamID();
	GSTeam* pteam=GetOrganiCtrl()->GetGameTeamByID(guid);
	if (pteam)
	{
		//向WS发送更新队伍当前任务ID消息
		if (NULL_GUID!=pteam->IsMaster(m_guid))
		{
			SetMercQuest(lQuestID,false);
			CMessage msg(MSG_S2W_SHARE_CURRQUEST);
			msg.Add(this->GetExID());
			msg.Add(guid);
			msg.Add(lQuestID);
			msg.Send(false);
		}
	}
}
//删险共享列表中的任务
void CPlayer::DeleteTeamQuest(long lQuestID)
{
	CGUID guid=this->GetTeamID();
	GSTeam* pteam=GetOrganiCtrl()->GetGameTeamByID(guid);
	if (pteam)
	{
		if (NULL_GUID!=pteam->IsMember(m_guid))
		{
			if (pteam->FindShareQuest(lQuestID))
			{
				CMessage msg(MSG_S2W_SHARE_DELQUEST);
				long len = (long)strlen(GetName());
				msg.Add(len);
				msg.AddEx((void*)GetName(),len);
				msg.Add(m_guid);
				msg.Add(guid);
				msg.Send(false);
			}
		}
	}
}

bool CPlayer::UpdateTeamQuestList(long lQuestID)
{
	//存在于赏金列表中，但不存在于放弃的赏金任务列表中
	if ( IsMercListQuest(lQuestID) && 
		m_MerQuestSys.FindDisbandQuest(lQuestID)==0 &&
		m_TeamID!=NULL_GUID)
	{
		//推荐与接受合为一个操作
		//SetMercQuest(lQuestID);
		//stRunScript sScript;
		//sScript.desShape = NULL;
		//CRegion *pRegion = dynamic_cast<CRegion*>(GetGame()->FindRegion(this->GetRegionGUID()));
		//if (pRegion==NULL)
		//	return false;
		//sScript.pRegion = pRegion;
		//sScript.srcShape=this;	
		//const char* strScript =CQuestSystem::GetAddQuestScript();
		//if (strScript == NULL)
		//	return false;
		//sScript.pszFileName = (char*)strScript;
		//RunScript( &sScript, const_cast<CHAR*>( GetGame() -> GetScriptFileData(strScript) ), lQuestID ,0);

		CRegion *pRegion = dynamic_cast<CRegion*>(GetGame()->FindRegion(this->GetRegionGUID()));
		if (pRegion==NULL)
			return false;
		const char* strScript =CQuestSystem::GetAddQuestScript();
		if (strScript == NULL)
			return false;

		Script::RunEnv env( this, NULL, pRegion );
		GetInst( ScriptSys ).RunScript( env, strScript );

		m_MerQuestSys.DisbandQuestRecord(lQuestID);
		return true;
	}
	return false;	
}
//俑兵任务刷新时间超时
//void CPlayer::OnUpdateMercQuestTimeOut()
//{
//	ResetAllMercQuestData();
//	//刷性上次界面打开的俑兵任务
//	AcquireMercQuestList(GetLastAcquireMercListID());
//	//发送到客户端
//	SendMercQuestDataToClient(GetLastAcquireMercListID());
//}

void CPlayer::OnCompleteQuest(long lQuestID)
{
	if (m_TeamID != NULL_GUID)
	{
		GSTeam* pTeam = GetOrganiCtrl()->GetGameTeamByID(m_TeamID);
		if (pTeam)
		{
			//是队长
			if (pTeam->IsMaster(m_guid)!=NULL_GUID)
			{
				//SetTeamCurrQuest(0);
				DeleteTeamQuest(lQuestID);
			}
			if (pTeam->GetCurrShareQuest() == lQuestID)
			{
				CMessage msg(MSG_S2W_SHARE_SETMEMBERQUEST);
				msg.Add(m_guid);
				msg.Add(m_TeamID);
				msg.Add(lQuestID);
				msg.Send(false);
			}
		}		
	}
}

bool CPlayer::IsCompleteQuest(long lQuestID)
{
	if (m_TeamID != NULL_GUID)
	{
		GSTeam* pTeam = GetOrganiCtrl()->GetGameTeamByID(m_TeamID);
		if(pTeam)
		{
			if (pTeam->IsCompleteQuest(m_guid,lQuestID))
			{
				return true;
			}
			else
				return false;
		}	
	}
	return false;
}

DWORD CPlayer::GetQuestOdds(long lQuestID)
{
	const CQuest* quest=CQuestSystem::GetQuest(lQuestID);
	if (quest)
	{
		return quest->GetAttribute().GetOdds();
	}
	return 0;
}

long CPlayer::CanAcceptMercQuest(long lQuestID)
{
	if (lQuestID == m_MerQuestSys.GetMasterQuestID())
	{
		return lQuestID;
	}
	return 0;
}
//设置俑兵任务
void CPlayer::SetMercQuest(long lQuestID,bool bcall)
{
	//接受任务后,将队伍共享的任务置0,不允许重接
	/*if (GetTeamCurrQuest() == lQuestID)
	{
		m_MerQuestSys.SetMasterQuestID(0);
	}*/
	long lQuestListID = m_MerQuestSys.OnAddQuest(lQuestID);
	if(lQuestListID==0)
	{	
		GSTeam* pTeam=GetOrganiCtrl()->GetGameTeamByID(this->GetTeamID());
		if (pTeam)
		{
			//是队长则在列表中查找是否有此任务
			if (pTeam->IsMaster(m_guid)!=NULL_GUID && !bcall)
			{
				if (pTeam->FindShareQuest(lQuestID))
				{
					m_MerQuestSys.SetCurQuestID(lQuestID);
					SetSavePartFlag(GetSavePartFlag()|SAVE_DETAIL_ATTRMERQUESTLIST);
				}
			}
			else
			{
				if (pTeam->GetCurrShareQuest()==lQuestID)
				{
					m_MerQuestSys.SetCurQuestID(lQuestID);
					SetSavePartFlag(GetSavePartFlag()|SAVE_DETAIL_ATTRMERQUESTLIST);
				}	
			}
		}	
	}
	else
	{
		//不存在于放弃任务列表中
		if (!IsDisbandQuest(lQuestID))
		{
			m_MerQuestSys.SetCurQuestID(lQuestID);
			SetSavePartFlag(GetSavePartFlag()|SAVE_DETAIL_ATTRMERQUESTLIST);
		}
		
	}
	SendMercQuestDataToClient(lQuestListID);
}

//玩家进入GS以后初始化相关俑兵任务数据
void CPlayer::IniMercQuest()
{
	//判断当前佣兵任务是否在任务列表中
	if( GetQuestState(m_MerQuestSys.GetCurQuestID()) == eQS_No )
		m_MerQuestSys.SetCurQuestID(0);

}

//是否能刷新
long CPlayer::CanFreshMercQuest(long lMercListID)
{
	long type= CQuestSystem::GetQuestListType(lMercListID);
	DWORD Time= CQuestSystem::GetQuestListFreshTime(lMercListID);
	CMercenaryQuestSys::tagMerQuest mq;
	m_MerQuestSys.GetMercQuests(lMercListID,mq);

	time_t curTime;
	time(&curTime);

	//mq.Quests.size()==0 &&
	if (mq.Quests.size()==0 || mq.dwLastUpdateTime+ Time< DWORD(curTime))
	{
		return FT_YES;
	}

	if (type== 2)
	{
		return FT_NEEDGOODS;
	}

	return FT_NO;
}

//刷新
void CPlayer::FreshMercQuest(long lMercListID,bool usegoods)
{
	int FreshType= CanFreshMercQuest(lMercListID);
	if (FreshType==FT_YES)
	{
		AcquireMercQuestList(lMercListID);
		SendMercQuestDataToClient(lMercListID);
		//OpenMercQuestInterface(lMercListID);
		return;
	}
	else if (FreshType== FT_NEEDGOODS )
	{
		map<long,long>& mapItem=CQuestSystem::GetMerItem();
		map<long,long>::iterator it = mapItem.begin();
		for (;it!=mapItem.end();it++)
		{
			char* goodsname= (char*)GoodsSetup::QueryGoodsOriginalName((DWORD)it->second);
			if (goodsname!=NULL)
			{
		if (usegoods && DelGoods(goodsname,1))
		{
			AcquireMercQuestList(lMercListID,usegoods);
			SendMercQuestDataToClient(lMercListID);
			return;
		}
			}
		}

		//发送失败信息
	}
	//发送失败信息
}

//判断是否是俑兵列表任务
bool CPlayer::IsMercListQuest(long lQuestID)
{
	//任务列表中不存在此任务
	if (m_MerQuestSys.IsExistQuest(lQuestID)==0)
	{
		return false;
	}
	return true;
}

bool CPlayer::IsDisbandQuest(long lQuestID)
{
	//存在于任务列表也存在于放弃任务列表中
	if (m_MerQuestSys.FindDisbandQuest(lQuestID)!=0)
	{
		return true;
	}
	else
		return false;
}

void CPlayer::ShowTalkBox(DWORD dwQuestID,long lTextStep)
{
	CMessage msg(MSG_S2C_QUEST);
    msg.Add((BYTE)QS::QS_TALK);
	msg.Add((LONG)dwQuestID);
	msg.Add(lTextStep);
	msg.SendToPlayer(GetExID());	
}
//给该玩家运行一个脚本
void CPlayer::PlayerRunScript(const char *strScriptName)
{
	if(strScriptName == NULL)	return;

	Script::RunEnv env( this, NULL, static_cast<CRegion*>( GetFather() ) );
	GetInst( ScriptSys ).RunScript( env, strScriptName );
}

long CPlayer::GetCurrentMasterRoleID()
{
	list<CQuestRecord*>* pRoleList= m_RoleQueue.GetQuestRecordList();
	if (pRoleList!= NULL)
	{
		for (list<CQuestRecord*>::iterator it= pRoleList->begin(); it!= pRoleList->end(); it++)
		{
			const CQuest* pQuest =CQuestSystem::GetQuest((*it)->GetQuestID());
			if (pQuest== NULL) break;
			const CQuestAttribute& pAttri= pQuest->GetAttribute();
			if (pAttri.GetQuestType()== 1) return (*it)->GetQuestID();
		}
	}
	return 0;
}

FLOAT CPlayer::GetWeaponModifier( LONG lTargetLevel )
{
	FLOAT fResult = 1.0f;
	GlobalSetup::tagSetup *pSetup = GlobalSetup::GetSetup();
	if( pSetup )
	{
		DWORD dwWeaponModifier = 0;
		CGoods* pGoods = m_cEquipment.GetGoods( EC_E_WEAPON );
		if( pGoods )
		{
			dwWeaponModifier = pGoods -> GetAddonPropertyValues( GAP_WEAPON_DAMAGE_LEVEL, 1 );
		}

		long lTemp = ( dwWeaponModifier - lTargetLevel );
		if (lTemp<0)
			lTemp = 0;
		fResult = lTemp / pSetup -> m_fWeaponPowerModifier;

		if( fResult > 1.0f )
		{
			fResult = 1.0f;
		}
		if( fResult < pSetup -> m_fWeaponMinPowerModifier )
		{
			fResult = pSetup -> m_fWeaponMinPowerModifier;
		}
	}
	return fResult;
}

DWORD CPlayer::GetWeaponDamageLevel() {		// 获取当前使用的武器的攻击强度
	DWORD dwWeaponDamageLevel = 0;
	CGoods *pGoods = m_cEquipment.GetGoods(EC_E_WEAPON); 

	if(!pGoods) return 0;

	dwWeaponDamageLevel = pGoods->GetAddonPropertyValues( GAP_WEAPON_DAMAGE_LEVEL, 1 );	


	return dwWeaponDamageLevel;	
}

//
bool CPlayer::AddOneRecord(long roleid)
{
	const CQuest* quest=CQuestSystem::GetQuest(roleid);
	const CQuestStep* step=0;

	if (quest==0)
	{
		return false;
	}
	step=quest->GetStep();

	if (step==0)
	{
		return false;
	}

	CQuestRecord* record=new CQuestRecord(roleid, step->GetStepID());

	if(!AddQuest(record))
	{
		delete record;
		//输出错误信息： 加入任务不成功
		return false;
	}
	return true;
}

bool CPlayer::SetChapter(long num)
{ 
	m_RoleQueue.SetChapter(num);
	GetGame()->UpdateSavePlayer(GetExID(), SAVE_DETAIL_ATTRQUESTLIST);
	return true;
}

bool  CPlayer::AddFinishedRoleRecord(long index)
{
	if( m_RoleQueue.AddFinishedQuestRecord(index))
	{
		CMessage msg(MSG_S2C_QUEST);//MSG_S2C_Quest_UpdateRecordData
        msg.Add((BYTE)QS::QS_UPDATE_RECORD);
		msg.Add(index);
		msg.SendToPlayer(GetExID());
		GetGame()->UpdateSavePlayer(GetExID(), SAVE_DETAIL_ATTRQUESTLIST);
		return true;
	}
	return false;
}

bool CPlayer::DelFinishedRoleRecord(long index)
{
	return m_RoleQueue.DelFinishedQuestRecord(index);
}

eQuestTextId CPlayer::GetQuestStateId(long QuestId)
{
    /// 没有领取
    eQuestState st =  GetQuestState( QuestId ) ;
    if ( st == eQS_No )
    {
         return eNoQuest;
    }
    /// 领取已经完成
    else  if( st == eQS_Complete )
    {
         return eComQuest;
    }
    /// 已接
    else 
    {
        const CQuestStep * step = GetStep( QuestId , 1 );

        if ( step  )
        {
            for ( int i = 0 ; i < step->GetStepAimNum() ; i++ )
            {
                const CQuestAim * aim = step->GetAimHead();
                if ( aim )
                {
                    if( aim->GetAimNum() > GetRoleParam(QuestId,1,aim->GetID()-1 ) )
                        return eNoComQuest_No;
                }
            }
            return eNoComQuest_Fin;
        }
    }
    return   eNoQuestId;
}

const char*  CPlayer::GetQuestTextId(long QuestId,long QState)
{
    return AppFrame::LoadQuestTextRes(QuestId,QState);
}

//////////////////////////////////////////////////////////.. 任务系统，对任务的值的查询
const CQuest*	CPlayer::GetQuest(long RoleID)
{
	return CQuestSystem::GetQuest(RoleID);
}

const CQuestStep* CPlayer::GetStep(long RoleID, long StepID)				//寻找步骤
{
	const CQuest* pQuest=CQuestSystem::GetQuest(RoleID);
	if (pQuest==NULL)
	{
		return NULL;
	}
	return pQuest->GetStep(StepID);
}
const CQuestAim*  CPlayer::GetAim(long RoleID, long StepID,long AimID)	//寻找任务目标
{
	const CQuest* pQuest=CQuestSystem::GetQuest(RoleID);
	if (pQuest==NULL)
	{
		return NULL;
	}
	const CQuestStep*pStep=pQuest->GetStep(StepID);
	if (pStep==NULL)
	{
		return NULL;
	}
	return pStep->GetAim(AimID);

}

const CQusetEncouragement *CPlayer::GetQuestEncouragement(long RoleID,long Index)
{
	CQusetEncouragement* encouragement=CQuestSystem::GetEncouragementHead(RoleID);//quest->GetEncouragementHead();
	int EncouragementSum=CQuestSystem::GetEncouragementSum(RoleID);
	if (Index > EncouragementSum)
	{
		return NULL;
	}
	for (int a=0; a<Index-1  ; a++)
	{
		encouragement=encouragement->next;
	}
	return encouragement;
}

const CQusetEncouragement *CPlayer::GetStepEncouragement(long RoleID,long  StepID,long Index)
{
// 	const CQuestStep* step=GetStep(RoleID,StepID);
// 
// 	if (step==NULL)
// 	{
// 		return NULL;
// 	}
// 
// 	const CQusetEncouragement* encouragement=step->GetRolePropHead();
// 	int EncouragementSum=step->GetRolePropSum();
// 
// 	if (Index> EncouragementSum)
// 	{
// 		return NULL;
// 	}
// 
// 	for (int a=0; a<Index-1; a++)
// 	{
// 		encouragement=encouragement->next;
// 	}
    return NULL;
}

//取得文本类型
long CPlayer::GetTextType(long RoleID)
{
	const CQuest* quest=GetQuest(RoleID);
	if (quest)
	{
		return quest->GetTextType();
	}
	return 0;
}
//attribute:
//LONG AttributeGetID(LONG RoleID)const{return m_lID;}
//取得任务名字
const char* CPlayer::GetAttributeRoleName(long RoleID)
{
    char   quest[32];
    sprintf_s(quest,"Q%6d",RoleID);
    return AppFrame::GetText(quest);
}
//是否被记录
long CPlayer::GetAttributeBeRecord(long RoleID)
{
	const CQuest * quest=GetQuest(RoleID);
	if (quest)
	{
		return quest->GetAttribute().GetBeRecord();
	}
	return 0;
}
//任务类型
long CPlayer::GetAttributeQuestType(long RoleID)
{
	const CQuest * quest=GetQuest(RoleID);
	if (quest)
	{
		return quest->GetAttribute().GetQuestType();
	}
	return 0;
}
//任务是否可以共享
long CPlayer::GetAttributeQuestIsShare(long RoleID)
{
	const CQuest * quest=GetQuest(RoleID);
	if (quest)
	{
		return quest->GetAttribute().GetQuestIsShare();
	}
	return 0;
}
//是否被显示
long CPlayer::GetAttributeShowAble(long RoleID)
{
	const CQuest * quest=GetQuest(RoleID);
	if (quest)
	{
		return quest->GetAttribute().GetShowAble();
	}
	return 0;
}
//是否能反复接
long CPlayer::GetAttributeRepeated(long RoleID)
{
	const CQuest * quest=GetQuest(RoleID);
	if (quest)
	{
		return quest->GetAttribute().GetRepeated();
	}
	return 0;
}
//死亡是否失败
long CPlayer::GetAttributeDeadLost(long RoleID)
{
	const CQuest * quest=GetQuest(RoleID);
	if (quest)
	{
		return quest->GetAttribute().GetDeadLost();
	}
	return 0;
}
//任务失败后能否重新接
long CPlayer::GetAttributeLostRepeated(long RoleID)
{
	const CQuest * quest=GetQuest(RoleID);
	if (quest)
	{
		return quest->GetAttribute().GetLostRepeated();
	}
	return 0;
}
//最小等级限制
long CPlayer::GetAttributeMinGrade(long RoleID)
{
	const CQuest * quest=GetQuest(RoleID);
	if (quest)
	{
		return quest->GetAttribute().GetMinGrade();
	}
	return 0;
}
//最大等级限制
long CPlayer::GetAttributeMaxGrade(long RoleID)
{
	const CQuest * quest=GetQuest(RoleID);
	if (quest)
	{
		return quest->GetAttribute().GetMaxGrade();
	}
	return 0;
}
//职业限制
bool CPlayer::GetAttributeOccupationRestrict(long RoleID,int index)
{
	const CQuest * quest=GetQuest(RoleID);
	if (quest)
	{
		return quest->GetAttribute().GetOccupationRestrict(index);
	}
	return false;
}
//任务几率
long CPlayer::GetAttributeOdds(long RoleID)
{
	const CQuest * quest=GetQuest(RoleID);
	if (quest)
	{
		return quest->GetAttribute().GetOdds();
	}
	return 0;
}
// 性别限制
bool CPlayer::GetAttributeQuestSex(long QuestID)
{
	const CQuest * quest=GetQuest(QuestID);
	if (quest)
	{
		BYTE lSex = quest->GetAttribute().GetQuestSex();
        if ( lSex == 2 )
            return true;
        return GetSex() == lSex ;
//		return quest->GetAttribute().GetEngageArmsRepute();
	}
	return false;
}
// 技能限制
bool CPlayer::GetAttributeQuestSkill(long QuestID)
{
    const CQuest * quest=GetQuest(QuestID);
    if (quest)
    {
        map<long,tagSkill>::iterator it = m_Skills.find(quest->GetAttribute().GetReqSkill());
        if (it!=m_Skills.end())
        {
            return true;
        }
    }
    return false;
}
// 携带宠物
bool CPlayer::GetAttributeTotePet(long QuestID)
{
    const CQuest * quest =GetQuest(QuestID);
    if ( quest )
    {
        long  PetID = quest->GetAttribute().GetTotePet();
    }
    return false;
}

//下一任务的id
long CPlayer::GetAttributeNextRoleID(long RoleID)
{
	const CQuest* quest=GetQuest(RoleID);
	if (quest)
	{
		return quest->GetAttribute().GetNextRoleID();
	}

	return 0;
}
//章节数
long CPlayer::GetAttributeChapterNum(long RoleID)
{
	const CQuest * quest=GetQuest(RoleID);
	if (quest)
	{
		return quest->GetAttribute().GetChapterNum();
	}
	return 0;
}

//
long CPlayer::GetAttributeFinishEffect(long RoleID)
{
	const CQuest * quest=GetQuest(RoleID);
	if (quest)
	{
		return quest->GetAttribute().GetFinishEffect();
	}
	return 0;
}

//取得index对应的前置任务
long CPlayer::GetLabelPrefix(long RoleID,long Index)
{
	const CQuest * quest=GetQuest(RoleID);
	if (quest)
	{
		return quest->GetLabelPrefix().GetLabelPrefix(Index);
	}
	return 0;
}
//取得前置任务数量
long CPlayer::GetLabelSum(long RoleID)
{
	const CQuest * quest=GetQuest(RoleID);
	if (quest)
	{
		return quest->GetLabelPrefix().GetLabelSum();
	}
	return 0;
}

//目标模板类型
long	CPlayer::GetAimAMTType(long RoleID,long StepID,long AimID)
{
	const CQuestAim* aim=GetAim(RoleID,StepID,AimID);
	if (aim)
	{
		return aim->GetAMTType();
	}
	return 0;
}
//目标类型
long CPlayer::GetAimType(long RoleID,long StepID,long AimID)
{
	const CQuestAim* aim=GetAim(RoleID,StepID,AimID);
	if (aim)
	{
		return aim->GetAimType();
	}
	return 0;
}
//取得目标id
long CPlayer::GetAimId(long RoleID,long StepID,long AimID)
{
	const CQuestAim* aim=GetAim(RoleID,StepID,AimID);
	if (aim)
	{
		return aim->GetAimId();
	}
	return 0;
}
//取得目标数量
long CPlayer::GetAimNum(long RoleID,long StepID,long AimID)
{
	const CQuestAim* aim=GetAim(RoleID,StepID,AimID);
	if (aim)
	{
		return aim->GetAimNum();
	}
	return 0;
}
//取得 变量索引
long CPlayer::GetAimParameIndex(long RoleID,long StepID,long AimID)
{
	const CQuestAim* aim=GetAim(RoleID,StepID,AimID);
	if (aim)
	{
		return aim->GetParameIndex();
	}
	return 0;
}
//掉落这种物品的生物类型（对物品）
long CPlayer::GetDropFrom(long RoleID,long StepID,long AimID)
{
	const CQuestAim* aim=GetAim(RoleID,StepID,AimID);
	if (aim)
	{
		return aim->GetDropFrom();
	}
	return 0;
}
//几率
long CPlayer::GetDropOdds(long RoleID,long StepID,long AimID)
{
	const CQuestAim* aim=GetAim(RoleID,StepID,AimID);
	if (aim)
	{
		return aim->GetDropOdds();
	}
	return 0;
}

const char* CPlayer::GetAimName(long RoleID,long StepID,long AimID)
{
	const CQuestAim* aim=GetAim(RoleID,StepID,AimID);
	if (aim)
	{
		return aim->GetName();
	}
	return 0;
}

//取得备用属性
long CPlayer::GetPrepareParam(long RoleID,long StepID,long AimID,long index)
{
	const CQuestAim* aim=GetAim(RoleID,StepID,AimID);
	if (aim)
	{
		return aim->GetPrepareParam(index);
	}
	return 0;
}


long CPlayer::GetQuestEncouragementSum(long RoleID)
{
	return CQuestSystem::GetEncouragementSum(RoleID);
}

//取得奖励的名字
const char* CPlayer::GetSetpEncouragementName(long RoleID,long Step,long Index)
{
	const CQusetEncouragement* encouragement=GetStepEncouragement(RoleID,Step,Index);
	if (encouragement==NULL)
	{
		return NULL;
	}
	return encouragement->GetName();
}
const char* CPlayer::GetQuestEncouragementName(long RoleID,long Index)
{
	const CQusetEncouragement* encouragement=GetQuestEncouragement(RoleID,Index);
	if (encouragement==NULL)
	{
		return NULL;
	}
	return encouragement->GetName();
}
//取得奖励类型
long CPlayer::GetStepEncouragementType(long RoleID,long Step,long Index)
{
	const CQusetEncouragement* encouragement=GetStepEncouragement(RoleID,Step,Index);
	if (encouragement==NULL)
	{
		return 0;
	}
	return encouragement->GetType();
}
long CPlayer::GetQuestEncouragementType(long RoleID,long Index)
{
	const CQusetEncouragement* encouragement=GetQuestEncouragement(RoleID,Index);
	if (encouragement==NULL)
	{
		return 0;
	}
	return encouragement->GetType();
}
//奖励的具体id
long CPlayer::GetStepEncouragementID(long RoleID,long Step,long Index)
{
	const CQusetEncouragement* encouragement=GetStepEncouragement(RoleID,Step,Index);
	if (encouragement==NULL)
	{
		return 0;
	}
	return encouragement->GetID();
}
long CPlayer::GetQuestEncouragementID(long RoleID,long Index)
{
	const CQusetEncouragement* encouragement=GetQuestEncouragement(RoleID,Index);
	if (encouragement==NULL)
	{
		return 0;
	}
	return encouragement->GetID();
}
//奖励数量
long CPlayer::GetStepEncouragementNum(long RoleID,long Step,long Index)
{
	const CQusetEncouragement* encouragement=GetStepEncouragement(RoleID,Step,Index);
	if (encouragement==NULL)
	{
		return 0;
	}
	return encouragement->GetNum();
}
long CPlayer::GetQuestEncouragementNum(long RoleID,long Index)
{
	const CQusetEncouragement* encouragement=GetQuestEncouragement(RoleID,Index);
	if (encouragement==NULL)
	{
		return NULL;
	}
	return encouragement->GetNum();
}
//奖励的格子数
long CPlayer::GetStepEncouragementCell(long RoleID,long Step)
{
	const CQuestStep* step=GetStep(RoleID,Step);
	if (step==NULL)
	{
		return 0;
	}
	return step->GetRolePropCell();
}
long CPlayer::GetQuestEncouragementCell(long RoleID)
{
	 return CQuestSystem::GetEncouragementCell(RoleID);
}
//执行奖励
bool CPlayer::PerformEncouragement(long RoleID)
{
	const CQuest* quest=GetQuest(RoleID);
	return false;
}
bool CPlayer::PerformEncouragement(long RoleID,long StepID)
{
	const CQuestStep* step=GetStep(RoleID,StepID);
	return false;
}

//单个任务相关
long CPlayer::GetStepSum(long RoleID)
{
	const CQuest*  quest=GetQuest(RoleID);
	if (quest == NULL)
	{
		return 0;
	}
	return quest->GetStepSum();
}

long CPlayer::GetStepAimNum(long RoleID,long StepID)
{
	const CQuestStep* step=GetStep(RoleID,StepID);
	if (step==NULL)
	{
		return 0;
	}
	return step->GetStepAimNum();
}
//任务系统相关
long CPlayer::FindRoleIsInSystem(long RoleID)
{
	const CQuest* quest=GetQuest(RoleID);
	if (quest==NULL)
	{
		return 0;
	}
	return 1;
}

bool CPlayer::FindRoleStepIsInSystem(long RoleID,long StepID)
{
	const CQuest* quest=GetQuest(RoleID);
	if (quest==NULL)
	{
		return false;
	}
	const CQuestStep* step=quest->GetStep(StepID);
	if (step==NULL)
	{
		return false;
	}
	return true;
}
//////////////////////////////////////////////////////////..end



//添加一个帮助按钮提示
long CPlayer::AddHelpHint(const char* pszScript,long lFutureDelTime)
{
	if(NULL == pszScript)	return 0;
	//如果大于制定数目，则删除最前边的提示
	long lHintID = 0;
	if( (long)m_HelpHints.size() > GlobalSetup::GetSetup()->lMaxHelpHintNum)
	{
		lHintID = m_HelpHints.begin()->first;
		//移出一个帮助提示
		RemoveHelpHint(lHintID);
	}

	//添加帮助按钮提示
	lHintID = ++m_nHintCounter;

	tagTimerVar* pTimvar = OBJ_CREATE(tagTimerVar);
	pTimvar->TimerType = ChildObjType_PlayerHint;
	pTimvar->lvar = lHintID;
	long lTimerID = TIMER_SCHEDULE(this,(void*)pTimvar,timeGetTime()+lFutureDelTime,0);
	if(lTimerID != 0)
	{
		tagHint hint = {pszScript,lTimerID,random(1000)};
		m_HelpHints[lHintID] = hint;

		CMessage msg(MSG_S2C_OTHER_SETHELPHINT);
		//添加
		msg.Add((BYTE)0);
		msg.Add(lHintID);
		msg.Add(hint.lFlag);
		msg.Add(lFutureDelTime);
		msg.SendToPlayer(GetExID());
	}
	else
	{
		Log4c::Warn(ROOT_MODULE, GetText("GS_PLAYER_4"));//添加帮助提示按钮时,定制定时器出错。
		lHintID = 0;
	}
	return lHintID;
}

//移出一个帮助提示
void CPlayer::RemoveHelpHint(long lHintID)
{
	itHint it = m_HelpHints.find(lHintID);
	if( it != m_HelpHints.end())
	{
		tagHint &hint = (*it).second;
		CMessage msg(MSG_S2C_OTHER_SETHELPHINT);
		//移出
		msg.Add((BYTE)1);
		msg.Add((*it).first);
		msg.SendToPlayer(GetExID());
		//删除定时器
		if(hint.lTimerID != 0)
		{
			TIMER_CANCEL(hint.lTimerID);
		}
		m_HelpHints.erase(it);
	}
}

//当玩家点击帮助提示按钮
void CPlayer::OnPlayerHitHelpHint(long lHintID,long lFlag)
{
	string strScript("");
	itHint it = m_HelpHints.find(lHintID);
	if( it != m_HelpHints.end())
	{
		tagHint &hint = (*it).second;
		//标志位不同,则返回
		if(hint.lFlag != lFlag)
			return;		
		strScript = hint.strScript;
		//删除定时器
		if(hint.lTimerID != 0)
		{
			TIMER_CANCEL(hint.lTimerID);
		}
		m_HelpHints.erase(it);
	}

	//运行脚本
	PlayerRunScript((char*)strScript.c_str());
}

//添加一个界面提示
long CPlayer::AddInterfaceHint(const char* pszScript,long lFutureDelTime,
							   const char* pszInterName,long lX,long lY,long lWidth,long lHeight)
{
	if(NULL == pszScript || NULL == pszInterName)	return 0;

	//添加界面提示
	long lHintID = ++m_nHintCounter;

	tagTimerVar* pTimvar = OBJ_CREATE(tagTimerVar);
	pTimvar->TimerType = ChildObjType_PlayerHint;
	pTimvar->lvar = lHintID;
	long lTimerID = TIMER_SCHEDULE(this,(void*)pTimvar,timeGetTime()+lFutureDelTime,0);
	if(lTimerID != 0)
	{
		tagHint hint = {pszScript,lTimerID,random(1000)};
		m_InterfaceHints[lHintID] = hint;

		CMessage msg(MSG_S2C_OTHER_SETINTERFACEHINT);
		//添加
		msg.Add((BYTE)0);
		msg.Add(lHintID);
		msg.Add(hint.lFlag);
		msg.Add(lFutureDelTime);
		msg.Add(pszInterName);
		msg.Add(lX);
		msg.Add(lY);
		msg.Add(lWidth);
		msg.Add(lHeight);
		msg.SendToPlayer(GetExID());
	}
	else
	{
		Log4c::Warn(ROOT_MODULE, GetText("GS_PLAYER_5"));//添加界面提示时,定制定时器出错。
		lHintID = 0;
	}
	return lHintID;
}

//移出一个界面提示
void CPlayer::RemoveInterfaceHint(long lHintID)
{
	itHint it = m_InterfaceHints.find(lHintID);
	if( it != m_InterfaceHints.end())
	{
		tagHint &hint = (*it).second;
		CMessage msg(MSG_S2C_OTHER_SETINTERFACEHINT);
		//移出
		msg.Add((BYTE)1);
		msg.Add((*it).first);
		msg.SendToPlayer(GetExID());
		//删除定时器
		if(hint.lTimerID != 0)
		{
			TIMER_CANCEL(hint.lTimerID);
		}
		m_InterfaceHints.erase(it);
	}
}
//加信
//bool CPlayer::SetMapMail(CMail *pMail)
//{
//	time_t t_CurrentTime = time(NULL);
//	map<CGUID,CMail*>::iterator it=m_mapMail.find(pMail->GetExID());
//	if (it==m_mapMail.end())
//	{
//		m_mapMail[pMail->GetExID()] = pMail;		
//	}
//	else
//	{
//		it=m_mapMail.erase(it);
//		m_mapMail[pMail->GetExID()] = pMail;
//	}
//
//	if (pMail->GetRemainTime() - t_CurrentTime >0)
//		return true;
//	else		
//		return false;
//}

//当玩家点击界面提示
void CPlayer::OnPlayerHitInterfaceHint(long lHintID,long lFlag)
{
	string strScript("");
	itHint it = m_InterfaceHints.find(lHintID);
	if( it != m_InterfaceHints.end())
	{
		tagHint &hint = (*it).second;
		//标志位不同,则返回
		if(hint.lFlag != lFlag)
			return;		
		strScript = hint.strScript;
		//删除定时器
		if(hint.lTimerID != 0)
		{
			TIMER_CANCEL(hint.lTimerID);
		}
		m_InterfaceHints.erase(it);
	}
	//运行脚本
	PlayerRunScript((char*)strScript.c_str());
}

//当提示超时时
void CPlayer::OnHintTimeOut(long lHintID)
{
	itHint it = m_HelpHints.find(lHintID);
	if( it != m_HelpHints.end())
	{
		m_HelpHints.erase(it);
		CMessage msg(MSG_S2C_OTHER_SETHELPHINT);
		//移出
		msg.Add((BYTE)1);
		msg.Add(lHintID);
		msg.SendToPlayer(GetExID());
		return;
	}

	it = m_InterfaceHints.find(lHintID);
	if( it != m_InterfaceHints.end())
	{
		m_InterfaceHints.erase(it);
		CMessage msg(MSG_S2C_OTHER_SETINTERFACEHINT);
		//移出
		msg.Add((BYTE)1);
		msg.Add(lHintID);
		msg.SendToPlayer(GetExID());
		return;
	}
	return;
}

//清除所有提示
void CPlayer::ClearAllHint()
{
	itHint it = m_HelpHints.begin();
	for(; it != m_HelpHints.end();it++)
	{
		tagHint &hint = (*it).second;
		//删除定时器
		if(hint.lTimerID != 0)
		{
			TIMER_CANCEL(hint.lTimerID);
		}
	}
	m_HelpHints.clear();

	it = m_InterfaceHints.begin();
	for(; it != m_InterfaceHints.end();it++)
	{
		tagHint &hint = (*it).second;
		//删除定时器
		if(hint.lTimerID != 0)
		{
			TIMER_CANCEL(hint.lTimerID);
		}
	}
	m_InterfaceHints.clear();
}

//! [2007-7-24 move] 添加队伍成员信息同步操作
void CPlayer::SetLevel(WORD l, BOOL bScriptSet)
{ 
	if(l <= 0 || l > PlayerList::GetLevelNum() )
	{
		char strGUID[50]="";
		m_guid.tostring(strGUID);
		Log4c::Warn(ROOT_MODULE,FormatText("GS_PLAYER_6",l,GetName(),strGUID));//设置玩家等级(%d)出错(Name:%s,GUID:%s)。
		return;
	}
	LONG lOldLevel = m_Property.wLevel;
	m_Property.wLevel = l;

	if(bScriptSet)
		GetLogicLogInterface()->logT100_player_level_log(this, lOldLevel, "脚本直接设置等级");
}

void  CPlayer::SetOccuLvl(eOccupation byOccu,BYTE l, BOOL bScriptSet)
{
	if(l <= 0 || l > PlayerList::GetOccuLevelNum(byOccu) ||
		byOccu < OCC_Novice || byOccu >= OCC_Max)
	{
		char strGUID[50]="";
		m_guid.tostring(strGUID);
		Log4c::Warn(ROOT_MODULE,FormatText("GS_PLAYER_7",byOccu,l,GetName(),strGUID));//设置玩家职业(%d)等级(%d)出错(Name:%s,GUID:%s)。
		return;
	}
	LONG lOldOccuLevel = m_Property.byOccuLvl[byOccu];
	m_Property.byOccuLvl[byOccu] = l;

	if(bScriptSet)
		GetLogicLogInterface()->logT101_player_Occulevel_log(this, lOldOccuLevel, "GS_PLAYER_8");//脚本直接设置等级
}

void CPlayer::SetHP(DWORD l)
{
	m_Property.dwHp = l;
	if(m_Property.dwHp > GetMaxHP())
	{
		m_Property.dwHp = GetMaxHP();
	}
	if (GetState()==STATE_DIED && l>0)
	{
		m_Property.dwHp = 0;
	}

	GSTeam *pTeam = (GSTeam*)GetOrganiCtrl()->GetGameTeamByID(GetTeamID());
	if(NULL != pTeam)
	{
		pTeam->OnPlayerIdioinfoChange(GetExID(), eMIS_CurrHp);
	}
}

void CPlayer::SetMP(WORD l)
{
	m_Property.wMp = l;
	if(m_Property.wMp > GetMaxMP()) 
	{
		m_Property.wMp = GetMaxMP();
	}
	GSTeam *pTeam = (GSTeam*)GetOrganiCtrl()->GetGameTeamByID(GetTeamID());
	if(NULL != pTeam)
	{
		pTeam->OnPlayerIdioinfoChange(GetExID(), eMIS_CurrMp);
	}
}

void CPlayer::SetEnergy(WORD l)
{
	m_Property.wEnergy = l;
	if(m_Property.wEnergy > GetMaxEnergy())
	{
		m_Property.wEnergy = GetMaxEnergy();
	}
}

void CPlayer::SetPKCP( long v )
{
	m_Property.lCurPKCP = v;
	long lMaxPKCP = GetMaxPKCP();
	if( m_Property.lCurPKCP > lMaxPKCP )
	{
		m_Property.lCurPKCP = lMaxPKCP;
	}

	GSTeam *pTeam = (GSTeam*)GetOrganiCtrl()->GetGameTeamByID(GetTeamID());
	if(NULL != pTeam)
	{
		pTeam->OnPlayerIdioinfoChange(GetExID(), eMIS_CurrEnergy);
	}
}

long CPlayer::GetMaxPKCP()
{
	return GetAttribute( "C_MaxPKCP" );
}

void CPlayer::SetBaseMaxPKCP( long v ) 
{ 
	m_BaseFightProperty.lMaxPKCP = v; 
	GSTeam *pTeam = (GSTeam*)GetOrganiCtrl()->GetGameTeamByID(GetTeamID());
	if(NULL != pTeam)
	{
		pTeam->OnPlayerIdioinfoChange(GetExID(), eMIS_MaxEnergy);
	}
}

//long CPlayer::GetAllEquipTimesCP()
//{
//	typedef map<eEquipType, CGoods*> EquipTableT;
//	EquipTableT *pEquipList = GetEquipmentGoodsMap();
//	long cpvalue = 0;
//	for( EquipTableT::iterator it = pEquipList->begin(); it != pEquipList->end(); ++ it )
//	{
//		CGoods *pGoods = it->second;
//		if( pGoods != NULL )
//		{
//			if( pGoods->HasAddonProperty( CGoodsBaseProperties::GAP_TIMES_MAX_PKCP ) )
//			{
//				cpvalue += pGoods->GetAddonPropertyValues( CGoodsBaseProperties::GAP_TIMES_MAX_PKCP, 1 );
//			}
//		}
//	}
//	return cpvalue;
//}

void CPlayer::SetMaxHP(DWORD l)
{
	m_mapDirtyAttrData.insert("wMaxHP");
	m_FightProperty.dwMaxHp = l; 
	if(GetHP()>GetMaxHP()) 
	{
		SetHP(GetMaxHP());
	}

	GSTeam *pTeam = (GSTeam*)GetOrganiCtrl()->GetGameTeamByID(GetTeamID());
	if(NULL != pTeam)
	{
		pTeam->OnPlayerIdioinfoChange(GetExID(), eMIS_MaxHp);
	}
}

void CPlayer::SetMaxMP(WORD l)
{
	m_mapDirtyAttrData.insert("wMaxMP");
	m_FightProperty.wMaxMp = l;
	if(GetMP()>GetMaxMP())
	{
		SetMP(GetMaxMP());
	}

	GSTeam *pTeam = (GSTeam*)GetOrganiCtrl()->GetGameTeamByID(GetTeamID());
	if(NULL != pTeam)
	{
		pTeam->OnPlayerIdioinfoChange(GetExID(), eMIS_MaxMp);
	}
}

void CPlayer::SetMaxEnergy(WORD l)
{
	m_mapDirtyAttrData.insert("wMaxEnergy");
	m_FightProperty.wMaxEnergy = l;
	if(GetEnergy()>GetMaxEnergy()) 
	{
		SetEnergy(GetMaxEnergy());
	}
}


void CPlayer::OnAddBuff(DWORD dwStateID, DWORD dwBuffLevel)
{
	CMoveShape::OnAddBuff(dwStateID, dwBuffLevel);
	GSTeam *pTeam = GetOrganiCtrl()->GetGameTeamByID(GetTeamID());
	if(NULL != pTeam)
	{
		pTeam->SendMemberBuffToAll(this, dwStateID, dwBuffLevel, FALSE);
	}
}

void CPlayer::OnDelBuff(DWORD dwStateID, DWORD dwBuffLevel)
{
	CMoveShape::OnDelBuff(dwStateID, dwBuffLevel);
	GSTeam *pTeam = GetOrganiCtrl()->GetGameTeamByID(GetTeamID());
	if(NULL != pTeam)
	{
		pTeam->SendMemberBuffToAll(this, dwStateID, dwBuffLevel, TRUE);
	}
}

void CPlayer::SetRecruitState(bool bIsRecruiting, char* pRecruitName, char* pPassword)
{
	m_RecruitState.bIsRecruiting = bIsRecruiting;

	CMessage msg(MSG_S2C_TEAM_RECRUIT);


	do 
	{
		msg.Add(GetExID());
		msg.Add((long)((bIsRecruiting)? 1 : 0));
		if (!bIsRecruiting)
		{
			break;
		}

		if(NULL != pRecruitName)
		{
			long lStrSize = (long)strlen(pRecruitName);
			lStrSize = (lStrSize < MAX_RECRUIT_TEXT_SIZE ) ? lStrSize : MAX_RECRUIT_TEXT_SIZE - 1;
			memmove(m_RecruitState.szRecruitName, pRecruitName, MAX_RECRUIT_TEXT_SIZE - 1);

			msg.Add(m_RecruitState.szRecruitName);
		}
		else
		{
			m_RecruitState.szRecruitName[0] = 0;
		}

		long lUsePassword = 0;

		if (NULL != pPassword)
		{
			long lStrSize = (long)strlen(pPassword);
			lStrSize = (lStrSize < MAX_RECRUIT_PWD_SIZE) ? lStrSize : MAX_RECRUIT_PWD_SIZE - 1;
			memmove(m_RecruitState.szPassword, pPassword, MAX_RECRUIT_PWD_SIZE - 1);
			if(0 != strlen(m_RecruitState.szPassword))
			{
				lUsePassword = 1;
			}
		}
		else
		{
			m_RecruitState.szPassword[0] = 0;
		}

		msg.Add(lUsePassword);

	} while(false);


	msg.SendToRegion( GetGame()->FindRegion(GetRegionGUID()) );

}

tagRecruitState& CPlayer::GetRecruitState(void)
{
	return m_RecruitState;
}
// ------------------------------------------------------------
// 设置接口
// ------------------------------------------------------------
//体力上限
void CPlayer::SetMaxStamina(WORD l)			
{
	m_mapDirtyAttrData.insert("C_MaxStamina"); 
	m_FightProperty.wMaxStamina = l;
	if(GetStamina()>GetMaxStamina()) SetStamian(GetMaxStamina());
}


//最小攻击力
void CPlayer::SetMinAtk(WORD l)				
{
	m_mapDirtyAttrData.insert("C_MinAtk"); 
	m_FightProperty.wMinAtk = l;
}
//最大攻击力
void CPlayer::SetMaxAtk(WORD l)	
{
	m_mapDirtyAttrData.insert("C_MaxAtk"); 
	m_FightProperty.wMaxAtk = l;
}
//命中能力
void CPlayer::SetHit(WORD l)
{
	m_mapDirtyAttrData.insert("C_Hit"); 
	m_FightProperty.wHit = l;
}
//物理防御
void CPlayer::SetDef(WORD l)			
{
	m_mapDirtyAttrData.insert("C_Def"); 
	m_FightProperty.wDef = l;
}
//闪避
void CPlayer::SetDodge(WORD l)		
{
	m_mapDirtyAttrData.insert("C_Dodge");
	m_FightProperty.wDodge = l;
}
//元素抗性
void CPlayer::SetElementResistant(long lpos,WORD l)	
{
	string name;
	switch(lpos)
	{
	case ET_Fire:
		name = "E_ElemDef0";
		break;
	case ET_Water:
		name = "E_ElemDef1";
		break;
	case ET_Earth:
		name = "E_ElemDef2";
		break;
	case ET_Wind:
		name = "E_ElemDef3";
		break;
	case ET_Dark:
		name = "E_ElemDef4";
		break;
	case ET_Holy:
		name = "E_ElemDef5";
		break;
	}
	m_mapDirtyAttrData.insert(name);
	m_FightProperty.wElemDef[lpos] = l;
}
//生命恢复速度
void CPlayer::SetHpRecoverSpeed(WORD l)	
{
	m_mapDirtyAttrData.insert("C_HpRecoverSpeed"); 
	m_FightProperty.wHpRecoverSpeed = l;
}
//法力恢复速度
void CPlayer::SetMpRecoverSpeed(WORD l)
{
	m_mapDirtyAttrData.insert("C_MpRecoverSpeed");
	m_FightProperty.wMpRecoverSpeed = l;
}
//体力恢复速度
void CPlayer::SetStamRecoverSpeed(WORD l) 
{
	m_mapDirtyAttrData.insert("C_MaxHP"); 
	m_FightProperty.wStamRecoverSpeed = l;
}
//力量
void CPlayer::SetStr(short s)
{
    m_mapDirtyAttrData.insert("C_Str");
    m_FightProperty.wStr = s;
}
//身法
void CPlayer::SetDex(short s)
{
    m_mapDirtyAttrData.insert("C_Dex");
    m_FightProperty.wDex = s;
}
//根骨
void CPlayer::SetCon(short s)
{
    m_mapDirtyAttrData.insert("C_Con");
    m_FightProperty.wCon = s;
}
//意志
void CPlayer::SetIntellect(short s)
{
    m_mapDirtyAttrData.insert("C_Intellect");
    m_FightProperty.wIntellect = s;
}
//灵性
void CPlayer::SetSpiritualism(short s)
{
    m_mapDirtyAttrData.insert("C_Spiritualism");
    m_FightProperty.wSpiritualism = s;
}
//潜能点
void CPlayer::SetPoint(short s)
{
    m_mapDirtyAttrData.insert("C_Point");
    m_FightProperty.wPoint = s; 
}

//格挡
void CPlayer::SetFightBlock(WORD l)	
{
	m_mapDirtyAttrData.insert("C_Block"); 
	m_FightProperty.wFightBlock = l;
}
//招架
void CPlayer::SetParry(WORD l)	
{
	m_mapDirtyAttrData.insert("C_Parry");
	m_FightProperty.wParry = l;
}
//魅力
void CPlayer::SetCharm(WORD l)	
{
	m_mapDirtyAttrData.insert("C_Charm"); 
	m_FightProperty.wCharm = l;
}
//专注
void CPlayer::SetAbsorb(WORD l)	
{
	m_mapDirtyAttrData.insert("C_Absorb");
	m_FightProperty.wAbsorb = l;
}
//魔法抗性
void CPlayer::SetMdef(WORD l)		
{
	m_mapDirtyAttrData.insert("C_Mdef"); 
	m_FightProperty.wMdef = l;
}
//物理暴击
void CPlayer::SetCri(WORD l)	
{
	m_mapDirtyAttrData.insert("C_Cri"); 
	m_FightProperty.wCri = l;
}
//施法速度
void CPlayer::SetMAtSpeed(WORD l)	
{
	m_mapDirtyAttrData.insert("C_MAtkSpeed");
	m_FightProperty.wMAtkSpeed = l;
}
//Buff攻击强度
void CPlayer::SetBuffPower(WORD l)	
{
	m_mapDirtyAttrData.insert("C_BuffPower"); 
	m_FightProperty.wBuffPower = l;
}
//魔法攻击力
void CPlayer::SetMAtk(WORD l)	
{
	m_mapDirtyAttrData.insert("C_MAtk"); 
	m_FightProperty.wMAtk = l;
}
//魔法爆击率
void CPlayer::SetMCri(WORD l)		
{
	m_mapDirtyAttrData.insert("C_MCri"); 
	m_FightProperty.wMCri = l;
}
//生命吸收率
void CPlayer::SetBloodSuk(WORD l)	
{
	m_mapDirtyAttrData.insert("C_BloodSuck"); 
	m_FightProperty.wBloodSuck = l;
}
//法力吸收率
void CPlayer::SetManaSuck(WORD l)	
{
	m_mapDirtyAttrData.insert("C_ManaSuck");
	m_FightProperty.wManaSuck = l;
}
//意志抗性
void CPlayer::SetWillDef(WORD l)	
{
	m_mapDirtyAttrData.insert("C_WillDef"); 
	m_FightProperty.wWillDef = l;
}
//体质抗性
void CPlayer::SetConstiDef(WORD l)
{
	m_mapDirtyAttrData.insert("C_ConstiDef");
	m_FightProperty.wConstiDef = l;
}
//Debuff抗性
void CPlayer::SetDefBuffDef(long lpos,WORD l)			
{
	string name;
	switch(lpos)
	{
	case DFT_Bled:
		name = "E_DeBuffDef0";
		break;
	case DET_Dizzy:
		name = "E_DeBuffDef1";
		break;
	case DET_Lull:
		name = "E_DeBuffDef2";
		break;
	case DET_Hypnosis:
		name = "E_DeBuffDef3";
		break;
	case DET_Poison:
		name = "E_DeBuffDef4";
		break;
	case DET_Fix:
		name = "E_DeBuffDef5";
		break;
	case DET_Silence:
		name = "E_DeBuffDef6";
		break;
	}
	m_mapDirtyAttrData.insert(name);
	m_FightProperty.wDeBuffDef[lpos] = l;
}
//对各个种族伤害
void CPlayer::SetClassDam(long lpos,WORD l)			
{
	string name;
	switch(lpos)
	{
	case C_Human:
		name = "E_ClassDam0";
		break;
	case C_Element:
		name = "E_ClassDam1";
		break;
	case C_Alien:
		name = "E_ClassDam2";
		break;
	case C_Animal:
		name = "E_ClassDam3";
		break;
	case C_Undead:
		name = "E_ClassDam4";
		break;
	case C_Evil:
		name = "E_ClassDam5";
		break;
	case C_Hexapods:
		name = "E_ClassDam6";
		break;
	}
	m_mapDirtyAttrData.insert(name); 
	m_FightProperty.wClassDam[lpos] = l;
}
void CPlayer::SetElemDam(long lpos,WORD l)	
{
	string name;
	switch(lpos)
	{
	case ET_Fire:
		name = "E_ElemDam0";
		break;
	case ET_Water:
		name = "E_ElemDam1";
		break;
	case ET_Earth:
		name = "E_ElemDam2";
		break;
	case ET_Wind:
		name = "E_ElemDam3";
		break;
	case ET_Dark:
		name = "E_ElemDam4";
		break;
	case ET_Holy:
		name = "E_ElemDam5";
		break;
	}
	m_mapDirtyAttrData.insert(name);
	m_FightProperty.wElemDam[lpos] = l;
}
//战斗时生命恢复速度
void CPlayer::SetFightHPRecoverSpeed(WORD l)	
{
	m_mapDirtyAttrData.insert("C_FightHPRecoverSpeed"); 
	m_FightProperty.wFightHpRecoverSpeed = l;
}
//战斗时法力恢复速度
void CPlayer::SetFightMPRecoverSpeed(WORD l)
{
	m_mapDirtyAttrData.insert("C_FightMPRecoverSpeed"); 
	m_FightProperty.wFightMpRecoverSpeed = l;
}
//能量恢复速度
void CPlayer::SetEnterRecoverSpeed(WORD l)		
{
	m_mapDirtyAttrData.insert("C_EnterRecoverSpeed"); 
	m_FightProperty.wEnerRecoverSpeed = (short)l;
}
//战斗能量恢复速度
void CPlayer::SetFightEnterRecoverSpeed(WORD l)	
{
	m_mapDirtyAttrData.insert("C_FightEnterRecoverSpeed"); 
	m_FightProperty.wFightEnerRecoverSpeed = (short)l;
}
//能量吸收率
void CPlayer::SetEnergySuck(float f)					
{
	m_mapDirtyAttrData.insert("C_EnergySuck");
	m_FightProperty.fEnergySuck = f;
}
//物理爆击抗性
void CPlayer::SetCriDef(WORD l)	
{
	m_mapDirtyAttrData.insert("C_CriDef");
	m_FightProperty.wCriDef = l;
}
//魔法爆击抗性
void CPlayer::SetMCriDef(WORD l)	
{
	m_mapDirtyAttrData.insert("C_MCriDef");
	m_FightProperty.wMCriDef = l;
}

long CPlayer::OnTimeOut(DWORD timerid,DWORD curtime,const void* var)
{
	DWORD dwStartTime = timeGetTime();
	const tagTimerVar* pTimerVar = (const tagTimerVar*)var;
	long lTimerType =pTimerVar ->lvar1<<16 | pTimerVar->TimerType;
#ifdef _RUNSTACKINFO3_
	char pszStatckInfo[256]="";
	_snprintf(pszStatckInfo,256,"CPlayer::OnTimeOut(TimerType:%d,Name:%s) Start",pTimerVar?pTimerVar->TimerType:0,GetName());
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStatckInfo);
#endif

    // player move
    if( pTimerVar->TimerType == ChildObjType_ShapeMove )
    {
        m_mover.OnTimeOut();
    }
    else if (ChildObjType_GoodsTimeout == pTimerVar->TimerType)
	{    //! 限时物品处理
		OnLimitGoodsTimeout(timerid);
		/*GameManager::GetInstance()->FreeBlock((void*)var);*/
	}
    else if (ChildObjType_Budhism == pTimerVar->TimerType)
    {
        if( !static_cast<StateAI::PlayerAI*>(m_pAI)->IsInSkillActTime() && !m_Buddhism.IsEmpty() )
        {
            m_Buddhism.UpdateDistract( -1*GlobalSetup::GetSetup()->lDistractValue );
            /// 添加杂念值
            AddAttributeToDirty("lDistract");
            UpdateAttribute(1);
#ifdef  _DEBUG
            char  szText[256];
            _snprintf(szText,256,"[Colour 'ffffffff'] 杂念值=%d,燥的数量=%d,结束时间=%d",m_Buddhism.lDistract,m_Buddhism.lDryNum,m_Buddhism.lDryEndTime);
            SendNotifyMessage( szText, eNOTIFYPOS_LEFT_BOTTOM );
#endif
        }
    }
    else if (ChildObjType_ClearDry == pTimerVar->TimerType )
    {
        m_Buddhism.ClearDryNum();
        /// 添加杂念值
        AddAttributeToDirty("lDryNum");
        UpdateAttribute(1);
#ifdef  _DEBUG
        char  szText[256];
        _snprintf(szText,256,"[Colour 'ffffffff'] 杂念值=%d,燥的数量=%d,结束时间=%d",m_Buddhism.lDistract,m_Buddhism.lDryNum,m_Buddhism.lDryEndTime);
        SendNotifyMessage( szText,eNOTIFYPOS_LEFT_BOTTOM );
#endif
    }
	else if( pTimerVar->TimerType == ChildObjType_GoodsExTimeOut )
	{
		long lSubType = pTimerVar->lvar;
		long lTriger = pTimerVar->lvar1;
		if( lSubType == GoodsExTimerType::GETT_GUARDIAN_ENERGY )
		{
	//		m_GoodsTimerHandler.OnTimeOut( this, GoodsTimerHandler::TimerKey( lTriger ), 
	//			&CPlayer::OnGuardianEnergyTimeOut );
		}
		// add more goods timer handle...
	}
	//战斗状态时间
	else if(timerid == m_lFightStateTimerID)
	{
		//EnterPeaceState();
		m_lFightStateTimerID = -1;
	}
	// queue time
	else if (ChildObjType_CreateRgnQueue == pTimerVar->TimerType)
	{
		OnQueueTimeout(timerid);
	}

	// prgn del time
	else if (ChildObjType_CreatePRgn == pTimerVar->TimerType)
	{
		OnCreatePRgnTimeout(timerid);
	}
	// trgn del time
	else if (ChildObjType_CreateTRgn == pTimerVar->TimerType)
	{
		OnCreateTRgnTimeout(timerid);
	}

	else if(ChildObjType_InvestLimit==pTimerVar->TimerType)
	{
		//清除贸易点投资记录
		ClearInvestRecord();
	}

	else if(ChildObjType_PlayerHint == pTimerVar->TimerType)
	{
		OnHintTimeOut(pTimerVar->lvar);
	}

	else if (pTimerVar->TimerType == ChildObjType_PKValue)
	{
		OnDecreasePKValue();
	}
	else if (pTimerVar->TimerType == ChildObjType_PvPValue)
	{
		OnDecreasePvPValue();
	}
	//刷新俑兵任务
	else if(pTimerVar->TimerType == ChildObjType_UpdateMercQuest)
	{
		//OnUpdateMercQuestTimeOut();
		return 0;
	}
	//自动恢复Hp,Mp
	else if (pTimerVar->TimerType == ChildObjType_AutoRecoverHpMp)
	{
		OnIncreaseHpMpValue();
	}
	//else if (pTimerVar->TimerType == ChildObjType_AutoRecoverEnergy)
	//{
	//	OnUpdateEnergy();
	//}
	else if (pTimerVar->TimerType == ChildObjType_RgnKickPlayerTime)
	{
		OnRgnKickPlayerTimeout(timerid);
	}
	else if(pTimerVar->TimerType == ChildObjType_Fcm_KickPlayerTime)
	{
		GetGame()->KickPlayer(GetExID());
	}
	else if(pTimerVar->TimerType == ChildObjType_ReliveTimer)
	{
		OnRelive(0);
	}
	//else if (pTimerVar->TimerType == ChildObjType_Mail)
	//{
	//	OnMailTimeOut(pTimerVar);
	//}
	else
	{
		CMoveShape::OnTimeOut(timerid,curtime,var);
	}

	GetGame()->AddTimerExpendTime(lTimerType,timeGetTime()-dwStartTime);
#ifdef _RUNSTACKINFO3_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CPlayer::OnTimeOut() End");
#endif
	return 0;
}
//邮件超时处理
//void 	CPlayer::OnMailTimeOut(const tagTimerVar* pTimerVar)
//{
//
//	CMail::tagMail *ptgMail  = (CMail::tagMail*)(pTimerVar->pvar);
//	CMail *pMail =  GetMail(ptgMail->MailGuid); //(CMail*)(pTimerVar->pvar)
//	if (pMail)
//	{
//		GameManager::GetInstance()->GetMailManager()->OnMailTimerOut(pMail,this);
//	}
//
//}
//long CMoveShape::Proc(eEventType type,const tagProcVar *pVar)
//{
//	switch(pVar->lType)
//	{
//	case ChildObjType_SysMail:
//		{
//
//		}
//		break;
//	}
//	CShape::Proc(type,pVar);
//	return 1;
//}

const CGUID& CPlayer::GetPersonalRgnGUID(void)
{
	return m_PersonalRgnGUID;
}
const CGUID& CPlayer::GetPersonalHouseRgnGUID(void)
{
	return m_PersonalHouseRgnGUID;
}
void CPlayer::SetPersonalRgnGUID(const CGUID& guid)
{
	m_PersonalRgnGUID = guid;
}
void CPlayer::SetPersonalHouseRgnGUID(const CGUID& guid)
{
	m_PersonalHouseRgnGUID = guid;
}

//! 技能加成开始
long CPlayer::SkillReinforceBegin(bool isMagic, bool isAttack)
{
	++m_EffectIdSeed;
	CGoods *pArm = m_cEquipment.GetGoods(EC_E_WEAPON); 
	if( NULL != pArm)
	{
		pair<float, float> ReturnReinforce;
		GoodsExManage::GetInstance().ArmReinforceBegin(this, pArm, isMagic, isAttack, ReturnReinforce);
		m_mapReturnReinforce[m_EffectIdSeed] = ReturnReinforce;
	}
	return m_EffectIdSeed;
}

//! 技能加成结束
void CPlayer::SkillReinforceEnd(long lKey)
{
	m_mapReturnReinforce.erase(lKey);
}

//! 获得技能加成值
const pair<float, float>& CPlayer::GetSkillReinforceAmend(long lKey)
{
	map<long, pair<float, float>>::iterator ite = m_mapReturnReinforce.find(lKey);
	if (m_mapReturnReinforce.end() != ite)
	{
		return ite->second;
	}

	return m_FixedReinforce;
}

//! 获得武器当前能量
long CPlayer::GetArmCurrPower(void)
{
	CGoods *pArm = m_cEquipment.GetGoods(EC_E_WEAPON); 
	if(NULL != pArm && pArm->HasAddonProperty(GAP_CURR_POWER))
		return pArm->GetAddonPropertyValues(GAP_CURR_POWER, 1);
	return -1;
}

//! 返回物品当前能量与最大值的比例（用百分数分子表示）
long CPlayer::GetArmCurrPowerSacle(void)
{
	CGoods *pArm = m_cEquipment.GetGoods(EC_E_WEAPON); 
	if(NULL != pArm && pArm->HasAddonProperty(GAP_CURR_POWER))
		return pArm->GetAddonPropertyValues(GAP_CURR_POWER, 1) * 100 / pArm->GetAddonPropertyValues(GAP_MAX_POWER, 1);
	return 100;
}

//! 根据输入的条件得到一个配置的加成值
float CPlayer::GetReinforceByEnum(long eValuePos, BOOL isMagic)
{
	long lAimLevel = 0;
	long lPowerIntensity = 0; 
	CGoods *pArm = m_cEquipment.GetGoods(EC_E_WEAPON);
	if(NULL != pArm && pArm->HasAddonProperty(GAP_CURR_POWER))
	{
		if (0 != GoodsExManage::GetInstance().CanExpendPower(pArm, isMagic))
		{
			lAimLevel = pArm->GetAddonPropertyValues(GAP_WEAPON_DAMAGE_LEVEL, 1);
			lPowerIntensity = pArm->GetAddonPropertyValues(GAP_POWER_INTENSITY, 1);
			return GoodsExManage::GetInstance().GetReinforceByEnum(lAimLevel, lPowerIntensity, eValuePos);
		}
	}
	return 1.0f;
}
//插入系统邮件列表
void CPlayer::InsertSysMail(long lID,long lEndTime)
{

	map<long,long>::iterator it=m_mapSysMail.find(lID);
	if (it==m_mapSysMail.end() && lEndTime>0)
	{
		m_mapSysMail[lID] = lEndTime;
		GetGame()->UpdateSavePlayer(this, SAVE_DETAIL_ATTRPROPERTY);
	}
}
//检查系统邮件列表
void CPlayer::CheckSysMailTime()
{
	time_t t_CurrentTime = time(NULL);
	map<long,long>::iterator it=m_mapSysMail.begin();
	for (;it!=m_mapSysMail.end();)
	{
		if (t_CurrentTime>(*it).second && (*it).second!=1 )
		{
			it=m_mapSysMail.erase(it);
		}
		else
		{
			++it;
		}
	}

}
bool	CPlayer::AddReceiveMailList(CGUID &MailGuid)
{
	list<CGUID>::iterator it = find(m_lReceiveMail.begin(),m_lReceiveMail.end(),MailGuid);
	if (it == m_lReceiveMail.end())
	{
		m_lReceiveMail.push_back(MailGuid);
		return true;
	}
	return false;
}
void	CPlayer::DeleteReceiveMailList(CGUID &MailGuid)
{
	list<CGUID>::iterator it = find(m_lReceiveMail.begin(),m_lReceiveMail.end(),MailGuid);
	if (it != m_lReceiveMail.end())
	{
		m_lReceiveMail.erase(it);
	}
}

//检查有没有接收过lSysID系统邮件
bool	CPlayer::CheckSysMail(long lSysID)
{
	map<long,long>::iterator it=m_mapSysMail.find(lSysID);
	if(it!=m_mapSysMail.end())
	{
		return true;
	}
	return false;
}

//! 扣除一次消耗的武器的能量
bool CPlayer::ExpendPower(BOOL isMagic)
{
	CGoods *pArm = m_cEquipment.GetGoods(EC_E_WEAPON);
	if(NULL == pArm) return true;
	long lUsePower = GoodsExManage::GetInstance().GetExpendPower(pArm, isMagic);
	if(0 >= lUsePower) return false;
	long lCurrPower = pArm->GetAddonPropertyValues(GAP_CURR_POWER, 1);
	lCurrPower -= lUsePower;
	lCurrPower = (0 > lCurrPower) ? 0 : lCurrPower;
	pArm->ChangeAttribute(GAP_CURR_POWER, lCurrPower, 1, true);
	CMessage msg(MSG_S2C_GOODS_POWER_WARN);
	msg.Add(GetExID());
	msg.Add(pArm->GetAddonPropertyValues(GAP_POWER_INTENSITY, 1));
	msg.Add(lCurrPower);
	msg.SendToAround(this);
	return true;
}

//!	注册一个排队定时器，并返回定时器ID
long CPlayer::RegisterQueueTimerID(long lFutureTime)
{
	//! m_PlayerTimer对象会自动释放pVar
	void* pVar = OBJ_CREATE(tagTimerVar);
	//void* pVar = GameManager::GetInstance()->CreateBlock(sizeof(tagTimerVar));
	/*assert(NULL != pVar);*/
	((tagTimerVar*)pVar)->TimerType = ChildObjType_CreateRgnQueue;

	long lTimerID = GameManager::GetInstance()->GetRgnManager()->GetQueueTimer()->Schedule(this, pVar, lFutureTime + timeGetTime());
	m_lQueueTimerID = lTimerID;
	return lTimerID;
}
//!	取消一个排队的Timer
void CPlayer::CancelQueueTimer(void)
{
#ifdef _RUNSTACKINFO1_
	char pszStatckInfo[1024]="";
	_snprintf(pszStatckInfo,1024,"CPlayer::CancelQueueTimer(TimerID:%d)",m_lQueueTimerID);
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStatckInfo);
#endif
	void* pVar = NULL;
	GameManager::GetInstance()->GetRgnManager()->GetQueueTimer()->Cancel(m_lQueueTimerID, (const void**)&pVar);
}
//!	响应排队定时器触发的事件
void CPlayer::OnQueueTimeout(long lTimerID)
{
}

//!	注册一个创建P副本定时器，并返回定时器ID
long CPlayer::RegisterCreatePRgnTimerID(long lFutureTime)
{
	// 定时器已存在
	if(-1 != m_lPRgnTimerID) return m_lPRgnTimerID;
	//! m_PlayerTimer对象会自动释放pVar
	void* pVar = OBJ_CREATE(tagTimerVar);
	//void* pVar = GameManager::GetInstance()->CreateBlock(sizeof(tagTimerVar));
	/*assert(NULL != pVar);*/
	((tagTimerVar*)pVar)->TimerType = ChildObjType_CreatePRgn;
	m_lPRgnTimerID = GameManager::GetInstance()->GetRgnManager()->GetCreatePRgnTimer()->Schedule(this, pVar, lFutureTime + timeGetTime(), lFutureTime);
	return m_lPRgnTimerID;
}
//!	取消一个创建P副本的Timer
void CPlayer::CancelCreatePRgnTimer(void)
{
#ifdef _RUNSTACKINFO1_
	char pszStatckInfo[1024]="";
	_snprintf(pszStatckInfo,1024,"CPlayer::CancelCreatePRgnTimer(TimerID:%d)",m_lPRgnTimerID);
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStatckInfo);
#endif
	void* pVar = NULL;
	GameManager::GetInstance()->GetRgnManager()->GetCreatePRgnTimer()->Cancel(m_lPRgnTimerID, (const void**)&pVar);
	m_lPRgnTimerID = -1;
}
//!	响应创建P副本定时器触发的事件
void CPlayer::OnCreatePRgnTimeout(long lTimerID)
{
	m_CurPRgnNum--;
	if(m_CurPRgnNum == 0)
		CancelCreatePRgnTimer();
}

void CPlayer::ClearIncShopCur10()
{
	IncShopCur10Iter it=m_mapIncShopCur10.begin();
	for(;it!=m_mapIncShopCur10.end();it++)
	{
		it->second.clear();
	}
}

// 增值商店相关数据
void CPlayer::CodeIncShopToDataBlock(DBWriteSet& setWriteDB)
{
    size_t nSize=m_mapIncShopCur10.size();
    setWriteDB.AddToByteArray(static_cast<DWORD>(nSize));
    IncShopCur10Iter IncShopCurIt=m_mapIncShopCur10.begin();
    for(;IncShopCurIt!=m_mapIncShopCur10.end();IncShopCurIt++)
    {
        setWriteDB.AddToByteArray(static_cast<DWORD>(IncShopCurIt->second.size()));
        list<DWORD>::iterator goodsIdIt=IncShopCurIt->second.begin();
        for(;goodsIdIt!=IncShopCurIt->second.end();goodsIdIt++)
        {
            setWriteDB.AddToByteArray(*goodsIdIt);
        }
    }

    //增值商店限量购买记录
    setWriteDB.AddToByteArray((DWORD)m_mapLimitGoodsRecord.size());
    map<DWORD,DWORD>::iterator buyRecordIt=m_mapLimitGoodsRecord.begin();
    for(;buyRecordIt!=m_mapLimitGoodsRecord.end();buyRecordIt++)
    {
        setWriteDB.AddToByteArray(buyRecordIt->first);
        setWriteDB.AddToByteArray(buyRecordIt->second);
    }

    //增值商店交易全记录
    setWriteDB.AddToByteArray( (DWORD)m_vecTradeList.size() );
    CIncrementShopList::ITR_BUYLIST buyListIt = m_vecTradeList.begin();
    for ( ; buyListIt != m_vecTradeList.end(); ++buyListIt )
    {
        setWriteDB.AddToByteArray( buyListIt->strBuyTime.c_str() );
        setWriteDB.AddToByteArray( buyListIt->dwPriceNum );
        setWriteDB.AddToByteArray( buyListIt->dwPriceType );
        setWriteDB.AddToByteArray( buyListIt->dwGoodsIdx );
        setWriteDB.AddToByteArray( buyListIt->dwBuyNum );  
    }
}

void CPlayer::DecodeIncShopFromDataBlock(DBReadSet& setReadDB)
{
    char strTemp[256];
    DWORD dwSize = setReadDB.GetDwordFromByteArray();
#ifdef _RUNSTACKINFO1_
    char pszStatckInfo[1024]="";
    _snprintf(pszStatckInfo,1024,"Decode ShopCur10(Size:%u) start.",dwSize);
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStatckInfo);
#endif
    ClearIncShopCur10();
    IncShopCur10Iter IncShopIt=m_mapIncShopCur10.begin();
    for(;IncShopIt!=m_mapIncShopCur10.end();IncShopIt++)
    {
        DWORD dwRecordSize=setReadDB.GetDwordFromByteArray();
        DWORD dwId=0;
        for (DWORD i=0;i<dwRecordSize;i++)
        {
            dwId=setReadDB.GetDwordFromByteArray();
            IncShopIt->second.push_back(dwId);
        }
    }

    //增值商店限量购买记录
    m_mapLimitGoodsRecord.clear();
    DWORD dwBuyRecordSize=setReadDB.GetDwordFromByteArray();
    for(DWORD i=0;i<dwBuyRecordSize;i++)
    {
        DWORD dwBuyGoodsId=setReadDB.GetDwordFromByteArray();
        DWORD dwBuyGoodsNum=setReadDB.GetDwordFromByteArray();
        m_mapLimitGoodsRecord[dwBuyGoodsId]=dwBuyGoodsNum;
    }

    //增值商店交易全记录
    m_vecTradeList.clear();
    DWORD dwTradeListNum = setReadDB.GetDwordFromByteArray();
    for ( DWORD i = 0; i < dwTradeListNum; ++i )
    {
        setReadDB.GetStringFromByteArray( strTemp, 256 );
        DWORD dwPriceNum = setReadDB.GetDwordFromByteArray();
        DWORD dwPriceType = setReadDB.GetDwordFromByteArray();
        DWORD dwGoodsIdx = setReadDB.GetDwordFromByteArray();
        DWORD dwBuyNum = setReadDB.GetDwordFromByteArray();
        CIncrementShopList::IncShopTradeLog stIncTradeElem( strTemp, dwPriceNum, dwPriceType, dwGoodsIdx, dwBuyNum );
        m_vecTradeList.push_back( stIncTradeElem );
    }
}

// 添加到最近购买列表中
void CPlayer::Add2IncShopCur10( DWORD dwMode,DWORD dwGoodsID )
{
	IncShopCur10Iter it=m_mapIncShopCur10.find(dwMode);
	if(it==m_mapIncShopCur10.end())
		return;
	list<DWORD>::iterator idIt=it->second.begin();
	for(;idIt!=it->second.end();idIt++)
	{
		if(*idIt==dwGoodsID)
			break;
	}
	if(idIt==it->second.end())
		it->second.push_front(dwGoodsID);
	while (it->second.size()>10)
	{
		it->second.pop_back();
	}
	// 更新玩家属性
	GetGame()->UpdateSavePlayer(this, SAVE_DETAIL_ATTRPROPERTY);
	}

//添加到个人购买详细记录列表中
void CPlayer::Add2IncTradeList( DWORD dwMode, DWORD dwGoodsId, DWORD dwGoodsNum, long lTotalAmount )
	{
	char strBuyTime[64] = "";

	SYSTEMTIME	stTime;
	GetLocalTime(&stTime);
	_snprintf(strBuyTime,64, "%d.%d.%d  %d:%d:%d" ,
		stTime.wYear,stTime.wMonth, stTime.wDay,
		stTime.wHour,stTime.wMinute,stTime.wSecond);
	CIncrementShopList::IncShopTradeLog stIncTradeElem( strBuyTime, lTotalAmount, dwMode, dwGoodsId, dwGoodsNum );
	m_vecTradeList.push_back( stIncTradeElem );

	// 更新玩家属性
	GetGame()->UpdateSavePlayer(this, SAVE_DETAIL_ATTRPROPERTY);
}

//!	注册一个副本清人定时器，并返回定时器ID
long CPlayer::RegisterRgnKickPlayerTimerID(long lFutureTime)
{
	// 定时器已存在
	if(-1 != m_lRgnKickPlayerTimerID) return m_lRgnKickPlayerTimerID;
	//! m_PlayerTimer对象会自动释放pVar
	void* pVar = OBJ_CREATE(tagTimerVar);
	((tagTimerVar*)pVar)->TimerType = ChildObjType_RgnKickPlayerTime;
	m_lRgnKickPlayerTimerID = GameManager::GetInstance()->GetRgnManager()->GetRgnKickPlayerTimer()->Schedule(this, pVar, lFutureTime + timeGetTime());
	return m_lRgnKickPlayerTimerID;
}

//!	注册一个防沉迷系统踢人的定时器，并返回定时器ID
long CPlayer::RegisterFcmKickPlayerTimerID(long lFutureTime)
{
	// 定时器已存在
	if(-1 != m_lRgnKickPlayerTimerID) return m_lRgnKickPlayerTimerID;
	//! m_PlayerTimer对象会自动释放pVar
	void* pVar = OBJ_CREATE(tagTimerVar);
	((tagTimerVar*)pVar)->TimerType = ChildObjType_Fcm_KickPlayerTime;
	m_lRgnKickPlayerTimerID = GameManager::GetInstance()->GetRgnManager()->GetRgnKickPlayerTimer()->Schedule(this, pVar, lFutureTime + timeGetTime());
	return m_lRgnKickPlayerTimerID;
}

//!	取消一个副本清人Timer
void CPlayer::CancelRgnKickPlayerTimer(void)
{
	void* pVar = NULL;
	GameManager::GetInstance()->GetRgnManager()->GetRgnKickPlayerTimer()->Cancel(m_lRgnKickPlayerTimerID, (const void**)&pVar);
	m_lRgnKickPlayerTimerID = -1;
}
//!	响应副本清人定时器触发的事件
void CPlayer::OnRgnKickPlayerTimeout(long lTimerID)
{
	// 取消该定时器
	CancelRgnKickPlayerTimer();

	// 将玩家放置到该副本场景的副本回城点
	CServerRegion* pRgn = GameManager::GetInstance()->GetRgnManager()->FindRgnByTemplateID(GetCurRgnType(), GetRegionID());
	if(pRgn)
	{
		CGUID retRgnGuid(NULL_GUID);
		long lRetRgnID = 0;
		long l, t, r, b, d;
		if(pRgn->GetRegionSetup()->bDupRgnIsUsed)
			pRgn->GetDupRgnReturnPoint(this, lRetRgnID, l, t, r, b, d);
		CServerRegion* pRetRgn = NULL;
		if(lRetRgnID > 0) // 普通场景返回
		{
			pRetRgn = GameManager::GetInstance()->GetRgnManager()->FindRgnByTemplateID(RGN_NORMAL, lRetRgnID);
		}
		else if(lRetRgnID == -1)// 返回国都
		{
			BYTE byCountry = GetCountry();
			lRetRgnID = GetCountryParam()->GetCountryMainRegionID(byCountry);
			pRetRgn = GameManager::GetInstance()->GetRgnManager()->FindRgnByTemplateID(RGN_NORMAL, lRetRgnID);
			long l = GetCountryParam()->GetCountryMainRectLeft(byCountry);
			long t = GetCountryParam()->GetCountryMainRectTop(byCountry);
			long r = GetCountryParam()->GetCountryMainRectRight(byCountry);
			long b = GetCountryParam()->GetCountryMainRectBottom(byCountry);

			if(pRetRgn)
			{				
				SetRegionID(lRetRgnID);
				SetRegionGUID(pRetRgn->GetExID());
				SetCurRgnType((eRgnType)RGN_NORMAL);
			}			
		}
		else if(lRetRgnID == 0) // 原场景返回
		{
			pRetRgn = pRgn;
		}

		if(pRetRgn) retRgnGuid = pRetRgn->GetExID();
		ChangeRegion(RGN_NORMAL, retRgnGuid, l, t, d, lRetRgnID);
	}
}
//!	注册一个创建T副本定时器，并返回定时器ID
long CPlayer::RegisterCreateTRgnTimerID(long lFutureTime)
{
	// 定时器已存在
	if(-1 != m_lTRgnTimerID) return m_lTRgnTimerID;
	//! m_PlayerTimer对象会自动释放pVar
	void* pVar = OBJ_CREATE(tagTimerVar);
	((tagTimerVar*)pVar)->TimerType = ChildObjType_CreateTRgn;
	m_lTRgnTimerID = GameManager::GetInstance()->GetRgnManager()->GetCreateTRgnTimer()->Schedule(this, pVar, lFutureTime + timeGetTime(), lFutureTime);
	return m_lTRgnTimerID;
}
//!	取消一个创建T副本的Timer
void CPlayer::CancelCreateTRgnTimer(void)
{
#ifdef _RUNSTACKINFO1_
	char pszStatckInfo[1024]="";
	_snprintf(pszStatckInfo,1024,"CPlayer::CancelCreateTRgnTimer(TimerID:%d)",m_lTRgnTimerID);
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStatckInfo);
#endif
	void* pVar = NULL;
	GameManager::GetInstance()->GetRgnManager()->GetCreateTRgnTimer()->Cancel(m_lTRgnTimerID, (const void**)&pVar);
	m_lTRgnTimerID = -1;
}
//!	响应创建T副本定时器触发的事件
void CPlayer::OnCreateTRgnTimeout(long lTimerID)
{
	m_CurTRgnNum--;
	if(m_CurTRgnNum == 0)
		CancelCreateTRgnTimer();
}

//能否采集
tagUseFailed CPlayer::CanDOccuCollect(CMoveShape* moveshape)
{
	if (moveshape==NULL)
	{
		return SKILL_USE_FAILED_C_NOCOLLECTION;//无采集对象
	}
	const CCollectionLayout* pCollectionLayout= CDeputyOccuSystem::GetCollectionLayout(moveshape->GetCollectionName().c_str());
	if (pCollectionLayout==NULL)
	{
		return SKILL_USE_FAILED_C_UNKNOW_COLLECTION;//未知采集物
	}
	//职业限制
	if (GetDOccupation()!= pCollectionLayout->GetOccuID())
	{
		return SKILL_USE_FAILED_C_DOCCU_ERROR;//职业错误
	}
	//采集等级限制
	if (GetCollectionGrade()< (long)pCollectionLayout->GetCollectLevel())
	{
		return SKILL_USE_FAILED_C_LEVEL_ERROR;//采集等级错误
	}
	//条件物品判断
	const list<tgConditionGoods> list_conditiongoods= pCollectionLayout->GetConditionList();
	for (list<tgConditionGoods>::const_iterator it= list_conditiongoods.begin(); 
		it!= list_conditiongoods.end(); it++)
	{
		//寻找条件物品
		int goodsnum=GetGoodsAmountByOriginName((*it).cGoodsName);
		if ((goodsnum< (*it).iNum && (*it).iNumType==0)
			|| (goodsnum!=(*it).iNum && (*it).iNumType==1)
			|| (goodsnum>= (*it).iNum && (*it).iNumType==2))
		{
			return SKILL_USE_FAILED_C_GOODSCONDITION_ERROR;//物品条件错误
		}
	}

	//背包空格判断
	//DWORD dwEmptyGrid=0;
	//dwEmptyGrid+=getOriginPack()->GetEmptySpaceNum();
	//for(int i=0;i<CSubpackContainer::SUBPACK_NUM;i++)
	//{
	//	CSubpackContainer::tagSubpack* pTagSubpack=GetPackage()->GetSubpack(i);
	//	if(pTagSubpack && pTagSubpack->pSubpackContainer)
	//	{
	//		dwEmptyGrid+=pTagSubpack->pSubpackContainer->GetEmptySpaceNum();
	//	}
	//}
	//if (dwEmptyGrid<=0)
	//{
	//	return SKILL_USE_FAILED_C_NO_BAGSPACE;//背包空间错误
	//}

	return SKILL_USE_NO_FAILED;
}

//采集
BOOL CPlayer::DOccuCollect(CMoveShape* moveshape)
{
	if (moveshape==NULL)
	{
		return FALSE;
	}
	const CCollectionLayout* pCollectionLayout= CDeputyOccuSystem::GetCollectionLayout(moveshape->GetCollectionName().c_str());
	if (pCollectionLayout==NULL)
	{
		return FALSE;
	}
	//增加经验
	DWORD exp=CDeputyOccuSystem::GetCollectionExp(GetCollectionGrade()-pCollectionLayout->GetCollectLevel());
	AddCollectionExp(exp);
	moveshape->BeenCollected();

	vector<CGoods*> GainList;

	//获得物品
	const list<tgOddsGoods>& list_goods=pCollectionLayout->GetGainedList();
	for (list<tgOddsGoods>::const_iterator it=list_goods.begin(); 
		it!= list_goods.end(); it++)
	{
		if (random(10000)< (*it).fOdds * 10000)
		{
			DWORD dwGoodsIndex=GoodsSetup::QueryGoodsIDByOriginalName((*it).cGoodsName);
			CGoodsFactory::CreateGoods( dwGoodsIndex, (*it).iGoodNum, GainList ,9);
		}
	}
	CTestContainer testContainer;
	testContainer.Init(this);
	vector<tagTestResult> vecResult;
	//循环获得品列表,如果不能添加到背包,则添加到场景
	CGUID guid = NULL_GUID;
	CGUID::CreateGUID(guid);
	for (vector<CGoods*>::iterator it=GainList.begin();it!=GainList.end();it++)
	{
		CGoods* pGoods=*it;
		if (pGoods)
		{
			BOOL bSucced=testContainer.AddTest( pGoods,&vecResult);
			if (bSucced)
			{
				GetLogicLogInterface()->logT055_collect(this,pGoods,guid,pGoods->GetAmount());
				if (vecResult.size()==1)
				{
					AddGoodsByTest(&vecResult[0],pGoods);
				}
			}
			else
			{
				GetLogicLogInterface()->logT055_collect(this,pGoods,guid,pGoods->GetAmount());
				SendNotifyMessage(AppFrame::GetText("1"));
				//添加到场景并设置保护时间
				CServerRegion* pRegion=dynamic_cast<CServerRegion*>(GetFather());
				if(pRegion)
				{
					long x=0, y=0;
					long lDropGoodsStartPos=0;

					lDropGoodsStartPos= pRegion->GetDropGoodsPos(GetTileX(), GetTileY(), x, y, lDropGoodsStartPos);
					if( lDropGoodsStartPos != -1 )
					{
						(pGoods)->SetPosXY(x+0.5f, y+0.5f);
						pRegion->AddObject( pGoods);				
						pRegion -> SetGoodsProtection( this, pGoods,FALSE);				
						// 通知每个玩家
						CS2CContainerObjectMove comMsg;
						comMsg.SetOperation( CS2CContainerObjectMove::OT_NEW_OBJECT );
						comMsg.SetDestinationContainer( pRegion -> GetType(), 
							pRegion -> GetExID(), y * pRegion -> GetWidth() + x );
						comMsg.SetDestinationObject(pGoods -> GetType(),pGoods-> GetExID() );
						comMsg.SetDestinationObjectAmount(pGoods-> GetAmount() );
						pGoods-> SerializeForOldClient( &( comMsg.GetObjectStream() ) );
						comMsg.SendToAround( this );
						comMsg.Send(GetExID());
					}
				}
			}
		}

	}
	return TRUE;
}
//是否能任务采集
tagUseFailed CPlayer::CanRoleCollect(CMoveShape* pMoveShape)
{
	if (pMoveShape==NULL)
	{
		return SKILL_USE_FAILED_C_NOCOLLECTION;//无采集对象
	}
	const CCollectionLayout* pCollectionLayout= CDeputyOccuSystem::GetCollectionLayout(pMoveShape->GetCollectionName().c_str());
	if (pCollectionLayout==NULL)
	{
		return SKILL_USE_FAILED_C_UNKNOW_COLLECTION;//未知采集物
	}
	//职业限制
	if ( DOccu_Other!= pCollectionLayout->GetOccuID())
	{
		return SKILL_USE_FAILED_C_NOT_ROLECOLLECTION;//不是任务采集物品 
	}

	if (pMoveShape->GetBeneficiaryID()!=NULL_GUID)
	{
		if(pMoveShape->GetBeneficiaryID()!=m_guid && pMoveShape->GetBeneficiaryID() != m_TeamID)
		{
			return SKILL_USE_FAILED_C_STATE_ERROR;//不能采集状态
		}
	}

	//采集任务
	//int RoleID= pCollectionLayout->GetRoleID();
	//	DWORD RoleSetp= pCollectionLayout->GetRoleStep();
	vector<tagQuestGoods>vecquestgoods=pCollectionLayout->GetRoleInfo();
	vector<tagQuestGoods>::iterator it=vecquestgoods.begin();
	BOOL bFind=false;
	for (;it!=vecquestgoods.end();it++)
	{
		//无任务条件
		if (( (*it).nQuestId ==-1 ) )
		{
			bFind = true;
			break;
		}
		else
		{
			if(FindRoleStep((*it).nQuestId,(*it).dwRoleStep))
			{
				bFind = true;
				break;	
			}
		}	
	}
	if (!bFind)
	{
		return SKILL_USE_FAILED_C_QUESTSTEP_ERROR;//任务id 和步骤不能对应
	}

	//条件物品判断
	const list<tgConditionGoods> list_conditiongoods= pCollectionLayout->GetConditionList();
	for (list<tgConditionGoods>::const_iterator it= list_conditiongoods.begin(); 
		it!= list_conditiongoods.end(); it++)
	{
		//寻找条件物品
		int goodsnum=GetGoodsAmountByOriginName((*it).cGoodsName);
		if ((goodsnum< (*it).iNum && (*it).iNumType==0)
			|| (goodsnum!=(*it).iNum && (*it).iNumType==1)
			|| (goodsnum>=(*it).iNum && (*it).iNumType==2))
		{
			return SKILL_USE_FAILED_C_GOODSCONDITION_ERROR;//物品条件
		}
	}

	//背包空格判断
	//DWORD dwEmptyGrid=0;
	//dwEmptyGrid+=getOriginPack()->GetEmptySpaceNum();
	//for(int i=0;i<CSubpackContainer::SUBPACK_NUM;i++)
	//{
	//	CSubpackContainer::tagSubpack* pTagSubpack=GetPackage()->GetSubpack(i);
	//	if(pTagSubpack && pTagSubpack->pSubpackContainer)
	//	{
	//		dwEmptyGrid+=pTagSubpack->pSubpackContainer->GetEmptySpaceNum();
	//	}
	//}
	//if (dwEmptyGrid<=0)
	//{
	//	return SKILL_USE_FAILED_C_NO_BAGSPACE;//背包空间不满足
	//}

	return SKILL_USE_NO_FAILED;//无失败记录
}
//任务采集
BOOL CPlayer::RoleCollect(CMoveShape* pMoveShape)
{
	if (pMoveShape==NULL)
	{
		return FALSE;
	}
	const CCollectionLayout* pCollectionLayout= CDeputyOccuSystem::GetCollectionLayout(pMoveShape->GetCollectionName().c_str());
	if (pCollectionLayout==NULL)
	{
		return FALSE;
	}
	vector<CGoods*> GainList;
	//获得物品
	const list<tgOddsGoods>& list_goods=pCollectionLayout->GetGainedList();
	for (list<tgOddsGoods>::const_iterator it=list_goods.begin(); 
		it!= list_goods.end(); it++)
	{
		if (random(10000)< (*it).fOdds * 10000)
		{
			DWORD dwGoodsIndex=GoodsSetup::QueryGoodsIDByOriginalName((*it).cGoodsName);
			CGoodsFactory::CreateGoods( dwGoodsIndex, (*it).iGoodNum, GainList,10 );
		}
	}
	CTestContainer testContainer;
	testContainer.Init(this);
	vector<tagTestResult> vecResult;
	//循环获得品列表,如果不能添加到背包,则添加到场景
	CGUID guid = NULL_GUID;
	CGUID::CreateGUID(guid);
	for (vector<CGoods*>::iterator it=GainList.begin();it!=GainList.end();it++)
	{
		CGoods* pGoods=*it;
		if (pGoods)
		{
			BOOL bSucced=testContainer.AddTest( pGoods,&vecResult);
			if (bSucced)
			{
				GetLogicLogInterface()->logT055_collect(this,pGoods,guid,pGoods->GetAmount());										
				if (vecResult.size()==1)
				{
					AddGoodsByTest(&vecResult[0],pGoods);
				}
			}
			else
			{
				GetLogicLogInterface()->logT055_collect(this,pGoods,guid,pGoods->GetAmount());
				SendNotifyMessage(AppFrame::GetText("1"));
				//添加到场景并设置保护时间
				CServerRegion* pRegion=dynamic_cast<CServerRegion*>(GetFather());
				if(pRegion)
				{
					long x=0, y=0;
					long lDropGoodsStartPos=0;

					lDropGoodsStartPos= pRegion->GetDropGoodsPos(GetTileX(), GetTileY(), x, y, lDropGoodsStartPos);
					if( lDropGoodsStartPos != -1 )
					{
						(pGoods)->SetPosXY(x+0.5f, y+0.5f);
						pRegion->AddObject( pGoods);				
						pRegion -> SetGoodsProtection( this, pGoods,FALSE);				
						// 通知每个玩家
						CS2CContainerObjectMove comMsg;
						comMsg.SetOperation( CS2CContainerObjectMove::OT_NEW_OBJECT );
						comMsg.SetDestinationContainer( pRegion -> GetType(), 
							pRegion -> GetExID(), y * pRegion -> GetWidth() + x );
						comMsg.SetDestinationObject(pGoods -> GetType(),pGoods-> GetExID() );
						comMsg.SetDestinationObjectAmount(pGoods-> GetAmount() );
						pGoods-> SerializeForOldClient( &( comMsg.GetObjectStream() ) );
						comMsg.SendToAround( this );
						comMsg.Send(GetExID());
					}
				}
			}
		}
	}
	pMoveShape->BeenCollected();
	return TRUE;
}

tagUseFailed CPlayer::CanCollect(CMoveShape* pMoveShape)
{
	//当前状态不能被采集

	//pPlayer->SetCurrentProgress( CPlayer::PROGRESS_OPEN_STALL );//摆摊
	//pPlayer -> SetCurrentProgress( CPlayer::PROGRESS_TRADING );//交易
	if (GetCurrentProgress()==CPlayer::PROGRESS_OPEN_STALL 
		|| GetCurrentProgress()==CPlayer::PROGRESS_TRADING 
        ||  IsLockPlayerState() )
	{
		return SKILL_USE_FAILED_CANNOTCOLLECTION_STATE;
	}

	//采集物能否被采集
	if (pMoveShape==NULL )
	{
		return SKILL_USE_FAILED_C_NOCOLLECTION;//无采集物对象
	}
	if ((!pMoveShape->GetCanBeenCollect()) || pMoveShape->GetIsBeenCollect()|| 
		pMoveShape->GetCollectionTimes() <=0)
	{
		return SKILL_USE_FAILED_C_STATE_ERROR;//不能采集状态
	}
	if (pMoveShape->GetRoleProtect())//任务保护状态
	{
		return SKILL_USE_FAILED_C_ROLE_PROTECT;
	}

	//剥皮技能时判断受益人
	if(pMoveShape->GetType() == TYPE_MONSTER)
	{
		if (pMoveShape->GetBeneficiaryID()!=NULL_GUID)
		{
			if(pMoveShape->GetBeneficiaryID()!=m_guid && pMoveShape->GetBeneficiaryID() != m_TeamID)
			{
				return SKILL_USE_FAILED_C_STATE_ERROR;//不能采集状态
			}
		}
	}

	const CCollectionLayout* pCollectionLayout= CDeputyOccuSystem::GetCollectionLayout(pMoveShape->GetCollectionName().c_str());
	if (pCollectionLayout==NULL)
	{
		return SKILL_USE_FAILED_C_UNKNOW_COLLECTION;//未知采集物品
	}

	tagUseFailed tgUseSkillRes = SKILL_USE_FAILED_INVALID_UNKNOWN;

	if(pCollectionLayout->GetOccuID()< DOccu_Other)
	{
		tgUseSkillRes= CanDOccuCollect(pMoveShape);
	}
	else if (pCollectionLayout->GetOccuID()== DOccu_Other)
	{
		tgUseSkillRes= CanRoleCollect(pMoveShape);
	}

	if (tgUseSkillRes!= SKILL_USE_NO_FAILED)
	{
		return tgUseSkillRes;
	}

	return tgUseSkillRes;

}
tagUseFailed CPlayer::CheckCanCollect(CMoveShape* pMoveShape)
{
	//当前状态不能被采集
	if (GetCurrentProgress()==CPlayer::PROGRESS_OPEN_STALL 
		|| GetCurrentProgress()==CPlayer::PROGRESS_TRADING )
	{
		return SKILL_USE_FAILED_CANNOTCOLLECTION_STATE;
	}

	//采集物能否被采集
	if (pMoveShape==NULL )
	{
		return SKILL_USE_FAILED_C_NOCOLLECTION;//无采集物对象
	}

	const CCollectionLayout* pCollectionLayout= CDeputyOccuSystem::GetCollectionLayout(pMoveShape->GetCollectionName().c_str());
	if (pCollectionLayout==NULL)
	{
		return SKILL_USE_FAILED_C_UNKNOW_COLLECTION;//未知采集物品
	}

	if(pCollectionLayout->GetOccuID()< DOccu_Other)
	{
		return CanDOccuCollect(pMoveShape);
	}
	else if (pCollectionLayout->GetOccuID()== DOccu_Other)
	{
		return CanRoleCollect(pMoveShape);
	}
	else
		return SKILL_USE_FAILED_INVALID_UNKNOWN;
}
BOOL CPlayer::CheckCollect(CMoveShape* pMoveShape)
{
	const CCollectionLayout* pCollectionLayout= CDeputyOccuSystem::GetCollectionLayout(pMoveShape->GetCollectionName().c_str());
	if (pCollectionLayout==NULL)
	{
		return FALSE;
	}

	BOOL rst=FALSE;

	if(pCollectionLayout->GetOccuID()< DOccu_Other)
	{
		rst= DOccuCollect(pMoveShape);
	}
	else if (pCollectionLayout->GetOccuID()== DOccu_Other)
	{
		rst= RoleCollect(pMoveShape);
	}
	else
		rst= FALSE;

	if (rst== TRUE)
	{
		const string& strScrpt = pCollectionLayout->GetSuccScriptName();
		if (strScrpt!="")
		{
			//stRunScript st;
			//st.pszFileName   = (char*)(strScrpt.c_str());
			//st.srcShape  = this;
			//st.desShape	 = pMoveShape;
			//st.pRegion   = dynamic_cast<CRegion*>(GetFather());
			//RunScript(&st,(char*)GetGame()->GetScriptFileData(st.pszFileName));	
			Script::RunEnv env( this, pMoveShape, static_cast<CRegion*>( GetFather() ) );
			GetInst( ScriptSys ).RunScript( env, strScrpt.c_str() );
		}
	}
	return rst;
}
//锁定采集物
void CPlayer::LockCollection(CMoveShape* pMoveShape)
{
	pMoveShape->SetIsBeenCollect(TRUE);
	pMoveShape->SetCanBeenCollect(FALSE);
}
//解除采集物锁定
void CPlayer::OutlockCollection(CMoveShape* pMoveShape)
{
	pMoveShape->SetIsBeenCollect(FALSE);
	if (pMoveShape->GetCollectionTimes()>0)
	{
		pMoveShape->SetCanBeenCollect(TRUE);
	}
	else
	{
		pMoveShape->SetCanBeenCollect(FALSE);
	}

}
//是否能制作
tagUseFailed CPlayer::CanFacture()
{
	//当前状态不能制作
	if (GetCurrentProgress()==CPlayer::PROGRESS_OPEN_STALL 
		|| GetCurrentProgress()==CPlayer::PROGRESS_TRADING )
	{
		return SKILL_USE_FAILED_CANNOTFACTURE_STATE;
	}

	DWORD factureID=GetElectFactureID();
	const CFacture* pFacture= CDeputyOccuSystem::GetFacture(factureID);
	if (pFacture==NULL)
	{
		return SKILL_USE_FAILED_F_NO_FACTURE;// 不存在的制作项目
	}
	//职业限制
	if (GetDOccupation()!= pFacture->GetArtisanID())
	{
		return SKILL_USE_FAILED_F_DOCCU_ERROR;//副职业限制
	}
	//有无此制作条目
	if (!FindFacture(factureID))
	{
		return SKILL_USE_FAILED_F_NOTHAS_FACTURE;//不存在制作条目
	}
	//工具条件限制
	const list<tgGoods>& list_Tools= pFacture->GetToolList();
	for (list<tgGoods>::const_iterator it= list_Tools.begin(); it!= list_Tools.end(); it++)
	{
		//工具条件判断
		int goodsnum=GetGoodsAmountByOriginName((*it).cGoodsName);
		if (goodsnum< (*it).iGoodNum)
		{
			return SKILL_USE_FAILED_F_GOODSCONDITION_ERROR;//工具限制
		}
	}
	//
	//材料判断
	const list<tgGoods>& list_Material=pFacture->GetMaterialList();
	for (list<tgGoods>::const_iterator it= list_Material.begin(); it!= list_Material.end(); it++)
	{
		//材料判断
		int goodsnum=GetGoodsAmountByOriginName((*it).cGoodsName);
		if (goodsnum< (*it).iGoodNum)
		{
			return SKILL_USE_FAILED_F_MATERIALS_ERROR;//原料限制
		}
	}
	//npc距离条件限制
	const list<tgNpcDistance>& list_NpcDis= pFacture->GetNpcList();
	CServerRegion* pRegion = dynamic_cast<CServerRegion*>( GetFather() );//寻找所在场景
	if (pRegion==NULL)
	{
		return SKILL_USE_FAILED_INVALID_UNKNOWN;
	}
	vector<CMonster*> vecNPC;
	if (list_NpcDis.size()!=0)
	{

		vector<CShape*> vOut;
		pRegion -> FindAroundObject( this, TYPE_MONSTER ,vOut );

		for (long a=0; a< (long)vOut.size(); a++)
		{
			CMonster* monster=dynamic_cast<CMonster*>(vOut[a]);
			if (monster!= NULL && monster->GetNpcType()==NT_Normal)
			{
				vecNPC.push_back(monster);
			}
		}
		if (vecNPC.size()==0)
		{
			return SKILL_USE_FAILED_F_NPC_ERROR;//npc距离限制
		}

	}


	for (list<tgNpcDistance>::const_iterator it= list_NpcDis.begin(); it!= list_NpcDis.end(); it++)
	{
		int iNPCnum=0;
		for (long a=0; a< (long) vecNPC.size(); a++)
		{
			CMonster* pNPC= vecNPC[a];
			if (pNPC==NULL && 0==strcmp(pNPC->GetOriginName(),(*it).cNpcName) && Distance(pNPC)<(*it).iDistace )
			{
				iNPCnum++;
			}
		}
		if (iNPCnum==0)
		{
			return SKILL_USE_FAILED_F_NPC_ERROR;//npc条件限制
		}
	}
	//背包空格判断
	//DWORD dwEmptyGrid=0;
	//dwEmptyGrid+=getOriginPack()->GetEmptySpaceNum();
	//for(int i=0;i<CSubpackContainer::SUBPACK_NUM;i++)
	//{
	//	CSubpackContainer::tagSubpack* pTagSubpack=GetPackage()->GetSubpack(i);
	//	if(pTagSubpack && pTagSubpack->pSubpackContainer)
	//	{
	//		dwEmptyGrid+=pTagSubpack->pSubpackContainer->GetEmptySpaceNum();
	//	}
	//}
	//if (dwEmptyGrid<=0)
	//{
	//	return SKILL_USE_FAILED_F_NO_BAGSPACE;//背包空间限制
	//}

	return SKILL_USE_NO_FAILED;//无失败

}
//制作
BOOL CPlayer::Facture()
{
	DWORD factureID=GetElectFactureID();

	const CFacture* pFacture= CDeputyOccuSystem::GetFacture(factureID);
	if (pFacture==NULL)
	{
		return FALSE;
	}



	//添加物品
	//DWORD dwRandomNum=random(CDeputyOccuSystem::GetAdvancedGainedBacePro());

	vector<CGoods*> GainList;

	//const list<tgGoods>& list_goods;
	/*if (dwRandomNum< GetFactureSuccRatio())
	{
	const list<tgGoods>&list_goods= pFacture->GetAdvanceGainedList();
	ClearFactureSuccRatio();
	for (list<tgGoods>::const_iterator it = (list_goods).begin(); it!= (list_goods).end(); it++)
	{
	DWORD dwGoodsIndex= GoodsSetup::QueryGoodsIDByOriginalName((*it).cGoodsName);
	GoodsSetup::CreateGoods( dwGoodsIndex, (*it).iGoodNum, GainList );
	}
	}
	else
	{*/
	const list<tgGoods>&list_goods = pFacture->GetGainedList();
	AddFactureSuccRatio(1);
	for (list<tgGoods>::const_iterator it = (list_goods).begin(); it!= (list_goods).end(); it++)
	{
		DWORD dwGoodsIndex= GoodsSetup::QueryGoodsIDByOriginalName((*it).cGoodsName);
		CGoodsFactory::CreateGoods( dwGoodsIndex, (*it).iGoodNum, GainList ,11);
	}
	//	}
	//获取材料列表,测试删除
	vector<CGoods*>vecMaterial;
	const list<tgGoods>& list_Material=pFacture->GetMaterialList();
	tagTestArg_ID_Num tagTestArg;
	vector<tagTestArg_ID_Num>vecArg;
	for (list<tgGoods>::const_iterator it= list_Material.begin(); it!= list_Material.end(); it++)
	{
		tagTestArg._dwGoodsID=GoodsSetup::QueryGoodsIDByOriginalName(it->cGoodsName);
		tagTestArg._dwNum=it->iGoodNum;
		vecArg.push_back(tagTestArg);
		/*DWORD dwGoodsIndex= GoodsSetup::QueryGoodsIDByOriginalName((*it).cGoodsName);
		GoodsSetup::CreateGoods( dwGoodsIndex, (*it).iGoodNum, vecMaterial );*/
	}


	CTestContainer testContainer;
	testContainer.Init(this);
	vector<tagDelTestResult> vecDelResult;
	BOOL bDelSucceed=testContainer.DelTest(vecArg,&vecDelResult);
	vector<tagTestResult>vecResult;
	BOOL bSucced=testContainer.AddTest(GainList,&vecResult);
	if (bDelSucceed && bSucced && vecResult.size() == GainList.size())
	{
		CGUID guid = NULL_GUID;
		CGUID::CreateGUID(guid);
		//增加经验
		DWORD exp=CDeputyOccuSystem::GetFactureExp(GetFactureGrade()- pFacture->GetFactureLevel());//GetCollectionExp(GetCollectionGrade()-pCollectionLayout->GetCollectLevel());
		exp *= (DWORD)pFacture->GetQuotiety();
		AddFactureExp(exp);
		DWORD credit= CDeputyOccuSystem::GetFactureCredit(GetCollectionGrade() - pFacture->GetFactureLevel());
		if (credit>0)
		{
		//	SetArtisanCredit(GetArtisanCredit()+ credit);
		}

		//材料删除
		for (long i=0;i<(long)vecDelResult.size();i++)
		{
			CGoodsContainer *pContainer = FindContainer(vecDelResult[i].dwContainerId);
			if (pContainer)
			{
				CGoods* pGoods = dynamic_cast<CGoods*>( pContainer -> GetGoods(vecDelResult[i].dwPos) );
				if (pGoods)
				{
                    GetLogicLogInterface()->logT056_facture(this,pGoods,guid,0,vecDelResult[i].dwNum);
				}
				DelGoodsByTest(&vecDelResult[i]);
			}	
		}

		for(long i=0;i<(long)vecResult.size();i++)
		{
			if (GainList[i])
			{
				//记录制作者
				const char* goodsname = GoodsSetup::QueryGoodsOriginalName(GainList[i]->GetBasePropertiesIndex());
				if (goodsname != NULL)
				{
					list<tgGoods>::const_iterator it = list_goods.begin();
					for (; it!= list_goods.end(); it++)
					{
						if (strcmp(goodsname,(*it).cGoodsName) == 0 &&
							(*it).bMaker == 1)
						{
							GainList[i]->SetMakerName(this->GetName());
							break;
						}	
					}
				}
				GetLogicLogInterface()->logT056_facture(this,GainList[i],guid,1,GainList[i]->GetAmount());
				AddGoodsByTest(&vecResult[i],GainList[i]);
			}
		}
	}
	else
	{
		//添加不成功,回收内存
		for(long i=0;i<(long)GainList.size();i++)
		{
#ifdef _GOODSLOG1_
			if(GainList[i])
			{
				char pszGoodsLog[1024]="";			
				char srcGoodsGUID[50]="";
				GainList[i]->GetExID().tostring(srcGoodsGUID);					
				_snprintf(pszGoodsLog,1024,"[物品回收:CPlayer::Facture][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d]",
					GainList[i]->GetBasePropertiesIndex(),GoodsSetup::QueryGoodsName(GainList[i]->GetBasePropertiesIndex()),srcGoodsGUID,GainList[i]->GetAmount());
				CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);	
			}									
#endif
			CGoodsFactory::GarbageCollect(&GainList[i],20);
		}
		SendNotifyMessage(AppFrame::GetText("1"));
		return FALSE;
	}
	return TRUE;
}

void CPlayer::FactureSkillStudyBegin(CMoveShape* moveshape)
{
	if (moveshape==NULL || moveshape->GetType()!=TYPE_MONSTER )	return;
	CMonster* pMonster=dynamic_cast<CMonster*>(moveshape);
	if (pMonster->GetNpcType()!=NT_Normal)	return;

	const CFactureStudy* pfacturestudy= CDeputyOccuSystem::GetFactureStudy(pMonster->GetOriginName());
	if (pfacturestudy==NULL)	return ;
	vector<DWORD> vecStudy;
	for (long i=0;i<(long)pfacturestudy->GetStudySum(); i++)
	{
		DWORD factureID= pfacturestudy->GetFactureID(i);
		const CFacture* pfacture= CDeputyOccuSystem::GetFacture(factureID);
		if (pfacture==NULL) continue;
		if (GetDOccupation()== pfacture->GetArtisanID()
			//&& GetFactureGrade()>= pfacture->GetFactureLevel()
			&& !FindFacture(factureID))
		{
			vecStudy.push_back(factureID);
		}
	}
	CMessage msg(MSG_S2C_OCCUSKILL_STUDY_BEGIN);
	msg.Add(pMonster->GetExID());
	msg.Add((DWORD)vecStudy.size());
	for (long a=0;a<(long)vecStudy.size(); a++)
	{
		msg.Add(vecStudy[a]);
		msg.Add(pfacturestudy->FindFactureCost(vecStudy[a]));
		////#ifdef _DUMP
		//char filename[256];
		//_snprintf(filename,"facture_studyinfo id= %d, cost= %d",vecStudy[a],pfacturestudy->FindFactureCost(vecStudy[a]));
		//PutStringToFile("facture_studyinfo",filename);
		//#endif
	}
	msg.SendToPlayer(GetExID());
}

void CPlayer::StudyFactureSkill(CMoveShape* moveshape, DWORD factureID)
{
	if (moveshape==NULL || moveshape->GetType()!=TYPE_MONSTER )	return;
	CMonster* pMonster=dynamic_cast<CMonster*>(moveshape);
	if (pMonster->GetNpcType()!=NT_Normal)	return;

	const CFactureStudy* pfacturestudy= CDeputyOccuSystem::GetFactureStudy(pMonster->GetOriginName());
	if (pfacturestudy==NULL)	return ;

	const CFacture* pfacture= CDeputyOccuSystem::GetFacture(factureID);
	if (pfacture== NULL) return;

	if (GetDOccupation()== pfacture->GetArtisanID()&& 
		GetFactureGrade()>= (LONG)pfacture->GetFactureLevel())
	{
		if (FindFacture(factureID)==FALSE && pfacturestudy->FindFacture(factureID)!=FALSE)
		{
			DWORD cost= pfacturestudy->FindFactureCost(factureID);
			if (GetMoney()>= cost)
			{
				SetMoney(GetMoney()-cost,8);
				AddFacture(factureID);
				return ;
			}
		}
	}
	CMessage msg(MSG_S2C_OCCUSKILL_STUDY);
	msg.Add(LONG(0));
	msg.Add(factureID);
	msg.SendToPlayer(GetExID());
}

void CPlayer::CleanFacture()
{
	CMessage msg(MSG_S2C_OCCUSKILL_DELETE);
	msg.SendToPlayer(GetExID());
	m_DOccuRecord.CleanFacture();
}

void CPlayer::AddCollectionExp(DWORD exp)
{
	if (exp==0)
	{
		return;
	}
	DWORD Level=GetCollectionGrade();
	DWORD CurrentExp=GetCollectionExp();
	long all = CurrentExp + exp;
	if (Level == CDeputyOccuSystem::GetMaxCollectionLevel() &&
		all > (long)CDeputyOccuSystem::GetCollectionNeededExp(Level) )
	{
		SetCollectionExp(CDeputyOccuSystem::GetCollectionNeededExp(Level));
		return;
	}
	bool bUpLevel = false;
	long lastexp =0;
	while (all > (long)CDeputyOccuSystem::GetCollectionNeededExp(Level)&& Level < CDeputyOccuSystem::GetMaxCollectionLevel())
	{
		all -=CDeputyOccuSystem::GetCollectionNeededExp(Level);
		if (all > 0)
		{
			lastexp =all;
		Level++;
			bUpLevel = true;
		}		
	}
	//升级
	if (bUpLevel)
	{
		long temp = CDeputyOccuSystem::GetCollectionNeededExp(Level);
		SetCollectionGrade(Level);
		SetCollectionExp(lastexp>temp?temp:lastexp);	
	}
	else
	{
		SetCollectionExp(all);
	}
}

void CPlayer::AddFactureExp(DWORD exp)
{
	if (exp==0)
	{
		return;
	}

	long Level=GetFactureGrade();
	long CurrExp = GetFactureExp();
	long all = CurrExp + exp;
	//等级最高且经验已满
	if (Level == CDeputyOccuSystem::GetMaxFactureLevel() &&
		all > (long)CDeputyOccuSystem::GetFactureNeededExp(Level) )
	{
		SetFactureExp(CDeputyOccuSystem::GetFactureNeededExp(Level));
		return;
	}
	bool bUpLevel = false;
	long lastexp =0;
	while (all > (long)CDeputyOccuSystem::GetFactureNeededExp(Level)&& Level < (long)CDeputyOccuSystem::GetMaxFactureLevel())
	{
		all -= (long)CDeputyOccuSystem::GetFactureNeededExp(Level);
		if (all > 0)
		{
			lastexp =all;
		Level++;
			bUpLevel = true;
	}
	}
	//升级
	if (bUpLevel)
	{
		long temp = CDeputyOccuSystem::GetFactureNeededExp(Level);
		SetFactureGrade(Level);
		SetFactureExp(lastexp>temp?temp:lastexp);	
	}
	else
	{
		SetFactureExp(all);
	}	
	}

void CPlayer::SendDOccuSKillFailMsg(int nValue,int iSkillID, int iSkillLevel)//SendSKillFailMsg(int nValue)
{
	CMessage msg(MSG_S2C_SKILL_USE);
	msg.Add((CHAR)SKILL_USE_RESULT_FAILED);
	msg.Add( GetExID() );
	//id和等级
	msg.Add((LONG)iSkillID);
	msg.Add((BYTE)iSkillLevel );
	msg.Add((DWORD)0);
	//错误类型
	msg.Add((BYTE)nValue);
	msg.SendToPlayer(GetExID());
#ifdef _SKILL_DEBUG
	Log4c::Trace(ROOT_MODULE,"发送技能失败消息:技能ID:%6d\n",iSkillID);
#endif
}

//对副职业系统的查询
long CPlayer::GetFactureNeedsLevel(DWORD ID)//{return CDeputyOccuSystem::GetFacture(ID)->GetFactureLevel();}
{
	const CFacture* p= CDeputyOccuSystem::GetFacture(ID);
	if (p)
	{
		return p->GetFactureLevel();
	}
	else 
		return -1;
}
long CPlayer::GetFactureNeedsDOccu(DWORD ID)//{return CDeputyOccuSystem::GetFacture(ID)->GetArtisanID();}
{
	const CFacture* p= CDeputyOccuSystem::GetFacture(ID);
	if (p)
	{
		return p->GetArtisanID();
	}
	else 
		return -1;

}

BOOL CPlayer::AddFacture(DWORD l)
{
	if (m_DOccuRecord.AddFacture(l))
	{
		CMessage msg(MSG_S2C_OCCUSKILL_STUDY);
		msg.Add((LONG)1);
		msg.Add(l);
		msg.SendToPlayer(GetExID());
		GetGame()->UpdateSavePlayer(GetExID(), SAVE_DETAIL_ATTRPROFESSION);
		GetGame()->UpdateSavePlayer(this, SAVE_DETAIL_ATTRPROPERTY);
		return TRUE;
	}
	return FALSE;
	//m_DOccuRecord.AddFacture(l);
}
BOOL CPlayer::DeleteFacture(DWORD l)
{
	BOOL bflag = m_DOccuRecord.DeleteFacture(l);
	GetGame()->UpdateSavePlayer(GetExID(), SAVE_DETAIL_ATTRPROFESSION);
	GetGame()->UpdateSavePlayer(this, SAVE_DETAIL_ATTRPROPERTY);
	return bflag;
}

void CPlayer::SetCollectionExp(LONG l)
{
	m_mapDirtyAttrData.insert("dwCollectionExp");
	m_DOccuRecord.m_lCollectionExp=  l;
	GetGame()->UpdateSavePlayer(GetExID(), SAVE_DETAIL_ATTRPROFESSION);
	GetGame()->UpdateSavePlayer(this, SAVE_DETAIL_ATTRPROPERTY);
	UpdateAttribute(true);
}
void CPlayer::SetCollectionGrade(LONG l)
{
	m_DOccuRecord.m_lCollectionGrade= l;
	CMessage msg(MSG_S2C_PLAYER_COLLECTION_LEVELUP);
	msg.Add(l);
	msg.Add(GetExID());
	msg.SendToAround(this);
	GetGame()->UpdateSavePlayer(GetExID(), SAVE_DETAIL_ATTRPROFESSION);
	GetGame()->UpdateSavePlayer(this, SAVE_DETAIL_ATTRPROPERTY);
}

void CPlayer::SetFactureExp(LONG l)
{
	m_mapDirtyAttrData.insert("dwFactureExp");
	m_DOccuRecord.m_lFactureExp = l;
	GetGame()->UpdateSavePlayer(GetExID(), SAVE_DETAIL_ATTRPROFESSION);
	GetGame()->UpdateSavePlayer(this, SAVE_DETAIL_ATTRPROPERTY);
	UpdateAttribute(true);
}

void CPlayer::SetFactureGrade(LONG l)
{
	m_DOccuRecord.m_lFactureGrade = l;
	GetGame()->UpdateSavePlayer(GetExID(), SAVE_DETAIL_ATTRPROFESSION);
	GetGame()->UpdateSavePlayer(this, SAVE_DETAIL_ATTRPROPERTY);
	CMessage msg(MSG_S2C_PLAYER_FACTURE_LEVELUP);
	msg.Add(l);
	msg.Add(GetExID());
	msg.SendToAround(this);
	//发送提示消息
}
//int CPlayer::SetScriptVarValue(CScript* scriptPtr, const char* varName, const char* value)
//{
//	int retFlag = GetVariableList()->SetVarValue(varName, value);
//	if(retFlag == CVariableList::VAR_NOT_FOUND && scriptPtr)
//		retFlag = scriptPtr->GetVariableList()->SetVarValue(varName, value);
//	if(retFlag == CVariableList::VAR_NOT_FOUND)
//	{
//		if(CScript::GetGeneralVariableList())
//			retFlag = CScript::GetGeneralVariableList()->SetVarValue(varName, value);
//		if(1 == retFlag)
//			CScript::UpdateToWorldServer(varName, value);
//	}
//
//	return retFlag;
//}
//int CPlayer::SetScriptVarValue(CScript* scriptPtr, const char* varName, long lArray, double value)
//{
//	int retFlag = GetVariableList()->SetVarValue(varName, lArray, value);
//	if(retFlag == CVariableList::VAR_NOT_FOUND && scriptPtr)
//		retFlag = scriptPtr->GetVariableList()->SetVarValue(varName, lArray, value);
//	if(retFlag == CVariableList::VAR_NOT_FOUND)
//	{
//		if(CScript::GetGeneralVariableList())
//			retFlag = CScript::GetGeneralVariableList()->SetVarValue(varName, lArray, value);
//		if(1 == retFlag)
//			CScript::UpdateToWorldServer(varName, lArray, value);
//	}
//
//	return retFlag;
//}
//
//const char* CPlayer::GetScriptVarValue(CScript* scriptPtr, const char* varName)
//{
//	const char* retStr = GetVariableList()->GetVarValue(varName);
//	if(retStr == NULL && scriptPtr)
//		retStr = scriptPtr->GetVariableList()->GetVarValue(varName);
//	if(retStr == NULL)
//	{
//		if(CScript::GetGeneralVariableList())
//			retStr = CScript::GetGeneralVariableList()->GetVarValue(varName);
//	}
//
//	return retStr;
//}
//double CPlayer::GetScriptVarValue(CScript* scriptPtr, const char* varName, long lArray)
//{
//	double retFlag = GetVariableList()->GetVarValue(varName, lArray);
//	if(retFlag == CVariableList::VAR_NOT_FOUND && scriptPtr)
//		retFlag = scriptPtr->GetVariableList()->GetVarValue(varName, lArray);
//	if(retFlag == CVariableList::VAR_NOT_FOUND)
//	{
//		if(CScript::GetGeneralVariableList())
//			retFlag = CScript::GetGeneralVariableList()->GetVarValue(varName, lArray);
//	}
//
//	return retFlag;
//}
//int CPlayer::SetScriptGuidValue(CScript* scriptPtr, const char* varName, const CGUID& guid)
//{
//	int retFlag = GetVariableList()->SetGuidValue(varName, &guid);
//	if(retFlag == CVariableList::VAR_NOT_FOUND && scriptPtr)
//		retFlag = scriptPtr->GetVariableList()->SetGuidValue(varName, &guid);
//	if(retFlag == CVariableList::VAR_NOT_FOUND)
//	{
//		if(CScript::GetGeneralVariableList())
//			retFlag = CScript::GetGeneralVariableList()->SetGuidValue(varName, &guid);
//	}
//
//	return retFlag;
//}
//const CGUID& CPlayer::GetScriptGuidValue(CScript* scriptPtr, const char* varName)
//{
//	return scriptPtr->GetScriptGuidValue( this, varName );
//}

//取信
//CMail *CPlayer::GetMail(CGUID &guid)
//{
//	map<CGUID,CMail*>::iterator it=m_mapMail.find(guid);
//	if (it!=m_mapMail.end())
//	{
//		return (*it).second;
//	}
//	return NULL;
//}
void CPlayer::UpdateEffect()
{
	CMessage msg(MSG_S2C_RGN_UPDATEEFFECT);
	msg.SendToPlayer(GetExID());
}

//! 设置称号的显示类型
BOOL CPlayer::SetTitleType(LONG lType)
{
	if(ePTT_Hide <= lType && ePTT_LOVER >= lType && lType != m_lCurrTitleType)
	{
		m_lCurrTitleType = lType;
		UpdateTitle(TRUE);

	}
	return FALSE;
}

//! 更新称号
void CPlayer::UpdateTitle(BOOL bSendNotify)
{
	const char* pNewTitle = NULL;
	//! 不能直接获得完整称号的时候，用到的缓存字符串
	char szTileBuf[128] = {0};

	switch(m_lCurrTitleType)
	{
	case ePTT_Knight:	//! 爵位称号
		{
	//		pNewTitle = AppFrame::GetText(IDS_GS_NOBILITYTITILE,GetRankofNobility());
		}
		break;
	case ePTT_Faction:	//! 帮会称号
		{
			GameFaction *pFaction = GetOrganiCtrl()->GetFactionOrganizing(GetFactionID());
			if(NULL != pFaction)
			{
				pNewTitle = pFaction->GetMemberTitle(GetExID());
			}
			else
				m_lCurrTitleType = ePTT_Hide;
		}
		break;
	case ePTT_Medal1:	//! 勋章1称号
		{
			CGoods *pMedal = m_cEquipment.GetGoods(EC_MEDAL1);
			if(NULL != pMedal)
			{
				pNewTitle = pMedal->GetName();
			}
			else
				m_lCurrTitleType = ePTT_Hide;
		}
		break;
	case ePTT_Medal2:	//! 勋章2称号
		{
			CGoods *pMedal = m_cEquipment.GetGoods(EC_MEDAL2);
			if(NULL != pMedal)
			{
				pNewTitle = pMedal->GetName();
			}
			else
				m_lCurrTitleType = ePTT_Hide;
		}
		break;
	case ePTT_Medal3:	//! 勋章3称号
		{
			CGoods *pMedal = m_cEquipment.GetGoods(EC_MEDAL3);
			if(NULL != pMedal)
			{
				pNewTitle = pMedal->GetName();
			}
			else
				m_lCurrTitleType = ePTT_Hide;
		}
		break;
	case ePTT_LOVER:	//! 配偶名称
		{
			if(GlobalSetup::GetSetup()->lShowSpouseNameStep <= GetMarriageStep())
			{
				const char * pSpouseName = GetSpouseName();
				if(NULL != pSpouseName)
				{
					//! 0：男，1：女
					if(0 == GetSex())
						_snprintf(szTileBuf, 128, "%s%s", pSpouseName, AppFrame::GetText("72"));
					else
						_snprintf(szTileBuf, 128, "%s%s", pSpouseName, AppFrame::GetText("71"));

					pNewTitle = szTileBuf;
				}
			}
			else
				m_lCurrTitleType = ePTT_Hide;
				
		}
		break;
	}

	if(NULL == pNewTitle)
		pNewTitle = "";

	if(0 != strcmp(pNewTitle, m_AppendProperty.strTitle))
	{
		LONG lCpyLen = (long)strlen(pNewTitle);
		lCpyLen = (lCpyLen < TITLE_SIZE) ? lCpyLen : TITLE_SIZE - 1;
		memset(m_AppendProperty.strTitle, 0, TITLE_SIZE);
		memmove(m_AppendProperty.strTitle, pNewTitle, lCpyLen);

		if(bSendNotify)
		{
			CMessage msg(MSG_S2C_PLAYER_UPDATE_TITLE_TYPE);
			msg.Add(GetExID());
			msg.Add(m_lCurrTitleType);
			msg.Add(pNewTitle);
			msg.SendToAround(this);
		}
	}
}

//! 得到当前显示的勋章的原始名
//const char* CPlayer::GetShowMedalBaseName(void)
//{
//	switch(m_lCurrTitleType)
//	{
//	case ePTT_Medal1:	//! 勋章1称号
//		{
//			CGoods *pMedal = m_cEquipment.GetGoods(EC_MEDAL1);
//			if(NULL != pMedal)
//			{
//				return GoodsSetup::QueryGoodsOriginalName(pMedal->GetBasePropertiesIndex());
//			}
//		}
//		break;
//	case ePTT_Medal2:	//! 勋章2称号
//		{
//			CGoods *pMedal = m_cEquipment.GetGoods(EC_MEDAL2);
//			if(NULL != pMedal)
//			{
//				return GoodsSetup::QueryGoodsOriginalName(pMedal->GetBasePropertiesIndex());
//			}
//		}
//		break;
//	case ePTT_Medal3:	//! 勋章3称号
//		{
//			CGoods *pMedal = m_cEquipment.GetGoods(EC_MEDAL3);
//			if(NULL != pMedal)
//			{
//				return GoodsSetup::QueryGoodsOriginalName(pMedal->GetBasePropertiesIndex());
//			}
//		}
//		break;
//	}
//
//	return NULL;
//}


BOOL CPlayer::SerializeSubDepot(DBWriteSet& setWriteDB, LONG eContainerId)
{
	CSubpackContainer *pContainer = (CSubpackContainer*)m_pPlayerDepot.FindContainer(eDCT_Secondary, true);
	assert(NULL != pContainer);
	if(NULL == pContainer) return FALSE;

	LONG lIndex = eContainerId - eDCT_Secondary1;
	if(pContainer->GetSubpack(lIndex)->pGoods)
	{
		setWriteDB.AddToByteArray((BYTE)1);
		setWriteDB.AddToByteArray(static_cast<DWORD>(eContainerId));
		pContainer->GetSubpack(lIndex)->pGoods->Serialize(setWriteDB);
		pContainer->GetSubpack(lIndex)->pSubpackContainer->Serialize(setWriteDB);
	}
	else
	{
		setWriteDB.AddToByteArray((BYTE)0);
		setWriteDB.AddToByteArray(static_cast<DWORD>(eContainerId));

	}
	return TRUE;
}


void CPlayer::SetFactionID(const CGUID& FactionID)
{
	m_FactionID=FactionID;
	if(NULL_GUID == m_FactionID)
	{
		memset(m_szFactionName, 0, MAXSIZE_FactionName);
	}
	else
	{
		GameFaction *pFaction = GetOrganiCtrl()->GetFactionOrganizing(m_FactionID);
		assert(NULL != pFaction);//! 这里断言失败，说明过早调用了SetFactionID()接口，应用SetFactionInfo()代替
		if(NULL != pFaction)
			memmove(m_szFactionName, pFaction->GetName(), MAXSIZE_FactionName - 1);
	}

	// Fox@20081113 刷新角色帮会名称
	GetInst(CPlayerMan).RefreshElem(PET_FACTION, this);
}

void CPlayer::SetFactionInfo(GameFaction *pFaction)
{
	if(NULL == pFaction)
	{
		m_FactionID = NULL_GUID;
		memset(m_szFactionName, 0, MAXSIZE_FactionName);
	}
	else
	{
		m_FactionID = pFaction->GetExID();
		memmove(m_szFactionName, pFaction->GetName(), MAXSIZE_FactionName - 1);
	}

	// Fox@20081113 刷新角色帮会名称
	GetInst(CPlayerMan).RefreshElem(PET_FACTION, this);
}

void CPlayer::SetUnionID(const CGUID& UnionID)
{
	m_UnionID=UnionID;
	if(NULL_GUID == m_UnionID)
	{
		memset(m_szUnionName, 0, MAXSIZE_FactionName);
	}

	else
	{
		GameUnion *pUnion = GetOrganiCtrl()->GetUnion(m_UnionID);
		assert(NULL != pUnion);
		if(NULL != pUnion)
			memmove(m_szUnionName, pUnion->GetName(), MAXSIZE_FactionName - 1);
	}

	// Fox@20081113 刷新角色帮会名称
	GetInst(CPlayerMan).RefreshElem(PET_UNION, this);
}

CPet* CPlayer::CreatePet(const CGUID& rID)
{
	if( NULL_GUID == rID )
		return NULL;

	CPet* pPet = OBJ_CREATE(CPet);
	pPet->SetHost(this);
	pPet->SetExID(rID);
	m_Pets[rID] = pPet;
	return pPet;
}

/// 捕获侍从
bool CPlayer::CatchPet(CMonster* pCatchTarget)
{
	bool bWlid = false;
	long lTargetLev = pCatchTarget->GetLevel();
	if (lTargetLev>1)
	{
		bWlid = true;
	}

	// 侍从栏位有空余位置
	if (GetPetsNum() >= UNI_CONF.maxTakeNum)
	{
		Log4c::Trace(PET_MODULE,"CatchPet Failure : Allready have %ld Servants. Allow have %ld Servants",
            GetPetsNum(),UNI_CONF.maxTakeNum);
		return false;
	}
	
	// 目标不能捕获（在Monster文件配置） && 目标正在被其他玩家捕获中
	if (!pCatchTarget->IsCatchable())
	{
		Log4c::Trace(PET_MODULE,"CatchPet Failure : NPC Cann't be Catch or other's catching..");
		return false;
	}

	// 目标等级大于玩家5级
	if (GetLevel()+5 < pCatchTarget->GetLevel())
	{
		Log4c::Trace(PET_MODULE,"CatchPet Failure : NPC's level is too high");
		return false;
	}
	
	// 目标血量太高，无法捕获
	if (pCatchTarget->GetMaxHP()*0.2 > pCatchTarget->GetAttribute("Hp"))
	{
		Log4c::Trace(PET_MODULE, "CatchPet Failure : NPC's HP is too high");
		return false;
	}
	
	// 目标不属于当前玩家，不可捕获

	pCatchTarget->SetCatchFlag(true);

	//捕获技能影响捕获抗性
	
	//怪物捕获抗性为0
	if (pCatchTarget->GetCatchFastness()==0)
	{
		int rate = random(100);
		//50%捕获成功
		if (rate<50)
		{
			long lType = PET_TYPE_SERVANT;
			long lIndex = pCatchTarget->GetServantID();
			//删除目标怪物
			CServerRegion *pRegion = dynamic_cast<CServerRegion*>(GetFather());
			pRegion->DelShapeToAround(dynamic_cast<CMonster*>(pCatchTarget));

			//创建侍从
			GetInst(CPetCtrl).CreatePet(this,lType,lIndex,bWlid);
			return true;
		}
		//40%挣脱
		else if (rate >=50 && rate<90)
		{
			pCatchTarget->ResetMaxCatchFastness();
			return false;
		}
		else if (rate>=90 && rate<100)

		{
			//10%触发脚本
			const char* strScript = pCatchTarget->GetCacthScript().c_str();
			if(strScript)
			{
				Script::RunEnv env(this, NULL, static_cast<CRegion*>(GetFather()));
				GetInst(ScriptSys).RunScript(env, strScript);
			}
			return false;
		}
	}
	return false;
}

CPet* CPlayer::GetPet(const CGUID& rID)
{
	CPet* pPet = NULL;
	C_ITR_PET itr = m_Pets.find(rID);
	if( itr != m_Pets.end() )
	{
		pPet = itr->second;
	}

	return pPet;
}

void CPlayer::AddPet(CPet* pPet)
{
	//PutoutLog("Pet", LT_DEFAULT, AppFrame::GetText("GS_PET_10"), ++lPetsCount);
	m_Pets[pPet->GetExID()] = pPet;
}

bool CPlayer::DelPet(const CGUID& rID)
{
	ITR_PET itr = m_Pets.find(rID);
	if( itr != m_Pets.end() )
	{
		char str[100];
		sprintf_s(str,AppFrame::GetText("GS_PET_9"), itr->second->GetPetName());
		/***********************************************************************/
		/*  zhaohang 2010-11-8 fix 直接剔除颜色，颜色加入字符串中，客户端能自动解析*/
		/***********************************************************************/
		//GetGame()->SendNotifyMessage(str, GetExID(), NOTIFY_COLOR_RED);
		GetGame()->SendNotifyMessage(str,GetExID());
		DelPet(itr);
		return true;
	}
	return false;
}

void CPlayer::DelPet(ITR_PET& itr)
{
	Log4c::Trace(PET_MODULE, "DelPet : %ld.", --lPetsCount);
	m_Pets.erase(itr);

}

void CPlayer::ReleaseAllPets(void)
{
	ACT_PETS::iterator itrAP = m_FightPets.begin();
	while( itrAP != m_FightPets.end() )
	{
		CPet* pPet = GetFightPet(*itrAP);
		if( pPet != NULL )
		{
			++itrAP;
			//GetInst(CPetCtrl).DelPet(pPet, PET_OT_PLAYER_EXIT);
		}
		else
		{
			//PutoutLog("Pet", LT_ERROR, ".....");
			m_FightPets.erase(itrAP++);
		}
	}

	ITR_PET itr = m_Pets.begin();
	while( !m_Pets.empty() )
	{
		CPet* pPet = itr->second;
		if (pPet != NULL && pPet->IsDied())
		{
			pPet->DelFromRegion(PET_OT_PLAYER_EXIT);
		}

		OBJ_RELEASE(CPet,pPet);
		DelPet(itr++);
	}
	m_Pets.clear();
}

void CPlayer::ActivatePet(bool bActive)
{
	CPet* pPet = GetActivePet();

	if(NULL == pPet)
	{
        Log4c::Error(PET_MODULE,"%-15s %s",__FUNCTION__,FormatText("GS_PET_17",GetName()));//玩家%s没有激活宠物.
		return;
	}

	if(bActive && pPet->IsActive())
	{
		GetInst(CPetCtrl).ActivePet(pPet);
	}
	//else
	//{
	//	GetInst(CPetCtrl).DelPet(pPet, PET_OT_DISAPPEAR);
	//}
}

void CPlayer::SetFocusPetGUID(CGUID guid)
{
	m_FocusPet = guid;
}

const CGUID& CPlayer::GetFocusPetGUID(void)
{
	return m_FocusPet;
}

void CPlayer::SetActivePetGUID(CGUID guid)
{
	m_ActivePet = guid;
}

const CGUID& CPlayer::GetActivePetGUID(void)
{
	return m_ActivePet;
}

CPet* CPlayer::GetFocusPet()
{
	CPet* pPet = NULL;
	if (NULL_GUID==m_FocusPet)
		return NULL;

	C_ITR_PET itrPet = m_Pets.find(m_FocusPet);
	if (itrPet!=m_Pets.end())
	{
		pPet = itrPet->second;
		if( NULL == pPet )
		{
            Log4c::Error(PET_MODULE,"%-15s %s",__FUNCTION__,FormatText("GS_PET_18",GetName(),itrPet->first));//玩家%s类型为%d的焦点宠物为NULL.

		}
	}
	else
	{
        Log4c::Error(PET_MODULE,"%-15s %s",__FUNCTION__,FormatText("GS_PET_19",GetName(),itrPet->first));//玩家%s类型为%d的焦点宠物不在宠物列表中.
	}
	return pPet;
}

CPet* CPlayer::GetActivePet()
{
	CPet* pPet = NULL;
	if (NULL_GUID==m_ActivePet)
		return NULL;

	C_ITR_PET itrPet = m_Pets.find(m_ActivePet);
	if (itrPet!=m_Pets.end())
	{
		pPet = itrPet->second;
		if( NULL == pPet )
		{
            Log4c::Error(PET_MODULE,"%-15s %s",__FUNCTION__,FormatText("GS_PET_20",GetName(),itrPet->first));//玩家%s类型为%d的激活宠物为NULL.
		}
	}
	else
	{
        Log4c::Error(PET_MODULE,"%-15s %s",__FUNCTION__,FormatText("GS_PET_21",GetName()));//玩家%s类型的激活宠物不在宠物列表中.
	}
	return pPet;
}

CPet* CPlayer::GetFightPet(const CGUID& guid)
{
	CPet* pPet = NULL;
	//C_ITR_ACT_PET itr = m_FightPets.find(guid);
	C_ITR_ACT_PET itr = std::find(m_FightPets.begin(), m_FightPets.end(), guid);
	if( itr != m_FightPets.end() )
	{
		C_ITR_PET itrPet = m_Pets.find(*itr);
		if( itrPet != m_Pets.end() )
		{
			pPet = itrPet->second;
			if( NULL == pPet )
			{
                Log4c::Error(PET_MODULE,"%-15s %s",__FUNCTION__,FormatText("GS_PET_22",GetName(),itrPet->first));//玩家%s类型为%d的出战宠物为NULL.
			}
		}
		else
		{
            Log4c::Error(PET_MODULE,"%-15s %s",__FUNCTION__,FormatText("GS_PET_23",GetName(),itrPet->first));
		}
	}

	return pPet;
}

void CPlayer::AddFightPet(const CGUID& guid)
{
	Log4c::Trace(PET_MODULE,"AddActivePet : %ld.", ++lFightPetsCount);
	m_FightPets.push_back(guid);
}

bool CPlayer::AddFightPet(CPet* pPet)
{
	AddFightPet(pPet->GetExID());
	return true;
}

void CPlayer::DelFightPet(const CGUID& guid)
{
	Log4c::Trace(PET_MODULE, "DelFightPet : %ld.", --lFightPetsCount);
	ITR_ACT_PET itr = std::find(m_FightPets.begin(), m_FightPets.end(), guid);
	if( itr != m_FightPets.end() )
		m_FightPets.erase(itr);
}

// 检查宠物移动
void CPlayer::CheckPetsMove(void)
{
	// 只有激活宠物需要进行移动检测
	CPet* pPet = GetActivePet();
	if( NULL == pPet )
	{
        Log4c::Error(PET_MODULE,"%-15s %s",__FUNCTION__,FormatText("GS_PET_24",GetName(),m_ActivePet.tostring()));//玩家%s类型为%s的激活宠物为NULL.
	}
	pPet->ModifyPos(IT_SAME_RGN);
}

//获取队友和自己的GUID
void CPlayer::GetPlayerIDAndTeammateID(hash_set<CGUID,hash_guid_compare>& hsPlayers)
{
	if (GetTeamID()==NULL_GUID)
	{
		hsPlayers.insert(GetExID());
	}
	else
	{
		GSTeam *pGSTeam = (GSTeam*)(GetOrganiCtrl()->GetGameTeamByID( GetTeamID()));
		CServerRegion *pRegion = dynamic_cast<CServerRegion*>(GetFather());
		if (pGSTeam && pRegion)
		{
			map<CGUID,CPlayer*> mpTeamates;
			pGSTeam->GetLucrativeArea_Alive(mpTeamates,pRegion->GetExID(),GetTileX(),GetTileY(), TRUE);
			map<CGUID,CPlayer*>::iterator it=mpTeamates.begin();
			for (;it!=mpTeamates.end();++it)
			{
				hsPlayers.insert((*it).second->GetExID());
			}
		}
	}
}


// 修改宠物状态
void CPlayer::ChangeActivePetState(long lState)
{
	CPet* pPet = GetActivePet();
	if (pPet==NULL)
	{
        Log4c::Error(PET_MODULE,"%-15s %s",__FUNCTION__,FormatText("GS_PET_25",GetName()));//玩家%s无激活宠物
		return;
	}

	if( pPet->GetState() == lState ) return;

	StateAI::PetAI* pPetAI = (StateAI::PetAI*) pPet->GetAI();
	if( NULL == pPetAI )
	{
        Log4c::Error(PET_MODULE,"%-15s %s",__FUNCTION__,FormatText("GS_PET_26",GetName()));//玩家%s的激活宠物AI为NULL.
		return;
	}

	pPetAI->ChangeStateTo(lState);
}
//判断技能是否发送正常
bool	CPlayer::JudgeSkilllegality()
{
	bool bSkill = false;

	++m_lTotalSkillNum;

	if (m_lTotalSkillNum>GlobalSetup::GetSetup()->lTotalMaxSkillNum)
	{
		//技能总数大于越界数量
		DWORD dwEndTime = timeGetTime();
		float fIntelTime = ((float)(dwEndTime - m_dwSkillStartTime))/1000.0f;
		float fOneSecSkillNum = m_lTotalSkillNum/fIntelTime;
		if (fOneSecSkillNum>GlobalSetup::GetSetup()->fOneSecMaxSkillNum)
		{
			bSkill = true;
			Log4c::Trace(ROOT_MODULE,FormatText("GS_PLAYER_9",GetName(),fOneSecSkillNum));

			GetGame()->KickPlayer(GetExID());			
		}
		m_lTotalSkillNum = 0;
		m_dwSkillStartTime = dwEndTime;
	}

	return bSkill;
}
//判断NPC脚本是否发送正常
bool CPlayer::JudgeNpcScriptslegality(void)
{
	bool bRet = false;

	++m_lTotalNpcScriptsNum;

	if (m_lTotalNpcScriptsNum>GlobalSetup::GetSetup()->lTotalMaxNpcScriptsNum)
	{
		//技能总数大于越界数量
		DWORD dwEndTime = timeGetTime();
		float fIntelTime = ((float)(dwEndTime - m_dwNpcScriptsStartTIme))/1000.0f;
		float fOneSecNum = m_lTotalNpcScriptsNum/fIntelTime;
		if (fOneSecNum>GlobalSetup::GetSetup()->fOneSecMaxNpcScriptsNum)
		{
			bRet = true;
			Log4c::Warn(ROOT_MODULE,FormatText("GS_PLAYER_10",GetName(),fOneSecNum));//玩家%s,1秒内触发NPC脚本个数:%f 超出正常范围
			GetGame()->KickPlayer(GetExID());			
		}
		m_lTotalNpcScriptsNum = 0;
		m_dwNpcScriptsStartTIme = dwEndTime;
	}

	return bRet;
}
//判断当前场景能否原地复活
bool	CPlayer::CheckReliveByOrigin()
{
	CServerRegion* pRegion = dynamic_cast<CServerRegion*>( this->GetFather() );
	if (NULL!=pRegion && pRegion->GetRelive())
	{
		//当前场景可以原地复活
		return true;
	}
	//不能原地复活
	return false;
}


//释放挂起的死亡复活脚本
void CPlayer::FreeReliveScript()
{
	//CScript::PushDelScript(m_guidRelivScript);
    GetInst( ScriptSys ).PushDelScript( m_guidRelivScript );
}

//判断原地复活物品是否满足条件
bool	CPlayer::CheckReliveGoods(long &lIndex, long &lRatio, long &lCostNum,char* strOriginName)
{
		//获取复活物品列表
		list<ReLiveGoodsList::tagReLiveGoods*> lReliveGoods = ReLiveGoodsList::GetReliveGoodsList();

		if (lReliveGoods.size()==0)
			return false;
			
		list<ReLiveGoodsList::tagReLiveGoods*>::iterator it = lReliveGoods.begin();
		long lNum = 0;

		if(NULL != (*it))
		{
			lIndex = (*it)->dwIndex;
			lCostNum = (*it)->dwNum;
			lRatio = (*it)->dwRecoverRatio;
			strcpy_s(strOriginName,256, (*it)->strOrignalName.c_str());
			lNum =  GetGoodsAmountByOriginName((*it)->strOrignalName.c_str());
		}

		for (; it!=lReliveGoods.end(); ++it)
		{
			if(NULL!=(*it))
			{
				long lGoodsNum = GetGoodsAmountByOriginName((*it)->strOrignalName.c_str());
				 if(lGoodsNum>0)
				 {
					 //玩家身上有该物品
					 if (lGoodsNum >= (long)(*it)->dwNum)
					 {
						//满足物品条件
						 lIndex = (*it)->dwIndex;
						 lCostNum = (*it)->dwNum;
						 lRatio = (*it)->dwRecoverRatio;
						 lNum =  lGoodsNum;
						 strcpy_s(strOriginName, 256, (*it)->strOrignalName.c_str());
						 return true;
					 }
					 else if(lGoodsNum< (long)(*it)->dwNum && lNum==0)
					 {
						 //保存优先级最高不符合条件物品信息
						 lIndex = (*it)->dwIndex;
						 lCostNum = (*it)->dwNum;
						 lRatio = (*it)->dwRecoverRatio;
						 lNum =  lGoodsNum;
						 strcpy_s(strOriginName, 256, (*it)->strOrignalName.c_str());
					 }
				 }
			}			
		}
		return false;

}

// 通告玩家被攻击消息
void CPlayer::SendNotifyWarningMsg(bool isKilled, CPlayer* pAttacker)
{
	if(!pAttacker) return;
	CServerRegion* pRgn = static_cast<CServerRegion*>(GetFather());
	if(pRgn)
	{
		if(pRgn->GetSetup().bWarningFlag == true)
		{
			if( (GetCountry() % 2) != (pAttacker->GetCountry() % 2) ) //所属阵营不一致
			{
				long lCurTime = timeGetTime();
				if(!isKilled) // 是否被杀死
				{
					if( (long)(lCurTime- pRgn->GetLastWarningTime()) < (GlobalSetup::GetSetup()->lWarningDeltaTime*1000) )
					{
						return;
					}
					else
						pRgn->SetLastWarningTime(lCurTime);
				}

				CMessage msg(MSG_S2W_OTHER_WARNING);
				msg.Add((BYTE)GetCountry());
				msg.Add((BYTE)pAttacker->GetCountry());
				msg.Add((long)pRgn->GetID());
				msg.Add((long)GetTileX());
				msg.Add((long)GetTileY());
				msg.Add((BYTE)isKilled);
				msg.Add(GetName());
				msg.Add(pAttacker->GetName());
				msg.Send();
			}
		}
	}
}

//! 得到玩家CP
long CPlayer::GetActive(void)
{
	return m_Property.lCurrBaseActive + m_Property.lCurrExActive;
}

//! 重置玩家CP
void CPlayer::ResetActive(DWORD dwCurrTime)
{
	m_Property.lCurrBaseActive = (long)PlayerList::GetBaseCP(GetLevel());
//	if(GetGame()->GetStarDay() == m_Property.byConst)
//		m_Property.lCurrBaseActive += m_Property.lCurrBaseActive * 10 / 100;
	m_Property.lCurrBaseActive = min(99999, m_Property.lCurrBaseActive);
	m_Property.lLastResetActiveTime = dwCurrTime;

	// 更新属性数据
	GetGame()->UpdateSavePlayer(this, SAVE_DETAIL_ATTRPROPERTY);

	CMessage msg(MSG_S2C_PLAYER_ACTIVE_CHANGE);
	msg.Add(m_Property.lCurrBaseActive);
	msg.Add(m_Property.lCurrExActive);
	msg.SendToPlayer(GetExID(), false);
}

//! 添加扩展CP
bool CPlayer::AddExActive(long lAddNum)
{
	if(0 < lAddNum)
	{
		m_Property.lCurrExActive += lAddNum;

		// 更新属性数据
		GetGame()->UpdateSavePlayer(this, SAVE_DETAIL_ATTRPROPERTY);

		CMessage msg(MSG_S2C_PLAYER_ACTIVE_CHANGE);
		msg.Add(m_Property.lCurrBaseActive);
		msg.Add(m_Property.lCurrExActive);
		msg.SendToPlayer(GetExID(), false);

		return true;
	}

	return false;
}

//! 扣除CP
bool CPlayer::TakeActive(long lTakeNum)
{
	if(0 < lTakeNum && GetActive() >= lTakeNum)
	{
		if(m_Property.lCurrBaseActive >= lTakeNum)
		{
			m_Property.lCurrBaseActive -= lTakeNum;
		}
		else
		{
			m_Property.lCurrExActive -= lTakeNum - m_Property.lCurrBaseActive;
			m_Property.lCurrBaseActive = 0;
		}

		// 更新属性数据
		GetGame()->UpdateSavePlayer(this, SAVE_DETAIL_ATTRPROPERTY);

		CMessage msg(MSG_S2C_PLAYER_ACTIVE_CHANGE);
		msg.Add(m_Property.lCurrBaseActive);
		msg.Add(m_Property.lCurrExActive);
		msg.SendToPlayer(GetExID(), false);
		return true;
	}
	
	return false;
}

void CPlayer::SetCountryContribute(long l)
{
	m_Property.lCountryContribute = l;
}


//! 得到玩家结婚步骤
long CPlayer::GetMarriageStep(void)
{
	return m_Property.lMarriageStep;
}

//! 设置玩家结婚步骤
long CPlayer::SetMarriageStep(long lStep)
{
	long lOldValue = m_Property.lMarriageStep; 
	m_Property.lMarriageStep = lStep;

	//! 跨越显示值界限时，发送消息到客户端
	if(m_Property.lMarriageStep >= GlobalSetup::GetSetup()->lShowSpouseNameStep &&
		lOldValue < GlobalSetup::GetSetup()->lShowSpouseNameStep)
	{
		CMessage msg(MSG_S2C_PLAYER_SHOW_SPOUSENAME);
		msg.Add((BYTE)true);
		msg.SendToPlayer(GetExID(), false);
		UpdateTitle(TRUE);
	}
	else if (m_Property.lMarriageStep < GlobalSetup::GetSetup()->lShowSpouseNameStep &&
		lOldValue >= GlobalSetup::GetSetup()->lShowSpouseNameStep)
	{
		CMessage msg(MSG_S2C_PLAYER_SHOW_SPOUSENAME);
		msg.Add((BYTE)false);
		msg.SendToPlayer(GetExID(), false);
		UpdateTitle(TRUE);
	}

	return lStep;
}
//! 设置对象
bool CPlayer::SetSpouse(CPlayer *pPlayer)
{
	if (NULL != pPlayer)
		LinkmanSystem::GetInstance().AddSpouse(this, pPlayer);
	else
		LinkmanSystem::GetInstance().DelSpouse(this);

	return true;
}

//! 得到对象ID
const CGUID& CPlayer::GetSpouseGuid(void)
{
	const tagLinkman *pLinkman = LinkmanSystem::GetInstance().FindSpouseInfo(this);
	if(NULL != pLinkman)
		return pLinkman->guid;
	return NULL_GUID;
}
//! 得到对象名字
const char* CPlayer::GetSpouseName(void)
{
	const tagLinkman *pLinkman = LinkmanSystem::GetInstance().FindSpouseInfo(this);
	if(NULL != pLinkman)
		return pLinkman->szName;
	return NULL;
}
//注意:使用此函数,在物品叠加的情况下会删除原物品指针
BOOL CPlayer::AddGoodsByTest(tagTestResult* tgResult,CGoods* pGoods)
{
	if (tgResult==NULL || pGoods==NULL)
		return FALSE;
	CVolumeLimitGoodsContainer* pVolumeContainer=dynamic_cast<CVolumeLimitGoodsContainer*>(this->FindContainer(tgResult->dwContainerId));
	if(pVolumeContainer)
	{
		if(tgResult->eOperType == CS2CContainerObjectMove::OT_NEW_OBJECT)
		{
			//添加新物品							
			CS2CContainerObjectMove comMsg;
			if(pVolumeContainer->Add(tgResult->dwPos, pGoods, NULL, &comMsg) )
			{
				OnGetGoods(pGoods->GetBasePropertiesIndex(),pGoods);
				comMsg.SetOperation( CS2CContainerObjectMove::OT_NEW_OBJECT );
				comMsg.SetDestinationContainerExtendID( tgResult->dwContainerId );
				pGoods->SerializeForOldClient( &comMsg.GetObjectStream() );
				comMsg.Send( this->GetExID() );

			}
			else
				return FALSE;
		}
		else if(tgResult->eOperType == CS2CContainerObjectMove::OT_CHANGE_AMOUNT)						
		{
			//改变物品数量
			DWORD dwGoodsID=pGoods->GetBasePropertiesIndex();
			if(pVolumeContainer->Add(tgResult->dwPos, pGoods, NULL) )
			{
				OnGetGoods(dwGoodsID);
				CGoods* pExistedGoods=pVolumeContainer->GetGoods(tgResult->dwPos);
				if(pExistedGoods)
				{
					CS2CContainerObjectAmountChange coacMsg;
					coacMsg.SetSourceContainer( TYPE_PLAYER, this->GetExID(), 
						tgResult->dwPos );
					coacMsg.SetSourceContainerExtendID( tgResult->dwContainerId );
					coacMsg.SetObject( pExistedGoods->GetType(), pExistedGoods->GetExID() );
					coacMsg.SetObjectAmount( pExistedGoods->GetAmount() );
					coacMsg.Send( this->GetExID() );
				}	
				else
					return FALSE;
			}
			else
				return FALSE;
		}
		return TRUE;
	}
	else
		return FALSE;	
}

BOOL CPlayer::DelGoodsByTest(tagDelTestResult* tgResult)
{
	if (tgResult==NULL)
		return FALSE;
	CGoodsContainer *pContainer = this->FindContainer(tgResult->dwContainerId);
	if (pContainer)
	{
		CGoods* pGoods = dynamic_cast<CGoods*>( pContainer -> GetGoods(tgResult->dwPos) );
		if (pGoods==NULL)
			return FALSE;
		if(tgResult->eOperType == CS2CContainerObjectMove::OT_DELETE_OBJECT)
		{
			CS2CContainerObjectMove comMsg;
			if (pContainer->Remove(tgResult->dwPos,tgResult->dwNum,&comMsg))
			{
				OnLoseGoods(pGoods);
				comMsg.SetOperation( CS2CContainerObjectMove::OT_DELETE_OBJECT );
				comMsg.SetSourceContainerExtendID( tgResult->dwContainerId );
				comMsg.Send( this->GetExID() );
				CGoodsFactory::GarbageCollect(&pGoods,14);
			}
			else
				return FALSE;
		}
		else if(tgResult->eOperType == CS2CContainerObjectMove::OT_CHANGE_AMOUNT)						
		{
			CGoods* pExistedGoods=pContainer->GetGoods(tgResult->dwPos);
			if(pExistedGoods)
			{
				OnLoseGoods(pGoods);
				pExistedGoods->SetAmount(pExistedGoods->GetAmount() - tgResult->dwNum);
				CS2CContainerObjectAmountChange coacMsg;
				coacMsg.SetSourceContainer( TYPE_PLAYER, this->GetExID(), 
					tgResult->dwPos );
				coacMsg.SetSourceContainerExtendID( tgResult->dwContainerId );
				coacMsg.SetObject( pExistedGoods->GetType(), pExistedGoods->GetExID() );
				coacMsg.SetObjectAmount( pExistedGoods->GetAmount() );
				coacMsg.Send( this->GetExID() );
			}
			else
				return FALSE;
		}
		return TRUE;
	}
	else
		return FALSE;	
}


//! 得到元素属性
DWORD CPlayer::GetElementProperty(DWORD dwElementType, DWORD dwAttackType)
{
	if(MT_Max > dwElementType && AD_Max > dwAttackType)
		return m_arrElementProperty[dwElementType].uArrPro[dwAttackType];
	assert(false);
	return 0;
}
//! 设置元素属性
DWORD CPlayer::SetElementProperty(DWORD dwElementType, DWORD dwAttackType, long lNewValue)
{
	if(MT_Max > dwElementType && AD_Max > dwAttackType)
	{
		CMessage msg(MSG_S2C_PLAYER_ELEMENT_PROPERTY_CHANGE);
		msg.Add((BYTE)dwElementType);
		msg.Add((BYTE)dwAttackType);
		msg.Add(lNewValue);
		msg.SendToPlayer(GetExID(), false);
		return (m_arrElementProperty[dwElementType].uArrPro[dwAttackType] = lNewValue);
	}
	assert(false);
	return 0;
}
//! 添加元素属性
DWORD CPlayer::AddElementProperty(DWORD dwElementType, DWORD dwAttackType, long lAddValue)
{
	if(MT_Max > dwElementType && AD_Max > dwAttackType)
	{
		m_arrElementProperty[dwElementType].uArrPro[dwAttackType] += lAddValue;
		m_arrElementProperty[dwElementType].uArrPro[dwAttackType] = max(0, m_arrElementProperty[dwElementType].uArrPro[dwAttackType]);
		CMessage msg(MSG_S2C_PLAYER_ELEMENT_PROPERTY_CHANGE);
		msg.Add((BYTE)dwElementType);
		msg.Add((BYTE)dwAttackType);
		msg.Add(m_arrElementProperty[dwElementType].uArrPro[dwAttackType]);
		msg.SendToPlayer(GetExID(), false);
		return m_arrElementProperty[dwElementType].uArrPro[dwAttackType];
	}
	assert(false);
	return 0;
}

bool CPlayer::OnIncreaseHp( long &validHp, CMoveShape *firer )
{
    if( CMoveShape::OnIncreaseHp( validHp, firer ) || !firer )
    {
        return false;
    }
    m_tauntList->OnRescued( firer->GetExID(), firer->GetType(), validHp );
    return true;
}

void CPlayer::SendValidateMsgForCs()
{
    // send role information to chat server
    CMessage csmsg(MSG_S2CS_PLAYER_ENTER_GAME);
    csmsg.Add(GetExID());       // role's id
    csmsg.Add(GetSignCode());   // code used for validate
    csmsg.Add(GetName());       // role's name
    csmsg.Add(GetTeamID());     // role's team id
    csmsg.Add(GetFactionID());  // role's faction id
    csmsg.Add(GetRegionID());   // role's region id
    csmsg.Add(GetCountry());    // role's country id
    csmsg.Add(GetGame()->GetWSNumber());   // role's world server id
    csmsg.SendToChatServer();

    // send chat server information and sign code to game client
    CMessage gcmsg(MSG_S2C_OTHER_VALIDATE);
    gcmsg.Add(GetGame()->GetSetup()->strChatServerIp.c_str());
    gcmsg.Add(GetGame()->GetSetup()->dwChatServerPort4Gc);
    gcmsg.Add(GetSignCode());
    gcmsg.SendToPlayer(GetExID());
}