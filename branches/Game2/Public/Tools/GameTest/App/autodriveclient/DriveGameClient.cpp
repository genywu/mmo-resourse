#include "stdafx.h"
#include "DriveGameClient.h"
#include "../../GameTest/resource.h"
#include "../../GameTest/DriveClientDlg.h"

#include "..\..\Utility\TestMsgType.h"
#include "..\..\Script\MapScriptSystem.h"
#include "..\..\Script\ScriptSys.h"
#include "..\GameClient.h"
#include "../../Script/MapScriptSystem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//默认构造
CDriveGameClient::CDriveGameClient(unsigned uID) 
: m_pDrivePlayer(NULL), 
m_pDriveRegion(NULL)


{
	SetDriveMainPlayer(CreateMainPlayer());
	SetDriveMainRegion(CreateMainRegion());
	SetMemServeruID(uID);
}

//调用自定义极其父类构造
CDriveGameClient::CDriveGameClient(Timer* pTimer, unsigned uID)
: CGameClient(pTimer),
m_pDrivePlayer(NULL), 
m_pDriveRegion(NULL)
{
	SetDriveMainPlayer(CreateMainPlayer());
	SetDriveMainRegion(CreateMainRegion());
	SetMemServeruID(uID);
}

CDriveGameClient::~CDriveGameClient()
{	
	//这个player放在后面的m_listObject进行释放
	m_pDriveRegion->RemoveObject(m_pDrivePlayer);
	SAFE_DELETE(m_pDriveRegion);
	SAFE_DELETE(m_pDrivePlayer);	
	//ReleaseMapFileMsg();
}


//设置主玩家
inline void CDriveGameClient::SetDriveMainPlayer(CPlayer *pPlayer)
{	
	if (pPlayer != NULL)
	{
		m_pDrivePlayer = pPlayer;
		this->SetMainPlayer(m_pDrivePlayer);				//用于保证沿用继承于父类函数正确性
	}
}

//设置主Region
inline void CDriveGameClient::SetDriveMainRegion(CClientRegion *pRegion)
{
	if (pRegion != NULL)
	{
		m_pDriveRegion = pRegion;
		this->SetClientRegion(m_pDriveRegion);				//用于保证沿用继承于父类函数正确性
	}
}

//初始化主玩家数据
void CDriveGameClient::InitPlayerData(CMessage *pMsg)
{		
	long lCode = pMsg->GetLong();
	long lFlag = pMsg->GetLong();
	if (lFlag)
	{
		CGUID PlayerID;
		pMsg->GetGUID(PlayerID);

		if (m_pDrivePlayer==NULL)
		{
			m_pDrivePlayer = CreateMainPlayer();
		}
		if (m_pDrivePlayer)
		{
			// 属性转换设置
			float fStr2MinAtk[33];
			long  lHitTime			= 0;
			WORD  wBaseRp_Lv1		= 0;
			WORD  wBaseRp_Lv2       = 0;
			WORD  wPkCountPerKill   = 0;
			pMsg->GetEx(fStr2MinAtk, sizeof(float)*33);
			lHitTime		= pMsg->GetLong();
			wBaseRp_Lv1		= pMsg->GetWord();
			wBaseRp_Lv2		= pMsg->GetWord();
			wPkCountPerKill = pMsg->GetWord();
			//////////////////////////////////////////世界范围内的属性///////////////////////////////////
			float fBasePriceRate = 0;
			float fTradeInRate	 = 0;
			float fRepairFactor  = 0;
			fBasePriceRate = pMsg->GetFloat();
			fTradeInRate   = pMsg->GetFloat();
			fRepairFactor  = pMsg->GetFloat();
			char szTalkCountryGoodsName[32] = " ";
			pMsg->GetStr(szTalkCountryGoodsName, 32);
			int	nTalkCountryGoodsNum = 0;
			DWORD dwTalkCountryMoney = 0;
			nTalkCountryGoodsNum = pMsg->GetLong();
			dwTalkCountryMoney   = pMsg->GetDWord();
			char szTalkWorldGoodsName[32] = " "; 
			pMsg->GetStr(szTalkWorldGoodsName, 32);
			int	nTalkWorldGoodsNum = 0;
			DWORD dwTalkWorldMoney = 0;
			nTalkWorldGoodsNum = pMsg->GetLong();
			dwTalkWorldMoney = pMsg->GetDWord();
			// 盟国
			//pMsg->GetStr(GetGame()->GetSetup()->szTalkCountryUnionGoodsName, 32);
			//GetGame()->GetSetup()->nTalkCountryUnionGoodsNum = 0;//pMsg->GetLong();
			//GetGame()->GetSetup()->dwTalkCountryUnionMoney = 0;//pMsg->GetDWord();
			/////////////////////////////////////////////////////////////////////////////////////////////
			pMsg->GetByte();//m_pDrivePlayer->SetEnemyPlayerLevelFirstQuotiety(pMsg->GetByte());
			pMsg->GetByte();//m_pDrivePlayer->SetEnemyPlayerLevelSecondQuotiety(pMsg->GetByte());

			extern long AREA_WIDTH;
			extern long AREA_HEIGHT;
			AREA_WIDTH = pMsg->GetLong();
			AREA_HEIGHT = pMsg->GetLong();

			if (m_pDrivePlayer->GetFather())
			{
				((CClientRegion*)m_pDrivePlayer->GetFather())->RemoveObject(m_pDrivePlayer);
			}
			for(int i=0;i<EQUIP_SUM;i++)
			{
//				m_pDrivePlayer->SetEquip(i,NULL);
			}

			//开始解人物数据db
			DBReadSet setReadDB;
			pMsg->GetDBReadSet(setReadDB);
			m_pDrivePlayer->DecordFromDataBlock(setReadDB);

			//解读地图
			InitGameRegionData(pMsg);	

			//	CDriveClientDlg * pDlg = CMapScriptSystem::getInstance()->GetTestDialog()->GetDriveDlg();
			/*	pDlg->SetEvent(TRUE);*/
		}
	}
}

