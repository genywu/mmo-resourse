#include "stdafx.h"
#include "Player.h"
#include "../../Input/KeyBoard.h"
#include "../../Net/Message.h"
#include "../../EngineExtend/TextureProjective.h"
#include "../../EngineExtend/DisplayModel.h"
#include "../../GameClient/ClientApp/Skills/SkillXml.h"
#include "../../GameClient/ClientApp/ClientRegion.h"
#include "../../GameClient/ClientApp/Other/CountryList.h"
#include "../../GameClient/ClientApp/Other/PicList.h"
#include "../../GameClient/ClientApp/MsgBoxSys/MsgEventManager.h"
#include "../../GameClient/GameControl.h"
#include "../../Public/Common/Public.h"
#include "../../Public/Common/SkillRelated.h"
#include "../../Public/Common/StudySkillCondition.h"
#include "../../Public/Common/DeputyOccu.h"
#include "Other/GlobeSetup.h"
#include "Other/AudioList.h"
#include "QuestSystem/QuestSystem.h"
#include "organizingsystem/Union.h"
#include "MainPlayerHand.h"
#include "Monster.h"
#include "PetSystem/Pet.h"
#include "Depot.h"
#include "../Game.h"
#include "../../UI/GamePage/Package/PackagePage.h"
#include "../../UI/Windows/ShopCityDef.h"
#include "Other/AutoSupply.h"

extern void UpdateEquipmentIcon(CPlayer::eEquipIndex index);

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define _DECODE_GENIUS_
typedef list<CPlayer::tagGoods>::iterator tagGoodsIter;

class Skill_Compare
{
public:
	bool operator ()(CPlayer::tagSkill& left, CPlayer::tagSkill& right) const
	{
		CPlayer *pPlayer = GetGame()->GetMainPlayer();
		if(pPlayer == NULL)
			return false;
		DWORD leftID = left.dwID;
		DWORD rightID = right.dwID;
		if(CSkillListXml::OccuJudgeStudy(leftID,pPlayer->GetOccupation()) && 
			!CSkillListXml::OccuJudgeStudy(rightID,pPlayer->GetOccupation()))
		{
			return true;
		}
		return false;
	}
};

string CPlayer::s_strOccupation[OCC_Max] = {""};			// 职业
string CPlayer::s_strConstellation[CONST_Pisces+1] = {""};	// 星座

BYTE CPlayer::BuyBackGoodsNum = 10;

// 初始化职业类型
void CPlayer::InitPlayerOccuType()
{
	for (int i = 0;i<OCC_Max;i++)
	{
        char strIndex[128];
        sprintf_s<128>(strIndex, "Player_%d", i+1);
        s_strOccupation[i] = AppFrame::GetText(strIndex);
	}
}
// 获得职业描述
const char* CPlayer::GetPlayerOccuType(long lOccuTypeIndex)
{
	if (lOccuTypeIndex>=0&&lOccuTypeIndex<OCC_Max)
	{
		return s_strOccupation[lOccuTypeIndex].c_str();
	}
	return "";
}

// 初始化星座
void CPlayer::InitPlayerConstType()
{
	for (int i = 0;i <= CONST_Pisces; i++)
    {
        char strIndex[128];
        sprintf_s<128>(strIndex,"Player_%d",i+200);
        s_strConstellation[i] = AppFrame::GetText(strIndex);
    }
}

// 获得指定星座的名字
const char* CPlayer::GetPlayerConstType(long lConstTypeIndex)
{
	if (lConstTypeIndex > CONST_No && lConstTypeIndex <= CONST_Pisces)
	{
		return s_strConstellation[lConstTypeIndex].c_str();
	}
	return "";
}

long	CPlayer::m_iEnemyPlayerLevelFirstQuotiety = 0;		//服务器敌国玩家等级系数1
long	CPlayer::m_iEnemyPlayerLevelSecondQuotiety = 0;	//服务器敌国玩家等级系数1
WORD	CPlayer::m_wPublicCoolTime = 0;			// 公用冷却时间（使用技能或消耗道具之后的基本冷却时间）

vector<CEmotionShower::tagEmotion> vecEmotions;

void LoadEmotionsConfig(char *filename)
{
	CRFile* prfile = rfOpen(filename);
	if( prfile )
	{	
		stringstream stream;
		prfile->ReadToStream(stream);
		rfClose(prfile);
		string s,action;
		int count=0,type,num;
		stream>>s>>count;
		vecEmotions.clear();
		for(int i=0;i<count;i++)
		{
			CEmotionShower::tagEmotion stEmotion;
			stream>>s;//跳过"*"号
			stream>>stEmotion.dwIndex;
			stream>>s;//跳过是否循环标志
			stream>>stEmotion.strName>>num;
			for(int j=0;j<num;j++)
			{
				stream>>action>>type;
				stEmotion.vecActions.push_back(MAKEFOURCC(action[0],action[1],action[2],action[3]));
				stEmotion.vecTypes.push_back(type);
			}
			stream>>stEmotion.dwType;
			stream>>stEmotion.strText>>stEmotion.strSound>>stEmotion.nSoundLoops;
			vecEmotions.push_back(stEmotion);
		}
	}
}

CPlayer::CPlayer(void):m_dwMoney(0),m_dwYuanBao(0),m_dwJiFen(0)
{
	m_bSelf = false;

	m_lPlayerShopPlugID = 0;
	m_dwEnergyHitEffectID = 0;
	//m_setPetID.clear();
	m_vecFactureEntrys.clear();

	m_pEmotionShower=new CEmotionShower(this);
	SetType(TYPE_PLAYER);

	SetUpgradeExp(0);
	m_lGrowTimer = 0;
	int i=0;
	for(;i<24;i++)
	{
		SetHotKey(i,0);
	}

	memset(&m_Property, 0, sizeof(tagPPro));
	memset(&m_FightProperty, 0, sizeof(tagPFPro));
	memset(&m_AppendProperty, 0, sizeof(tagPAPro));
	memset(&m_FriendshipCounty, 0, sizeof(tagFriendshipCounty));
	SetShowFashion(FASHION_TYPE_GREATCOAT | FASHION_TYPE_MANTEAU/* | FASHION_TYPE_HELM*/);

	//SetReAnk(0);
	for(i=0;i<EQUIP_SUM;i++)
	{
		m_Equip[i] = NULL;
		m_dwEquipIndex[i] = 0;
		m_btEquipLevel[i] = 0;
	}
	for (int i = 0;i<COTE_NUM;i++)
	{
		m_Cote[i] = new tagCote;
	}

    for(i=0; i<EX_EQUIP_NUM; i++)
    {
        m_eEquipGod[i] = NULL;
        m_eEquipLand[i] = NULL;
        m_eEquipHuman[i] = NULL;
    }
	//m_stHand.bChange=false;
	//m_stHand.lNum=0;
	//m_stHand.lPos=0;
	//m_stHand.pGoods=NULL;

	m_CoteBussinesPack = new tagCote;

	m_lFactionJob = 0;

    m_strShopName = AppFrame::GetText("Shop_26");

	m_strTraderName = "";
	//m_bTradeModeSign = false;
	m_mapPlayerCreditForZone.clear();
	m_mapPlayerInvestmentForPoint.clear();
	m_dwCollectExp = 0;
	m_dwCollectLevel = 0;
	m_dwFactureExp = 0;
	m_dwFactureLevel = 0;
	//m_wTradeTaskID = 0;
	//m_fTradeTaskCompleteDegree = 0.0f;

	m_lTeammateAmount = 1;
	m_bSinState = false;
	m_lHitNum = 0;
	m_dwLastHitTimer = 0;
	m_lGMLevel = 0;

	m_bTradeDisAble=true;
	m_bFriendDisAble=true;
	m_bCanSetHotKey = true;

	m_lAllotMade = 0;
	m_lLimtSkillID = 0;
	m_lLimtSkillLev = 0;
	m_bIsRecruitingTeam=false;
	m_strFactionName="";
	m_strFactionTitle="";
	m_bPlayerShopOpened = false;
	m_bContendState = false;
	m_lCityWarDiedState = false;
	m_dwToolIndex = 0;

	m_cRemainDays = -1;
	m_EnemyFactions.clear();
	m_CityWarEnemyFactions.clear();

	m_queBuyBackGoodsItem.clear();

	m_VecMedalOnPlayer.clear();

	InitNameValueMap();
	m_LEnergy.SetLocator(LOCATOR_LEFTHAND);
	m_REnergy.SetLocator(LOCATOR_RIGHTHAND);
	m_LeftHandWeapon.SetLocator(LOCATOR_LEFTHAND);
	m_RightHandWeapon.SetLocator(LOCATOR_RIGHTHAND);
	m_CollectTool.SetLocator(LOCATOR_RIGHTHAND);
	m_CollectTool.SetVisible(false);
	m_Wing.SetLocator(LOCATOR_BACK);
	m_HeadWear.SetLocator(LOCATOR_HEAD);
	m_bPlayEnergyEffect = FALSE;

	m_guMoneyID = CGUID::GUID_INVALID;
	m_guSilverMoneyID = CGUID::GUID_INVALID;
	m_guBaoShiID = CGUID::GUID_INVALID;
	m_guYuanBaoID = CGUID::GUID_INVALID;
	m_guJiFenID = CGUID::GUID_INVALID;

	m_dwLevelCurrentDisplayNum = 0;
	m_dwLevelMax = (CGlobeSetup::GetSetup()->dwExpTipTextLineMax == 0 ? 3 : CGlobeSetup::GetSetup()->dwExpTipTextLineMax);
	m_vecTipText.resize(m_dwLevelMax);

	for (size_t st = 0; st < m_vecTipText.size(); ++st)
		m_vecTipText[st].fAlpha = 0.0f;

	m_lOldContribute = 0;
	m_lCreditNum=0;
	m_bIsShowLoverName = false;
	//m_bIsHideInfo = false;
	//m_strHideInfoName = "匿名者";

	m_PetList.clear();
	m_ShowPets.clear();
	m_ActivePetGUID = CGUID::GUID_INVALID;
}

CPlayer::~CPlayer(void)
{
	m_mapPlayerCreditForZone.clear();
	m_mapPlayerInvestmentForPoint.clear();
	m_VecMedalOnPlayer.clear();
	//m_setPetID.clear();
	deque<CGoods*>::iterator GoodsIter = m_queBuyBackGoodsItem.begin();
	for (;GoodsIter!=m_queBuyBackGoodsItem.end();GoodsIter++)
	{
		SAFE_DELETE(*GoodsIter);
	}
	m_queBuyBackGoodsItem.clear();
	for(int i=0;i<EQUIP_SUM;i++)
	{
		if(i!=EQUIP_MEDAL1&&i!=EQUIP_MEDAL2&&i!=EQUIP_MEDAL3)  //勋章单独释放
		{
			SAFE_DELETE(m_Equip[i]);
		}
		else
			m_Equip[i] = NULL;
	}

	for(tagGoodsIter it=m_listGoods.begin(); it!=m_listGoods.end(); it++)
	{
		SAFE_DELETE(it->pGoods);
	}
	m_listGoods.clear();
	for (int i = 0;i<COTE_NUM;i++)
	{
		if (m_Cote[i]!=NULL)
		{
			if(m_Cote[i]->pGoods!=NULL)
			{
				if (!m_Cote[i]->m_listCoteGoods.empty())
				{
					tagGoodsIter it = m_Cote[i]->m_listCoteGoods.begin();
					for (;it!=m_Cote[i]->m_listCoteGoods.end();it++)
					{
						SAFE_DELETE(it->pGoods);
					}
					m_Cote[i]->m_listCoteGoods.clear();
				}
				SAFE_DELETE(m_Cote[i]->pGoods);
			}
			SAFE_DELETE(m_Cote[i]);
		}
	}

	if (m_CoteBussinesPack!=NULL)
	{
		if(m_CoteBussinesPack->pGoods!=NULL)
		{
			if (!m_CoteBussinesPack->m_listCoteGoods.empty())
			{
				tagGoodsIter it = m_CoteBussinesPack->m_listCoteGoods.begin();
				for (;it!=m_CoteBussinesPack->m_listCoteGoods.end();it++)
				{
					SAFE_DELETE(it->pGoods);
				}
				m_CoteBussinesPack->m_listCoteGoods.clear();
			}
			SAFE_DELETE(m_CoteBussinesPack->pGoods);
		}
		SAFE_DELETE(m_CoteBussinesPack);
	}



	//if(GetHand()->pGoods!=NULL)
	//{
	//	SAFE_DELETE(GetHand()->pGoods);
	//	ClearHand();
	//}
	SAFE_DELETE(m_pEmotionShower);
	RemoveAllPet();

	//m_vecTipText.clear();

	//vector<CPet*>::iterator petIter = m_PetList.begin();
	//for (; petIter!=m_PetList.end(); ++petIter)
	//{
	//	SAFE_DELETE(*petIter);
	//}
	//m_PetList.clear();
	//m_ShowPets.clear();
}



/*
* 功能: 初始化变量名到值得影射
* 摘要: -
* 参数: -
* 返回值: -
* 作者: -
* 创建日期: -
* 修改日志:
*	2008.09.16 - lpf
*		增加了"ShowCountry"的解码
*	2009.03.13 - lpf
*		增加了"bShowFashion"的解码,替换了原有的头盔解码
*/
void CPlayer::InitNameValueMap()
{
	CMoveShape::InitNameValueMap();
	tagPty st;// 0: byte 1:WORD 2:DWORD	3:float 4:long 5:short 6:bool

	st.lType=6; st.pbValue=&m_bContendState;			m_PropertyEnumToName[P_ATTR_CONTENDSTATE]="ContendState";					m_mapNameValue["ContendState"]=st;			
	st.lType=6; st.pbValue=&m_lCityWarDiedState;		m_PropertyEnumToName[P_ATTR_CITYWARDIEDSTATE]="CityWarDiedState"; 			m_mapNameValue["CityWarDiedState"]=st;		


	st.lType=4; st.pValue=&m_lType;;			m_PropertyEnumToName[P_ATTR_TYPE]="Type";				m_mapNameValue["Type"]=st;			
	st.lType=4; st.pValue=&m_lGraphicsID;;		m_PropertyEnumToName[P_ATTR_GRAPHICSID]="GraphicsID";	m_mapNameValue["GraphicsID"]=st;	
	st.lType=4; st.pValue=&m_lRegionID;			m_PropertyEnumToName[P_ATTR_REGIONID]="RegionID";		m_mapNameValue["RegionID"]=st;
	st.lType=3;	st.pfValue=&m_fPosX;			m_PropertyEnumToName[P_ATTR_TILEX]="TileX";				m_mapNameValue["TileX"]=st;		
	st.lType=3;	st.pfValue=&m_fPosY;			m_PropertyEnumToName[P_ATTR_TILEY]="TileY";				m_mapNameValue["TileY"]=st;
	st.lType=3;	st.pfValue=&m_fDir;				m_PropertyEnumToName[P_ATTR_DIR]="Dir";					m_mapNameValue["Dir"]=st;					
	st.lType=1;	st.pwValue=&m_wState;			m_PropertyEnumToName[P_ATTR_STATE]="State";				m_mapNameValue["State"]=st;			
	st.lType=1;	st.pwValue=&m_wAction;			m_PropertyEnumToName[P_ATTR_ACTION]="Action";			m_mapNameValue["Action"]=st;		


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*人物属性*/
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	st.lType=0;	st.plValue  = (BYTE*)(&m_Property.byOccu);			m_PropertyEnumToName[P_ATTR_OCCU]="Occu";					m_mapNameValue["Occu"]=st;		
	st.lType=0;	st.plValue  = (BYTE*)(&m_Property.byAssOccu);		m_PropertyEnumToName[P_ATTR_DOCCU]="DOccu";					m_mapNameValue["DOccu"]=st;
//	st.lType=0;	st.plValue  = (BYTE*)(&m_Property.byConst);			m_PropertyEnumToName[P_ATTR_CONST]="Const";					m_mapNameValue["Const"]=st;
	st.lType=0;	st.plValue	= &m_Property.bySex;					m_PropertyEnumToName[P_ATTR_SEX]="Sex";						m_mapNameValue["Sex"]=st;
//	st.lType=0;	st.plValue	= &m_Property.byClass;					m_PropertyEnumToName[P_ATTR_CLASS]="Class";					m_mapNameValue["Class"]=st;
	st.lType=0;	st.plValue	= &m_Property.byHairPic;				m_PropertyEnumToName[P_ATTR_HEADPIC]="HeadPic";				m_mapNameValue["HeadPic"]=st;				
	st.lType=0;	st.plValue	= &m_Property.byFacePic;;				m_PropertyEnumToName[P_ATTR_FACEPIC]="FacePic";				m_mapNameValue["FacePic"]=st;				
	st.lType=1;	st.pdwValue	= &m_Property.dwHp;						m_PropertyEnumToName[P_ATTR_HP]="Hp";						m_mapNameValue["Hp"]=st;				
	st.lType=1;	st.pwValue	= &m_Property.wMp;;						m_PropertyEnumToName[P_ATTR_MP]="Mp";						m_mapNameValue["Mp"]=st;				
	st.lType=1;	st.pwValue  = &m_Property.wEnergy;					m_PropertyEnumToName[P_ATTR_ENERGY]="Energy";				m_mapNameValue["Energy"]=st;			
	st.lType=1; st.pwValue  = &m_Property.wStamina;					m_PropertyEnumToName[P_ATTR_STAMINA]="Stamina";	;			m_mapNameValue["Stamina"]=st;				
	st.lType=0;	st.pwValue	= &m_Property.wLevel;					m_PropertyEnumToName[P_ATTR_LEVEL]="Level";					m_mapNameValue["Level"]=st;				
	st.lType=2;	st.pdwValue	= &m_Property.dwExp;					m_PropertyEnumToName[P_ATTR_EXP]="Exp";						m_mapNameValue["Exp"]=st;
	st.lType=2;	st.pdwValue	= &m_Property.dwExpMultiple;			m_PropertyEnumToName[P_ATTR_EXPMULTIPLE]="ExpMultiple";		m_mapNameValue["ExpMultiple"]=st;
	st.lType=2;	st.pdwValue	= &m_Property.dwPresentExp;				m_PropertyEnumToName[P_ATTR_PRESENTEXP]="PresentExp";		m_mapNameValue["PresentExp"]=st;
	/*st.lType=0; st.plValue	= &m_Property.byRankOfNobility;			m_PropertyEnumToName[P_ATTR_RANKOFNOBILITY]="RankOfNobility";m_mapNameValue["RankOfNobility"]=st;	
	st.lType=2;	st.pdwValue = &m_Property.dwRankOfNobCredit;		m_PropertyEnumToName[P_ATTR_RANKOFNOBCREDIT]="RankOfNobCredit";	m_mapNameValue["RankOfNobCredit"]=st;		
	st.lType=0; st.plValue	= &m_Property.byRankOfMercenary;		m_PropertyEnumToName[P_ATTR_RANKOFMERCENARY]="RankOfMercenary";	m_mapNameValue["RankOfMercenary"]=st;		
	st.lType=2;	st.pdwValue = &m_Property.dwMercenaryCredit;		m_PropertyEnumToName[P_ATTR_MERCENARYCREDIT]="MercenaryCredit";	m_mapNameValue["MercenaryCredit"]=st;		
	st.lType=2;	st.pdwValue = &m_Property.dwBatakCredit;			m_PropertyEnumToName[P_ATTR_BATAKCREDIT]="BatakCredit";		m_mapNameValue["BatakCredit"]=st;
	st.lType=2;	st.pdwValue = &m_Property.dwZanDoHunterCredit;		m_PropertyEnumToName[P_ATTR_ZANDOCREDIT]="ZanDoCredit";		m_mapNameValue["ZanDoCredit"]=st;
	st.lType=2;	st.pdwValue = &m_Property.dwMedalScores;			m_PropertyEnumToName[P_ATTR_MEDALSCORES]="MedalScores";		m_mapNameValue["MedalScores"]=st;*/
	st.lType=0;	st.plValue  = &m_Property.byCountry;				m_PropertyEnumToName[P_ATTR_COUNTRY]="Country";					m_mapNameValue["Country"]=st;				
	st.lType=4;	st.pValue = &m_Property.lCountryContribute;			m_PropertyEnumToName[P_ATTR_COUNTRYCONTRIBUTE]="CountryContribute";	m_mapNameValue["CountryContribute"]=st;	
	st.lType=2;	st.pdwValue = &m_Property.dwSpouseID;				m_PropertyEnumToName[P_ATTR_SPOUSEID]="SpouseId";			m_mapNameValue["SpouseId"]=st;			
	st.lType=2;	st.pdwValue = &m_Property.dwSpouseParam;			m_PropertyEnumToName[P_ATTR_SPOUSEPARAM]="SpouseParam";		m_mapNameValue["SpouseParam;"]=st;		
	st.lType=0; st.plValue  = &m_Property.byBusinessLevel;			m_PropertyEnumToName[P_ATTR_BUSINESSLEVEL]="BusinessLevel";	m_mapNameValue["BusinessLevel"]=st;		
	st.lType=2; st.pdwValue = &m_Property.dwBusinessExp;			m_PropertyEnumToName[P_ATTR_BUSINESSEXP]="BusinessExp";		m_mapNameValue["BusinessExp"]=st;
	/*st.lType=2; st.pdwValue = &m_Property.dwArtisanCredit;			m_PropertyEnumToName[P_ATTR_ARTISANCREDIT]="ArtisanCredit";	m_mapNameValue["ArtisanCredit"]=st;
	st.lType=0; st.plValue  = &m_Property.byArtisanLevel;			m_PropertyEnumToName[P_ATTR_ARTISANLEVEL]="ArtisanLevel";	m_mapNameValue["ArtisanLevel"]=st;		
	st.lType=2; st.pdwValue = &m_Property.dwArtisanExp;				m_PropertyEnumToName[P_ATTR_ARTISANEXP]="ArtisanExp";	;	m_mapNameValue["ArtisanExp"]=st;		*/
	st.lType=2;	st.pdwValue	= &m_Property.dwPkValue;				m_PropertyEnumToName[P_ATTR_PKVALUE]="PkValue";				m_mapNameValue["PkValue"]=st;				
	st.lType=2;	st.pdwValue	= &m_Property.dwPkCount;				m_PropertyEnumToName[P_ATTR_PKCOUNT]="PkCount";				m_mapNameValue["PkCount"]=st;			
	st.lType=2;	st.pdwValue	= &m_Property.dwPVPCount;				m_PropertyEnumToName[P_ATTR_PVPCOUNT]="PVPCount";			m_mapNameValue["PVPCount"]=st;			
	st.lType=2;	st.pdwValue	= &m_Property.dwPVPValue;;				m_PropertyEnumToName[P_ATTR_PVPVALUE]="PVPValue";			m_mapNameValue["PVPValue"]=st;			
	st.lType=0;	st.plValue  = &m_Property.byPKOnOff;  				m_PropertyEnumToName[P_ATTR_PKONOFF]="PkOnOff";				m_mapNameValue["PkOnOff"]=st;			
	st.lType=2; st.pdwValue = &m_Property.dwUpgradeExp;				m_PropertyEnumToName[P_ATTR_UPGRADEEXP]="UpgradeExp";		m_mapNameValue["UpgradeExp"]=st;
	st.lType=2; st.pdwValue = &m_Property.dwCurOccuUpgradeExp;		m_PropertyEnumToName[P_ATTR_CUROCCUUPGRADEEXP]="OccuUpgradeExp";m_mapNameValue["OccuUpgradeExp"]=st;
	st.lType=4; st.pValue = &m_Property.lCurPKCP;		            m_PropertyEnumToName[P_ATTR_CURPKCP]="CurPKCP";              m_mapNameValue["CurPKCP"]=st;
//	st.lType=2; st.pdwValue = &m_Property.dwChurchCredit;		    m_PropertyEnumToName[P_ATTR_CHURCHCREDIT]="ChurchCredit";   m_mapNameValue["ChurchCredit"]=st;

	st.lType=6; st.pbValue  = &m_Property.bShowCountry;				m_PropertyEnumToName[P_ATTR_SHOWCOUNTRY]="ShowCountry";		m_mapNameValue["ShowCountry"]=st;
	char tmp[1024];
	for(int i=0; i<24; ++i)
	{
		sprintf(tmp, "dwHotKey%d", i);
		st.lType=2;	st.pdwValue	= &m_Property.dwHotKey[0];	m_PropertyEnumToName[P_ATTR_HOTKEY0+i]=tmp;		m_mapNameValue[tmp]=st;			
	}
	//职业等级数据
	for(int i=0;i<OCC_Max;i++)
	{
		sprintf(tmp, "OccuLvl%d", i);
		st.lType=0;	st.plValue	= (BYTE*)(&m_Property.byOccuLvl[i]);	m_PropertyEnumToName[P_ATTR_OCCULVL0+i]=tmp;		m_mapNameValue[tmp]=st;
	}
	//职业经验数据
	for(int i=0;i<OCC_Max;i++)
	{
		sprintf(tmp, "OccuExp%d", i);
		st.lType=2;	st.pdwValue	= &m_Property.dwOccuExp[i];	m_PropertyEnumToName[P_ATTR_OCCUEXP0+i]=tmp;		m_mapNameValue[tmp]=st;
	}
	//职业SP数据
	for(int i=0;i<OCC_Max;i++)
	{
		sprintf(tmp, "OccuSP%d", i);
		st.lType=2;	st.pdwValue	= &m_Property.dwOccuSP[i];	m_PropertyEnumToName[P_ATTR_OCCUSP0+i]=tmp;		m_mapNameValue[tmp]=st;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*战斗属性*/ 
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	st.lType=5;	st.psValue	= &m_FightProperty.wShieldDef;		m_PropertyEnumToName[P_ATTR_SHIELD_DEF]="C_ShieldDef";		m_mapNameValue["C_ShieldDef"]=st;							
	st.lType=5;	st.psValue	= &m_FightProperty.wImmunity;		m_PropertyEnumToName[P_ATTR_IMMUNITY]="C_Immunity";			 m_mapNameValue["C_Immunity"]=st;
	st.lType=5;	st.psValue	= &m_FightProperty.wPierce;			m_PropertyEnumToName[P_ATTR_PIERCE]="C_Pierce";				 m_mapNameValue["C_Pierce"]=st;
	st.lType=5;	st.psValue	= &m_FightProperty.wLuck;			m_PropertyEnumToName[P_ATTR_LUCK]="C_Luck";					 m_mapNameValue["C_Luck"]=st;

	//HP恢复比例
	st.lType=5;	st.psValue	= &m_FightProperty.wHpRecoverRatio;	m_PropertyEnumToName[P_ATTR_HPRECOVERRATIO]="C_HpRecoverRatio"; m_mapNameValue["C_HpRecoverRatio"]=st;							

	st.lType=5;	st.psValue	= &m_FightProperty.wEnerRecoverSpeed;	m_PropertyEnumToName[P_ATTR_ENERRECOVERSPEED]="C_EnerRecoverSpeed";	 m_mapNameValue["C_EnerRecoverSpeed"]=st;							
	st.lType=5;	st.psValue	= &m_FightProperty.wFightEnerRecoverSpeed;m_PropertyEnumToName[P_ATTR_FIGHTENERRECOVERSPEED]="C_FightEnerRecoverSpeed";m_mapNameValue["C_FightEnerRecoverSpeed"]=st;				

	st.lType=4;	st.pValue	= &m_FightProperty.dwMaxHp;				m_PropertyEnumToName[P_ATTR_MAXHP]="C_MaxHp";			   	m_mapNameValue["C_MaxHp"]=st;			
	st.lType=5;	st.psValue	= &m_FightProperty.wMaxMp;				m_PropertyEnumToName[P_ATTR_MAXMP]="C_MaxMp";		m_mapNameValue["C_MaxMp"]=st;			
	st.lType=5;	st.psValue	= &m_FightProperty.wMaxStamina;			m_PropertyEnumToName[P_ATTR_MAXSTAMINA]="C_MaxStamina";		m_mapNameValue["C_MaxStamina"]=st;		
	st.lType=5;	st.psValue	= &m_FightProperty.wMaxEnergy;			m_PropertyEnumToName[P_ATTR_MAXENERGY]="C_MaxEnergy";		m_mapNameValue["C_MaxEnergy"]=st;		
	st.lType=5;	st.psValue	= &m_FightProperty.wHpRecoverSpeed;		m_PropertyEnumToName[P_ATTR_HPRECOVERSPEED]="C_HpRecoverSpeed";	m_mapNameValue["C_HpRecoverSpeed"]=st;	
	st.lType=5;	st.psValue	= &m_FightProperty.wMpRecoverSpeed;		m_PropertyEnumToName[P_ATTR_MPRECOVERSPEED]="C_MpRecoverSpeed";	m_mapNameValue["C_MpRecoverSpeed"]=st;	
	st.lType=5;	st.psValue	= &m_FightProperty.wStamRecoverSpeed;	m_PropertyEnumToName[P_ATTR_STAMRECOVERSPEED]="C_StamRecoverSpeed";	m_mapNameValue["C_StamRecoverSpeed"]=st;	
	/*st.lType=5;	st.psValue	= &m_FightProperty.wStrenth;			m_PropertyEnumToName[P_ATTR_STRENTH]="C_Strenth";			m_mapNameValue["C_Strenth"]=st;				
	st.lType=5;	st.psValue	= &m_FightProperty.wAgility;			m_PropertyEnumToName[P_ATTR_AGILITY]="C_Agility";			m_mapNameValue["C_Agility"]=st;				
	st.lType=5;	st.psValue	= &m_FightProperty.wConsti;				m_PropertyEnumToName[P_ATTR_CONSTI]="C_Consti";			m_mapNameValue["C_Consti"]=st;			
	st.lType=5;	st.psValue	= &m_FightProperty.wIntell;				m_PropertyEnumToName[P_ATTR_INTELL]="C_Intell";			m_mapNameValue["C_Intell"]=st;			
	st.lType=5;	st.psValue	= &m_FightProperty.wWisdom;				m_PropertyEnumToName[P_ATTR_WISDOM]="C_Wisdom";			m_mapNameValue["C_Wisdom"]=st;			
	st.lType=5;	st.psValue	= &m_FightProperty.wSpirit;				m_PropertyEnumToName[P_ATTR_SPIRIT]="C_Spirit";			m_mapNameValue["C_Spirit"]=st;		*/	
	st.lType=5;	st.psValue	= &m_FightProperty.wFightBlock;			m_PropertyEnumToName[P_ATTR_BLOCK]="C_Block";				m_mapNameValue["C_Block"]=st;			
	st.lType=5;	st.psValue	= &m_FightProperty.wParry;				m_PropertyEnumToName[P_ATTR_PARRY]="C_Parry";				m_mapNameValue["C_Parry"]=st;			
	st.lType=5;	st.psValue	= &m_FightProperty.wDodge;				m_PropertyEnumToName[P_ATTR_DODGE]="C_Dodge";				m_mapNameValue["C_Dodge"]=st;			

	st.lType=5;	st.psValue	= &m_FightProperty.wCharm;				m_PropertyEnumToName[P_ATTR_CHARM]="C_Charm";			m_mapNameValue["C_Charm"]=st;			
	st.lType=5;	st.psValue	= &m_FightProperty.wHit;				m_PropertyEnumToName[P_ATTR_HIT]="C_Hit";			m_mapNameValue["C_Hit"]=st;					
	st.lType=5;	st.psValue	= &m_FightProperty.wAbsorb;				m_PropertyEnumToName[P_ATTR_ABSORB]="C_Absorb";		m_mapNameValue["C_Absorb"]=st;			
	st.lType=4;	st.psValue	= &m_FightProperty.wDef;				m_PropertyEnumToName[P_ATTR_DEF]="C_Def";			m_mapNameValue["C_Def"]=st;					
	st.lType=4;	st.psValue	= &m_FightProperty.wMdef;				m_PropertyEnumToName[P_ATTR_MDEF]="C_Mdef";			m_mapNameValue["C_Mdef"]=st;				
	st.lType=4;	st.psValue	= &m_FightProperty.wCri;				m_PropertyEnumToName[P_ATTR_CRI]="C_Cri";			m_mapNameValue["C_Cri"]=st;					
	st.lType=5;	st.psValue	= &m_FightProperty.wMAtkSpeed;			m_PropertyEnumToName[P_ATTR_MATKSPEED]="C_MAtkSpeed";		m_mapNameValue["C_MAtkSpeed"]=st;		
	st.lType=5;	st.psValue	= &m_FightProperty.wBuffPower;			m_PropertyEnumToName[P_ATTR_BUFFPOWER]="SpeedC_BuffPower";m_mapNameValue["C_BuffPower"]=st;		
	st.lType=4;	st.psValue	= &m_FightProperty.wMinAtk;;			m_PropertyEnumToName[P_ATTR_MINATK]="C_MinAtk";		m_mapNameValue["C_MinAtk"]=st;			
	st.lType=4;	st.psValue	= &m_FightProperty.wMaxAtk;				m_PropertyEnumToName[P_ATTR_MAXATK]="C_MaxAtk";		m_mapNameValue["C_MaxAtk"]=st;			
	st.lType=4;	st.pValue	= &m_FightProperty.wMAtk;				m_PropertyEnumToName[P_ATTR_MATK]="C_MAtk";			m_mapNameValue["C_MAtk"]=st;				
	st.lType=4;	st.pValue	= &m_FightProperty.wMCri;				m_PropertyEnumToName[P_ATTR_MCRI]="C_MCri";			m_mapNameValue["C_MCri"]=st;				
	st.lType=5;	st.psValue	= &m_FightProperty.wBloodSuck;			m_PropertyEnumToName[P_ATTR_BLOODSUCK]="C_BloodSuck";		m_mapNameValue["C_BloodSuck"]=st;			
	st.lType=5;	st.psValue	= &m_FightProperty.wManaSuck;			m_PropertyEnumToName[P_ATTR_MANASUCK]="C_ManaSuck";		m_mapNameValue["C_ManaSuck"]=st;			
	st.lType=5;	st.psValue	= &m_FightProperty.wWillDef;			m_PropertyEnumToName[P_ATTR_WILLDEF]="C_WillDef";		m_mapNameValue["C_WillDef"]=st;				
	st.lType=5;	st.psValue	= &m_FightProperty.wConstiDef;			m_PropertyEnumToName[P_ATTR_CONSTIDEF]="C_ConstiDef";	m_mapNameValue["C_ConstiDef"]=st;		

	st.lType=5;st.psValue = &m_FightProperty.wDeBuffDef[DFT_Bled];	m_PropertyEnumToName[P_ATTR_DEBUFFDEF_BLED]="C_DeBuffDef0";	 m_mapNameValue["C_DeBuffDef0"]=st;		
	st.lType=5;st.psValue = &m_FightProperty.wDeBuffDef[DET_Dizzy];	m_PropertyEnumToName[P_ATTR_DEBUFFDEF_DIZZY]="C_DeBuffDef1";	 m_mapNameValue["C_DeBuffDef1"]=st;			
	st.lType=5;st.psValue = &m_FightProperty.wDeBuffDef[DET_Lull];	m_PropertyEnumToName[P_ATTR_DEBUFFDEF_LULL]="C_DeBuffDef2";	 m_mapNameValue["C_DeBuffDef2"]=st;		
	st.lType=5;st.psValue = &m_FightProperty.wDeBuffDef[DET_Hypnosis];m_PropertyEnumToName[P_ATTR_DEBUFFDEF_HYPNOSIS]="C_DeBuffDef3";	m_mapNameValue["C_DeBuffDef3"]=st;	
	st.lType=5;st.psValue = &m_FightProperty.wDeBuffDef[DET_Poison];	m_PropertyEnumToName[P_ATTR_DEBUFFDEF_POSION]="C_DeBuffDef4"; m_mapNameValue["C_DeBuffDef4"]=st;	
	st.lType=5;st.psValue = &m_FightProperty.wDeBuffDef[DET_Fix];		m_PropertyEnumToName[P_ATTR_DEBUFFDEF_FIX]="C_DeBuffDef5";	 m_mapNameValue["C_DeBuffDef5"]=st;		

	st.lType=5;st.psValue = &m_FightProperty.wElemDef[ET_Fire];			 m_PropertyEnumToName[P_ATTR_ELEMDEF_FIRE]="C_ElemDef0";  m_mapNameValue["C_ElemDef0"]=st;	
	st.lType=5;st.psValue = &m_FightProperty.wElemDef[ET_Water];		 m_PropertyEnumToName[P_ATTR_ELEMDEF_WATER]="C_ElemDef1";  m_mapNameValue["C_ElemDef1"]=st;	
	st.lType=5;st.psValue = &m_FightProperty.wElemDef[ET_Earth];		 m_PropertyEnumToName[P_ATTR_ELEMDEF_EARTH]="C_ElemDef2";  m_mapNameValue["C_ElemDef2"]=st;	
	st.lType=5;st.psValue = &m_FightProperty.wElemDef[ET_Wind];		 m_PropertyEnumToName[P_ATTR_ELEMDEF_WIND]="C_ElemDef3";  m_mapNameValue["C_ElemDef3"]=st;	
	st.lType=5;st.psValue = &m_FightProperty.wElemDef[ET_Dark];			 m_PropertyEnumToName[P_ATTR_ELEMDEF_DARK]="C_ElemDef4";	  m_mapNameValue["C_ElemDef4"]=st;	
	st.lType=5;st.psValue = &m_FightProperty.wElemDef[ET_Holy];			 m_PropertyEnumToName[P_ATTR_ELEMDEF_HOLY]="C_ElemDef5";	  m_mapNameValue["C_ElemDef5"]=st;	


	st.lType=5;st.psValue = &m_FightProperty.wClassDam[C_Human];			m_PropertyEnumToName[P_ATTR_CLASSDAM_HUMAN]="C_ClassDam0";   m_mapNameValue["C_ClassDam0"]=st;		
	st.lType=5;st.psValue = &m_FightProperty.wClassDam[C_Element];	;		m_PropertyEnumToName[P_ATTR_CLASSDAM_ELEMENT]="C_ClassDam1";   m_mapNameValue["C_ClassDam1"]=st;	
	st.lType=5;st.psValue = &m_FightProperty.wClassDam[C_Alien];			m_PropertyEnumToName[P_ATTR_CLASSDAM_ALIEN]="C_ClassDam2";   m_mapNameValue["C_ClassDam2"]=st;		
	st.lType=5;st.psValue = &m_FightProperty.wClassDam[C_Animal];			m_PropertyEnumToName[P_ATTR_CLASSDAM_ANIMAL]="C_ClassDam3";  m_mapNameValue["C_ClassDam3"]=st;		
	st.lType=5;st.psValue = &m_FightProperty.wClassDam[C_Undead];			m_PropertyEnumToName[P_ATTR_CLASSDAM_UNDEAD]="C_ClassDam4";  m_mapNameValue["C_ClassDam4"]=st;		
	st.lType=5;st.psValue = &m_FightProperty.wClassDam[C_Evil];			    m_PropertyEnumToName[P_ATTR_CLASSDAM_EVIL]="C_ClassDam5";   m_mapNameValue["C_ClassDam5"]=st;		
	st.lType=5;st.psValue = &m_FightProperty.wClassDam[C_Hexapods];		m_PropertyEnumToName[P_ATTR_CLASSDAM_HEXAPODS]="C_ClassDam6";  m_mapNameValue["C_ClassDam6"]=st;	

	st.lType=5;st.psValue = &m_FightProperty.wElemDam[ET_Fire];			    m_PropertyEnumToName[P_ATTR_ELEMDAM_FIRE]="C_ElemDam0";		m_mapNameValue["C_ElemDam0"]=st;		
	st.lType=5;st.psValue = &m_FightProperty.wElemDam[ET_Water];			m_PropertyEnumToName[P_ATTR_ELEMDAM_WATER]="C_ElemDam1";		m_mapNameValue["C_ElemDam1"]=st;	
	st.lType=5;st.psValue = &m_FightProperty.wElemDam[ET_Earth];			m_PropertyEnumToName[P_ATTR_ELEMDAM_EARTH]="C_ElemDam2";		m_mapNameValue["C_ElemDam2"]=st;	
	st.lType=5;st.psValue = &m_FightProperty.wElemDam[ET_Wind];	;		    m_PropertyEnumToName[P_ATTR_ELEMDAM_WIND]="C_ElemDam3";		m_mapNameValue["C_ElemDam3"]=st;	
	st.lType=5;st.psValue = &m_FightProperty.wElemDam[ET_Dark];		    	m_PropertyEnumToName[P_ATTR_ELEMDAM_DARK]="C_ElemDam4";		m_mapNameValue["C_ElemDam4"]=st;	
	st.lType=5;st.psValue = &m_FightProperty.wElemDam[ET_Holy];			    m_PropertyEnumToName[P_ATTR_ELEMDAM_HOLY]="C_ElemDam5";		m_mapNameValue["C_ElemDam5"]=st;	

	st.lType=5;st.psValue = &m_FightProperty.wElemAtk[ET_Fire];			    m_PropertyEnumToName[P_ATTR_ELEMATK_FIRE]="C_ElemAtk0";		m_mapNameValue["C_ElemAtk0"]=st;		
	st.lType=5;st.psValue = &m_FightProperty.wElemAtk[ET_Water];			m_PropertyEnumToName[P_ATTR_ELEMATK_WATER]="C_ElemAtk1";		m_mapNameValue["C_ElemAtk1"]=st;	
	st.lType=5;st.psValue = &m_FightProperty.wElemAtk[ET_Earth];			m_PropertyEnumToName[P_ATTR_ELEMATK_EARTH]="C_ElemAtk2";		m_mapNameValue["C_ElemAtk2"]=st;	
	st.lType=5;st.psValue = &m_FightProperty.wElemAtk[ET_Wind];			    m_PropertyEnumToName[P_ATTR_ELEMATK_WIND]="C_ElemAtk3";		m_mapNameValue["C_ElemAtk3"]=st;	
	st.lType=5;st.psValue = &m_FightProperty.wElemAtk[ET_Dark];			    m_PropertyEnumToName[P_ATTR_ELEMATK_DARK]="C_ElemAtk4";		m_mapNameValue["C_ElemAtk4"]=st;	
	st.lType=5;st.psValue = &m_FightProperty.wElemAtk[ET_Holy];			    m_PropertyEnumToName[P_ATTR_ELEMATK_HOLY]="C_ElemAtk5";		m_mapNameValue["C_ElemAtk5"]=st;	

	st.lType=5;st.psValue = &m_FightProperty.wCriDef;		m_PropertyEnumToName[P_ATTR_CRIDEF]="C_CriDef";								m_mapNameValue["C_CriDef"]=st;	
	st.lType=5;st.psValue = &m_FightProperty.wMCriDef;		m_PropertyEnumToName[P_ATTR_MCRIDEF]="C_MCriDef";							m_mapNameValue["C_MCriDef"]=st;	
	st.lType=5;st.psValue = &m_FightProperty.wCriDamage;	m_PropertyEnumToName[P_ATTR_CRIDAMAGE]="C_CriDamage";						m_mapNameValue["C_CriDamage"]=st;	
	st.lType=5;st.psValue = &m_FightProperty.wMCriDamage;	m_PropertyEnumToName[P_ATTR_MCRIDAMAGE]="C_MCriDamage";						m_mapNameValue["C_MCriDamage"]=st;	

	st.lType=4; st.pValue = &m_FightProperty.lMaxPKCP;		            m_PropertyEnumToName[P_ATTR_MAXPKCP]="C_MaxPKCP";              m_mapNameValue["C_MaxPKCP"]=st;


	st.lType=2;	st.pdwValue=&m_dwFactureExp;				m_PropertyEnumToName[P_ATTR_FACTUREEXP]="FactureExp";						m_mapNameValue["FactureExp"]=st;						//	制作经验
	st.lType=2;	st.pdwValue=&m_dwCollectExp;				m_PropertyEnumToName[P_ATTR_COLLECTIONEXP]="CollectionExp";					m_mapNameValue["CollectionExp"]=st;					//	采集经验
	st.lType=2;	st.pdwValue=&m_dwFactureLevel;				m_PropertyEnumToName[P_ATTR_FACTUREGRADE]="FactureGrade";					m_mapNameValue["FactureGrade"]=st;					//	制作等级
	st.lType=2;	st.pdwValue=&m_dwCollectLevel;				m_PropertyEnumToName[P_ATTR_COLLECTIONGRADE]="CollectionGrade";				m_mapNameValue["CollectionGrade"]=st;				//	采集等级

	st.lType=0;	st.plValue  = &m_AppendProperty.bShowFashion;		m_PropertyEnumToName[P_ATTR_DISPLAYHEADPIECE]="ShowFashion";m_mapNameValue["ShowFashion"]=st;
	//GM隐蔽
	st.lType=6;	st.pbValue  = &m_bGMConcealment;		m_PropertyEnumToName[P_ATTR_CONCEALMENT]="GMConcealment";m_mapNameValue["GMConcealment"]=st;

	// 王峰：魔化属性
	st.lType=4;st.pValue = &m_arrElementProperty[ME_Earth].uArrPro[AD_Attack];	m_PropertyEnumToName[P_EARTH_ATTACK] = "Earth_Attack";	m_mapNameValue["Earth_Attack"]=st;
	st.lType=4;st.pValue = &m_arrElementProperty[ME_Earth].uArrPro[AD_Defend];	m_PropertyEnumToName[P_EARTH_DEFEND] = "Earth_Defend";	m_mapNameValue["Earth_Defend"]=st;
	st.lType=4;st.pValue = &m_arrElementProperty[ME_Water].uArrPro[AD_Attack];	m_PropertyEnumToName[P_WATER_ATTACK] = "Water_Attack";	m_mapNameValue["Water_Attack"]=st;
	st.lType=4;st.pValue = &m_arrElementProperty[ME_Water].uArrPro[AD_Defend];	m_PropertyEnumToName[P_WATER_DEFEND] = "Water_Defend";	m_mapNameValue["Water_Defend"]=st;
	st.lType=4;st.pValue = &m_arrElementProperty[ME_Fire].uArrPro[AD_Attack];	m_PropertyEnumToName[P_FIRE_ATTACK] = "Fire_Attack";		m_mapNameValue["Fire_Attack"]=st;
	st.lType=4;st.pValue = &m_arrElementProperty[ME_Fire].uArrPro[AD_Defend];	m_PropertyEnumToName[P_FIRE_DEFEND] = "Fire_Defend";		m_mapNameValue["Fire_Defend"]=st;
	st.lType=4;st.pValue = &m_arrElementProperty[ME_Wind].uArrPro[AD_Attack];	m_PropertyEnumToName[P_WIND_ATTACK] = "Wind_Attack";		m_mapNameValue["Wind_Attack"]=st;
	st.lType=4;st.pValue = &m_arrElementProperty[ME_Wind].uArrPro[AD_Defend];	m_PropertyEnumToName[P_WIND_DEFEND] = "Wind_Defend";		m_mapNameValue["Wind_Defend"]=st;
	st.lType=4;st.pValue = &m_arrElementProperty[ME_Bright].uArrPro[AD_Attack];	m_PropertyEnumToName[P_BRIGHT_ATTACK] = "Bright_Attack";	m_mapNameValue["Bright_Attack"]=st;
	st.lType=4;st.pValue = &m_arrElementProperty[ME_Bright].uArrPro[AD_Defend];	m_PropertyEnumToName[P_BRIGHT_DEFEND] = "Bright_Defend";	m_mapNameValue["Bright_Defend"]=st;
	st.lType=4;st.pValue = &m_arrElementProperty[ME_Dark].uArrPro[AD_Attack];	m_PropertyEnumToName[P_DARK_ATTACK] = "Dark_Attack";		m_mapNameValue["Dark_Attack"]=st;
	st.lType=4;st.pValue = &m_arrElementProperty[ME_Dark].uArrPro[AD_Defend];	m_PropertyEnumToName[P_DARK_DEFEND] = "Dark_Defend";		m_mapNameValue["Dark_Defend"]=st;

	st.lType=4;st.pValue = &m_arrElementProperty[ME_Earth].uArrPro[AD_AttackXiang];		m_PropertyEnumToName[P_EARTH_ATTACKXIANG] = "Earth_AttackXiang";	m_mapNameValue["Earth_AttackXiang"]=st;
	st.lType=4;st.pValue = &m_arrElementProperty[ME_Earth].uArrPro[AD_DefendXiang];		m_PropertyEnumToName[P_EARTH_DEFENDXIANG] = "Earth_DefendXiang";	m_mapNameValue["Earth_DefendXiang"]=st;
	st.lType=4;st.pValue = &m_arrElementProperty[ME_Water].uArrPro[AD_AttackXiang];		m_PropertyEnumToName[P_WATER_ATTACKXIANG] = "Water_AttackXiang";	m_mapNameValue["Water_AttackXiang"]=st;
	st.lType=4;st.pValue = &m_arrElementProperty[ME_Water].uArrPro[AD_DefendXiang];		m_PropertyEnumToName[P_WATER_DEFENDXIANG] = "Water_DefendXiang";	m_mapNameValue["Water_DefendXiang"]=st;
	st.lType=4;st.pValue = &m_arrElementProperty[ME_Fire].uArrPro[AD_AttackXiang];		m_PropertyEnumToName[P_FIRE_ATTACKXIANG] = "Fire_AttackXiang";	m_mapNameValue["Fire_AttackXiang"]=st;
	st.lType=4;st.pValue = &m_arrElementProperty[ME_Fire].uArrPro[AD_DefendXiang];		m_PropertyEnumToName[P_FIRE_DEFENDXIANG] = "Fire_DefendXiang";	m_mapNameValue["Fire_DefendXiang"]=st;
	st.lType=4;st.pValue = &m_arrElementProperty[ME_Wind].uArrPro[AD_AttackXiang];		m_PropertyEnumToName[P_WIND_ATTACKXIANG] = "Wind_AttackXiang";	m_mapNameValue["Wind_AttackXiang"]=st;
	st.lType=4;st.pValue = &m_arrElementProperty[ME_Wind].uArrPro[AD_DefendXiang];		m_PropertyEnumToName[P_WIND_DEFENDXIANG] = "Wind_DefendXiang";	m_mapNameValue["Wind_DefendXiang"]=st;
	st.lType=4;st.pValue = &m_arrElementProperty[ME_Bright].uArrPro[AD_AttackXiang];	m_PropertyEnumToName[P_BRIGHT_ATTACKXIANG] = "Bright_AttackXiang";	m_mapNameValue["Bright_AttackXiang"]=st;
	st.lType=4;st.pValue = &m_arrElementProperty[ME_Bright].uArrPro[AD_DefendXiang];	m_PropertyEnumToName[P_BRIGHT_DEFENDXIANG] = "Bright_DefendXiang";	m_mapNameValue["Bright_DefendXiang"]=st;
	st.lType=4;st.pValue = &m_arrElementProperty[ME_Dark].uArrPro[AD_AttackXiang];		m_PropertyEnumToName[P_DARK_ATTACKXIANG] = "Dark_AttackXiang";	m_mapNameValue["Dark_AttackXiang"]=st;
	st.lType=4;st.pValue = &m_arrElementProperty[ME_Dark].uArrPro[AD_DefendXiang];		m_PropertyEnumToName[P_DARK_DEFENDXIANG] = "Dark_DefendXiang";	m_mapNameValue["Dark_DefendXiang"]=st;
}

CGoods* CPlayer::GetEquip(int index)
{
	if(index<0 || index>=EQUIP_SUM )
		return NULL;
	return m_Equip[index];
}

void CPlayer::SetEquip(int index,CGoods *pGood)
{
	if(index<0 || index>=EQUIP_SUM)
		return ;
	m_Equip[index]=pGood;
}

// 得到某个Player能够穿的防具类型
eEquipLimitType CPlayer::QueryPlayerEquipLimitType(CPlayer* pPlayer)
{
	eEquipLimitType eEquipType = ELT_UNKNOWN;
	if (pPlayer!=NULL)
	{
		eOccupation eOccu = pPlayer->GetOccupation();
		switch(eOccu)
		{
		case OCC_Mystic:		// 法师
		case OCC_Warlock:		// 术士
		case OCC_Cleric:		// 牧师
		case OCC_Sorcerer:		// 巫师
		case OCC_Necromancer:	// 亡灵法师
		case OCC_Bishop:		// 主教
		case OCC_Prophet:		// 贤者
			eEquipType = ELT_LIGHT;
			break;
		case OCC_Rogue:			// 行者
		case OCC_Scout:			// 斥候
		case OCC_Craftsman:		// 艺人
		case OCC_Ranger:		// 巡守
		case OCC_Assassin:		// 刺客
		case OCC_BladeDancer:	// 剑舞者
		case OCC_Bard:			// 吟游诗人
			eEquipType = ELT_MID;
			break;
		case OCC_Fighter:		// 战士
		case OCC_Knight:		// 骑士
		case OCC_Warrior:		// 斗士
		case OCC_Paladin:		// 圣骑士
		case OCC_DarkKnight:	// 暗骑士
		case OCC_Gladiator:		// 剑士
		case OCC_Berserker:		// 狂战士
			eEquipType = ELT_HEAVY;
			break;
		}
	}
	return eEquipType;
}

//获取当前背包的空格子数量
long CPlayer::GetEmptyGridNum()
{
	// 求背包总的容量
	DWORD dwTotlePackageDimen = 0;		
	// 求背包中物品的总数
	DWORD dwTotleGoodsNum = 0;
	for (int i = 0;i<PACK_NUM;i++)
	{
		if (i==0)
		{
			dwTotlePackageDimen+=GetPacketDimen(PEI_PACKET);
		}
        else
			dwTotlePackageDimen+=GetPacketDimen(PEI_SUBPACK+i);

		list<CPlayer::tagGoods>*pGoodsList = GetGoodsList(i);
		if (pGoodsList!=NULL)
		{
			dwTotleGoodsNum += (DWORD)pGoodsList->size();
		}
	}
	return dwTotlePackageDimen-dwTotleGoodsNum;
}

//bool CPlayer::CanEquip(DWORD dwGoodsIndex)
//{
//	if (dwGoodsIndex!=0)
//	{
//		DWORD dwGoodsType = CGoodsList::GetProperty(dwGoodsIndex)->BaseProperty.dwType;
//		if (dwGoodsType==GBT_EQUIPMENT)
//		{
//			//装备各种限制判定
//			DWORD dwVal=0;
//			//等级限制
//			dwVal = CGoodsList::GetEffectVal( dwGoodsIndex,GAP_ROLE_LEVEL_LIMIT, 1 );
//			if(dwVal!=0 && dwVal>GetLevel())
//				return false;
//
//			//技能限制
//			dwVal = CGoodsList::GetEffectVal( dwGoodsIndex,GAP_SKILL_LIMIT,1);
//			if(dwVal!=0)
//			{
//				DWORD dwLev=CheckSkill(dwVal);
//				if(dwLev<=0)
//					return false;
//			}
//
//			//防具类型限制
//			dwVal = CGoodsList::GetEffectVal( dwGoodsIndex, GAP_GUARD_EQUIP_CATEGORY,1);
//			if(dwVal!=0)
//			{
//				eEquipLimitType elt = QueryPlayerEquipLimitType(this);
//				if(elt!=dwVal)
//					return false;
//			}
//
//			//职业限制
//			if (CGoodsList::HaveEffect(dwGoodsIndex, GAP_REQUIRE_OCCUPATION))
//			{
//				dwVal = CGoodsList::GetEffectVal( dwGoodsIndex, GAP_REQUIRE_OCCUPATION, 1 );
//				long lVal = 0;
//				int i = 0;
//				for (;i<OCC_Max;i++)
//				{
//					lVal = (dwVal>>i)&0x1; 
//					if (lVal!=0)
//					{
//						if (GetOccupation()==(eOccupation)i)
//						{
//							break;
//						}
//					}
//				}
//				if(i==OCC_Max)
//					return false;
//			}
//
//
//			//性别限制
//			dwVal = CGoodsList::GetEffectVal( dwGoodsIndex, GAP_REQUIRE_GENDER, 1 );
//			if(dwVal!=0 && dwVal!=GetSex()+1)
//				return false;
//
//			// （主手武器、副手武器、骑乘的一些特殊判定）。。。。
//			switch(dwGoodsType)
//			{
//			case GBT_E_WEAPON:
//				{
//					long eWT = CGoodsList::GetEffectVal( dwGoodsIndex, GAP_WEAPON_CATEGORY,1);
//				//	CGoods* pWeapon2 = GetEquip(EQUIP_WEAPON2);
//					CGoods* pWeapon1 = GetEquip(EQUIP_WEAPON);
//					if (pWeapon1!=NULL)
//					{
//						long eWP = pWeapon1->GetEffectVal( GAP_WEAPON_CATEGORY,1);
//					//	long eWP2 = pWeapon2->GetEffectVal(CGoods::GAP_WEAPON_CATEGORY,1);
//						if ((eWT!=WT_SINGLE_SWORD&&eWT!=WT_SINGLE_HAMMER&&eWT!=WT_SINGLE_KNIFE)
//							&&GetEmptyGridNum()<2)
//						{
//							return false;
//						}
//					}
//					//switch(eWT)
//					//{
//					//case WT_SINGLE_SWORD:	//单手剑
//					//case WT_SINGLE_HAMMER:	//单手锤
//					//case WT_SINGLE_KNIFE:	//匕首
//					//case WT_BOW:
//					//	{
//
//					//	}
//					//    break;
//					//case WT_WAND:			//法杖
//					//case WT_HEAVY_SWORD:	//双手剑
//					//case WT_HEAVY_AX:		//双手斧
//					//case WT_DOUBLE_SWORD:	//双剑
//					//case WT_FIST:			//拳刃
//					//	{
//					//		if (pWeapon2!=NULL&&
//					//		   (pWeapon2->GetProperty()->dwType==GT_WEAPON2||pWeapon2->GetProperty()->dwType==GT_ARROW))
//					//		{
//					//			return false;
//					//		}
//					//	}
//					//    break;
//					//}
//				}
//				break;
//			/*case GT_WEAPON2:
//				{
//					CGoods* pWeapon = GetEquip(EQUIP_WEAPON);
//					if (pWeapon!=NULL)
//					{
//						long eWT = pWeapon->GetEffectVal(CGoods::GAP_WEAPON_CATEGORY,1);
//						if (eWT!=WT_SINGLE_SWORD&&eWT!=WT_SINGLE_HAMMER&&eWT!=WT_SINGLE_KNIFE)
//						{
//							return false;
//						}
//					}
//				}
//				break;
//			case GT_ARROW:
//				{
//					CGoods* pWeapon = GetEquip(EQUIP_WEAPON);
//					if (pWeapon!=NULL)
//					{
//						long eWT = pWeapon->GetEffectVal(CGoods::GAP_WEAPON_CATEGORY,1);
//						if (eWT!=WT_BOW)
//						{
//							return false;
//						}
//					}
//				}
//				break;*/
//				//case GT_HORSE:
//				//	{
//				//		// 预留。。暂时还没有限制
//				//	}
//				//	break;
//			}
//		}
//		return true;
//	}
//	return false;
//}

// 检查此物品当前是否能装备
bool CPlayer::CanEquip(CGoods* pGoods)
{
	if (pGoods!=NULL)
	{
		DWORD dwGoodsType = pGoods->GetProperty()->dwType;
		if (dwGoodsType==GBT_EQUIPMENT)
		{
			//装备各种限制判定
			DWORD dwVal=0;
			//等级限制
			dwVal = pGoods ->GetEffectVal( GAP_ROLE_LEVEL_LIMIT, 1 );
			if(dwVal!=0 && dwVal>GetLevel())
				return false;

			//技能限制
			dwVal=pGoods->GetEffectVal(GAP_SKILL_LIMIT,1);
			if(dwVal!=0)
			{
				DWORD dwLev=CheckSkill(dwVal);
				if(dwLev<=0)
					return false;
			}

			//防具类型限制
			dwVal = pGoods->GetEffectVal(GAP_GUARD_EQUIP_CATEGORY,1);
			if(dwVal!=0)
			{
				eEquipLimitType elt = QueryPlayerEquipLimitType(this);
				if(elt!=dwVal)
					return false;
			}

			//职业限制
			if (pGoods->HasEffect(GAP_REQUIRE_OCCUPATION))
			{
				dwVal=pGoods -> GetEffectVal( GAP_REQUIRE_OCCUPATION, 1 );
				long lVal = 0;
				int i = 0;
				for (;i<OCC_Max;i++)
				{
					lVal = (dwVal>>i)&0x1; 
					if (lVal!=0)
					{
						if (GetOccupation()==(eOccupation)i)
						{
							break;
						}
					}
				}
				if(i==OCC_Max)
					return false;
			}

			//性别限制
			dwVal=pGoods -> GetEffectVal( GAP_REQUIRE_GENDER, 1 );
			if(dwVal!=0 && dwVal!=GetSex()+1)
				return false;

			// （主手武器、副手武器、骑乘的一些特殊判定）。。。。
			ulong equipType = pGoods->GetEquipType();
			switch(equipType)
			{
			case EC_E_WEAPON:
				{
					long eWT = pGoods->GetEffectVal(GAP_WEAPON_CATEGORY,1);
					CGoods* pWeapon1 = GetEquip(EQUIP_WEAPON);
					if (pWeapon1!=NULL)
					{
						long eWP = pWeapon1->GetEffectVal(GAP_WEAPON_CATEGORY,1);
						if ((eWT!=WT_SINGLE_SWORD&&eWT!=WT_SINGLE_HAMMER&&eWT!=WT_SINGLE_KNIFE)
							&&GetEmptyGridNum()<2)
						{
							return false;
						}
					}
					//switch(eWT)
					//{
					//case WT_SINGLE_SWORD:	//单手剑
					//case WT_SINGLE_HAMMER:	//单手锤
					//case WT_SINGLE_KNIFE:	//匕首
					//case WT_BOW:			//弓
					//	{

					//	}
					//    break;
					//case WT_WAND:			//法杖
					//case WT_HEAVY_SWORD:	//双手剑
					//case WT_HEAVY_AX:		//双手斧
					//case WT_DOUBLE_SWORD:	//双剑
					//case WT_FIST:			//拳刃
					//	{
					//		if (pWeapon2!=NULL&&
					//		   (pWeapon2->GetProperty()->dwType==GT_WEAPON2||pWeapon2->GetProperty()->dwType==GT_ARROW))
					//		{
					//			return false;
					//		}
					//	}
					//    break;
					//}
				}
				break;
			/*case GT_WEAPON2:
				{
					CGoods* pWeapon = GetEquip(EQUIP_WEAPON);
					if (pWeapon!=NULL)
					{
						long eWT = pWeapon->GetEffectVal(CGoods::GAP_WEAPON_CATEGORY,1);
						if (eWT!=WT_SINGLE_SWORD&&eWT!=WT_SINGLE_HAMMER&&eWT!=WT_SINGLE_KNIFE)
						{
							return false;
						}
					}
				}
				break;
			case GT_ARROW:
				{
					CGoods* pWeapon = GetEquip(EQUIP_WEAPON);
					if (pWeapon!=NULL)
					{
						long eWT = pWeapon->GetEffectVal(CGoods::GAP_WEAPON_CATEGORY,1);
						if (eWT!=WT_BOW)
						{
							return false;
						}
					}
				}
				break;*/
				//case GT_HORSE:
				//	{
				//		// 预留。。暂时还没有限制
				//	}
				//	break;
			}
		}
		return true;
	}
	return false;
}

//void CPlayer::SetHand(bool bChanged,WORD lNum,DWORD lPos,CGoods *pGood,DWORD lType)
//{
//	m_stHand.bChange=bChanged;
//	m_stHand.lNum=lNum;
//	m_stHand.lPos=lPos;
//	m_stHand.pGoods=pGood;
//	m_stHand.lType=lType;
//}

CGoods* CPlayer::GetGoodsByGUID(const CGUID& guID,int *pPos,long* lExtendID)
{
	CGoods *pGoods=NULL;

	//if(m_stHand.pGoods && m_stHand.pGoods->GetExID() == guID)
	//	return m_stHand.pGoods;


	//在装备中查找
	for(int i=0;i<CPlayer::EQUIP_SUM;i++)
	{
		pGoods = GetEquip(i);
		if(pGoods)
		{
			if(pGoods->GetExID() == guID)
			{
				if(pPos!=NULL) pPos[0]=i;
				if (lExtendID!=NULL) (*lExtendID) = PEI_EQUIPMENT;
				return pGoods;
			}
		}
	}
	//在主背包中查找
	list<tagGoods>* pGoodList = GetGoodsList();
	if (pGoodList)
	{
		tagGoodsIter GoodsIter=pGoodList->begin();
		for(;GoodsIter!=pGoodList->end();GoodsIter++)
		{
			pGoods = GoodsIter->pGoods;
			if(pGoods && pGoods->GetExID() == guID)
			{
				if(pPos!=NULL) pPos[0]=GoodsIter->lPos;
				if (lExtendID!=NULL) (*lExtendID) = PEI_PACKET;
				return pGoods;
			}
		}
	}
	//查询子背包
	for (int i = 0;i<COTE_NUM;i++)
	{	
		//在子背包栏位上查找
		pGoods = GetSubPGoods(i);
		if (pGoods&&pGoods->GetExID()==guID)
		{
			if(pPos!=NULL) pPos[0] = PEI_PACK1+i;
			if (lExtendID!=NULL) (*lExtendID) = PEI_SUBPACK;
			return pGoods;
		}
		//在子背包中查找
		pGoodList = GetSubGoodslist(i);
		if (pGoodList!=NULL)
		{
			tagGoodsIter GoodsIter=pGoodList->begin();
			for(;GoodsIter!=pGoodList->end();GoodsIter++)
			{
				pGoods = GoodsIter->pGoods;
				if(pGoods && pGoods->GetExID() == guID)
				{
					if(pPos!=NULL) pPos[0]=GoodsIter->lPos;
					if (lExtendID!=NULL) (*lExtendID) = (PEI_SUBPACK+i+1);;
					return pGoods;
				}
			}
		}
	}

	// 王峰：在仓库中查找
	CPlayer::tagGoods	tagGoodsInDepot = CGCMana::Inst()->GetGoods( guID );
	if( tagGoodsInDepot.pGoods )
	{
		if( lExtendID )
			*lExtendID = tagGoodsInDepot.lType;

		if( pPos )
			*pPos = tagGoodsInDepot.lPos;

		return tagGoodsInDepot.pGoods;
	}

	/// 跑墒背包栏位上查找
	if (m_CoteBussinesPack!=NULL&&m_CoteBussinesPack->pGoods!=NULL)
	{
		if (m_CoteBussinesPack->pGoods->GetExID()==guID)
		{
			return m_CoteBussinesPack->pGoods;
		}
	}

	///在跑墒背包中查找
	pGoodList = NULL;
	if (m_CoteBussinesPack!=NULL)
	{
		pGoodList = &m_CoteBussinesPack->m_listCoteGoods;
	}
	if (pGoodList)
	{
		tagGoodsIter GoodsIter=pGoodList->begin();
		for(;GoodsIter!=pGoodList->end();GoodsIter++)
		{
			pGoods = GoodsIter->pGoods;
			if(pGoods && pGoods->GetExID() == guID)
			{
				if(pPos!=NULL) pPos[0]=GoodsIter->lPos;
				if (lExtendID!=NULL) (*lExtendID) = PEI_BUSINESSPACK;
				return pGoods;
			}
		}
	}

	return NULL;
}



long CPlayer::CheckSpace(CGoods* pGoods)
{
	/*int w=pGoods->GetBaseProperty()->lX_Size,h=pGoods->GetBaseProperty()->lY_Size;
	int w=1,h=1;
	for(long i=0;i<48;i++)
	{
	if(GetGame()->GetInterface()->GetItemsPage()->CheckDataIsEmpty(i,w,h))
	{
	return i;
	}
	}*/
	return -1;
}

DWORD CPlayer::TotalRepairPrice()
{
	DWORD dwTotalPrice=0;
	list<CPlayer::tagGoods>* pGoodList = GetGoodsList();
	if (pGoodList)
	{
		list<CPlayer::tagGoods>::iterator GoodsIter=pGoodList->begin();
		for(;GoodsIter!=pGoodList->end();GoodsIter++)
		{
			if(GoodsIter->pGoods->CanRepair())
			{
				//vector<CGoodsList::tagEffect>::iterator EffectIter=GoodsIter->pGoods->GetEffect()->begin();
				//for(;EffectIter!=GoodsIter->pGoods->GetEffect()->end();EffectIter++)
				//{
				//	if(EffectIter->wType==37 && EffectIter->dwValue2<EffectIter->dwValue1)
				//	{
				//		DWORD dwPrice=DWORD(GoodsIter->pGoods->GetBaseProperty()->dwValue * 
				//			(EffectIter->dwValue1 - EffectIter->dwValue2)/EffectIter->dwValue1 * 1.3 * (1+0.1));
				//		dwTotalPrice+=dwPrice;
				//	}
				//}
				dwTotalPrice+=GoodsIter->pGoods->RepairPrice();
			}
		}
	}
	for(int i=0;i<CPlayer::EQUIP_SUM;i++)
	{
		CGoods* pGoods=GetEquip(i);
		if(pGoods)
		{
			//vector<CGoodsList::tagEffect>::iterator EffectIter=pGoods->GetEffect()->begin();
			//for(;EffectIter!=pGoods->GetEffect()->end();EffectIter++)
			//{
			//	if(EffectIter->wType==37 && EffectIter->dwValue2<EffectIter->dwValue1)
			//	{
			//		DWORD dwPrice=DWORD(pGoods->GetBaseProperty()->dwValue * 
			//			(EffectIter->dwValue1 - EffectIter->dwValue2)/EffectIter->dwValue1 * 1.3 * (1+0.1));
			//		dwTotalPrice+=dwPrice;
			//	}
			//}
			dwTotalPrice+=pGoods->RepairPrice();
		}
	}
	return dwTotalPrice;
}



/*
* 功能: 初始化显示模型
* 摘要: -
* 参数: -
* 返回值: -
* 作者: -
* 创建日期: -
* 修改日志:
*	2008.04.02 - lpf
*		增加了脸部,发型,发色模型部件的设定;
*	2008.10.17 - lpf
*		增加了局部变量赋予初始值;
*	2009.03.05 - lpf
*		增加了更新自己的披风时,对披风开关是否开启做了判断;
*	2009.03.13 - lpf
*		增加了披风显示新规则:考虑特殊地图,和自身是否显示开关;
*/
void CPlayer::SetDisplayModelGroup(bool bAsyn)
{
	if (m_pDisplayModel == NULL)
	{
		return;
	}
	//初始化显示模型数据
	DWORD dwHelm=0, dwBody=0,dwGlove=0, dwShoes=0, dwWaistBand=0, dwWeaponActType=0,dwPifeng = 0;
	int   lHelmLevel=0, lBodyLevel=0,lGloveLevel=0, lShoesLevel=0, lBackLevel=0,dwPifengLevel = 0;

	// 主角自己
	if (m_bSelf)
	{
		CGoods* pEquip = GetEquip(CPlayer::EQUIP_HEAD);
		dwHelm = pEquip?pEquip->GetProperty()->dwEquipID:0;
		lHelmLevel = pEquip?pEquip->GetEffectVal(GAP_WEAPON_LEVEL,1):0;

		pEquip = GetEquip(CPlayer::EQUIP_BODY);
		dwBody = pEquip?pEquip->GetProperty()->dwEquipID:1;
		lBodyLevel = pEquip?pEquip->GetEffectVal(GAP_WEAPON_LEVEL,1):0;

		if (GetGame()->GetRegion() == NULL ||
			(GetGame()->GetRegion() != NULL && GetGame()->GetRegion()->IsChangeEquip()))
		{
			if (GetShowFashion() & FASHION_TYPE_MANTEAU/*!GetGame()->GetCGuiEx()->GetGameConfigPageEx()->GetOption(CGameConfigPageEx::OPTION_NoManteau)*/)
			{
				/*pEquip = GetEquip(CPlayer::EQUIP_PIFENG);
				dwPifeng = pEquip?pEquip->GetProperty()->dwEquipID:0;
				dwPifengLevel = pEquip?pEquip->GetEffectVal(CGoods::GAP_WEAPON_LEVEL,1):0;*/
			}
		}

		//dwWeaponActType = pEquip?pEquip->GetProperty()->dwWeaponActType:0;
		pEquip = GetEquip(CPlayer::EQUIP_HAND);
		dwGlove = pEquip?pEquip->GetProperty()->dwEquipID:1;
		lGloveLevel = pEquip?pEquip->GetEffectVal(GAP_WEAPON_LEVEL,1):0;

		pEquip = GetEquip(CPlayer::EQUIP_BOOT);
		dwShoes = pEquip?pEquip->GetProperty()->dwEquipID:1;
		lShoesLevel = pEquip?pEquip->GetEffectVal(GAP_WEAPON_LEVEL,1):0;

		pEquip = GetEquip(CPlayer::EQUIP_BACK);
		dwWaistBand = pEquip?pEquip->GetProperty()->dwEquipID:0;
		lBackLevel = pEquip?pEquip->GetEffectVal(GAP_WEAPON_LEVEL,1):0;
	}
	else	// 其他玩家
	{
		CGoodsList::tagGoods2 * pst = CGoodsList::GetProperty(GetEquipIndex(CPlayer::EQUIP_HEAD));
		dwHelm = pst?pst->BaseProperty.dwEquipID:0;
		lHelmLevel = GetEquipLevel(CPlayer::EQUIP_HEAD);

		pst = CGoodsList::GetProperty(GetEquipIndex(CPlayer::EQUIP_BODY));
		dwBody = pst?pst->BaseProperty.dwEquipID:1;
		lBodyLevel = GetEquipLevel(CPlayer::EQUIP_BODY);

		if (GetGame()->GetRegion() == NULL ||
			(GetGame()->GetRegion() != NULL && GetGame()->GetRegion()->IsChangeEquip()))
		{
			if (GetShowFashion() & FASHION_TYPE_MANTEAU)
			{
				/*pst = CGoodsList::GetProperty(GetEquipIndex(CPlayer::EQUIP_PIFENG));
				dwPifeng = pst?pst->BaseProperty.dwEquipID:0;
				dwPifengLevel = GetEquipLevel(CPlayer::EQUIP_PIFENG);*/
			}
		}
		//dwWeaponActType = pst?pst->BaseProperty.dwWeaponActType:0;

		pst = CGoodsList::GetProperty(GetEquipIndex(CPlayer::EQUIP_HAND));
		dwGlove = pst?pst->BaseProperty.dwEquipID:1;
		lGloveLevel = GetEquipLevel(CPlayer::EQUIP_HAND);

		pst = CGoodsList::GetProperty(GetEquipIndex(CPlayer::EQUIP_BOOT));
		dwShoes = pst?pst->BaseProperty.dwEquipID:1;
		lShoesLevel = GetEquipLevel(CPlayer::EQUIP_BOOT);

		pst = CGoodsList::GetProperty(GetEquipIndex(CPlayer::EQUIP_BACK));
		dwWaistBand = pst?pst->BaseProperty.dwEquipID:0;
		lBackLevel = GetEquipLevel(CPlayer::EQUIP_BACK);
	}

	static char str[32];
	static DWORD dwFACE=MAKEFOURCC('F','A','C','E');
	static DWORD dwHAIR=MAKEFOURCC('H','A','I','R');
	static DWORD dwHELM=MAKEFOURCC('H','E','L','M');
	static DWORD dwARMO=MAKEFOURCC('A','R','M','O');
	static DWORD dwPIFE=MAKEFOURCC('P','I','F','E');//披风
	static DWORD dwHAND=MAKEFOURCC('H','A','N','D');
	static DWORD dwFOOT=MAKEFOURCC('F','O','O','T');
	static DWORD dwBACK=MAKEFOURCC('B','A','C','K');
	static DWORD dwSCAB=MAKEFOURCC('S','C','A','B');
	static DWORD dwSPMS=MAKEFOURCC('S','P','M','S');

	// 显示简单部件
	m_pDisplayModel->ShowGroup(dwSPMS,0,0);

	// 显示脸部部件
	DWORD dwName;
	sprintf(str, "%04d", GetFacePic());
	dwName = MAKEFOURCC(str[0], str[1], str[2], str[3]);
	m_pDisplayModel->ShowGroup(dwFACE, dwName, 0);

	// 显示发型部件
	sprintf(str, "%04d", GetHairStyle());
	dwName = MAKEFOURCC(str[0], str[1], str[2], str[3]);
	m_pDisplayModel->ShowGroup(dwHAIR, dwName, 0);

	// 显示发色(等级)
	m_pDisplayModel->ShowGroup(dwHAIR, dwName, GetHairColor());

	// 头盔
	//sprintf(str, "%04d", dwHelm);
	//if(GetShowFashion())
	//{
	//	m_pDisplayModel->ShowGroup(dwHELM,MAKEFOURCC(str[0],str[1],str[2],str[3]),lHelmLevel);
	//}
	//else
	//	m_pDisplayModel->HideGroup(dwHELM);

	// 盔甲
	sprintf(str, "%04d", dwBody);
	m_pDisplayModel->ShowGroup(dwARMO,MAKEFOURCC(str[0],str[1],str[2],str[3]),lBodyLevel);

	// 披风
	if(dwPifeng != 0)
	{
		sprintf(str, "%04d", dwPifeng);
		m_pDisplayModel->ShowGroup(dwPIFE,MAKEFOURCC(str[0],str[1],str[2],str[3]),dwPifengLevel);
	}else
		m_pDisplayModel->HideGroup(dwPIFE);

	// 护腕
	sprintf(str, "%04d", dwGlove);
	m_pDisplayModel->ShowGroup(dwHAND,MAKEFOURCC(str[0],str[1],str[2],str[3]),lGloveLevel);

	// 靴子
	sprintf(str, "%04d", dwShoes);
	m_pDisplayModel->ShowGroup(dwFOOT,MAKEFOURCC(str[0],str[1],str[2],str[3]),lShoesLevel);
	//左手武器

	//右手武器
	//翅膀
	UpdateSpecialEquipment();

	// 更新饰品
	UpdateAccouterment();
}

// 该Player是否需要显示
bool CPlayer::IsNeedDisplay()
{
	if( GetCityWarDiedState() == true )	return false;
	return CMoveShape::IsNeedDisplay();
}
void CPlayer::UpdateSpecialEquipment()
{
	DWORD dwLocator = 0;

	//CClientRegion *pClientRegion = GetGame()->GetRegion();

	/*if (pClientRegion)
	{*/
	//GameMap *pGameMap = pClientRegion->GetGameMap();
	//翅膀
	if (m_Wing.IsVisible())
	{
		dwLocator= CShape::LOCATOR_BACK;
		m_Wing.SetLocator(dwLocator);
		//m_Wing->Render(pCamera,pGameMap,!pPlayer->GetIsConceal());
		//D3DXMatrixTranslation(&matWorld,vLocator.x,vLocator.y,vLocator.z);


	}
	dwLocator = 0;
	//头饰
	if (m_HeadWear.IsVisible())
	{
		dwLocator= CShape::LOCATOR_HEAD;
		m_HeadWear.SetLocator(dwLocator);
		//m_Wing->Render(pCamera,pGameMap,!pPlayer->GetIsConceal());
		//D3DXMatrixTranslation(&matWorld,vLocator.x,vLocator.y,vLocator.z);
	}
	dwLocator = 0;
	//主手武器
	if (m_RightHandWeapon.IsVisible())
	{
		if (this->GetState() == CPlayer::STATE_PEACE
			||this->IsRide()
			||(this->GetAction() >= ACT_FACEACTION && this->GetAction() <= ACT_FACEACTIONMAX)
			||(this->GetAction() >= ACT_MINE && this->GetAction() <= ACT_MINEMAX)
			)

		{
			switch(m_RightHandWeapon.GetWeaponType())
			{
			case CWeapon::WEAPONSHOW_SINGLESWORD:
			case CWeapon::WEAPONSHOW_SINGLEHAMMER:
			case CWeapon::WEAPONSHOW_SINGLEKNIFE:
				dwLocator= CShape::LOCATOR_LEFTWAIST;
				break;
			case CWeapon::WEAPONSHOW_BIGWANG:
			case CWeapon::WEAPONSHOW_BIGAX:
				dwLocator= CShape::LOCATOR_BACKSTAFF;
				break;
			case CWeapon::WEAPONSHOW_BIGSWORD:
				dwLocator= CShape::LOCATOR_BIGSWORD;
				break;
			case CWeapon::WEAPONSHOW_BOXINGRIGHT:
				dwLocator= CShape::LOCATOR_RIGHTBOXING;
				break;
			case CWeapon::WEAPONSHOW_DOUBLEWEAPON:
				dwLocator= CShape::LOCATOR_RIGHTBACK;
				break;
			case CWeapon::WEAPONSHOW_BOW:
				dwLocator= CShape::LOCATOR_BACKBOW;
				break;

			}


		}
		else if (GetState() == CPlayer::STATE_FIGHT)
		{
			switch(m_RightHandWeapon.GetWeaponType())
			{
			case CWeapon::WEAPONSHOW_SINGLESWORD:
			case CWeapon::WEAPONSHOW_SINGLEHAMMER:
			case CWeapon::WEAPONSHOW_SINGLEKNIFE:
				dwLocator= CShape::LOCATOR_RIGHTHAND;
				break;
			case CWeapon::WEAPONSHOW_BIGWANG:
			case CWeapon::WEAPONSHOW_BIGAX:
				dwLocator= CShape::LOCATOR_RIGHTHAND;
				break;
			case CWeapon::WEAPONSHOW_BIGSWORD:
				dwLocator= CShape::LOCATOR_RIGHTHAND;
				break;

			case CWeapon::WEAPONSHOW_DOUBLEWEAPON:
				dwLocator= CShape::LOCATOR_RIGHTHAND;
				break;
			case CWeapon::WEAPONSHOW_BOXINGRIGHT:
				dwLocator= CShape::LOCATOR_RIGHTBOXING;
				break;

			case CWeapon::WEAPONSHOW_BOW:
				dwLocator= CShape::LOCATOR_LEFTHAND;
				break;
			}

		}
		if (dwLocator != 0)
		{
			m_RightHandWeapon.SetLocator(dwLocator);
			m_REnergy.SetLocator(dwLocator);
		}

		//m_RightHandWeapon->Render(pCamera,pGameMap,!pPlayer->GetIsConceal());
		//D3DXMatrixTranslation(&matWorld,vLocator.x,vLocator.y,vLocator.z);


	}
	dwLocator = 0;
	//副手武器
	if (m_LeftHandWeapon.IsVisible())
	{


		if (GetState() == CPlayer::STATE_PEACE
			||this->IsRide()
			||(this->GetAction() >= ACT_FACEACTION && this->GetAction() <= ACT_FACEACTIONMAX)
			||(this->GetAction() >= ACT_MINE && this->GetAction() <= ACT_MINEMAX)
			)

		{
			switch(m_LeftHandWeapon.GetWeaponType())
			{
			case CWeapon::WEAPONSHOW_SINGLESWORD:
			case CWeapon::WEAPONSHOW_SINGLEHAMMER:
			case CWeapon::WEAPONSHOW_SINGLEKNIFE:
				dwLocator= CShape::LOCATOR_RIGHTWAIST;
				break;
			case CWeapon::WEAPONSHOW_BOXINGLEFT:
				dwLocator= CShape::LOCATOR_LEFTBOXING;
				break;
			case CWeapon::WEAPONSHOW_DOUBLEWEAPON:
				dwLocator= CShape::LOCATOR_LEFTBACK;
				break;
			case CWeapon::WEAPONSHOW_SHIELD:
				dwLocator= CShape::LOCATOR_LEFTARM;
				break;


			}

		}
		else if (GetState() == CPlayer::STATE_FIGHT)
		{
			switch(m_LeftHandWeapon.GetWeaponType())
			{
			case CWeapon::WEAPONSHOW_SINGLESWORD:
			case CWeapon::WEAPONSHOW_SINGLEHAMMER:
			case CWeapon::WEAPONSHOW_SINGLEKNIFE:
				dwLocator= CShape::LOCATOR_LEFTHAND;
				break;
			case CWeapon::WEAPONSHOW_BOXINGLEFT:
				dwLocator= CShape::LOCATOR_LEFTBOXING;
				break;
			case CWeapon::WEAPONSHOW_DOUBLEWEAPON:
				dwLocator= CShape::LOCATOR_LEFTHAND;
				break;
			case CWeapon::WEAPONSHOW_SHIELD:
				dwLocator= CShape::LOCATOR_LEFTARM;
				break;


			}

		}
		if (dwLocator != 0)
		{
			m_LeftHandWeapon.SetLocator(dwLocator);
			m_LEnergy.SetLocator(dwLocator);
		}

		//m_LeftHandWeapon->Render(pCamera,pGameMap,!pPlayer->GetIsConceal());
		//D3DXMatrixTranslation(&matWorld,vLocator.x,vLocator.y,vLocator.z);
	}
	//	}
}
void CPlayer::RendShadow(bool bsimpleshadow)
{
	if (m_pDisplayModel->GetLoadState() != Load_Did)
	{
		return;
	}
	CTextureProjective::SetSimpleShadow(bsimpleshadow);
	D3DXVECTOR3 pos;
	pos.x = m_pAnimInfo->GetWorldMatrix()->_41;
	pos.y = GetHeight();
	pos.z = m_pAnimInfo->GetWorldMatrix()->_43;
	render::BoundingBox * pboundbox = m_pDisplayModel->GetSizeBox();
	float wid = (max(pboundbox->GetWidth(),pboundbox->GetLength()))/2.0f;
	float hei = pboundbox->GetHeight();
	wid *= m_pAnimInfo->GetScalMatrix()->_11;
	hei *= m_pAnimInfo->GetScalMatrix()->_22;
	CTextureProjective::BeginRendShadow(pos,(int)max(wid,hei * 2));
	CTextureProjective::AddRendShadow(m_pDisplayModel,m_pAnimInfo);
	if (m_busesimplemodel)
	{
		if (m_RightHandWeapon.GetSimpleModel()&&m_RightHandWeapon.IsVisible())
		{
			if (m_RightHandWeapon.GetSimpleModel()->GetLoadState() == Load_Did)
			{
				//m_RightHandWeapon.GetSimpleModel()->ProcessAnimJoint(m_RightHandWeapon.GetAnimInfo());
				CTextureProjective::AddRendShadow(m_RightHandWeapon.GetSimpleModel(),m_RightHandWeapon.GetAnimInfo());
			}

		}

		if (m_LeftHandWeapon.GetSimpleModel()&&m_LeftHandWeapon.IsVisible())
		{
			if (m_LeftHandWeapon.GetSimpleModel()->GetLoadState() == Load_Did)
			{
				//m_LeftHandWeapon.GetSimpleModel()->ProcessAnimJoint(m_LeftHandWeapon.GetAnimInfo());
				CTextureProjective::AddRendShadow(m_LeftHandWeapon.GetSimpleModel(),m_LeftHandWeapon.GetAnimInfo());
			}

		}

		if (m_Wing.GetDisplayModel()&&m_Wing.IsVisible())
		{
			if (m_Wing.GetDisplayModel()->GetLoadState() == Load_Did)
			{
				//m_Wing.GetDisplayModel()->ProcessAnimJoint(m_Wing.GetAnimInfo());
				CTextureProjective::AddRendShadow(m_Wing.GetDisplayModel(),m_Wing.GetAnimInfo());
			}

		}
		if (m_Horse.GetDisplayModel()&&m_Horse.IsVisible())
		{
			if (m_Horse.GetDisplayModel()->GetLoadState() == Load_Did)
			{
				//m_Horse.GetDisplayModel()->ProcessAnimJoint(m_Horse.GetAnimInfo());
				CTextureProjective::AddRendShadow(m_Horse.GetDisplayModel(),m_Horse.GetAnimInfo());
			}

		}
	}
	else
	{
		if (m_RightHandWeapon.GetDisplayModel()&&m_RightHandWeapon.IsVisible())
		{
			if (m_RightHandWeapon.GetDisplayModel()->GetLoadState() == Load_Did)
			{
				//m_RightHandWeapon.GetDisplayModel()->ProcessAnimJoint(m_RightHandWeapon.GetAnimInfo());
				CTextureProjective::AddRendShadow(m_RightHandWeapon.GetDisplayModel(),m_RightHandWeapon.GetAnimInfo());
			}

		}

		if (m_LeftHandWeapon.GetDisplayModel()&&m_LeftHandWeapon.IsVisible())
		{
			if (m_LeftHandWeapon.GetDisplayModel()->GetLoadState() == Load_Did)
			{
				//m_LeftHandWeapon.GetDisplayModel()->ProcessAnimJoint(m_LeftHandWeapon.GetAnimInfo());
				CTextureProjective::AddRendShadow(m_LeftHandWeapon.GetDisplayModel(),m_LeftHandWeapon.GetAnimInfo());
			}

		}

		if (m_Wing.GetDisplayModel()&&m_Wing.IsVisible())
		{
			if (m_Wing.GetDisplayModel()->GetLoadState() == Load_Did)
			{
				//m_Wing.GetDisplayModel()->ProcessAnimJoint(m_Wing.GetAnimInfo());
				CTextureProjective::AddRendShadow(m_Wing.GetDisplayModel(),m_Wing.GetAnimInfo());
			}

		}
		if (m_HeadWear.GetDisplayModel()&&m_HeadWear.IsVisible())
		{
			if (m_HeadWear.GetDisplayModel()->GetLoadState() == Load_Did)
			{
				//m_HeadWear.GetDisplayModel()->ProcessAnimJoint(m_HeadWear.GetAnimInfo());
				CTextureProjective::AddRendShadow(m_HeadWear.GetDisplayModel(),m_HeadWear.GetAnimInfo());
			}

		}
		if (m_Horse.GetDisplayModel()&&m_Horse.IsVisible())
		{
			if (m_Horse.GetDisplayModel()->GetLoadState() == Load_Did)
			{
				//m_Horse.GetDisplayModel()->ProcessAnimJoint(m_Horse.GetAnimInfo());
				CTextureProjective::AddRendShadow(m_Horse.GetDisplayModel(),m_Horse.GetAnimInfo());
			}

		}
		if (m_CollectTool.GetDisplayModel()&&m_CollectTool.IsVisible())
		{
			if (m_CollectTool.GetDisplayModel()->GetLoadState() == Load_Did)
			{
				//m_CollectTool.GetDisplayModel()->ProcessAnimJoint(m_CollectTool.GetAnimInfo());
				CTextureProjective::AddRendShadow(m_CollectTool.GetDisplayModel(),m_CollectTool.GetAnimInfo());
			}

		}
	}



	CTextureProjective::EndRendShadow();
}
void CPlayer::PlayEnergyEffect(DWORD energylevel)
{
	m_bPlayEnergyEffect = TRUE;
	m_bEnergyTimeStarted = FALSE;
	m_LEnergy.GetAnimInfo()->m_tActionInfo.bCurActionLooped = 1;
	m_LEnergy.GetAnimInfo()->m_tActionInfo.dwCurActionElapseTime = 0;
	m_LEnergy.GetAnimInfo()->m_tActionInfo.dwCurActionLoopCount = 0;
	m_REnergy.GetAnimInfo()->m_tActionInfo.bCurActionLooped = 1;
	m_REnergy.GetAnimInfo()->m_tActionInfo.dwCurActionElapseTime = 0;
	m_REnergy.GetAnimInfo()->m_tActionInfo.dwCurActionLoopCount = 0;
	UpdateEnergyEffect(energylevel);
}
void CPlayer::EndEnergyEffect()
{
	if (m_REnergy.GetAnimInfo()->m_tActionInfo.dwCurActionLoopCount >= 1 
		|| m_LEnergy.GetAnimInfo()->m_tActionInfo.dwCurActionLoopCount >= 1)
	{
		m_bPlayEnergyEffect = FALSE;
		m_bEnergyTimeStarted = FALSE;
		SetEnergyHitEffectID(0);
	}

}
void CPlayer::UpdateEnergyEffect(DWORD energylevle)
{
	DWORD type = 0;
	DWORD index = 0;
	DWORD level = 0;
	DWORD modelid = 0;
	if (energylevle%2 == 0)
	{
		modelid = 20;
	}
	else
	{
		modelid = 10;
	} 
	DWORD hiteffectID = 0; 
	if (m_bSelf)
	{
		index = GetEquip(CPlayer::EQUIP_WEAPON)->GetIndex();
	}
	else
	{
		index = GetEquipIndex(CPlayer::EQUIP_WEAPON);
	}
	level = CGoodsList::GetEffectVal(index,GAP_ROLE_LEVEL_LIMIT,1);
	if (level >= 0 && level <= 39)
	{
		level = 0;
	}
	else if(level >= 40 && level <= 79)
	{
		level = 1;
	}
	else if(level >= 80 && level <= 119)
	{
		level = 2;
	}
	else if(level >= 120 && level <= 159)
	{
		level = 3;
	}
	else if(level >= 160 && level <= 200)
	{
		level = 4;
	}
	CGoodsList::tagEffect* ptegeffect = CGoodsList::GetEffectTag(index,GAP_WEAPON_CATEGORY);
	type = ptegeffect->dwValue1;

	switch(type)
	{
	case WT_SINGLE_SWORD:
	case WT_SINGLE_HAMMER:
	case WT_SINGLE_KNIFE:
		m_REnergy.SetModelID(60001 + modelid);
		m_REnergy.SetVisible(true);
		m_LEnergy.SetVisible(false);
		break;
	case WT_WAND://法杖
	case WT_HEAVY_SWORD://双手剑
	case WT_HEAVY_AX://双手斧:
		m_REnergy.SetModelID(60002 + modelid);
		m_REnergy.SetVisible(true);
		m_LEnergy.SetVisible(false);
		break;
	case WT_BOW:
		m_LEnergy.SetModelID(60004 + modelid);
		m_LEnergy.SetVisible(true);
		m_REnergy.SetVisible(false);
		break;
	case WT_DOUBLE_SWORD:
		m_LEnergy.SetModelID(60003 + modelid);
		m_REnergy.SetModelID(60003 + modelid);
		m_LEnergy.SetVisible(true);
		m_REnergy.SetVisible(true);
		break;
	case WT_FIST:
		m_LEnergy.SetModelID(60005 + modelid);
		m_REnergy.SetModelID(61005 + modelid);
		m_LEnergy.SetVisible(true);
		m_REnergy.SetVisible(true);
		break;
	}
	hiteffectID = 61000 + modelid + level + 1;
	SetEnergyHitEffectID(hiteffectID);
	m_LEnergy.SetDisplayModel();
	m_REnergy.SetDisplayModel();
	m_LEnergy.SetLevel(level);
	m_REnergy.SetLevel(level);

}



/*
* 功能: 设定外套的可见性
* 摘要: -
* 参数: bIsVisible - 是否可见
* 返回值: -
* 作者: lpf
* 创建日期: 2009.03.05
* 修改日志:
*	2009.04.10- lpf
* 		增加隐藏身体时,披风也会被隐藏.且增加了不显示外套时披风的相关显示;
*/
void CPlayer::SetGreatcoatVisible(BOOL bIsVisible)
{
	static DWORD dwGreatcoatGroup = MAKEFOURCC('B','O','D','Y');
	static DWORD dwHairGroup	  = MAKEFOURCC('H','A','I','R');
	static DWORD dwArmoGroup	  = MAKEFOURCC('A','R','M','O');
	static DWORD dwHandGroup	  = MAKEFOURCC('H','A','N','D');
	static DWORD dwFootGroup	  = MAKEFOURCC('F','O','O','T');
	static DWORD dwFaceGroup	  = MAKEFOURCC('F','A','C','E');
	static DWORD dwPIFEGroup	  = MAKEFOURCC('P','I','F','E');		//披风
	static char str[32];

	if (m_pDisplayModel == NULL)
		return;

	if (bIsVisible)
	{
		if (m_bSelf)
		{
			CGoods * pGoods = GetEquip(CPlayer::EQUIP_P_FROCK);
			if (pGoods != NULL)
			{
				DWORD dwGreatcoatPart  = pGoods->GetProperty()->dwEquipID;
				DWORD dwGreatcoatLevel = pGoods->GetEffectVal(GAP_WEAPON_LEVEL, 1);

				if (pGoods->GetEffectVal(GAP_HIDE_HEAR, 1) != 0)
					m_pDisplayModel->HideGroup(dwHairGroup);
				if (pGoods->GetEffectVal(GAP_HIDE_FACE, 1) != 0)
					m_pDisplayModel->HideGroup(dwFaceGroup);
				if (pGoods->GetEffectVal(GAP_HIDE_BODY, 1) != 0)
				{
					m_pDisplayModel->HideGroup(dwArmoGroup);
					m_pDisplayModel->HideGroup(dwHandGroup);
					m_pDisplayModel->HideGroup(dwFootGroup);
					m_pDisplayModel->HideGroup(dwPIFEGroup);
				}

				if (dwGreatcoatPart != 0)
				{
					sprintf(str, "%04d", dwGreatcoatPart);
					m_pDisplayModel->ShowGroup(dwGreatcoatGroup, MAKEFOURCC(str[0],str[1],str[2],str[3]), dwGreatcoatLevel);
				}
			}else
				m_pDisplayModel->HideGroup(dwGreatcoatGroup);
		}else
		{
			DWORD dwEquipIndex = GetEquipIndex(CPlayer::EQUIP_P_FROCK);
			CGoodsList::tagGoods2 * pGoods = CGoodsList::GetProperty(dwEquipIndex);
			if (pGoods != NULL)
			{				
				DWORD dwGreatcoatPart  = pGoods->BaseProperty.dwEquipID;
				DWORD dwGreatcoatLevel = GetEquipLevel(CPlayer::EQUIP_P_FROCK);

				if (CGoodsList::GetEffectVal(dwEquipIndex, GAP_HIDE_HEAR, 1) != 0)
					m_pDisplayModel->HideGroup(dwHairGroup);
				if (CGoodsList::GetEffectVal(dwEquipIndex, GAP_HIDE_FACE, 1) != 0)
					m_pDisplayModel->HideGroup(dwFaceGroup);
				if (CGoodsList::GetEffectVal(dwEquipIndex, GAP_HIDE_BODY, 1) != 0)
				{
					m_pDisplayModel->HideGroup(dwArmoGroup);
					m_pDisplayModel->HideGroup(dwHandGroup);
					m_pDisplayModel->HideGroup(dwFootGroup);
					m_pDisplayModel->HideGroup(dwPIFEGroup);
				}

				if (dwGreatcoatPart != 0)
				{
					sprintf(str, "%04d", dwGreatcoatPart);
					m_pDisplayModel->ShowGroup(dwGreatcoatGroup, MAKEFOURCC(str[0],str[1],str[2],str[3]), dwGreatcoatLevel);
				}
			}else
				m_pDisplayModel->HideGroup(dwGreatcoatGroup);
		}
	}else
	{
		DWORD dwArmo, dwGlove, dwShoes, dwPifeng; 
		DWORD dwArmoLevel, lGloveLevel, lShoesLevel, dwPifengLevel;
		if (m_bSelf)
		{
			// 盔甲
			CGoods * pGoods = GetEquip(CPlayer::EQUIP_BODY);
			dwArmo = pGoods ? pGoods->GetProperty()->dwEquipID : 1;
			dwArmoLevel = pGoods ? pGoods->GetEffectVal(GAP_WEAPON_LEVEL,1) : 0;

			// 护腕
			pGoods = GetEquip(CPlayer::EQUIP_HAND);
			dwGlove = pGoods ? pGoods->GetProperty()->dwEquipID : 1;
			lGloveLevel = pGoods ? pGoods->GetEffectVal(GAP_WEAPON_LEVEL,1) : 0;

			// 靴子
			pGoods = GetEquip(CPlayer::EQUIP_BOOT);
			dwShoes = pGoods ? pGoods->GetProperty()->dwEquipID : 1;
			lShoesLevel = pGoods ? pGoods->GetEffectVal(GAP_WEAPON_LEVEL,1) : 0;

			// 披风
			pGoods = GetEquip(CPlayer::EQUIP_P_FROCK);
			dwPifeng = pGoods ? pGoods->GetProperty()->dwEquipID : 1;
			dwPifengLevel = pGoods ? pGoods->GetEffectVal(GAP_WEAPON_LEVEL,1) : 0;
		}else
		{
			// 盔甲
			CGoodsList::tagGoods2 * pGoods = CGoodsList::GetProperty(GetEquipIndex(CPlayer::EQUIP_BODY));
			dwArmo = pGoods ? pGoods->BaseProperty.dwEquipID : 1;
			dwArmoLevel = GetEquipLevel(CPlayer::EQUIP_BODY);

			// 护腕
			pGoods = CGoodsList::GetProperty(GetEquipIndex(CPlayer::EQUIP_HAND));
			dwGlove = pGoods ? pGoods->BaseProperty.dwEquipID : 1;
			lGloveLevel = GetEquipLevel(CPlayer::EQUIP_HAND);

			// 靴子
			pGoods = CGoodsList::GetProperty(GetEquipIndex(CPlayer::EQUIP_BOOT));
			dwShoes = pGoods ? pGoods->BaseProperty.dwEquipID : 1;
			lShoesLevel = GetEquipLevel(CPlayer::EQUIP_BOOT);

			// 披风
			pGoods = CGoodsList::GetProperty(GetEquipIndex(CPlayer::EQUIP_P_FROCK));
			dwPifeng = pGoods ? pGoods->BaseProperty.dwEquipID : 1;
			dwPifengLevel = GetEquipLevel(CPlayer::EQUIP_P_FROCK);
		}

		// 显示脸部部件
		sprintf(str, "%04d", GetFacePic());
		DWORD dwPart = MAKEFOURCC(str[0], str[1], str[2], str[3]);
		m_pDisplayModel->ShowGroup(dwFaceGroup, dwPart, 0);

		// 显示发型部件
		sprintf(str, "%04d", GetHairStyle());
		dwPart = MAKEFOURCC(str[0], str[1], str[2], str[3]);

		// 显示发色(等级)
		m_pDisplayModel->ShowGroup(dwHairGroup, dwPart, GetHairColor());

		sprintf(str, "%04d", dwArmo);
		m_pDisplayModel->ShowGroup(dwArmoGroup, MAKEFOURCC(str[0],str[1],str[2],str[3]), dwArmoLevel);
		sprintf(str, "%04d", dwGlove);
		m_pDisplayModel->ShowGroup(dwHandGroup, MAKEFOURCC(str[0],str[1],str[2],str[3]), lGloveLevel);
		sprintf(str, "%04d", dwShoes);
		m_pDisplayModel->ShowGroup(dwFootGroup, MAKEFOURCC(str[0],str[1],str[2],str[3]), lShoesLevel);

		if (GetShowFashion() & FASHION_TYPE_MANTEAU)
		{
			sprintf(str, "%04d", dwPifeng);
			m_pDisplayModel->ShowGroup(dwPIFEGroup, MAKEFOURCC(str[0],str[1],str[2],str[3]), dwPifengLevel);
		}

		m_pDisplayModel->HideGroup(dwGreatcoatGroup);
	}
}



/*
* 功能: 设定披风的可见性
* 摘要: -
* 参数: bIsVisible - 是否可见
* 返回值: -
* 作者: lpf
* 创建日期: 2009.03.03
* 修改日志:
*/
void CPlayer::SetManteauVisible(BOOL bIsVisible)
{
	DWORD dwManteauGroup = MAKEFOURCC('P','I','F','E');

	if (bIsVisible)
	{
		static char str[32];
		if (m_bSelf)
		{
			CGoods * pGoods = GetEquip(CPlayer::EQUIP_P_FROCK);
			if (pGoods != NULL)
			{
				DWORD dwManteauPart  = pGoods->GetProperty()->dwEquipID;
				DWORD dwManteauLevel = pGoods->GetEffectVal(GAP_WEAPON_LEVEL, 1);
				if (dwManteauPart != 0)
				{
					sprintf(str, "%04d", dwManteauPart);
					m_pDisplayModel->ShowGroup(dwManteauGroup, MAKEFOURCC(str[0],str[1],str[2],str[3]), dwManteauLevel);
				}
			}else
				m_pDisplayModel->HideGroup(dwManteauGroup);
		}else
		{
			DWORD dwEquipIndex = GetEquipIndex(CPlayer::EQUIP_P_FROCK);
			CGoodsList::tagGoods2 * pGoods = CGoodsList::GetProperty(dwEquipIndex);
			if (pGoods != NULL)
			{
				DWORD dwManteauPart  = pGoods->BaseProperty.dwEquipID;
				DWORD dwManteauLevel = GetEquipLevel(CPlayer::EQUIP_P_FROCK);
				if (dwManteauPart != 0)
				{
					sprintf(str, "%04d", dwManteauPart);
					m_pDisplayModel->ShowGroup(dwManteauGroup, MAKEFOURCC(str[0],str[1],str[2],str[3]), dwManteauLevel);
				}
			}else
				m_pDisplayModel->HideGroup(dwManteauGroup);
		}
	}else
		m_pDisplayModel->HideGroup(dwManteauGroup);
}



/*
* 功能: 玩家的显示
* 摘要: 该函数执行玩家显示,并处理了光照效果
* 参数: -
* 返回值: 执行成功返回ture,否则返回false
* 作者: -
* 创建日期: -
* 修改日志:
*	2008.01.12 - lpf
*		在该函数中增加了对武器模型附加闪光特效的渲染
*	2008.01.16 - lpf
*		在该函数中增加了对武器模型异常状态特效的渲染
*	2008.07.28 - lpf
*		修改了玩家在骑乘状态时,Locator位置,现在是从后背开始
*	2008.10.17 - lpf
*		修改了武器显示的调用
*	2008.10.23 - lpf
*		修改了坐骑显示的调用
*	2009.04.24 - lpf
*		坐骑显示时,传入对象指针采用了dynamic_cast形式,检查错误的转换;
*/
bool CPlayer::Display(void)
{
	if (m_pDisplayModel)
	{
		m_pDisplayModel->SetRendSimpleMesh(m_busesimplemodel);
		m_Horse.SetSimpleMesh(m_busesimplemodel);
		m_Wing.SetSimpleMesh(m_busesimplemodel);
		m_HeadWear.SetSimpleMesh(m_busesimplemodel);
		m_LeftHandWeapon.SetSimpleMesh(m_busesimplemodel);
		m_RightHandWeapon.SetSimpleMesh(m_busesimplemodel);
	}
	if (IsRide())
	{
		if (m_Horse.GetDisplayModel() && GetDisplayModel() &&
			m_Horse.GetDisplayModel()->GetLoadState() == Load_Did && GetDisplayModel()->GetLoadState() == Load_Did)
		{
			//m_Horse.Display(GetPosX() + m_fXIncrement, GetPosY() + m_fYIncrement, GetHeight(), GetNowDir());
			m_Horse.Display(dynamic_cast<CMoveShape *>(this));
			if( !CMoveShape::DisplayByLocator(m_Horse.GetDisplayModel(),m_Horse.GetAnimInfo(),CShape::LOCATOR_BACK))
			{
				return false;
			}
		}
	}
	else
	{
		m_Horse.SetVisible(false);
		if( !CMoveShape::Display() )
			return false;
	}

	// 处理模型附加效果闪光时的武器闪光效果
	if (GetGame()->GetSetup()->bRoleFlash && m_listAddEffect[ADDEFFECT_FLASH].bPlayState)			//闪光效果
	{
		if (timeGetTime() - m_listAddEffect[ADDEFFECT_FLASH].dwStartTime >= m_listAddEffect[ADDEFFECT_FLASH].dwTimeLength)
			m_listAddEffect[ADDEFFECT_FLASH].bPlayState = false;

		DWORD dwColor = 0xff000000 |
			(m_listAddEffect[ADDEFFECT_FLASH].dwValue << 16) |
			(m_listAddEffect[ADDEFFECT_FLASH].dwValue << 8) |
			m_listAddEffect[ADDEFFECT_FLASH].dwValue;

		// 处理子模型颜色
		m_RightHandWeapon.DisplayForFlashColor(m_pDisplayModel, m_pAnimInfo, dwColor);
		m_LeftHandWeapon.DisplayForFlashColor(m_pDisplayModel, m_pAnimInfo, dwColor);
		m_Wing.DisplayForFlashColor(m_pDisplayModel, m_pAnimInfo, dwColor);
		m_HeadWear.DisplayForFlashColor(m_pDisplayModel, m_pAnimInfo, dwColor);
		m_CollectTool.DisplayForFlashColor(m_pDisplayModel, m_pAnimInfo, dwColor);
	}else if (m_bStateColorPlay)				//处理异常状态下武器变色效果
	{
		//显示其他子模型
		m_RightHandWeapon.DisplayForExStateColor(m_pDisplayModel, m_pAnimInfo, m_dwCurrentModelCol);
		m_LeftHandWeapon.DisplayForExStateColor(m_pDisplayModel, m_pAnimInfo, m_dwCurrentModelCol);	
		m_Wing.DisplayForExStateColor(m_pDisplayModel, m_pAnimInfo, m_dwCurrentModelCol);
		m_HeadWear.DisplayForExStateColor(m_pDisplayModel, m_pAnimInfo, m_dwCurrentModelCol);
		m_CollectTool.DisplayForExStateColor(m_pDisplayModel, m_pAnimInfo, m_dwCurrentModelCol);
	}else
	{
		//显示其他子模型
		m_RightHandWeapon.Display(dynamic_cast<CShape *>(this));
		m_LeftHandWeapon.Display(dynamic_cast<CShape *>(this));	
		m_Wing.Display(dynamic_cast<CShape *>(this));
		m_HeadWear.Display(dynamic_cast<CShape *>(this));
		m_CollectTool.Display(dynamic_cast<CShape *>(this));
		if (m_bPlayEnergyEffect == TRUE)
		{
			if (!m_bEnergyTimeStarted)
			{
				m_dwEnergyStartTime = timeGetTime();
				m_bEnergyTimeStarted = true;
			}
			if (timeGetTime() - m_dwEnergyStartTime > 500)
			{
				m_LEnergy.Display(dynamic_cast<CShape *>(this));
				m_REnergy.Display(dynamic_cast<CShape *>(this));
			}

			/*	m_LEnergy.Display(m_pDisplayModel, m_pAnimInfo);
			m_REnergy.Display(m_pDisplayModel, m_pAnimInfo);*/
			if (timeGetTime() - m_dwEnergyStartTime > 1300)
			{
				EndEnergyEffect();
			}



		}
	}
	return true;
}



//显示战斗状态的相关图标
void  CPlayer::DisplayWarBitmap()
{
	D3DXVECTOR3 pos(GetPosY(),GetHeight()+2,GetPosX());
	D3DXMATRIX matFinal;
	GetGame()->GetRegion()->CalculateFinalTransformMatrix(matFinal);
	D3DXVECTOR3 pos2;
	D3DXVec3TransformCoord(&pos2,&pos,&matFinal);

	long x = (long)pos2.x;
	long y = (long)pos2.y-2;

	//如果是敌对行会玩家，显示敌对状态
	if(GetFactionID() != CGUID::GUID_INVALID)
	{
		if( GetGame()->GetRegion()->GetWarType() == RWT_VillageWar )	//如果是村战地图
		{
			if(GetGame()->GetMainPlayer()->IsFriendFaction(GetFactionID(),GetUnionID()) == true)
			{
				x = x - GetGame()->GetFactionWarUnionBitmap()->GetWidth()/2;
				GetGame()->GetFactionWarUnionBitmap()->Draw(x,y);
			}
			else
			{
				x = x - GetGame()->GetFactionWarEnemyBitmap()->GetWidth()/2;
				GetGame()->GetFactionWarEnemyBitmap()->Draw(x,y);
			}
		}
		else if( GetGame()->GetMainPlayer()->IsHaveCityWarEnemyFactions() )
		{
			if( GetGame()->GetMainPlayer()->IsCityWarEnemyFaction(GetFactionID()) == true )
			{
				GetGame()->GetCityWarEnemyBitmap()->Draw(x-GetGame()->GetCityWarEnemyBitmap()->GetWidth()-2,y);
			}
			else if(GetGame()->GetMainPlayer()->IsFriendFaction(GetFactionID(),GetUnionID()) == true)
			{
				GetGame()->GetCityWarUnionBitmap()->Draw(x-GetGame()->GetCityWarUnionBitmap()->GetWidth()-2,y);
			}
		}
		else if( GetGame()->GetMainPlayer()->IsHaveEnemyFactions() )
		{
			if( GetGame()->GetMainPlayer()->IsEnemyFaction(GetFactionID()) == true)
			{
				x = x - GetGame()->GetFactionWarEnemyBitmap()->GetWidth()/2;
				GetGame()->GetFactionWarEnemyBitmap()->Draw(x,y);
			}
			else if(GetGame()->GetMainPlayer()->IsFriendFaction(GetFactionID(),GetUnionID()) == true)
			{
				x = x - GetGame()->GetFactionWarUnionBitmap()->GetWidth()/2;
				GetGame()->GetFactionWarUnionBitmap()->Draw(x,y);
			}
		}
	}
}



/*
* 功能: 显示玩家头顶信息
* 摘要: -
* 参数: str		- 需要显示的文字信息
*		 color		- 字体颜色
*		 bkcolor	- 字体背景颜色
*		 fPos		- 绘制起始高度
*		 bkBarColor - 绘制背景条颜色
* 返回值: -
* 作者: -
* 创建日期: -
* 修改日志:
*	2008.02.18 - lpf
*		绘制高度的long y(起始坐标)以传入的fPos为基准
*	2008.03.01 - lpf
*		按照新的需求调整了玩家头顶信息的显示布局,增加了勋章和爵位显示,另外还有状态对玩家显示信息的控制
*	2008.06.18 - lpf
*		显示玩家名字样式由原来的描边,改为阴影
*	2008.07.07 - lpf
*		玩家头顶名字行改为:同行所有显示信息居中显示,以适应新的选中玩家显示方式
*   2008.07.10 - lpf
*		增加了对象头顶名字绘制的控制,主要是为了配合游戏设置页面(是否绘制自身名字)
*   2008.07.17 - lpf
*		取消了行会绘制的超出屏幕判断;
*		修改了一些头顶信息的绘制位置;
*   2008.07.22 - lpf
*		取消了玩家名字绘制高度超出屏幕下方判断;
*		修改了公会名和选中玩家箭头在同行显示问题;
*   2008.07.30 - lpf
*		工会信息颜色固定为绿色带阴影;
*   2008.08.28 - lpf
*		增加了行会自定义标志显示;
*		修改了行会名颜色(和玩家当前名字颜色一致);
*		取消了行会称号显示;
*		头顶显示的信息顶部增加了一行玩家当前选择的称号爵位显示;
*   2008.09.16 - lpf
*		增加了国家标志根据玩家对应的属性来决定是否显示;
*   2008.10.09 - lpf
*		修正了当有称号,名字,公会时,选择不显示名字,则会空出一行;
*   2008.10.20 - lpf
*		增加了玩家在国旗关闭状态下,该名字颜色在其他任何玩家看来都显示为蓝色,而不是通常情况下按阵营显示;
*   2008.10.22 - lpf
*		修正了玩家名字显示的阴影颜色和字体颜色反了;
*   2008.12.05 - lpf
*		增加了玩家头顶爵位信息,通过游戏设置界面选项控制;
*   2008.12.11 - lpf
*		增加了同盟国和非同盟国玩家名字显示判断;
*   2008.12.25 - lpf
*		在"名字行箭头显示"位置,调整了左箭头向左2个像素,修正头顶名字和箭头信息不居中;
*   2008.01.04 - lpf
*		修正了自己看其他红民玩家时,名字不是红民,增加了名字显示时的PK值判定;
*   2008.01.07 - lpf
*		回滚了2008.12.25的修改;
*   2009.03.14 - lpf
*		主要修改了关于称号的颜色显示:
*		竞争阵营玩家所有头顶信息颜色随名字颜色显示;
*		家族自定义称号颜色随玩家名字颜色显示(原来是根据配置文件显示);
*   2009.11.13 - lpf
*		增加了函数参数bIsDisplayHPBar,用于记录血条是否显示:
*		调整了显示内容,在PK场景下,不同阵营玩家名字前会有双剑交叉图片显示(目前是双剑交叉);
*/
void CPlayer::DisplayText(const char * str, D3DCOLOR color, D3DCOLOR bkcolor, float fPos, D3DCOLOR bkBarColor, bool bIsDisplayHPBar)
{
	//	static bool bPlayerTitleCheck = true;			//玩家头顶勋章和爵位信息开关
	bool bPlayerGuildInfoCheck = false; //玩家头顶行会及行会称号信息开关

	D3DXVECTOR3 pos(GetPosY(), GetPickBoxHeight() + 0.2f + GetHeight(), GetPosX());
	D3DXMATRIX matFinal;
	GetGame()->GetRegion()->CalculateFinalTransformMatrix(matFinal);
	D3DXVECTOR3 pos2;
	D3DXVec3TransformCoord(&pos2,&pos,&matFinal);

	long x			 = (long)pos2.x;
	long y			 = (long)fPos;
	long lFontHeight = GetGame()->GetFontHeight(-1);
//	long lMaxLeft	 = x - 50;
	int	 len = 0;
	long lLineCount		  = 0;
	long lDrawNameLineLen = 0;
	long lDrawNamePos	  = 0;
	bool bPlayerNameCheck = true;			//玩家头顶名字开关
	bool bIsMatchingNameColor = false;		//是否匹配名字颜色(针对其他信息字体颜色)
	bool bIsPkFlag = false;					//是否为pk场景标志

	//if (bIsDisplayHPBar)
	//	lMaxLeft = x - 50;
	//else
	//	lMaxLeft = x;

	// 对名字显示的判断
	if (this == GetGame()->GetMainPlayer())
		bPlayerNameCheck = true;//GetGame()->GetCGuiEx()->GetGameConfigPageEx()->GetOption(CGameConfigPageEx::OPTION_ShowMyName);
	else
	{
		CCountryList::tagCountryList * countrySelf	  = CCountryList::GetCountry(GetGame()->GetMainPlayer()->GetCountry());
		CCountryList::tagCountryList * countryAnother = CCountryList::GetCountry(this->GetCountry());

		if (countrySelf && countryAnother)
		{
			// 判断国家是否相符
			if (countrySelf->nLeague == countryAnother->nLeague)
			{
				if (this->GetIsConceal())
					bPlayerNameCheck = false;//GetGame()->GetCGuiEx()->GetGameConfigPageEx()->GetOption(CGameConfigPageEx::OPTION_ShowAlliedPlayerName);
			}else
			{
				bIsMatchingNameColor = true;

				if (this->GetIsConceal())
					bPlayerNameCheck = false;//GetGame()->GetCGuiEx()->GetGameConfigPageEx()->GetOption(CGameConfigPageEx::OPTION_ShowNotAlliedPlayerName);

				// 判断pk相关
				if (GetGame()->GetRegion()->GetPKRegionType() == RPKT_PK && bPlayerNameCheck)
				{
					// 判断是否组队
					if (GetGame()->GetMainPlayer()->GetExID() != GetExID() &&
						GetGame()->GetMainPlayer()->IsTeammate(GetExID()))
						bIsPkFlag = false;
					else
						bIsPkFlag = true;
				}
			}
		}
	}
	if (m_bIsSelected)
		bPlayerNameCheck = true;

	// 国家
	bool bIsKing = false;

	CCountryList::tagCountryList * pst = CCountryList::GetCountry(GetCountry());
	CBitmapX * pbmPirate = NULL;
	CBitmapX * pSelectShape = NULL;
	static char strCountry[256];

	// 这里显示是否是国王
	// 通过行会是否拥有主城
	// 在通过改玩家是否是帮主来判定 bugs
	for( list<tagownedReg>::iterator it = m_OwnedRegions.begin(); it!=m_OwnedRegions.end(); ++it)
	{
		if(it->wType == 2) // 主城 虽然有4个国家，但每个国家只有一个主城，本国的主城只能被本国的行会打下。
		{
			if(GetExID() == m_FacMasterID)
			{
				bIsKing = true;
				break;
			}
		}
	}

	// 计算名字行显示信息像素长度
	if (bPlayerNameCheck)
		lDrawNameLineLen += (GetGame()->GetFontWidth(str) + 4);

	if (GetShowCountry())
	{
		if (pst)
			lDrawNameLineLen += 30;
		else
		{
            sprintf(strCountry, AppFrame::GetText("Base_100"));
			lDrawNameLineLen += GetGame()->GetFontWidth(strCountry);
		}
	}

	if (GetPVPValue())
	{
		if( GetPVPValue() < PVP_PIRATE1 )
		{
			pbmPirate = CBitmapX::GetBitmapX("pictures/state/pirate1.tga");
		}
		else if( GetPVPValue() < PVP_PIRATE2 )
		{
			pbmPirate = CBitmapX::GetBitmapX("pictures/state/pirate2.tga");
		}
		else if( GetPVPValue() < PVP_PIRATE3 )
		{
			pbmPirate = CBitmapX::GetBitmapX("pictures/state/pirate3.tga");
		}
		else
		{
			pbmPirate = CBitmapX::GetBitmapX("pictures/state/pirate4.tga");
		}
		if (pbmPirate)
			lDrawNameLineLen += pbmPirate->GetWidth();
	}

	// 名字行箭头显示
	if (m_bIsSelected)
	{
		long lTmpY;
		pSelectShape = GetGame()->m_pSelectShape;
		DWORD dwW2	 = pSelectShape->GetWidth() / 2;
		DWORD dwH	 = pSelectShape->GetHeight();
		RECT  rc	 = { 0, 0, dwW2, dwH };
		lDrawNamePos = (long)pos2.x - dwW2 - lDrawNameLineLen / 2;		//计算名字行绘制X起始坐标

		if (GetFactionID() != CGUID::GUID_INVALID && bPlayerGuildInfoCheck)
			lTmpY = y - lFontHeight - 3;
		else
			lTmpY = y;

		GetGame()->m_pSelectShape->Draw(lDrawNamePos - 8, lTmpY, rc);

		rc.left  = dwW2;
		rc.right = dwW2 * 2;

		GetGame()->m_pSelectShape->Draw(lDrawNamePos + 8 + dwW2 + lDrawNameLineLen, lTmpY, rc);
		lDrawNamePos += dwW2;
	}else
		lDrawNamePos = (long)pos2.x - lDrawNameLineLen / 2;


	/*	// 显示国家官阶
	D3DCOLOR textColor = D3DCOLOR_ARGB(255,255,255,255);
	D3DCOLOR backTextColor = D3DCOLOR_ARGB(255,255,255,255);
	//int iCountryCredit  = GetBaseProperty()->dwCredit;
	string strOfficialRank = "";
	SetOfficialRank("");
	vector<COffciailRankConfig::tagOfficialRankConfig>::iterator ConfigIter= COffciailRankConfig::GetOfficialRankVec().begin();
	for(;ConfigIter!= COffciailRankConfig::GetOfficialRankVec().end();ConfigIter++)
	{
	if(iCountryCredit >= (*ConfigIter).CreditMinValue && iCountryCredit <= (*ConfigIter).CreditMaxValue)
	{
	if(GetOccupation()==CPlayer::OC_FIGHTER)
	{
	SetOfficialRank((*ConfigIter).strFighterOfficialRank);
	}
	else if(GetOccupation()==CPlayer::OC_HUNTER)
	{
	SetOfficialRank((*ConfigIter).strHunterOfficialRank);
	}
	else if(GetOccupation()==CPlayer::OC_TAOIST)
	{
	SetOfficialRank((*ConfigIter).strDaoshiOfficialRank);
	}
	textColor = D3DCOLOR_ARGB((*ConfigIter).textColorA,(*ConfigIter).textColorR,(*ConfigIter).textColorG,(*ConfigIter).textColorB);
	backTextColor =  D3DCOLOR_ARGB((*ConfigIter).textBkColorA,(*ConfigIter).textBkColorR,(*ConfigIter).textBkColorG,(*ConfigIter).textBkColorB);
	SetOffcialRankTextColor(textColor);
	SetOffcialRankBkTextColor(backTextColor);
	break;
	}
	}

	if(GetOfficialRank()!="")
	{
	strOfficialRank += GetOfficialRank();
	}

	if(bIsKing)
	{
	string strCountry = CCountryList::GetCountry(GetCountry())->strName;
	strCountry = strCountry.substr(2,4);
	strOfficialRank = strCountry + "国 国王";
	}



	y = y - GetGame()->GetFontHeight()-2;

	if(GetFactionID() > 0)
	{
	y = y - GetGame()->GetFontHeight()-2;
	}

	if (x<0)
	x = 0;
	if (y<0)
	y = 0;
	if (x > SCREEN_WIDTH-len * GetGame()->GetFontWidth(-1))
	x = SCREEN_WIDTH-len * GetGame()->GetFontWidth(-1);
	if (y > SCREEN_HEIGHT-63-GetGame()->GetFontHeight())
	y = SCREEN_HEIGHT-63-GetGame()->GetFontHeight();

	if (bIsKing)
	{
	SetOffcialRankTextColor( CGlobeSetup::GetSetup()->colorGoodsDarkGold);
	}

	GetGame()->OutPutText(x, y-1, strOfficialRank.c_str(), len, GetOffcialRankBkTextColor(), 1.f, false);
	GetGame()->OutPutText(x-1, y, strOfficialRank.c_str(), len, GetOffcialRankBkTextColor(), 1.f, false);
	GetGame()->OutPutText(x, y+1, strOfficialRank.c_str(), len, GetOffcialRankBkTextColor(), 1.f, false);
	GetGame()->OutPutText(x+1, y, strOfficialRank.c_str(), len, GetOffcialRankBkTextColor(), 1.f, false);
	GetGame()->OutPutText(x, y, strOfficialRank.c_str(), len, GetOffcialRankTextColor(), 1.f, false);
	*/

	// 越界判断
	//////////////////临时使用/////////////////
	//string strOfficialRank = "";
	//int len = strOfficialRank.length();
	//long x = (long)pos2.x - len * GetGame()->GetFontWidth(-1) / 2;
	//long y = (long)fPos - GetGame()->GetFontWidth(-1);
	//////////////////////////////////////////	

	// 显示行会
	if(GetFactionID() != CGUID::GUID_INVALID && bPlayerGuildInfoCheck)
	{
		CBitmapX * pPic		  = NULL;
		string strFactionName =  "<";
		strFactionName		  += GetFactionName();
		strFactionName		  += ">";
		//if(GetFactionTitle() != "")
		//{
		//	strFactionName += " ";
		//	strFactionName += GetFactionTitle();
		//}
		len = (int)strFactionName.length();
		x = (long)pos2.x - GetGame()->GetFontWidth(strFactionName.c_str()) / 2;

		if( NULL_GUID != m_FactionID )
		{
			string strFacIconFile = GetGame()->GetFactionManager()->GetFacIconPath( m_FactionID );

			fstream file;
			file.open(strFacIconFile.c_str(), ios::in);
			if (file.is_open())
			{
				file.close();
				x -= (12 + 5);
				pPic = CBitmapX::GetBitmapX(strFacIconFile);
				if( pPic )
				{
					pPic->Draw(x, y, 24, 16);
	/*				if (lMaxLeft > x)
						lMaxLeft = x;*/

					x += (24 + 5);
				}
			}
		}

		//if (x<0)
		//	x = 0;
		//if (y<0)
		//	y = 0;
		//if (x > SCREEN_WIDTH-len * lFontWidth)
		//	x = SCREEN_WIDTH-len * lFontWidth;
		//if (y > SCREEN_HEIGHT-63-lFontHeight)
		//	y = SCREEN_HEIGHT-63-lFontHeight;

		if (bkBarColor)
			GetGame()->DrawSolidQuadRect(x-2, y-2, x + GetGame()->GetFontWidth(strFactionName.c_str()) + 2, y+lFontHeight+2, bkBarColor);

		if( GetGame()->GetRegion()->GetWarType() == RWT_VillageWar )	//如果是村战地图
		{
			if(GetGame()->GetMainPlayer()->IsFriendFaction(GetFactionID(),GetUnionID()) == true)
			{
				GetGame()->GetCityWarUnionBitmap()->Draw(x-GetGame()->GetCityWarUnionBitmap()->GetWidth()-2,y);
			}
			else
			{
				GetGame()->GetCityWarEnemyBitmap()->Draw(x-GetGame()->GetCityWarEnemyBitmap()->GetWidth()-2,y);
			}
		}
		else if( GetGame()->GetMainPlayer()->IsHaveCityWarEnemyFactions() )
		{
			if( GetGame()->GetMainPlayer()->IsCityWarEnemyFaction(GetFactionID()) == true )
			{
				GetGame()->GetCityWarEnemyBitmap()->Draw(x-GetGame()->GetCityWarEnemyBitmap()->GetWidth()-2,y);
			}
			else if(GetGame()->GetMainPlayer()->IsFriendFaction(GetFactionID(),GetUnionID()) == true)
			{
				GetGame()->GetCityWarUnionBitmap()->Draw(x-GetGame()->GetCityWarUnionBitmap()->GetWidth()-2,y);
			}
		}
		else if( GetGame()->GetMainPlayer()->IsHaveEnemyFactions() )
		{
			if( GetGame()->GetMainPlayer()->IsEnemyFaction(GetFactionID()) == true )
			{
				GetGame()->GetFactionWarEnemyBitmap()->Draw(x-GetGame()->GetFactionWarEnemyBitmap()->GetWidth()-2,y);
			}
			else if(GetGame()->GetMainPlayer()->IsFriendFaction(GetFactionID(),GetUnionID()) == true)
			{
				GetGame()->GetFactionWarUnionBitmap()->Draw(x-GetGame()->GetFactionWarUnionBitmap()->GetWidth()-2,y);
			}
		}

		// 显示行会名字
		if (bIsPkFlag)
		{
			GetGame()->OutPutText(x+1, y+1, strFactionName.c_str(), len, bkcolor);//0xff808080);
			GetGame()->OutPutText(x, y, strFactionName.c_str(), len, 0xffff0000);//0xff00ff00);

			//if (lMaxLeft > x)
			//	lMaxLeft = x;
		}else if (IsSelf() || GetShowCountry() || GetPkValue())
		{
			//GetGame()->OutPutText(x, y-1, strFactionName.c_str(), len, bkcolor);
			//GetGame()->OutPutText(x-1, y, strFactionName.c_str(), len, bkcolor);
			//GetGame()->OutPutText(x, y+1, strFactionName.c_str(), len, bkcolor);
			//GetGame()->OutPutText(x+1, y, strFactionName.c_str(), len, bkcolor);
			//GetGame()->OutPutText(x, y, strFactionName.c_str(), len, color);
			//GetGame()->OutPutText(x, y-1, strFactionName.c_str(), len, 0xff00ff00);
			//GetGame()->OutPutText(x-1, y, strFactionName.c_str(), len, 0xff00ff00);
			//GetGame()->OutPutText(x, y+1, strFactionName.c_str(), len, 0xff00ff00);
			//GetGame()->OutPutText(x+1, y, strFactionName.c_str(), len, 0xff00ff00);
			GetGame()->OutPutText(x+1, y+1, strFactionName.c_str(), len, bkcolor);//0xff808080);
			GetGame()->OutPutText(x, y, strFactionName.c_str(), len, color);//0xff00ff00);
		}else
		{
			GetGame()->OutPutText(x+1, y+1, strFactionName.c_str(), len, CGlobeSetup::GetSetup()->colorNoShowCountryNameBack);
			GetGame()->OutPutText(x, y, strFactionName.c_str(), len, CGlobeSetup::GetSetup()->colorNoShowCountryName);
		}

		WORD wIconType = GetDisplayOwneRegionIcon();
		if(wIconType == 2)
		{
			GetGame()->GetOwnedCityBitmap()->Draw(x-GetGame()->GetOwnedCityBitmap()->GetWidth()-2,y);
		}
		else if(wIconType == 1)
		{
			GetGame()->GetOwnedVillageBitmap()->Draw(x-GetGame()->GetOwnedVillageBitmap()->GetWidth()-2,y);
		}

		y -= (lFontHeight + 3);
		lLineCount++;
	}

	// 显示国家
	if (GetShowCountry())
	{
		if (pst)
		{
			GetGame()->GetPicList()->Display(CPicList::PT_COUNTRY_ICON, pst->lIconID, lDrawNamePos, y);
			lDrawNamePos += 30;
		}else
		{
			static D3DCOLOR colorCountry = 0xffffffff, colorCountryBack = 0xff000000;
            sprintf(strCountry, AppFrame::GetText("Base_100"));
			colorCountry	 = 0xffffffff;
			colorCountryBack = 0xff000000;

			len = lstrlen(strCountry);
			x   = x - GetGame()->GetFontWidth(strCountry);
			lDrawNamePos += GetGame()->GetFontWidth(strCountry);

			GetGame()->OutPutText(x + 1, y + 1, strCountry, len, colorCountryBack);
			GetGame()->OutPutText(x, y, strCountry, len, colorCountry);
		}
	}

	// 计算名字显示信息
	if (bPlayerNameCheck)
	{
		len = lstrlen(str);

		//if(GetFactionID() == CGUID::GUID_INVALID)
		//{
		//	if (y > SCREEN_HEIGHT-63-lFontHeight)
		//		y = SCREEN_HEIGHT-63-lFontHeight;
		//}else
		//{
		//	if (y > SCREEN_HEIGHT-63-lFontHeight*2-2)
		//		y = SCREEN_HEIGHT-63-lFontHeight*2-2;
		//}

		if (bkBarColor)
			GetGame()->DrawSolidQuadRect(lDrawNamePos - 2, y - 2, lDrawNamePos + GetGame()->GetFontWidth(str) + 2, y + lFontHeight + 2, bkBarColor);

		// 显示名字
		if (bIsPkFlag)
		{
			GetGame()->OutPutText(lDrawNamePos + 1, y + 1, str, len, bkcolor);
			GetGame()->OutPutText(lDrawNamePos, y, str, len, 0xffff0000);
		}else if (IsSelf() || GetShowCountry() || GetPkValue())
		{
			GetGame()->OutPutText(lDrawNamePos + 1, y + 1, str, len, bkcolor);
			GetGame()->OutPutText(lDrawNamePos, y, str, len, color);
		}else
		{
			color = CGlobeSetup::GetSetup()->colorNoShowCountryName;
			bkcolor = CGlobeSetup::GetSetup()->colorNoShowCountryNameBack;
			GetGame()->OutPutText(lDrawNamePos + 1, y + 1, str, len, bkcolor);
			GetGame()->OutPutText(lDrawNamePos, y, str, len, color);
		}

		//if (lMaxLeft > lDrawNamePos)
		//	lMaxLeft = lDrawNamePos;

		lDrawNamePos += GetGame()->GetFontWidth(str);
		lLineCount++;
	}

	// 王峰：绘制海盗标志，只要恶名值不为0
	if (GetPVPValue())
	{
		// 海盗图片
		if (pbmPirate)
			pbmPirate->Draw(lDrawNamePos + 3, y);
	}

	if (GetShowCountry() || bPlayerNameCheck || GetPVPValue())
		y -= (lFontHeight + 3);

	// 称号显示
	D3DCOLOR dwColor = 0;
	//if (GetGame()->GetCGuiEx()->GetGameConfigPageEx()->GetOption(CGameConfigPageEx::OPTION_ShowOfficialTitle))
	//{
	//	CGlobeSetup::tagSetup * pSetup = CGlobeSetup::GetSetup();
	//	switch (GetTitleType()/*GetGame()->GetCGuiEx()->GetCharacterPageEx()->GetTitleType()*/)
	//	{
	//	case ePTT_Hide:
	//		break;
	//	case ePTT_Knight:
	//		dwColor = pSetup->colorKnight;
	//		break;
	//	case ePTT_Faction:
	//		dwColor = color/*pSetup->colorFaction*/;
	//		break;
	//	case ePTT_Medal1:
	//	case ePTT_Medal2:
	//	case ePTT_Medal3:
	//		if (bIsMatchingNameColor)
	//			dwColor = color;
	//		else
	//			dwColor = pSetup->colorMedal;
	//		break;
	//	case ePTT_LOVER:
	//		dwColor = 0xffff63c2;
	//		break;
	//	}
	//}

	if (dwColor)
	{
		if (bIsPkFlag)
			dwColor = 0xffff0000;

		string sPlayerTitle = GetPlayerTitle()/*GetGame()->GetCGuiEx()->GetCharacterPageEx()->GetPlayerTitle()*/;
		x = (long)(pos2.x - GetGame()->GetFontWidth(sPlayerTitle.c_str()) / 2);
		GetGame()->OutPutText(x + 1, y + 1, sPlayerTitle.c_str(), (int)sPlayerTitle.length(), bkcolor);
		GetGame()->OutPutText(x, y, sPlayerTitle.c_str(), (int)sPlayerTitle.length(), dwColor);
		lLineCount++;

		//if (lMaxLeft > x)
		//	lMaxLeft = x;
	}

	// 女性标志
	//if (GetFacePic())
	//{
	//	GetGame()->GetFemaleIcon()->Draw(x - 24, y - 4);
	//}

	// 显示国王图标
	//tx = x + 10;
	//ty = y - 16;
	//if( bIsKing )
	//{
	//	GetGame()->GetKingIcon()->Draw(x+10,y-16-GetGame()->GetFontHeight()-2);
	//}

	// 显示PK场景标志
	if (bIsPkFlag)
	{
	//	y = (long)fPos - (lLineCount * lFontHeight) / 2 - GetGame()->m_pPKIcon->GetHeight() / 2 + 16;
	//	GetGame()->m_pPKIcon->Draw(lMaxLeft - GetGame()->m_pPKIcon->GetWidth(), y);
		y = (long)fPos - (lLineCount - 1) * lFontHeight - (GetGame()->m_pPKIcon->GetHeight());
		GetGame()->m_pPKIcon->Draw((long)pos2.x - GetGame()->m_pPKIcon->GetWidth() / 2, y);
	}
}

// 在shape身上显示隐藏信息后的文字
void CPlayer::DisplayHideInfo(const char* str, D3DCOLOR color, D3DCOLOR bkcolor, float fPos, D3DCOLOR bkBarColor)
{
	// 玩家头顶行会及行会称号信息开关
	bool bPlayerGuildInfoCheck = false;//GetGame()->GetCGuiEx()->GetGameConfigPageEx()->GetOption(CGameConfigPageEx::OPTION_ShowFactionName);
	D3DXVECTOR3 pos(GetPosY(), GetPickBoxHeight() + 0.2f + GetHeight(), GetPosX());
	D3DXMATRIX matFinal;
	GetGame()->GetRegion()->CalculateFinalTransformMatrix(matFinal);
	D3DXVECTOR3 pos2;
	D3DXVec3TransformCoord(&pos2,&pos,&matFinal);

	long x			 = (long)pos2.x;
	long y			 = (long)fPos;
	long lFontHeight = GetGame()->GetFontHeight(-1);
	int	 len = 0;
	long lDrawNameLineLen = 0;
	long lDrawNamePos	  = 0;
	bool bPlayerNameCheck = true;			//玩家头顶名字开关
	bool bIsMatchingNameColor = false;		//是否匹配名字颜色(针对其他信息字体颜色)

	// 对名字显示的判断
	if (this == GetGame()->GetMainPlayer())
		bPlayerNameCheck = false;//GetGame()->GetCGuiEx()->GetGameConfigPageEx()->GetOption(CGameConfigPageEx::OPTION_ShowMyName);
	else
	{
		CCountryList::tagCountryList * countrySelf	  = CCountryList::GetCountry(GetGame()->GetMainPlayer()->GetCountry());
		CCountryList::tagCountryList * countryAnother = CCountryList::GetCountry(this->GetCountry());

		if (countrySelf && countryAnother)
		{
			if (countrySelf->nLeague == countryAnother->nLeague)
			{
				if (this->GetIsConceal())
					bPlayerNameCheck = false;//GetGame()->GetCGuiEx()->GetGameConfigPageEx()->GetOption(CGameConfigPageEx::OPTION_ShowAlliedPlayerName);
			}
			else
			{
				bIsMatchingNameColor = true;
				if (this->GetIsConceal())
					bPlayerNameCheck = false;//GetGame()->GetCGuiEx()->GetGameConfigPageEx()->GetOption(CGameConfigPageEx::OPTION_ShowNotAlliedPlayerName);
			}
		}
	}
	if (m_bIsSelected)
		bPlayerNameCheck = true;

	// 国家
	bool bIsKing = false;

	CCountryList::tagCountryList * pst = CCountryList::GetCountry(GetCountry());
	CBitmapX * pbmPirate = NULL;
	CBitmapX * pSelectShape = NULL;
	static char strCountry[256];

	// 这里显示是否是国王
	// 通过行会是否拥有主城
	// 在通过该玩家是否是帮主来判定 bugs
	for( list<tagownedReg>::iterator it = m_OwnedRegions.begin(); it!=m_OwnedRegions.end(); ++it)
	{
		if(it->wType == 2) // 主城 虽然有4个国家，但每个国家只有一个主城，本国的主城只能被本国的行会打下。
		{
			if(GetExID() == m_FacMasterID)
			{
				bIsKing = true;
				break;
			}
		}
	}

	// 计算名字行显示信息像素长度
	if (bPlayerNameCheck)
		lDrawNameLineLen += (GetGame()->GetFontWidth(str) + 4);

	if (GetShowCountry())
	{
		if (pst)
			lDrawNameLineLen += 30;
		else
		{
			sprintf(strCountry, AppFrame::GetText("Base_100"));
			lDrawNameLineLen +=  GetGame()->GetFontWidth(strCountry);
		}
	}

	if (GetPVPValue())
	{
		if( GetPVPValue() < PVP_PIRATE1 )
		{
			pbmPirate = CBitmapX::GetBitmapX("pictures/state/pirate1.tga");
		}
		else if( GetPVPValue() < PVP_PIRATE2 )
		{
			pbmPirate = CBitmapX::GetBitmapX("pictures/state/pirate2.tga");
		}
		else if( GetPVPValue() < PVP_PIRATE3 )
		{
			pbmPirate = CBitmapX::GetBitmapX("pictures/state/pirate3.tga");
		}
		else
		{
			pbmPirate = CBitmapX::GetBitmapX("pictures/state/pirate4.tga");
		}
		if (pbmPirate)
			lDrawNameLineLen += pbmPirate->GetWidth();
	}

	// 名字行箭头显示
	if (m_bIsSelected)
	{
		long lTmpY;
		pSelectShape = GetGame()->m_pSelectShape;
		DWORD dwW2	 = pSelectShape->GetWidth() / 2;
		DWORD dwH	 = pSelectShape->GetHeight();
		RECT  rc	 = { 0, 0, dwW2, dwH };
		lDrawNamePos = (long)pos2.x - dwW2 - lDrawNameLineLen / 2;		//计算名字行绘制X起始坐标

		if (GetFactionID() != CGUID::GUID_INVALID  && GetFactionID() == GetGame()->GetMainPlayer()->GetFactionID() && bPlayerGuildInfoCheck)
			lTmpY = y - lFontHeight - 3;
		else
			lTmpY = y;

		GetGame()->m_pSelectShape->Draw(lDrawNamePos - 8, lTmpY, rc);

		rc.left  = dwW2;
		rc.right = dwW2 * 2;

		GetGame()->m_pSelectShape->Draw(lDrawNamePos + 8 + dwW2 + lDrawNameLineLen, lTmpY, rc);
		lDrawNamePos += dwW2;
	}else
		lDrawNamePos = (long)pos2.x - lDrawNameLineLen / 2;

	// 本行会会员，显示行会
	if(GetFactionID() != CGUID::GUID_INVALID && GetFactionID() == GetGame()->GetMainPlayer()->GetFactionID() && bPlayerGuildInfoCheck)
	{
		CBitmapX * pPic		  = NULL;
		string strFactionName =  "<";
		strFactionName		  += GetFactionName();
		strFactionName		  += ">";
		len = (int)strFactionName.length();
		x = (long)pos2.x - GetGame()->GetFontWidth(strFactionName.c_str()) / 2;

		if( NULL_GUID != m_FactionID )
		{
			string strFacIconFile = GetGame()->GetFactionManager()->GetFacIconPath( m_FactionID );

			fstream file;
			file.open(strFacIconFile.c_str(), ios::in);
			if (file.is_open())
			{
				file.close();
				x -= (12 + 5);
				pPic = CBitmapX::GetBitmapX(strFacIconFile);
				if( pPic )
				{
					pPic->Draw(x, y, 24, 16);
					x += (24 + 5);
				}
			}
		}
		// 显示行会名字
		if (IsSelf() || GetShowCountry() || GetPkValue())
		{
			GetGame()->OutPutText(x+1, y+1, strFactionName.c_str(), len, bkcolor);
			GetGame()->OutPutText(x, y, strFactionName.c_str(), len, color);
		}else
		{
			GetGame()->OutPutText(x+1, y+1, strFactionName.c_str(), len, CGlobeSetup::GetSetup()->colorNoShowCountryNameBack);
			GetGame()->OutPutText(x, y, strFactionName.c_str(), len, CGlobeSetup::GetSetup()->colorNoShowCountryName);
		}
		y -= (lFontHeight + 3);
	}

	// 显示国家
	if (GetShowCountry())
	{
		if (pst)
		{
			GetGame()->GetPicList()->Display(CPicList::PT_COUNTRY_ICON, pst->lIconID, lDrawNamePos, y);
			lDrawNamePos += 30;
		}else
		{
			static D3DCOLOR colorCountry = 0xffffffff, colorCountryBack = 0xff000000;
			sprintf(strCountry, AppFrame::GetText("Base_100"));
			colorCountry	 = 0xffffffff;
			colorCountryBack = 0xff000000;

			len = lstrlen(strCountry);
			x   = x - GetGame()->GetFontWidth(strCountry);
			lDrawNamePos += GetGame()->GetFontWidth(strCountry) * lstrlen(strCountry);

			GetGame()->OutPutText(x + 1, y + 1, strCountry, len, colorCountryBack);
			GetGame()->OutPutText(x, y, strCountry, len, colorCountry);
		}
	}

	// 计算名字显示信息
	if (bPlayerNameCheck)
	{
		len = lstrlen(str);
		// 显示名字
		if (IsSelf() || GetShowCountry() || GetPkValue())
		{
			GetGame()->OutPutText(lDrawNamePos + 1, y + 1, str, len, bkcolor);
			GetGame()->OutPutText(lDrawNamePos, y, str, len, color);
		}else
		{
			color = CGlobeSetup::GetSetup()->colorNoShowCountryName;
			bkcolor = CGlobeSetup::GetSetup()->colorNoShowCountryNameBack;
			GetGame()->OutPutText(lDrawNamePos + 1, y + 1, str, len, bkcolor);
			GetGame()->OutPutText(lDrawNamePos, y, str, len, color);
		}

		lDrawNamePos += GetGame()->GetFontWidth(str);
	}

	// 王峰：绘制海盗标志，只要恶名值不为0
	if (GetPVPValue())
	{
		// 海盗图片
		if (pbmPirate)
			pbmPirate->Draw(lDrawNamePos + 3, y);
	}
}

/////////////////////////套装接口/////////////////////////////////////////////////////////
//判断人物是否有此ID的套装
bool CPlayer::IsSuitBeHold(WORD SuitID)
{
	map<WORD,tagSuitAttribute_P>::iterator iter = m_SuitList.find(SuitID);
	if (iter != m_SuitList.end())
	{
		return true;
	}
	return false;
}

//判断是否有此ID的装备被穿上了,true表示至少有一件装备被穿上了，false表示一件都没有
bool CPlayer::IsSuitBeSetuped(WORD SuitID)
{
	if (IsSuitBeHold(SuitID))
	{
		map<WORD,tagSuitAttribute_P>::iterator iter = m_SuitList.find(SuitID);
		if (iter != m_SuitList.end())
		{
			tagSuitAttribute_P temp = iter->second;
			map<string,bool>::iterator iiter = temp.Suit_Equit.begin();
			for (;iiter!=temp.Suit_Equit.end();iiter++)
			{
				if (iiter->second)
				{
					return true;
				}			
			}
		}
		m_SuitList[SuitID].Suit_SetupedNum = 0;
	}
	return false;
}

//将此SuitID的套装加入m_SuitList中，并把第一件装备的原始名加入Suit_OriginName中
void CPlayer::AddSuit(WORD SuitID,string SuitOriginName)			
{
	if (!IsSuitBeHold(SuitID))
	{
		tagSuitAttribute_P temp;
		temp.Suit_Num = 1;
		temp.Suit_SetupedNum = 0;
		temp.Suit_Equit[SuitOriginName] = false;
		m_SuitList.insert(pair<WORD,tagSuitAttribute_P>(SuitID,temp));
	}else if(!CompareSuitOriginName(SuitID,SuitOriginName))
	{
		m_SuitList[SuitID].Suit_Equit.insert(pair<string,bool>(SuitOriginName,false));
		m_SuitList[SuitID].Suit_Num++;
		m_SuitList[SuitID].Suit_Equit[SuitOriginName] = false;
	}
}

//比较判断已拥有的套装中是否有此原始名的装备
bool CPlayer::CompareSuitOriginName(WORD SuitID,string SuitOriginName)
{
	if (IsSuitBeHold(SuitID))
	{	
		map<string,bool>::iterator it = m_SuitList[SuitID].Suit_Equit.find(SuitOriginName);
		if (it!=m_SuitList[SuitID].Suit_Equit.end())
		{
			return true;
		}
	}
	return false;
}

//获取此ID的套装已拥有的件数
WORD CPlayer::GetNumOfPossessionSuit(WORD SuitID)									
{
	if (SuitID)
	{
		if (IsSuitBeHold(SuitID))
		{	
			return m_SuitList[SuitID].Suit_Num;
		}
	}
	return 0;
}

//获取此ID的套装已装备上的件数
WORD CPlayer::GetNumOfSetupEquip(WORD SuitID)	
{
	if (SuitID)
	{
		if (IsSuitBeSetuped(SuitID))
		{
			return m_SuitList[SuitID].Suit_SetupedNum;
		}
	}
	return 0;
}

//void CPlayer::AddEquipToSuit(WORD SuitID,string SuitOriginName)													//将此原始名的装备加入对应ID的套装列表中
//{
//	if (IsSuitBeHold(SuitID))
//	{
//		if (!CompareSuitOriginName(SuitID,SuitOriginName))
//		{
//			m_SuitList[SuitID].Suit_Equit.insert(pair<string,bool>(SuitOriginName,false));
//			m_SuitList[SuitID].Suit_Num++;
//		}
//	}
//}


//检查此原始名的装备是否已被装备上
bool CPlayer::CompareSetEquipOriName(WORD SuitID,string SuitOriginName)
{
	if (IsSuitBeHold(SuitID))
	{
		if (CompareSuitOriginName(SuitID,SuitOriginName))
		{
			if (m_SuitList[SuitID].Suit_Equit[SuitOriginName])
			{
				return true;
			}
		}
	}
	return false;
}

//将此原始名的装备穿在身上
void CPlayer::SetupEquip(WORD SuitID,string SuitOriginName)
{
	if (IsSuitBeSetuped(SuitID))
	{
		if (!CompareSetEquipOriName(SuitID,SuitOriginName))
		{
			m_SuitList[SuitID].Suit_SetupedNum++;
		}
	}else
	{
		m_SuitList[SuitID].Suit_SetupedNum = 1;
	}
	AddSuit(SuitID,SuitOriginName);
	m_SuitList[SuitID].Suit_Equit[SuitOriginName] = true;
}

//检查装备栏中此ID原始名为SuitOriginName的物品数量
WORD CPlayer::FindSetupEquipForOriName(WORD SuitID,string SuitOriginName)
{
	WORD num = 0;
	//在装备中查找
	for(int i=0;i<CPlayer::EQUIP_SUM;i++)
	{
		CGoods* pEquip=GetEquip(i);
		if(pEquip)
		{
			if(pEquip->GetEffectVal(GAP_SUITID,1) == SuitID&&
				pEquip->GetProperty()->strOrginName == SuitOriginName)
			{
				num++;
			}
		}
	}
	return num;
}

//将此原始名的装备从身上脱下
void CPlayer::UnfixEquip(WORD SuitID,string SuitOriginName)
{
	if (m_SuitList[SuitID].Suit_Equit[SuitOriginName]&&
		FindSetupEquipForOriName(SuitID,SuitOriginName)==0)
	{
		m_SuitList[SuitID].Suit_Equit[SuitOriginName] = false;
		m_SuitList[SuitID].Suit_SetupedNum--;
	}
}

//判断此ID的套装是否已凑齐
bool CPlayer::IsSuitComplete(WORD SuitID)
{
	WORD Num = (WORD)CGoodsList::GetSuitCount(SuitID);
	if (m_SuitList[SuitID].Suit_Num == Num)
	{
		return true;
	}
	return false;
}

//查找人物身上有多少此ID原始名为SuitOriginName的物品
WORD CPlayer::FindEquipByOrName(WORD SuitID,string SuitOriginName)
{
	WORD num = 0;
	//if(m_stHand.pGoods && m_stHand.pGoods->GetEffectVal(CGoods::GAP_SUITID,1) == SuitID&&
	//	m_stHand.pGoods->GetProperty()->strOrginName==SuitOriginName)
	//	num++;

	//在装备中查找
	for(int i=0;i<CPlayer::EQUIP_SUM;i++)
	{
		CGoods* pEquip=GetEquip(i);
		if(pEquip)
		{
			if(pEquip->GetEffectVal(GAP_SUITID,1) == SuitID&&
				pEquip->GetProperty()->strOrginName == SuitOriginName)
			{
				num++;
			}
		}
	}
	//在背包中查找
	list<tagGoods>* pGoodList = GetGoodsList();
	if (pGoodList)
	{
		tagGoodsIter GoodsIter=pGoodList->begin();
		for(;GoodsIter!=pGoodList->end();GoodsIter++)
		{
			CGoods* pGoods = GoodsIter->pGoods;
			if(pGoods->GetEffectVal(GAP_SUITID,1) == SuitID&&
				pGoods->GetProperty()->strOrginName == SuitOriginName)
			{
				num++;
			}
		}
	}
	//查询子背包
	for (int i = 0;i<COTE_NUM;i++)
	{	
		//在子背包中查找
		pGoodList = GetSubGoodslist(i);
		if (pGoodList)
		{
			tagGoodsIter GoodsIter=pGoodList->begin();
			for(;GoodsIter!=pGoodList->end();GoodsIter++)
			{
				CGoods* pGoods = GoodsIter->pGoods;
				if(pGoods)
				{
					if(pGoods->GetEffectVal(GAP_SUITID,1) == SuitID&&
						pGoods->GetProperty()->strOrginName == SuitOriginName)
					{
						num++;
					}
				}
			}
		}
	}

	//在仓库中查找
	/*list<tagGoods> *pDepotGoodsList=GetGame()->GetInterface()->GetDepotPage()->GetDepotList();
	GoodsIter=pDepotGoodsList->begin();
	for(;GoodsIter!=pDepotGoodsList->end();GoodsIter++)
	{
	CGoods* pGoods = GoodsIter->pGoods;
	if(pGoods && pGoods->GetExID() == guID)
	{	
	if(pPos)pPos[0]=GoodsIter->lPos;
	return pGoods;
	}
	}*/
	//在个人商店里查找

	return num;
}

//把此ID的套装中原始名为SuitOriginName的装备从tagSuitAttribute_P中删除
bool CPlayer::RemoveEquipFromSuit(WORD SuitID,string SuitOriginName)
{
	if (FindEquipByOrName(SuitID,SuitOriginName)==0)
	{
		map<string,bool>::iterator iter = m_SuitList[SuitID].Suit_Equit.find(SuitOriginName);
		if (iter!=m_SuitList[SuitID].Suit_Equit.end())
		{
			m_SuitList[SuitID].Suit_Equit.erase(iter);
			m_SuitList[SuitID].Suit_Num--;
			return true;
		}
	}
	return false;
}

//把此ID的套装从m_SuitList中删除
bool CPlayer::RemoveSuit(WORD SuitID)
{
	if(m_SuitList[SuitID].Suit_Num==0)
	{
		map<WORD,tagSuitAttribute_P>::iterator iter = m_SuitList.find(SuitID);
		m_SuitList.erase(iter);
		return true;
	}
	return false;
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//更新坐骑的显示
void CPlayer::UpdateRideShow()
{
	extern bool g_bForceUpdate;
	g_bForceUpdate = true;
	if (IsRide())
	{
		DWORD index = 0;
		SetAction(ACT_STAND);
		if (m_bSelf)
		{
			CGoods* pgoods = GetEquip(CPlayer::EQUIP_RIDER);
			if (pgoods)
			{
				index = GetEquip(CPlayer::EQUIP_RIDER)->GetIndex();
			}

		}
		else
		{
			index = GetEquipIndex(CPlayer::EQUIP_RIDER);
		}
		CGoodsList::tagGoods2* pGoods2 = CGoodsList::GetProperty(index);
		if (pGoods2)
		{
			m_Horse.SetHorseType(pGoods2->BaseProperty.dwWeaponActType);
			m_Horse.SetModelID(pGoods2->BaseProperty.dwEquipID);
			m_Horse.ChangeStatus(GetAction(),true);
			m_Horse.SetVisible(true);
			m_Horse.SetDisplayModel();
		}
		else
		{
			m_Horse.SetHorseType(0);
			m_Horse.SetModelID(0);
			m_Horse.SetVisible(false);

		}

	}
	else
	{
		m_Horse.SetHorseType(0);
		m_Horse.SetModelID(0);
		m_Horse.SetVisible(false);
		SetAction(GetAction());
	}
	SetState(GetState());
	GetAnimInfo()->GetActionInfo()->bInterpolation = FALSE;
}
//////////////////////////////////////////////////////////////////////////
//更新装备模型的显示
void CPlayer::UpdateAllWeaponShow()
{
	//***********
	//更新显示武器ADD by 彭谦
	//***********
	long mod1ID = 0;
	long mod2ID = 0;

	CGoods* prequip = GetEquip(CPlayer::EQUIP_WEAPON);
//	CGoods* psequip = GetEquip(CPlayer::EQUIP_WEAPON2);
	CGoods* pwequip = GetEquip(CPlayer::EQUIP_WING);
	CGoods* phequip = GetEquip(CPlayer::EQUIP_P_HEADGEAR);
	DWORD RLevel = 0;
	DWORD LLevel = 0;
	DWORD WLevel = 0;
	DWORD HLevel = 0;
	CGoodsList::tagEffect* ptegeffect;
	CGoodsList::tagGoods2* taggoods2;
	if (prequip != NULL)
	{
		RLevel = prequip->GetEffectVal(GAP_WEAPON_LEVEL,1);
		taggoods2 = CGoodsList::GetProperty(prequip->GetIndex());
		if(prequip->GetEquipType() == EC_E_WEAPON)
		{

			long ltype = prequip->GetEffectVal(GAP_WEAPON_CATEGORY,1);

			switch(ltype)
			{
				//单手剑
				//单手锤
				//匕首
			case WT_SINGLE_SWORD:
				m_RightHandWeapon.SetWeaponType(CWeapon::WEAPONSHOW_SINGLESWORD);
				mod1ID = taggoods2->BaseProperty.dwEquipID;
				m_RightHandWeapon.SetModelID(mod1ID);
				m_RightHandWeapon.SetVisible(true);
				m_RightHandWeapon.SetDisplayModel();
				m_RightHandWeapon.SetLevel(RLevel);
				break;
			case WT_SINGLE_HAMMER:
				m_RightHandWeapon.SetWeaponType(CWeapon::WEAPONSHOW_SINGLEHAMMER);
				mod1ID = taggoods2->BaseProperty.dwEquipID;
				m_RightHandWeapon.SetModelID(mod1ID);
				m_RightHandWeapon.SetVisible(true);
				m_RightHandWeapon.SetDisplayModel();
				m_RightHandWeapon.SetLevel(RLevel);
				break;
			case WT_SINGLE_KNIFE:
				m_RightHandWeapon.SetWeaponType(CWeapon::WEAPONSHOW_SINGLEKNIFE);
				mod1ID = taggoods2->BaseProperty.dwEquipID;
				m_RightHandWeapon.SetModelID(mod1ID);
				m_RightHandWeapon.SetVisible(true);
				m_RightHandWeapon.SetDisplayModel();
				m_RightHandWeapon.SetLevel(RLevel);
				break;
				//巨剑
			case WT_HEAVY_SWORD:
				m_RightHandWeapon.SetWeaponType(CWeapon::WEAPONSHOW_BIGSWORD);
				mod1ID = taggoods2->BaseProperty.dwEquipID;
				m_RightHandWeapon.SetModelID(mod1ID);
				m_RightHandWeapon.SetVisible(true);
				m_RightHandWeapon.SetDisplayModel();
				m_RightHandWeapon.SetLevel(RLevel);
				break;
				//法杖
				//大斧
			case WT_WAND:
				m_RightHandWeapon.SetWeaponType(CWeapon::WEAPONSHOW_BIGWANG);
				mod1ID = taggoods2->BaseProperty.dwEquipID;
				m_RightHandWeapon.SetModelID(mod1ID);
				m_RightHandWeapon.SetVisible(true);
				m_RightHandWeapon.SetDisplayModel();
				m_RightHandWeapon.SetLevel(RLevel);
				break;
			case WT_HEAVY_AX:
				m_RightHandWeapon.SetWeaponType(CWeapon::WEAPONSHOW_BIGAX);
				mod1ID = taggoods2->BaseProperty.dwEquipID;
				m_RightHandWeapon.SetModelID(mod1ID);
				m_RightHandWeapon.SetVisible(true);
				m_RightHandWeapon.SetDisplayModel();
				m_RightHandWeapon.SetLevel(RLevel);
				break;
				//弓
			case WT_BOW:
				//左手
				ptegeffect = CGoodsList::GetEffectTag(prequip->GetIndex(),GAP_BOTHHAND_GRAPHIC);
				mod1ID = ptegeffect->dwValue1;
				m_RightHandWeapon.SetWeaponType(CWeapon::WEAPONSHOW_BOW);
				m_RightHandWeapon.SetModelID(mod1ID);
				m_RightHandWeapon.SetVisible(true);
				m_RightHandWeapon.SetDisplayModel();
				m_RightHandWeapon.SetLevel(RLevel);
				//箭带,还未加功能
				/*m_RightHandWeapon.SetWeaponType(CWeapon::WEAPONSHOW_BOW);
				mod2ID = pGoods->GetEffectVal(CGoods::GAP_COSPLAYID,2);
				m_RightHandWeapon.SetModelID(mod1ID);
				m_RightHandWeapon.SetVisible(true);*/
				break;
				//双手持剑
			case WT_DOUBLE_SWORD:
				//左手
				ptegeffect = CGoodsList::GetEffectTag(prequip->GetIndex(),GAP_BOTHHAND_GRAPHIC);
				mod1ID = ptegeffect->dwValue1;
				m_LeftHandWeapon.SetWeaponType(CWeapon::WEAPONSHOW_DOUBLEWEAPON);
				m_LeftHandWeapon.SetModelID(mod1ID);
				m_LeftHandWeapon.SetVisible(true);
				m_LeftHandWeapon.SetDisplayModel();
				m_LeftHandWeapon.SetLevel(RLevel);
				//右手
				m_RightHandWeapon.SetWeaponType(CWeapon::WEAPONSHOW_DOUBLEWEAPON);
				mod2ID = ptegeffect->dwValue2;
				m_RightHandWeapon.SetModelID(mod2ID);
				m_RightHandWeapon.SetVisible(true);
				m_RightHandWeapon.SetDisplayModel();
				m_RightHandWeapon.SetLevel(RLevel);
				break;
				//拳套
			case WT_FIST:
				//左手
				ptegeffect = CGoodsList::GetEffectTag(prequip->GetIndex(),GAP_BOTHHAND_GRAPHIC);
				mod1ID = ptegeffect->dwValue1;
				m_LeftHandWeapon.SetWeaponType(CWeapon::WEAPONSHOW_BOXINGLEFT);
				m_LeftHandWeapon.SetModelID(mod1ID);
				m_LeftHandWeapon.SetVisible(true);
				m_LeftHandWeapon.SetDisplayModel();
				m_LeftHandWeapon.SetLevel(RLevel);
				//右手
				m_RightHandWeapon.SetWeaponType(CWeapon::WEAPONSHOW_BOXINGRIGHT);
				mod2ID = ptegeffect->dwValue2;
				m_RightHandWeapon.SetModelID(mod2ID);
				m_RightHandWeapon.SetVisible(true);
				m_RightHandWeapon.SetDisplayModel();
				m_RightHandWeapon.SetLevel(RLevel);
				break;


			}



		}
	}
	/*if (psequip != NULL)
	{
		LLevel = psequip->GetEffectVal(CGoods::GAP_WEAPON_LEVEL,1);
		if(CGoodsList::GetProperty(psequip->GetIndex())->BaseProperty.dwType == CGoodsList::GT_WEAPON2)
		{
			m_LeftHandWeapon.SetWeaponType(CWeapon::WEAPONSHOW_SHIELD);
			mod1ID = CGoodsList::GetProperty(psequip->GetIndex())->BaseProperty.dwEquipID;
			m_LeftHandWeapon.SetModelID(mod1ID);
			m_LeftHandWeapon.SetVisible(true);
			m_LeftHandWeapon.SetDisplayModel();
			m_LeftHandWeapon.SetLevel(LLevel);
		}
	}*/
	if (pwequip != NULL)
	{
		WLevel = pwequip->GetEffectVal(GAP_WEAPON_LEVEL,1);
		if(pwequip->GetEquipType() == EC_E_WING)
		{
			mod1ID = CGoodsList::GetProperty(pwequip->GetIndex())->BaseProperty.dwEquipID;
			m_Wing.SetModelID(mod1ID);
			m_Wing.SetVisible(true);
			m_Wing.SetDisplayModel();
			m_Wing.SetLevel(WLevel);
		}
	}
	if (phequip != NULL)
	{
		HLevel = phequip->GetEffectVal(GAP_WEAPON_LEVEL,1);
		if(phequip->GetEquipType() == EC_E_HEAD)
		{
			mod1ID = CGoodsList::GetProperty(phequip->GetIndex())->BaseProperty.dwEquipID;
			m_HeadWear.SetModelID(mod1ID);
			m_HeadWear.SetVisible(true);
			m_HeadWear.SetDisplayModel();
			m_HeadWear.SetLevel(HLevel);
		}
	}
	extern bool g_bForceUpdate;
	g_bForceUpdate = true;
	SetAction(GetAction());
	g_bForceUpdate = false;
	//***********
	//更新显示武器ADD by 彭谦
	//***********

}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//更新装备模型的显示
void CPlayer::UpdateWeaponShow(CGoods* pGoods)
{
	if (pGoods == NULL)
	{
		return;
	}
	//***********
	//更新显示武器ADD by 彭谦
	//***********
	long mod1ID = 0;
	long mod2ID = 0;
	DWORD index = pGoods->GetIndex();
	long level = 0;
	CGoodsList::tagEffect* ptegeffect;
	CGoodsList::tagGoods2* ptaggoods2 = CGoodsList::GetProperty(index);
	if (ptaggoods2)
	{
		eEquipType equipType = pGoods->GetEquipType();
		if(equipType == EC_E_WEAPON)
		{

			long ltype = pGoods->GetEffectVal(GAP_WEAPON_CATEGORY,1);
			level = pGoods->GetEffectVal(GAP_WEAPON_LEVEL,1);

			switch(ltype)
			{
				//单手剑
				//单手锤
				//匕首
			case WT_SINGLE_SWORD:
				m_RightHandWeapon.SetWeaponType(CWeapon::WEAPONSHOW_SINGLESWORD);
				mod1ID = ptaggoods2->BaseProperty.dwEquipID;
				m_RightHandWeapon.SetModelID(mod1ID);
				m_RightHandWeapon.SetVisible(true);
				m_RightHandWeapon.SetDisplayModel();
				m_RightHandWeapon.SetLevel(level);
				break;
			case WT_SINGLE_HAMMER:
				m_RightHandWeapon.SetWeaponType(CWeapon::WEAPONSHOW_SINGLEHAMMER);
				mod1ID = ptaggoods2->BaseProperty.dwEquipID;
				m_RightHandWeapon.SetModelID(mod1ID);
				m_RightHandWeapon.SetVisible(true);
				m_RightHandWeapon.SetDisplayModel();
				m_RightHandWeapon.SetLevel(level);
				break;
			case WT_SINGLE_KNIFE:
				m_RightHandWeapon.SetWeaponType(CWeapon::WEAPONSHOW_SINGLEKNIFE);
				mod1ID = ptaggoods2->BaseProperty.dwEquipID;
				m_RightHandWeapon.SetModelID(mod1ID);
				m_RightHandWeapon.SetVisible(true);
				m_RightHandWeapon.SetDisplayModel();
				m_RightHandWeapon.SetLevel(level);
				break;
				//巨剑
			case WT_HEAVY_SWORD:
				m_RightHandWeapon.SetWeaponType(CWeapon::WEAPONSHOW_BIGSWORD);
				mod1ID = ptaggoods2->BaseProperty.dwEquipID;
				m_RightHandWeapon.SetModelID(mod1ID);
				m_RightHandWeapon.SetVisible(true);
				m_RightHandWeapon.SetDisplayModel();
				m_RightHandWeapon.SetLevel(level);
				break;
				//法杖
				//大斧
			case WT_WAND:
				m_RightHandWeapon.SetWeaponType(CWeapon::WEAPONSHOW_BIGWANG);
				mod1ID = ptaggoods2->BaseProperty.dwEquipID;
				m_RightHandWeapon.SetModelID(mod1ID);
				m_RightHandWeapon.SetVisible(true);
				m_RightHandWeapon.SetDisplayModel();
				m_RightHandWeapon.SetLevel(level);
				break;
			case WT_HEAVY_AX:
				m_RightHandWeapon.SetWeaponType(CWeapon::WEAPONSHOW_BIGAX);
				mod1ID = ptaggoods2->BaseProperty.dwEquipID;
				m_RightHandWeapon.SetModelID(mod1ID);
				m_RightHandWeapon.SetVisible(true);
				m_RightHandWeapon.SetDisplayModel();
				m_RightHandWeapon.SetLevel(level);
				break;
				//弓
			case WT_BOW:
				//主手
				ptegeffect = CGoodsList::GetEffectTag(index,GAP_BOTHHAND_GRAPHIC);
				mod1ID = ptegeffect->dwValue1;
				m_RightHandWeapon.SetWeaponType(CWeapon::WEAPONSHOW_BOW);
				m_RightHandWeapon.SetModelID(mod1ID);
				m_RightHandWeapon.SetVisible(true);
				m_RightHandWeapon.SetDisplayModel();
				m_RightHandWeapon.SetLevel(level);
				//箭带,还未加功能
				m_LeftHandWeapon.SetWeaponType(0);
				//mod2ID = pGoods->GetEffectVal(0);
				m_LeftHandWeapon.SetModelID(0);
				m_LeftHandWeapon.SetVisible(false);
				m_LeftHandWeapon.SetDisplayModel();
				m_LeftHandWeapon.SetLevel(0);
				break;

				//双手持剑
			case WT_DOUBLE_SWORD:
				//左手
				ptegeffect = CGoodsList::GetEffectTag(index,GAP_BOTHHAND_GRAPHIC);
				mod1ID = ptegeffect->dwValue1;
				m_LeftHandWeapon.SetWeaponType(CWeapon::WEAPONSHOW_DOUBLEWEAPON);
				m_LeftHandWeapon.SetModelID(mod1ID);
				m_LeftHandWeapon.SetVisible(true);
				m_LeftHandWeapon.SetDisplayModel();
				m_LeftHandWeapon.SetLevel(level);
				//右手
				mod2ID = ptegeffect->dwValue2;
				m_RightHandWeapon.SetWeaponType(CWeapon::WEAPONSHOW_DOUBLEWEAPON);
				m_RightHandWeapon.SetModelID(mod2ID);
				m_RightHandWeapon.SetVisible(true);
				m_RightHandWeapon.SetDisplayModel();
				m_RightHandWeapon.SetLevel(level);
				break;
				//拳套
			case WT_FIST:
				//左手
				ptegeffect = CGoodsList::GetEffectTag(index,GAP_BOTHHAND_GRAPHIC);
				mod1ID = ptegeffect->dwValue1;
				m_LeftHandWeapon.SetWeaponType(CWeapon::WEAPONSHOW_BOXINGLEFT);
				m_LeftHandWeapon.SetModelID(mod1ID);
				m_LeftHandWeapon.SetVisible(true);
				m_LeftHandWeapon.SetDisplayModel();
				m_LeftHandWeapon.SetLevel(level);
				//右手
				mod2ID = ptegeffect->dwValue2;
				m_RightHandWeapon.SetWeaponType(CWeapon::WEAPONSHOW_BOXINGRIGHT);
				mod2ID = pGoods->GetEffectVal(GAP_BOTHHAND_GRAPHIC,2);
				m_RightHandWeapon.SetModelID(mod2ID);
				m_RightHandWeapon.SetVisible(true);
				m_RightHandWeapon.SetDisplayModel();
				m_RightHandWeapon.SetLevel(level);

				break;

			}




		}
		// 护符（盾牌）
		else if(equipType == EC_E_CARD)
		{

			level = pGoods->GetEffectVal(GAP_WEAPON_LEVEL,1);
			mod1ID = ptaggoods2->BaseProperty.dwEquipID;
			m_LeftHandWeapon.SetWeaponType(CWeapon::WEAPONSHOW_SHIELD);
			m_LeftHandWeapon.SetModelID(mod1ID);
			m_LeftHandWeapon.SetVisible(true);
			m_LeftHandWeapon.SetDisplayModel();
			m_LeftHandWeapon.SetLevel(level);
		}
		else if(equipType == EC_E_WING)
		{

			level = pGoods->GetEffectVal(GAP_WEAPON_LEVEL,1);
			mod1ID = ptaggoods2->BaseProperty.dwEquipID;
			m_Wing.SetModelID(mod1ID);
			m_Wing.SetVisible(true);
			m_Wing.SetDisplayModel();
			m_Wing.SetLevel(level);
		}
		else if(equipType == EC_E_HEAD)
		{

			level = pGoods->GetEffectVal(GAP_WEAPON_LEVEL,1);
			mod1ID = ptaggoods2->BaseProperty.dwEquipID;
			m_HeadWear.SetModelID(mod1ID);
			m_HeadWear.SetVisible(true);
			m_HeadWear.SetDisplayModel();
			m_HeadWear.SetLevel(level);
		}


		extern bool g_bForceUpdate;
		g_bForceUpdate = true;
		if (GetAction() != CShape::ACT_PICK)
		{
			SetAction(GetAction());
		}
		g_bForceUpdate = false;
		//***********
		//更新显示武器ADD by 彭谦
		//***********
	}


}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//更新装备模型的显示
void CPlayer::UpdateWeaponShow(DWORD dwPos)
{

	//***********
	//更新显示武器ADD by 彭谦
	//***********
	DWORD index = m_dwEquipIndex[dwPos];
	DWORD level = m_btEquipLevel[dwPos];
	CGoodsList::tagGoods2* pGoods = CGoodsList::GetProperty(index);
	if (pGoods == NULL)
	{
		return;
	}
	long mod1ID = 0;
	long mod2ID = 0;

	long equipType = CGoodsList::GetEffectTag(index,GAP_EQUIP_CATEGORY)->dwValue1;
	if(equipType == EC_E_WEAPON)
	{

		CGoodsList::tagEffect* ptegeffect = CGoodsList::GetEffectTag(index,GAP_WEAPON_CATEGORY);
		long ltype = ptegeffect->dwValue1;
		switch(ltype)
		{
			//单手剑
			//单手锤
			//匕首

		case WT_SINGLE_SWORD:
			m_RightHandWeapon.SetWeaponType(CWeapon::WEAPONSHOW_SINGLESWORD);
			mod1ID = pGoods->BaseProperty.dwEquipID;
			m_RightHandWeapon.SetModelID(mod1ID);
			m_RightHandWeapon.SetVisible(true);
			m_RightHandWeapon.SetDisplayModel();
			m_RightHandWeapon.SetLevel(level);
			break;
		case WT_SINGLE_HAMMER:
			m_RightHandWeapon.SetWeaponType(CWeapon::WEAPONSHOW_SINGLEHAMMER);
			mod1ID = pGoods->BaseProperty.dwEquipID;
			m_RightHandWeapon.SetModelID(mod1ID);
			m_RightHandWeapon.SetVisible(true);
			m_RightHandWeapon.SetDisplayModel();
			m_RightHandWeapon.SetLevel(level);
			break;
		case WT_SINGLE_KNIFE:
			m_RightHandWeapon.SetWeaponType(CWeapon::WEAPONSHOW_SINGLEKNIFE);
			mod1ID = pGoods->BaseProperty.dwEquipID;
			m_RightHandWeapon.SetModelID(mod1ID);
			m_RightHandWeapon.SetVisible(true);
			m_RightHandWeapon.SetDisplayModel();
			m_RightHandWeapon.SetLevel(level);
			break;
			//巨剑
		case WT_HEAVY_SWORD:
			m_RightHandWeapon.SetWeaponType(CWeapon::WEAPONSHOW_BIGSWORD);
			mod1ID = pGoods->BaseProperty.dwEquipID;
			m_RightHandWeapon.SetModelID(mod1ID);
			m_RightHandWeapon.SetVisible(true);
			m_RightHandWeapon.SetDisplayModel();
			m_RightHandWeapon.SetLevel(level);
			break;
			//法杖
			//大斧
		case WT_WAND:
			m_RightHandWeapon.SetWeaponType(CWeapon::WEAPONSHOW_BIGWANG);
			mod1ID = pGoods->BaseProperty.dwEquipID;
			m_RightHandWeapon.SetModelID(mod1ID);
			m_RightHandWeapon.SetVisible(true);
			m_RightHandWeapon.SetDisplayModel();
			m_RightHandWeapon.SetLevel(level);
			break;
		case WT_HEAVY_AX:
			m_RightHandWeapon.SetWeaponType(CWeapon::WEAPONSHOW_BIGAX);
			mod1ID = pGoods->BaseProperty.dwEquipID;
			m_RightHandWeapon.SetModelID(mod1ID);
			m_RightHandWeapon.SetVisible(true);
			m_RightHandWeapon.SetDisplayModel();
			m_RightHandWeapon.SetLevel(level);
			break;
			//弓
		case WT_BOW:
			ptegeffect = CGoodsList::GetEffectTag(index,GAP_BOTHHAND_GRAPHIC);
			ASSERT(ptegeffect);
			//左手
			m_RightHandWeapon.SetWeaponType(CWeapon::WEAPONSHOW_BOW);
			mod1ID = ptegeffect->dwValue1;
			m_RightHandWeapon.SetModelID(mod1ID);
			m_RightHandWeapon.SetVisible(true);
			m_RightHandWeapon.SetDisplayModel();
			m_RightHandWeapon.SetLevel(level);
			//箭带,还未加功能
			m_LeftHandWeapon.SetWeaponType(0);
			m_LeftHandWeapon.SetModelID(0);
			m_LeftHandWeapon.SetVisible(false);
			m_LeftHandWeapon.SetDisplayModel();
			m_LeftHandWeapon.SetLevel(0);
			break;

			//双手持剑
		case WT_DOUBLE_SWORD:
			ptegeffect = CGoodsList::GetEffectTag(index,GAP_BOTHHAND_GRAPHIC);
			ASSERT(ptegeffect);
			//左手
			m_LeftHandWeapon.SetWeaponType(CWeapon::WEAPONSHOW_DOUBLEWEAPON);
			mod1ID = ptegeffect->dwValue1;
			m_LeftHandWeapon.SetModelID(mod1ID);
			m_LeftHandWeapon.SetVisible(true);
			m_LeftHandWeapon.SetDisplayModel();
			m_LeftHandWeapon.SetLevel(level);
			//右手
			m_RightHandWeapon.SetWeaponType(CWeapon::WEAPONSHOW_DOUBLEWEAPON);
			mod2ID = ptegeffect->dwValue2;
			m_RightHandWeapon.SetModelID(mod2ID);
			m_RightHandWeapon.SetVisible(true);
			m_RightHandWeapon.SetDisplayModel();
			m_RightHandWeapon.SetLevel(level);
			break;
			//拳套
		case WT_FIST:
			ptegeffect = CGoodsList::GetEffectTag(index,GAP_BOTHHAND_GRAPHIC);
			ASSERT(ptegeffect);
			//左手
			m_LeftHandWeapon.SetWeaponType(CWeapon::WEAPONSHOW_BOXINGLEFT);
			mod1ID = ptegeffect->dwValue1;
			m_LeftHandWeapon.SetModelID(mod1ID);
			m_LeftHandWeapon.SetVisible(true);
			m_LeftHandWeapon.SetDisplayModel();
			m_LeftHandWeapon.SetLevel(level);
			//右手
			m_RightHandWeapon.SetWeaponType(CWeapon::WEAPONSHOW_BOXINGRIGHT);
			mod2ID = ptegeffect->dwValue2;
			m_RightHandWeapon.SetModelID(mod2ID);
			m_RightHandWeapon.SetVisible(true);
			m_RightHandWeapon.SetDisplayModel();
			m_RightHandWeapon.SetLevel(level);
			break;

		}



	}
	else if(equipType == EC_E_CARD)
	{
		m_LeftHandWeapon.SetWeaponType(CWeapon::WEAPONSHOW_SHIELD);
		mod1ID = pGoods->BaseProperty.dwEquipID;
		m_LeftHandWeapon.SetModelID(mod1ID);
		m_LeftHandWeapon.SetVisible(true);
		m_LeftHandWeapon.SetDisplayModel();
		m_LeftHandWeapon.SetLevel(level);
	}
	else if(equipType == EC_E_WING)
	{
		mod1ID = pGoods->BaseProperty.dwEquipID;
		m_Wing.SetModelID(mod1ID);
		m_Wing.SetVisible(true);
		m_Wing.SetDisplayModel();
		m_Wing.SetLevel(level);
	}
	else if(equipType == EC_E_HEAD)
	{
		mod1ID = pGoods->BaseProperty.dwEquipID;
		m_HeadWear.SetModelID(mod1ID);
		m_HeadWear.SetVisible(true);
		m_HeadWear.SetDisplayModel();
		m_HeadWear.SetLevel(level);
	}
	extern bool g_bForceUpdate;
	g_bForceUpdate = true;
	SetAction(GetAction());
	g_bForceUpdate = false;
	//***********
	//更新显示武器ADD by 彭谦
	//***********

}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//取下物品更新显示
void CPlayer::RemoveWeaponShow(CGoods* pGoods)
{
	//*******
	//取消武器显示ADD BY 彭谦
	//*******
	if (!pGoods)
	{
		return;
	}
	DWORD index = pGoods->GetIndex();
	CGoodsList::tagGoods2* ptaggoods2 = CGoodsList::GetProperty(index);
	if (ptaggoods2)
	{
		eEquipType equipType = pGoods->GetEquipType();
		if (equipType == EC_E_WEAPON)
		{
			CGoodsList::tagEffect* ptegeffect = CGoodsList::GetEffectTag(index,GAP_WEAPON_CATEGORY);
			long ltype = 0;
			if (ptegeffect!=NULL) 
				ltype = ptegeffect->dwValue1;
			if (ltype >=WT_BOW&&ltype <=WT_FIST)
			{
				m_LeftHandWeapon.Hide();
				m_LeftHandWeapon.SetVisible(false);
				m_LeftHandWeapon.SetWeaponType(0);
				m_LeftHandWeapon.SetModelID(0);
				m_LeftHandWeapon.SetLevel(0);
				m_RightHandWeapon.Hide();
				m_RightHandWeapon.SetVisible(false);
				m_RightHandWeapon.SetWeaponType(0);
				m_RightHandWeapon.SetModelID(0);
				m_RightHandWeapon.SetLevel(0);
			}
			else
			{
				m_RightHandWeapon.Hide();
				m_RightHandWeapon.SetVisible(false);
				m_RightHandWeapon.SetWeaponType(0);
				m_RightHandWeapon.SetModelID(0);
				m_RightHandWeapon.SetLevel(0);
			}

		}
		else if(equipType == EC_E_CARD)
		{
			if (m_RightHandWeapon.GetWeaponType() == CWeapon::WEAPONSHOW_DOUBLEWEAPON
				||m_RightHandWeapon.GetWeaponType() == CWeapon::WEAPONSHOW_BOW
				||m_RightHandWeapon.GetWeaponType() == CWeapon::WEAPONSHOW_BOXINGRIGHT)
			{

			}
			else
			{
				m_LeftHandWeapon.Hide();
				m_LeftHandWeapon.SetVisible(false);
				m_LeftHandWeapon.SetWeaponType(0);
				m_LeftHandWeapon.SetModelID(0);
				m_LeftHandWeapon.SetLevel(0);
			}

		}
		else if(equipType == EC_E_WING)
		{
			m_Wing.Hide();
			m_Wing.SetVisible(false);
			m_Wing.SetModelID(0);
			m_Wing.SetLevel(0);
		}
		else if(equipType == EC_E_HEAD)
		{
			m_HeadWear.Hide();
			m_HeadWear.SetVisible(false);
			m_HeadWear.SetModelID(0);
			m_HeadWear.SetLevel(0);
		}
		extern bool g_bForceUpdate;
		g_bForceUpdate = true;
		SetAction(GetAction());
		g_bForceUpdate = false;
		//*******
		//取消武器显示ADD BY 彭谦
		//*******
	}

}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//取下物品更新显示
void CPlayer::RemoveWeaponShow(DWORD dwPos)
{
	//*******
	//取消武器显示ADD BY 彭谦
	//*******
	DWORD index = m_dwEquipIndex[dwPos];
	CGoodsList::tagGoods2* ptaggoods2 = CGoodsList::GetProperty(index);
	if (ptaggoods2)
	{
		long equipType = CGoodsList::GetEffectTag(index,GAP_EQUIP_CATEGORY)->dwValue1;
		if (equipType == EC_E_WEAPON)
		{
			CGoodsList::tagEffect* ptegeffect = CGoodsList::GetEffectTag(index,GAP_WEAPON_CATEGORY);
			long ltype = ptegeffect->dwValue1;
			if (ltype >=WT_BOW&&ltype <=WT_FIST)
			{
				m_LeftHandWeapon.Hide();
				m_LeftHandWeapon.SetVisible(false);
				m_LeftHandWeapon.SetWeaponType(0);
				m_LeftHandWeapon.SetModelID(0);
				m_LeftHandWeapon.SetLevel(0);
				m_RightHandWeapon.Hide();
				m_RightHandWeapon.SetVisible(false);
				m_RightHandWeapon.SetWeaponType(0);
				m_RightHandWeapon.SetModelID(0);
				m_RightHandWeapon.SetLevel(0);
			}
			else
			{
				m_RightHandWeapon.Hide();
				m_RightHandWeapon.SetVisible(false);
				m_RightHandWeapon.SetWeaponType(0);
				m_RightHandWeapon.SetModelID(0);
				m_RightHandWeapon.SetLevel(0);
			}
		}
		else if(equipType == EC_E_CARD)
		{
			if (m_RightHandWeapon.GetWeaponType() == CWeapon::WEAPONSHOW_DOUBLEWEAPON
				||m_RightHandWeapon.GetWeaponType() == CWeapon::WEAPONSHOW_BOW
				||m_RightHandWeapon.GetWeaponType() == CWeapon::WEAPONSHOW_BOXINGRIGHT)
			{

			}
			else
			{
				m_LeftHandWeapon.Hide();
				m_LeftHandWeapon.SetVisible(false);
				m_LeftHandWeapon.SetWeaponType(0);
				m_LeftHandWeapon.SetModelID(0);
				m_LeftHandWeapon.SetLevel(0);
			}
		}
		else if(equipType == EC_E_WING)
		{
			m_Wing.Hide();
			m_Wing.SetVisible(false);
			m_Wing.SetModelID(0);
			m_Wing.SetLevel(0);
		}
		else if(equipType == EC_E_HEAD)
		{
			m_HeadWear.Hide();
			m_HeadWear.SetVisible(false);
			m_HeadWear.SetModelID(0);
			m_HeadWear.SetLevel(0);
		}
		extern bool g_bForceUpdate;
		g_bForceUpdate = true;
		SetAction(GetAction());
		g_bForceUpdate = false;
		//*******
		//取消武器显示ADD BY 彭谦
		//*******
	}

}



//////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////

// 更新属性
void CPlayer::UpdateProperty(CMessage* pMsg/*,char** &strName,DWORD* &dwValue,char** &str,WORD& wNum*/)
{
	//BYTE pByte[102400];
	//memset(pByte,0,102400);
	//pMsg->GetEx(pByte,102400);
	//long lpos = 0;
	//DecordChangeProperty(pByte,lpos/*,strName,dwValue,str,wNum*/);
	DBReadSet setReadDB;
	pMsg->GetDBReadSet(setReadDB);
	CPlayer::DecodeChangePropertyFromDataBlock(setReadDB);
}

// 解析变化后的属性
void CPlayer::DecordChangeProperty(BYTE* pByte, long& pos/*,char** &strName,DWORD* &dwValue,char** &str,WORD& wNum*/)
{
	CMoveShape::DecordChangeProperty(pByte,pos);
	WORD wNum = _GetWordFromByteArray(pByte,pos);
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

	//if (wNum>0)
	//{
	//	strName = new char*[wNum];
	//	dwValue = new DWORD[wNum];
	//	str = new char*[wNum];
	//}
	for (int i = 0;i<wNum;i++)
	{	
		//strName[i] = new char[30];
		//str[i] = new char[30];
		//strcpy(strName[i],"");
		//strcpy(str[i],"");
		//dwValue[i] = -1;
		pro_type = _GetWordFromByteArray(pByte,pos);
		data_type = _GetByteFromByteArray(pByte, pos);
		data_length = _GetWordFromByteArray(pByte,pos);		
		if (data_length<=0) continue;

		//根据数据类型获取具体的数据值
		switch(data_type)
		{
		case ATTR_DATA_BYTE:
			data = _GetByteFromByteArray(pByte, pos);
			break;
		case ATTR_DATA_WORD:
			data = _GetWordFromByteArray(pByte,pos);
			break;
		case ATTR_DATA_DWORD:
			data = _GetDwordFromByteArray(pByte,pos);
			break;
		case ATTR_DATA_FLOAT:
			data = (long)_GetFloatFromByteArray(pByte,pos);
			break;
		case ATTR_DATA_LONG:
			data = _GetLongFromByteArray(pByte,pos);
			break;
		case ATTR_DATA_SHORT:
			data = _GetShortFromByteArray(pByte,pos);
			break;
		case ATTR_DATA_BOOL:
			data = _GetByteFromByteArray(pByte,pos);
			break;
		case ATTR_DATA_BYTEARRAY:
			byte_Array = new BYTE[data_length];
			if (!byte_Array) return;
			for (int j = 0;j<data_length;j++)
			{
				byte_Array[j] = _GetByteFromByteArray(pByte, pos);
			}
			break;
		case ATTR_DATA_WORDARRAY:
			word_Array = new WORD[data_length];
			if (!word_Array) return;
			for (int j = 0;j<data_length;j++)
			{
				word_Array[j] = _GetWordFromByteArray(pByte,pos);
			}
			break;
		case ATTR_DATA_DWORDARRAY:
			dword_Array = new DWORD[data_length];
			if (!dword_Array) return;
			for (int j = 0;j<data_length;j++)
			{
				dword_Array[j] = _GetDwordFromByteArray(pByte,pos);
			}
			break;
		case ATTR_DATA_FLOATARRAY:
			flaot_Array = new float[data_length];
			if (!flaot_Array) return;
			for (int j = 0;j<data_length;j++)
			{
				flaot_Array[j] = _GetFloatFromByteArray(pByte,pos);
			}
			break;
		case ATTR_DATA_LONGARRAY:
			long_Array = new long[data_length];
			if (!long_Array) return;
			for (int j = 0;j<data_length;j++)
			{
				long_Array[j] = _GetLongFromByteArray(pByte,pos);
			}
			break;
		case ATTR_DATA_SHORTARRAY:
			short_Array = new short[data_length];
			if (!short_Array) return;
			for (int j = 0;j<data_length;j++)
			{
				short_Array[j] = _GetShortFromByteArray(pByte,pos);
			}
			break;
		case ATTR_DATA_CHARARRAY:
			char_Array = new char[data_length];
			if (!char_Array) return;
			_GetStringFromByteArray(pByte, pos, char_Array);
			break;
		case ATTR_DATA_BOOLARRAY:
			bool_Array = new bool[data_length];
			if (!bool_Array) return;
			for (int j = 0;j<data_length;j++)
			{
				data = _GetByteFromByteArray(pByte, pos);
				if (data!=0)
				{
					bool_Array[j] = true;
				}else
					bool_Array[j] = false;
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


void CPlayer::DecodeChangePropertyFromDataBlock(DBReadSet& setReadDB)
{
	CMoveShape::DecodeChangePropertyFromDataBlock(setReadDB);
	WORD wNum = setReadDB.GetWordFromByteArray();
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

	for (int i = 0;i<wNum;i++)
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
			for (int j = 0;j<data_length;j++)
			{
				byte_Array[j] = setReadDB.GetByteFromByteArray();
			}
			break;
		case ATTR_DATA_WORDARRAY:
			word_Array = new WORD[data_length];
			if (!word_Array) return;
			for (int j = 0;j<data_length;j++)
			{
				word_Array[j] = setReadDB.GetWordFromByteArray();
			}
			break;
		case ATTR_DATA_DWORDARRAY:
			dword_Array = new DWORD[data_length];
			if (!dword_Array) return;
			for (int j = 0;j<data_length;j++)
			{
				dword_Array[j] = setReadDB.GetDwordFromByteArray();
			}
			break;
		case ATTR_DATA_FLOATARRAY:
			flaot_Array = new float[data_length];
			if (!flaot_Array) return;
			for (int j = 0;j<data_length;j++)
			{
				flaot_Array[j] = setReadDB.GetFloatFromByteArray();
			}
			break;
		case ATTR_DATA_LONGARRAY:
			long_Array = new long[data_length];
			if (!long_Array) return;
			for (int j = 0;j<data_length;j++)
			{
				long_Array[j] = setReadDB.GetLongFromByteArray();
			}
			break;
		case ATTR_DATA_SHORTARRAY:
			short_Array = new short[data_length];
			if (!short_Array) return;
			for (int j = 0;j<data_length;j++)
			{
				short_Array[j] = setReadDB.GetShortFromByteArray();
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
			for (int j = 0;j<data_length;j++)
			{
				data = setReadDB.GetByteFromByteArray();
				if (data!=0)
				{
					bool_Array[j] = true;
				}else
					bool_Array[j] = false;
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

//清空道具背包
void CPlayer::ClearCoteGoods(WORD index)
{
	if (index<0||index>=COTE_NUM)
		return ;
	if (m_Cote[index]!=NULL&&!m_Cote[index]->m_listCoteGoods.empty())
	{
		list<CPlayer::tagGoods>::iterator it = m_Cote[index]->m_listCoteGoods.begin();
		for (;it!=m_Cote[index]->m_listCoteGoods.end();it++)
		{
			SAFE_DELETE(it->pGoods);						//清空每个背包里的物品的信息
		}
		m_Cote[index]->m_listCoteGoods.clear();	
	}
}

//获取指定序号的子背包自身作为物品的属性
CGoods* CPlayer::GetSubPGoods(WORD index)
{
	if (index>=0&&index<5&&m_Cote[index]!=NULL)
	{
		return m_Cote[index]->pGoods;
	}
	return NULL;
}	

//设置指定序号的子背包的pGoods
void CPlayer::SetSubPGoods(WORD index,CGoods* pGoods)
{
	if (index>=0&&index<5&&m_Cote[index]!=NULL)
	{
		m_Cote[index]->pGoods = pGoods;
	}
}

//获取指定序号的子背包指定位置的物品
CPlayer::tagGoods* CPlayer::GetSubGoods(WORD ExtendID,WORD lPos)
{
	if (ExtendID>=0&&ExtendID<=4)
	{
		list<tagGoods>* pLGoods = GetSubGoodslist(ExtendID);
		if (pLGoods==NULL)
		{
			return NULL;
		}
		tagGoodsIter it  = pLGoods->begin();
		for (;it!=pLGoods->end();it++)
		{
			if (it->lPos==lPos)
			{
				return &(*it);
			}
		}
	}
	return NULL;
}

//根据容器ID和指定的位置获取人身上的物品（包括原始背包和子背包）
CPlayer::tagGoods* CPlayer::GetGoodsOnPlayer(WORD ExtendID,WORD lPos)
{
	if (ExtendID==PEI_PACKET)
	{
		return GetGoods(lPos);
	}
	else if (ExtendID<=PEI_PACK5&&ExtendID>=PEI_PACK1)
	{
		return GetSubGoods(ExtendID - PEI_PACK1,lPos);
	}
	else if (ExtendID==PEI_BUSINESSPACK)
	{
		return GetBusinessGoods(lPos);
	}
	return NULL;
}

//获取原始背包指定位置的物品
CPlayer::tagGoods* CPlayer::GetGoods(WORD wPos)
{
	if (wPos >= 0 && wPos <= 20)
	{
		list<tagGoods>* pLGoods = GetGoodsList();
		if (pLGoods==NULL)
		{
			return NULL;
		}
		tagGoodsIter it = pLGoods->begin();
		for (;it!=pLGoods->end();it++)
		{
			if (it->lPos==wPos)
			{
				return &(*it);
			}
		}
	}
	return NULL;
}

///获取跑墒背包指定位置的物品
CPlayer::tagGoods* CPlayer::GetBusinessGoods(WORD wPos)
{
	if (wPos>=0&&wPos<=20 && m_CoteBussinesPack!=NULL)
	{
		list<tagGoods>* pLGoods = &m_CoteBussinesPack->m_listCoteGoods;
		if (pLGoods==NULL)
		{
			return NULL;
		}
		tagGoodsIter it = pLGoods->begin();
		for (;it!=pLGoods->end();it++)
		{
			if (it->lPos==wPos)
			{
				return &(*it);
			}
		}
	}
	return NULL;
}

///获取跑墒背包栏位上的背包
CGoods* CPlayer::GetBusinessPackGoods()
{
	if (m_CoteBussinesPack!=NULL)
	{
		return m_CoteBussinesPack->pGoods;
	}
	return NULL;
}

//检测此背包是否有物品被锁定了
bool CPlayer::IsHaveLockedGoodsOnPackage(DWORD dwPacketIndex)
{
	if (dwPacketIndex!=PEI_PACKET&&dwPacketIndex!=PEI_PACK1
		&&dwPacketIndex!=PEI_PACK2&&dwPacketIndex!=PEI_PACK3
		&&dwPacketIndex!=PEI_PACK4&&dwPacketIndex!=PEI_PACK5)
		return false;
	list<CPlayer::tagGoods>* pGoodsList = NULL;
	DWORD dwIndex = 0;
	if (dwPacketIndex==PEI_PACKET)
	{
		dwIndex = 0;
	}else if (dwPacketIndex>=PEI_PACK1&&dwPacketIndex<=PEI_PACK5)
	{
		dwIndex = dwPacketIndex-PEI_PACK1+1;
	}
	pGoodsList = GetGoodsList((WORD)dwIndex);
	if (pGoodsList!=NULL)
	{
		list<CPlayer::tagGoods>::iterator GoodsIter = pGoodsList->begin();
		for (;GoodsIter!=pGoodsList->end();GoodsIter++)
		{
			if (GoodsIter->pGoods!=NULL&&GoodsIter->pGoods->IsLocked())
			{
				return true;
			}
		}
	}
	return false;
}

///是否装上了可用的跑墒背包
bool CPlayer::IsHaveBusinessPack()
{
	if (m_CoteBussinesPack!=NULL&&m_CoteBussinesPack->pGoods!=NULL)
	{
		return true;
	}
	return false;
}

///清理已有的跑墒背包（跑墒背包的CGoods对象，背包里面的物品链表没有变化）
bool CPlayer::ClearBussinesPackGoods()
{
	if (m_CoteBussinesPack!=NULL&&m_CoteBussinesPack->pGoods!=NULL)
	{
		m_CoteBussinesPack->pGoods = NULL;
		return true;
	}
	return false;
}

///设置跑墒背包
void CPlayer::SetBussinesPack(CGoods*pPackGoods)
{
	if (m_CoteBussinesPack!=NULL&&pPackGoods!=NULL)
	{
		ClearBussinesPackGoods();
		m_CoteBussinesPack->pGoods = pPackGoods;
	}
}

///获取跑墒背包的格子数
long CPlayer::GetBussinesPackDimen()
{
	if (m_CoteBussinesPack!=NULL&&m_CoteBussinesPack->pGoods!=NULL)
	{
		return m_CoteBussinesPack->pGoods->GetEffectVal(GAP_PACK_CAPACITY,1);
	}
	return 0;
}

///获取跑墒背包的容量（此处的“容量”指能存放的跑墒物品总的数量，跟背包的耐久值有关）
long CPlayer::GetBussinesPackCapacity()
{
	if (m_CoteBussinesPack!=NULL&&m_CoteBussinesPack->pGoods!=NULL)
	{
		return m_CoteBussinesPack->pGoods->GetEffectVal(GAP_PACK_CAPACITY,2);
	}
	return 0;
}

///获取跑墒背包的当前容量
long CPlayer::GetBussinesPackCurCapacity()
{
	long lGoodsTotalNum = 0;
	if (m_CoteBussinesPack!=NULL)
	{
		for (tagGoodsIter it=m_CoteBussinesPack->m_listCoteGoods.begin();
			it!=m_CoteBussinesPack->m_listCoteGoods.end();it++)
		{
			lGoodsTotalNum+=it->lNum;
		}
	}
	return lGoodsTotalNum;
}

///背包格子是否满了
bool CPlayer::IsGrimFull()
{
	if (m_CoteBussinesPack!=NULL&&m_CoteBussinesPack->pGoods!=NULL)
	{
		if (GetBussinesPackDimen()>(long)m_CoteBussinesPack->m_listCoteGoods.size())
		{
			return false;
		}
	}
	return true;
}

///背包容量是否满了
bool CPlayer::IsCapacityFull()
{
	if (m_CoteBussinesPack!=NULL)
	{
		if (GetBussinesPackCurCapacity()<GetBussinesPackCapacity())
		{
			return false;
		}
	}
	return true;
}

///获取跑墒背包的附加属性值
long CPlayer::GetEffectValueOfPackGoods(WORD wEffectType,int iWhich)
{
	if (m_CoteBussinesPack!=NULL&&m_CoteBussinesPack->pGoods!=NULL)
	{
		return m_CoteBussinesPack->pGoods->GetEffectVal(wEffectType,iWhich);
	}
	return 0;
}

//获取指定序号的背包的容积（包括主背包和子背包）
WORD CPlayer::GetPacketDimen(DWORD dwPacketIndex)
{
	if (dwPacketIndex!=PEI_PACKET&&dwPacketIndex!=PEI_PACK1
		&&dwPacketIndex!=PEI_PACK2&&dwPacketIndex!=PEI_PACK3
		&&dwPacketIndex!=PEI_PACK4&&dwPacketIndex!=PEI_PACK5)
		return 0;
	if (dwPacketIndex==PEI_PACKET)
	{
		return GetHostPackgeDimen();
	}else if (dwPacketIndex<=PEI_PACK5&&dwPacketIndex>=PEI_PACK1)
	{
		return GetPackageDimen((WORD)dwPacketIndex-PEI_PACK1);
	}
	return 0;
}

//获取指定序号的子背包的容积
WORD CPlayer::GetPackageDimen(WORD index)	
{
	if (index<0||index>=COTE_NUM||m_Cote[index]==NULL||m_Cote[index]->pGoods==NULL)
		return 0;
	else if(m_Cote[index]->pGoods!=NULL)
	{
		return (WORD)m_Cote[index]->pGoods->GetEffectVal(GAP_PACK_CAPACITY,1);//获取道具背包的格子数
	}
	return 0;
}

//判断是否所有的背包都是满的
bool CPlayer::IsAllPackageFull()
{
	bool bResult = true;
	for (int i = 0;i<PACK_NUM;i++)
	{
		if (!IsFullForPackage(i))
		{
			bResult = false;
			break;
		}
	}
	return bResult;
}

//判断某个背包是否已满(包括主背包和子背包，序号从0~5)
bool CPlayer::IsFullForPackage(long PackageIndex)
{
	long GoodsNum = (long)GetGoodsList((WORD)PackageIndex)->size();
	long dimension = 0;
	if (PackageIndex==0)
	{
		dimension = HOST_GRIDITEM;
	}
    else if (GetSubPGoods(WORD(PackageIndex-1)))
	{
		dimension = GetPackageDimen(WORD(PackageIndex-1));
	}
	if (GoodsNum == dimension)
	{
		return true;
	}
	return false;
}

//获取未满背包的序号（从原始背包开始计）
int CPlayer::GetNotFullPackage()
{
	for (WORD i= 0;i<PACK_NUM;i++)
	{
		long num = (long)GetGoodsList(i)->size();
		long dimension = 0;
		if (i == 0)
		{
			dimension = HOST_GRIDITEM;
		}
		else if (GetSubPGoods(i-1))
		{
			dimension = GetPackageDimen(i-1);
		}
		if (num != dimension)
		{
			return	i;
		}
	}
	return -1;
}

//获取未满的背包的第一个空格位置(dwExtendID>=0&&dwExtendID<=5)
int CPlayer::GetFirstEmptyGrid(DWORD dwExtendID)									
{
	list<tagGoods>* pLGoods = GetGoodsList((WORD)dwExtendID);
	long dimension = 0;
	if (dwExtendID == 0)
	{
		dimension = HOST_GRIDITEM;
	}
	else if (GetSubPGoods((WORD)dwExtendID-1))
	{
		dimension = GetPackageDimen((WORD)dwExtendID-1);
	}
	for (int i = 0;i<dimension;i++)
	{
		tagGoodsIter it = pLGoods->begin();
		for (;it!=pLGoods->end();it++)
		{
			if (it->lPos==i)
			{
				break;
			}
		}
		if (it==pLGoods->end())
		{
			return i;
		}
	}
	return -1;
	//long* GoodsPos = NULL;
	//GoodsPos = new long[num];
	//long MinPos = 0;
	//	/*遍历背包中的物品,将每个物品在背包中的位置存入一个数组中*/
	//	for (int i = 0;i<num;i++)
	//	{ 
	//		GoodsPos[i] = it->lPos;
	//		it++;
	//	}
	//	/*对这个数组进行冒泡排序*/
	//	for (int j = 0;j<num-1;j++)
	//	{	
	//		for (int i = 0;i<num-1-j;i++)
	//		{
	//			if (GoodsPos[i+1]<GoodsPos[i])
	//			{
	//				MinPos = GoodsPos[i];
	//				GoodsPos[i] = GoodsPos[i+1];
	//				GoodsPos[i+1] = MinPos;
	//			}
	//		}
	//	}
	//	/*比较数组中前后两个值的差值，如果大于1则可找到他们之间的第一个空格*/
	//	if (GoodsPos[0]==0)
	//	{	
	//		int i = 0;
	//		for(;i<num-1;i++)
	//		{
	//			BYTE temp = GoodsPos[i+1]-GoodsPos[i];
	//			if (temp>1)
	//			{
	//				return GoodsPos[i]+1;
	//			}
	//		}
	//		return GoodsPos[num-1]+1;
	//	}else
	//		return GoodsPos[0]-1;
}

//获取人物身上的物品所在的容器ID
WORD CPlayer::GetGoodsContainerID(CGoods* pGoods)
{
	if (pGoods!=NULL)
	{
		//if(m_stHand.pGoods && m_stHand.pGoods->GetExID() == pGoods->GetExID())
		//	return (WORD)PEI_HAND;

		//在装备中查找
		for(int i=0;i<CPlayer::EQUIP_SUM;i++)
		{
			CGoods* pEquip=GetEquip(i);
			if(pEquip)
			{
				if(pEquip->GetExID() == pGoods->GetExID())
				{
					return (WORD)PEI_EQUIPMENT;
				}
			}
		}
		//在背包中查找
		list<tagGoods>* pGoodList = GetGoodsList();
		if (pGoodList)
		{
			CGoods* ptGoods = NULL;
			tagGoodsIter GoodsIter=pGoodList->begin();
			for(;GoodsIter!=pGoodList->end();GoodsIter++)
			{
				ptGoods = GoodsIter->pGoods;
				if(ptGoods!=NULL && ptGoods->GetExID() == pGoods->GetExID())
				{
					return (WORD)PEI_PACKET;
				}
			}
		}
		//查询子背包
		for (int i = 0;i<COTE_NUM;i++)
		{	
			//在子背包中查找
			pGoodList = GetSubGoodslist(i);
			if (pGoodList)
			{
				CGoods* ptGoods = NULL;
				tagGoodsIter GoodsIter=pGoodList->begin();
				for(;GoodsIter!=pGoodList->end();GoodsIter++)
				{
					ptGoods = GoodsIter->pGoods;
					if(ptGoods!=NULL && ptGoods->GetExID() == pGoods->GetExID())
					{
						return (WORD)(PEI_SUBPACK+i+1);
					}
				}
			}
		}

		//在跑墒背包中查找
		pGoodList = NULL;
		if (m_CoteBussinesPack!=NULL)
		{
			pGoodList = &m_CoteBussinesPack->m_listCoteGoods;
		}
		if (pGoodList)
		{
			tagGoodsIter GoodsIter=pGoodList->begin();
			for(;GoodsIter!=pGoodList->end();GoodsIter++)
			{
				if(GoodsIter->pGoods!=NULL && pGoods->GetExID() == GoodsIter->pGoods->GetExID())
				{
					return PEI_BUSINESSPACK;
				}
			}
		}
	}
	return -1;
}

//查找人物身上(仅仅指主背包和子背包范围内)是否有某种类型的物品，如果有返回所在的容器ID
WORD CPlayer::FindContainerIDOfGoods(CGoods* pGoods)
{
	if (pGoods==NULL) return -1;

	//在原始背包中查找
	list<tagGoods>* pGoodList = GetGoodsList();
	if (pGoodList)
	{
		tagGoodsIter GoodsIter=pGoodList->begin();
		for(;GoodsIter!=pGoodList->end();GoodsIter++)
		{
			if(GoodsIter->pGoods!=NULL && GoodsIter->pGoods->GetProperty()->dwType == pGoods->GetProperty()->dwType)
			{
				return (WORD)PEI_PACKET;
			}
		}
	}
	//查询子背包
	for (int i = 0;i<COTE_NUM;i++)
	{	
		//在子背包中查找
		pGoodList = GetSubGoodslist(i);
		if (pGoodList)
		{
			tagGoodsIter GoodsIter=pGoodList->begin();
			for(;GoodsIter!=pGoodList->end();GoodsIter++)
			{
				if(GoodsIter->pGoods!=NULL && GoodsIter->pGoods->GetProperty()->dwType == pGoods->GetProperty()->dwType)
				{
					return (WORD)(PEI_SUBPACK+i+1);
				}
			}
		}
	}
	return -1;
}

//根据物品所在位置查找物品，lGoodType:1为装备2为物品 lGoodId物品的类型或者编号 lContainerID 物品所在位置
bool CPlayer::FindAppointGoods(long lGoodType,long lGoodId, long lGoodNum, long lContainerID)
{
	bool bIsHave=false;
	long lNum = 0;
	int iCount = 0;
	switch(lGoodType)
	{
	case 1:
		//在装备中查找
		if (lContainerID == 1)
		{
			for(int i=0;i<CPlayer::EQUIP_SUM;i++)
			{
				CGoods* pEquip = m_Equip[i];
				if (pEquip&&pEquip->GetEffectVal(GAP_WEAPON_CATEGORY,1) == lGoodId)
				{
					iCount++;
				}
			}
		}
		else if (lContainerID == 2)				//查背包
		{
			//在原始背包中查找装备
			list<tagGoods>* pGoodList = GetGoodsList();
			if (pGoodList)
			{
				tagGoodsIter GoodsIter=pGoodList->begin();
				for(;GoodsIter!=pGoodList->end();GoodsIter++)
				{
					CGoods* pGoods = GoodsIter->pGoods;
					if(pGoods && pGoods->GetEffectVal(GAP_WEAPON_CATEGORY,1)==lGoodId)
					{
						iCount++;
					}
				}
			}		

			//查询子背包装备
			for (int i = 0;i<COTE_NUM;i++)
			{	
				//在子背包中查找
				pGoodList = GetSubGoodslist(i);
				if (pGoodList)
				{
					tagGoodsIter GoodsIter=pGoodList->begin();
					for(;GoodsIter!=pGoodList->end();GoodsIter++)
					{
						CGoods* pGoods = GoodsIter->pGoods;
						if(pGoods && pGoods->GetEffectVal(GAP_WEAPON_CATEGORY,1)==lGoodId)
						{
							iCount++;
						}
					}
				}
			}
		}
		else			//查仓库（待处理）
			bIsHave = false;
		if(iCount>0)
			bIsHave = true;
		else
			bIsHave = false;
		break;
	case 2:
		//在装备中查找物品
		if (lContainerID == 1)
		{
			for(int i=0;i<CPlayer::EQUIP_SUM;i++)
			{
				CGoods* pEquip = m_Equip[i];
				if (pEquip&&pEquip->GetProperty()->dwIndex == lGoodId)
				{
					lNum += pEquip->GetNum();
					bIsHave = true;
				}
			}
		}
		else if (lContainerID == 2)				//查背包
		{
			//在原始背包中查找物品
			list<tagGoods>* pGoodList = GetGoodsList();
			if (pGoodList)
			{
				tagGoodsIter GoodsIter=pGoodList->begin();
				for(;GoodsIter!=pGoodList->end();GoodsIter++)
				{
					CGoods* pGoods = GoodsIter->pGoods;
					if(pGoods && pGoods->GetProperty()->dwIndex == lGoodId)
					{
						lNum += pGoods->GetNum();
						iCount++;
					}
				}
			}		

			//查询子背包物品
			for (int i = 0;i<COTE_NUM;i++)
			{	
				//在子背包中查找
				pGoodList = GetSubGoodslist(i);
				if (pGoodList)
				{
					tagGoodsIter GoodsIter=pGoodList->begin();
					for(;GoodsIter!=pGoodList->end();GoodsIter++)
					{
						CGoods* pGoods = GoodsIter->pGoods;
						if(pGoods && pGoods->GetProperty()->dwIndex == lGoodId)
						{
							lNum += pGoods->GetNum();
							iCount++;
						}
					}
				}
			}
		}
		else 			//查仓库（待处理）
			bIsHave = false;
		if(iCount>0)
			bIsHave = true;
		else
			bIsHave = false;
		break;
	}
	if(bIsHave && lNum >= lGoodNum)		// 找到符合数量要求的物品
		return true;
	else if(bIsHave && lNum == 0)		// 找到武器
		return true;
	else
		return false;
}
//通过位置查找物品
CGoods* CPlayer::GetGoodsByPosition(long lExtendID,DWORD dwPos)
{
	CGoods* pGoods = NULL;
	if (lExtendID==PEI_PACKET&&dwPos>=0&&dwPos<HOST_GRIDITEM)
	{
		tagGoods* pstGoods = GetGoods((WORD)dwPos);
		if (pstGoods)
		{
			pGoods = pstGoods->pGoods;
		}
	}
	else if (lExtendID==PEI_BUSINESSPACK&&dwPos>=0&&dwPos<HOST_GRIDITEM)
	{
		tagGoods* pstGoods = GetBusinessGoods((WORD)dwPos);
		if (pstGoods)
		{
			pGoods = pstGoods->pGoods;
		}
	}
	else if (lExtendID<=PEI_PACK5&&lExtendID>=PEI_PACK1)
	{
		WORD wGridNum = GetPackageDimen((WORD)lExtendID-PEI_PACK1);
		if (dwPos>=0&&dwPos<wGridNum)
		{
			tagGoods* pstGoods = GetSubGoods((WORD)(lExtendID-PEI_PACK1),(WORD)dwPos);
			if (pstGoods)
			{
				pGoods = pstGoods->pGoods;
			}
		}
	}else if (lExtendID==PEI_EQUIPMENT&&dwPos>=0&&dwPos<EQUIP_SUM)
	{
		pGoods = m_Equip[dwPos];
	}
	// 王峰：在仓库中查找
	else if( CGoods* pGoodsInDeopt = CGCMana::Inst()->GetGoods( lExtendID, dwPos ) )
	{
		pGoods = pGoodsInDeopt;
	}

	return pGoods;
}

//将此ID的制作条目加入玩家已学的制作条目列表中
bool CPlayer::AddFactureSkill(DWORD dwFactureID)
{
	for(int i = 0; i < (int)m_vecFactureEntrys.size(); i++)
	{
		if (m_vecFactureEntrys[i]==dwFactureID)
		{
			return false;
		}
	}
	m_vecFactureEntrys.push_back(dwFactureID);
	return true;
}

// 删除某个已学会的制作条目
void CPlayer::DeleteFactureSkill(DWORD dwFactureID)
{
	vector<DWORD>::iterator FactureSkillIter = m_vecFactureEntrys.begin();
	for (;FactureSkillIter!=m_vecFactureEntrys.end();FactureSkillIter++)
	{
		if ((*FactureSkillIter)==dwFactureID)
		{
			m_vecFactureEntrys.erase(FactureSkillIter);
			return;
		}
	}
}

// 删除已学会的全部制作条目
void CPlayer::DeleteAllFactureSkill()
{
	m_vecFactureEntrys.clear();
}

//获取某个背包的物品链表（包括原始背包）
list<CPlayer::tagGoods>* CPlayer::GetGoodsList(WORD index)
{
	if (index<0||index>=PACK_NUM)
	{
		return NULL;
	}else if(index==0)
	{
		return &m_listGoods;
	}else
	{
		return GetSubGoodslist(index-1);
	}
}

//获取某个背包的物品链表（不包括原始背包）
list<CPlayer::tagGoods>* CPlayer::GetSubGoodslist(WORD index) 
{
	if (index<0||index>=COTE_NUM||m_Cote[index]==NULL)
	{
		return	NULL;
	}
	return &m_Cote[index]->m_listCoteGoods;
}

//获取人物指定序号的子背包
CPlayer::tagCote* CPlayer::GetCotePackage(WORD index) 
{
	if (index<0||index>=COTE_NUM)
	{
		return	NULL;
	}
	return m_Cote[index];
}										

// 添加到CByteArray
bool CPlayer::AddToByteArray(vector<BYTE>* pByteArray, bool bExData)
{
	CMoveShape::AddToByteArray(pByteArray, bExData);

	if( bExData )
	{
		_AddToByteArray(pByteArray, (BYTE*)&m_Property, (long)sizeof(tagPPro));
		_AddToByteArray(pByteArray, (BYTE*)&m_FightProperty, (long)sizeof(tagPFPro));
		_AddToByteArray(pByteArray, GetAccount());
		_AddToByteArray(pByteArray, GetTitle());
		//_AddToByteArray(pByteArray, &m_Property, (long)sizeof(tagPPro));
		//_AddToByteArray(pByteArray, m_dwNextExp);

		//m_listSkillID
		_AddToByteArray(pByteArray, (long)m_listSkillID.size());
		list<tagSkill>::iterator SkillIter;
		for(SkillIter=m_listSkillID.begin();SkillIter!=m_listSkillID.end();SkillIter++)
		{
			_AddToByteArray(pByteArray, (BYTE*)&(*SkillIter), (long)sizeof(tagSkill));
		}

		//m_listGoods
		_AddToByteArray(pByteArray,(long)m_listGoods.size());//物品个数
		tagGoodsIter GoodIter;
		for(GoodIter=m_listGoods.begin();GoodIter!=m_listGoods.end();GoodIter++)
		{
			//_AddToByteArray(pByteArray,(BYTE*)&(*GoodIter),long(sizeof(bool)+sizeof(BYTE)*2));//先压前3个字节

			_AddToByteArray(pByteArray, (BYTE)GoodIter->bChange);
			_AddToByteArray(pByteArray, (WORD)GoodIter->lNum);
			_AddToByteArray(pByteArray, (BYTE)GoodIter->lPos);
			if(GoodIter->bChange)
			{
				_AddToByteArray(pByteArray,GoodIter->pGoods->GetIndex());//压入物品INDEX
				_AddToByteArray(pByteArray,(long)(GoodIter->pGoods->GetEffect()->size()));//先压该物品的EFFECT个数
				vector<CGoodsList::tagEffect>::iterator EffectIter;
				for(EffectIter=GoodIter->pGoods->GetEffect()->begin();EffectIter!=GoodIter->pGoods->GetEffect()->end();EffectIter++)
				{
					_AddToByteArray(pByteArray,(BYTE*)&(*EffectIter),(long)sizeof(CGoodsList::tagEffect));
				}
			}			
		}

		//// 国家
		//_AddToByteArray(pByteArray, GetCountry() );
		//_AddToByteArray(pByteArray, GetCountryContribute() );
	}

	return true;
}



/*
* 功能: 解码
* 摘要: -
* 参数: pByte   - 
*		 pos	 - 
*		 bExData - 
* 返回值: -
* 作者: -
* 创建日期: -
* 修改日志:
*	2008.09.16 - lpf
*		增加了对是否显示国家标志的解码
*	2009.03.13 - lpf
*		增加了对时装秀的解码,并初始化了界面按钮状态
*	2009.03.20 - lpf
*		取消了解码"时装秀"时,对游戏设置界面按钮状态的初始化工作
*	2009.04.10- lpf
* 		增加"设定玩家国家贡献初始值";
*/
// 此接口没有使用了，参考DecodeFromDataBlock
bool CPlayer::DecordFromByteArray(BYTE* pByte, long& pos, bool bExData)
{
	m_listSkillID.clear();
	m_SuitList.clear();
	//清空子背包
	for (int i = 0;i<COTE_NUM;i++)
	{
		if (m_Cote[i]!=NULL&&!m_Cote[i]->m_listCoteGoods.empty())
		{
			tagGoodsIter it = m_Cote[i]->m_listCoteGoods.begin();
			for (;it!=m_Cote[i]->m_listCoteGoods.end();it++)
			{
				SAFE_DELETE(it->pGoods);						//清空背包里的每个物品
			}
			m_Cote[i]->m_listCoteGoods.clear();				
		}
	}
	//清空原始背包
	if (!m_listGoods.empty())
	{
		tagGoodsIter it = m_listGoods.begin();
		for (;it != m_listGoods.end();it++)
		{
			SAFE_DELETE(it->pGoods);					
		}
		m_listGoods.clear();									//清空原始背包
	}

	//清空跑墒背包
	if (m_CoteBussinesPack!=NULL&&!m_CoteBussinesPack->m_listCoteGoods.empty())
	{
		tagGoodsIter it = m_CoteBussinesPack->m_listCoteGoods.begin();
		for (;it!=m_CoteBussinesPack->m_listCoteGoods.end();it++)
		{
			SAFE_DELETE(it->pGoods);						//清空背包里的每个物品
		}
		m_CoteBussinesPack->m_listCoteGoods.clear();				
	}

	m_listStateEx.clear();

	CMoveShape::DecordFromByteArray(pByte, pos, bExData);

	if( bExData )
	{
		//int n  = (long)sizeof(m_Property);

		_GetBufferFromByteArray(pByte, pos, &m_Property, (long)sizeof(tagPPro));

		DecordChangeProperty(pByte,pos);

		// 时装秀
		SetShowFashion(_GetByteFromByteArray(pByte, pos));

		// 设定玩家国家贡献初始值
		SetOldContribute(GetCountryContribute());

		// skill
		long lNum = _GetLongFromByteArray(pByte, pos);
		m_wPublicCoolTime = _GetWordFromByteArray(pByte, pos);
		tagSkill stSkill;
		for(int i=0; i<lNum; i++)
		{
			stSkill.dwID = _GetDwordFromByteArray(pByte, pos);
			stSkill.wLevel = _GetWordFromByteArray(pByte, pos);
			stSkill.dwRestoreTime = _GetDwordFromByteArray(pByte, pos);
			stSkill.fAffectRangeMin = _GetFloatFromByteArray(pByte, pos);
			stSkill.fAffectRangeMax = _GetFloatFromByteArray(pByte, pos);
			stSkill.wCostMP = _GetWordFromByteArray(pByte, pos);
			DWORD	dwTime = _GetDwordFromByteArray(pByte, pos);
			if(stSkill.dwRestoreTime > 0)
			{
				stSkill.dwRestoreTime += dwTime;
				stSkill.dwCoolDownStartTime = timeGetTime() - dwTime;
			}
			// 战斗技能才压入
			if(stSkill.dwID < SKILL_DEPUTY_OCCU)
			{
				m_listSkillID.push_back(stSkill);
				//IsAddOccupedList(stSkill.dwID);
			}
		}
		SortSkillList();

		//玩家在商业系统中的属性
		lNum = _GetDwordFromByteArray(pByte,pos);
		for (int i = 0;i<lNum;i++)
		{
			DWORD dwIndexOfZone = _GetDwordFromByteArray(pByte,pos);
			DWORD dwCreditForZoneValue = _GetDwordFromByteArray(pByte,pos);
			m_mapPlayerCreditForZone[dwIndexOfZone] = dwCreditForZoneValue;
		}

		map<DWORD,DWORD> tempGoodsCoolTime;
		/// 解码物品所属的冷却类型的冷却时间
		lNum = _GetDwordFromByteArray(pByte,pos);
		for (int i = 0;i<lNum;i++)
		{
			DWORD dwCoolDowmID = _GetDwordFromByteArray(pByte,pos);
			DWORD dwCoolDownReTime = _GetDwordFromByteArray(pByte,pos);
			tempGoodsCoolTime.insert(pair<DWORD,DWORD>(dwCoolDowmID,dwCoolDownReTime));
		}

		/// 解码单个物品冷却时间
		lNum = _GetDwordFromByteArray(pByte,pos);
		for (int i = 0;i<lNum;i++)
		{
			DWORD dwGoodsID = _GetDwordFromByteArray(pByte,pos);
			DWORD dwCoolDownTime = _GetDwordFromByteArray(pByte,pos);
			AddShortCutGoods((WORD)dwGoodsID);
			SetGoodsRemainTime((WORD)dwGoodsID,dwCoolDownTime);
		}


		//equipments
		bool bVal=false;
		lNum=_GetWordFromByteArray(pByte,pos);
		for(int i=0;i<lNum;i++)
		{
			CGoods *pGood=new CGoods;
			bVal = pGood->DecordFromByteArray(pByte, pos);
			WORD lIndex = _GetWordFromByteArray(pByte,pos);
			if(bVal&&lIndex>=0&&lIndex<EQUIP_SUM)
			{
				m_Equip[lIndex] = pGood;
				WORD SuitID = (WORD)pGood->GetEffectVal(GAP_SUITID,1);			
				if (SuitID!=0)
				{
					string stOrName = pGood->GetProperty()->strOrginName;					
					AddSuit(SuitID,stOrName);	
					SetupEquip(SuitID,stOrName);
				}
				pGood = NULL;
			}
			else
				SAFE_DELETE(pGood);
		}

		//勋章列表
		//GetGame()->GetCGuiEx()->GetCharacterPageEx()->GetFormByteArrayAndInit(pByte, pos);

		//Goods
		//初始化原始背包
		lNum = _GetLongFromByteArray(pByte,pos);
		if (lNum>0)
		{
			for (int i = 0;i<lNum;i++)
			{
				tagGoods stGood;
				stGood.bChange=_GetByteFromByteArray(pByte,pos) != 0;
				stGood.lNum = _GetWordFromByteArray(pByte,pos);
				stGood.lPos = _GetByteFromByteArray(pByte,pos);
				stGood.lType = PEI_PACKET;
				stGood.pGoods = new CGoods;
				bVal=stGood.pGoods->DecordFromByteArray(pByte, pos);
				if (CGoodsList::GetProperty(stGood.pGoods->GetIndex())->BaseProperty.dwType==GBT_CONSUMABLE)
				{
					AddShortCutGoods((WORD)stGood.pGoods->GetIndex());
					/// 获取此种物品的冷却类型
					DWORD dwCoolType = CGoodsList::GetEffectVal(stGood.pGoods->GetIndex(),GAP_COOLDOWN,2);
					map<DWORD,DWORD>::const_iterator CoolTimeIter = tempGoodsCoolTime.find(dwCoolType);
					if (CoolTimeIter!=tempGoodsCoolTime.end())
					{
						SetGoodsRemainTime((WORD)stGood.pGoods->GetIndex(),CoolTimeIter->second);
					}
					DWORD dwRemainTime = GetGoodsRemainTime((WORD)stGood.pGoods->GetIndex());
					if (dwRemainTime!=0)
					{
						DWORD dwTimeElapse = CGoodsList::GetEffectVal((WORD)stGood.pGoods->GetIndex(),GAP_COOLDOWN,1)-dwRemainTime;
						SetGoodsCoolDownStartTime((WORD)stGood.pGoods->GetIndex(),timeGetTime()-dwTimeElapse);
						SetGoodsUseState((WORD)stGood.pGoods->GetIndex(),false);
					}
				}
				if(bVal)
				{
					m_listGoods.push_back(stGood);
					WORD SuitID = (WORD)stGood.pGoods->GetEffectVal(GAP_SUITID,1);			
					if (SuitID!=0)
					{
						string stOrName = stGood.pGoods->GetProperty()->strOrginName;					
						AddSuit(SuitID,stOrName);	
						//SetupEquip(SuitID,stOrName);
					}
					stGood.pGoods = NULL;
				}
				else
				{
					SAFE_DELETE(stGood.pGoods);
				}
			}
		}

		//##初始化每个背包栏位的属性值
		BYTE lnum = _GetByteFromByteArray(pByte,pos);					    //道具背包数
		for (int i = 0;i<lnum;i++)
		{
			CGoods *pGood=new CGoods;
			bVal=pGood->DecordFromByteArray(pByte, pos);
			BYTE index=_GetByteFromByteArray(pByte,pos);
			if(bVal&&m_Cote[index]!=NULL)
			{
				m_Cote[index]->pGoods = pGood;							//pGoods中包含了道具背包的大小等属性
				pGood = NULL;
			}
			else
				SAFE_DELETE(pGood);	
		}
		//##初始化每个背包里的物品的信息（包括每个背包栏位）
		for (int i = 0;i<COTE_NUM;i++)
		{	
			lNum=_GetLongFromByteArray(pByte,pos);
			if (lNum>0)
			{
				for(int j=0;j<lNum;j++)
				{
					tagGoods stGood;
					stGood.bChange=_GetByteFromByteArray(pByte,pos) != 0;
					stGood.lNum = _GetWordFromByteArray(pByte,pos);
					stGood.lPos = _GetByteFromByteArray(pByte,pos);
					stGood.lType = PEI_PACK1+i;
					stGood.pGoods = new CGoods;
					bVal = stGood.pGoods->DecordFromByteArray(pByte, pos);
					if (CGoodsList::GetProperty(stGood.pGoods->GetIndex())->BaseProperty.dwType==GBT_CONSUMABLE)
					{
						AddShortCutGoods((WORD)stGood.pGoods->GetIndex());
						/// 获取此种物品的冷却类型
						DWORD dwCoolType = CGoodsList::GetEffectVal(stGood.pGoods->GetIndex(),GAP_COOLDOWN,2);
						map<DWORD,DWORD>::const_iterator CoolTimeIter = tempGoodsCoolTime.find(dwCoolType);
						if (CoolTimeIter!=tempGoodsCoolTime.end())
						{
							SetGoodsRemainTime((WORD)stGood.pGoods->GetIndex(),CoolTimeIter->second);
						}
						DWORD dwRemainTime = GetGoodsRemainTime((WORD)stGood.pGoods->GetIndex());
						if (dwRemainTime!=0)
						{
							DWORD dwTimeElapse = CGoodsList::GetEffectVal((WORD)stGood.pGoods->GetIndex(),GAP_COOLDOWN,1)-dwRemainTime;
							SetGoodsCoolDownStartTime((WORD)stGood.pGoods->GetIndex(),timeGetTime()-dwTimeElapse);
							SetGoodsUseState((WORD)stGood.pGoods->GetIndex(),false);
						}
					}
					if(bVal)
					{
						m_Cote[i]->m_listCoteGoods.push_back(stGood);
						WORD SuitID = (WORD)stGood.pGoods->GetEffectVal(GAP_SUITID,1);			
						if (SuitID!=0)
						{
							string stOrName = stGood.pGoods->GetProperty()->strOrginName;					
							AddSuit(SuitID,stOrName);	
							//SetupEquip(SuitID,stOrName);
						}
						stGood.pGoods = NULL;
					}
					else
					{
						SAFE_DELETE(stGood.pGoods);
					}
				}
			}
		}


		// #金币
		lNum = _GetLongFromByteArray(pByte, pos);
		CGUID guID;
		_GetBufferFromByteArray(pByte, pos, guID);
		SetMoney(lNum);
		SetguMoneyID(guID);

		// #银币
		lNum = _GetLongFromByteArray(pByte, pos);
		_GetBufferFromByteArray(pByte, pos, guID);
		SetSilverMoney(lNum);
		SetguSilverMoneyID(guID);

		// 宝石
		lNum = _GetLongFromByteArray(pByte, pos);
		_GetBufferFromByteArray(pByte, pos, guID);
		SetBaoShi(lNum);
		SetguBaoShiID(guID);

		// #积分
		lNum = _GetLongFromByteArray(pByte, pos);
		//_GetBufferFromByteArray(pByte, pos, guID);
		SetJiFen(lNum);
		//SetguJiFenID(guID);

		// 仓库信息头 解码
		//GetGame()->GetCGuiEx()->GetDepotPageEx()->DecordDepotHeadInfo( pByte, pos );

		// 组织系统
		DecordOrgSysFromByteArray(pByte,pos);

		//当前使用的称号
		long lTitleType = _GetLongFromByteArray(pByte,pos);
		SetTitleType(lTitleType);
		//GetGame()->GetCGuiEx()->GetCharacterPageEx()->SetTitleType(lTitleType);
		char strTitleName[1024];
		string strTitlle = _GetStringFromByteArray(pByte,pos,strTitleName);
		SetPlayerTitle(strTitlle);
		//GetGame()->GetCGuiEx()->GetCharacterPageEx()->SetPlayerTitle(strTitlle);


		//小精灵系统
		//GetGame()->GetCGuiEx()->GetAutoPageEx()->GetFormByteArrayAndInit(pByte,pos);

		// 
		m_PlayerSetupOnServer.GetFormByteArray( pByte, pos );

		//得到任务数据
		//DecordQuestDataFromByteArray(pByte,pos);
		//-----07.9.6刘科修改------
		m_RoleQueue.DecordQuestDataFromByteArray(pByte,pos);
		//////////副职业系统的属性//////////
		m_dwCollectExp = _GetLongFromByteArray(pByte,pos);
		m_dwCollectLevel = _GetLongFromByteArray(pByte,pos);
		m_dwFactureExp=_GetLongFromByteArray(pByte,pos);
		m_dwFactureLevel=_GetLongFromByteArray(pByte,pos);
		lNum =_GetLongFromByteArray(pByte,pos);
		for (int i=0; i<lNum; i++)
		{
			DWORD dwFactureID =_GetLongFromByteArray(pByte,pos);
			m_vecFactureEntrys.push_back(dwFactureID);
		}

		long lLev = _GetLongFromByteArray(pByte,pos);
		long lID = _GetLongFromByteArray(pByte,pos);

		GetGame()->GetMainPlayer()->SetLimtSkill(lID,lLev);

		// 招募信息
		DecordRecruitDataFromByteArray(pByte, pos);
		////////////////////////////////////
		/*CRoleRecord *pQuest = new CRoleRecord(77,1);
		m_RoleQueue.AddOneRecord(pQuest);*/
		//-------------------------


		//GetGame()->GetInterface()->GetContainerPage()->SetGoods(NULL);
	}
	else
	{
		// 装备编号
		SetHairPic(_GetByteFromByteArray(pByte, pos));
		SetShowFashion(_GetByteFromByteArray(pByte, pos));

		// 装备编号
        for(long l=EQUIP_HEAD; l<EQUIP_SUM; l++)
        {
            m_dwEquipIndex[l] = _GetDwordFromByteArray(pByte, pos);
            m_btEquipLevel[l] = _GetLongFromByteArray(pByte, pos);
        }

		// 是否显示国家标志
		SetShowCountry(_GetDwordFromByteArray(pByte, pos) != 0 ? true : false);

		SetHp(_GetDwordFromByteArray(pByte, pos));
		SetMaxHp(_GetDwordFromByteArray(pByte, pos));
		//--------07.8.3取消注释(刘科)---------
		SetPkCount(_GetDwordFromByteArray(pByte, pos));
		SetPkValue(_GetByteFromByteArray(pByte, pos) != 0);

		//组织系统
		DecordOrgSysFromByteArray(pByte,pos);

		//称号系统
		long lTitleType = _GetLongFromByteArray(pByte,pos);
		SetTitleType(lTitleType);
		char strTitleName[1024];
		string strTitlle = _GetStringFromByteArray(pByte,pos,strTitleName);
		SetPlayerTitle(strTitlle); 

		//职业+性别
		SetOccupation(_GetByteFromByteArray(pByte, pos));
		SetSex(_GetByteFromByteArray(pByte, pos));
		SetAssOccu(_GetByteFromByteArray(pByte,pos));

		// PK值 和 恶名值
		this->SetPkValue( _GetDwordFromByteArray( pByte, pos ) );
		this->SetPVPValue( _GetDwordFromByteArray( pByte, pos ) );

		//
		_GetBufferFromByteArray(pByte, pos,m_PlayerShopSessionID);
		m_lPlayerShopPlugID=_GetLongFromByteArray(pByte,pos);
		char strPlayerShopName[17];
		memset(strPlayerShopName,0,sizeof(strPlayerShopName));
		_GetStringFromByteArray(pByte,pos,strPlayerShopName);
		if(m_PlayerShopSessionID != CGUID::GUID_INVALID && m_lPlayerShopPlugID)
		{
			this->SetPlayerShopName(strPlayerShopName);
			this->SetPlayerShopOpen(true);
		}
		else
		{
			this->SetPlayerShopOpen(false);
		}
		DWORD dwEmotionIndex=_GetDwordFromByteArray(pByte, pos);
		DWORD dwTime=_GetDwordFromByteArray(pByte, pos);
		if(dwEmotionIndex)
		{
			SetEmotion(dwEmotionIndex-1,dwTime);
		}
		// 国家ID
		SetCountry( _GetByteFromByteArray(pByte,pos));
		SetFacePic(_GetByteFromByteArray(pByte, pos));
		SetLevel(_GetWordFromByteArray(pByte, pos));
		//SetRanOfNobCredit(_GetLongFromByteArray(pByte, pos));
		// 招募信息
		DecordRecruitDataFromByteArray(pByte, pos);
	}
	return true;
}

// 招募信息
void CPlayer::DecordRecruitDataFromByteArray(BYTE* pByte, long &pos)
{
	// 招募状态
	long lFlag = _GetLongFromByteArray(pByte, pos);
	// 停止招
	if(lFlag==0)
	{
		SetRecruitingTeam(false);
		return;
	}
	long lMemNum = _GetLongFromByteArray(pByte, pos);
	SetTeammateAmount(lMemNum);
	char strTeamName[20];
	memset(strTeamName,0,sizeof(strTeamName));
	_GetStringFromByteArray(pByte,pos,strTeamName);
	SetRecruitingTeam(true);
	SetTeamname(strTeamName);
	long lHasPass = _GetLongFromByteArray(pByte, pos);
	SetTeamHasPassword((lHasPass)==0?false:true);
}

bool CPlayer::DecordOrgSysFromByteArray(BYTE* pByte, long& pos)
{
	// 行会和联盟 GUID
	_GetBufferFromByteArray(pByte, pos,m_FactionID);
	_GetBufferFromByteArray(pByte, pos,m_UnionID);

	if((BOOL)_GetLongFromByteArray(pByte, pos))
	{
		LONG lFacDemiseWarning = 0;
		LONG lFacDisbandWarning = 0;

		if( this->IsSelf() )
		{
			lFacDemiseWarning = _GetLongFromByteArray( pByte, pos ); //! 帮会传位提醒倒记时
			lFacDisbandWarning = _GetLongFromByteArray( pByte, pos ); //! 帮会解散提醒倒记时
		}

		// 称号
		TCHAR szFactionTitle[ MAX_MEMBER_NAME_SIZE ];
		_GetStringFromByteArray( pByte, pos, szFactionTitle );

		// 获取PUBLIC信息
		tagFacPublicData stFacPublicData;
		_GetBufferFromByteArray( pByte, pos, &stFacPublicData, sizeof(tagFacPublicData) );

		// 获取官阶
		m_lFactionJob = _GetLongFromByteArray( pByte, pos );

		// 获取权限数据
		long lMyJobPurview = _GetLongFromByteArray( pByte, pos );

		// 公告
		TCHAR	szPronounce[ MAX_PronounceCharNum ];
		_GetStringFromByteArray( pByte, pos, szPronounce );

		// 帮会名称和称号
		m_strFactionName = stFacPublicData.szName;
		m_strFactionTitle = szFactionTitle;

		// 创建一个行会
		CFactionManager*	pFactionManager = GetGame()->GetFactionManager();

        CFaction::SFacInfo	stFacInfo;
		ZeroMemory( &stFacInfo, sizeof(stFacInfo) );
		stFacInfo.guidFac = stFacPublicData.guid;
		stFacInfo.strFacName = stFacPublicData.szName;
		stFacInfo.strPlacard = szPronounce;
		stFacInfo.arrLevel[ CFaction::FUN_FACTION ] = stFacPublicData.lLevel;

		stFacInfo.tMasterLastOnlineTime = (time_t)lFacDemiseWarning;
		stFacInfo.tLessMemNumTime = (time_t)lFacDisbandWarning;

		if( pFactionManager->AddFaction( &stFacInfo ) )
			pFactionManager->GetFaction( stFacPublicData.guid )->SetJobPurview( m_lFactionJob, lMyJobPurview );
	}

	return true;
}



/*
* 功能: 新的解码
* 摘要: -
* 参数: setReadDB   - 
*		 bExData - 
* 返回值: -
* 作者: -
* 创建日期: -
* 修改日志:
*	2009.03.13 - lpf
*		增加了对时装秀的解码,并初始化了界面按钮状态
*	2009.03.20 - lpf
*		取消了解码"时装秀"时,对游戏设置界面按钮状态的初始化工作
*	2009.04.10- lpf
* 		增加"设定玩家国家贡献初始值";
*/
bool CPlayer::DecodeFromDataBlock(DBReadSet& setReadDB, bool bExData )
{
	m_listSkillID.clear();
    m_mapBrotherInfo.clear();
	m_SuitList.clear();
	//清空道具背包
	for (int i = 0;i<COTE_NUM;i++)
	{
		if (m_Cote[i]!=NULL&&!m_Cote[i]->m_listCoteGoods.empty())
		{
			tagGoodsIter it = m_Cote[i]->m_listCoteGoods.begin();
			for (;it!=m_Cote[i]->m_listCoteGoods.end();it++)
			{
				SAFE_DELETE(it->pGoods);						//清空每个背包里的物品的信息
			}
			m_Cote[i]->m_listCoteGoods.clear();				
		}
	}
	//清空原始背包
	if (!m_listGoods.empty())
	{
		tagGoodsIter it = m_listGoods.begin();
		for (;it != m_listGoods.end();it++)
		{
			SAFE_DELETE(it->pGoods);					
		}
		m_listGoods.clear();									//清空原始背包
	}

	//清空跑墒背包
	if (m_CoteBussinesPack!=NULL&&!m_CoteBussinesPack->m_listCoteGoods.empty())
	{
		tagGoodsIter it = m_CoteBussinesPack->m_listCoteGoods.begin();
		for (;it!=m_CoteBussinesPack->m_listCoteGoods.end();it++)
		{
			SAFE_DELETE(it->pGoods);						//清空背包里的每个物品
		}
		m_CoteBussinesPack->m_listCoteGoods.clear();				
	}

	// 释放装备
	for(int i=0;i<EQUIP_SUM;i++)
	{
		if(i!=EQUIP_MEDAL1&&i!=EQUIP_MEDAL2&&i!=EQUIP_MEDAL3)  //勋章单独释放
		{
			SAFE_DELETE(m_Equip[i]);
		}
		else
			m_Equip[i] = NULL;
	}

	m_listStateEx.clear();

	m_vecFactureEntrys.clear();

	CMoveShape::DecodeFromDataBlock(setReadDB, bExData);

	if( bExData )
	{
		setReadDB.GetBufferFromByteArray(&m_Property, (long)sizeof(tagPPro));

		DecodeChangePropertyFromDataBlock(setReadDB);

		// 王峰：魔化属性
		setReadDB.GetBufferFromByteArray( m_arrElementProperty, sizeof(m_arrElementProperty) );

		// 时装秀
		SetShowFashion(setReadDB.GetByteFromByteArray());

		// 设定玩家国家贡献初始值
		SetOldContribute(GetCountryContribute());

		// skill
		long lNum = setReadDB.GetLongFromByteArray();
		m_wPublicCoolTime = setReadDB.GetWordFromByteArray();
		tagSkill stSkill;
		for(int i=0; i<lNum; i++)
		{
			stSkill.dwID = setReadDB.GetDwordFromByteArray();
			stSkill.wLevel = setReadDB.GetWordFromByteArray();
			stSkill.dwRestoreTime = setReadDB.GetDwordFromByteArray();
            // 技能范围改为float TODO:修改客户端技能范围相关逻辑
			stSkill.fAffectRangeMin = setReadDB.GetFloatFromByteArray();
			stSkill.fAffectRangeMax = setReadDB.GetFloatFromByteArray();
			stSkill.wCostMP = setReadDB.GetWordFromByteArray();
			DWORD	dwTime = setReadDB.GetDwordFromByteArray();
			if(stSkill.dwRestoreTime > 0)
			{
				stSkill.dwRestoreTime += dwTime;
				stSkill.dwCoolDownStartTime = timeGetTime() - dwTime;
			}
			// 战斗技能才压入
			if(stSkill.dwID < SKILL_DEPUTY_OCCU)
			{
				m_listSkillID.push_back(stSkill);
				//IsAddOccupedList(stSkill.dwID);
			}
		}
		SortSkillList();

#define _DECODE_GENIUS_

#ifdef  _DECODE_GENIUS_
        /// 解码天赋相关属性
        DecodeGeniusFromDataBlock(setReadDB);
#endif
        // 结义金兰
        long lBrotherNum = setReadDB.GetLongFromByteArray();
        tagBrotherInfo stBrother;
        for(long l=0; l<lBrotherNum; l++)
        {
            setReadDB.GetBufferFromByteArray(stBrother.guidBrother);
            setReadDB.GetLongFromByteArray();
            m_mapBrotherInfo[stBrother.guidBrother] = stBrother;
        }

		//玩家在商业系统中的属性
		lNum = setReadDB.GetDwordFromByteArray();
		SetCreditNum(lNum);
		for (int i = 0;i<lNum;i++)
		{
			DWORD dwIndexOfZone = setReadDB.GetDwordFromByteArray();
			DWORD dwCreditForZoneValue = setReadDB.GetDwordFromByteArray();
			m_mapPlayerCreditForZone[dwIndexOfZone] = dwCreditForZoneValue;
		}

		/// 解码物品所属的冷却类型的冷却时间
		lNum = setReadDB.GetDwordFromByteArray();
		for (int i = 0;i<lNum;i++)
		{
			DWORD dwCoolDowmID = setReadDB.GetDwordFromByteArray();
			DWORD dwCoolDownReTime = setReadDB.GetDwordFromByteArray();
			AddPublicCooldownType((WORD)dwCoolDowmID,dwCoolDownReTime);
		}

		/// 解码单个物品冷却时间
		lNum = setReadDB.GetDwordFromByteArray();
		for (int i = 0;i<lNum;i++)
		{
			DWORD dwGoodsID = setReadDB.GetDwordFromByteArray();
			DWORD dwCoolDownTime = setReadDB.GetDwordFromByteArray();
			AddShortCutGoods((WORD)dwGoodsID);
			SetGoodsRemainTime((WORD)dwGoodsID,dwCoolDownTime);
		}

		//equipments
		bool bVal=false;
		lNum=setReadDB.GetWordFromByteArray();
		for(int i=0;i<lNum;i++)
		{
			CGoods *pGood=new CGoods;
			bVal = pGood->DecodeFromDataBlock(setReadDB,bExData);
			WORD lIndex = setReadDB.GetWordFromByteArray();
			if(bVal&&lIndex>=0&&lIndex<EQUIP_SUM)
			{
				m_Equip[lIndex] = pGood;
				WORD SuitID = (WORD)pGood->GetEffectVal(GAP_SUITID,1);			
				if (SuitID!=0)
				{
					string stOrName = pGood->GetProperty()->strOrginName;					
					AddSuit(SuitID,stOrName);	
					SetupEquip(SuitID,stOrName);
				}
				pGood = NULL;
			}
			else
				SAFE_DELETE(pGood);
		}

        DWORD dwPos = 0;
        lNum = setReadDB.GetDwordFromByteArray();        
        for(int i=0; i<lNum; i++)
        {
            dwPos = setReadDB.GetByteFromByteArray();
            CGoods* pGoods = new CGoods;
            pGoods->DecodeFromDataBlock(setReadDB, bExData);
            m_eEquipGod[dwPos] = pGoods;
        }

        lNum = setReadDB.GetDwordFromByteArray();        
        for(int i=0; i<lNum; i++)
        {
            dwPos = setReadDB.GetByteFromByteArray();
            CGoods* pGoods = new CGoods;
            pGoods->DecodeFromDataBlock(setReadDB, bExData);
            m_eEquipLand[dwPos] = pGoods;
        }

        lNum = setReadDB.GetDwordFromByteArray();        
        for(int i=0; i<lNum; i++)
        {
            dwPos = setReadDB.GetByteFromByteArray();
            CGoods* pGoods = new CGoods;
            pGoods->DecodeFromDataBlock(setReadDB, bExData);
            m_eEquipHuman[dwPos] = pGoods;
        }

		//勋章列表
		//GetGame()->GetCGuiEx()->GetMadalPageEx()->DecodeFromDataBlockAndInit(setReadDB);
		DecodeMedalFromDataBlockAndInit(setReadDB);

		long lTest = setReadDB.GetLongFromByteArray();
		//Goods
		//初始化原始背包
		lNum = setReadDB.GetLongFromByteArray();
		if (lNum>0)
		{
			for (int i = 0;i<lNum;i++)
			{
				tagGoods stGood;
				stGood.bChange=setReadDB.GetByteFromByteArray() != 0;
				stGood.lNum = setReadDB.GetWordFromByteArray();
				stGood.lPos = setReadDB.GetByteFromByteArray();
				stGood.lType = PEI_PACKET;
				stGood.pGoods = new CGoods;
				bVal=stGood.pGoods->DecodeFromDataBlock(setReadDB,bExData);
				if (CGoodsList::GetProperty(stGood.pGoods->GetIndex())->BaseProperty.dwType==GBT_CONSUMABLE)
				{
					AddShortCutGoods((WORD)stGood.pGoods->GetIndex());
					/// 获取此种物品的冷却类型
					DWORD dwRemainTime = GetGoodsRemainTime((WORD)stGood.pGoods->GetIndex());
					if (dwRemainTime!=0)
					{
						DWORD dwTimeElapse = CGoodsList::GetEffectVal((WORD)stGood.pGoods->GetIndex(),GAP_COOLDOWN,1)-dwRemainTime;
						SetGoodsCoolDownStartTime((WORD)stGood.pGoods->GetIndex(),timeGetTime()-dwTimeElapse);
						SetGoodsUseState((WORD)stGood.pGoods->GetIndex(),false);
					}
				}
				if(bVal)
				{
					m_listGoods.push_back(stGood);
					WORD SuitID = (WORD)stGood.pGoods->GetEffectVal(GAP_SUITID,1);			
					if (SuitID!=0)
					{
						string stOrName = stGood.pGoods->GetProperty()->strOrginName;					
						AddSuit(SuitID,stOrName);	
						//SetupEquip(SuitID,stOrName);
					}
					stGood.pGoods = NULL;
				}
				else
				{
					SAFE_DELETE(stGood.pGoods);
				}
			}
		}

		//##初始化每个背包栏位的属性值
		BYTE lnum = setReadDB.GetByteFromByteArray();					    //道具背包数
		for (int i = 0;i<lnum;i++)
		{
			CGoods *pGood=new CGoods;
			bVal=pGood->DecodeFromDataBlock(setReadDB,bExData);
			BYTE index=setReadDB.GetByteFromByteArray();
			if(bVal&&m_Cote[index]!=NULL)
			{
				m_Cote[index]->pGoods = pGood;							//pGoods中包含了道具背包的大小等属性
				pGood = NULL;
			}
			else
				SAFE_DELETE(pGood);	
		}
		//##初始化每个背包里的物品的信息（包括每个背包栏位）
		for (int i = 0;i<COTE_NUM;i++)
		{	
			lNum=setReadDB.GetLongFromByteArray();
			if (lNum>0)
			{
				for(int j=0;j<lNum;j++)
				{
					tagGoods stGood;
					stGood.bChange=setReadDB.GetByteFromByteArray() != 0;
					stGood.lNum = setReadDB.GetWordFromByteArray();
					stGood.lPos = setReadDB.GetByteFromByteArray();
					stGood.lType = PEI_PACK1+i;
					stGood.pGoods = new CGoods;
					bVal = stGood.pGoods->DecodeFromDataBlock(setReadDB,bExData);
					if (CGoodsList::GetProperty(stGood.pGoods->GetIndex())->BaseProperty.dwType==GBT_CONSUMABLE)
					{
						AddShortCutGoods((WORD)stGood.pGoods->GetIndex());
						/// 获取此种物品的冷却类型
						DWORD dwRemainTime = GetGoodsRemainTime((WORD)stGood.pGoods->GetIndex());
						if (dwRemainTime!=0)
						{
							DWORD dwTimeElapse = CGoodsList::GetEffectVal((WORD)stGood.pGoods->GetIndex(),GAP_COOLDOWN,1)-dwRemainTime;
							SetGoodsCoolDownStartTime((WORD)stGood.pGoods->GetIndex(),timeGetTime()-dwTimeElapse);
							SetGoodsUseState((WORD)stGood.pGoods->GetIndex(),false);
						}
					}
					if(bVal)
					{
						m_Cote[i]->m_listCoteGoods.push_back(stGood);
						WORD SuitID = (WORD)stGood.pGoods->GetEffectVal(GAP_SUITID,1);			
						if (SuitID!=0)
						{
							string stOrName = stGood.pGoods->GetProperty()->strOrginName;					
							AddSuit(SuitID,stOrName);	
							//SetupEquip(SuitID,stOrName);
						}
						stGood.pGoods = NULL;
					}
					else
					{
						SAFE_DELETE(stGood.pGoods);
					}
				}
			}
		}


		lTest = setReadDB.GetLongFromByteArray();
		// #金币
		lNum = setReadDB.GetLongFromByteArray();
		CGUID guID;
		setReadDB.GetBufferFromByteArray(guID);
		SetMoney(lNum);
		SetguMoneyID(guID);

		// #银币
		lNum = setReadDB.GetLongFromByteArray();
		setReadDB.GetBufferFromByteArray(guID);
		SetSilverMoney(lNum);
		SetguSilverMoneyID(guID);

		// 宝石
		lNum = setReadDB.GetLongFromByteArray();
		setReadDB.GetBufferFromByteArray(guID);
		SetBaoShi(lNum);
		SetguBaoShiID(guID);

		// #ticket
		lNum = setReadDB.GetLongFromByteArray();
		SetJiFen(lNum);

		// 王峰：仓库信息头 解码
		CDepot::Inst()->DecodeDepotHeadInfo( setReadDB );
		// 王峰：组织系统
		DecodeOrgSysFromDataBlock(setReadDB);

		//当前使用的称号
		long lTitleType = setReadDB.GetLongFromByteArray();
		SetTitleType(lTitleType);
		//GetGame()->GetCGuiEx()->GetCharacterPageEx()->SetTitleType(lTitleType);
		char strTitleName[1024];
		string strTitlle = setReadDB.GetStringFromByteArray(strTitleName,1024);
		SetPlayerTitle(strTitlle);
		//GetGame()->GetCGuiEx()->GetCharacterPageEx()->SetPlayerTitle(strTitlle);

		//GM隐蔽
		BYTE bMConcealment = setReadDB.GetByteFromByteArray();
		SetGMConcealment(bMConcealment==0?false:true);

		//自动补给
        GetGame()->GetAutoSupply()->DecodeFromDataBlockAndInit(setReadDB);
		//GetGame()->GetCGuiEx()->GetAutoPageEx()->DecodeFromDataBlockAndInit(setReadDB);

		// 
		m_PlayerSetupOnServer.DecodeFromDataBlock( setReadDB );

		//得到任务数据
		//DecordQuestDataFromByteArray();
		//-----07.9.6刘科修改------
		m_RoleQueue.DecodeFromDataBlock(setReadDB);
		//////////副职业系统的属性//////////
		m_dwCollectExp = setReadDB.GetLongFromByteArray();
		m_dwCollectLevel = setReadDB.GetLongFromByteArray();
		m_dwFactureExp=setReadDB.GetLongFromByteArray();
		m_dwFactureLevel=setReadDB.GetLongFromByteArray();
		lNum =setReadDB.GetLongFromByteArray();
		for (int i=0; i<lNum; i++)
		{
			DWORD dwFactureID =setReadDB.GetLongFromByteArray();
			m_vecFactureEntrys.push_back(dwFactureID);
		}

		long lLev = setReadDB.GetLongFromByteArray();
		long lID = setReadDB.GetLongFromByteArray();

		GetGame()->GetMainPlayer()->SetLimtSkill(lID,lLev);

		// 招募信息
		DecodeRecruitDataFromDataBlock(setReadDB);
		////////////////////////////////////
		/*CRoleRecord *pQuest = new CRoleRecord(77,1);
		m_RoleQueue.AddOneRecord(pQuest);*/
		//-------------------------

		//宠物编解码 2009/1/06 nikai
		DecodePetFromDataBlock(setReadDB,bExData);

		BYTE bIsHaveBusinessPack = setReadDB.GetByteFromByteArray();
		if (bIsHaveBusinessPack>0)
		{
			//##初始化跑墒背包栏位的属性值
			CGoods *pBusinessPackGoods = new CGoods;
			bVal=pBusinessPackGoods->DecodeFromDataBlock(setReadDB,bExData);
			if(bVal&&m_CoteBussinesPack!=NULL)
			{
				m_CoteBussinesPack->pGoods = pBusinessPackGoods;							//pGoods中包含了道具背包的大小等属性
				pBusinessPackGoods = NULL;		
				//##初始化跑墒背包里的每个物品的信息
				lNum=setReadDB.GetLongFromByteArray();	// 物品个数
				if (lNum>0)
				{
					for(int j=0;j<lNum;j++)
					{
						tagGoods stGood;
						stGood.bChange=setReadDB.GetByteFromByteArray() != 0;
						stGood.lNum = setReadDB.GetWordFromByteArray();
						stGood.lPos = setReadDB.GetByteFromByteArray();
						stGood.lType = PEI_BUSINESSPACK;
						stGood.pGoods = new CGoods;
						bVal = stGood.pGoods->DecodeFromDataBlock(setReadDB,bExData);
						if(bVal)
						{
							m_CoteBussinesPack->m_listCoteGoods.push_back(stGood);
							stGood.pGoods = NULL;
						}
						else
						{
							SAFE_DELETE(stGood.pGoods);
						}
					}
				}
			}
			else
				SAFE_DELETE(pBusinessPackGoods);	
		}
		SetState(GetState());
		//
		m_bIsShowLoverName = (setReadDB.GetByteFromByteArray()>0);
	}
	else
	{
		// 装备编号
		SetHairPic(setReadDB.GetByteFromByteArray());
		SetFacePic(setReadDB.GetByteFromByteArray());
		SetShowFashion(setReadDB.GetByteFromByteArray());

		// 装备编号
        for(long l=EQUIP_HEAD; l<EQUIP_SUM; l++)
        {
            m_dwEquipIndex[l] = setReadDB.GetDwordFromByteArray();
            m_btEquipLevel[l] = setReadDB.GetByteFromByteArray();
        }

		// 是否显示国家标志
		SetShowCountry(setReadDB.GetByteFromByteArray() != 0 ? true : false);

		SetHp(setReadDB.GetDwordFromByteArray());
		SetMaxHp(setReadDB.GetDwordFromByteArray());
		//--------07.8.3取消注释(刘科)---------
		SetPkValue( setReadDB.GetDwordFromByteArray() );
		SetPVPValue( setReadDB.GetDwordFromByteArray() );

		//组织系统
		DecodeOrgSysFromDataBlock(setReadDB);

		//称号系统
		long lTitleType = setReadDB.GetLongFromByteArray();
		SetTitleType(lTitleType);
		char strTitleName[1024];
		string strTitlle = setReadDB.GetStringFromByteArray(strTitleName,1024);
		SetPlayerTitle(strTitlle); 

		//GM隐蔽
		BYTE bMConcealment = setReadDB.GetByteFromByteArray();
		SetGMConcealment(bMConcealment==0?false:true);

		//职业+性别
		SetOccupation(setReadDB.GetByteFromByteArray());
		SetSex(setReadDB.GetByteFromByteArray());
		SetAssOccu(setReadDB.GetByteFromByteArray());

		//
		setReadDB.GetBufferFromByteArray(m_PlayerShopSessionID);
		m_lPlayerShopPlugID = 0;
		SetPlayerShopName("");
		SetPlayerShopOpen(false);
		if(m_PlayerShopSessionID != NULL_GUID)
		{
			m_lPlayerShopPlugID=setReadDB.GetLongFromByteArray();
			char strPlayerShopName[17];
			memset(strPlayerShopName,0,sizeof(strPlayerShopName));
			setReadDB.GetStringFromByteArray(strPlayerShopName,17);
			SetPlayerShopName(strPlayerShopName);
			SetPlayerShopOpen(true);			
		}

		DWORD dwEmotionIndex=setReadDB.GetDwordFromByteArray();
		DWORD dwTime=setReadDB.GetDwordFromByteArray();
		if(dwEmotionIndex)
		{
			SetEmotion(dwEmotionIndex-1,dwTime);
		}
		// 国家ID
		SetCountry( setReadDB.GetByteFromByteArray());
		SetLevel(setReadDB.GetWordFromByteArray());
		// 招募信息
		DecodeRecruitDataFromDataBlock(setReadDB);
		//玩家CP值
		long lCurPKCP = setReadDB.GetLongFromByteArray();
		long lMaxPKCP = setReadDB.GetLongFromByteArray();
		SetCurPKCP(lCurPKCP);
		SetMaxPKCP(lMaxPKCP);

        //设置目标点
        SetDestXY(setReadDB.GetFloatFromByteArray(),
                  setReadDB.GetFloatFromByteArray());
        
        float fRadianDir = GetLineDirF(m_fPosX,m_fPosY,m_fDestX,m_fDestY);
        //设置新的方向
        SetDirEx(fRadianDir);
        
        //得出移动时间
        float fTotalDistance = RealDistance(m_fDestX,m_fDestY);
        float dwMoveTime = fTotalDistance/m_fSpeed;
        SetEndMoveTime(timeGetTime()+(DWORD)dwMoveTime);
        SetMoveOffset((m_fDestX-m_fPosX)/dwMoveTime, (m_fDestY-m_fPosY)/dwMoveTime);
	}

	return true;
}

void CPlayer::DecodeGeniusFromDataBlock(DBReadSet& setReadDB)
{

    /// 编解码天赋属性
    long   GisCount = setReadDB.GetLongFromByteArray();
    for ( int l = 0 ; l < GisCount ; l++ )
    {
        long  GisID = setReadDB.GetLongFromByteArray();
        long  GisLvl= setReadDB.GetLongFromByteArray();
        m_GeniusTable [ GisID ] = GisLvl ;
    }

    /// 影响技能列表
    long   ListCount = setReadDB.GetLongFromByteArray();
    for ( int l = 0 ; l < ListCount ; l++ )
    {
        /// 技能 id 和 lvl
        long  skillid = setReadDB.GetLongFromByteArray();
        long  skillLvl= setReadDB.GetLongFromByteArray();
        /// 技能属性影响个数        
        long  ProNum  = setReadDB.GetLongFromByteArray();
		std::vector<string>vecStrKey;
		std::vector<long>vecValue;
        for ( int j = 0 ; j < ProNum ; j++ )
        {
            char pStr[64] ;

            /// 技能属性值
            setReadDB.GetStringFromByteArray(pStr,64);
			vecStrKey.push_back(pStr);
            /// 技能值
            long pValue = setReadDB.GetLongFromByteArray();
			vecValue.push_back(pValue);

			//更新玩家身上技能属性
			UpdateSkillAttr(skillid,skillLvl,pStr,pValue);
        }
		//更新技能模块
		UpdateSkillXmlList(skillid,skillLvl,vecStrKey,vecValue);
    }
}

bool CPlayer::DecodeOrgSysFromDataBlock(DBReadSet& setReadDB)
{
	// 行会和联盟 GUID
	setReadDB.GetBufferFromByteArray(m_FactionID);
	setReadDB.GetBufferFromByteArray(m_UnionID);

	if((BOOL)setReadDB.GetByteFromByteArray())
	{
		LONG lFacDemiseWarning = 0;
		LONG lFacDisbandWarning = 0;

		if( this->IsSelf() )
		{
			lFacDemiseWarning = setReadDB.GetLongFromByteArray(); //! 帮会传位提醒倒记时
			lFacDisbandWarning = setReadDB.GetLongFromByteArray(); //! 帮会解散提醒倒记时
		}

		// 称号
		TCHAR szFactionTitle[ MAX_MEMBER_NAME_SIZE ];
		setReadDB.GetStringFromByteArray( szFactionTitle, MAX_MEMBER_NAME_SIZE );

		// 行会图标CRC
		m_dwFacIconCRC = setReadDB.GetDwordFromByteArray();

		// PUBLIC信息、权限、官阶
		tagFacPublicData stFacPublicData;
		ZeroMemory( &stFacPublicData, sizeof(stFacPublicData) );

		TCHAR	szPronounce[ MAX_PronounceCharNum ];
		ZeroMemory( szPronounce, sizeof(szPronounce) );

		long lMyJobPurview = 0;

		if( this->IsSelf() )
		{
			// 获取PUBLIC信息
			setReadDB.GetBufferFromByteArray( &stFacPublicData, sizeof(tagFacPublicData) );

			// 获取官阶
			m_lFactionJob = setReadDB.GetLongFromByteArray();

			// 获取权限数据
			lMyJobPurview = setReadDB.GetLongFromByteArray();

			// 公告
			setReadDB.GetStringFromByteArray( szPronounce, MAX_PronounceCharNum);
		}
		else
		{
			setReadDB.GetStringFromByteArray( stFacPublicData.szName, MAX_MEMBER_NAME_SIZE );
		}

		// 帮会名称和称号
		m_strFactionName = stFacPublicData.szName;
		m_strFactionTitle = szFactionTitle;

		// 创建一个行会
		CFactionManager*	pFactionManager = GetGame()->GetFactionManager();

		CFaction::SFacInfo	stFacInfo;
		ZeroMemory( &stFacInfo, sizeof(stFacInfo) );
		stFacInfo.guidFac = m_FactionID;
		stFacInfo.guidUnion = m_UnionID;
		stFacInfo.strFacName = stFacPublicData.szName;
		stFacInfo.strPlacard = szPronounce;
		stFacInfo.arrLevel[ CFaction::FUN_FACTION ] = stFacPublicData.lLevel;

		stFacInfo.tMasterLastOnlineTime = (time_t)lFacDemiseWarning;
		stFacInfo.tLessMemNumTime = (time_t)lFacDisbandWarning;

		if( CFaction* pFaction = pFactionManager->GetFaction( m_FactionID ) )
		{
			pFaction->SetUnionGUID( m_UnionID );
			pFaction->SetName( stFacPublicData.szName );
			pFaction->SetPlacard( szPronounce );
			pFaction->SetLevel( stFacPublicData.lLevel );
			pFaction->GetFacInfo()->tMasterLastOnlineTime = stFacInfo.tMasterLastOnlineTime;
			pFaction->GetFacInfo()->tLessMemNumTime = stFacInfo.tLessMemNumTime;
			pFaction->SetJobPurview( m_lFactionJob, lMyJobPurview );
		}
		else
		{
			if( pFactionManager->AddFaction( &stFacInfo ) )
				pFactionManager->GetFaction( m_FactionID )->SetJobPurview( m_lFactionJob, lMyJobPurview );
		}

		// 校验徽标
		if( pFactionManager->CheckFacIconCRC( m_FactionID, m_dwFacIconCRC ) )
		{
			pFactionManager->SendUpdateFacIconMsg( m_FactionID );
		}
	}

	return true;
}

void CPlayer::DecodeRecruitDataFromDataBlock(DBReadSet& setReadDB)
{
	// 招募状态
	long lFlag = setReadDB.GetByteFromByteArray();
	// 停止招
	if(lFlag==0)
	{
		SetRecruitingTeam(false);
		return;
	}
	long lMemNum = setReadDB.GetByteFromByteArray();
	SetTeammateAmount(lMemNum);
	char strTeamName[20];
	memset(strTeamName,0,sizeof(strTeamName));
	setReadDB.GetStringFromByteArray(strTeamName,20);
	SetRecruitingTeam(true);
	SetTeamname(strTeamName);
	long lHasPass = setReadDB.GetByteFromByteArray();
	SetTeamHasPassword((lHasPass)==0?false:true);
	return;
}

void CPlayer::RandomMoveGoods()
{
	if (!m_listGoods.empty())
	{
		CMainPlayerHand* pHand = 0;//GetGame()->GetCGuiEx()->GetPMainPlayerHand();
		if (pHand!=NULL)
		{  
			int iPackDem = 0;
			long pack[PACK_NUM] = { PEI_PACKET,PEI_PACK1,PEI_PACK2,PEI_PACK3,PEI_PACK4,PEI_PACK5 };	
			//for (int i = 0;i<PACK_NUM;i++)
			//{
			for(tagGoodsIter GoodsIter = GetGoodsList(0)->begin();
				GoodsIter!=GetGoodsList(0)->end(); ++GoodsIter)
			{
				//long lExtendID = pack[random((int)PACK_NUM)];
				//if (lExtendID==PEI_PACKET)
				//{
				iPackDem = (int)GetHostPackgeDimen();
				//}
				//else if (lExtendID<=PEI_PACK5&&lExtendID>=PEI_PACK1)
				//{
				//	iPackDem = (int)GetPackageDimen(lExtendID-PEI_PACK1);
				//}
				if (iPackDem>0)
				{
					pHand->SendPutDownMes(TYPE_PLAYER,this->GetExID(),PEI_PACKET,GoodsIter->lPos,TYPE_PLAYER,this->GetExID(),PEI_PACKET,random((int)iPackDem),TYPE_GOODS,(CGUID&)GoodsIter->pGoods->GetExID(),GoodsIter->pGoods->GetNum());
					//pHand->SendPutDownMes(TYPE_PLAYER,this->GetExID(),GoodsIter->lType,GoodsIter->lPos,TYPE_PLAYER,this->GetExID(),lExtendID,random((int)iPackDem),TYPE_GOODS,(CGUID&)GoodsIter->pGoods->GetExID(),GoodsIter->pGoods->GetNum());
				}
			}
			//}
		}
	}
}



/*
* 功能: 解码
* 摘要: -
* 参数: -
* 返回值: -
* 作者: -
* 创建日期: -
* 修改日志:
*	2009.02.09 - lpf
*		增加了"玩家获得经验提示"的处理
*	2009.04.08 - lpf
*		增加了"玩家功勋值改变"的处理
*/
void CPlayer::AI()
{
	CMoveShape::AI();

	// 缓慢补充HP/MP/MA
	m_lGrowTimer++;
	if( GetCurTickCount() - m_lGrowTimer > 400 )
	{
		m_lGrowTimer = GetCurTickCount();

		if( m_listGrowHP.size() )
		{
			SetHp( GetHp() + *(m_listGrowHP.begin()) );
			m_listGrowHP.pop_front();
		}
		if( m_listGrowMP.size() )
		{
			SetMp( GetMp() + *(m_listGrowMP.begin()) );
			m_listGrowMP.pop_front();
		}
	}
	//刷新表情动作
	m_pEmotionShower->Update();

	//头顶文字消失
	vector<tagPlayerInfo>::iterator it;
	for(it=m_vecPlayerInfo.begin();it!=m_vecPlayerInfo.end();it++)
	{
		if(GetCurTickCount()-it->dwStartTime>5000)
		{
			if(it->backAlpha>=3)
			{
				it->backAlpha-=3;
				it->dwBackColor=it->backAlpha<<24|(it->dwBackColor&0xffffff);
			}
			if(it->foreAlpha>=3)
			{
				it->foreAlpha-=3;
				it->dwForeColor=it->foreAlpha<<24|(it->dwForeColor&0xffffff);
			}
			else
			{
				m_vecPlayerInfo.erase(it);
				break;
			}
		}
	}

	// 玩家获得经验提示
	static DWORD dwExpTipTextTimeLength = CGlobeSetup::GetSetup()->dwExpTipTextTimeLength;
	DWORD dwCurrentTime = timeGetTime();
	m_dwLevelCurrentDisplayNum = 0;
	for (DWORD dw = 0; dw < m_dwLevelMax; ++dw)
	{
		if (m_vecTipText[dw].fAlpha <= 0.0f)
			continue;

		m_vecTipText[dw].fAlpha = (float)((long)dwExpTipTextTimeLength - (long)(dwCurrentTime - m_vecTipText[dw].dwStartTime)) / (float)dwExpTipTextTimeLength;
		++m_dwLevelCurrentDisplayNum;
	}

	// 玩家功勋值改变
	long lIncrement = (long)GetCountryContribute() - m_lOldContribute;
	if (lIncrement == 0)
	{
		// do nothing
	}else if (lIncrement > 0)
	{
		AddTipText(TT_CON, lIncrement);
		m_lOldContribute = GetCountryContribute();
	}else if (lIncrement < 0)
		m_lOldContribute = GetCountryContribute();
}

void CPlayer::SetEquipIndex(long lIndex,DWORD dwIndex)
{
	if(lIndex>=0 && lIndex<EQUIP_SUM)
	{
		m_dwEquipIndex[lIndex]=dwIndex;
	}
}

void CPlayer::SetEquipLevel(long lIndex,DWORD level)
{
	if(lIndex>=0 && lIndex<EQUIP_SUM)
	{
		m_btEquipLevel[lIndex]=level;
	}
}

void CPlayer::MountEquip(CGoods *pGood,bool bPositive)
{
	//if(pGood)
	//{
	//	vector<CGoodsList::tagEffect>::iterator iter=pGood->GetEffect()->begin();
	//	for(;iter!=pGood->GetEffect()->end();iter++)
	//	{
	//		switch(iter->wType) 
	//		{			
	//		case 14:	//最小攻击力
	//			{
	//				long lret=GetMinAtk()+iter->dwValue1;
	//				if(bPositive)
	//				{
	//					if(iter->dwValue1>=0)
	//					{
	//						SetMinAtk(lret);
	//					}
	//				}
	//				else
	//				{
	//					if(iter->dwValue1<0)
	//					{
	//						if(lret>=0)
	//							SetMinAtk(lret);
	//						else
	//							SetMinAtk(0);
	//					}
	//				}	
	//			}
	//			break;
	//		case 15://最大攻击力
	//			{
	//				long lret=GetMaxAtk()+iter->dwValue1;
	//				if(bPositive)
	//				{
	//					if(iter->dwValue1>0)
	//						SetMaxAtk(lret);
	//				}
	//				else
	//				{
	//					if(iter->dwValue1<0)
	//					{
	//						if(lret>=0)
	//							SetMaxAtk(lret);
	//						else
	//							SetMaxAtk(0);
	//					}
	//				}	
	//			}
	//			break;
	//		case 16:	//法术攻击系数
	//			{	
	//				
	//			}
	//			break;
	//		case 17://物理防御力
	//			{
	//				long lret=GetDef()+iter->dwValue1;
	//				if(bPositive)
	//				{
	//					if(iter->dwValue1>0)
	//						SetDef(lret);
	//				}
	//				else
	//				{
	//					if(iter->dwValue1<0)
	//					{
	//						if(lret>=0)
	//							SetDef(lret);
	//						else
	//							SetDef(0);
	//					}
	//				}	
	//			}
	//			break;
	//		case 18:	//攻击速度修正
	//			{
	//				
	//			}
	//			break;
	//		case 19:	//命中能力
	//			{
	//				long lret=GetHit()+iter->dwValue1;
	//				if(bPositive)
	//				{
	//					if(iter->dwValue1>0)
	//						SetHit(lret);
	//				}
	//				else
	//				{
	//					if(iter->dwValue1<0)
	//					{
	//						if(lret>=0)
	//							SetHit(lret);
	//						else
	//							SetHit(0);
	//					}
	//				}
	//			}
	//			break;
	//		case 20:	//致命一击
	//			{
	//
	//			}
	//			break;
	//		case 21:	//闪避能力
	//			{
	//				long lret=GetDodge()+iter->dwValue1;
	//				if(bPositive)
	//				{
	//					if(iter->dwValue1>0)
	//						SetDodge(lret);
	//				}
	//				else
	//				{
	//					if(iter->dwValue1<0)
	//					{
	//						if(lret>=0)
	//							SetDodge(lret);
	//						else
	//							SetDodge(0);
	//					}
	//				}	
	//			}
	//			break;
	//		case 22:	
	//			{					
	//			}
	//			break;
	//		case 23://法术抗性修正
	//			{
	//
	//			}
	//			break;
	//		case 24:
	//			{
	//			}
	//			break;
	//		case 25://生命恢复速度
	//			{
	//				long lret=GetHpRecoverSpeed()+iter->dwValue1;
	//				if(bPositive)
	//				{
	//					if(iter->dwValue1>0)
	//						SetHpRecoverSpeed(lret);
	//				}
	//				else
	//				{
	//					if(iter->dwValue1<0)
	//					{
	//						if(lret>=0)
	//							SetHpRecoverSpeed(lret);
	//						else
	//							SetHpRecoverSpeed(0);							
	//					}
	//				}	
	//			}
	//			break;
	//		case 26:	//法力恢复速度	
	//			{
	//				long lret=GetMpRecoverSpeed()+iter->dwValue1;
	//				if(bPositive)
	//				{
	//					if(iter->dwValue1>0)
	//						SetMpRecoverSpeed(lret);
	//				}
	//				else
	//				{
	//					if(iter->dwValue1<0)
	//					{							
	//						if(lret>=0)
	//							SetMpRecoverSpeed(lret);
	//						else
	//							SetMpRecoverSpeed(0);							
	//					}
	//				}	
	//			}
	//			break;
	//		case 27://力量追加
	//			{
	//				long lret=GetStrenth()+iter->dwValue1;
	//				if(bPositive)
	//				{
	//					if(iter->dwValue1>=0)
	//					{
	//						SetStrenth(lret);
	//						CGame::tagSetup *pSetup = GetGame()->GetSetup();
	//						short sStrChange = short(iter->dwValue1);
	//						SetMinAtk(WORD(GetMinAtk() + sStrChange * pSetup->fStr2MinAtk[GetOccupation()]));
	//						SetMaxAtk(WORD(GetMaxAtk()+ sStrChange * pSetup->fStr2MaxAtk[GetOccupation()]));
	//						SetHit(WORD(GetHit() + sStrChange * pSetup->fStr2Hit[GetOccupation()]));				
	//					}
	//				}
	//				else
	//				{
	//					if(iter->dwValue1<0)
	//					{
	//						if(lret>=0)							
	//							SetStrenth(lret);							
	//						else							
	//							SetStrenth(0);
	//						CGame::tagSetup *pSetup = GetGame()->GetSetup();
	//						short sStrChange = short(iter->dwValue1);
	//						float fVal=GetMinAtk() + sStrChange * pSetup->fStr2MinAtk[GetOccupation()];
	//						if(fVal>=0)
	//							SetMinAtk(fVal);
	//						else
	//							SetMinAtk(0);
	//						fVal=GetMaxAtk() + sStrChange * pSetup->fStr2MaxAtk[GetOccupation()];
	//						if(fVal>=0)
	//							SetMaxAtk(fVal);
	//						else
	//							SetMaxAtk(0);
	//						fVal=GetHit() + sStrChange * pSetup->fStr2Hit[GetOccupation()];
	//						if(fVal>=0)
	//							SetHit(fVal);
	//						else
	//							SetHit(0);
	//					}
	//				}				
	//			}
	//			break;
	//		case 28://敏捷追加
	//			{
	//				long lret=GetAgility()+iter->dwValue1;
	//				if(bPositive)
	//				{
	//					if(iter->dwValue1>=0)
	//					{
	//						SetAgility(lret);
	//						CGame::tagSetup *pSetup = GetGame()->GetSetup();
	//						short sDexChange = short(iter->dwValue1);
	//						SetDodge(WORD(GetDodge() + sDexChange * pSetup->fDex2Flee[GetOccupation()]));
	//						//SetAtcSpeed(WORD(m_Property.wAtcSpeed + sDexChange * pSetup->fDex2Speed[GetOccupation()]));
	//						SetHit(WORD(GetHit() + sDexChange * pSetup->fDex2Hit[GetOccupation()]));
	//					}
	//				}
	//				else
	//				{
	//					if(iter->dwValue1<0)
	//					{
	//						if(lret>0)
	//							SetAgility(lret);
	//						else
	//							SetAgility(0);
	//						CGame::tagSetup *pSetup = GetGame()->GetSetup();
	//						short sDexChange = short(iter->dwValue1);
	//						float fVal=GetDodge() + sDexChange * pSetup->fDex2Flee[GetOccupation()];
	//						if(fVal>=0)
	//							SetDodge(fVal);
	//						else
	//							SetDodge(0);
	//						fVal=GetHit() + sDexChange * pSetup->fDex2Hit[GetOccupation()];
	//						if(fVal>=0)
	//							SetHit(fVal);
	//						else
	//							SetHit(0);
	//					}
	//				}				
	//			}
	//			break;
	//		case 29://体质追加
	//			{
	//				long lret=GetConsti()+iter->dwValue1;
	//				if(bPositive)
	//				{
	//					if(iter->dwValue1>=0)
	//					{
	//						SetConsti(lret);
	//						CGame::tagSetup *pSetup = GetGame()->GetSetup();
	//						short sConChange = short(iter->dwValue1);
	//						SetMaxHP(WORD(GetMaxHp() + sConChange * pSetup->fCon2MaxHp[GetOccupation()]));
	//						SetDef(WORD(GetDef() + sConChange * pSetup->fCon2Def[GetOccupation()]));
	//					}
	//				}
	//				else
	//				{
	//					if(iter->dwValue1<0)
	//					{
	//						if(lret>=0)
	//							SetConsti(lret);
	//						else
	//							SetConsti(0);
	//						CGame::tagSetup *pSetup = GetGame()->GetSetup();
	//						short sConChange = short(iter->dwValue1);
	//						float fVal=GetMaxHp() + sConChange * pSetup->fCon2MaxHp[GetOccupation()];
	//						if(fVal>=0)
	//							SetMaxHP(fVal);
	//						else
	//							SetMaxHP(0);
	//						fVal=GetDef() + sConChange * pSetup->fCon2Def[GetOccupation()];
	//						if(fVal>=0)
	//							SetDef(fVal);
	//						else
	//							SetDef(0);
	//					}
	//				}
	//			}
	//			break;
	//		case 30://灵力追加
	//			{
	//				
	//			}
	//			break;
	//		case 31:	//生命上限修正		
	//			{
	//				long lret=GetMaxHp()+iter->dwValue1;
	//				if(bPositive)
	//				{
	//					if(iter->dwValue1>0)
	//						SetMaxHP(lret);
	//				}
	//				else
	//				{
	//					if(iter->dwValue1<0)
	//					{
	//						if(lret>=0)
	//							SetMaxHP(lret);
	//						else
	//							SetMaxHP(0);
	//					}
	//				}
	//			}
	//			break;
	//		case 32://法力上限修正
	//			{
	//				long lret=GetMaxMP()+iter->dwValue1;
	//				if(bPositive)
	//				{
	//					if(iter->dwValue1>0)
	//						SetMaxMP(lret);
	//				}
	//				else
	//				{
	//					if(iter->dwValue1<0)
	//					{
	//						if(lret>=0)
	//							SetMaxMP(lret);
	//						else
	//							SetMaxMP(0);
	//					}
	//				}
	//			}
	//			break;
	//		case 33:
	//			{
	//			}
	//			break;
	//		case 34:
	//			{
	//			}
	//			break;
	//		case 35:
	//			{
	//			}
	//			break;
	//		case 36:
	//			{
	//			}
	//			break;
	//		case 37:
	//			{
	//			}
	//			break;
	//		case 38:
	//			{
	//			}
	//			break;
	//		case 39:
	//			{
	//			}
	//			break;
	//		case 40:
	//			{
	//			}
	//			break;
	//		case 41:
	//			{
	//			}
	//			break;
	//		case 42:
	//			{
	//			}
	//			break;
	//		case 43:
	//			{
	//			}
	//			break;
	//		case 44:
	//			{
	//			}
	//			break;
	//		case 45:
	//			{
	//			}
	//			break;
	//		case 46:
	//			{
	//			}
	//			break;
	//		case 47:
	//			{
	//			}
	//			break;
	//		case 51:
	//			{
	//				
	//			}
	//			break;

	//		case 91:
	//			{
	//					
	//			}
	//			break;
	//		case 92:
	//			{	
	//			}
	//			break;
	//		case 93:
	//			{
	//
	//			}
	//			break;
	//		default:
	//			break;
	//		}
	//	}
	//}
}

void CPlayer::UnMountEquip(CGoods *pGood)
{
	//if(pGood)
	//{
	//	vector<CGoodsList::tagEffect>::iterator iter=pGood->GetEffect()->begin();
	//	for(;iter!=pGood->GetEffect()->end();iter++)
	//	{
	//		switch(iter->wType) 
	//		{			
	//		case 14:	//最小攻击力
	//			{
	//				SetMinAtk(GetMinAtk()-iter->dwValue1);
	//			}
	//			break;
	//		case 15://最大攻击力
	//			{
	//				SetMaxAtk(GetMaxAtk()-iter->dwValue1);
	//			}
	//			break;
	//		case 16:	//法术攻击系数	
	//			{
	//				
	//			}
	//			break;
	//		case 17://物理防御力
	//			{
	//				SetDef(GetDef()-iter->dwValue1);
	//			}
	//			break;
	//		case 18:	//攻击速度修正
	//			{
	//			
	//			}
	//			break;
	//		case 19:	//命中能力
	//			{
	//				SetHit(GetHit()-iter->dwValue1);
	//			}
	//			break;
	//		case 20:	//致命一击
	//			{
	//				
	//			}
	//			break;
	//		case 21:	//闪避能力
	//			{
	//				SetDodge(GetDodge()-iter->dwValue1);
	//			}
	//			break;
	//		case 22:	
	//			{					
	//			}
	//			break;
	//		case 23://法术抗性修正
	//			{
	//				
	//			}
	//			break;
	//		case 24:
	//			{
	//			}
	//			break;
	//		case 25:	//生命恢复速度
	//			{
	//				SetHpRecoverSpeed(GetHpRecoverSpeed()-iter->dwValue1);
	//			}
	//			break;
	//		case 26:	//法力恢复速度	
	//			{
	//                   SetMpRecoverSpeed(GetMpRecoverSpeed()-iter->dwValue1);
	//			}
	//			break;
	//		case 27://力量追加
	//			{
	//				SetStrenth(GetStrenth()-iter->dwValue1);

	//				CGame::tagSetup *pSetup = GetGame()->GetSetup();
	//				short sStrChange = -short(iter->dwValue1);
	//				SetMinAtk(WORD(GetMinAtk() + sStrChange * pSetup->fStr2MinAtk[GetOccupation()]));
	//				SetMaxAtk(WORD(GetMaxAtk() + sStrChange * pSetup->fStr2MaxAtk[GetOccupation()]));
	//				SetHit(WORD(GetHit() + sStrChange * pSetup->fStr2Hit[GetOccupation()]));
	//				
	//			}
	//			break;
	//		case 28://敏捷追加
	//			{
	//				SetAgility(GetDex()-iter->dwValue1);

	//				CGame::tagSetup *pSetup = GetGame()->GetSetup();
	//				short sDexChange = -short(iter->dwValue1);
	//				SetDodge(WORD(m_Property.wDodge + sDexChange * pSetup->fDex2Flee[GetOccupation()]));
	//				SetAtcSpeed(WORD(m_Property.wAtcSpeed + sDexChange * pSetup->fDex2Speed[GetOccupation()]));
	//				SetHit(WORD(m_Property.wHit + sDexChange * pSetup->fDex2Hit[GetOccupation()]));
	//			}
	//			break;
	//		case 29://体质追加
	//			{
	//				SetCon(GetCon()-iter->dwValue1);

	//				CGame::tagSetup *pSetup = GetGame()->GetSetup();
	//				short sConChange = -short(iter->dwValue1);
	//				SetMaxHP(WORD(m_Property.wMaxHp + sConChange * pSetup->fCon2MaxHp[GetOccupation()]));
	//				SetDef(WORD(m_Property.wDef + sConChange * pSetup->fCon2Def[GetOccupation()]));
	//			}
	//			break;
	//		case 30://灵力追加
	//			{
	//				SetInt(GetInt()-iter->dwValue1);

	//				CGame::tagSetup *pSetup = GetGame()->GetSetup();
	//				short sIntChange = -short(iter->dwValue1);
	//				SetMaxMP(WORD(m_Property.wMaxMp + sIntChange * pSetup->fInt2MaxMp[GetOccupation()]));
	//				SetElementResistant(WORD(m_Property.wElementResistant + sIntChange * pSetup->fInt2Resistant[GetOccupation()]));
	//			}
	//			break;
	//		case 31:	//生命上限修正	
	//			{
	//				SetMaxHP(GetMaxHp()-iter->dwValue1);
	//			}
	//			break;
	//		case 32://法力上限修正
	//			{
	//				SetMaxMP(GetMaxMP()-iter->dwValue1);
	//			}
	//			break;
	//		case 33:
	//			{
	//			}
	//			break;
	//		case 34:
	//			{
	//			}
	//			break;
	//		case 35:
	//			{
	//			}
	//			break;
	//		case 36:
	//			{
	//			}
	//			break;
	//		case 37:
	//			{
	//			}
	//			break;
	//		case 38:
	//			{
	//			}
	//			break;
	//		case 39:
	//			{
	//			}
	//			break;
	//		case 40:
	//			{
	//			}
	//			break;
	//		case 41:
	//			{
	//			}
	//			break;
	//		case 42:
	//			{
	//			}
	//			break;
	//		case 43:
	//			{
	//			}
	//			break;
	//		case 44:
	//			{
	//			}
	//			break;
	//		case 45:
	//			{
	//			}
	//			break;
	//		case 46:
	//			{
	//			}
	//			break;
	//		case 47:
	//			{
	//			}
	//			break;
	//		case 51:
	//			{
	//				SetReAnk(GetReAnk()-iter->dwValue1);
	//			}
	//			break;

	//		case 91:
	//			{
	//				SetAttackAvoid(GetAttackAvoid() - iter->dwValue1);
	//			}
	//			break;
	//		case 92:
	//			{
	//				SetElementAvoid(GetElementAvoid() - iter->dwValue1);
	//			}
	//			break;
	//		case 93:
	//			{
	//				SetFullMiss(GetFullMiss() - iter->dwValue1);
	//			}
	//			break;
	//		default:
	//			break;
	//		}
	//	}
	//}
}

long CPlayer::CanMountEquip(CGoods *pGoods)
{
	//装备各种限制判定
	DWORD dwVal=0;
	//等级限制
	dwVal=pGoods->GetEffectVal(6,1);
	if(dwVal!=0 && dwVal>GetLevel())
		return LEVEL_NOTFIT;

	//力量限制
	dwVal=pGoods->GetEffectVal(7,1);
	if(dwVal!=0 && dwVal>GetStr())
		return STR_NOTFIT;

	//敏捷限制
	dwVal=pGoods->GetEffectVal(8,1);
	if(dwVal!=0 && dwVal>GetDex())
		return DEX_NOTFIT;

	//体质限制
	dwVal=pGoods->GetEffectVal(9,1);
	if(dwVal!=0 && dwVal>GetCon())
		return CON_NOTFIT;

	//职业限制
	dwVal=CGoodsList::GetEffectVal(pGoods->GetIndex(),11,1);//pGoods->GetEffectVal(11,1);
	if(dwVal!=0 && dwVal!=GetOccupation()+1)
		return OCCUPATION_NOTFIT;

	//性别限制
	//dwVal=pGoods->GetEffectVal(12,1);
	//if(dwVal!=0 && dwVal!=GetSex()+1)
	//	return SEX_NOTFIT;
	return EQUIPFIT;
}

// 根据传入的物品ID和要增加的数量查找一个堆叠数不会超过堆叠上限并且未被锁定的物品
CGoods* CPlayer::FindOverLapGoodsByIndex(DWORD dwIndex,DWORD dwGoodsNum,int& lExtendID,int& pPos)
{
	CGoods *pGoods=NULL;


	//在背包中查找
	list<tagGoods>* pGoodList = GetGoodsList();
	if (pGoodList)
	{
		tagGoodsIter GoodsIter=pGoodList->begin();
		for(;GoodsIter!=pGoodList->end();GoodsIter++)
		{
			pGoods = GoodsIter->pGoods;
			if(pGoods!=NULL && pGoods->GetIndex()==dwIndex&&
				(pGoods->GetNum()+dwGoodsNum)<=(WORD)pGoods->GetEffectVal(GAP_MAXOVERLAP,1)&&!pGoods->IsLocked())
			{
				lExtendID = GetGoodsContainerID(pGoods);
				pPos = GoodsIter->lPos;
				return pGoods;
			}
		}
	}
	//查询子背包
	for (int i = 0;i<COTE_NUM;i++)
	{	
		//在子背包中查找
		pGoodList = GetSubGoodslist(i);
		if (pGoodList)
		{
			tagGoodsIter GoodsIter=pGoodList->begin();
			for(;GoodsIter!=pGoodList->end();GoodsIter++)
			{
				pGoods = GoodsIter->pGoods;
				if(pGoods!=NULL && pGoods->GetIndex()==dwIndex&&
					(pGoods->GetNum()+dwGoodsNum)<=(WORD)pGoods->GetEffectVal(GAP_MAXOVERLAP,1)&&!pGoods->IsLocked())
				{
					lExtendID = GetGoodsContainerID(pGoods);
					pPos = GoodsIter->lPos;
					return pGoods;
				}
			}
		}
	}

	return NULL;
}

//查找第一个物编ID等于传入的ID的未被锁定的物品
CGoods* CPlayer::FindFirstGoodsByIndex(DWORD dwIndex,int& lExtendID,int& pPos)
{
	CGoods *pGoods=NULL;

	//if(m_stHand.pGoods && m_stHand.pGoods->GetIndex()==dwIndex)
	//	return m_stHand.pGoods;

	//在背包中查找
	list<tagGoods>* pGoodList = GetGoodsList();
	if (pGoodList)
	{
		tagGoodsIter GoodsIter=pGoodList->begin();
		for(;GoodsIter!=pGoodList->end();GoodsIter++)
		{
			pGoods = GoodsIter->pGoods;
			if(pGoods!=NULL && pGoods->GetIndex()==dwIndex&&!pGoods->IsLocked())
			{
				lExtendID = GetGoodsContainerID(pGoods);
				pPos = GoodsIter->lPos;
				return pGoods;
			}
		}
	}
	//查询子背包
	for (int i = 0;i<COTE_NUM;i++)
	{	
		//在子背包中查找
		pGoodList = GetSubGoodslist(i);
		if (pGoodList)
		{
			tagGoodsIter GoodsIter=pGoodList->begin();
			for(;GoodsIter!=pGoodList->end();GoodsIter++)
			{
				pGoods = GoodsIter->pGoods;
				if(pGoods!=NULL && pGoods->GetIndex()==dwIndex&&!pGoods->IsLocked())
				{
					lExtendID = GetGoodsContainerID(pGoods);
					pPos = GoodsIter->lPos;
					return pGoods;
				}
			}
		}
	}

	//在仓库中查找
	/*list<tagGoods> *pDepotGoodsList=GetGame()->GetInterface()->GetDepotPage()->GetDepotList();
	GoodsIter=pDepotGoodsList->begin();
	for(;GoodsIter!=pDepotGoodsList->end();GoodsIter++)
	{
	CGoods* pGoods = GoodsIter->pGoods;
	if(pGoods && pGoods->GetExID() == guID)
	{	
	if(pPos)pPos[0]=GoodsIter->lPos;
	return pGoods;
	}
	}*/
	//在个人商店里查找
	return NULL;
}


//发送一个使用消耗性道具的消息给gameserver
void CPlayer::UseItem(DWORD dwIndex,int nExtendId,int iPos)
{
	long lEffectVal=CGoodsList::GetEffectVal(dwIndex,84,1);
	if( !GetGame()->GetMainPlayer()->GetHorse()->IsVisible() && lEffectVal )//坐骑
	{
		if( GetState() == STATE_FIGHT || IsPlayerShopOpened())
		{
            GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("Package_12")); //您现在不能使用该物品！
			return;
		}
	}
	tagGoods* pstGoods = NULL;
	CGoods* pGoods = NULL;
	if (nExtendId==PEI_PACKET)
	{
		pstGoods = GetGoods(iPos);
		if (pstGoods!=NULL)
		{
			pGoods = pstGoods->pGoods;
		}else
			return;
	}else if (nExtendId<=PEI_PACK5&&nExtendId>=PEI_PACK1)
	{
		pstGoods = GetSubGoods(nExtendId-PEI_PACK1,iPos);
		if (pstGoods!=NULL)
		{
			pGoods = pstGoods->pGoods;
		}else
			return;
	}else if (nExtendId == -1&&iPos == -1)
	{
		pGoods = FindFirstGoodsByIndex(dwIndex,nExtendId,iPos);
	}

	if (pGoods&&nExtendId!=-1&&iPos!=-1)
	{
		CPlayer::tagGoods TagGoods;
		TagGoods.lNum = pGoods->GetNum();
		TagGoods.pGoods = pGoods;
		TagGoods.lType = nExtendId;
		TagGoods.lPos = iPos;

		if(pGoods->GetProperty()->dwIndex==dwIndex && pGoods->GetNum()>=1)
		{
			//for(int i = 0;i<PACK_NUM;i++)
			//{
			//	list<CPlayer::tagGoods>* GoodsList = GetGoodsList(i);
			//	list<CPlayer::tagGoods>::iterator GoodsIter = GoodsList->begin();
			//	for(;GoodsIter!=GoodsList->end();GoodsIter++)
			//	{
			//		if(GoodsIter->pGoods->GetEffectVal(CGoods::GAP_COOLDOWN,2)==pGoods->GetEffectVal(CGoods::GAP_COOLDOWN,2))
			//		{
			//			AddShortCutGoods(GoodsIter->pGoods->GetIndex());
			//		}
			//	}
			//}

			// 王峰：如果是冲能，需要先确认
			if( pGoods->HasEffect( GAP_POWER_INTENSITY ) )
			{
				//CItemPageEx* pItemUI = GetGame()->GetCGuiEx()->GetItemPageEx();
				// 如果手上没有装备则不能冲能
				if( GetEquip( EQUIP_WEAPON ) )
				{

					//pItemUI->m_powerExtendId = nExtendId;
					//pItemUI->m_powerPos	= iPos;

					// 发送 冲能协议
					CMessage msg(MSG_C2S_PLAYER_USEITEM);
					msg.Add((WORD)nExtendId);
					msg.Add((BYTE)iPos);
					msg.Send();
					//GetGame()->GetCGuiEx()->GetMessageBoxPageEx()->m_lCallerID = 1;
					//GetGame()->GetCGuiEx()->GetMessageBoxPageEx()->Open( pItemUI, CStringReading::LoadText( 208, 210 ), CMessageBoxPageEx::MSG_OK_NO, true );
				}
				else
				{
					//GetGame()->GetCGuiEx()->GetMessageBoxPageEx()->m_lCallerID = -1;
					//GetGame()->GetCGuiEx()->GetMessageBoxPageEx()->Open( pItemUI, CStringReading::LoadText( 208, 211 ), CMessageBoxPageEx::MSG_OK, true );
				}
			}
			
			else if (GetGoodsUseState((WORD)dwIndex))
			{
				CGUID shapGuid = CGUID::GUID_INVALID;
				long shapType = 0;	
				long lShapX = 0;
				long lShapY = 0;

				DWORD dwGoodsSkillID = 0;

				//if (pGoods->GetEffectVal(CGoods::GAP_SCRIPT,1)!=0)
				//{
				//	//dwGoodsSkillID = 0;
				//}
				/// 当物品配了触发脚本时就发送固定的技能40050
				/*else */
				if (pGoods->GetEffectVal(GAP_SKILL_CALL_SCRIPT,1)!=0)
				{
					dwGoodsSkillID = 40050;
				}
				else
					dwGoodsSkillID = pGoods->GetEffectVal(GAP_ADD_SKILL,1);
				CShape* pShape = dynamic_cast<CShape*>(GetGame()->GetGameControl()->GetSelectedShape());
				if (pShape!=NULL)
				{
					shapType = pShape->GetType();
					shapGuid = pShape->GetExID();
					lShapX = pShape->GetTileX();
					lShapY = pShape->GetTileY();
				}
				// 当此物品在使用时若能触发某个技能，就发送使用某个技能的消息
				if (dwGoodsSkillID!=0)
				{
					CSkillListXml::tagSkill *pSkill = CSkillListXml::GetProperty(dwGoodsSkillID);
					if (pSkill!=NULL&&pSkill->bHasAct)
					{
						if(GetGame()->GetGameControl()->GetMPActionMode() == CGameControl::MPAM_AutoMove)
							GetGame()->GetGameControl()->AutoMoveStop();
					}
					// 锁定目标技能，非连续攻击技能，同时按下‘~’键，将释放目标设置成自己，且停止目前的自动攻击
					if(pSkill!=NULL&&!pSkill->bLockSkill && pSkill->bLockTarget && GetKeyboard()->IsKeyDown(DIK_GRAVE))
					{
						GetGame()->GetGameControl()->AutoAttactStop();
						pShape = GetGame()->GetMainPlayer();
						shapType = pShape->GetType();
						shapGuid = pShape->GetExID();
						lShapX = pShape->GetTileX();
						lShapY = pShape->GetTileY();
					}
					// 如果需要锁定目标且当前无锁定目标，将攻击目标设为自己
					else if(pSkill != NULL && pShape == NULL && pSkill->bLockTarget)
					{
						pShape = GetGame()->GetMainPlayer();
						shapType = pShape->GetType();
						shapGuid = pShape->GetExID();
						lShapX = pShape->GetTileX();
						lShapY = pShape->GetTileY();
					}
					/***********************************************************************/
					/* 赵航 fix */
					// CSkillXml skill;
					/***********************************************************************/
					ConcreteSkillComp SkillCom;
					//SkillCom的内存交给了CSkillXml的析构来管理
					CSkillXml skill(&SkillCom); 
					if(skill.StepSend(dwGoodsSkillID,1,GetGame()->GetMainPlayer(),(CMoveShape*)pShape,lShapX,lShapY))
					{
						//缓冲技能
						GetGame()->GetGameControl()->SetBufferSkillID(dwGoodsSkillID);
						// 设置触发技能的物品
						GetGame()->GetGameControl()->SetSkillGoods((long)nExtendId,(long)iPos);
						// 对目标使用技能
						GetGame()->GetGameControl()->StartAttack(pShape);
					}
					//pGoods->Lock();
				}
				// 当此物品能被使用但又被点起在手上处于阴影状态时，取消物品的阴影状态。
				if (pGoods->IsHaveShadow())
				{
					pGoods->SetHaveShadowState(false);
				}
				/// 当触发的技能不是40050或者没有触发技能时才发送使用物品的消息
				if (dwGoodsSkillID!=40050)
				{
					// 发送使用物品的消息
					CMessage msg(MSG_C2S_PLAYER_USEITEM);
					msg.Add((WORD)nExtendId);
					msg.Add((BYTE)iPos);
					msg.Add(shapType);
					msg.Add(shapGuid);
					msg.Send();
				}
			}
			else
			{
				//GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,CStringReading::LoadText(208,218),D3DCOLOR_ARGB(255,255,0,0));
			}
		}
	}
}

// 外部使用的一个静态调用接口
void CPlayer::CallUseItem(DWORD dwIndex,int nExtendId,int iPos,CPlayer* pPlayer)
{
	if (pPlayer!=NULL)
	{
		pPlayer->UseItem(dwIndex,nExtendId,iPos);
	}else
		GetGame()->GetMainPlayer()->UseItem(dwIndex,nExtendId,iPos);
}

//获得玩家在某个区域的声望值
DWORD CPlayer::GetCreditForZone(DWORD dwZoneID)
{
	if (m_mapPlayerCreditForZone.find(dwZoneID)!=m_mapPlayerCreditForZone.end())
	{
		return m_mapPlayerCreditForZone[dwZoneID];
	}
	return 0;
}

long CPlayer::CanUseItem(DWORD dwIndex)
{
	DWORD dwVal=0;
	//等级限制检测
	dwVal=CGoodsList::GetEffectVal(dwIndex,6,1);	
	if(dwVal!=0 && dwVal>GetLevel())
		return LEVEL_NOTFIT;

	//力量限制检测
	dwVal=CGoodsList::GetEffectVal(dwIndex,7,1);
	if(dwVal!=0 && dwVal>GetStr())
		return STR_NOTFIT;

	//敏捷限制检测
	dwVal=CGoodsList::GetEffectVal(dwIndex,8,1);
	if(dwVal!=0 && dwVal>GetDex())
		return DEX_NOTFIT;

	//体质限制检测
	dwVal=CGoodsList::GetEffectVal(dwIndex,9,1);
	if(dwVal!=0 && dwVal>GetCon())
		return CON_NOTFIT;

	//职业限制检测
	dwVal=CGoodsList::GetEffectVal(dwIndex,11,1);
	if(dwVal!=0 && dwVal!=GetOccupation()+1)
		return OCCUPATION_NOTFIT;

	//性别限制检测
	dwVal=CGoodsList::GetEffectVal(dwIndex,12,1);
	if(dwVal!=0 && dwVal!=GetSex()+1)
		return SEX_NOTFIT;

	return EQUIPFIT;
}

void  CPlayer::RefreshProperty()
{
	for(int i=0;i<EQUIP_SUM;i++)
	{
		if(m_Equip[i])
		{
			MountEquip(m_Equip[i]);
		}
	}
	for(int i=0;i<EQUIP_SUM;i++)
	{
		if(m_Equip[i])
		{
			MountEquip(m_Equip[i],false);
		}
	}
}


//当属性值改变以后出发
bool CPlayer::OnPropertyChanged(const char* attrName)
{
	if(NULL == attrName)
		return false;

	if(strlen(attrName) == 0)
		return false;

	CMoveShape::OnPropertyChanged(attrName);
	if(this == GetGame()->GetMainPlayer())
	{
		if(strncmp(attrName,"PresentExp",10) == 0)
		{
			/*CMainBarPageEx *pMainPage = GetGame()->GetCGuiEx()->GetMainBarPageEx();
			if(pMainPage)
				pMainPage->UpdataPlayerExp();*/
		}
		// 转职
		else if(strcmp(attrName,"Occu") == 0)
		{
			SortSkillList();
			/*CMainBarPageEx* pMainBarPage = GetGame()->GetCGuiEx()->GetMainBarPageEx();
			if (pMainBarPage)
			{
				for (int i = 0;i<24;i++)
					pMainBarPage->UpdataHotComp(i);
			}
			CSkillPageEx * pSkillPage = GetGame()->GetCGuiEx()->GetSkillPageEx();
			if(pSkillPage && pSkillPage->IsOpen())
			{
				pSkillPage->Close();
				pSkillPage->Open();
			}*/
		}
		// 技能点数（SP）
		else if(strncmp(attrName,"OccuSP",6) == 0)
		{
			//CSkillPageEx* pSkillPage = GetGame()->GetCGuiEx()->GetSkillPageEx();
			//if(pSkillPage && pSkillPage->IsOpen())
			//{
			//	pSkillPage->UpdataOccupExp();
			//	// 更新当前职业对应的技能列表
			//	if(pSkillPage->GetCurActPage() == 0)
			//		pSkillPage->UpdataOccupSkill(pSkillPage->GetCurSelectedOccup(),pSkillPage->GetCurOccupSkillPage());
			//}
		}
		// 升级、专职、学习副职业整理可搜索的任务列表
		if(strcmp(attrName,"Level") == 0 || 
			strcmp(attrName,"Occu") == 0 ||
			strcmp(attrName,"DOccu") == 0)
		{
			TidySearchQuestList();
		}
	}
	if(strncmp(attrName,"Ride",4) == 0)
	{
		UpdateRideShow();
	}
	else if(strncmp(attrName,"Invisible",9) == 0)
	{
		if(GetAction() < ACT_PERFORM)
		{
			extern bool g_bForceUpdate;
			g_bForceUpdate = true;
			SetAction(GetAction());
		}
	}

	// 魔化属性更新，很多。。。
	else
	{
		/*CCharacterPageEx* pCharacterPageEx = ::GetGame()->GetCGuiEx()->GetCharacterPageEx();
		if( pCharacterPageEx )
		{
			pCharacterPageEx->UpdateMagicInfo();
		}*/
	}
	return true;
}

//得到属性值的字符串表示
const char* CPlayer::GetPropertyStringValue(const char* pszPropertyName)
{
	return CMoveShape::GetPropertyStringValue(pszPropertyName);
}

// 察看指定的玩家是否是本队伍的(guid)
bool CPlayer::IsTeammate(const CGUID& id)
{
	vector<tagWSMemberInfo>::iterator iter = m_vecTeamMateInfo.begin();
	for (;iter != m_vecTeamMateInfo.end();++iter)
	{
		tagWSMemberInfo t = *iter;
		if ( t.guid == id ) return true;
	}
	return false;
}

// 察看指定的玩家是否是本队伍的(名字)
bool CPlayer::IsTeammate(const char *szName)
{
	string strName = szName;
	vector<tagWSMemberInfo>::iterator iter = m_vecTeamMateInfo.begin();
	for (;iter != m_vecTeamMateInfo.end();++iter)
	{
		tagWSMemberInfo t = *iter;
		if ( t.szName == strName ) 
			return true;
	}
	return false;
}

// 获得指定ID成员的信息
tagWSMemberInfo * CPlayer::GetWsMemberInfo(const CGUID& id)
{
	vector<tagWSMemberInfo>::iterator iter = m_vecTeamMateInfo.begin();
	for (;iter != m_vecTeamMateInfo.end();++iter)
	{
		tagWSMemberInfo t = *iter;
		if ( t.guid == id )
			return &(*iter);
	}
	return NULL;
}

void CPlayer::SetCaptainID(CGUID& id)
{
	m_CaptainID = id;
	if(this != GetGame()->GetMainPlayer())
		return;
	// 本人当队长
	/*if(m_CaptainID == GetGame()->GetMainPlayer()->GetExID())
	{
		GetGame()->GetCGuiEx()->GetMainBarPageEx()->SetTeamCapPic(true);
		GetGame()->GetCGuiEx()->GetTeamPageEx()->SetTeamCapPic(false);
	}
	else
	{
		GetGame()->GetCGuiEx()->GetMainBarPageEx()->SetTeamCapPic(false);
		GetGame()->GetCGuiEx()->GetTeamPageEx()->SetTeamCapPic(true);
	}*/
}

// 是否可以操作组队相关
bool CPlayer::IsOperforTeam()
{
	return true;//CheckSkill(m_lLimtSkillID) >= m_lLimtSkillLev?true:false;
}
//////////////////////////////////////////////////////////////////////////
//	技能
//////////////////////////////////////////////////////////////////////////

// 添加技能
bool CPlayer::AddSkill(long lSkillId, WORD wLevel/* =1 */,
					   DWORD dwRestoreTime/* =0 */, 
					   float RangeMin/* =1 */, 
					   float RangMax/* =1 */, 
					   WORD wCostMP/* =0 */)
{
	for (list<tagSkill>::iterator it=m_listSkillID.begin(); it!=m_listSkillID.end(); it++)
	{
		if ( it->dwID == lSkillId )
		{
			it->wLevel = wLevel;
			it->dwRestoreTime = dwRestoreTime;
			it->fAffectRangeMin = RangeMin;
			it->fAffectRangeMax = RangMax;
			/*it->wCostMP = wCostMP;*/
			return false;
		}
	}

	tagSkill st;
	st.dwID = lSkillId;
	st.wLevel = wLevel;
	st.dwRestoreTime = dwRestoreTime;
	st.fAffectRangeMin = RangeMin;
	st.fAffectRangeMax = RangMax;
	/*st.wCostMP = wCostMP;*/
	m_listSkillID.push_back(st);
	//CSkillPageEx *pPage = GetGame()->GetCGuiEx()->GetSkillPageEx();
	//if(IsAddOccupedList(st.dwID) && pPage && pPage->IsOpen())
	//	pPage->UpdataOccupList();
	SortSkillList();
	return true;
}

// 删除技能
bool CPlayer::DelSkill(DWORD lSkillId)
{
	for (list<tagSkill>::iterator it=m_listSkillID.begin(); it!=m_listSkillID.end(); it++)
	{
		if ( it->dwID == lSkillId )
		{
			m_listSkillID.erase(it);
			return true;
		}
	}
	return false;
}

// 删除技能（位置)
bool CPlayer::DelSkill(long lPos)
{
	long n = 0;
	for (list<tagSkill>::iterator it=m_listSkillID.begin(); it!=m_listSkillID.end(); it++)
	{
		if (n++ == lPos)
		{
			m_listSkillID.erase(it);
			return true;
		}
	}
	return false;
}

// 为技能列表重新排序
void CPlayer::SortSkillList()
{
	if(!m_listSkillID.empty())
		m_listSkillID.sort(Skill_Compare());
}
// 更新玩家的技能属性
void CPlayer::UpdateSkillAttr(long skillid,long level, const char* strKey,long value)
{
	tagSkill* pSkill = GetSkill(skillid);
	if(pSkill)
	{
		//吟唱时间
		if(strcmp("IntonateTime",strKey) == 0)
			pSkill->dwIntonateTime = value;
		//冷却时间
		else if( strcmp("CoolDown",strKey) == 0)
			pSkill->dwRestoreTime = value;
		//消耗mP
		else if( strcmp("SMp", strKey ) == 0)
			pSkill->wCostMP = (WORD)value;
	}
}

// 获取技能
CPlayer::tagSkill*	CPlayer::GetSkill(DWORD dwID)
{
	for (list<tagSkill>::iterator it=m_listSkillID.begin(); it!=m_listSkillID.end(); it++)
	{
		tagSkill* pSkill = &(*it);
		if (pSkill && pSkill->dwID == dwID)
		{
			return pSkill;
		}
	}
	return NULL;
}


// 设置技能等级
bool CPlayer::SetSkillLevel(const char* strName, WORD wLevel, DWORD dwRestoreTime, 
							float RangeMin, 
							float RangeMax,
							WORD wCostMP)
{
	CSkillListXml::tagSkill *pt = CSkillListXml::GetPropertyByName(strName);
	if (pt==NULL)
	{
		return false;
	}
	DWORD dwID = pt->dwSkillID;

	for (list<tagSkill>::iterator it=m_listSkillID.begin(); it!=m_listSkillID.end(); it++)
	{
		if ( it->dwID == dwID )
		{
			it->wLevel = wLevel;
			it->dwRestoreTime = dwRestoreTime;
			/*it->fAffectRangeMin = RangeMin;
			it->fAffectRangeMax = RangeMax;
			it->wCostMP = wCostMP;*/
			return true;
		}
	}

	return false;
}

// 检测是否有指定技能，返回等级 (0代表没有)
WORD CPlayer::CheckSkill(DWORD dwIndex)
{
	for (list<tagSkill>::iterator it=m_listSkillID.begin(); it!=m_listSkillID.end(); it++)
	{
		if ( it->dwID == dwIndex )
		{
			return it->wLevel;
		}
	}

	return 0;
}

// 刷新技能冷却时间(参数标示是否刷新公共冷却时间)
void CPlayer::UpdateSkillCoolDownTime(bool bIsPublicTime)
{
	for (list<tagSkill>::iterator it=m_listSkillID.begin(); it!=m_listSkillID.end(); it++)
	{
		tagSkill *pSkill = &(*it);//GetSkill(it->wID);
		if (pSkill && pSkill->dwRestoreTime>=0)
		{
			//取得流逝时间
			DWORD dwTimeElapse = timeGetTime() - pSkill->dwCoolDownStartTime;

			// 需要更新技能的公共冷却时间
			if(bIsPublicTime)
			{
				// 如果没有冷却开始时间，直接重新设置冷却开始时间
				// 如果有冷却开始时间，同时，剩下的冷却时间小于公共冷却时间，重新设定冷却的开始时间
				if( pSkill->dwCoolDownStartTime == 0 ||
					( (pSkill->dwCoolDownStartTime > 0 && pSkill->dwCoolDownStartTime < 0xffffffff) &&
					(pSkill->dwRestoreTime - dwTimeElapse < m_wPublicCoolTime)
					)
					)
				{
					pSkill->dwCoolDownStartTime = timeGetTime();
					pSkill->bIsPublicCoolEffect = bIsPublicTime;
					continue;
				}
			}
			// cool down
			if (pSkill->dwCoolDownStartTime > 0 && pSkill->dwCoolDownStartTime < 0xffffffff)
			{
				// 属于公共冷却时间
				if(pSkill->bIsPublicCoolEffect)
				{
					// 公共冷却时间到
					if(dwTimeElapse >= m_wPublicCoolTime)
					{
						pSkill->dwCoolDownStartTime = 0;
						pSkill->bIsPublicCoolEffect = false;
					}
				}
				// 技能本身的冷却时间到
				else if (dwTimeElapse >= pSkill->dwRestoreTime)
				{
					pSkill->dwCoolDownStartTime = 0;
				}
			}
			else	// 灰色3秒钟检测时间
			{
				if ( GetCurTickCount() - pSkill->dwCoolDownGrayStartTime > 1500)
				{
					pSkill->dwCoolDownStartTime = 0;
					pSkill->dwCoolDownGrayStartTime = 0;
				}
			}
		}
	}
}

///添加(或设置)物品公共冷却类型的剩余冷却时间
bool CPlayer::AddPublicCooldownType(WORD wType,DWORD dwTime)
{
	map<WORD,PCooldownRTime>::iterator Iter = m_mapPublicCooldown.find(wType);
	if (Iter!=m_mapPublicCooldown.end())
	{
		Iter->second = dwTime;
		return FALSE;
	}else
	{
		m_mapPublicCooldown.insert(pair<WORD,PCooldownRTime>(wType,dwTime));
		return TRUE;
	}
}

///获取某个类型的物品公共冷却剩余时间
DWORD CPlayer::GetReTimeOfPCooldownType(WORD wType)
{
	DWORD dwReTime = 0;
	map<WORD,PCooldownRTime>::iterator Iter = m_mapPublicCooldown.find(wType);
	if (Iter!=m_mapPublicCooldown.end())
	{
		dwReTime = Iter->second;
	}
	return dwReTime;
}

//添加物编号为wGoodsIndex的物品到列表中
bool CPlayer::AddShortCutGoods(ulong wGoodsIndex)
{
	if (CGoodsList::GetEffectVal(wGoodsIndex,GAP_COOLDOWN,1)!=0)
	{
		tagShortCutGoods temp;
		temp.wID = wGoodsIndex;
		temp.wPublicCooldownType = (WORD)CGoodsList::GetEffectVal(wGoodsIndex,GAP_COOLDOWN,2);
		temp.dwRestoreTime = CGoodsList::GetEffectVal(wGoodsIndex,GAP_COOLDOWN,1);
		list<tagShortCutGoods>::iterator it = m_listShortCutGoods.begin();
		for (;it!=m_listShortCutGoods.end();it++)
		{
			/// 如果现有的物品冷却表中有同类的冷却ID（GAP_COOLDOWN的值2相同），则让新加入的物品的冷却开始时间与其同步
			if (temp.wPublicCooldownType==CGoodsList::GetEffectVal(it->wID,GAP_COOLDOWN,2)
				&&it->bCanBeUsed==false)
			{
				temp.dwCoolDownStartTime = it->dwCoolDownStartTime;
			}
			if (it->wID==wGoodsIndex)
			{
				//if (GetGoodsAmount(wGoodsIndex)==0)
				//{
				//	it->bCanBeUsed = true;
				//	return true;
				//}else
				return false;
			}
		}
		m_listShortCutGoods.push_back(temp);
		return true;
	}
	return false;
}

//从列表中删除物编号为wGoodsIndex的物品
bool CPlayer::DelShortCutGoods(ulong wGoodsIndex)
{
	list<tagShortCutGoods>::iterator it = m_listShortCutGoods.begin();
	for (;it!=m_listShortCutGoods.end();it++)
	{
		if (it->wID==wGoodsIndex)
		{
			m_listShortCutGoods.erase(it);
			return true;
		}
	}
	return false;
}

//设置物编号为wGoodsIndex的物品的冷却时间
void CPlayer::SetGoodsRestoreTime(ulong wGoodsIndex,DWORD dwTime)
{
	list<tagShortCutGoods>::iterator it = m_listShortCutGoods.begin();
	for (;it!=m_listShortCutGoods.end();it++)
	{
		if (it->wID==wGoodsIndex)
		{
			it->dwRestoreTime = dwTime;
			break;
		}
	}
}

//设置物编号为wGoodsIndex的物品的冷却开始时间
void CPlayer::SetGoodsCoolDownStartTime(ulong wGoodsIndex,ulong dwTime)
{
	list<tagShortCutGoods>::iterator it = m_listShortCutGoods.begin();
	for (;it!=m_listShortCutGoods.end();it++)
	{
		if (it->wID==wGoodsIndex)
		{
			it->dwCoolDownStartTime = dwTime;
			if (GetGoodsRemainTime(wGoodsIndex)!=0&&dwTime==0)
			{
				SetGoodsRemainTime(wGoodsIndex,0);
			}
			break;
		}
	}
}

///设置物编号为wGoodsIndex的物品的剩余冷却时间
void CPlayer::SetGoodsRemainTime(ulong wGoodsIndex,DWORD dwTime)
{
	list<tagShortCutGoods>::iterator it = m_listShortCutGoods.begin();
	for (;it!=m_listShortCutGoods.end();it++)
	{
		if (it->wID==wGoodsIndex)
		{
			it->dwRemainTime = dwTime;
			if (GetReTimeOfPCooldownType(it->wPublicCooldownType)!=0&&dwTime==0)
			{
				AddPublicCooldownType(it->wPublicCooldownType,dwTime);
			}
			break;
		}
	}
}

///得到物编号为wGoodsIndex的物品的冷却时间
ulong CPlayer::GetGoodsRemainTime(ulong wGoodsIndex)
{
	DWORD dwReTime = 0;
	list<tagShortCutGoods>::iterator it = m_listShortCutGoods.begin();
	for (;it!=m_listShortCutGoods.end();it++)
	{
		if (it->wID==wGoodsIndex)
		{
			DWORD dwPReTime = GetReTimeOfPCooldownType(it->wPublicCooldownType);
			if (dwPReTime!=0)
			{
				it->dwRemainTime = dwPReTime;
				dwReTime = dwPReTime;
			}
			else
				dwReTime = it->dwRemainTime;
		}
	}
	return dwReTime;
}

//得到物编号为wGoodsIndex的物品的冷却时间
ulong CPlayer::GetGoodsRestoreTime(ulong wGoodsIndex)
{
	list<tagShortCutGoods>::iterator it = m_listShortCutGoods.begin();
	for (;it!=m_listShortCutGoods.end();it++)
	{
		if (it->wID==wGoodsIndex)
		{
			return it->dwRestoreTime;
		}
	}
	return 0;
}

//得到物编号为wGoodsIndex的物品的冷却开始时间
ulong CPlayer::GetGoodsCoolDownStartTime(ulong wGoodsIndex)
{
	list<tagShortCutGoods>::iterator it = m_listShortCutGoods.begin();
	for (;it!=m_listShortCutGoods.end();it++)
	{
		if (it->wID==wGoodsIndex)
		{
			return it->dwCoolDownStartTime;
		}
	}
	return 0;
}

//设置物编号为wGoodsIndex的物品当前是否可被使用的状态
void CPlayer::SetGoodsUseState(ulong wGoodsIndex,bool bState)
{
	list<tagShortCutGoods>::iterator it = m_listShortCutGoods.begin();
	for (;it!=m_listShortCutGoods.end();it++)
	{
		if (it->wID==wGoodsIndex)
		{
			//if (bState&&GetGoodsAmount(wGoodsIndex)==0)
			//{
			//	DelShortCutGoods(wGoodsIndex);
			//}else
			it->bCanBeUsed = bState;
			break;
		}
	}
}

//取得物编号为wGoodsIndex的物品当前是否可被使用的状态
bool CPlayer::GetGoodsUseState(ulong wGoodsIndex)
{
	list<tagShortCutGoods>::iterator it = m_listShortCutGoods.begin();
	for (;it!=m_listShortCutGoods.end();it++)
	{
		if (it->wID==wGoodsIndex)
		{
			return it->bCanBeUsed;
		}
	}
	if (CGoodsList::GetEffectVal(wGoodsIndex,GAP_COOLDOWN,1)==0)
	{
		return true;
	}
	return false;
}

//------------------07.11.22liuke技能学习条件判断--------------
// 根据指定的想提升的技能和等级，判断是否能学习
bool CPlayer::IsCanStudySkill(DWORD dwSkillID, WORD wLevel)
{
	CStudySkillCondition::tagCondition * pCondition = NULL;
	pCondition = CStudySkillCondition::GetCondition(dwSkillID,wLevel);
	if(pCondition)
	{
		DWORD dwPlayerOccuLev = GetOccuLvl(GetOccupation());
		long lNeedOccuLev = (long)pCondition->wPlayerLev - (long)GetOccuLvlCoe((eOccupation)GetOccupation(),(WORD)dwPlayerOccuLev);
		if(lNeedOccuLev < 1)
			lNeedOccuLev = 1;
		// 满足基本学习条件
		if(dwPlayerOccuLev >= (WORD)lNeedOccuLev && (WORD)GetSkillPoint() >= pCondition->dwSp)
		{
			vector<CStudySkillCondition::tagConditionEx*>::iterator it = pCondition->vec_ConditionEx.begin();
			for(; it != pCondition->vec_ConditionEx.end(); ++it)
			{
				// 道具附加条件
				if((*it)->wConditionExType==CStudySkillCondition::CONDITIONEX_GOODS)
				{
					CGoodsList::tagGoods2* ptgGoods = CGoodsList::GetProperty((*it)->strValue.c_str());
					if(ptgGoods)
					{
						// 如果背包中的该道具数量不足，返回false
						if((*it)->lValue > GetGoodsAmount(ptgGoods->BaseProperty.dwIndex))
							return false;
					}
				}
				// 前置技能附加条件，只要有一条前置技能不达要求，返回false
				else if((*it)->wConditionExType==CStudySkillCondition::CONDITIONEX_PRESKILL)
				{
					WORD wlev = CheckSkill((DWORD)atoi((*it)->strValue.c_str()));
					// 该前置技能等级不达要求
					if( (*it)->lValue > wlev )
						return false;
				}
				// 冲突限制技能附加条件，只要存在限制技能，返回false
				else if((*it)->wConditionExType==CStudySkillCondition::CONDITIONEX_CONFLICTSKILL)
				{
					WORD wlev = CheckSkill((DWORD)atoi((*it)->strValue.c_str()));
					// 学过这个限制技能
					if(wlev != 0)
						return false;
				}
			}
			return true;
		}
	}
	return false;
}
//-------------------------------------------------------------


void	CPlayer::SetguID(LONG lExtendID,const CGUID& guid)
{
	if( lExtendID==8 )
	{
		//GetGame()->GetInterface()->GetDepotPage()->SetguDepotMoneyID(guid);
	}
	else if( lExtendID==PEI_WALLET )
	{
		SetguMoneyID(guid);
	}
	else if( lExtendID==PEI_SILVERWALLET )
	{
		SetguSilverMoneyID(guid);
	}
	else if( lExtendID==PEI_YUANBAO )
	{
		SetguYuanBaoID(guid);
	}
	else if( lExtendID==6 )
	{
		SetguJiFenID(guid);
	}
}

// 加入要购回的物品
bool CPlayer::AddBuyBackGoods(CGoods* pBuyBackGoods)
{
	if (pBuyBackGoods!=NULL)
	{
		if (m_queBuyBackGoodsItem.size()>=BuyBackGoodsNum)
		{
			DelBuyBackGoods();
		}
		if (pBuyBackGoods!=NULL)
		{
			m_queBuyBackGoodsItem.push_back(pBuyBackGoods);
		}
		return true;
	}
	return false;
}

// 删除一个回购队列中的物品(默认按队列的先进先出删除队列头的数据) 
bool CPlayer::DelBuyBackGoods(int index)
{
	if (!m_queBuyBackGoodsItem.empty())
	{
		if (index == -1)
		{
			CGoods* pGoods = m_queBuyBackGoodsItem.front();
			SAFE_DELETE(pGoods);
			m_queBuyBackGoodsItem.pop_front();
		}
        else if (index >= 0 && index < (WORD)m_queBuyBackGoodsItem.size())
		{
			deque<CGoods*>::iterator GoodsIter = m_queBuyBackGoodsItem.begin();
			for (int i = 0; i < (WORD)m_queBuyBackGoodsItem.size(); i++,GoodsIter++)
			{
				if (i==index)
				{
					break;
				}
			}
			if (GoodsIter!=m_queBuyBackGoodsItem.end())
			{
				SAFE_DELETE(*GoodsIter);
				if (m_queBuyBackGoodsItem.size()==1)
				{
					m_queBuyBackGoodsItem.clear();
				}else
				{
					m_queBuyBackGoodsItem.erase(GoodsIter);
				}
			}

		}
		return true;
	}
	return false;
}

/*
*	功能：自动装备物品到装备栏
*	细节描述：在背包加入一个新物品的时候，如果这个物品可以装备，
*				且原装备栏无装备，则向服务器发送装备消息。
*	参数：	pGoods - 物品
*			lSExtend - 来源容器（只接收背包、子背包或银行中的来源）
*			lDExtend - 目的容器（一般是背包或子背包有效）
*			dwDPos	 - 目的的位置（背包或子背包中的位置）
*	返回值： true - 可以装备，向服务器发送装备消息；false - 不能装备
*	作者: 刘科
*	创建日期: 09.10.23
*	修改日志: -
*/
bool CPlayer::AutoEquip(CGoods * pGoods,long lSExtend,long lDExtend,DWORD dwDPos)
{
	if ( pGoods && IS_PACKETCONTAINER(lDExtend) &&	// 物品要放入背包或子背包
		! IS_PACKETCONTAINER(lSExtend)  &&			// 物品来源不是背包或子背包
		! CDepot::Inst()->IsDepotGC(lSExtend) &&	// 物品来源不是仓库
		PEI_EQUIPMENT != lSExtend &&				// 物品来源不是装备栏
		CanEquip(pGoods) )							// 该物品可被装备
	{
		// 得到该物品的基本类型（对应eGoodsBaseType）
		//DWORD dwType = pGoods->GetProperty()->dwType;
		DWORD equipType = (DWORD)pGoods->GetEquipType();
		WORD wAction = GetAction();
		DWORD dwBind = pGoods->GetEffectVal(GAP_BIND, 1);
		// 眩晕或战斗时不自动换武器
		// 装备后绑定，不自动换
		if( (equipType == EC_E_WEAPON && wAction > CShape::ACT_MEGRIM && wAction != CShape::ACT_PICK) || 
			eBT_DressAndCantUntie == dwBind  ||  eBT_DressAndBind == dwBind)
			return false;
		//CItemPageEx * pPage = GetGame()->GetCGuiEx()->GetItemPageEx();
		// 该物品不是箭矢或坐骑
		if (/*dwType != GT_ARROW && */equipType != EC_E_RIDER )
		{
			// 设置目标容器为装备栏
			//long lDExtendId = PEI_EQUIPMENT;
			long nIndex = -2;
			if (equipType>EC_MEDAL3)
			{
				pGoods->SetHaveShadowState(false);
				//if ( lDExtend == PEI_PACKET )
				//	pPage->UpdataItem(dwDPos);
				//else if ( lDExtend >= PEI_PACK1 && lDExtend <= PEI_PACK5 )
				//	pPage->UpdataSubPackData(lDExtend-PEI_PACK1,dwDPos);
				return false;
			}

			// 原装备栏位没有物品，自动装备
			if ( !GetEquip(equipType) )
			{
				//pPage->ChangeEquip(nIndex,dwDPos,PEI_EQUIPMENT,lDExtend);
				return true;
			}
		}
	}
	return false;
}

/*
* 功能: 增加物品
* 摘要: -
* 参数: -
* 返回值: -
* 作者: -
* 创建日期: -
* 修改日志:
*	2008.12.02 - lpf
*		修正了播放仪态动作的同时,进行换装,会出现的动作被重置现象;
*/
bool CPlayer::AddGoods(CGoods *pGoods,DWORD dwPos,LONG lExtendID)
{
	bool results = false;
	bool bDeleteGoods = false;
	if(pGoods==NULL) return results;

	// 刷新个人商店中的显示
	//CPlayerShopPageEx* pPlayerShopPage = GetGame()->GetCGuiEx()->GetPlayerShopPageEx();
	//if (pPlayerShopPage!=NULL&&pPlayerShopPage->IsOpen()&&pPlayerShopPage->GetCurShopState()==CPlayerShopPageEx::SET_SHOP)
	//{
	//	//CGoods* pUpdateGoods = GetGoodsByGUID(SourceGUID);
	//	int iPos = -1;
	//	if (pPlayerShopPage->IsItemReadyToTrade(pGoods->GetIndex()))
	//	{
	//		pPlayerShopPage->UpdateMyShopList(pGoods->GetExID(),pGoods->GetIndex(),lExtendID,dwPos,pGoods->GetNum());
	//		pPlayerShopPage->Open();
	//	}
	//}

	if( lExtendID==10)
	{
		//GetGame()->GetInterface()->GetContainerPage()->SetGoods(pGoods);
	}
	else if( lExtendID==9 )
	{
		//GetGame()->GetInterface()->GetDepotPage()->AddGoods(pGoods,dwPos);
	}
	else if( lExtendID==8 )
	{
		//GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-09.wav");
		//GetGame()->GetInterface()->GetDepotPage()->SetMoney(pGoods->GetNum());
		//delete pGoods;
	}
	else if( lExtendID==7 )
	{
		//GetGame()->GetInterface()->GetEquipUpgradePage()->SetGoods(pGoods);
	}
	else if( lExtendID==PEI_WALLET )
	{
		//CItemPageEx* pPage = GetGame()->GetCGuiEx()->GetItemPageEx();
		GetGame()->GetAudioList()->Play2DSound("SOUNDS\\equipments\\00018.wav"); 
		//SetguMoneyID(pGoods->GetExID());
		if (m_dwMoney == 0)
			SetguMoneyID(pGoods->GetExID());
		SetMoney(m_dwMoney+pGoods->GetNum());
		/*if (pPage)
		{
			char temp[32];
			sprintf(temp,"%d",m_dwMoney);
			pPage->UpdataYBMoney("Gold",temp);
		}*/
		//SAFEDELETE(pGoods);
		return true;
	}
    else if (lExtendID==PEI_SILVERWALLET)
	{
		//CItemPageEx* pPage = GetGame()->GetCGuiEx()->GetItemPageEx();
		GetGame()->GetAudioList()->Play2DSound("SOUNDS\\equipments\\00018.wav");
		//SetguSilverMoneyID(pGoods->GetExID());
		if (m_dwSilverMoney == 0)
			SetguSilverMoneyID(pGoods->GetExID());
		SetSilverMoney(m_dwSilverMoney+pGoods->GetNum());
		/*if (pPage)
		{
			char temp[32];
			sprintf(temp,"%d",m_dwSilverMoney);
			pPage->UpdataYBMoney("Silver",temp);
		}*/
		//SAFEDELETE(pGoods);
		return true;
	}
	else if( lExtendID==PEI_YUANBAO )
	{
		//CItemPageEx* pPage = GetGame()->GetCGuiEx()->GetItemPageEx();
		SetYuanBao(m_dwYuanBao+pGoods->GetNum());
		//SetguYuanBaoID(pGoods->GetExID());
		if (m_dwYuanBao == 0)
			SetguYuanBaoID(pGoods->GetExID());
		/*if (pPage)
		{
			char temp[32];
			sprintf(temp,"%d",m_dwYuanBao);
			pPage->UpdataYBMoney("Gem",temp);
		}*/
		//SAFEDELETE(pGoods);
		return true;
	}
	else if( lExtendID==PEI_JIFEN )
	{
		SetJiFen(pGoods->GetNum());
		//SAFEDELETE(pGoods);
		return true;
	}
	else if( lExtendID==3 )		// 手上
	{
		//GetGame()->GetInterface()->GetItemsPage()->SetDragState(true);
		//tagGoods stGoods;
		//stGoods.pGoods=pGoods;
		//stGoods.bChange=false;
		//stGoods.lNum=pGoods->GetNum();
		//stGoods.lPos=dwPos;
		//SetHand(&stGoods);
		//results = true;
	}
	else if( lExtendID==PEI_EQUIPMENT )		// 装备
	{
		//CCharacterPageEx *pCharacterPage = GetGame()->GetCGuiEx()->GetCharacterPageEx();
		//CSkillPageEx * pSkillPage = GetGame()->GetCGuiEx()->GetSkillPageEx();
		//if (pCharacterPage==NULL) return false;
		SetEquip(dwPos,pGoods);
		pGoods->SetHaveShadowState(false);
		//DWORD index = pGoods->GetIndex();
		//WORD suitID = CGoodsList::GetEffectVal(index,CGoods::GAP_SUITID,1);
		//if (suitID)
		//{
		//	string strName = pGoods->GetProperty()->strOrginName;
		//	SetupEquip(suitID,strName);
		//}
		//pCharacterPage->UpdataEquip(dwPos);
        
		char str[256]="";
        sprintf(str,AppFrame::GetText("Equip_16"),pGoods->GetName());
		//GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,str,D3DCOLOR_ARGB(255,255,0,0));
		// 该装备是武器（或副手武器），更新习得技能页面的基本攻击技能图标
		//if(pSkillPage && (dwPos == EQUIP_WEAPON || dwPos == EQUIP_WEAPON2||dwPos == EQUIP_WING||dwPos == EQUIP_HEADGEAR))
		//{
		//	pSkillPage->UpdataPlayerBaseSkill();

        if(dwPos == EQUIP_WEAPON ||dwPos == EQUIP_WING||dwPos == EQUIP_P_HEADGEAR)
        {	// 判断没有播放仪态动作时才更新 - Add by Lpf 2008-12-02
			if (!GetPlayStateOfActionAnim())
				UpdateWeaponShow(pGoods);
        }
		GetGame()->GetAudioList()->PlayEquipmentSound(pGoods->GetIndex()); 
        //更新人物属性界面装备ICON
        UpdateEquipmentIcon((eEquipIndex)dwPos);
		SetDisplayModelGroup();

		// 刷新动作
		//GetGame()->GetCGuiEx()->GetEquipEwearPageEx()->CheckEquip();
		extern bool g_bForceUpdate;
		g_bForceUpdate=true;
		SetAction(ACT_STAND);
		results = true;
	}
	else if (lExtendID==PEI_SUBPACK)					//子背包栏
	{
		//GetGame()->GetAudioList()->PlayEquipmentSound(pGoods->GetIndex());
		//CItemPageEx *pItems=GetGame()->GetCGuiEx()->GetItemPageEx();
		//CMainBarPageEx* pMainBarPage = GetGame()->GetCGuiEx()->GetMainBarPageEx();
		pGoods->SetHaveShadowState(false);
		if (dwPos>=PEI_PACK1&&dwPos<=PEI_PACK5)
		{				
			/*if (pItems!=NULL)
			{
				pItems->ClearPackData(dwPos-PEI_PACK1);
				SetSubPGoods(dwPos-PEI_PACK1,pGoods);
				pGoods = NULL;
				pItems->UpdataPackData(dwPos-PEI_PACK1);
				pItems->UpdataPack(dwPos-PEI_PACK1);
				if(pItems->GetSwitchForPack()&&pItems->IsOpen())
				{
					pItems->Close();
					pItems->Open();
				}else
					pItems->UpdataSubPackSize(dwPos-PEI_PACK1);
			}
			if (pMainBarPage!=NULL)
			{
				pMainBarPage->UpdataPacketInfor();
			}*/
			results = true;
		}
	}
	// 放跑墒背包的栏位
	else if (lExtendID==PEI_BUSINESPACKFIELD)
    

	{
		///刷新跑墒背包栏位的界面显示
		//CItemPageEx *pItems=GetGame()->GetCGuiEx()->GetItemPageEx();
		SetBussinesPack(pGoods);

		/*if (pItems!=NULL)
		{
			pItems->CloseBusinessPack();
			pItems->UpdateBusinessPackField();
		}*/
		///////////////////////////////////////////
		//SAFEDELETE(pGoods);
	}
	// 跑墒背包容器
	else if (lExtendID==PEI_BUSINESSPACK)
	{
		//CItemPageEx *pItems=GetGame()->GetCGuiEx()->GetItemPageEx();
		tagGoods *pstGoods= GetBusinessGoods((WORD)dwPos);
		list<tagGoods>* pLGoods = NULL;
		pGoods->SetHaveShadowState(false);
		/*首先判断点击处是否有物品，如果有再判断要放上去的物品的m_dwIndex是否和点击处的物品的m_dwIndex相同，
		如果相同则进行递加处理，否则视作放上了新的物品*/
		if(pstGoods&& pstGoods->pGoods&&pstGoods->pGoods->GetIndex()==pGoods->GetIndex() )
		{
			long num = 0;
			num = pstGoods->pGoods->GetNum()+pGoods->GetNum();
			if (num<=pGoods->GetEffectVal(GAP_MAXOVERLAP,1)&&num>0)
			{				
				pstGoods->pGoods->SetNum(num);
				pstGoods->lNum=num;
				pstGoods->bChange=true;
				bDeleteGoods = true;
				//SAFEDELETE(pGoods);
			}
            else if(num>pstGoods->pGoods->GetEffectVal(GAP_MAXOVERLAP,1))
			{
				long lGoodsNum = pGoods->GetNum();
				pGoods->SetNum(pstGoods->pGoods->GetNum());
				pstGoods->pGoods->SetNum(lGoodsNum);
				pstGoods->lNum = lGoodsNum;
			}			
		}
		else 
		{
			tagGoods stGoods;
			stGoods.bChange=true;
			stGoods.pGoods=pGoods;
			stGoods.lPos=dwPos;
			stGoods.lType = PEI_BUSINESSPACK;
			stGoods.lNum=pGoods->GetNum();
			if (m_CoteBussinesPack!=NULL)
			{
				m_CoteBussinesPack->m_listCoteGoods.push_back(stGoods);
			}
			//pGoods = NULL;
		}
		/*对界面及其数据进行更新*/
		/*if (pItems!=NULL&&pItems->IsBusinessPackOpen())
		{
			pItems->UpdateBusinessPack(dwPos);
			/// 刷新耐久的显示
			pItems->UpdateBusinessPackDurability();
			/// 刷新容积的显示
			pItems->UpdateBusinessPackCapacity();
		}*/
		results = true;
	}
	else if( lExtendID==PEI_PACKET )				//原始背包
	{
		//CItemPageEx *pItems=GetGame()->GetCGuiEx()->GetItemPageEx();
		//CMainBarPageEx* pMainBarPage = GetGame()->GetCGuiEx()->GetMainBarPageEx();
		tagGoods *pstGoods= GetGoods((WORD)dwPos);
		list<tagGoods>* pLGoods = NULL;
		pGoods->SetHaveShadowState(false);
		/*首先判断点击处是否有物品，如果有再判断要放上去的物品的m_dwIndex是否和点击处的物品的m_dwIndex相同，
		如果相同则进行递加处理，否则视作放上了新的物品*/
		if(pstGoods&& pstGoods->pGoods&&pstGoods->pGoods->GetIndex()==pGoods->GetIndex() )
		{
			long num = 0;
			num = pstGoods->pGoods->GetNum()+pGoods->GetNum();
			if (num<=pGoods->GetEffectVal(GAP_MAXOVERLAP,1)&&num>0)
			{				
				pstGoods->pGoods->SetNum(num);
				pstGoods->lNum=num;
				pstGoods->bChange=true;
				bDeleteGoods = true;
				//SAFEDELETE(pGoods);
			}else if(num>pstGoods->pGoods->GetEffectVal(GAP_MAXOVERLAP,1))
			{
				long lGoodsNum = pGoods->GetNum();
				pGoods->SetNum(pstGoods->pGoods->GetNum());
				pstGoods->pGoods->SetNum(lGoodsNum);
				pstGoods->lNum = lGoodsNum;
			}			
		}
		else 
		{
			tagGoods stGoods;
			stGoods.bChange=true;
			stGoods.pGoods=pGoods;
			stGoods.lPos=dwPos;
			stGoods.lType = PEI_PACKET;
			stGoods.lNum=pGoods->GetNum();
			pLGoods = GetGoodsList();
			if (pLGoods)
			{
				pLGoods->push_back(stGoods);
			}
		}
		/*对界面及其数据进行更新*/
        
        GetInst(PackagePage).UpdatePage(dwPos);
		results = true;
	}
	else if (lExtendID == PEI_PACK1||lExtendID == PEI_PACK2||lExtendID == PEI_PACK3||lExtendID == PEI_PACK4||lExtendID == PEI_PACK5) //子背包
	{	
		tagGoods* pCoteGood = GetSubGoods((WORD)lExtendID-PEI_PACK1,(WORD)dwPos);
		list<tagGoods>* pLGoods = GetSubGoodslist((WORD)lExtendID-PEI_PACK1);
		pGoods->SetHaveShadowState(false);
		/*首先判断点击处是否有物品，如果有再判断要放上去的物品的m_dwIndex是否和点击处的物品的m_dwIndex相同，
		如果相同则进行递加处理，否则视作放上了新的物品*/
		if (pCoteGood&&pCoteGood->pGoods&&pCoteGood->pGoods->GetIndex()==pGoods->GetIndex())
		{
			long num = 0;
			num = pCoteGood->pGoods->GetNum()+pGoods->GetNum();
			if (num<=pGoods->GetEffectVal(GAP_MAXOVERLAP,1)&&num>0)
			{	
				pCoteGood->pGoods->SetNum(num);
				pCoteGood->lNum = num;
				pCoteGood->bChange = true;
				bDeleteGoods = true;
				//SAFEDELETE(pGoods);
			}
            else if (num>pCoteGood->pGoods->GetEffectVal(GAP_MAXOVERLAP,1))
			{
				long lGoodsNum = pGoods->GetNum();
				pGoods->SetNum(pCoteGood->pGoods->GetNum());
				pCoteGood->pGoods->SetNum(lGoodsNum);
				pCoteGood->lNum = lGoodsNum;
			}
		}
        else 
		{
			tagGoods stGoods;
			stGoods.bChange=true;
			stGoods.pGoods=pGoods;
			stGoods.lType = lExtendID;
			stGoods.lPos=dwPos;
			stGoods.lNum=pGoods->GetNum();
			if (pLGoods)
			{
				pLGoods->push_back(stGoods);
			}
		}		
		results = true;
	}
	// 王峰：处理 仓库的物品添加消息
	else if( CGCMana::Inst()->AddGoods( pGoods, lExtendID, dwPos ) )
	{
		results = true;

		// 更新仓库
	}

	if (results&&lExtendID!=3&&lExtendID!=PEI_SUBPACK)
	{	
		// 检查物品的套装属性
		WORD SuitID = (WORD)pGoods->GetEffectVal(GAP_SUITID,1);			
		if (SuitID!=0)
		{
			string stOrName = pGoods->GetProperty()->strOrginName;	
			if (lExtendID==2)
			{
				SetupEquip(SuitID,stOrName);
			}
			AddSuit(SuitID,stOrName);
		}
	}
	if (bDeleteGoods)
	{
		SAFEDELETE(pGoods);
	}
    else
	{
		pGoods = NULL;
	}

	// 更新增值商店货币显示
	if (results)
		FireUIEvent(SHOPCITY_PAGE_NAME,SHOPCITY_PAGE_EVENT_UPDATEMONEYEDBOXES);

	return results;
}



/*
* 功能: 移除物品
* 摘要: -
* 参数: -
* 返回值: -
* 作者: -
* 创建日期: -
* 修改日志:
*	2008.12.02 - lpf
*		修正了播放仪态动作的同时,进行换装,会出现的动作被重置现象;
*/
CGoods *CPlayer::RemoveGoods(const CGUID& SourceGUID,const CGUID& DesGUID,DWORD dwPos,LONG lExtendID,DWORD dwAmount)
{
	// 王峰：删除物品时刷新界面
	
	bool bIsRemoveBack = false;
	CGoods* preGoods = NULL;
	CMainPlayerHand* pHand = 0;
	if (pHand!=NULL)
	{
		tagGoods *pstGoods = pHand->GetTGoodsOfMainPlayerHand();
		if( pstGoods!=NULL&&pstGoods->pGoods!=NULL&&pstGoods->pGoods->GetExID()==SourceGUID )
		{
			pHand->ClearMainPlayerHand();
		}
	}

	// 刷新个人商店中的显示


	if( lExtendID==10 )	// 容器
	{
		//CGoods *pGoods=GetGame()->GetInterface()->GetContainerPage()->GetGoods();
		//GetGame()->GetInterface()->GetContainerPage()->SetGoods(NULL);
	}
	else if( lExtendID==9 )	// 仓库
	{
		//CGoods *pGoods=NULL;//GetGame()->GetInterface()->GetDepotPage()->RemoveGoods(dwPos,dwAmount);
		//return preGoods;
	}
	else if( lExtendID==8 )	// 银行
	{
		//GetGame()->GetInterface()->GetDepotPage()->SetMoney(GetGame()->GetInterface()->GetDepotPage()->GetMoney()-dwAmount);
		//preGoods=new CGoods();
		//preGoods->SetExID(SourceGUID);
		//preGoods->GetProperty()->dwType = eGoodsBaseType::GT_MONEY;
		//preGoods->SetNum(dwAmount);
		//return preGoods;
	}
	else if( lExtendID==7 )	// 升级
	{
		//CGoods* pGoods=GetGame()->GetInterface()->GetEquipUpgradePage()->GetGoods();
		//GetGame()->GetInterface()->GetEquipUpgradePage()->SetGoods(NULL);
		//return NULL;//pGoods;
	}
	else if( lExtendID==PEI_WALLET )	// 金币钱包
	{
		SetMoney(GetMoney()-dwAmount);
		preGoods=new CGoods();
		preGoods->SetExID(SourceGUID);
		preGoods->SetIndex(CGlobeSetup::GetSetup()->dwMoneyIndex);
		preGoods->GetProperty()->dwType = GBT_MONEY;
		preGoods->SetNum(dwAmount);
		char strValue[64];
		sprintf(strValue,"%d",GetMoney());
		//pItems->UpdataYBMoney("Gold",strValue);
		//return preGoods;
	}
	else if( lExtendID==PEI_SILVERWALLET )	// 银币钱包
	{
		SetSilverMoney(GetSilverMoney()-dwAmount);
		preGoods=new CGoods();
		preGoods->SetExID(SourceGUID);
		preGoods->SetIndex(CGlobeSetup::GetSetup()->dwSilverMoneyIndex);
		preGoods->GetProperty()->dwType = GBT_SILVERMONEY;
		preGoods->SetNum(dwAmount);
		char strValue[64];
		sprintf(strValue,"%d",GetSilverMoney());
		//pItems->UpdataYBMoney("Silver",strValue);
		//return preGoods;
	}
	else if( lExtendID==PEI_YUANBAO )	// 元宝
	{
		SetYuanBao(GetYuanBao() - dwAmount);
		preGoods=new CGoods();
		preGoods->SetExID(SourceGUID);
		preGoods->SetType(TYPE_GOODS);
		preGoods->SetIndex(YUANBAO_INDEX);
		preGoods->SetNum(dwAmount);
		char strValue[64];
		sprintf(strValue,"%d",GetYuanBao());
		//pItems->UpdataYBMoney("Gem",strValue);
		//return pGoods;
	}
	else if( lExtendID==PEI_JIFEN )	// 积分
	{
		SetJiFen(GetJiFen() - dwAmount);
		preGoods=new CGoods();
		preGoods->SetExID(SourceGUID);
		preGoods->SetType(TYPE_GOODS);
		preGoods->SetIndex(JIFEN_INDEX);
		preGoods->SetNum(dwAmount);
		//return pGoods;
	}

	else if( lExtendID==3 )	//手
	{
		//RemoveGoods(SourceGUID,DesGUID,pstGoods->lPos,pstGoods->lType,pstGoods->pGoods->GetNum());
		//GetGame()->GetInterface()->GetItemsPage()->SetDragState(false);
		//return pGoods;
	}
	else if( lExtendID==PEI_EQUIPMENT )	//装备
	{
		//int nIndex=(int)dwPos;
		//CGoods *pGoods = NULL;

		preGoods=GetEquip(dwPos);
		SetEquip(dwPos,NULL);
		
		// 该装备是武器（或副手武器），更新习得技能页面的基本攻击技能图标
		if((dwPos == EQUIP_WEAPON ||dwPos == EQUIP_WING||dwPos == EQUIP_P_HEADGEAR))
		{
			// 判断没有播放仪态动作时才更新 - Add by Lpf 2008-12-02
			if (!GetPlayStateOfActionAnim())
				RemoveWeaponShow(preGoods);
		}

		SetDisplayModelGroup();
        UpdateEquipmentIcon(eEquipIndex(dwPos));

	}
	// 放跑墒背包的栏位
	else if (lExtendID == PEI_BUSINESPACKFIELD)
	{
		
	}
	// 跑墒背包容器
	else if (lExtendID==PEI_BUSINESSPACK)
	{
		int nIndex=(int)dwPos;
		if (m_CoteBussinesPack==NULL) return NULL;
		list<tagGoods> *pGoodList = &m_CoteBussinesPack->m_listCoteGoods;
		if (pGoodList==NULL)
		{
			return NULL;
		}
		CGoods *pTmp = NULL;
		list<CPlayer::tagGoods>::iterator it = pGoodList->begin();

		for(;it!=pGoodList->end();)
		{
			/*根据传入的GUID查找物品链表中对应的物品*/
			if( it->pGoods->GetExID()==SourceGUID )
			{
				/*如果传入的操作数量小于物品的数量，则表示要进行拆分操作，否则只进行拿起所有物品的操作*/
				if( it->pGoods->GetNum() > (WORD)dwAmount && dwAmount )
				{
					DWORD Id = it->pGoods->GetIndex();
					preGoods = it->pGoods;
					pTmp = new CGoods();
					CGoodsList::FillGoodsInfo(Id,pTmp);
					preGoods->SetNum(preGoods->GetNum()-dwAmount);
					pTmp->SetExID(DesGUID);
					pTmp->SetType(preGoods->GetType());
					pTmp->SetIndex(preGoods->GetIndex());
					pTmp->SetNum(dwAmount);
					pTmp->SetBaseProperty(preGoods->GetBaseProperty());
					pTmp->SetName(preGoods->GetName());
					pTmp->SetGraphicsID(preGoods->GetGraphicsID());
					it->lNum=preGoods->GetNum();
					preGoods = pTmp;
					it->pGoods->SetHaveShadowState(false);
				}
				else
				{
					preGoods = it->pGoods;
					it = pGoodList->erase(it);
				}					
				break;
			}
			else
			{
				it++;
			}
		}	
		
		bIsRemoveBack = true;
	}
	else if( lExtendID==PEI_PACKET )	//背包
	{
		int nIndex=(int)dwPos;
		list<tagGoods> *pGoodList= GetGoodsList();
		if (pGoodList==NULL)
		{
			return NULL;
		}
		CGoods *pTmp = NULL;
		list<CPlayer::tagGoods>::iterator it = pGoodList->begin();

		for(;it!=pGoodList->end();)
		{
			/*根据传入的GUID查找物品链表中对应的物品*/
			if( it->pGoods->GetExID()==SourceGUID )
			{
				/*如果传入的操作数量小于物品的数量，则表示要进行拆分操作，否则只进行拿起所有物品的操作*/
				if( it->pGoods->GetNum() > (WORD)dwAmount && dwAmount )
				{
					DWORD Id = it->pGoods->GetIndex();
					preGoods = it->pGoods;
					pTmp = new CGoods();
					CGoodsList::FillGoodsInfo(Id,pTmp);
					preGoods->SetNum(preGoods->GetNum()-dwAmount);
					pTmp->SetExID(DesGUID);
					pTmp->SetType(preGoods->GetType());
					pTmp->SetIndex(preGoods->GetIndex());
					pTmp->SetNum(dwAmount);
					pTmp->SetBaseProperty(preGoods->GetBaseProperty());
					pTmp->SetName(preGoods->GetName());
					pTmp->SetGraphicsID(preGoods->GetGraphicsID());
					it->lNum=preGoods->GetNum();
					preGoods = pTmp;
					it->pGoods->SetHaveShadowState(false);
				}
				else
				{
					preGoods = it->pGoods;
					it = pGoodList->erase(it);
				}					
				break;
			}
			else
			{
				it++;
			}
		}	
		bIsRemoveBack = true;
        GetInst(PackagePage).UpdatePage(dwPos);
	}
	else if (lExtendID==PEI_SUBPACK)						//子背包栏
	{
		int nIndex=(int)dwPos;
		if (nIndex>=PEI_PACK1&&nIndex<=PEI_PACK5)
		{				
			preGoods = GetCotePackage(nIndex-PEI_PACK1)->pGoods;
			if (preGoods==NULL)
			{
				return NULL;
			}
			
		}
	}
	else if (lExtendID == PEI_PACK1||lExtendID == PEI_PACK2||lExtendID == PEI_PACK3||lExtendID == PEI_PACK4||lExtendID == PEI_PACK5)
	{
		int nIndex=(int)dwPos;
		CGoods *pTmp = NULL;
		list<tagGoods> *pGoodList = GetSubGoodslist((WORD)lExtendID-PEI_PACK1);
		if (pGoodList==NULL)
		{
			return NULL;
		}
		list<CPlayer::tagGoods>::iterator it = pGoodList->begin();
		for(;it!=pGoodList->end();)
		{
			/*根据传入的GUID查找物品链表中对应的物品*/
			if( it->pGoods->GetExID()== SourceGUID)
			{
				/*如果传入的操作数量小于物品的数量，则表示要进行拆分操作，否则只进行拿起所有物品的操作*/
				if( it->pGoods->GetNum() > (WORD)dwAmount && dwAmount )
				{
					DWORD Id = it->pGoods->GetIndex();
					preGoods=it->pGoods;
					pTmp=new CGoods();
					CGoodsList::FillGoodsInfo(Id,pTmp);
					preGoods->SetNum(preGoods->GetNum()-dwAmount);
					pTmp->SetExID(DesGUID);
					pTmp->SetType(preGoods->GetType());
					pTmp->SetIndex(preGoods->GetIndex());
					pTmp->SetNum(dwAmount);
					pTmp->SetBaseProperty(preGoods->GetBaseProperty());
					pTmp->SetName(preGoods->GetName());
					pTmp->SetGraphicsID(preGoods->GetGraphicsID());
					it->lNum=preGoods->GetNum();
					preGoods = pTmp;
					it->pGoods->SetHaveShadowState(false);
				}
				else
				{
					preGoods = it->pGoods;
					it = pGoodList->erase(it);
				}					
				break;
			}else
			{
				it++;
			}
		}	
		
		bIsRemoveBack = true;
	}
	// 王峰：处理 仓库的物品删除消息
	else if( CGoods* pGoodsInDepot = CGCMana::Inst()->DelGoods( SourceGUID, DesGUID, dwAmount ) )
	{
		preGoods = pGoodsInDepot;
	}

	//检查人物身上的套装情况，并根据情况作删除套装信息的操作
	if (preGoods)
	{
		WORD SuitID = (WORD)preGoods->GetEffectVal(GAP_SUITID,1);
		if (SuitID!=0)
		{
			string stOrName = preGoods->GetProperty()->strOrginName;
			if (lExtendID==2)
			{
				UnfixEquip(SuitID,stOrName);
			}
			RemoveEquipFromSuit(SuitID,stOrName);
			if (GetNumOfPossessionSuit(SuitID)==0)
			{
				RemoveSuit(SuitID);
			}
		}
		//------------察看任务列表是否有该物品有关的任务追踪需要更新显示(07.9.28刘科添加)----------- 
		if(bIsRemoveBack)
		{
			// 更新增值商店货币显示
			FireUIEvent(SHOPCITY_PAGE_NAME,SHOPCITY_PAGE_EVENT_UPDATEMONEYEDBOXES);
		}
	}

	// 刷新邮件物品显示

	return preGoods;
}

//-----------07.9.14liuke修改（所有背包中查找）-------------
long CPlayer::GetGoodsAmount(long nIdx)
{
	long ret = 0;
	WORD index = 0;
	while(GetGoodsList(index))
	{
		for(tagGoodsIter iter = GetGoodsList(index)->begin();
			iter!=GetGoodsList(index)->end(); ++iter)
		{
			if( iter->pGoods->GetIndex() == nIdx ) ret += iter->pGoods->GetNum();
		}
		++index;
	}
	return ret;
}

//根据物品名称查找人物身上所有的此类物品的总量
long CPlayer::GetGoodsAmount(const char* cGoodsName)
{
	long ret = 0;
	WORD index = 0;
	while(GetGoodsList(index))
	{
		for(tagGoodsIter iter = GetGoodsList(index)->begin();
			iter!=GetGoodsList(index)->end(); ++iter)
		{
			if( strcmp(iter->pGoods->GetOrginName(),cGoodsName) == 0 ) ret += iter->pGoods->GetNum();
		}
		++index;
	}
	return ret;
}

//bool CPlayer::HasPetArount()
//{
//	return m_setPetID.size()>0;
//}
//
//long CPlayer::GetPetAmount()
//{
//	return m_setPetID.size();
//}

void CPlayer::SetEmotion(long lIndex,DWORD dwTime)
{
	//if(lIndex<0)
	//{
	//	m_pEmotionShower->Stop();
	//	return;
	//}
	//if(GetAction()!=ACT_STAND && GetAction()!=ACT_IDLE && this==GetGame()->GetMainPlayer() )
	//{
	//	//GetGame()->GetInterface()->AddText("您现在不能使用表情！",0xFFFF0000);
	//	return;
	//}
	//if(GetHorse()->IsVisible() && this==GetGame()->GetMainPlayer() )
	//{
	//	//GetGame()->GetInterface()->AddText("您现在不能使用表情！",0xFFFF0000);
	//	return;
	//}
	//if(IsPlayerShopOpened() && this==GetGame()->GetMainPlayer() )
	//{
	//	//GetGame()->GetInterface()->AddText("您现在不能使用表情！",0xFFFF0000);
	//	return;
	//}
	//if(IsDied() && this==GetGame()->GetMainPlayer() )
	//{
	//	//GetGame()->GetInterface()->AddText("您现在不能使用表情！",0xFFFF0000);
	//	return;
	//}
	//static DWORD dwLastEmotionTime=0;
	//if(GetCurTickCount()-dwLastEmotionTime<5000 && this==GetGame()->GetMainPlayer() )
	//{
	//	//GetGame()->GetInterface()->AddText("您不能频繁的使用表情！",0xFFFF0000);
	//	return;
	//}
	//dwLastEmotionTime=GetCurTickCount();
	//if(g_bDebug)
	//	LoadEmotionsConfig("data/emotions.ini");
	//if(vecEmotions.empty())
	//	LoadEmotionsConfig("data/emotions.ini");
	//m_pEmotionShower->Stop();
	//m_pEmotionShower->SetActionInfo(m_cAnimInfo.GetActionInfo());
	//m_pEmotionShower->Init(lIndex);
	//m_pEmotionShower->Start(dwTime);
	//if(this==GetGame()->GetMainPlayer())
	//{
	//	CMessage msg(MSG_C2S_SHAPE_EMOTION);
	//	msg.Add(GetType());
	//	msg.Add(GetExID());
	//	msg.Add(lIndex+1);
	//	msg.Send();
	//}
}

bool CPlayer::IsEmotionShowing()
{
	return !m_pEmotionShower->IsStopped();
}

//设置当前所拿的工具
void CPlayer::SetToolsID(DWORD dwBeavioralID,DWORD dwActionID)
{
	if (dwBeavioralID==CDeputyOccuSystem::OB_TaskCollection)
	{
		switch(dwActionID)
		{
		case 2:
			m_dwToolIndex = 318033;	//矿工锄
			break;
		case 3:
			m_dwToolIndex = 318035;	//采药镰刀
			break;
		case 4:
			m_dwToolIndex = 318031;	//剥皮小刀
			break;
		default:
			break;
		}
	}else if (dwBeavioralID==CDeputyOccuSystem::OB_Collection)
	{
		switch(this->GetAssOccu())
		{
		case DOccu_Smithing:
			m_dwToolIndex = 318033;	//矿工锄
			break;
		case DOccu_Metallurgy:
			m_dwToolIndex = 318035;	//采药镰刀
			break;
		case DOccu_Sewing:
			m_dwToolIndex = 318031;	//剥皮小刀
			break;
		default:
			break;
		}
	}else if (dwBeavioralID==CDeputyOccuSystem::OB_Facture)
	{
		switch(this->GetAssOccu())
		{
		case DOccu_Smithing:
			m_dwToolIndex = 318034;	//锻造铁锤
			break;
		case DOccu_Metallurgy:
			m_dwToolIndex = 318036;	//炼制烧杯
			break;
			//case DOccu_Sewing:
			//	m_dwToolIndex = 1180;	//制衣剪刀
			//	break;
		default:
			break;
		}
	}else
	{
		m_dwToolIndex = 0;
	}
}

void CPlayer::SetAction(WORD l)
{
	DWORD dwCurTime = GetCurTickCount();
	//char strInfo[512]="";
	//_snprintf(strInfo,512,CStringReading::LoadText(eIDS_CONTROL,eIDS_MOVETEST_15),
	//	dwCurTime,l,GetTileX(),GetTileY());
	//PutStringToFile("MoveTest",strInfo);

	if(IsEmotionShowing())
	{
		if(l==ACT_IDLE||l==ACT_STAND)
			return;
	}
	if(!m_pEmotionShower->IsStopped())
		m_pEmotionShower->Stop();
	if( m_Horse.IsVisible() )
	{
		int eState = CHorse::HS_STAND ;
		switch( l )
		{
		case ACT_BEATTACK:
		case ACT_WALK:
		case ACT_RUN:
		case ACT_IDLE:	
		case ACT_STAND:
		case ACT_MISS:
		case ACT_MEGRIM:
			eState = l;
			break;
		}
		m_Horse.ChangeStatus( eState );

	}
	if( m_Wing.IsVisible() )
	{
		int eState = CWing::WING_STAND ;
		switch( l )
		{
		case ACT_WALK:
		case ACT_RUN:
		case ACT_IDLE:
		case ACT_STAND:
			eState = l;
			break;
		}
		m_Wing.ChangeStatus( eState );
	}
	if( m_HeadWear.IsVisible() )
	{
		int eState = CHeadWear::HeadWear_STAND ;
		switch( l )
		{
		case ACT_WALK:
		case ACT_RUN:
		case ACT_IDLE:
		case ACT_STAND:
			eState = l;
			break;
		}
		m_HeadWear.ChangeStatus( eState );
	}
	if (l >= ACT_MINE && l <= ACT_MINEMAX)
	{
		/*if (GetState() == STATE_FIGHT)
		{
		m_LeftHandWeapon.SetVisible(false);
		m_RightHandWeapon.SetVisible(false);
		}
		else
		{
		m_LeftHandWeapon.SetVisible(true);
		m_RightHandWeapon.SetVisible(true);
		}*/
		m_CollectTool.SetVisible(true);
		m_CollectTool.SetLocator(LOCATOR_RIGHTHAND);
		if (!GetToolsID())
		{
			m_CollectTool.SetVisible(false);
		}
		else
		{
			m_CollectTool.SetModelID(GetToolsID());
			m_CollectTool.SetDisplayModel();
		}

	}
	else
	{
		m_LeftHandWeapon.SetVisible(true);
		m_RightHandWeapon.SetVisible(true);
		m_CollectTool.SetModelID(0);
		m_CollectTool.SetVisible(false);
	}
	CMoveShape::SetAction(l);
	UpdateSpecialEquipment();

}
void CPlayer::SetState(WORD l)
{
	WORD state = GetState();
	CMoveShape::SetState(l);
	UpdateSpecialEquipment();
	// 玩家自己，根据状态的变化设置人物图标上的闪动效果
	if(this == GetGame()->GetMainPlayer())
	{
		//CMainBarPageEx * pPage = GetGame()->GetCGuiEx()->GetMainBarPageEx();
		//if(pPage)
		//{
		//	// 战斗状态
		//	if(l == STATE_FIGHT && state != l)
		//		pPage->UpdataStateIcon(true);
		//	else if(l != STATE_FIGHT)
		//		pPage->UpdataStateIcon(false);
		//}
	}
}

/// 改变指定容器里的某个物品的数量（参数：容器ID、操作对象类型、操作对象的GUID、操作对象改变后的数量）
bool CPlayer::ChangeObjectAmount(LONG lContainerExtendID,LONG lObjectType,CGUID& guObjectID,DWORD dwAmount)
{
	bool ret = false;
	CGoods * pGoods = NULL;
	if(lContainerExtendID==PEI_WALLET)
	{
		
		SetMoney(dwAmount);
		char strValue[64];
		sprintf(strValue,"%d",dwAmount);
		
		ret = true;
        FireUIEvent("PackagePage","UpdateMoney");
	}
	else if(lContainerExtendID==PEI_SILVERWALLET)
	{
		
		SetSilverMoney(dwAmount);
		char strValue[64];
		sprintf(strValue,"%d",dwAmount);
		FireUIEvent("PackagePage","UpdateMoney");
		ret = true;
	}
	else if(lContainerExtendID==PEI_BUSINESSPACK)
	{
		
		tagGoodsIter it;
		if (m_CoteBussinesPack!=NULL)
		{
			it = m_CoteBussinesPack->m_listCoteGoods.begin();
			for(;it!=m_CoteBussinesPack->m_listCoteGoods.end();it++)
			{
				if( it->pGoods->GetExID()==guObjectID && it->pGoods->GetType()==lObjectType )
				{
					int nIndex = it->lPos;
					it->pGoods->SetNum(dwAmount);
					it->lNum=dwAmount;
					
					/// 刷新容积的显示
					ret = true;
					pGoods = it->pGoods;
					break;
				}
			}
		}
	}
	else if(lContainerExtendID==PEI_PACKET)
	{
		tagGoodsIter it;
		for(it=GetGoodsList()->begin();it!=GetGoodsList()->end();it++)
		{
			if( it->pGoods->GetExID()==guObjectID && it->pGoods->GetType()==lObjectType )
			{
				int nIndex = it->lPos;
				it->pGoods->SetNum(dwAmount);
				it->lNum=dwAmount;
				/*更新界面数据及显示*/
				//pItems->ClearItemGoodData(nIndex);
				//pItems->UpdataItemData(nIndex);
				//pItems->UpdataItem(nIndex);
                GetInst(PackagePage).UpdatePage(nIndex);
				ret = true;
				pGoods = it->pGoods;
				break;
			}
		}
	}
	else if (lContainerExtendID<=PEI_PACK5&&lContainerExtendID>=PEI_PACK1)
	{
		
		tagGoodsIter it;
		list<tagGoods>* pLGoods = GetSubGoodslist((WORD)lContainerExtendID - PEI_PACK1);
		if (!pLGoods) return false;
		for(it=pLGoods->begin();it!=pLGoods->end();it++)
		{
			if( it->pGoods->GetExID()==guObjectID && it->pGoods->GetType()==lObjectType )
			{
				int nIndex = it->lPos;
				it->pGoods->SetNum(dwAmount);
				it->lNum=dwAmount;
				
				ret = true;
				pGoods = it->pGoods;
				break;
			}
		}
	}
	else if(lContainerExtendID==PEI_YUANBAO)
	{
		
		SetYuanBao(dwAmount);
		char strValue[64];
		sprintf(strValue,"%d",dwAmount);
		FireUIEvent(SHOPCITY_PAGE_NAME,SHOPCITY_PAGE_EVENT_UPDATEMONEYEDBOXES);
		ret = true;
	}
	else if(lContainerExtendID==PEI_JIFEN)
	{
		SetJiFen(dwAmount);
		FireUIEvent(SHOPCITY_PAGE_NAME,SHOPCITY_PAGE_EVENT_UPDATEMONEYEDBOXES);
		ret = true;
	}
	//------------察看任务列表是否有该物品有关的任务追踪需要更新显示(07.9.28刘科添加)----------- 
	if(pGoods)
	{
		// 刷新物品相关的任务
		UpdataQuestByGoods(pGoods);
		//CPlayerShopPageEx* pPlayerShopPage = GetGame()->GetCGuiEx()->GetPlayerShopPageEx();
		//if (pPlayerShopPage!=NULL&&pPlayerShopPage->IsOpen()&&pPlayerShopPage->GetCurShopState()==CPlayerShopPageEx::SET_SHOP)
		//{
		//	int lPos = -1;
		//	int iPos = -1;
		//	CGoods* pChangeGoods = GetGoodsByGUID(guObjectID,&lPos);
		//	if (pChangeGoods!=NULL&&pPlayerShopPage->IsItemOnShopPage(GetGoodsOnPlayer(lContainerExtendID,lPos),iPos))
		//	{
		//		pPlayerShopPage->UpdateMyShopList(guObjectID,pChangeGoods->GetIndex(),lContainerExtendID,lPos,dwAmount);
		//		pPlayerShopPage->SetbSetTrue(false);
		//		pPlayerShopPage->Open();
		//	}
		//}
		FireUIEvent(SHOPCITY_PAGE_NAME,SHOPCITY_PAGE_EVENT_UPDATEMONEYEDBOXES);
	}
	return ret;
}

bool CPlayer::ClearGoods()
{
	return false;
}


CEmotionShower::CEmotionShower(CPlayer *pPlayer)
{
	//m_pActionInfo=NULL;
	m_nCurIndex=-1;
	m_pSource=pPlayer;
}

CEmotionShower::~CEmotionShower()
{
	Stop();
}

//void CEmotionShower::SetActionInfo(GameModel::AnimInfo::tagActionInfo *pActionInfo)
//{
//	//m_pActionInfo=pActionInfo;
//}
void CEmotionShower::Init(int eType)
{
	if(eType<0||eType >= (int)vecEmotions.size()) return;
	m_stEmotion=vecEmotions[eType];
	m_stEmotion.nSoundChannel=-1;
}
void CEmotionShower::Start(DWORD dwTime)
{
	//如果玩家先前已经在场景中做了表情，只看到动作，不显示提示文字
	if( m_stEmotion.strText!="null" && dwTime==0 )
	{
		char msg[128];
		sprintf(msg,m_stEmotion.strText.c_str(),m_pSource->GetName());
		//GetGame()->GetInterface()->AddText(msg);
	}
	if( m_stEmotion.strSound!="null" )
	{
		//如果玩家先前已经在场景做了表情，只看到动作，不播放声音
		if( dwTime==0 || m_stEmotion.nSoundLoops==0 )
		{
			sprintf(m_stEmotion.sounds,m_stEmotion.strSound.c_str(),m_pSource->GetOccupation());
			m_stEmotion.nSoundChannel=GetGame()->GetAudioList()->Play3DSound(m_stEmotion.sounds,m_stEmotion.nSoundLoops,m_pSource->GetPosY(),m_pSource->GetHeight(),m_pSource->GetPosX());
		}
	}
	m_nCurIndex=0;
	//根据玩家做表情的时间，往后推动作播放的索引，假设每个动作播放的时间是1秒
	while(dwTime >= 1000 && m_nCurIndex < (WORD)m_stEmotion.vecActions.size())
	{
		if(m_stEmotion.vecTypes[m_nCurIndex]>0)
		{
			dwTime=0;
			break;
		}
		m_nCurIndex++;
		dwTime-=1000;
	}
	if(!IsStopped())
		ShowNextAction();
}
void CEmotionShower::ShowNextAction()
{
	/*if(m_pActionInfo==NULL)return;
	DWORD dwCurrentTime=GetCurTickCount();

	m_pActionInfo->bInterpolation = FALSE;
	m_pActionInfo->bCurActionLooped=(m_stEmotion.vecTypes[m_nCurIndex]!=1);
	m_pActionInfo->dwCurActionLoopCount = 0;
	m_pActionInfo->dwOldAction = m_pActionInfo->dwCurAction;
	m_pActionInfo->dwOldActionTimeElapse = m_pActionInfo->dwCurActionElapseTime;
	m_pActionInfo->dwInterpolationElapseTime = 0;
	m_pActionInfo->dwInterpolationStartTime  = dwCurrentTime;
	m_pActionInfo->dwInterpolationTimeLength = 240;	
	m_pActionInfo->dwCurActionElapseTime = 0;
	m_pActionInfo->dwCurActionStartTime = dwCurrentTime;
	m_pActionInfo->dwCurAction = m_stEmotion.vecActions[m_nCurIndex];*/
}
bool CEmotionShower::IsStopped()
{
	//return m_nCurIndex<0||m_nCurIndex>=m_stEmotion.vecActions.size()||m_pActionInfo==NULL;
	return true;
}
void CEmotionShower::Stop()
{
	if(m_stEmotion.nSoundChannel>0)
	{
		GetGame()->GetAudioList()->Stop3DSound(m_stEmotion.sounds,m_stEmotion.nSoundChannel);
		//char msg[64];
		//sprintf(msg,"释放声音:%s,%d",m_stEmotion.strSound.c_str(),m_stEmotion.nSoundChannel);
		//GetGame()->GetInterface()->AddText(msg);
		m_stEmotion.nSoundChannel=-1;
	}
	if(IsStopped())
		return;
	m_nCurIndex=-1;
	RestoreToNormal();
}
void CEmotionShower::RestoreToNormal()
{
	extern bool g_bForceUpdate;
	g_bForceUpdate=true;
	WORD wOriginAction=CShape::ACT_STAND;
	//if(m_pSource->GetState()==CShape::STATE_FIGHT)
	//	wOriginAction=m_pSource->GetAction();
	m_pSource->SetAction(wOriginAction);
}
void CEmotionShower::Update()
{
	
}

//得到显示拥有场景的IcongID
WORD CPlayer::GetDisplayOwneRegionIcon()
{
	if(m_OwnedRegions.size() > 0)
	{
		list<tagownedReg>::iterator it = m_OwnedRegions.begin();
		for(;it != m_OwnedRegions.end();it++)
		{
			if( (*it).wType == 2 )
				return 2;
		}
		return 1;
	}
	return 0;
}

//======================任务相关方法（07.9.6刘科注释）=======================//
// 添加指定步骤的一个任务
bool CPlayer::AddQuest(long lQuestID, long lStepID)
{
	const CQuest *pQuest = CQuestSystem::GetQuest(lQuestID);
	// 任务配置存在，玩家自身任务列表中加入这个任务记录
	if(pQuest && m_RoleQueue.AddOneRecord(lQuestID,lStepID))
	{
        OutputDebugStr(AppFrame::GetText("Quest_4"));
		CQuestRecord* pRecord = m_RoleQueue.GetRecordByID(lQuestID);
		if(pRecord)
		{
			// 界面添加
			OutputDebugStr(AppFrame::GetText("Quest_5"));
            CQuestSystem::SetTempQuestRecord(pRecord);
            FireUIEvent("Quest/Tab/Normal/Tree", "AddNormalQuest");
			OutputDebugStr(AppFrame::GetText("Quest_6"));
			// 提示信息
			OutputDebugStr(AppFrame::GetText("Quest_7"));
			// 接受了任务后，将该任务所对应的发起NPC的任务提示状态列表更新
			CMonster *pNpc = dynamic_cast<CMonster *>(GetGame()->GetRegion()->RecursiveFindObject(TYPE_MONSTER,(char *)pQuest->GetAttribute().GetNPCName().c_str()));
			if(pNpc)
			{
				pNpc->RemoveStateByQuest(CMonster::STATE_QUEST,lQuestID);
				OutputDebugStr(AppFrame::GetText("Quest_8"));
			}
			// 在自身的任务列表中查找并更新该NPC相关的任务提示信息
			UpdataQuestInfoAboutNpcByQuestID(lQuestID);
			// 有新任务时，整理可搜索的任务列表
			TidySearchQuestList();
			OutputDebugStr(AppFrame::GetText("Quest_9"));
			return true;
		}
	}
	return false;
}


// 删除一个任务
const char * CPlayer::RemoveQuest(long lQuestID)
{
	// 清空该任务下的所有地图特效
	DelAllQuestInfoByRgn(lQuestID);
	// 如果玩家任务列表中有这个任务
	CQuestRecord* pRecord = m_RoleQueue.GetRecordByID(lQuestID);
	if(pRecord)
	{
		const CQuest *pQuest = CQuestSystem::GetQuest(lQuestID);
		if(pQuest)
		{
            //从界面删除
            CQuestSystem::SetTempQuestRecord(pRecord);
            FireUIEvent("Quest/Tab/Normal/Tree", "DeleteNormalQuest");

			CMonster *pNpc = NULL;
			// 删除任务后，将该任务的当前步骤和当前步骤的所有任务追踪NPC的任务提示状态列表更新
			long lStep = pRecord->GetQuestStep();
			const CQuestStep* pStep = pQuest->GetStep(lStep);
			if(pStep)
			{
				// 找到这个任务的当前步骤所对应的目标NPC，并更新该NPC的任务提示状态
				pNpc = dynamic_cast<CMonster *>(GetGame()->GetRegion()->RecursiveFindObject(TYPE_MONSTER,(char *)pStep->GetNPCName().c_str()));
				if(pNpc)
					pNpc->RemoveStateByQuest(CMonster::STATE_STEP,lQuestID,lStep);
				// 还需要更新当前步骤下，所有追踪目标NPC的任务提示状态
				const CQuestStep* pStep = pQuest->GetStep(lStep);
				if(pStep)
				{
					long lNum = 0;
					const CQuestTrace* pTrace = pStep->GetQuestTraceHead();
					while(pTrace)
					{
						pNpc = dynamic_cast<CMonster *>(GetGame()->GetRegion()->RecursiveFindObject(TYPE_MONSTER,(char *)pTrace->GetNPCName().c_str()));
						if(pNpc)
							pNpc->RemoveStateByQuest(CMonster::STATE_TRACE,lQuestID,lStep,lNum);
						pTrace = pTrace->next;
						lNum++;
					}
				}
			}
			// 从玩家的任务列表中删除该任务
            m_RoleQueue.DelOneRecord(lQuestID);
			
			// 删除任务记录后，将周围所有NPC的任务状态刷新一次（该任务作为其他任务的前置任务时，刷新）
			GetGame()->GetRegion()->UpdataAllNpcQuestState();
			//string strName=pQuest->GetAttribute().GetNPCName();
			//pNpc = dynamic_cast<CMonster *>(GetGame()->GetRegion()->RecursiveFindObject(TYPE_MONSTER,(char *)strName.c_str()));
			//UpdataSpecialQuestInfoByNpc(pNpc);
			// 整理可搜索的任务列表
			TidySearchQuestList();
		}
	}
	return NULL;
}


// 更新一个任务步骤
bool CPlayer::UpdataQuestStep(long lQuestID, long lStepIndex)
{
	CQuestRecord* pRecord = m_RoleQueue.GetRecordByID(lQuestID);
	if(pRecord)
	{
		const CQuest *pQuest = CQuestSystem::GetQuest(lQuestID);
		if(pQuest)
		{
			long lStep = pRecord->GetQuestStep();
			const CQuestStep* pStep = pQuest->GetStep(lStep);
			if(pStep)
			{
				CMonster *pNpc = NULL;
				// 将该任务之前步骤信息，从对应NPC的任务信息列表中删除
				pNpc = dynamic_cast<CMonster *>(GetGame()->GetRegion()->RecursiveFindObject(TYPE_MONSTER,(char *)pStep->GetNPCName().c_str()));
				if(pNpc)
					pNpc->RemoveStateByQuest(CMonster::STATE_STEP,lQuestID,lStep);
				// 还需要更新当前步骤下，所有追踪目标NPC的任务提示状态
				const CQuestStep* pStep = pQuest->GetStep(lStep);
				if(pStep)
				{
					long lNum = 0;
					const CQuestTrace* pTrace = pStep->GetQuestTraceHead();
					while(pTrace)
					{
						pNpc = dynamic_cast<CMonster *>(GetGame()->GetRegion()->RecursiveFindObject(TYPE_MONSTER,(char *)pTrace->GetNPCName().c_str()));
						if(pNpc)
							pNpc->RemoveStateByQuest(CMonster::STATE_TRACE,lQuestID,lStep,lNum);
						pTrace = pTrace->next;
						lNum++;
					}
				}
			}
		}
		// 更新步骤
		pRecord->JumpStepTo((uchar)lStepIndex);
		// 清空该任务下的所有地图特效
		DelAllQuestInfoByRgn(lQuestID);
		// 在自身的任务列表中查找并更新该NPC相关的任务提示信息
		UpdataQuestInfoAboutNpcByQuestID(lQuestID);
		return true;
	}
	return false;
}

// 更新一个任务的任务追踪
bool CPlayer::UpdataQuestTarget(long lQuestID, long lIndex, long lValue,long lRegionID, long x, long y)
{
	CQuestRecord* pRecord = m_RoleQueue.GetRecordByID(lQuestID);
	if(pRecord)
	{
		pRecord->SetParam(lIndex,lValue);
		long lEffectOk = 0;
		long lEffectNO = 0;
		// 找到相应的任务追踪
		const CQuestTrace* pTrace = CQuestSystem::GetQuestTrace(lQuestID,pRecord->GetQuestStep(),lIndex);
		if(pTrace)
		{
			lEffectOk = pTrace->GetNPCEffect_1();
			lEffectNO = pTrace->GetNPCEffect_2();
		}

		// 添加该追踪相关的一个地图特效信息
		if(lRegionID > 0)
		{
			m_RoleQueue.SetRegionID(lQuestID,pRecord->GetQuestStep(),lIndex,lRegionID);
			m_RoleQueue.SetRegionX(lQuestID,pRecord->GetQuestStep(),lIndex,x);
			m_RoleQueue.SetRegionY(lQuestID,pRecord->GetQuestStep(),lIndex,y);
			stQuestInfoByRgn tagInfo;
			tagInfo.lQuestID = lQuestID;
			tagInfo.lIndex = lIndex;
			tagInfo.lRgnID = lRegionID;
			tagInfo.lRgnX = x;
			tagInfo.lRgnY = y;
			tagInfo.lEffectIDOK = lEffectOk;
			tagInfo.lEffectIDNO = lEffectNO;
			// 设置该条追踪的地图特效信息
			SetQuestInfoByRgn(lQuestID,lIndex,tagInfo);
		}
		// 如果该追踪没有相关地图特效，删除相关信息
		else
			DelQuestInfoByRgn(lQuestID,lIndex);
		
		UpdataQuestInfoAboutNpcByQuestID(lQuestID);
		return true;
	}
	return false;
}


// 更新指定任务的NPC的相关任务提示信息
void CPlayer::UpdataQuestInfoAboutNpcByQuestID(long lQuestID)
{
	CQuestRecord* pRecord = m_RoleQueue.GetRecordByID(lQuestID);
	if(pRecord)
	{
		const CQuest *pQuest = CQuestSystem::GetQuest(lQuestID);
		if(pQuest)
		{
			const CQuestStep* pStep = pQuest->GetStep(pRecord->GetQuestStep());
			if(pStep)
			{
				CMonster *pTNpc = NULL;
				// 比较该步骤的具体追踪目标NPC
				long lParam = 0;
				long lNum = 0;
				// 临时变量，用于表示该步骤是否达到完成条件（每个追踪达到完成条件）
				bool bIsTraceFinish = false;
				bool bIsStepFinish = true;
				const CQuestTrace* pTrace = pStep->GetQuestTraceHead();
				while(pTrace)
				{
					string strID = pTrace->GetGoodsStrID();
					// 如果该追踪的目标有原始名，表示需要在玩家背包查找该物品数量，然后设置给该追踪项数量。
					if(!strID.empty())
					{
						CGoodsList::tagGoods2* ptgGoods = CGoodsList::GetProperty(strID.c_str());
						if(ptgGoods)
							lParam = GetGoodsAmount(ptgGoods->BaseProperty.dwIndex);
					}
					// 已完成数
					else
						lParam = pRecord->GetParam(pTrace->GetParam());
					// 该条追踪达到完成条件
					if(lParam >= pTrace->GetSum())
						bIsTraceFinish = true;
					// 没有达到完成条件
					else
					{
						bIsTraceFinish = false;
						bIsStepFinish = false;
					}
					// 在玩家可见范围内查找对应的NPC，如果存在，则设置其状态
					pTNpc = dynamic_cast<CMonster *>(GetGame()->GetRegion()->RecursiveFindObject(TYPE_MONSTER,(char *)pTrace->GetNPCName().c_str()));
					if(pTNpc)
						pTNpc->SetStateByQuest(CMonster::STATE_TRACE,pTrace->GetNPCEffect_1(),pTrace->GetNPCEffect_2(),
						bIsTraceFinish,lQuestID,pRecord->GetQuestStep(),lNum);
					pTrace = pTrace->next;
					lNum++;
				}
				// 步骤的目标NPC。。。
				pTNpc = dynamic_cast<CMonster *>(GetGame()->GetRegion()->RecursiveFindObject(TYPE_MONSTER,(char *)pStep->GetNPCName().c_str()));
				if(pTNpc)
				{
					pTNpc->SetStateByQuest(CMonster::STATE_STEP,pStep->GetNPCEffect_1(),pStep->GetNPCEffect_2(),
						bIsStepFinish,lQuestID,pRecord->GetQuestStep());
				}
			}			
		}
	}
    FireUIEvent("Quest/Tab","UQuestText");
}

// 更新指定NPC，在玩家任务列表中的相关任务提示信息
void CPlayer::UpdataQuestInfoAboutNpc(CMonster *pNpc)
{
	CMonster * pTNpc = pNpc;
	// 在本玩家的任务列表中查找与该NPC相关的任务目标NPC，并设置这些NPC的任务（或任务的步骤或追踪）的状态
	list<CQuestRecord*>::iterator it = m_RoleQueue.GetQuestRecordList()->begin();
	for(; it!=m_RoleQueue.GetQuestRecordList()->end(); ++it)
	{
		const CQuest *pQuest = CQuestSystem::GetQuest((*it)->GetQuestID());
		if(pQuest)
		{
			const CQuestStep* pStep = pQuest->GetStep((*it)->GetQuestStep());
			if(pStep)
			{
				// 比较该步骤的具体追踪目标NPC
				long lParam = 0;
				long lNum = 0;
				// 临时变量，用于表示该步骤是否达到完成条件（每个追踪达到完成条件）
				bool bIsTraceFinish = false;
				bool bIsStepFinish = true;
				const CQuestTrace* pTrace = pStep->GetQuestTraceHead();
				while(pTrace)
				{
					string strID = pTrace->GetGoodsStrID();
					// 如果该追踪的目标有原始名，表示需要在玩家背包查找该物品数量，然后设置给该追踪项数量。
					if(!strID.empty())
					{
						CGoodsList::tagGoods2* ptgGoods = CGoodsList::GetProperty(strID.c_str());
						if(ptgGoods)
							lParam = GetGoodsAmount(ptgGoods->BaseProperty.dwIndex);
					}
					// 已完成数
					else
						lParam = (*it)->GetParam(pTrace->GetParam());
					// 该条追踪达到完成条件
					if(lParam >= pTrace->GetSum())
						bIsTraceFinish = true;
					// 没有达到完成条件
					else
					{
						bIsTraceFinish = false;
						bIsStepFinish = false;
					}
					// 如果指定NPC为空，那么更新改追踪所表示的NPC的任务提示状态
					if(pTNpc==NULL)
					{
						// 在玩家可见范围内查找该NPC，如果存在，则设置其状态
						pTNpc = dynamic_cast<CMonster *>(GetGame()->GetRegion()->RecursiveFindObject(TYPE_MONSTER,(char *)pTrace->GetNPCName().c_str()));
						// 该NPC是本国的或是中立的
						if(pTNpc && (pTNpc->GetCountry() == GetCountry() || pTNpc->GetCountry() == 7))
							pTNpc->SetStateByQuest(CMonster::STATE_TRACE,pTrace->GetNPCEffect_1(),pTrace->GetNPCEffect_2(),
							bIsTraceFinish,(*it)->GetQuestID(),(*it)->GetQuestStep(),lNum);
					}
					// 找到该条追踪的相关NPC
					else if(strcmp(pTNpc->GetOrigName(), pTrace->GetNPCName().c_str()) == 0)
					{
						if(pTNpc->GetCountry() == GetCountry() || pTNpc->GetCountry() == 7)
						{
							pTNpc->SetStateByQuest(CMonster::STATE_TRACE,pTrace->GetNPCEffect_1(),pTrace->GetNPCEffect_2(),
								bIsTraceFinish,(*it)->GetQuestID(),(*it)->GetQuestStep(),lNum);
						}
					}
					pTrace = pTrace->next;
					lNum++;
				}
				// 比较步骤的目标NPC。。。
				pTNpc = dynamic_cast<CMonster *>(GetGame()->GetRegion()->RecursiveFindObject(TYPE_MONSTER,(char *)pStep->GetNPCName().c_str()));
				if(pTNpc)
				{
					pTNpc->SetStateByQuest(CMonster::STATE_STEP,pStep->GetNPCEffect_1(),pStep->GetNPCEffect_2(),
						bIsStepFinish,(*it)->GetQuestID(),(*it)->GetQuestStep());
				}
			}			
		}
	}
}


// 更新场景上的特殊任务相关NPC的提示信息
void CPlayer::UpdataSpecialQuestInfoByNpc(CMonster* pNpc)
{
	if (!pNpc) return;
	// 该NPC不是本国的同时也不是中立的，返回
	if (pNpc->GetCountry() != GetCountry() && pNpc->GetCountry() != 7)
		return;
	// 获得该NPC作为发起者所能够发起的任务列表
	vector<LONG> vec_quest = CQuestSystem::GetSpecialQuestByNPCID(pNpc->GetOrigName());
	for (int i=0; i < (int)vec_quest.size(); ++i)
	{
		const CQuest* pQuest = CQuestSystem::GetQuest(vec_quest[i]);
		// 该任务存在
		if (pQuest)
		{
			// 获得任务属性
			const CQuestAttribute attrib = pQuest->GetAttribute();

            // 前置任务列表
			bool bNoPreQuest = true;
			for (uint size = 0; size < attrib.GetPreQuestList().size(); ++size)
			{
				long id = attrib.GetPreQuestList()[size];
				// 该任务没在所做过任务记录中
				if(!m_RoleQueue.GetQuestFinishedRecord(id))
				{
					bNoPreQuest = false;
					break;
				}
			}
			// 前置任务没做过
			if(!bNoPreQuest)
				continue;
			// 最低最高等级限制
			int iMin = attrib.GetMinGrade();
			int iMax = attrib.GetMaxGrade();
			WORD dwlev = GetLevel();
			// 在玩家身上查看是否存在该任务,
			CQuestRecord* pRecord = m_RoleQueue.GetRecordByID(vec_quest[i]);
			// 如果没有，同时，该任务没在所做过任务记录中
			if(!pRecord && !m_RoleQueue.GetQuestFinishedRecord(vec_quest[i]))
			{
				// 职业条件和副职业条件都达到要求，显示任务提示
				if(attrib.GetOccupationRestrict(GetOccupation()) &&
					attrib.GetDOccuRestrict((BYTE)GetAssOccu()))
				{
					// 等级条件达到
					if(dwlev >= iMin && dwlev <= iMax)
						pNpc->SetStateByQuest(CMonster::STATE_QUEST,attrib.GetNPCShowType(),attrib.GetNPCClewType(),true,vec_quest[i]);
					// 等级条件即将达到
					else if(dwlev >= iMin - 3 && dwlev < iMin)
						pNpc->SetStateByQuest(CMonster::STATE_QUEST,attrib.GetNPCShowType(),attrib.GetNPCClewType(),false,vec_quest[i]);
				}
			}
		}
	}
}
// 采集场景上NPC对应的任务提示信息
void CPlayer::CollectionQuestClewInfo(CMonster *pNpc)
{
	if(!pNpc) return;
	UpdataSpecialQuestInfoByNpc(pNpc);
	// 在自身的任务列表中查找并更新该NPC相关的任务提示信息
	UpdataQuestInfoAboutNpc(pNpc);
}

// 物品变化引起的任务更新
void CPlayer::UpdataQuestByGoods(CGoods *pGoods)
{
	if(pGoods)
	{
		list<CQuestRecord*>::iterator itrec = m_RoleQueue.GetQuestRecordList()->begin();
		for(; itrec != m_RoleQueue.GetQuestRecordList()->end(); ++itrec)
		{
			CQuestRecord* pRecord = (*itrec);
			if(pRecord)
			{
				const CQuest *pQuest = CQuestSystem::GetQuest(pRecord->GetQuestID());
				if(pQuest)
				{
					// 获得该任务当前所在的步骤
					bool bIsFind = false;
					const CQuestStep* pStep = pQuest->GetStep(pRecord->GetQuestStep());
					if(pStep)
					{
						const CQuestTrace* pTrace = pStep->GetQuestTraceHead();
						while(pTrace)
						{
							// 任务追踪相关的物品，更新该任务的追踪说明显示
							if(pTrace->GetGoodsStrID() == pGoods->GetOrginName())
							{
								//pQuestPage->UpdataQuestData(pRecord,false);
								//pQuestPage->CheckQuestIsFinish(pRecord);
								bIsFind = true;
								break;
							}
							pTrace = pTrace->next;
						}
						if(bIsFind)
							break;
					}
				}
			}
		}
		// 更新右边的任务追踪提示信息
		//pQuestPage->UpdataRightTrace();
		// 更新场景可见NPC的任务提示信息
		UpdataQuestInfoAboutNpc();
	}
}


// 设置指定任务的指定序列的地图特效
void CPlayer::SetQuestInfoByRgn(long lQuestID, long lIndex, CPlayer::stQuestInfoByRgn tagInfo)
{
	list<stQuestInfoByRgn>::iterator it = m_lsQuestInfoByRgn.begin();
	for(; it!=m_lsQuestInfoByRgn.end(); ++it)
	{
		if((*it).lQuestID == lQuestID && (*it).lIndex == lIndex)
		{
			(*it).lRgnID = tagInfo.lRgnID;
			(*it).lRgnX = tagInfo.lRgnX;
			(*it).lRgnY = tagInfo.lRgnY;
			(*it).lEffectIDOK = tagInfo.lEffectIDOK;
			(*it).lEffectIDNO = tagInfo.lEffectIDNO;
			// 地图特效切换场景，将原场景的相关特效删除
			if(GetGame()->GetRegion())
			{
				GetGame()->GetRegion()->DeleteChildObject(TYPE_EFFECT,(*it).guidEffect);
				(*it).guidEffect = CGUID::GUID_INVALID;
			}
			//if((*it).pEffect)
			//{
			//(*it).pEffect->SetIsDeleted(true);
			//(*it).pEffect = NULL;
			//}
			return;
		}
	}
	m_lsQuestInfoByRgn.push_back(tagInfo);
}

// 删除指定任务的指定序列的地图特效
void CPlayer::DelQuestInfoByRgn(long lQuestID, long lIndex)
{
	list<stQuestInfoByRgn>::iterator it = m_lsQuestInfoByRgn.begin();
	for(; it!=m_lsQuestInfoByRgn.end(); ++it)
	{
		if((*it).lQuestID == lQuestID && (*it).lIndex == lIndex)
		{
			if((*it).guidEffect != CGUID::GUID_INVALID && GetGame()->GetRegion())
			{
				GetGame()->GetRegion()->DeleteChildObject(TYPE_EFFECT,(*it).guidEffect);
				(*it).guidEffect = CGUID::GUID_INVALID;
			}
			//if((*it).pEffect)
			//{
			//GetGame()->GetRegion()->DeleteChildObject(TYPE_EFFECT,(*it).pEffect->GetExID());
			//(*it).pEffect->SetIsDeleted(true);
			//(*it).pEffect = NULL;
			//}
			m_lsQuestInfoByRgn.erase(it);
			return;
		}
	}
}

// 删除指定任务的所有地图特效
void CPlayer::DelAllQuestInfoByRgn(long lQuestID)
{
	list<stQuestInfoByRgn> lsQuestInfoByRgn = m_lsQuestInfoByRgn;
	list<stQuestInfoByRgn>::iterator it = lsQuestInfoByRgn.begin();
	for(; it!=lsQuestInfoByRgn.end(); ++it)
	{
		if((*it).lQuestID == lQuestID)
			DelQuestInfoByRgn(lQuestID,(*it).lIndex);
	}
}

// 整理可搜索的任务列表（达到可接条件的所有任务）
void CPlayer::TidySearchQuestList()
{
	m_vecCanSearchQuest.clear();
	//vector<CQuest*> vecQuest = CQuestSystem::GetBeSearchQuestList();
	//vector<CQuest *>::iterator it = vecQuest.begin();
	//for (; it != vecQuest.end();++it)
	//{
	//	CQuest *pQuest = (*it);
	//	if(pQuest)
	//	{
	//		const CQuestAttribute attrib = pQuest->GetAttribute();
	//		// 等级条件符合
	//		if(attrib.GetSpDescTextIDBySearch() > 0 && GetLevel() >= attrib.GetMinGrade() && GetLevel() <= attrib.GetMinGrade()+3)
	//		{
	//			// 前置任务列表
	//			bool bNoPreQuest = true;
	//			for (int i=0;i<attrib.GetPreRoleList().size();++i)
	//			{
	//				long id = attrib.GetPreRoleList()[i];
	//				// 该任务没在所做过任务记录中
	//				if(!m_RoleQueue.GetRoleFinishedRecord(id))
	//				{
	//					bNoPreQuest = false;
	//					break;
	//				}
	//			}
	//			// 前置任务没做过,或当前这个任务已经做过了，或者当前这个任务已经接收到了，继续找下一个
	//			if(!bNoPreQuest || m_RoleQueue.GetRoleFinishedRecord(pQuest->GetID())
	//				|| m_RoleQueue.GetRecordByID(pQuest->GetID()))
	//				continue;
	//			// 职业条件和副职业条件都达到要求，显示任务提示
	//			if(attrib.GetOccupationRestrict(GetOccupation()) &&
	//				attrib.GetDOccuRestrict(GetAssOccu()))
	//			{
	//				m_vecCanSearchQuest.push_back(pQuest);
	//			}
	//		}
	//	}
	//}
}

// 随机获得一个符合搜索条件的任务
CQuest * CPlayer::RedomCanSearchQuest(bool bIsReSearch)
{
	if(bIsReSearch || m_vecCanSearchQuest.empty())
		TidySearchQuestList();
	else if(bIsReSearch)
		TidySearchQuestList();
	long size = (long)m_vecCanSearchQuest.size();
	if(size > 0)
	{
		size = random(size);
		return m_vecCanSearchQuest[size];
	}
	return NULL;
}

//======================================================================

//bool CPlayer::DecordQuestDataFromByteArray(BYTE* pByte, long& pos)
//{
//	/*char strTempt[2002];
//	m_lMaxQuestNum = _GetLongFromByteArray(pByte,pos);
//	long lNum = _GetLongFromByteArray(pByte,pos);
//	for(int i = 0; i < lNum; i++)
//	{
//		tagPlayerQuest PlayerQuest;
//		PlayerQuest.wQuestID = _GetWordFromByteArray(pByte,pos);
//		PlayerQuest.strName = _GetStringFromByteArray(pByte,pos,strTempt);
//		PlayerQuest.strDesc = _GetStringFromByteArray(pByte,pos,strTempt);
//		PlayerQuest.bDisplay=_GetCharFromByteArray(pByte,pos)==0?false:true;
//		PlayerQuest.lRegionID = _GetLongFromByteArray(pByte,pos);
//		PlayerQuest.lTileX = _GetLongFromByteArray(pByte,pos);
//		PlayerQuest.lTileY= _GetLongFromByteArray(pByte,pos);
//		PlayerQuest.lEffectID = _GetLongFromByteArray(pByte,pos);
//		PlayerQuest.pEffect = NULL;
//		m_PlayerQuests[PlayerQuest.wQuestID] = PlayerQuest;
//	}
//	GetGame()->GetInterface()->GetTaskPage()->UpdateList();*/
//	return true;
//}
//
////给该玩家添加一个任务
//void CPlayer::AddQuest(WORD wQuestID,char* strName,char* strDesc,bool bDisplay,long lEffectID,long lRegionID,long lTileX,long lTileY)
//{
//	tagPlayerQuest PlayerQuest = {wQuestID,bDisplay,lEffectID,lRegionID,lTileX,lTileY,NULL,strName,strDesc};
//
//	if(GetGame()->GetRegion() && (lRegionID==GetGame()->GetRegion()->GetExID() ||
//		GetGame()->GetDupliRegionSetup()->IsDupliRegion(lRegionID,GetGame()->GetRegion()->GetExID()) ) )
//	{
//		PlayerQuest.pEffect=((CClientRegion *)GetGame()->GetRegion())->PlayEffect(lEffectID,lTileX+0.5f,lTileY+0.5f);
//		PlayerQuest.pEffect->SetLoop(true);
//		PlayerQuest.pEffect->SetTask(true);
//	}
//	m_PlayerQuests[PlayerQuest.wQuestID] = PlayerQuest;
//}
////移出一个任务
//void CPlayer::RemoveQuest(WORD wQuestID)
//{
//	if(((CClientRegion *)GetGame()->GetRegion())->FindChildObject(m_PlayerQuests[wQuestID].pEffect))
//	{
//		m_PlayerQuests[wQuestID].pEffect->SetLoop(false);
//		m_PlayerQuests[wQuestID].pEffect->SetTask(false);
//		GetGame()->GetRegion()->RemoveObject(m_PlayerQuests[wQuestID].pEffect);
//	}
//	m_PlayerQuests.erase(wQuestID);
//}
//
//void CPlayer::UpdateQuestPosXY(WORD wQuestID,long lReginID,long lTileX,long lTileY)
//{
//	map<WORD,tagPlayerQuest>::iterator it = m_PlayerQuests.find(wQuestID);
//	if(it != m_PlayerQuests.end())
//	{
//		tagPlayerQuest& quest = (*it).second;
//		if( GetGame()->GetRegion()->FindChildObject(quest.pEffect) )
//		{
//			quest.pEffect->SetLoop(false);
//			quest.pEffect->SetTask(false);
//			GetGame()->GetRegion()->RemoveObject(quest.pEffect);
//		}
//		quest.lRegionID = lReginID;
//		quest.lTileX = lTileX;
//		quest.lTileY = lTileY;
//
//		if(GetGame()->GetRegion() && (lReginID==GetGame()->GetRegion()->GetExID() ||
//				GetGame()->GetDupliRegionSetup()->IsDupliRegion(lReginID,GetGame()->GetRegion()->GetExID()) ) )
//		{
//			quest.pEffect=((CClientRegion *)GetGame()->GetRegion())->PlayEffect(quest.lEffectID,lTileX+0.5f,lTileY+0.5f);
//			quest.pEffect->SetLoop(true);
//			quest.pEffect->SetTask(true);
//		}
//	};
//}

//==========================================================================//

//技能的选择逻辑
DWORD CPlayer::GetDefaultAttackSkillID()
{
	//##0、空手;1、单手剑;2、单手锤;3、匕首;4、法杖(双手);5、双手剑(双手);6、双手斧(双手);7、弓;8、双剑(双手);9、拳刃(双手)
	//如果职业是魔狩而且装备了弓
	CGoods* pWeapon = GetEquip( EQUIP_WEAPON );
	LONG lWeaponCategory = 0;
	if( pWeapon )
	{
		lWeaponCategory = pWeapon->GetEffectVal( GAP_WEAPON_CATEGORY, 1);
	}
	// 武器类别有问题，按照空手标准返回
	if(lWeaponCategory < 0 || lWeaponCategory > 9)
		lWeaponCategory = 0;
	//if( lWeaponCategory == 7 || lWeaponCategory == 2 || lWeaponCategory == 3 || lWeaponCategory == 5 || lWeaponCategory == 6
	//	|| lWeaponCategory == 4 || lWeaponCategory == 8 || lWeaponCategory == 9)
	//{
	//	//当前攻击技能切换到弓箭
	//	return SKILL_BASE_ATTACK;
	//}
	//else if(lWeaponCategory == 1)
	//{
	//	//否则用普通攻击
	//	return SKILL_BASE_ARCHERY;
	//}
	return SKILL_BASE_ATTACK+lWeaponCategory;
}


/*
*功能：设置魔化属性
*细节描述： 
*参数： eAD		攻防枚举
*		eMT		魔化元素枚举
*返回值： 无
*修改时间：2000-11-4
*编写人：王峰
*/
void CPlayer::SetMagicElem( eAttackDefend eAD, eMagicElement eMT, long lVal )
{
	if( eAD < AD_Max && eMT < MT_Max )
	{
		m_arrElementProperty[ eMT ].uArrPro[ eAD ] = lVal;
	}
}

/*
*功能：获取魔化属性
*细节描述： 
*参数： eAD		攻防枚举
*		eMT		魔化元素枚举
*返回值： 相应的魔化属性值
*修改时间：2000-11-4
*编写人：王峰
*/
long CPlayer::GetMagicElem( eAttackDefend eAD, eMagicElement eMT )
{
	if( eAD < AD_Max && eMT < MT_Max )
	{
		return m_arrElementProperty[ eMT ].uArrPro[ eAD ];
	}

	return -1;
}


/*
*功能：获取总相性
*细节描述： 
*参数： 无
*返回值： 总的相性和
*修改时间：2000-11-4
*编写人：王峰
*/
long CPlayer::GetNumOfXiangXing()
{
	long lNumOfXiangXing = 0;

	for( long c = 0; c < MT_Max; c++ )
	{
		lNumOfXiangXing += this->GetMagicElem( AD_AttackXiang, (eMagicElement)c );
		lNumOfXiangXing += this->GetMagicElem( AD_DefendXiang, (eMagicElement)c );
	}

	return lNumOfXiangXing;
}


/*
* 功能: 更新饰品
* 摘要: -
* 参数: -
* 返回值: -
* 作者: lpf
* 创建日期: 2008.08.27
* 修改日志:
*	2008.09.22 - lpf
*		增加了对显示头饰或身体时,没有换装ID则不ShowGroup()的判断,修正了没有换装ID的头饰下线回到选人画面时,模型显示不出来的问题
*	2008.10.17 - lpf
*		增加了局部变量赋予初始值;
*	2009.03.05 - lpf
*		增加了更新自己的外套时,对外套开关是否开启做了判断;
*	2009.03.13 - lpf
*		增加了披风显示新规则:考虑特殊地图,和自身是否显示开关;
*	2009.03.27 - lpf
*		增加了当外套附加属性为隐藏全身时,也会隐藏披风的规则;
*/
void CPlayer::UpdateAccouterment()
{
	static char str[32];
	static DWORD dwFACE = MAKEFOURCC('F','A','C','E');
	static DWORD dwHAIR = MAKEFOURCC('H','A','I','R');
	static DWORD dwARMO = MAKEFOURCC('A','R','M','O');
	static DWORD dwHAND = MAKEFOURCC('H','A','N','D');
	static DWORD dwFOOT = MAKEFOURCC('F','O','O','T');
	static DWORD dwBODY = MAKEFOURCC('B','O','D','Y');
	static DWORD dwPIFE = MAKEFOURCC('P','I','F','E');		//披风

	DWORD dwHead	 = 0;
	DWORD dwBody	 = 0;
	long  lHeadLevel = 0;
	long  lBodyLevel = 0;

	if (m_bSelf)
	{
		// 头饰处理
		CGoods * pGoods = GetEquip(CPlayer::EQUIP_P_HEADGEAR);
		if (pGoods != NULL)
		{
			dwHead	   = pGoods->GetProperty()->dwEquipID;
			lHeadLevel = pGoods->GetEffectVal(GAP_WEAPON_LEVEL, 1);

			if (pGoods->GetEffectVal(GAP_HIDE_HEAR, 1) != 0)
				m_pDisplayModel->HideGroup(dwHAIR);
			if (pGoods->GetEffectVal(GAP_HIDE_FACE, 1) != 0)
				m_pDisplayModel->HideGroup(dwFACE);
			if (pGoods->GetEffectVal(GAP_HIDE_BODY, 1) != 0)
			{
				m_pDisplayModel->HideGroup(dwARMO);
				m_pDisplayModel->HideGroup(dwHAND);
				m_pDisplayModel->HideGroup(dwFOOT);
			}

			if (dwHead != 0)
			{
				/*sprintf(str, "%04d", dwHead);
				m_pDisplayModel->ShowGroup(dwHEAD, MAKEFOURCC(str[0],str[1],str[2],str[3]), lHeadLevel);*/
				m_HeadWear.SetVisible(true);
			}
		}else
			m_HeadWear.SetVisible(false);

		// 外套处理
		if (GetGame()->GetRegion() == NULL ||
			(GetGame()->GetRegion() != NULL && GetGame()->GetRegion()->IsChangeEquip()))
		{
			if (GetShowFashion() & FASHION_TYPE_GREATCOAT/*!GetGame()->GetCGuiEx()->GetGameConfigPageEx()->GetOption(CGameConfigPageEx::OPTION_NoGreatcoat)*/)
			{
				pGoods = GetEquip(CPlayer::EQUIP_P_FROCK);
				if (pGoods != NULL)
				{
					dwBody	   = pGoods->GetProperty()->dwEquipID;
					lBodyLevel = pGoods->GetEffectVal(GAP_WEAPON_LEVEL, 1);

					if (pGoods->GetEffectVal(GAP_HIDE_HEAR, 1) != 0)
						m_pDisplayModel->HideGroup(dwHAIR);
					if (pGoods->GetEffectVal(GAP_HIDE_FACE, 1) != 0)
						m_pDisplayModel->HideGroup(dwFACE);
					if (pGoods->GetEffectVal(GAP_HIDE_BODY, 1) != 0)
					{
						m_pDisplayModel->HideGroup(dwARMO);
						m_pDisplayModel->HideGroup(dwHAND);
						m_pDisplayModel->HideGroup(dwFOOT);
						m_pDisplayModel->HideGroup(dwPIFE);
					}

					if (dwBody != 0)
					{
						sprintf(str, "%04d", dwBody);
						m_pDisplayModel->ShowGroup(dwBODY, MAKEFOURCC(str[0],str[1],str[2],str[3]), lBodyLevel);
					}
				}else
					m_pDisplayModel->HideGroup(dwBODY);
			}else
				m_pDisplayModel->HideGroup(dwBODY);
		}else if (GetGame()->GetRegion() != NULL && !GetGame()->GetRegion()->IsChangeEquip())
		{
			pGoods = GetEquip(CPlayer::EQUIP_P_FROCK);
			if (pGoods != NULL)
			{
				dwBody	   = pGoods->GetProperty()->dwEquipID;
				lBodyLevel = pGoods->GetEffectVal(GAP_WEAPON_LEVEL, 1);

				if (pGoods->GetEffectVal(GAP_HIDE_HEAR, 1) != 0)
					m_pDisplayModel->HideGroup(dwHAIR);
				if (pGoods->GetEffectVal(GAP_HIDE_FACE, 1) != 0)
					m_pDisplayModel->HideGroup(dwFACE);
				if (pGoods->GetEffectVal(GAP_HIDE_BODY, 1) != 0)
				{
					m_pDisplayModel->HideGroup(dwARMO);
					m_pDisplayModel->HideGroup(dwHAND);
					m_pDisplayModel->HideGroup(dwFOOT);
					m_pDisplayModel->HideGroup(dwPIFE);
				}

				if (dwBody != 0)
				{
					sprintf(str, "%04d", dwBody);
					m_pDisplayModel->ShowGroup(dwBODY, MAKEFOURCC(str[0],str[1],str[2],str[3]), lBodyLevel);
				}
			}else
				m_pDisplayModel->HideGroup(dwBODY);
		}
	}else
	{
		// 头饰处理
		DWORD					dwEquipIndex = GetEquipIndex(CPlayer::EQUIP_P_HEADGEAR);
		CGoodsList::tagGoods2 * pGoods2		 = CGoodsList::GetProperty(dwEquipIndex);
		if (pGoods2 != NULL)
		{
			dwHead	   = pGoods2->BaseProperty.dwEquipID;
			lHeadLevel = GetEquipLevel(CPlayer::EQUIP_P_HEADGEAR);

			if (CGoodsList::GetEffectVal(dwEquipIndex, GAP_HIDE_HEAR, 1) != 0)
				m_pDisplayModel->HideGroup(dwHAIR);
			if (CGoodsList::GetEffectVal(dwEquipIndex, GAP_HIDE_FACE, 1) != 0)
				m_pDisplayModel->HideGroup(dwFACE);
			if (CGoodsList::GetEffectVal(dwEquipIndex, GAP_HIDE_BODY, 1) != 0)
			{
				m_pDisplayModel->HideGroup(dwARMO);
				m_pDisplayModel->HideGroup(dwHAND);
				m_pDisplayModel->HideGroup(dwFOOT);
			}

			if (dwHead != 0)
			{
				/*sprintf(str, "%04d", dwHead);
				m_pDisplayModel->ShowGroup(dwHEAD, MAKEFOURCC(str[0],str[1],str[2],str[3]), lHeadLevel);*/
				m_HeadWear.SetVisible(true);
			}
		}else
			m_HeadWear.SetVisible(false);

		// 外套处理
		if (GetGame()->GetRegion() == NULL ||
			(GetGame()->GetRegion() != NULL && GetGame()->GetRegion()->IsChangeEquip()))
		{
			if (GetShowFashion() & FASHION_TYPE_GREATCOAT)
			{
				dwEquipIndex = GetEquipIndex(CPlayer::EQUIP_P_FROCK);
				pGoods2		 = CGoodsList::GetProperty(dwEquipIndex);
				if (pGoods2 != NULL)
				{
					dwBody	   = pGoods2->BaseProperty.dwEquipID;
					lBodyLevel = GetEquipLevel(CPlayer::EQUIP_P_FROCK);

					if (CGoodsList::GetEffectVal(dwEquipIndex, GAP_HIDE_HEAR, 1) != 0)
						m_pDisplayModel->HideGroup(dwHAIR);
					if (CGoodsList::GetEffectVal(dwEquipIndex, GAP_HIDE_FACE, 1) != 0)
						m_pDisplayModel->HideGroup(dwFACE);
					if (CGoodsList::GetEffectVal(dwEquipIndex, GAP_HIDE_BODY, 1) != 0)
					{
						m_pDisplayModel->HideGroup(dwARMO);
						m_pDisplayModel->HideGroup(dwHAND);
						m_pDisplayModel->HideGroup(dwFOOT);
						m_pDisplayModel->HideGroup(dwPIFE);
					}

					if (dwBody != 0)
					{
						sprintf(str, "%04d", dwBody);
						m_pDisplayModel->ShowGroup(dwBODY, MAKEFOURCC(str[0],str[1],str[2],str[3]), lBodyLevel);
					}
				}else
					m_pDisplayModel->HideGroup(dwBODY);
			}else
				m_pDisplayModel->HideGroup(dwBODY);
		}else if (GetGame()->GetRegion() != NULL && !GetGame()->GetRegion()->IsChangeEquip())
		{
			dwEquipIndex = GetEquipIndex(CPlayer::EQUIP_P_FROCK);
			pGoods2		 = CGoodsList::GetProperty(dwEquipIndex);
			if (pGoods2 != NULL)
			{
				dwBody	   = pGoods2->BaseProperty.dwEquipID;
				lBodyLevel = GetEquipLevel(CPlayer::EQUIP_P_FROCK);

				if (CGoodsList::GetEffectVal(dwEquipIndex, GAP_HIDE_HEAR, 1) != 0)
					m_pDisplayModel->HideGroup(dwHAIR);
				if (CGoodsList::GetEffectVal(dwEquipIndex, GAP_HIDE_FACE, 1) != 0)
					m_pDisplayModel->HideGroup(dwFACE);
				if (CGoodsList::GetEffectVal(dwEquipIndex, GAP_HIDE_BODY, 1) != 0)
				{
					m_pDisplayModel->HideGroup(dwARMO);
					m_pDisplayModel->HideGroup(dwHAND);
					m_pDisplayModel->HideGroup(dwFOOT);
					m_pDisplayModel->HideGroup(dwPIFE);
				}

				if (dwBody != 0)
				{
					sprintf(str, "%04d", dwBody);
					m_pDisplayModel->ShowGroup(dwBODY, MAKEFOURCC(str[0],str[1],str[2],str[3]), lBodyLevel);
				}
			}else
				m_pDisplayModel->HideGroup(dwBODY);
		}
	}
}



/*
*	功能：拷贝指定对象的所有装备信息
*	细节描述： 
*	参数：  pShape - 复制的参照对象
*	返回值： 无
*	作者: 刘科
*	创建日期: 08.8.28
*	修改日志: 
*			刘科 - 09.11.10	将本接口放在CShape下，这里重写接口，并将参数改变成CShape
*/
void CPlayer::CopyAllEquip(CShape *pShape)
{
	CPlayer * pPlayer = dynamic_cast<CPlayer *>(pShape);
	if(pPlayer)
	{

		SetShowFashion(pPlayer->GetShowFashion());
		// 复制装备
		SetHairPic(pPlayer->GetHairPic());
		SetFacePic(pPlayer->GetFacePic());
		DWORD dwIndex = 0;
		/***********************************************************************/
		/*  zhaohang 2010-11-16 fix bylev 由byte改为DWORD 因此
		pPlayer->GetEquip前的byte全部去掉*/
		/***********************************************************************/
		DWORD byLev = 0;

		bool bIsMainPlayer = (pPlayer == GetGame()->GetMainPlayer());
		//	0：头盔
		if ( bIsMainPlayer )
		{
			if(pPlayer->GetEquip(CPlayer::EQUIP_HEAD))
			{
				dwIndex = pPlayer->GetEquip(CPlayer::EQUIP_HEAD)->GetIndex();
				byLev = pPlayer->GetEquip(CPlayer::EQUIP_HEAD)->GetEffectVal(GAP_WEAPON_LEVEL,1);
			}
		}
		else
		{
			dwIndex = pPlayer->GetEquipIndex(EQUIP_HEAD);
			byLev = pPlayer->GetEquipLevel(EQUIP_HEAD);
		}
		SetEquipIndex(CPlayer::EQUIP_HEAD, dwIndex);
		SetEquipLevel(CPlayer::EQUIP_HEAD, byLev);

		//	1：项链
		dwIndex = 0;
		byLev = 0;
		if ( bIsMainPlayer )
		{
			if(pPlayer->GetEquip(CPlayer::EQUIP_NECKLACE))
			{
				dwIndex = pPlayer->GetEquip(CPlayer::EQUIP_NECKLACE)->GetIndex();
				byLev = pPlayer->GetEquip(CPlayer::EQUIP_NECKLACE)->GetEffectVal(GAP_WEAPON_LEVEL,1);
			}
		}
		else
		{
			dwIndex = pPlayer->GetEquipIndex(EQUIP_NECKLACE);
			byLev = pPlayer->GetEquipLevel(EQUIP_NECKLACE);
		}
		SetEquipIndex(CPlayer::EQUIP_NECKLACE, dwIndex);
		SetEquipLevel(CPlayer::EQUIP_NECKLACE, byLev);

		//	2：翅膀
		dwIndex = 0;
		byLev = 0;
		if ( bIsMainPlayer )
		{
			if(pPlayer->GetEquip(CPlayer::EQUIP_WING))
			{
				dwIndex = pPlayer->GetEquip(CPlayer::EQUIP_WING)->GetIndex();
				byLev = pPlayer->GetEquip(CPlayer::EQUIP_WING)->GetEffectVal(GAP_WEAPON_LEVEL,1);
			}
		}
		else
		{
			dwIndex = pPlayer->GetEquipIndex(EQUIP_WING);
			byLev = pPlayer->GetEquipLevel(EQUIP_WING);
		}
		if (dwIndex != GetEquipIndex(CPlayer::EQUIP_WING) || byLev != GetEquipLevel(EQUIP_WING))
		{
			RemoveWeaponShow(EQUIP_WING);
			SetEquipIndex(CPlayer::EQUIP_WING, dwIndex);
			SetEquipLevel(CPlayer::EQUIP_WING, byLev);
			UpdateWeaponShow(EQUIP_WING);
		}

        //	3：盔甲
        dwIndex = 0;
        byLev = 0;
        if ( bIsMainPlayer )
        {
            if(pPlayer->GetEquip(CPlayer::EQUIP_BODY))
            {
                dwIndex = pPlayer->GetEquip(CPlayer::EQUIP_BODY)->GetIndex();
                byLev = pPlayer->GetEquip(CPlayer::EQUIP_BODY)->GetEffectVal(GAP_WEAPON_LEVEL,1);
            }
        }
        else
        {
            dwIndex = pPlayer->GetEquipIndex(EQUIP_BODY);
            byLev = pPlayer->GetEquipLevel(EQUIP_BODY);
        }
        SetEquipIndex(CPlayer::EQUIP_BODY, dwIndex);
        SetEquipLevel(CPlayer::EQUIP_BODY, byLev);
		

		//	4：腰带
		dwIndex = 0;
		byLev = 0;
		if ( bIsMainPlayer )
		{
			if(pPlayer->GetEquip(CPlayer::EQUIP_BACK))
			{
				dwIndex = pPlayer->GetEquip(CPlayer::EQUIP_BACK)->GetIndex();
				byLev = pPlayer->GetEquip(CPlayer::EQUIP_BACK)->GetEffectVal(GAP_WEAPON_LEVEL,1);
			}
		}
		else
		{
			dwIndex = pPlayer->GetEquipIndex(EQUIP_BACK);
			byLev = pPlayer->GetEquipLevel(EQUIP_BACK);
		}
		SetEquipIndex(CPlayer::EQUIP_BACK, dwIndex);
		SetEquipLevel(CPlayer::EQUIP_BACK, byLev);

		//	5：手套	
		dwIndex = 0;
		byLev = 0;
		if ( bIsMainPlayer )
		{
			if(pPlayer->GetEquip(CPlayer::EQUIP_HAND))
			{
				dwIndex = pPlayer->GetEquip(CPlayer::EQUIP_HAND)->GetIndex();
				byLev = pPlayer->GetEquip(CPlayer::EQUIP_HAND)->GetEffectVal(GAP_WEAPON_LEVEL,1);

			}
		}
		else
		{
			dwIndex = pPlayer->GetEquipIndex(EQUIP_HAND);
			byLev = pPlayer->GetEquipLevel(EQUIP_HAND);
		}
		SetEquipIndex(CPlayer::EQUIP_HAND, dwIndex);
		SetEquipLevel(CPlayer::EQUIP_HAND, byLev);

		//	6：鞋子
		dwIndex = 0;
		byLev = 0;
		if ( bIsMainPlayer )
		{
			if(pPlayer->GetEquip(CPlayer::EQUIP_BOOT))
			{
				dwIndex = pPlayer->GetEquip(CPlayer::EQUIP_BOOT)->GetIndex();
				byLev = pPlayer->GetEquip(CPlayer::EQUIP_BOOT)->GetEffectVal(GAP_WEAPON_LEVEL,1);
			}
		}
		else
		{
			dwIndex = pPlayer->GetEquipIndex(EQUIP_BOOT);
			byLev = pPlayer->GetEquipLevel(EQUIP_BOOT);
		}
		SetEquipIndex(CPlayer::EQUIP_BOOT, dwIndex);
		SetEquipLevel(CPlayer::EQUIP_BOOT, byLev);

		//	7：头饰
		dwIndex = 0;
		byLev = 0;
		if ( bIsMainPlayer )
		{
			if(pPlayer->GetEquip(CPlayer::EQUIP_SHOULDER))
			{
				dwIndex = pPlayer->GetEquip(CPlayer::EQUIP_SHOULDER)->GetIndex();
				byLev = pPlayer->GetEquip(CPlayer::EQUIP_SHOULDER)->GetEffectVal(GAP_WEAPON_LEVEL,1);
			}
		}
		else
		{
			dwIndex = pPlayer->GetEquipIndex(EQUIP_SHOULDER);
			byLev = pPlayer->GetEquipLevel(EQUIP_SHOULDER);
		}
		if (dwIndex != GetEquipIndex(CPlayer::EQUIP_SHOULDER) || byLev != GetEquipLevel(EQUIP_SHOULDER))
		{
			RemoveWeaponShow(EQUIP_SHOULDER);
			SetEquipIndex(CPlayer::EQUIP_SHOULDER, dwIndex);
			SetEquipLevel(CPlayer::EQUIP_SHOULDER, byLev);
			UpdateWeaponShow(EQUIP_SHOULDER);
		}

        dwIndex = 0;
        byLev = 0;
        if ( bIsMainPlayer )
        {
            if(pPlayer->GetEquip(CPlayer::EQUIP_WRIST))
            {
                dwIndex = pPlayer->GetEquip(CPlayer::EQUIP_WRIST)->GetIndex();
                byLev = pPlayer->GetEquip(CPlayer::EQUIP_WRIST)->GetEffectVal(GAP_WEAPON_LEVEL,1);
            }
        }
        else
        {
            dwIndex = pPlayer->GetEquipIndex(EQUIP_WRIST);
            byLev = pPlayer->GetEquipLevel(EQUIP_WRIST);
        }
        SetEquipIndex(CPlayer::EQUIP_WRIST, dwIndex);
        SetEquipLevel(CPlayer::EQUIP_WRIST, byLev);

        dwIndex = 0;
        byLev = 0;
        if ( bIsMainPlayer )
        {
            if(pPlayer->GetEquip(CPlayer::EQUIP_LEG))
            {
                dwIndex = pPlayer->GetEquip(CPlayer::EQUIP_LEG)->GetIndex();
                byLev = pPlayer->GetEquip(CPlayer::EQUIP_LEG)->GetEffectVal(GAP_WEAPON_LEVEL,1);
            }
        }
        else
        {
            dwIndex = pPlayer->GetEquipIndex(EQUIP_LEG);
            byLev = pPlayer->GetEquipLevel(EQUIP_LEG);
        }
        SetEquipIndex(CPlayer::EQUIP_LEG, dwIndex);
        SetEquipLevel(CPlayer::EQUIP_LEG, byLev);

        dwIndex = 0;
        byLev = 0;
        if ( bIsMainPlayer )
        {
            if(pPlayer->GetEquip(CPlayer::EQUIP_BOOT))
            {
                dwIndex = pPlayer->GetEquip(CPlayer::EQUIP_BOOT)->GetIndex();
                byLev = pPlayer->GetEquip(CPlayer::EQUIP_BOOT)->GetEffectVal(GAP_WEAPON_LEVEL,1);
            }
        }
        else
        {
            dwIndex = pPlayer->GetEquipIndex(EQUIP_BOOT);
            byLev = pPlayer->GetEquipLevel(EQUIP_BOOT);
        }
        SetEquipIndex(CPlayer::EQUIP_BOOT, dwIndex);
        SetEquipLevel(CPlayer::EQUIP_BOOT, byLev);

        //	9：左戒指
        dwIndex = 0;
        byLev = 0;
        if ( bIsMainPlayer )
        {
            if(pPlayer->GetEquip(CPlayer::EQUIP_LRING))
            {
                dwIndex = pPlayer->GetEquip(CPlayer::EQUIP_LRING)->GetIndex();
                byLev = pPlayer->GetEquip(CPlayer::EQUIP_LRING)->GetEffectVal(GAP_WEAPON_LEVEL,1);
            }
        }
        else
        {
            dwIndex = pPlayer->GetEquipIndex(EQUIP_LRING);
            byLev = pPlayer->GetEquipLevel(EQUIP_LRING);
        }
        SetEquipIndex(CPlayer::EQUIP_LRING, dwIndex);
        SetEquipLevel(CPlayer::EQUIP_LRING, byLev);

        //	10：右戒指
        dwIndex = 0;
        byLev = 0;
        if ( bIsMainPlayer )
        {
            if(pPlayer->GetEquip(CPlayer::EQUIP_RRING))
            {
                dwIndex = pPlayer->GetEquip(CPlayer::EQUIP_RRING)->GetIndex();
                byLev = pPlayer->GetEquip(CPlayer::EQUIP_RRING)->GetEffectVal(GAP_WEAPON_LEVEL,1);
            }
        }
        else
        {
            dwIndex = pPlayer->GetEquipIndex(EQUIP_RRING);
            byLev = pPlayer->GetEquipLevel(EQUIP_RRING);
        }
        SetEquipIndex(CPlayer::EQUIP_RRING, dwIndex);
        SetEquipLevel(CPlayer::EQUIP_RRING, byLev);

        //	15：武器
        dwIndex = 0;
        byLev = 0;
        if ( bIsMainPlayer )
        {
            if(pPlayer->GetEquip(CPlayer::EQUIP_WEAPON))
            {
                dwIndex = pPlayer->GetEquip(CPlayer::EQUIP_WEAPON)->GetIndex();
                byLev = pPlayer->GetEquip(CPlayer::EQUIP_WEAPON)->GetEffectVal(GAP_WEAPON_LEVEL,1);
            }
        }
        else
        {
            dwIndex = pPlayer->GetEquipIndex(EQUIP_WEAPON);
            byLev = pPlayer->GetEquipLevel(EQUIP_WEAPON);
        }
        if (dwIndex != GetEquipIndex(CPlayer::EQUIP_WEAPON) || byLev != GetEquipLevel(EQUIP_WEAPON))
        {
            RemoveWeaponShow(EQUIP_WEAPON);
            SetEquipIndex(CPlayer::EQUIP_WEAPON, dwIndex);
            SetEquipLevel(CPlayer::EQUIP_WEAPON, byLev);
            UpdateWeaponShow(EQUIP_WEAPON);
        }

        dwIndex = 0;
        byLev = 0;
        if ( bIsMainPlayer )
        {
            if(pPlayer->GetEquip(CPlayer::EQUIP_CARD))
            {
                dwIndex = pPlayer->GetEquip(CPlayer::EQUIP_CARD)->GetIndex();
                byLev = pPlayer->GetEquip(CPlayer::EQUIP_CARD)->GetEffectVal(GAP_WEAPON_LEVEL,1);
            }
        }
        else
        {
            dwIndex = pPlayer->GetEquipIndex(EQUIP_CARD);
            byLev = pPlayer->GetEquipLevel(EQUIP_CARD);
        }
        if (dwIndex != GetEquipIndex(CPlayer::EQUIP_CARD) || byLev != GetEquipLevel(EQUIP_CARD))
        {
            RemoveWeaponShow(EQUIP_CARD);
            SetEquipIndex(CPlayer::EQUIP_CARD, dwIndex);
            SetEquipLevel(CPlayer::EQUIP_CARD, byLev);
            UpdateWeaponShow(EQUIP_CARD);
        }

        //	17：披风
        dwIndex = 0;
        byLev = 0;
        if ( bIsMainPlayer )
        {
            if(pPlayer->GetEquip(CPlayer::EQUIP_GODGOODS))
            {
                dwIndex = pPlayer->GetEquip(CPlayer::EQUIP_GODGOODS)->GetIndex();
                byLev = pPlayer->GetEquip(CPlayer::EQUIP_GODGOODS)->GetEffectVal(GAP_WEAPON_LEVEL,1);
            }
        }
        else
        {
            dwIndex = pPlayer->GetEquipIndex(EQUIP_GODGOODS);
            byLev = pPlayer->GetEquipLevel(EQUIP_GODGOODS);
        }
        SetEquipIndex(CPlayer::EQUIP_GODGOODS, dwIndex);
        SetEquipLevel(CPlayer::EQUIP_GODGOODS, byLev);


		//	8：外套
		dwIndex = 0;
		byLev = 0;
		if ( bIsMainPlayer )
		{
			if(pPlayer->GetEquip(CPlayer::EQUIP_EFFIGY))
			{
				dwIndex = pPlayer->GetEquip(CPlayer::EQUIP_EFFIGY)->GetIndex();
				byLev = pPlayer->GetEquip(CPlayer::EQUIP_EFFIGY)->GetEffectVal(GAP_WEAPON_LEVEL,1);
			}
		}
		else
		{
			dwIndex = pPlayer->GetEquipIndex(EQUIP_EFFIGY);
			byLev = pPlayer->GetEquipLevel(EQUIP_EFFIGY);
		}
		SetEquipIndex(CPlayer::EQUIP_EFFIGY, dwIndex);
		SetEquipLevel(CPlayer::EQUIP_EFFIGY, byLev);

        dwIndex = 0;
        byLev = 0;
        if ( bIsMainPlayer )
        {
            if(pPlayer->GetEquip(CPlayer::EQUIP_LIFECOWRY))
            {
                dwIndex = pPlayer->GetEquip(CPlayer::EQUIP_LIFECOWRY)->GetIndex();
                byLev = pPlayer->GetEquip(CPlayer::EQUIP_LIFECOWRY)->GetEffectVal(GAP_WEAPON_LEVEL,1);
            }
        }
        else
        {
            dwIndex = pPlayer->GetEquipIndex(EQUIP_LIFECOWRY);
            byLev = pPlayer->GetEquipLevel(EQUIP_LIFECOWRY);
        }
        SetEquipIndex(CPlayer::EQUIP_LIFECOWRY, dwIndex);
        SetEquipLevel(CPlayer::EQUIP_LIFECOWRY, byLev);

		dwIndex = 0;
		byLev = 0;
		if ( bIsMainPlayer )
		{
			if(pPlayer->GetEquip(CPlayer::EQUIP_SOULGUARD))
			{
				dwIndex = pPlayer->GetEquip(CPlayer::EQUIP_SOULGUARD)->GetIndex();
				byLev = (BYTE)pPlayer->GetEquip(CPlayer::EQUIP_SOULGUARD)->GetEffectVal(GAP_WEAPON_LEVEL,1);
			}
		}
		else
		{
			dwIndex = pPlayer->GetEquipIndex(EQUIP_SOULGUARD);
			byLev = pPlayer->GetEquipLevel(EQUIP_SOULGUARD);
		}
		SetEquipIndex(CPlayer::EQUIP_SOULGUARD, dwIndex);
		SetEquipLevel(CPlayer::EQUIP_SOULGUARD, byLev);


		SetDisplayModelGroup(false);
	}
}

// 查看指定的职业是否存在于玩家的职业链之中
bool CPlayer::IsInOccupedList(DWORD dwOccupID)
{
	for (int i = 0; i < (int)m_listOccuped.size(); ++i)
	{
		if(dwOccupID == m_listOccuped[i])
			return true;
	}
	return false;
}

// 指定的技能是否能触使职业链的变化
bool CPlayer::IsAddOccupedList(DWORD dwSkillID)
{
	bool bRt = false;
	DWORD dwOcc = OCC_Max;
	switch(dwSkillID)
	{
	case SKILL_BASE_NOVICE_OCC:				// 初新者
		dwOcc = OCC_Novice;
		bRt = true;
		break;
	case SKILL_BASE_FIGHTER_OCC:				// 战士
		dwOcc = OCC_Fighter;
		bRt = true;
		break;
	case SKILL_BASE_RABBI_OCC:					// 法师
		dwOcc = OCC_Mystic;
		bRt = true;
		break;
	case SKILL_BASE_FAQUIR_OCC:					// 行者
		dwOcc = OCC_Rogue;
		bRt = true;
		break;
	case SKILL_BASE_KNIGHT_OCC:					// 骑士
		dwOcc = OCC_Knight;
		bRt = true;
		break;
	case SKILL_BASE_WARRIOR_OCC:				// 斗士
		dwOcc = OCC_Warrior;
		bRt = true;
		break;
	case SKILL_BASE_SCOUT_OCC:					// 斥候
		dwOcc = OCC_Scout;
		bRt = true;
		break;
	case SKILL_BASE_CRAFTSMAN_OCC:				// 艺人
		dwOcc = OCC_Craftsman;
		bRt = true;
		break;
	case SKILL_BASE_WARLOCK_OCC:				//术士
		dwOcc = OCC_Warlock;
		bRt = true;
		break;
	case SKILL_BASE_CLERIC_OCC:					//牧师
		dwOcc = OCC_Cleric;
		bRt = true;
		break;
	case SKILL_BASE_PALADIN_OCC:				//圣骑士
		dwOcc = OCC_Paladin;
		bRt = true;
		break;
	case SKILL_BASE_DARKKINGHT_OCC:				//暗骑士
		dwOcc = OCC_DarkKnight;
		bRt = true;
		break;
	case SKILL_BASE_GLADIATOR_OCC:				//角斗士
		dwOcc = OCC_Gladiator;
		bRt = true;
		break;
	case SKILL_BASE_BERSERKER_OCC:				//狂战士
		dwOcc = OCC_Berserker;
		bRt = true;
		break;
	case SKILL_BASE_RANGER_OCC:					//巡守
		dwOcc = OCC_Ranger;
		bRt = true;
		break;
	case SKILL_BASE_ASSASSIN_OCC:				//刺客
		dwOcc = OCC_Assassin;
		bRt = true;
		break;
	case SKILL_BASE_BLADEDANCER_OCC:			//剑舞者
		dwOcc = OCC_BladeDancer;
		bRt = true;
		break;
	case SKILL_BASE_BARD_OCC:					//吟游诗人
		dwOcc = OCC_Bard;
		bRt = true;
		break;
	case SKILL_BASE_SORCERER_OCC:				//巫师
		dwOcc = OCC_Sorcerer;
		bRt = true;
		break;
	case SKILL_BASE_NECROMANCER_OCC:			//亡灵巫师
		dwOcc = OCC_Necromancer;
		bRt = true;
		break;
	case SKILL_BASE_BISHOP_OCC:					//主教
		dwOcc = OCC_Bishop;
		bRt = true;
		break;
	case SKILL_BASE_PROPHET_OCC:				//贤者(先知)
		dwOcc = OCC_Prophet;
		bRt = true;
		break;
	}
	// 职业合法
	if(dwOcc >= OCC_Novice && dwOcc < OCC_Max)
		m_listOccuped.push_back(dwOcc);
	return bRt;
}

//根据当前的称号类型返回称号
string CPlayer::GetPlayerTitle()
{
	if(GetTitleType()==ePTT_Faction)
	{
		if(GetFactionID() == NULL_GUID)
		{
			string strFacTitle("");
			return strFacTitle;
		}else{
			return GetFactionTitle();
		}
	}else{
		return m_pPlayerTitle;
	}
}

CPet* CPlayer::GetActivePet() const
{
    map<CGUID, CPet*>::const_iterator it = m_PetList.find(m_ActivePetGUID);
    if (it == m_PetList.end())
        return NULL;
    return it->second;
}

void CPlayer::SetCurOperPetGUID(const CGUID& guid)
{
	m_CurOperPetGUID = guid;
	CMessage msg(MSG_C2S_PET_SET_FOCUS);
	msg.Add(m_CurOperPetGUID);
	msg.Send();
}

/// 创建宠物
CPet* CPlayer::CreatePet(const CGUID& guid)
{
	if( NULL_GUID == guid )
		return NULL;

	CPet* pet = new CPet;
	pet->SetHost(this);
	pet->SetExID(guid);
	m_PetList[guid] = pet;
	return pet;
}

/// 解码宠物信息
void CPlayer::DecodePetFromDataBlock(DBReadSet& setReadDB, bool bExData)
{
	if (bExData)
	{
		RemoveAllPet();
		long lCount = setReadDB.GetLongFromByteArray();	//获得所有宠物的数量
		if (lCount == 0)
			return ;

		for(int i=0;i<lCount;i++)
		{
			long lType = setReadDB.GetLongFromByteArray();
			if (lType == 0)
			{
				continue;
			}

			CGUID guid;
			setReadDB.GetBufferFromByteArray(guid);
			long lGrahpicsID = setReadDB.GetLongFromByteArray();
			char strName[32];
			setReadDB.GetStringFromByteArray(strName, 32);
			if (strcmp(strName, "") == 0)
				ASSERT(0);

			CPet* pet = CreatePet(guid);
			if (pet!=NULL)
			{
				pet->DecodeAttrFromDataBlock(setReadDB);

				if (PET_STATE_ACTIVE == pet->GetPetState())
				{
					SetActivePetGUID(pet->GetExID());
				}
			}
		}
	}
}

/// 取得出战状态宠物数目
long CPlayer::GetFightPetNum()
{
	long lNum = 0;
	map<CGUID, CPet*>::iterator PetIter = m_PetList.begin();
	for (; PetIter != m_PetList.end(); ++PetIter)
	{
		CPet* pPet = PetIter->second;
		if (pPet->IsActive())
		{
			lNum++;
		}
	}
	return lNum;
}
/// 添加宠物
void CPlayer::AddPet(CPet *pPet)
{
	if(pPet)
	{
		m_PetList[pPet->GetExID()] = pPet;
	}
	else
		ASSERT(0);
}

void CPlayer::SetPetFighting(CPet *pPet)
{
	if(!pPet)
	{
		ASSERT(0);
		return;
	}

	long lPetType = pPet->GetPetType();
	map<CGUID, CPet*>::iterator PetIter = m_PetList.begin();
	for (; PetIter!=m_PetList.end(); ++PetIter)
	{
		CPet* pOrigin = PetIter->second;
		if(pOrigin->GetPetType() == lPetType && pOrigin->GetExID() != pPet->GetExID() && pOrigin->IsActive())
		{
			pOrigin->SetPetIsActive(false);
			break;
		}
	}
	//设置宠物激活
	pPet->SetPetIsActive(true);
}

void CPlayer::PetEnterRegion()
{
	if(m_PetList.empty())
		return;

	map<CGUID, CPet*>::iterator PetIter = m_PetList.begin();
	for (; PetIter!=m_PetList.end(); ++PetIter)
	{
		CPet* pet = PetIter->second;
		if (pet && pet->IsActive())
		{
			pet->EnterRegion(this == GetGame()->GetMainPlayer());
		}
	}
}
/// 根据宠物GUID取得宠物
CPet* CPlayer::GetPet(const CGUID& guid)
{
	CPet* pet = NULL;

	map<CGUID, CPet*>::iterator PetIter = m_PetList.find(guid);
	if (PetIter!=m_PetList.end())
	{
		pet = PetIter->second;
		if (guid == pet->GetExID())
		{
			return pet;
		}
	}
	return NULL;
}

/// 删除宠物
void CPlayer::DelPet(const CGUID& guid)
{
	if (m_PetList.empty())
		return;

	map<CGUID, CPet*>::iterator PetIter = m_PetList.find(guid);
	if (PetIter!=m_PetList.end())
	{
		CPet* pPet = PetIter->second;
		if (guid == pPet->GetExID())
		{
			m_PetList.erase(PetIter);
			SAFE_DELETE(pPet);
		}
	}
}

/// 展示宠物
void CPlayer::ShowPet(const CGUID& guid)
{
	m_ShowPets.insert(guid);
}
/// 收回宠物
void CPlayer::UnShowPet(const CGUID& guid)
{
	m_ShowPets.erase(m_ShowPets.find(guid));
}

void CPlayer::RemoveAllPet()
{
	//释放宠物
	map<CGUID, CPet*>::iterator PetIter = m_PetList.begin();
	for (; PetIter!=m_PetList.end(); ++PetIter)
	{
        CPet *pPet = PetIter->second;
        if(pPet->IsActive())    //激活的宠物已交场景管理，这里就不释放了
            continue;
		SAFE_DELETE(pPet);
	}
	m_PetList.clear();
	//MessageBox(g_hWnd,"你所有的宠物被清除！","Info",MB_OK);
}

void CPlayer::ValidatePetInterface()
{

}

void CPlayer::SavePetSkillSetting()
{
	if (this == GetGame()->GetMainPlayer())
	{
		struct tagSkillAlter 
		{
			long lpos;
			long lActPos;
			bool bAuto;
		};

		long lPetActivedSize = GetFightPetNum();
		if (lPetActivedSize == 0)
			return;
		tagSkillAlter	petSkill;
		CMessage	msg(MSG_C2S_SKILL_SAVE);
		msg.Add(lPetActivedSize);						//曾经被激活过的宠物数目

		map<CGUID,CPet*>::iterator PetIter = m_PetList.begin();
		for (; PetIter!=m_PetList.end(); ++PetIter)
		{
			CPet* pPet = PetIter->second;
			//陷阱和饰宠 不保存
			//if (pPet->GetPetType() == PET_TYPE_TRAP || pPet->GetPetType() == PET_TYPE_ORNA)
			//	continue;
			if (pPet->IsPetSkillChanged())
			{
				msg.Add((long)pPet->GetPetType());		//宠物类型
				msg.Add(pPet->GetExID());				//宠物guid
				long lSkillCount = pPet->GetPetSkillCount();

				vector<tagSkillAlter>	vecPetSkillAlter;
				for (long lPos = 0;lPos < lSkillCount;++lPos)
				{
					tagPetSkillInfo *pSkill = pPet->GetPetSkillInfoByPos(lPos);

					if(pSkill && pSkill->bSkillIsChanged)
					{
						petSkill.lpos	 = lPos;				
						petSkill.lActPos = pSkill->lActPos;		
						petSkill.bAuto	 = pSkill->bAuto;		
						vecPetSkillAlter.push_back(petSkill);
					}
				}
				long lChangeSize = (long)vecPetSkillAlter.size();		//被改变技能的个数
				msg.Add(lChangeSize);
				for (long i = 0; i<lChangeSize; ++i)
				{
					msg.Add(vecPetSkillAlter[i].lpos);			//技能位置
					msg.Add(vecPetSkillAlter[i].lActPos);		//技能动作条位置
					msg.Add((BYTE)vecPetSkillAlter[i].bAuto);	//是否自动释放
				}
			}
			else
			{
				msg.Add((long)0);		//宠物类型
				continue;
			}
		}
		msg.Send();
	}
}


/*
* 功能: 添加经验提示文本
* 摘要: -
* 参数: eType	 - 提示文本类型
*		 lNumber - 需要显示的经验数值
* 返回值: -
* 作者: lpf
* 创建日期: 2009.02.05
* 修改日志:
*	2008.02.24 - lpf
*		在函数开始位置增加了对层级上限的错误判断
*	2009.04.08 - lpf
*		增加了提示文本类型参数
*/
void CPlayer::AddTipText(TIPTEXT_TYPE eType, long lNumber)
{
	if (lNumber <= 0 || m_dwLevelMax == 0)
		return;

	char szTmp[50];
	sprintf(szTmp, "%d", lNumber);

	DWORD dwLevel = 0;
	while (dwLevel < m_dwLevelMax - 1)
	{
		if (m_vecTipText[dwLevel].fAlpha <= 0.0f)
			break;

		dwLevel++;
	}

	tagTipText stTipText;
	stTipText.eType = eType;
	stTipText.sNumber = szTmp;
	stTipText.fAlpha = 1.0f;					//透明度
	stTipText.dwStartTime = timeGetTime();		//开始时间
	stTipText.vPos = D3DXVECTOR3(GetPosY(), GetPickBoxHeight() + 0.5f + GetHeight() + (float)dwLevel * CGlobeSetup::GetSetup()->fExpTipTextLineSpace, GetPosX());

	m_vecTipText[dwLevel] = stTipText;
}



/*
* 功能: 显示玩家的经验提示文本
* 摘要: -
* 参数: -
* 返回值: -
* 作者: lpf
* 创建日期: 2009.02.05
* 修改日志:
*	2009.04.08 - lpf
*		增加了提示文本类型处理
*/
void CPlayer::DisplayTipText()
{
	if (m_dwLevelCurrentDisplayNum == 0)
		return;

	DWORD dwColor = 0;
	DWORD dwColorBack;
	DWORD dwColorAlpha;
	DWORD dwDrawX = 0, dwDrawY = 0;
	float fNumberScale = CGlobeSetup::GetSetup()->fExpTipTextNumberScale;
	D3DXMATRIX mFinal;
	D3DXVECTOR3 vScreenPos;
	GetGame()->GetRegion()->CalculateFinalTransformMatrix(mFinal);

	const char * pszNumber = NULL;
	DWORD		 dwNumberLen = 0;
	CBitmapX   * pBitmap = NULL;

	for (DWORD dw = 0; dw < (DWORD)m_vecTipText.size(); ++dw)
	{
		if (m_vecTipText[dw].fAlpha <= 0.0f)
			continue;

		D3DXVec3TransformCoord(&vScreenPos, &m_vecTipText[dw].vPos, &mFinal);
		dwColorAlpha = (DWORD)(m_vecTipText[dw].fAlpha * 255.0f) << 24;

		// 判断提示文本类型
		if (m_vecTipText[dw].eType == TT_EXP)
		{
			dwColor		= dwColorAlpha | CGlobeSetup::GetSetup()->colorExpTipText;
			dwColorBack = dwColorAlpha | CGlobeSetup::GetSetup()->colorExpTipTextBack;
			pBitmap		= GetGame()->GetExpTip();
		}else
		{
			dwColor		= dwColorAlpha | CGlobeSetup::GetSetup()->colorConTipText;
			dwColorBack = dwColorAlpha | CGlobeSetup::GetSetup()->colorConTipTextBack;
			pBitmap		= GetGame()->GetConTip();
		}

		// 绘制"经验值"图片
		dwDrawX = (DWORD)vScreenPos.x - pBitmap->GetWidth();
		dwDrawY = (DWORD)vScreenPos.y - pBitmap->GetHeight() / 2;
		pBitmap->Draw(dwDrawX - 1, dwDrawY, dwColorBack);
		pBitmap->Draw(dwDrawX, dwDrawY - 1, dwColorBack);
		pBitmap->Draw(dwDrawX + 1, dwDrawY, dwColorBack);
		pBitmap->Draw(dwDrawX, dwDrawY + 1, dwColorBack);
		pBitmap->Draw(dwDrawX, dwDrawY, dwColor);

		// 绘制数字
		dwDrawX = (DWORD)vScreenPos.x;
		dwDrawY = (DWORD)vScreenPos.y - (DWORD)(((float)GetGame()->GetNumberHeight() * fNumberScale) * 0.5f);
		pszNumber = m_vecTipText[dw].sNumber.c_str();
		dwNumberLen = (DWORD)m_vecTipText[dw].sNumber.length();
		GetGame()->NumberOut(dwDrawX - 1, dwDrawY, pszNumber, dwNumberLen, dwColorBack, fNumberScale);
		GetGame()->NumberOut(dwDrawX, dwDrawY - 1, pszNumber, dwNumberLen, dwColorBack, fNumberScale);
		GetGame()->NumberOut(dwDrawX + 1, dwDrawY, pszNumber, dwNumberLen, dwColorBack, fNumberScale);
		GetGame()->NumberOut(dwDrawX, dwDrawY + 1, pszNumber, dwNumberLen, dwColorBack, fNumberScale);
		GetGame()->NumberOut(dwDrawX, dwDrawY, pszNumber, dwNumberLen, dwColor, fNumberScale);
	}
}

//解析角色的勋章列表
void CPlayer::DecodeMedalFromDataBlockAndInit(DBReadSet& setReadDB)
{
	//清空原有的数据
	if(m_VecMedalOnPlayer.size()>0)
	{
		m_VecMedalOnPlayer.clear();
	}
	//m_iDisplayMode = 0;
	LONG lGoodsNum = setReadDB.GetLongFromByteArray();
	for(int i=0;i<lGoodsNum;i++)
	{
		DWORD MedalIndex = setReadDB.GetLongFromByteArray();
		m_VecMedalOnPlayer.push_back(MedalIndex);
	}
	return;
}

//在玩家身上添加一个勋章
void CPlayer::AddMedal(DWORD Index)
{
	m_VecMedalOnPlayer.push_back(Index);
}
//在玩家身上删除一个勋章
void CPlayer::DeleteMedal(DWORD Index)
{
	if(m_VecMedalOnPlayer.size()>0&&FindMedalOnPlayer(Index))   //判断当前勋章列表不为0且身上有该勋章
	{
		int i=0;
		vector<DWORD>  m_VecMedalOnPlayerTemp; //建立一个临时数组用于存储变化后的数据
		for(vector<DWORD>::iterator it=m_VecMedalOnPlayer.begin();it!=m_VecMedalOnPlayer.end();it++)
		{
			if(m_VecMedalOnPlayer[i]!=Index)
			{
				m_VecMedalOnPlayerTemp.push_back(m_VecMedalOnPlayer[i]);
			}
			i++;
		}
		m_VecMedalOnPlayer.clear();    //清除原来的勋章数据
		int j=0;
		for(vector<DWORD>::iterator it=m_VecMedalOnPlayerTemp.begin();it!=m_VecMedalOnPlayerTemp.end();it++)
		{
			m_VecMedalOnPlayer.push_back(m_VecMedalOnPlayerTemp[j]);
			j++;
		}
	}
}

//查找玩家是否有该编号的物品
bool CPlayer::FindMedalOnPlayer(DWORD Index)
{
	int i=0;
	for(vector<DWORD>::iterator it=m_VecMedalOnPlayer.begin();it!=m_VecMedalOnPlayer.end();it++)
	{
		if(m_VecMedalOnPlayer[i]==Index)
		{
			return true;
		}
		i++;
	}
	return false;
}


WORD CPlayer::GetOccuLvlCoe(eOccupation byOccu,WORD wOccuLevel)
{
	WORD wRet = 0;
	switch(byOccu)
	{
		//初心者
	case OCC_Novice:
		wRet = 0;
		break;
		//战士、法师、行者
	case OCC_Fighter:
	case OCC_Mystic:
	case OCC_Rogue:
		wRet = 9;
		break;
		//骑士、斗士、斥侯、艺人、术士、牧师
	case OCC_Knight:
	case OCC_Warrior:
	case OCC_Scout:
	case OCC_Craftsman:
	case OCC_Warlock:
	case OCC_Cleric:
		wRet = 39;
		break;
		//圣骑士、暗骑士、剑士、狂战士、巡守、刺客、剑舞者、吟游诗人、巫师、死灵法师、主教、贤者
	case OCC_Paladin:
	case OCC_DarkKnight:
	case OCC_Gladiator:
	case OCC_Berserker:
	case OCC_Ranger:
	case OCC_Assassin:
	case OCC_BladeDancer:
	case OCC_Bard:
	case OCC_Sorcerer:
	case OCC_Necromancer:
	case OCC_Bishop:
	case OCC_Prophet:
		wRet = 79;
		break;
	};
	return wRet;
}

/// 检查玩家周围指定范围内是否有别的玩家在开店
bool CPlayer::CheckIsAnyBodyShopOpenedOnRange(DWORD dwRangeSize)
{
	/// 步长数组（用于划分四个象限，即单步增长的范围）
	int iStepArray[4][2] = {1,1,1,-1,-1,1,-1,-1};

	for(int iStepNum = 0;iStepNum<4/*sizeof(iStepArray)*/;iStepNum++)
	{
		for (int iIncreaseX = 0;abs(iIncreaseX) <= (int)dwRangeSize; iIncreaseX+=iStepArray[iStepNum][0])
		{
			for (int iIncreaseY = 0;abs(iIncreaseY)<= (int)dwRangeSize;iIncreaseY+=iStepArray[iStepNum][1])
			{
				list<CShape *> lsFindShape;
				((CClientRegion*)GetFather())->GetAllShapeByType(lsFindShape,GetTileX()+iIncreaseX,GetTileY()+iIncreaseY,TYPE_PLAYER);
				if(!lsFindShape.empty())
				{
					list<CShape *>::iterator ShapeIter = lsFindShape.begin();
					for(;ShapeIter!=lsFindShape.end();ShapeIter++)
					{
						if ((*ShapeIter)->GetType()==TYPE_PLAYER&&((CPlayer*)(*ShapeIter))->IsPlayerShopOpened())
						{      
							return true;
						}
					}
				}
			}
		}
	}
	return false;
}