//初始化主场景
void CDriveGameClient::InitGameRegionData(CMessage *pMsg)
{
	// 地图名字
	char strRegionName[256];
	pMsg->GetStr(strRegionName,256);
	long lTaxRate = pMsg->GetLong();							//得到税率
	REGION_TYPE eWarRegionType = (REGION_TYPE)pMsg->GetByte();//得到战斗场景类型
	BYTE eSpaceType=pMsg->GetByte();//是否是水下场景
	bool bChangeEquip = pMsg->GetByte()==0?false:true;
	CGUID RegionID;
	pMsg->GetGUID(RegionID);
	long lRegionID = pMsg->GetLong();
	long lResourceID = pMsg->GetLong();
	float fExpScale = pMsg->GetFloat();


	// 初始化地图
	if( m_pDriveRegion )
	{
		m_pDriveRegion->RemoveObject(m_pDrivePlayer);
		SAFE_DELETE(m_pDriveRegion);
	}

	m_pDriveRegion = CreateMainRegion();
	//------------------------------------------------------------------
	SetDriveMainRegion(m_pDriveRegion);			//必须设置主场景管理指针！
	//---------------------------------------------------------------------
	m_pDriveRegion->SetExID(RegionID);
	m_pDriveRegion->SetResourceID(lResourceID);
	m_pDriveRegion->Load();
	//m_pDriveRegion->Init();
	m_pDriveRegion->SetName( strRegionName );

	// 加入玩家并设置坐标
	m_pDrivePlayer->SetFather( m_pDriveRegion );
	m_pDrivePlayer->SetPosXY( m_pDrivePlayer->GetTileX()+0.5f, m_pDrivePlayer->GetTileY()+0.5f );
	m_pDrivePlayer->SetDir( (CShape::eDIR)m_pDrivePlayer->GetDir() );
	m_pDrivePlayer->SetRegionID( lRegionID );
	m_pDrivePlayer->GetRegionGUID( RegionID );

	m_pDrivePlayer->SetDestXY(m_pDrivePlayer->GetPosX(), m_pDrivePlayer->GetPosY());

	extern bool g_bForceUpdate;
	g_bForceUpdate = true;
	m_pDrivePlayer->SetAction(CShape::ACT_STAND);
	m_pDrivePlayer->SetFather( m_pDriveRegion );
	//m_pDrivePlayer->RefeashState();

	m_pDriveRegion->AddObject( m_pDrivePlayer );
	//m_pDriveRegion()->ScrollCenter( m_pDrivePlayer->GetPosX() , m_pDrivePlayer->GetPosY());
	//m_pDriveRegion()->SyncUpdate();

	//SetGameState( GS_BLACK );
	// 清空走路计数
	//重置自动状态
	//GetGameControl()->SetSendedMoveMsgCount(0);
	//GetGameControl()->SetMPActionMode(CGameControl::MPAM_No);
	//GetGameControl()->ContinueMoveToOtherRegion();

	//设置宠物
//	m_pDrivePlayer->PetEnterRegion();
	//设置税率
	if(m_pDriveRegion)
	{
		m_pDriveRegion->SetTaxRate(lTaxRate);
		m_pDriveRegion->SetWarRegionType(eWarRegionType);
		m_pDriveRegion->SetExpScale(fExpScale);
		//m_pDriveRegion->SetWaterRegion(bWaterRegion);
		//当主角进入场景
		//m_pDriveRegion->OnMainPlayerEnter();

	}
	//打开宠物血条 并加入场景shape链表
//	long lPetCount = m_pDrivePlayer->GetActivePetCount();
//	if ( lPetCount > 0)
// 	{
// 		for (int i = 0; i < lPetCount; i++)
// 		{
// //			CPet *pPet = m_pDrivePlayer->GetActivePet(i+1);
// //			if (pPet)
// //				m_pDriveRegion->AddObject((CBaseObject*)pPet);
// 		}
// 	}
}

void CDriveGameClient::InitGameRgionTileHeight(CMessage *pMsg)
{

}

//void CDriveGameClient::ReleaseMapFileMsg()
//{
//	if ( !MsgQueueMSG_FILE.empty() )
//	{
//		msgQueue::iterator it = MsgQueueMSG_FILE.begin();
//		for ( ; it!=MsgQueueMSG_FILE.end(); ++it)
//		{
//			CMessage *pMsg = (CMessage*)(*it);
//			if ( pMsg )
//			{	
//				CBaseMessage::FreeMsg(pMsg); //放到OnMessage释放
//			}
//		}
//		MsgQueueMSG_FILE.clear();
//	}
//}

// DriveGameClient Client To Test MsgLoop
void CDriveGameClient::OnMessage(CMessage *pMsg)
{
	long lType = pMsg->GetType();
	lType = lType - (lType&0x00000ff);	

	switch( lType )
	{	
		//for内存映射文件 文件操作
		//case MSG_FILE:
		//	{
		//		CRITICAL_SECTION * cs = CMapScriptSystem::getInstance()->GetTestDialog()->GetTestCS();
		//		EnterCriticalSection(cs);
		//		
		//		this->AddMsgQueueMSG_FILE(pMsg);
		//		
		//		LeaveCriticalSection(cs);
		//		return;
		//	}
		//	break;
		//MsgFile(pMsg);
		//单独线程处理		
		//break;					
	case MSG_C2T_LOG:
		C2TLog(pMsg);
		break;

	case MSG_L2T_LOG:
		L2TLog(pMsg);
		break;
	case MSG_C2T_LOGIC:
		C2TLogic(pMsg);
		break;
	case MSG_C2T_RGN:
		C2TRgn(pMsg);
		break;

	case MSG_C2T_SHAPE:
		C2TShape(pMsg);
		break;

	case MSG_C2T_PLAYER:
		this->S2CPlayer(pMsg);
		break;

	case MSG_C2T_OTHER:
		OnOtherMessage(pMsg);
		//			this->S2COther(pMsg);
		break;

	case MSG_C2T_GOODS:
		this->C2TGoods(pMsg);
		break;

	case MSG_C2T_SHOP:
		this->S2CShop(pMsg);
		break;

	case MSG_C2T_TEAM:
		this->OnTeamMessage(pMsg);		
		break;

	case MSG_C2T_SKILL:
		//			this->S2CSkill(pMsg);
		break;

	case MSG_C2T_ORGSYS:
		this->OnFamilyMessage(pMsg);
		break;

	case MSG_C2T_PLAYERSHOP:
		this->S2CPlayerShop(pMsg);
		break;

	case MSG_C2T_CONTAINER:
		//this->S2CContainer(pMsg);				//调用父类函数接口
		break;

	case MSG_C2T_PET:

		break;

	case MSG_C2T_LINKMAN:
		this->S2CLinkMan(pMsg);
		break;

	case MSG_C2T_SPRITE:
		this->OnSpriteMessage(pMsg);
		break;

	case MSG_C2T_MAIL:
		this->S2CMail(pMsg);
		break;

	default:
		break;
	}
}


//--------------------------------------------------------------------------
//	FILE Msg
//--------------------------------------------------------------------------
//void CDriveGameClient::MsgFile(CMessage *pMsg)
//{
//	long lType = pMsg->GetType();
//	switch(lType)
//	{				
//	case MSG_FILE_WRITE:
//		this->MsgFileWriteMappingFile(pMsg);
//		break;
//
//	case MSG_FILE_CLEAR:
//		this->MsgFileClearMappingFile(pMsg);
//		break;
//
//	case MSG_FILE_OPEN:
//		this->MsgFileOpenMappingFile(pMsg);
//		break;
//
//	case MSG_FILE_CLOSE:
//		this->MsgFileloseMappingFile(pMsg);
//		break;
//	}
//}
//
//void CDriveGameClient::MsgFileWriteMappingFile(CMessage *pMsg)
//{
//	char strFileName[256] = " ";
//	pMsg->GetStr(strFileName, 256);
//
//	char strFiletxt[256] = " ";
//	pMsg->GetStr(strFiletxt, 256);
//	CDriveClient::GetMemServerInstance()->WriteFile(strFileName, strFiletxt);
//}
//
//void CDriveGameClient::MsgFileClearMappingFile(CMessage *pMsg)
//{
//	char strFileName[256] = " ";
//	pMsg->GetStr(strFileName, 256);
//	CDriveClient::GetMemServerInstance()->ClearFile(strFileName);
//}
//
//void CDriveGameClient::MsgFileOpenMappingFile(CMessage *pMsg)
//{
//	char strFileName[256] = " ";
//	pMsg->GetStr(strFileName, 256);
//	CDriveClient::GetMemServerInstance()->OpenFile(strFileName);
//}
//
//void CDriveGameClient::MsgFileloseMappingFile(CMessage *pMsg)
//{
//	char strFileName[256] = " ";
//	pMsg->GetStr(strFileName, 256);
//	CDriveClient::GetMemServerInstance()->CloseFile(strFileName);
//}


//--------------------------------------------------------------------------
//	C2T Log Msg
//--------------------------------------------------------------------------
void CDriveGameClient::C2TLog(CMessage *pMsg)
{
	long lType = pMsg->GetType();

	switch(lType)
	{
	case MSG_C2T_LOG_ANSWER_PLAYERDATA:							//初始化palyer数据
		C2TLogAnswerPlayerData(pMsg);
		break;

	default:
		break;
	}
}

void CDriveGameClient::C2TLogAnswerPlayerData(CMessage *pMsg)
{	
	//调用初始化玩家
	InitPlayerData(pMsg);					
}

void CDriveGameClient::L2TLog(CMessage *pMsg)
{
	long lType = pMsg->GetType();

	switch( lType )
	{
	case MSG_L2T_LOG_ANSWERCDKEY:								// 服务器返回CDKEY
		break;
	case MSG_L2T_LOG_ANSWER_PLAYERBASELIST:						// 服务器返回玩家基本数据列表
		{
			char strCDKey[20],strName[50];
			BYTE nRoleCnt=0;

			//======================================
			DWORD nLevel=0,
				nOccupation=0,
				nSex=0,
				nCountry=0,
				nHead=0,
				nHair=0,
				nFace=0,
				nNecklace=0,
				nWing=0,
				nBody=0,
				nBack=0,
				nGlove=0,
				nBoot=0,
				nHeadgear=0,
				nFrock=0,
				nLring=0,
				nRring=0,
				nMedal1=0,
				nMedal2=0,
				nMedal3=0,
				nFairy=0,
				nWeapon=0,
				nWeapon2=0,
				nHeadLevel=0,
				nNecklaceLevel=0,
				nWingLevel=0,
				nBodyLevel=0,
				nBackLevel=0,
				nGloveLevel=0,
				nBootLevel=0,
				nHeadgearLevel=0,
				nFrockLevel=0,
				nLringLevel=0,
				nRringLevel=0,
				nMedal1Level=0,
				nMedal2Level=0,
				nMedal3Level=0,
				nFairyLevel=0,
				nWeaponLevel=0,
				nWeapon2Level=0,
				nRgnID = 0,
				nField = 0;

			//获取服务器验证码
			long lSignCode = pMsg->GetLong();

			char nRemainDays;
			CGUID playerid;
			BYTE bRet=0;
			bRet=pMsg->GetByte();
			int i;

			pMsg->GetStr(strCDKey,20);
			nRoleCnt=BYTE(pMsg->GetShort());
			if(nRoleCnt==0)
			{
				return;
			}

			// 创建角色时的随机国家范围
			BYTE nRandCountryRange = pMsg->GetByte();

			nRoleCnt=BYTE(pMsg->GetShort());


			// 这里是接受3个角色 bugs

			// 清除旧的角
			for(i=0;i<nRoleCnt;i++)
			{
				// 填充
				nField=BYTE(pMsg->GetShort());//栏位值
				pMsg->GetGUID(playerid);
				pMsg->GetStr(strName,50);//得到名字

				nLevel			=pMsg->GetShort();
				nOccupation		=pMsg->GetShort();
				nSex			=pMsg->GetShort();
				nCountry		=pMsg->GetShort();
				nHead			=pMsg->GetShort();
				nHair			=pMsg->GetShort();
				nFace			=pMsg->GetLong();
				nNecklace		=pMsg->GetShort();
				nWing			=pMsg->GetShort();
				nBody			=pMsg->GetShort();
				nBack			=pMsg->GetShort();
				nGlove			=pMsg->GetShort();
				nBoot			=pMsg->GetShort();
				nHeadgear		=pMsg->GetShort();
				nFrock			=pMsg->GetShort();
				nLring			=pMsg->GetShort();
				nRring			=pMsg->GetShort();
				nMedal1			=pMsg->GetShort();
				nMedal2			=pMsg->GetShort();
				nMedal3			=pMsg->GetShort();
				nFairy			=pMsg->GetShort();
				nWeapon			=pMsg->GetShort();
				nWeapon2		=pMsg->GetShort();
				nHeadLevel		=pMsg->GetShort();
				nNecklaceLevel	=pMsg->GetShort();
				nWingLevel		=pMsg->GetShort();
				nBodyLevel		=pMsg->GetShort();
				nBackLevel		=pMsg->GetShort();
				nGloveLevel		=pMsg->GetShort();
				nBootLevel		=pMsg->GetShort();
				nHeadgearLevel	=pMsg->GetShort();
				nFrockLevel		=pMsg->GetShort();
				nLringLevel		=pMsg->GetShort();
				nRringLevel		=pMsg->GetShort();
				nMedal1Level	=pMsg->GetShort();
				nMedal2Level	=pMsg->GetShort();
				nMedal3Level	=pMsg->GetShort();
				nFairyLevel		=pMsg->GetShort();
				nWeaponLevel	=pMsg->GetShort();
				nWeapon2Level	=pMsg->GetShort();
				nRemainDays		=pMsg->GetLong();
				nRgnID			=pMsg->GetLong(); 

				//CPlayer *pPlayer = new CPlayer;
				//m_pDrivePlayer->SetGraphicsID(nSex+1);
// 				m_pDrivePlayer->SetExID(playerid);
// 				m_pDrivePlayer->SetOccupation((eOccupation)nOccupation);
// 				m_pDrivePlayer->SetName(strName);
// 				m_pDrivePlayer->SetLevel(nLevel);
// 				m_pDrivePlayer->SetSex(nSex);
// 				m_pDrivePlayer->SetCountry(nCountry);
// 				m_pDrivePlayer->SetHairPic(nHair);
// 				m_pDrivePlayer->SetFacePic(nFace);

				//===========================
				// -- 左边装备 
				//pPlayer->SetEquipIndex(CPlayer::EQUIP_HEAD, nHead);
// 				m_pDrivePlayer->SetEquipIndex(EQUIP_NECKLACE, nNecklace);
// 				m_pDrivePlayer->SetEquipIndex(EQUIP_WING,  nWing);
// 				m_pDrivePlayer->SetEquipIndex(EQUIP_BODY, nBody);
// 				m_pDrivePlayer->SetEquipIndex(EQUIP_WAISTBAND, nBack);
// 				m_pDrivePlayer->SetEquipIndex(EQUIP_GLOVE, nGlove);
// 				m_pDrivePlayer->SetEquipIndex(EQUIP_SHOES, nBoot);
// 
// 				// -- 右边装备
// 				m_pDrivePlayer->SetEquipIndex(EQUIP_HEADGEAR, nHeadgear);
// 				m_pDrivePlayer->SetEquipIndex(EQUIP_FROCK, nFrock);
// 				m_pDrivePlayer->SetEquipIndex(EQUIP_LRING,  nLring);
// 				m_pDrivePlayer->SetEquipIndex(EQUIP_RRING, nRring);
// 				m_pDrivePlayer->SetEquipIndex(EQUIP_MEDAL1, nMedal1);
// 				m_pDrivePlayer->SetEquipIndex(EQUIP_MEDAL2, nMedal2);
// 				m_pDrivePlayer->SetEquipIndex(EQUIP_MEDAL3, nMedal3);
// 
// 				// -- 下边装备
// 				m_pDrivePlayer->SetEquipIndex(EQUIP_FAIRY, nFairy);
// 				m_pDrivePlayer->SetEquipIndex(EQUIP_WEAPON, nWeapon);
// 				m_pDrivePlayer->SetEquipIndex(EQUIP_WEAPON2, nWeapon2);
// 
// 				// --装备等级
// 				// -- 左边装备
// 				m_pDrivePlayer->SetEquipLevel(EQUIP_HEAD, nHeadLevel);
// 				m_pDrivePlayer->SetEquipLevel(EQUIP_NECKLACE, nNecklaceLevel);
// 				m_pDrivePlayer->SetEquipLevel(EQUIP_WING,  nWingLevel);
// 				m_pDrivePlayer->SetEquipLevel(EQUIP_BODY, nBodyLevel);
// 				m_pDrivePlayer->SetEquipLevel(EQUIP_WAISTBAND, nBackLevel);
// 				m_pDrivePlayer->SetEquipLevel(EQUIP_GLOVE, nGloveLevel);
// 				m_pDrivePlayer->SetEquipLevel(EQUIP_SHOES, nBootLevel);
// 
// 				// -- 右边装备
// 				m_pDrivePlayer->SetEquipLevel(EQUIP_HEADGEAR, nHeadgearLevel);
// 				m_pDrivePlayer->SetEquipLevel(EQUIP_FROCK, nFrockLevel);
// 				m_pDrivePlayer->SetEquipLevel(EQUIP_LRING,  nLringLevel);
// 				m_pDrivePlayer->SetEquipLevel(EQUIP_RRING, nRringLevel);
// 				m_pDrivePlayer->SetEquipLevel(EQUIP_MEDAL1, nMedal1Level);
// 				m_pDrivePlayer->SetEquipLevel(EQUIP_MEDAL2, nMedal2Level);
// 				m_pDrivePlayer->SetEquipLevel(EQUIP_MEDAL3, nMedal3Level);
// 
// 				// -- 下边装备
// 				m_pDrivePlayer->SetEquipLevel(EQUIP_FAIRY, nFairyLevel);
// 				m_pDrivePlayer->SetEquipLevel(EQUIP_WEAPON, nWeaponLevel);
// 				m_pDrivePlayer->SetEquipLevel(EQUIP_WEAPON2, nWeapon2Level);
				m_pDrivePlayer->SetRegionID(nRgnID);
/*				m_pDrivePlayer->SetPlayerRemainDays(nRemainDays);*/

				m_pDrivePlayer->SetState(CShape::STATE_PEACE);
			}
		}
		break;
	case MSG_L2T_LOG_ANSWERCREATE:								// 服务器返回建立角色的信息
		break;
	case MSG_L2T_LOG_ANSWERRESTORE:								// 是否成功回复角色
		break;
	case MSG_L2T_LOG_UPDATEQUEUENUM:							// 更新登陆缓冲队列的剩余数量
		break;
	case MSG_L2T_LOG_SENDACCOUNTINFO:							// 发送帐号的信息给客户端
		break;
	case MSG_L2T_LOG_UPDATEWORLDSERVERINFO:
		break;
	case MSG_L2T_LOG_ANSWER_PLAYERDATA:
		{
			long lChangeServer = pMsg->GetLong();
			int result=pMsg->GetChar();
			if(result==LOGIN_QUEST_PLAYERDATA_SUCCEED)
			{
				char strGsIp[20],strCDKey[20];
				long lGsPort;
				pMsg->GetStr(strCDKey,20);
				pMsg->GetStr(strGsIp,20);
				lGsPort=pMsg->GetLong();

				this->SetCDKey(strCDKey);									
			}
		}
		break;
	default:
		break;
	}
}
//--------------------------------------------------------------------------
//	C2T Rgn Msg
//--------------------------------------------------------------------------
void CDriveGameClient::C2TRgn(CMessage *pMsg)
{
	long lType = pMsg->GetType();

	switch(lType)
	{
	case MSG_C2T_RGN_ADDSHAPE:
		C2TRgnAddShape(pMsg);
		break;
	case MSG_C2T_RGN_DELSHAPE:
		C2TRgnDelShape(pMsg);
		break;
	case MSG_C2T_RGN_CHANGE:
		C2TRgnChange(pMsg);
		break;
	case MSG_C2T_RGN_CHANGESERVER:
		C2TRgnChangeServer(pMsg);
		break;
	case MSG_C2T_RGN_CHANGENAME:
		C2TRgnChangeName(pMsg);
		break;
	case MSG_C2T_RGN_CLEARDUPRGNPLAYER:
		C2TRgnClearDupRgnPlayer(pMsg);
		break;
	case MSG_C2T_RGN_PLAYACTION:
		C2TRgnPlayAction(pMsg);
		break;
	default:
		break;
	}
}

void CDriveGameClient::C2TRgnAddShape(CMessage *pMsg)
{	
	//调用父类
	//	this->RegionAddShape(pMsg);
}

void CDriveGameClient::C2TRgnDelShape(CMessage *pMsg)
{
	//	this->RegionDelShape(pMsg);
}

void CDriveGameClient::C2TRgnChange(CMessage *pMsg)
{
	//	this->RegionChanged(pMsg);
	//long lSocketid = pMsg->GetSocketID();

	//long lType = pMsg->GetLong();
	//CGUID PlayerGuid;
	//pMsg->GetGUID(PlayerGuid);
	//long lUseGoods = pMsg->GetLong();
	////
	//if( m_pDrivePlayer->GetExID() == PlayerGuid )
	//{
	//	CGUID RegionGuid;
	//	pMsg->GetGUID(RegionGuid);

	//	long lRegionID = pMsg->GetLong();
	//	long lPlayerX = pMsg->GetLong();
	//	long lPlayerY = pMsg->GetLong();
	//	long lPlayerDir = pMsg->GetLong();

	//	char strRegionName[256] = "";
	//	pMsg->GetStr(strRegionName,256);
	//	long lTaxRate = pMsg->GetLong();

	//	REGION_TYPE eWarRegionType = (REGION_TYPE)pMsg->GetLong();
	//	BYTE bWaterRegion = pMsg->GetByte();
	//	long lResourceID = pMsg->GetLong();
	//	float fExScale = pMsg->GetFloat();

	//	assert(lPlayerX>=0&&lPlayerY>=0);

	//	DBReadSet db_read;
	//	pMsg->GetDBReadSet(db_read);
	//	this->DecordLineFromDataBlock(db_read);

	//	//请求进入场景
	//	if( m_pDriveRegion )
	//	{
	//		m_pDriveRegion->RemoveObject(m_pDrivePlayer);
	//		//SAFE_DELETE(m_pRegion);
	//		//释放m_listBaseObj
	//		m_pDriveRegion->DeleteAllChildObject();
	//	}
	//	m_pDriveRegion->SetExID(RegionGuid);
	//	m_pDriveRegion->SetResourceID(lResourceID);
	//	m_pDriveRegion->Load();
	//	m_pDriveRegion->SetName(strRegionName);
	//	//
	//	m_pDrivePlayer->SetPosXY(lPlayerX + 0.5f,lPlayerY + 0.5f);
	//	m_pDrivePlayer->SetRegionID(lRegionID);

	//	m_pDrivePlayer->SetDestXY(lPlayerX + 0.5f,lPlayerY + 0.5f);
	//	m_pDrivePlayer->SetAction(CShape::ACT_STAND);


	//	m_pDrivePlayer->SetFather(m_pDriveRegion);

	//	m_pDriveRegion->AddObject(m_pDrivePlayer);

	//	if( m_pDriveRegion )
	//	{
	//		m_pDriveRegion->SetTaxRate(lTaxRate);
	//		m_pDriveRegion->SetWarRegionType(eWarRegionType);
	//		m_pDriveRegion->SetExpScale(fExScale);
	//	}
	//}
	//else{
	//	CShape* pShape = (CShape*)m_pDriveRegion->FindChildObject(lType,PlayerGuid);
	//	if( pShape )
	//	{
	//		m_pDriveRegion->RemoveObject(pShape);
	//		SAFE_DELETE(pShape);
	//	}
	//}
}

void CDriveGameClient::C2TRgnChangeName(CMessage *pMsg)
{

}

void CDriveGameClient::C2TRgnChangeServer(CMessage *pMsg)
{
	//	this->RegionChangeServer(pMsg);
	//获取验证码
	//long lChangeServerCode = pMsg->GetLong();
	//long lSocketID = pMsg->GetSocketID();
	//char strIP[256];
	//pMsg->GetStr(strIP,256);
	//DWORD dwPort = pMsg->GetDWord();
}

void CDriveGameClient::C2TRgnPlayAction(CMessage *pMsg)
{

}

void CDriveGameClient::C2TRgnClearDupRgnPlayer(CMessage *pMsg)
{

}

//--------------------------------------------------------------------------
//	Logic Msg 
//--------------------------------------------------------------------------
void CDriveGameClient::C2TLogic(CMessage *pMsg)
{
	long lType = pMsg->GetType();
	switch( lType )
	{
	case MSG_C2T_LOGIC_UIISOPENED:
		C2TLogicUiIsOpend(pMsg);
		break;
	default:
		break;
	}
}

void CDriveGameClient::C2TLogicUiIsOpend(CMessage *pMsg)
{

	char strUIName[256] = "";
	pMsg->GetStr(strUIName ,256);
	bool bState = pMsg->GetByte() != 0;

	if ( strcmp(strUIName, "CharacterPage") == 0 )
	{
//		m_pDrivePlayer->SetCharacterPageState(bState);
		return ;
	}
	else if( strcmp(strUIName, "SkillPage") == 0 )
	{
//		m_pDrivePlayer->SetSkillPageState(bState);
		return ;
	}
	else if( strcmp(strUIName, "DepotUIPage") == 0 )
	{
//		m_pDrivePlayer->SetDepotUIPageState(bState);
		return ;
	}
	else if( strcmp(strUIName, "MailPage") == 0 )
	{
//		m_pDrivePlayer->SetMailPageState(bState);
		return ;
	}
	else if( strcmp(strUIName, "FriendsUIPage") == 0 )
	{
//		m_pDrivePlayer->SetFriendsUIPageState(bState);
		return ;
	}
	else if( strcmp(strUIName, "TeamPage") == 0 )
	{
//		m_pDrivePlayer->SetTeamPageState(bState);
		return ;
	}
	else if( strcmp(strUIName, "ItemPage") == 0 )
	{
//		m_pDrivePlayer->SetItemPageState(bState);
		return ;
	}
	else if( strcmp(strUIName, "SubBackUIPage") == 0 )
	{
//		m_pDrivePlayer->SetSubBackUIPageState(bState);
		return ;
	}
}

//--------------------------------------------------------------------------
//	Shape Msg 
//--------------------------------------------------------------------------
void CDriveGameClient::C2TShape(CMessage *pMsg)
{	
	long lType = pMsg->GetType();

	switch( lType )
	{
	case MSG_C2T_SHAPE_CHANGEDIR:
		C2TShapeChangeDir(pMsg);
		break;

	case MSG_C2T_SHAPE_SETPOS:
		C2TShapeSetPos(pMsg);
		break;

	case MSG_C2T_SHAPE_FORCEMOVE:
		C2TShapeForceMove(pMsg);
		break;

	case MSG_C2T_SHAPE_MOVE:
		C2TShapeMove(pMsg);
		break;

	case MSG_C2T_SHAPE_STOPMOVE:
		C2TShapeStopMove(pMsg);
		break;

	case MSG_C2T_SHAPE_CHANGEMOVEMODE:
		C2TShapeMoveMode(pMsg);
		break;

	case MSG_C2T_SHAPE_CHANGESTATE:
		C2TShapeChangeState(pMsg);
		break;

	case MSG_C2T_SHAPE_ATK_DIED:
		C2TShapeAtkDied(pMsg);
		break;

	case MSG_C2T_SHAPE_SINSTATE:
		C2TShapeSinState(pMsg);
		break;

	case MSG_C2T_SHAPE_Refresh:
		C2TShapeRefresh(pMsg);
		break;

	case MSG_C2T_SHAPE_EMOTION:
		C2TShapeEmotion(pMsg);
		break;

	case MSG_C2T_SHAPE_ATK_BREAK:
		C2TShapeAtkBreak(pMsg);
		break;
	case MSG_C2T_SHAPE_ATK_DAMAGE:
		C2TShapeAtkDamage(pMsg);
		break;
	case MSG_C2T_SHAPE_CHANGE_PROPERTIES:
		C2TShapeChangeProperties(pMsg);
		break;
	default :
		break;
	}
}

void CDriveGameClient::C2TShapeAtkDied(CMessage *pMsg)
{
	//	this->ShapeAtkDied(pMsg);
}

void CDriveGameClient::C2TShapeChangeDir(CMessage *pMsg)
{
}

void CDriveGameClient::C2TShapeChangeState(CMessage *pMsg)
{

}

void CDriveGameClient::C2TShapeEmotion(CMessage *pMsg)
{

}

void CDriveGameClient::C2TShapeForceMove(CMessage *pMsg)
{

}

void CDriveGameClient::C2TShapeMove(CMessage *pMsg)
{
	//	this->ShapeMove(pMsg);
}

void CDriveGameClient::C2TShapeSetPos(CMessage *pMsg)
{
	//	this->ShapeSetPos(pMsg);
}

void CDriveGameClient::C2TShapeStopMove(CMessage *pMsg)
{	
	//	this->ShapeMoveStop(pMsg);
}

void CDriveGameClient::C2TShapeRefresh(CMessage *pMsg)
{

}

void CDriveGameClient::C2TShapeSinState(CMessage *pMsg)
{

}

void CDriveGameClient::C2TShapeMoveMode(CMessage *pMsg)
{

}

void CDriveGameClient::C2TShapeAtkBreak(CMessage *pMsg)
{
	//	this->BeAtk(pMsg);
}

void CDriveGameClient::C2TShapeAtkDamage(CMessage *pMsg)
{
	//	this->ShapeAtkDamage(pMsg);
}
void CDriveGameClient::C2TShapeChangeProperties(CMessage *pMsg)
{
	//	this->OnShapeUpdatePropertyMsg(pMsg);
}
//--------------------------------------------------------------------------
//	Player Msg
//--------------------------------------------------------------------------
void CDriveGameClient::C2TPlayer(CMessage *pMsg)
{
	//long lType = pMsg->GetType();
	//switch( lType )
	//{
	//case 1:
	//	break;
	//default :
	//	break;
	//}
}

//--------------------------------------------------------------------------
//	Goods Msg
//--------------------------------------------------------------------------
void CDriveGameClient::C2TGoods(CMessage *pMsg)
{
	long lType = pMsg->GetType();
	switch( lType )
	{
	case MSG_C2T_SETHOTKEY:
		{
			int result=pMsg->GetChar();
			int nIndex=pMsg->GetByte();
			DWORD dwVal=pMsg->GetDWord();
			if(nIndex>=0 && nIndex<24)
			{
//				m_pDrivePlayer->SetHotKey(nIndex, dwVal);
			}
		}
		break;
	case MSG_C2T_CLEARHOTKEY:
		{
			int result=pMsg->GetChar();
			int nIndex=pMsg->GetByte();

			if(result==HOTKEY_CLEAR_SUCCEED && nIndex>=0 && nIndex<24)
			{
				int extend = 0;
				int pos = 0;
//				DWORD dwHotKey = m_pDrivePlayer->GetHotKey(nIndex);
//				m_pDrivePlayer->SetHotKey(nIndex,0);
			}
		}
		break;
	case MSG_C2T_EXCHANGEHOTKEY:
		{
			int result=pMsg->GetChar();
			int nIndex=pMsg->GetByte();
			int extend = 0;
			int pos = 0;

			if(result==HOTKEY_SET_SUCCEED && nIndex>=0 && nIndex<24)
			{
				DWORD dwGoodIndex = pMsg->GetDWord();
				if (m_pDrivePlayer!=NULL)
				{
//					DWORD dwHotKey = m_pDrivePlayer->GetHotKey(nIndex);
//					m_pDrivePlayer->SetHotKey(nIndex,dwGoodIndex);
				}
			}
		}
		break;
	default :
		break;
	}
}

