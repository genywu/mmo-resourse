#include "StdAfx.h"
#include ".\GameClient.h"
#include "RgnObjMgr.h"
#include "SkillMgr.h"
#include "MailMgr.h"
#include "DepotMgr.h"
#include "EquipMgr.h"
#include "PetMgr.h"
#include "PlayerShop.h"
#include "GoodsMgr.h"
#include "PacketMgr.h"
#include "MedalMgr.h"

#include "..\GameTest\Resource.h"
#include "MoveShape.h"
#include "Player.h"
#include "ClientRegion.h"
#include "Monster.h"
#include "..\Utility\FindPath.h"
//#include "..\Net\Message.h"
#include "..\Script\ScriptSys.h"
#include "..\GameTest\GameTestDlg.h"
#include "CollectionGoods.h"

//////////////////////////////////////////////////////////////////////////
//	宠物
//#include "PetSystem/CPet.h"
//#include "PetSystem/CFairyPet.h"
//#include "PetSystem/CFighterPet.h"
//#include "PetSystem/CSummonPet.h"
//#include "PetSystem/CTrap.h"
//////////////////////////////////////////////////////////////////////////
//	@	增值商店
#include "IncrementShop.h"
//	@

#include "../Utility/gtDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CGameClient::CGameClient()
{
	m_pMainPlayer = NULL;
	m_lReGameCode = 0;
	m_pRegion = NULL;
	m_pClientObjMgr = new CClientRgnObjMgr();
	m_lSocketID = 0;
	m_strCDkey = "";
	m_pTimer = NULL;
	m_lTimeID = 0;
	m_uID = 0;
	m_FollowObjectGuid = CGUID::GUID_INVALID;
	m_lDestX = 0;
	m_lDestY = 0;
	m_lAimRegionID = 0;
	m_lLineNum = 0;
	m_lCurSelLineID = 0;
	m_lLineID = 0;
	m_dwAimConPos = 0;
	m_eCurOpenPageType = (ePageType)0;
	m_nCurOpenWindowsType = 0;
	m_bIsPlayerMove = false;
	m_lKilMotrNum = 0;

	ZeroMemory(&m_UpgradeStuff,sizeof(tagUpgradeStuff));
	ZeroMemory(&m_tagEnchaseSession,sizeof(m_tagEnchaseSession));

	m_pEnchaseGoods = new tagEnchaseGoods;
	ZeroMemory(m_pEnchaseGoods,sizeof(tagEnchaseGoods));

	ZeroMemory(&m_tagSynthesizeGoods,sizeof(m_tagSynthesizeGoods));
	m_lPrescriptionIndex = 0;

	ZeroMemory(&m_tagEvilStuff,sizeof(m_tagEvilStuff));

	m_dwLastPingTime = 0;
	m_dwBlankPingTime = 0;
	m_bIsCanPingSrv = true;
}
CGameClient::CGameClient(Timer*	pTimer)
{
	m_pMainPlayer = NULL;
	m_lReGameCode = 0;
	m_pRegion = NULL;
	m_pClientObjMgr = new CClientRgnObjMgr();
	m_lSocketID = 0;
	m_strCDkey = "";
	m_pTimer = pTimer;
	m_lTimeID = 0;
	m_uID = 0;
	m_FollowObjectGuid = CGUID::GUID_INVALID;
	m_lDestX = 0;
	m_lDestY = 0;
	m_lAimRegionID = 0;
	m_lLineNum = 0;
	m_lCurSelLineID = 0;
	m_lLineID = 0;
	m_dwAimConPos = 0;
	m_eCurOpenPageType = (ePageType)0;
	m_nCurOpenWindowsType = 0;
	m_bIsPlayerMove = false;
	m_lKilMotrNum = 0;
	
	ZeroMemory(&m_UpgradeStuff,sizeof(tagUpgradeStuff));
	ZeroMemory(&m_tagEnchaseSession,sizeof(m_tagEnchaseSession));
	
	m_pEnchaseGoods = new tagEnchaseGoods;
	ZeroMemory(m_pEnchaseGoods,sizeof(tagEnchaseGoods));

	ZeroMemory(&m_tagSynthesizeGoods,sizeof(m_tagSynthesizeGoods));
	m_lPrescriptionIndex = 0;

	ZeroMemory(&m_tagEvilStuff,sizeof(m_tagEvilStuff));

	m_dwLastPingTime = 0;
	m_dwBlankPingTime = 0;
	m_bIsCanPingSrv = true;
}

CGameClient::~CGameClient(void)
{
	SAFE_DELETE(m_pEnchaseGoods);
	SAFE_DELETE(m_pClientObjMgr);
}

//消息处理
void CGameClient::OnMessage(CMessage* msg)
{
	long lSocketID = msg->GetSocketID();
	CGameClient* pClient = CScriptSys::getInstance()->GetTestDialog()->GetGameClient(lSocketID);
	if( pClient == this && m_pMainPlayer != NULL && m_pRegion != NULL)
	{
		eMsgTYpe eType = (eMsgTYpe)msg->GetType();
		long lType = (long)eType;
		lType = lType - (lType&0x00000ff);
		switch( lType )
		{
		case MSG_S2C_SPRITE:
			OnSpriteMessage(msg);
			break;
		case MSG_S2C_OTHER:
			OnOtherMessage(msg);
			//			S2COther(msg);
			break;
		case MSG_S2C_SHAPE:
			OnShapeMessage(msg);
			//			S2CShape(msg);
			break;
		case MSG_S2C_RGN:
			OnReginMessage(msg);
			//			S2CRegion(msg);
			break;
		case MSG_S2C_CONTAINER:
			OnContainerMsg(msg);
			break;
		case MSG_S2C_SKILL:
			OnSkillMessage(msg);
			//			S2CSkill(msg);
			break;
		case MSG_S2C_PLAYER:
			OnPlayerMessage(msg);
			//S2CPlayer(msg);
			break;
		case MSG_S2C_GOODS:
			OnGoodsMessage(msg);
			//S2CGoods(msg);
			break;
		case MSG_S2C_TEAM:
			//S2CTeam(msg);
			OnTeamMessage(msg);
			break;
		case MSG_S2C_LINKMAN:
			S2CLinkMan(msg);
			break;
		case MSG_S2C_SHOP:
			S2CShop(msg);
			break;
		case MSG_S2C_ORGSYS:
			OnFamilyMessage(msg);
			break;
		case MSG_S2C_PLAYERSHOP:
			S2CPlayerShop(msg);
			break;
		case MSG_S2C_MAIL:
			S2CMail(msg);
			break;
		case MSG_S2C_INCREMENTSHOP:
			S2CIncrementShop(msg);
			break;
		case MSG_S2C_PM:
			S2CPlayerManage(msg);
			break;

		case MSG_S2C_PET:
			OnPetMessage(msg);
			break;
			//回答问题
		case MSG_S2C_ANTICHEAT:
			AntiCheat(msg);
			break;
			//	赠送
		case MSG_S2C_BOUNS:
			this->OnBounsMessage(msg);
		default:
			break;
		}
	}
}

//答题
//*******
void CGameClient::AntiCheat(CMessage *msg)
{
	switch(msg->GetType())
	{
	case MSG_S2C_AC_QUESTION://答题消息
		AntiCheatQuestion(msg);	
		break;
	default:
		break;
	}
}

void CGameClient::AntiCheatQuestion(CMessage *msg)
{	
	//DWORD  dwSize=msg->GetDWord();
	//BYTE *pBuf=new BYTE[dwSize];
	//msg->GetEx(pBuf,dwSize);

	//SAFE_DELETE_ARRAY(pBuf);
//	m_pMainPlayer->SetIsAnswerQuestion(true);
}

//答题end

//服务器返回玩家发送的玩家搜索消息
void CGameClient::S2CPlayerManage(CMessage* msg)
{
	long lType = msg->GetType();
	switch(lType)
	{
	case MSG_S2C_PM_ACK:
		this->PlayerManageAtk(msg);
		break;
	}
}

//设置已经找到的玩家的GUID
void CGameClient::PlayerManageAtk(CMessage* msg)
{
	CGUID playerguid;
	msg->GetGUID(playerguid);
	long msgsize = msg->GetLong();
}

//对增值商店的S2C消息处理
void CGameClient::S2CIncrementShop(CMessage* msg)
{
	long lType = msg->GetType();
	switch(lType)
	{
	case MSG_S2C_INCREMENTSHOP_OPEN:
		this->IncrementShopOpen(msg);
		break;
	}
}

//当打开增值商店以后服务器返回的消息进行处理
void CGameClient::IncrementShopOpen(CMessage* msg)
{
	CIncrementShop::GetIncrShop()->DecodeIncrShop(msg);
}

//-------------------------------------------------------------------------------------------------------
//邮件
//-------------------------------------------------------------------------------------------------------

void CGameClient::S2CMail(CMessage* msg)
{
	long lType = msg->GetType();
	switch(lType)
	{
	case MSG_S2C_MAIL_RECEIVER:
		this->MailReceiver(msg);
		break;
	case MSG_S2C_MAIL_SEND_FAILED:
		MailSendFailed(msg);
		break;
	case MSG_S2C_MAIL_SEND:
		MailSend(msg);
		break;
	case MSG_S2C_MAIL_READ:
		MailRead(msg);
		break;
	case MSG_S2C_MAIL_FAILED_MOVE_GOODS:
		MailFailedMoveGoods(msg);
		break;
	case MSG_S2C_MAIL_MOVE_GOODS_RESULT:
		MailMoveGoodsResult(msg);
		break;
// 	case MSG_S2C_MAIL_DELETE_RESULT:
// 		MailDeleteResult(msg);
// 		break;
	case MSG_S2C_MAIL_DELETE_OK:
		MailDeleteOk(msg);
		break;
	case MSG_S2C_MAIL_AGREED_PAY:
		MailAgreedPay(msg);
		break;
// 	case MSG_S2C_MAIL_AGREED_PRESENT:
// 		MailAgreedPresent(msg);
// 		break;
	case MSG_S2C_MAIL_GET_GOLD_RESULT:
		MailGetGoldResult(msg);
		break;
// 	case MSG_S2C_MAIL_PRESENT_FAILED:
// 		MailPersentFailed(msg);
// 		break;
	default:
		break;
	}
}

void CGameClient::MailReceiver(CMessage* pMsg)
{	
	CMailMgr* pMailMgr = m_pMainPlayer->GetMailMgr();
	if( pMailMgr == NULL )	return;

	pMailMgr->ClearMailData();
//	m_pMainPlayer->MailClearMailData();
	long dwSize=pMsg->GetLong();
	if( dwSize<=0 ) return ;
	BYTE *pBuff=new BYTE[dwSize];
	pMsg->GetEx(pBuff,dwSize);
	long lPos = 0;
	// 邮件个数
	long lNum = _GetLongFromByteArray(pBuff, lPos);
	for (int i=0; i!=lNum; ++i)
	{
		stMailData stData;

		pMailMgr->DecodeFromByteArray(stData,pBuff,lPos);
		pMailMgr->AddMail(stData);
//		m_pMainPlayer->MailDecordDataFromByteArray(stData, pBuff, lPos);
//		m_pMainPlayer->AddMail(stData);
	}
	SAFE_DELETE_ARRAY(pBuff);



}

void CGameClient::MailSendFailed(CMessage* msg)
{
	long index = msg->GetLong();
	string str = "";
}

void CGameClient::MailSend(CMessage* msg)
{}

void CGameClient::MailRead(CMessage* msg)
{
	CMailMgr* pMailMgr = m_pMainPlayer->GetMailMgr();
	if( pMailMgr == NULL ) return;

	BYTE bFlag = msg->GetByte();
	if(bFlag > 0)
	{
		CGUID guid;
		msg->GetGUID(guid);
		stMailData *stData = pMailMgr->GetMailByGUID(guid);
		if(stData)
		{
			stData->bIsReaded = true;
		}
	}
}

void CGameClient::MailFailedMoveGoods(CMessage* msg)
{}

void CGameClient::MailMoveGoodsResult(CMessage* msg)
{
	CMailMgr* pMailMgr = m_pMainPlayer->GetMailMgr();
	if( pMailMgr == NULL ) return;

	CGUID guid;
	msg->GetGUID(guid);
	long index = msg->GetLong();
	BYTE bFlag = msg->GetByte();
	if(bFlag > 0)
	{
		stMailData *stData = pMailMgr->GetMailByGUID(guid);
		if(stData)
		{
			list<CGoods *>::iterator it = stData->GoodsList.begin();
			for (int i=0; it != stData->GoodsList.end(); ++it, ++i)
			{
				if(i == index)
				{
					SAFE_DELETE((*it));
					stData->GoodsList.erase(it);
					break;
				}
			}
			stData->bIsCanReBack = false;
			// 物品和金币都没有，转化成普通类型的邮件
			if(stData->GoodsList.empty() && stData->dwGoldNum == 0)
				stData->eMailType = COMMON_MAIL;
		}
	}
}

void CGameClient::MailDeleteResult(CMessage* msg)
{}

void CGameClient::MailDeleteOk(CMessage* msg)
{	
	CMailMgr* pMailMgr = m_pMainPlayer->GetMailMgr();
	if( pMailMgr == NULL ) return;

	long size = msg->GetLong();
	if(size > 0)
	{
		for (int i=0; i<size; ++i)
		{
			CGUID guid;
			msg->GetGUID(guid);
			pMailMgr->DelMailByGuid(guid);
		}
	}
}

void CGameClient::MailAgreedPay(CMessage* msg)
{
	CMailMgr* pMailMgr = m_pMainPlayer->GetMailMgr();
	if( pMailMgr == NULL ) return;

	CGUID guid;
	msg->GetGUID(guid);
	eMAILTYPE eType = (eMAILTYPE)msg->GetByte();
	stMailData *stData = pMailMgr->GetMailByGUID(guid);
	if(stData)
	{
		stData->eMailType = eType;
		stData->dwGoldNum = 0;
	}
}

void CGameClient::MailAgreedPresent(CMessage* msg)
{}

void CGameClient::MailGetGoldResult(CMessage* msg)
{
	CMailMgr* pMailMgr = m_pMainPlayer->GetMailMgr();
	if( pMailMgr == NULL ) return;

	CGUID guid;
	msg->GetGUID(guid);
	BYTE bFlag = msg->GetByte();
	if(bFlag > 0)
	{
		stMailData *stData = pMailMgr->GetMailByGUID(guid);
		if(stData)
		{
			stData->dwGoldNum = 0;
			stData->bIsCanReBack = false;
			// 物品和金币都没有，转化成普通类型的邮件
			if(stData->GoodsList.empty() && stData->dwGoldNum == 0)
				stData->eMailType = COMMON_MAIL;
		}
	}
}

void CGameClient::MailPersentFailed(CMessage* msg)
{
	CMailMgr* pMailMgr = m_pMainPlayer->GetMailMgr();
	if( pMailMgr == NULL ) return;

	CGUID guid;
	msg->GetGUID(guid);
	
	stMailData *stData = pMailMgr->GetMailByGUID(guid);
	if(stData)
	{
		// 清空以前的附件
		list<CGoods *>::iterator it = stData->GoodsList.begin();
		for (int i=0; it != stData->GoodsList.end(); ++it, ++i)
		{
			SAFE_DELETE((*it));
		}
		stData->GoodsList.clear();
		// 加入新的附件信息
		long size = msg->GetLong();
		for (long i=0; i<size; ++i)
		{
			long index = msg->GetLong();
			long num = msg->GetLong();
			CGoods *pGoods = new CGoods();
			CGoodsList::FillGoodsInfo(index,pGoods);
			if(pGoods)
			{
				pGoods->SetNum(num);
				stData->GoodsList.push_back(pGoods);
			}
		}
	}
}

//玩家个人商店
void CGameClient::S2CPlayerShop(CMessage* msg)
{
	long lType = msg->GetType();
	switch(lType)
	{
	case MSG_S2C_PALYERSHOP_CREATE:
		this->PlayerShopCreate(msg);
		break;
	default:
		break;
	}
}

//服务器发送开启个人商店的消息后服务器返回的消息,包括面板ID,sessionID
void CGameClient::PlayerShopCreate(CMessage* msg)
{
	//CGUID sessionid;
	//msg->GetGUID(sessionid);
	//long plugid = msg->GetLong();
	//msg->GetStr()
	//m_pMainPlayer->SetPlayerShopSessionID(sessionid);
	//m_pMainPlayer->SetPlayerShopPlugID(plugid);
	CGUID sessionid;
	msg->GetGUID(sessionid);	//获得SESSIONID
	long plugid = msg->GetLong();	//获得面板ID
	char *str = NULL;
	long strl = 0;
	msg->GetStr(str,strl);

	m_pMainPlayer->GetShopMgr()->SetPlugID(plugid);
	m_pMainPlayer->GetShopMgr()->SetSssionID(sessionid);
	m_pMainPlayer->GetShopMgr()->SetOpened(true);
}

//-------------------------------------------------------------------------------------------------------
//添加好友消息
//-------------------------------------------------------------------------------------------------------

void CGameClient::S2CLinkMan(CMessage* msg)
{
	return;

	long lType = msg->GetType();
	switch( lType )
	{
	case MSG_S2C_LINKMAN_INIT:
		this->LinkManInit(msg);
		break;
	case MSG_S2C_LINKMAN_ADD:
		this->LinkManAdd(msg);
		break;
	case MSG_S2C_LINKMAN_DEL:
		this->DelLinkman(msg);
		break;
	case MSG_S2C_LINKMAN_MOVE:
		this->RemoveLinkMan(msg);
		break;
	default:
		break;
	}
}
//
void CGameClient::LinkManInit(CMessage* msg)
{
	//m_pMainPlayer
	eLinkmanGroupType grouptype;			//脚本下的类别	
	long maxnum = 0;								//当前group最大人数
	for (int i=0; i!=5; ++i)
	{
		grouptype = (eLinkmanGroupType)msg->GetLong();
		maxnum    = msg->GetLong();
		if ((maxnum<0) || (maxnum>20))
		{
			maxnum = 0;								//用于检测判断益出
		}
		for (int j=0; j!=maxnum; ++j)
		{	
			tagLinkman *LinkManData = new tagLinkman;
			ZeroMemory(LinkManData, sizeof(tagLinkman));
			msg->GetEx(LinkManData, sizeof(tagLinkman));

			m_pMainPlayer->GetLinkMan();
//			m_pMainPlayer->SetLinkManPanleData(LinkManData, i);

			SAFE_DELETE(LinkManData);
		}
	}
}

void CGameClient::LinkManAdd(CMessage* msg)
{
	if (msg->GetLong())												//检测成功
	{
		DWORD groupid = (DWORD)msg->GetLong();						//从消息里获取groupID
// 		int   panelid = m_pMainPlayer->GroupIDtoPanelID(groupid);
// 		int   number  = m_pMainPlayer->GetSPanle(panelid).number;
// 		if (number >= 20)											//如果单前分组已有人数大于20
// 		{
// 			return ;
// 		}
// 		tagLinkman *LinkManData = new tagLinkman;
// 		ZeroMemory(LinkManData, sizeof(tagLinkman));
// 		msg->GetEx(LinkManData, sizeof(tagLinkman));
// 		m_pMainPlayer->SetLinkManPanleData(LinkManData, panelid);
// 
// 		SAFE_DELETE(LinkManData);
		return ;
	}
}

void CGameClient::DelLinkman(CMessage* msg)
{
	if (msg->GetLong())
	{
		DWORD groupid = (DWORD)msg->GetLong();						//从消息里获取groupID
// 		int   panelid = m_pMainPlayer->GroupIDtoPanelID(groupid);
// 		CHAR  nameLinkman[21];
// 		msg->GetStr(nameLinkman, 21);
// 		//遍历当前Spanel下的map
// 		m_pMainPlayer->DelteLinkManPanleData(panelid, nameLinkman);

		return ;
	}
}

void CGameClient::RemoveLinkMan(CMessage* msg)
{

	if (msg->GetLong())
	{	
		// 通过取得源 idGroup 获得 面板ID
		DWORD srcGroupID  = (DWORD)msg->GetLong();
		// 通过取得目标 idGroup 获得 面板ID
		DWORD destGroupID = (DWORD)msg->GetLong();
// 		int   srcpanelid  = m_pMainPlayer->GroupIDtoPanelID(srcGroupID);
// 		int   destpanelid = m_pMainPlayer->GroupIDtoPanelID(destGroupID);
// 		tagLinkman *LinkManData = new tagLinkman;
// 		ZeroMemory(LinkManData, sizeof(tagLinkman));
// 		msg->GetEx(LinkManData, sizeof(tagLinkman));
// 		//删除源组类data
// 		m_pMainPlayer->DelteLinkManPanleData(srcpanelid, LinkManData->szName);
// 		//添加到目标分组
// 		m_pMainPlayer->SetLinkManPanleData(LinkManData, destpanelid);
// 
// 		SAFE_DELETE(LinkManData);

		return ;
	}
}

// 重写队伍消息 [12/26/2008 Square]
/*void CGameClient::OnTeamMessage(CMessage* pMsg)
{
	static bool bTeaming = false;	//是否正在组队
	switch(pMsg->GetType())
	{
	case MSG_S2C_TEAM_MemberPos:
		{

			long bufSize = pMsg->GetLong();
			if(bufSize==0)
				return;
			BYTE* pData = new BYTE[bufSize];
			// 			vector<BYTE> v_buf(bufSize);
			pMsg->GetEx(pData,bufSize);
// 			HSM_OneTM_Info* pInfo = m_pMainPlayer->GetOneTeammateInfo();
// 			long pos = 0;
// 			long size = _GetLongFromByteArray(pData,pos);
// 			for(long i =0;i<size;i++)
// 			{
// 				CGUID playerGuid;
// 				_GetBufferFromByteArray(pData,pos,playerGuid);
// 				long x = _GetLongFromByteArray(pData,pos);
// 				long y = _GetLongFromByteArray(pData,pos);
// 
// 				if( pInfo->find(playerGuid) != pInfo->end())
// 				{
// 					((*pInfo)[playerGuid]).fPosX = x;
// 					((*pInfo)[playerGuid]).fPosY = y;
// 				}
// 			}
			SAFE_DELETE_ARRAY(pData);
		}
		break;
		//招募
	case MSG_S2C_TEAM_RECRUIT:
		{
			CGUID PlayerGuiid;
			pMsg->GetGUID(PlayerGuiid);
			long lFlag = pMsg->GetLong();
			if( lFlag == 0)
				return;

			CPlayer* pPlayer = (CPlayer*)m_pClientObjMgr->FindObject(TYPE_PLAYER,PlayerGuiid);
			if( pPlayer == NULL )
				return;
			char strName[20];
			pMsg->GetStr(strName,20);
			long lPass = pMsg->GetLong();
// 			pPlayer->SetRecruitingTeam(true);
// 			pPlayer->SetTeamname(strName);
// 			pPlayer->SetTeamHasPassword( lPass ==0? false:true);
		}
		break;
		//更新招募人数
	case MSG_S2C_TEAM_RECRUITED_NUM:
		{
			CGUID playerGuid;
			pMsg->GetGUID(playerGuid);
			long lNum = pMsg->GetLong();
			CPlayer* pPlayer =(CPlayer*)m_pClientObjMgr->FindObject(TYPE_PLAYER,playerGuid);
			if( pPlayer == NULL )
				return;

//			pPlayer->SetTeammateAmount(lNum);
		}
		break;
	case MSG_S2C_TEAM_MastterChange:
		{

			CGUID playerGuid;
			pMsg->GetGUID(playerGuid);
//			m_pMainPlayer->SetCaptainID(playerGuid);
		}
		break;
		//物品分配 
	case MSG_S2C_TEAM_GoodsSetChange:
		{
			long lAllotMOde = pMsg->GetLong();
		}
		break;
		//玩家是否在线的状态改变
	case MSG_S2C_TEAM_MemberStateChange:
		{
			CGUID guid;
			pMsg->GetGUID(guid);
			if( guid == m_pMainPlayer->GetExID() )
				return;
			long lState = pMsg->GetLong();
//			HSM_OneTM_Info* pInfo = m_pMainPlayer->GetOneTeammateInfo();
// 			if( pInfo->find(guid) != pInfo->end())
// 			{
// 				((*pInfo)[guid]).bOnline = lState ==0?false:true;
//			}
		}
		break;
	case MSG_S2C_TEAM_TeamCreate:			// 创建队友
		{
			CGUID teamGuid;
			pMsg->GetGUID(teamGuid);
			CGUID captainGuid;
			pMsg->GetGUID(captainGuid);
			long lGoodsAllot = pMsg->GetLong();
			long lMemNum = pMsg->GetLong();

// 			HSM_OneTM_Info* pList = m_pMainPlayer->GetOneTeammateInfo();
// 			pList->clear();
// 
// 			for(int i=0;i<lMemNum;i++)
// 			{
// 				tagWSMemberInfo mi;
// 				pMsg->GetEx(&mi,sizeof(tagWSMemberInfo));
// 				CPlayer* pPlayer = (CPlayer*)m_pClientObjMgr->FindObject(TYPE_PLAYER,mi.guid);
// 				if(pPlayer)
// 				{
// 					pPlayer->SetTeamID(teamGuid);
// 					pPlayer->SetCaptainID(captainGuid);
// 					if( captainGuid == mi.guid )
// 						pPlayer->SetIsTeamLeader(true);
// 					else
// 						pPlayer->SetIsTeamLeader(false);
// 
// 					pPlayer->SetTeammateAmount(lMemNum);
// 				}
// 				tagOneTeammateInfo oneInfo;
// 				oneInfo.PlayerGuid = mi.guid;
// 				// 				oneInfo.lOccu = mi.lOccupation;
// 				// 				oneInfo.lSex = mi.lSex;
// 				oneInfo.bOnline = (mi.lState==0?false:true);
// 				strcpy(oneInfo.szName,mi.szName);
// 				(*pList)[mi.guid] = oneInfo;
//			}
		}
		break;
	case MSG_S2C_TEAM_TeamData:				// 所有队伍数据到成员
		{
			CGUID teamGuid;
			pMsg->GetGUID(teamGuid);
			CGUID captainGuid;
			pMsg->GetGUID(captainGuid);
			long lAllot = pMsg->GetLong();
			long lMemNum = pMsg->GetLong();

// 			HSM_OneTM_Info* pData = m_pMainPlayer->GetOneTeammateInfo();
// 			for(int i=0;i<lMemNum;i++)
// 			{
// 				tagWSMemberInfo mi;
// 				pMsg->GetEx(&mi,sizeof(tagWSMemberInfo));
// 				CPlayer* pPlayer = (CPlayer*)m_pClientObjMgr->FindObject(TYPE_PLAYER,mi.guid);
// 				if(pPlayer)
// 				{
// 					pPlayer->SetTeamID(teamGuid);
// 					pPlayer->SetCaptainID(captainGuid);
// 					if( captainGuid == mi.guid )
// 						pPlayer->SetIsTeamLeader(true);
// 					else
// 						pPlayer->SetIsTeamLeader(false);
// 					pPlayer->SetTeammateAmount(lMemNum);
// 				}
// 				tagOneTeammateInfo oneInfo;
// 				oneInfo.PlayerGuid = mi.guid;
// 				// 				oneInfo.lOccu = mi.lOccupation;
// 				// 				oneInfo.lSex = mi.lSex;
// 				oneInfo.bOnline = (mi.lState==0?false:true);
// 				strcpy(oneInfo.szName,mi.szName);
// 				(*pData)[mi.guid] = oneInfo;
// 			}
// 			long lTeamQuestID = pMsg->GetLong();
// 			m_pMainPlayer->SetTeamQuestID(lTeamQuestID);
		}
		break;
	case MSG_S2C_TEAM_MemberAllIdioinfo:// 队伍成员的详细信息
		{
			CGUID playerGuid;
			pMsg->GetGUID(playerGuid);
			if( playerGuid == m_pMainPlayer->GetExID())
				return;

			long lFlag = pMsg->GetLong();
			tagMemberIdioinfo mi;
			if(lFlag)
			{
				pMsg->GetEx(&mi,sizeof(tagMemberIdioinfo));
			}
			//--------------------------------------------
// 			HSM_OneTM_Info* pList = m_pMainPlayer->GetOneTeammateInfo();
// 			if(pList)
// 			{
// 				if(pList->find(playerGuid) != pList->end())
// 				{
// 					// 					((*pList)[playerGuid]).dwMaxHp = mi.lMaxHp;
// 					// 					((*pList)[playerGuid]).dwMaxMp = mi.lMaxMp;
// 					// 					((*pList)[playerGuid]).dwMaxRp = mi.lMaxEnergy;
// 					// 					((*pList)[playerGuid]).dwHp = mi.lCurrHp;
// 					// 					((*pList)[playerGuid]).dwMp = mi.lCurrMp;
// 					// 					((*pList)[playerGuid]).dwRp = mi.lCurrEnergy;
// 					// 					((*pList)[playerGuid]).lLevel = mi.lLevel;
// 					((*pList)[playerGuid]).fPosX = mi.fPosX;
// 					((*pList)[playerGuid]).fPosY = mi.fPosY;
// 				}
// 			}
		}
		break;
	case MSG_S2C_TEAM_UpdateIdioinfo:	// 更新某个队友的某个属性
		{
			CGUID PlayerGuid;
			pMsg->GetGUID(PlayerGuid);
			long lType = pMsg->GetLong();
			if(lType==eMIS_Pos)
			{
				float x = pMsg->GetFloat();
				float y = pMsg->GetFloat();
//				HSM_OneTM_Info *pInfo = m_pMainPlayer->GetOneTeammateInfo();
// 				if( pInfo->find(PlayerGuid) != pInfo->end() )
// 				{
// 					((*pInfo)[PlayerGuid]).fPosX = x;
// 					((*pInfo)[PlayerGuid]).fPosY = y;
// 				}
			}
		}
		break;
		//	有新玩家加入队伍后
	case MSG_S2C_TEAM_MemberData:
		{
			tagWSMemberInfo mi;
			pMsg->GetEx(&mi,sizeof(tagWSMemberInfo));
			if(mi.guid == m_pMainPlayer->GetExID())
				return;

			CGUID playerGuid = mi.guid;
// 			HSM_OneTM_Info* pMap = m_pMainPlayer->GetOneTeammateInfo();
// 			{
// 				tagOneTeammateInfo oneInfo;
// 				oneInfo.PlayerGuid = mi.guid;
// 				//				oneInfo.lOccu = mi.lOccupation;
// 				//				oneInfo.lSex = mi.lSex;
// 				oneInfo.bOnline = (mi.lState==0?false:true);
// 				strcpy(oneInfo.szName,mi.szName);
// 				(*pMap)[mi.guid] = oneInfo;
// 			}
// 			m_pMainPlayer->SetTeammateAmount(m_pMainPlayer->GetTeammateAmount()+1);
		}
		break;
	case MSG_S2C_TEAM_MemberLeaveHere:
		{
			CGUID guid;
			pMsg->GetGUID(guid);

// 			HSM_OneTM_Info * pMap = m_pMainPlayer->GetOneTeammateInfo();
// 
// 			if(m_pMainPlayer->GetExID() == guid)
// 			{
// 				long lSize = pMap->size();
// 				HSM_OneTM_Info::iterator it = pMap->begin();
// 				for(;it != pMap->end();it++)
// 				{
// 					//					it->second.dwHp = 0;
// 					//					it->second.dwMp = 0;
// 					//					it->second.dwRp = 0;
// 				}
// 			}
// 			else
// 			{
// 				if(pMap->find(guid) != pMap->end())
// 				{
// 					//					(*pMap)[guid].dwHp = 0;
// 					//					(*pMap)[guid].dwMp = 0;
// 					//					(*pMap)[guid].dwRp = 0;
// 				}
// 			}
		}
		break;
	case MSG_S2C_TEAM_MemberLeave:
		{
			CGUID playerGuid;
			pMsg->GetGUID(playerGuid);
			//自己退队
			if(playerGuid == m_pMainPlayer->GetExID())
			{
// 				m_pMainPlayer->SetTeamID(CGUID::GUID_INVALID);
// 				m_pMainPlayer->SetCaptainID(CGUID::GUID_INVALID);
// 				m_pMainPlayer->GetOneTeammateInfo()->clear();
// 				m_pMainPlayer->SetTeammateAmount(1);
			}
			else
			{
// 				HSM_OneTM_Info* pMap = m_pMainPlayer->GetOneTeammateInfo();
// 				if( pMap->find(playerGuid) != pMap->end())
// 				{
// 					pMap->erase(playerGuid);
// 					m_pMainPlayer->SetTeammateAmount(m_pMainPlayer->GetTeammateAmount() - 1 );
// 					CPlayer* pPlayer = (CPlayer*)m_pClientObjMgr->FindObject(TYPE_PLAYER,playerGuid);
// 					if(pPlayer)
// 					{
// 						pPlayer->SetTeamID(CGUID::GUID_INVALID);
// 						pPlayer->SetCaptainID(CGUID::GUID_INVALID);
// 						pPlayer->SetTeammateAmount(1);
// 					}
// 				}
			}
		}
		break;
	case MSG_S2C_TEAM_MembereExStateChange:	// 队友异常状态改变
		{
			CGUID playerGuid;
			pMsg->GetGUID(playerGuid);
			if( playerGuid == m_pMainPlayer->GetExID())
				return;

			DWORD dwFlag = pMsg->GetDWord();
			DWORD dwStateID = pMsg->GetDWord();
			DWORD dwLevel = pMsg->GetDWord();

// 			HSM_OneTM_Info* pMap = m_pMainPlayer->GetOneTeammateInfo();
// 			if(dwFlag && pMap)
// 			{
// 				if(pMap->find(playerGuid) != pMap->end())
// 				{
// 					//					((*pMap)[playerGuid]).lLevel = dwLevel;
// 				}
// 			}
		}
		break;
		// 邀请方发出请求
	case MSG_S2C_TEAM_QUERY_JOIN:
		{
			CGUID SelfGuid,RequesterGuid;
			pMsg->GetGUID(SelfGuid);
			pMsg->GetGUID(RequesterGuid);

			CPlayer* pRequestPlayer = (CPlayer*)m_pClientObjMgr->FindObject(TYPE_PLAYER,RequesterGuid);
			char name[256];
			string strName = pMsg->GetStr(name,256);

//			m_pMainPlayer->SetTeamRequesterGuid(RequesterGuid);
			pMsg->Reset();
		}
		break;
		// 	case MSG_S2C_TEAM_ANSWER_JOIN:
		// 		{
		// 			CGUID AimGuid,SorGuid;
		// 			pMsg->GetGUID(AimGuid);
		// 			pMsg->GetGUID(SorGuid);
		// 			CPlayer* pAimPlyaer = (CPlayer*)m_pRegion->FindChildObject(TYPE_PLAYER,AimGuid);
		// 			if( pAimPlyaer != NULL )
		// 				return;
		// 			char name[256];
		// 			strcpy(name,pAimPlyaer->GetName());
		// 			BYTE b = pMsg->GetByte();
		// 		}
		// 		break;
		// 	case MSG_S2C_TEAM_SYNCHRONIZE:
		// 		{
		// 			for(int i=0;i<4;i++)
		// 			{
		// 				pMsg->GetLong();
		// 			}
		// 			CGUID teamGuid,captionGuid;
		// 			pMsg->GetGUID(teamGuid);
		// 			char teamName[64],pass[64];
		// 			pMsg->GetStr(teamName,23);
		// 			pMsg->GetStr(pass,9);
		// 			pMsg->GetGUID(captionGuid);
		// 			DWORD dwAmount = pMsg->GetDWord();
		// 
		// 			m_pMainPlayer->GetOneTeammateInfo()->clear();
		// 			m_pMainPlayer->SetTeamID(teamGuid);
		// 			m_pMainPlayer->SetCaptainID(captionGuid);
		// 
		// 			CPlayer::HSM_OneTM_Info* pMap = m_pMainPlayer->GetOneTeammateInfo();
		// 
		// 			for(int i=0;i<dwAmount;i++)
		// 			{
		// 				pMsg->GetLong();
		// 				long lType = pMsg->GetLong();
		// 				CGUID guid;
		// 				pMsg->GetGUID(guid);
		// 				pMsg->GetLong();
		// 
		// 				CGUID regionGuid;
		// 				pMsg->GetGUID(regionGuid);
		// 				char strTeammate[32];
		// 				pMsg->GetStr(strTeammate,17);
		// 
		// 				if( pMap->find(guid) != pMap->end() )
		// 				{
		// //					((*pMap)[guid]).RegionGuid = regionGuid;
		// 					strcpy(((*pMap)[guid]).szName,strTeammate);
		// 				}
		// 			}
		// 		}
		// 		break;
		// 	case MSG_S2C_TEAM_TEAMATE_JOIN:
		// 		{
		// 			CGUID teamGuid;
		// 			pMsg->GetGUID(teamGuid);
		// 			long lType = pMsg->GetLong();
		// 			CGUID playerGuid;
		// 			pMsg->GetGUID(playerGuid);
		// 			CGUID regionGuid;
		// 			pMsg->GetGUID(regionGuid);
		// 			float fHpPercent = pMsg->GetFloat();
		// 			char strName[32];
		// 			pMsg->GetStr(strName,17);
		// 
		// 			CPlayer::HSM_OneTM_Info* pInfo = m_pMainPlayer->GetOneTeammateInfo();
		// 			if( pInfo->find(playerGuid) != pInfo->end())
		// 			{
		// 				CPlayer::tagOneTeammateInfo tmInfo;
		// 				tmInfo.PlayerGuid = playerGuid;
		// //				tmInfo.dwHp = (WORD)(fHpPercent*100);
		// //				tmInfo.dwMaxHp = 100;
		// 				strcpy(tmInfo.szName,strName);
		// //				tmInfo.RegionGuid = regionGuid;
		// 
		// 				m_pMainPlayer->SetTeamID(teamGuid);
		// 				(*pInfo)[playerGuid] = tmInfo;
		// 			}
		// 		}
		// 		break;
		// 	case MSG_S2C_TEAM_CHANGE_STATE:
		// 		{
		// 			CGUID TeamID;
		// 			pMsg->GetGUID(TeamID);
		// 			long lType=pMsg->GetLong();
		// 			CGUID PlayerID;
		// 			pMsg->GetGUID(PlayerID);
		// 			float fHp=pMsg->GetFloat();
		// 			CPlayer::HSM_OneTM_Info* pInfo = m_pMainPlayer->GetOneTeammateInfo();
		// 			{
		// 				if(pInfo->find(PlayerID) != pInfo->end())
		// 				{
		// //					(*pInfo)[PlayerID].dwHp=(WORD)(100*fHp);
		// //					(*pInfo)[PlayerID].dwMaxHp=100;
		// 					break;
		// 				}
		// 			}
		// 		}
		// 		break;
		// 	case MSG_S2C_TEAM_TEAMATE_CHANGE_REGION:
		// 		{
		// 			CGUID TeamID,PlayerID,RegionID;
		// 			pMsg->GetGUID(TeamID);
		// 			long lType=pMsg->GetLong();
		// 			pMsg->GetGUID(PlayerID);
		// 			pMsg->GetGUID(RegionID);
		// 			CPlayer::HSM_OneTM_Info* pInfo = m_pMainPlayer->GetOneTeammateInfo();
		// 			{
		// 				if(pInfo->find(PlayerID) != pInfo->end())
		// 				{
		// //					(*pInfo)[PlayerID].RegionGuid = RegionID;
		// 					break;
		// 				}
		// 			}
		// 		}
		// 		break;
		// 	case MSG_S2C_TEAM_CHANGE_LEAER:
		// 		{
		// 			CGUID TeamID,LeaderID;
		// 			pMsg->GetGUID(TeamID);
		// 			m_pMainPlayer->SetCaptainID(LeaderID);
		// 		}
		// 		break;
	}

}*/
//添加组队消息
void CGameClient::S2CTeam(CMessage* msg)
{
	long lType = msg->GetType();
	switch( lType )
	{
	case MSG_S2C_TEAM_UpdateIdioinfo:
		break;
	case MSG_S2C_TEAM_SKILL_ABOUT:
		break;
	case MSG_S2C_TEAM_TeamCreate:			//创建的时候初始化得到
		TeamCreat(msg);		
		break;
	case MSG_S2C_TEAM_TeamData:				//所有队伍数据到成员
		TeamData(msg);
		break;
	case MSG_S2C_TEAM_MemberData:			//一个成员的队伍数据
		TeamMemberData(msg);
		break;
	case MSG_S2C_TEAM_MemberLeave:			//! 成员离队
		TeamMemberLeave(msg);		
		break;
	case MSG_S2C_TEAM_MastterChange:		//! 队长改变
		TeamMastterChange(msg);				
		break;
	case MSG_S2C_TEAM_RECRUIT:				//! 更新招募状态
		TeamRecruit(msg);
		break;
	case MSG_S2C_TEAM_RECRUITED_NUM:		//! 更新已招募的人数
		TeamRecruitedNum(msg);
		break;
		// 	case MSG_S2C_TEAM_ANSWER_JOIN:			//回复邀请加入
		// 		TeamAnswerJoin(msg);
		// 		break;
		// 	case MSG_S2C_TEAM_SYNCHRONIZE:			//同步队伍的信息
		// 		TeamSynchronize(msg);
		// 		break;
		// 	case MSG_S2C_TEAM_TEAMATE_CHANGE_REGION:
		// 		TeamMatChangeRegion(msg);
		// 		break;
		// 	case MSG_S2C_TEAM_CHANGE_LEAER:			 //改变队长
		// 		TeamChangeLeader(msg);
		// 		break;
		// 	case MSG_S2C_TEAM_CHANGE_STATE:
		// 		TeamChangeState(msg);
		// 		break;
	case MSG_S2C_TEAM_QUERY_JOIN:
		this->TeamQueryJion(msg);
		break;
	case MSG_S2C_TEAM_MemberAllIdioinfo:
		this->TeamMemAllInfo(msg);
	default:
		break;
	}
}
void CGameClient::TeamCreat(CMessage *msg)
{
	CGUID TeamID;
	msg->GetGUID(TeamID);				//队伍id
	CGUID LeaderId;							
	msg->GetGUID(LeaderId);
	long lGoodsAllot  = msg->GetLong();
	long lMemberCount = msg->GetLong();

	for(long i=0; i<lMemberCount; ++i)
	{
		tagWSMemberInfo sMemberInfo;
		msg->GetEx(&sMemberInfo, sizeof(tagWSMemberInfo));
		CPlayer *pPlayer = (CPlayer *)m_pClientObjMgr->FindObject(TYPE_PLAYER, sMemberInfo.guid);
		if(pPlayer)// 该队友在场景内可见
		{
//			pPlayer->SetTeamID(TeamID);
// 			pPlayer->SetCaptainID(LeaderId);
// 			pPlayer->SetTeammateAmount(lMemberCount);

		}
//		m_pMainPlayer->GetTeamMateInfo().push_back(sMemberInfo);
		//Lua下逻辑消息发送判断接口
		if (LeaderId == m_pMainPlayer->GetExID())			//如果当前玩家为队长
		{
//			m_pMainPlayer->SetIsTeamLeader(true);
		}	
	}
}
void CGameClient::TeamData(CMessage *msg)
{
	CGUID TeamID;
	msg->GetGUID(TeamID);
	CGUID LeaderId;
	msg->GetGUID(LeaderId);
	long lGoodsAllot = msg->GetLong();
	long lMemberCount = msg->GetLong();

//	m_pMainPlayer->GetTeamMateInfo().clear();
	for(long i=0; i<lMemberCount; ++i)
	{
		tagWSMemberInfo sMemberInfo;
		msg->GetEx(&sMemberInfo, sizeof(tagWSMemberInfo));
		//CPlayer *pPlayer = (CPlayer*)m_pMainPlayer->FindChildObject(TYPE_PLAYER, sMemberInfo.guid);
		//  [12/25/2008 Square]
		CPlayer* pPlayer = (CPlayer*)m_pClientObjMgr->FindObject(TYPE_PLAYER,sMemberInfo.guid);
		if(pPlayer)// 该队友在场景内可见
		{
//			pPlayer->SetTeamID(TeamID);
// 			pPlayer->SetCaptainID(LeaderId);
// 			pPlayer->SetTeammateAmount(lMemberCount);
		}

//		m_pMainPlayer->GetTeamMateInfo().push_back(sMemberInfo);
	}

	if (LeaderId == m_pMainPlayer->GetExID())			//如果当前玩家为队长
	{
//		m_pMainPlayer->SetIsTeamLeader(true);
	}	
}

void CGameClient::TeamMemberData(CMessage *msg)
{
	tagWSMemberInfo sMemberInfo;
	msg->GetEx(&sMemberInfo, sizeof(tagWSMemberInfo));
	// 本人
	if(m_pMainPlayer->GetExID() == sMemberInfo.guid)
		return;
//	m_pMainPlayer->GetTeamMateInfo().push_back(sMemberInfo);
//	m_pMainPlayer->SetTeammateAmount(m_pMainPlayer->GetTeammateAmount()+1);
}

void CGameClient::TeamMemberLeave(CMessage *msg)
{
	CGUID PlayerID;
	msg->GetGUID(PlayerID);

	// 自己退队
	if(PlayerID == m_pMainPlayer->GetExID())
	{
//		m_pMainPlayer->SetTeamID(CGUID::GUID_INVALID);
//		m_pMainPlayer->SetCaptainID(CGUID::GUID_INVALID);
//		m_pMainPlayer->GetTeam().clear();
//		m_pMainPlayer->GetTeamMateInfo().clear();
//		m_pMainPlayer->SetTeammateAmount(1);
		//Lua下逻辑消息发送判断接口
//		if (m_pMainPlayer->IsTeamLeader())					//如果当前玩家为队长
		{
//			m_pMainPlayer->SetIsTeamLeader(false);
		}

	}
	else
	{
//		vector<tagWSMemberInfo>::iterator it = m_pMainPlayer->GetTeamMateInfo().begin();
// 		for( ; it!=m_pMainPlayer->GetTeamMateInfo().end(); ++it)
// 		{
// 			if(it->guid == PlayerID)
// 			{
// 				m_pMainPlayer->GetTeamMateInfo().erase(it);
// 				m_pMainPlayer->SetTeammateAmount(m_pMainPlayer->GetTeammateAmount()-1);
// 				CPlayer *pPlayer = (CPlayer*)m_pClientObjMgr->FindObject(TYPE_PLAYER,PlayerID);
// 				if(pPlayer)
// 				{
// 					pPlayer->SetTeamID(CGUID::GUID_INVALID);
// 					pPlayer->SetCaptainID(CGUID::GUID_INVALID);
// 					pPlayer->SetTeammateAmount(1);
// 				}
// 				break;
// 			}
// 		}
	}
}

void CGameClient::TeamMastterChange(CMessage *msg)
{
	CGUID PlayerID;
	msg->GetGUID(PlayerID);

// 	m_pMainPlayer->SetCaptainID(PlayerID);
// 	//Lua下逻辑消息发送判断接口
// 	m_pMainPlayer->SetIsTeamLeader(false);
// 	CPlayer *pPlayer = (CPlayer*)m_pClientObjMgr->FindObject(TYPE_PLAYER, PlayerID);
// 	if (pPlayer)
// 	{
// 		pPlayer->SetIsTeamLeader(true);
// 	}
}

void CGameClient::TeamRecruit(CMessage* msg)
{
	// 招募信息发起者的ID
	CGUID PlayerID;
	msg->GetGUID(PlayerID);
	long lFlag = msg->GetLong();

	//  [12/25/2008 Square]
	CPlayer* pPlayer = (CPlayer*)m_pClientObjMgr->FindObject(TYPE_PLAYER,PlayerID);
	//
	if(NULL == pPlayer)
	{
		return ;
	}
	// 取消招募
	if(lFlag==0)
	{
// 		pPlayer->SetRecruitingTeam(false);
// 		pPlayer->SetTeammateAmount(1);
		return;				
	}
	char strName[20];
	msg->GetStr(strName,20);
	long lPass = msg->GetLong();
//	pPlayer->SetRecruitingTeam(true);
// 	pPlayer->SetTeamname(strName);
// 	pPlayer->SetTeamHasPassword(lPass==0 ? false : true);

	if (PlayerID == m_pMainPlayer->GetExID())			//如果当前玩家为队长
	{
//		m_pMainPlayer->SetIsTeamLeader(true);
	}	
}

void CGameClient::TeamRecruitedNum(CMessage* msg)
{
	CGUID PlayerID;
	msg->GetGUID(PlayerID);
	long lNum = msg->GetLong();     // 该玩家当前队伍人数

	//  [12/25/2008 Square]
	CPlayer* pPlayer = (CPlayer*)m_pClientObjMgr->FindObject(TYPE_PLAYER,PlayerID);
	if(NULL == pPlayer)	
	{
		return ;
	}
//	pPlayer->SetTeammateAmount(lNum);
}

void CGameClient::TeamAnswerJoin(CMessage* msg)
{
	CGUID IDDst, IDScr;
	msg->GetGUID(IDDst);
	msg->GetGUID(IDScr);					// 邀请方ID

	CPlayer * pPlayerDst = dynamic_cast<CPlayer*>(m_pClientObjMgr->FindObject(TYPE_PLAYER, IDDst));

	if(NULL == pPlayerDst ) 
	{
		return ;
	}
	char name[255];
	strcpy(name,pPlayerDst->GetName());
	BYTE b = msg->GetByte();
}

void CGameClient::TeamSynchronize(CMessage* msg)
{
	for(int i=0; i!=4; ++i)
		msg->GetLong();
	CGUID TeamID, LeaderID;
	msg->GetGUID(TeamID);
	char strTeamname[64];
	char strPassword[64];
	msg->GetStr(strTeamname, 23);
	msg->GetStr(strPassword, 9);
	msg->GetGUID(LeaderID);
	DWORD dwAmount = msg->GetDWord();

//	m_pMainPlayer->GetTeam().clear();
//	m_pMainPlayer->SetTeamID(TeamID);
//	m_pMainPlayer->SetCaptainID(LeaderID);

	for(int i=0; i != dwAmount; ++i)
	{
		//
		msg->GetLong();
		long lType = msg->GetLong();
		CGUID ID;
		msg->GetGUID(ID);
		msg->GetLong();
		//
		CGUID RegionID;
		msg->GetGUID(RegionID);
		char strTeammate[32];
		msg->GetStr(strTeammate,17);
		//
		tagTeammate sTeam;
		sTeam.PlayerGuid	   = ID;
		//		sTeam.RegionID = RegionID;
		//		sTeam.strName  = strTeammate;
//		m_pMainPlayer->GetTeam().push_back(sTeam);
	}
}

void CGameClient::TeamMatChangeRegion(CMessage* msg)
{
	CGUID TeamID, PlayerID, RegionID;
	msg->GetGUID(TeamID);
	long lType=msg->GetLong();
	msg->GetGUID(PlayerID);
	msg->GetGUID(RegionID);
//	vector<tagTeammate>::iterator it = m_pMainPlayer->GetTeam().begin();
// 	for( ; it != m_pMainPlayer->GetTeam().end(); ++it)
// 	{
// 		if(it->PlayerGuid == PlayerID)
// 		{
// 			//			it->RegionID = RegionID;
// 			break;
// 		}
// 	}
}

void CGameClient::TeamChangeLeader(CMessage* msg)
{
	CGUID TeamID, LeaderID;
	msg->GetGUID(TeamID);
	msg->GetGUID(LeaderID);
//	m_pMainPlayer->SetCaptainID(LeaderID);
// 	vector<tagTeammate>::iterator it = m_pMainPlayer->GetTeam().begin();
// 	for( ; it != m_pMainPlayer->GetTeam().end(); ++it)
// 	{
// 		if(it->PlayerGuid == LeaderID)
// 		{
// 			if(m_pMainPlayer->GetTeam().begin() != it)
// 			{
// 				tagTeammate sTeam = (*it);
// 				m_pMainPlayer->GetTeam().erase(it);
// 				m_pMainPlayer->GetTeam().insert(m_pMainPlayer->GetTeam().begin(), sTeam);
// 			}
// 			break;
// 		}
// 	}
// 	//Lua下逻辑消息发送判断接口
// 	m_pMainPlayer->SetIsTeamLeader(false);
// 	CPlayer *pPlayer = (CPlayer*)m_pClientObjMgr->FindObject(TYPE_PLAYER, LeaderID);
// 	if (pPlayer)
// 	{
// 		pPlayer->SetIsTeamLeader(true);
// 	}
}

void CGameClient::TeamChangeState(CMessage* msg)
{
	CGUID TeamID;
	msg->GetGUID(TeamID);
	long lType = msg->GetLong();
	CGUID PlayerID;
	msg->GetGUID(PlayerID);
	float fHp = msg->GetFloat();
	vector<tagTeammate>::iterator it;
// 	for(it = m_pMainPlayer->GetTeam().begin(); it!=m_pMainPlayer->GetTeam().end(); ++it)
// 	{
// 		if(it->PlayerGuid == PlayerID)
// 		{
// 			//			it->wHP	   =(WORD)(100*fHp);
// 			//			it->wMaxHP =100;
// 			break;
// 		}
// 	}
}

void CGameClient::TeamQueryJion(CMessage *msg)
{
	CGUID PlayerID,IDSrc;
	// 弹出提示
	msg->GetGUID(PlayerID);		// 本人ID
	msg->GetGUID(IDSrc);		// 邀请方ID

	CPlayer * pPlayerSrc = dynamic_cast<CPlayer*>(m_pClientObjMgr->FindObject(TYPE_PLAYER, IDSrc));
	char name[255];
	string strName = msg->GetStr(name,255);

	/*m_pMainPlayer->SetTeamRequesterGuid(IDSrc);*/

	CBaseMessage* _msg = CBaseMessage::AllocMsg();
	_msg->Reset();
	_msg->Init(MSG_C2S_TEAM_ANSWER_JOIN);
	CMessage* pMsg = (CMessage*)_msg;

	pMsg->Add(PlayerID);
	pMsg->Add(IDSrc);
	pMsg->Add((BYTE)1);
	pMsg->SendBySocket(msg->GetSocketID());
	CBaseMessage::FreeMsg(pMsg);
}

void CGameClient::TeamMemAllInfo(CMessage* msg)
{	
	CGUID PlayerID;
	msg->GetGUID(PlayerID);

	// 自身信息不用显示
	if(PlayerID == m_pMainPlayer->GetExID())
		return;
	long lFlag = msg->GetLong();
	tagMemberIdioinfo sMemberInfo;
	if(lFlag)
	{
		msg->GetEx(&sMemberInfo, sizeof(tagMemberIdioinfo));
	}
}
//
//********************************************************************
void CGameClient::S2CGoods(CMessage* msg)
{
	long lType = msg->GetType();
	switch( lType )
	{
	case MSG_S2C_PICKGOOD:
		break;
	case MSG_S2C_EXCHANGEHOTKEY:
		{
			int result=msg->GetChar();
			int nIndex=msg->GetByte();
			int extend = 0;
			int pos = 0;
			if(result==HOTKEY_SET_SUCCEED && nIndex>=0 && nIndex<24)
			{
				DWORD dwGoodIndex = msg->GetDWord();
				if (m_pMainPlayer!=NULL)
				{
//					DWORD dwHotKey = m_pMainPlayer->GetHotKey(nIndex);
//					m_pMainPlayer->SetHotKey(nIndex,dwGoodIndex);
				}
			}
		}
		break;
	case MSG_S2C_CLEARHOTKEY:
		{
			int result=msg->GetChar();
			int nIndex=msg->GetByte();
			if(result==HOTKEY_CLEAR_SUCCEED && nIndex>=0 && nIndex<24)
			{
				int extend = 0;
				int pos = 0;
//				DWORD dwHotKey = m_pMainPlayer->GetHotKey(nIndex);
//				if (dwHotKey!=0)
				{
//					if ((dwHotKey & 0x80000000)==0)
//					{
						//pHand->SetMainPlayerHandByGoodsIndex(pPlayer->GetHotKey(nIndex),CMainPlayerHand::PlaceType_Fastfield);
//					}
//					else
//					{
						//DWORD wSkillID = dwHotKey&0x7fffffff;
						//CPlayer::tagSkill *pSkill = pPlayer->GetSkill(wSkillID);
						//if(pSkill)
						//{
						//	// 技能图标类型（灰度/正常）
						//	WORD wType = CPicList::PT_SKILL_ICON_GRAY;
						//	// 职业符合，使用彩色图标
						//	if(CSkillListXml::OccuJudgeStudy(wSkillID,pPlayer->GetOccupation()))
						//		wType = CPicList::PT_SKILL_ICON;
						//	pHand->SetMainPlayerHandBySkill(wSkillID,wType,CMainPlayerHand::PlaceType_Fastfield);
						//}
						//else
						//	pHand->SetMainPlayerHandBySkill(0);
//					}
				}
//				m_pMainPlayer->SetHotKey(nIndex,0);
			}
		}
		break;
	case MSG_S2C_SETHOTKEY:
		{
			int result=msg->GetChar();
			int nIndex=msg->GetByte();
			DWORD dwVal=msg->GetDWord();
			if(nIndex>=0 && nIndex<24)
			{
//				m_pMainPlayer->SetHotKey(nIndex, dwVal);
			}
		}
		break;
	case MSG_S2C_DROPGOOD:
		{
			int nRet = msg->GetChar();
			if( nRet == GOODS_DROP_SUCCEED)
			{
				DWORD lNum = msg->GetDWord();
//				DWORD oldNum = m_pMainPlayer->GetPlayerMoneyNum();
//				if( lNum>=1 && lNum<= oldNum)
				{
	//				m_pMainPlayer->SetPlayerSilverMoneyNum(oldNum-lNum);
				}
			}
		}
		break;
	case MSG_S2C_PICKMONEY:
		{
			int ret = msg->GetChar();
			if( ret == GOODS_PICK_SUCCEED )
			{
				DWORD lNum = msg->GetDWord();
//				DWORD oldNum = m_pMainPlayer->GetPlayerMoneyNum();
//				if(lNum>=0&&oldNum + lNum <= MAX_MONEY)
				{
//					m_pMainPlayer->SetPlayerSilverMoneyNum(oldNum+lNum);
				}
			}
		}
		break;
	case MSG_S2C_EQUIPDAMAGE:
		{
			int nPOs = msg->GetByte();
			CGoods* pEquip = m_pMainPlayer->GetGoodsMgr()->GetEquipMgr()->GetEquip(nPOs);
			if(pEquip)
			{
				SAFE_DELETE( pEquip);
				m_pMainPlayer->GetGoodsMgr()->GetEquipMgr()->SetEquip(nPOs,NULL);
			}
		}
		break;
	case MSG_S2C_SET_YUANBAO:
		{
			DWORD dwBalance = msg->GetDWord();
//			m_pMainPlayer->SetPlayerYuanBaoNum(dwBalance);
		}
		break;
	case MSG_S2C_SET_TICKET:
		{
			DWORD dwTicket = msg->GetDWord();
//			m_pMainPlayer->SetPlayerJiFenNum(dwTicket);
		}
		break;
	case MSG_S2C_EQUIPDURUPDATE:
		{
			int nPOs= msg->GetByte();
			DWORD dwVal = msg->GetDWord();
			CGoods* pEquip = m_pMainPlayer->GetGoodsMgr()->GetEquipMgr()->GetEquip(nPOs);
			if(pEquip)
			{
				long lMaxDurValue = pEquip->GetEffectVal(CGoods::GAP_GOODS_MAXIMUM_DURABILITY,1);
				int nPercent = 100;
				if(lMaxDurValue>0)
				{
					nPercent = dwVal*100/lMaxDurValue;
				}
				pEquip->SetEffectVal(CGoods::GAP_GOODS_MAXIMUM_DURABILITY,2,dwVal);
			}
		}
		break;
	case MSG_S2C_EQUIP_REFINE_UPDATE:
		{
			BYTE nPos = msg->GetByte();
			DWORD dwVal = msg->GetDWord();
			CGoods* pEquip = m_pMainPlayer->GetGoodsMgr()->GetEquipMgr()->GetEquip(nPos);
			if(pEquip)
			{
				pEquip->SetEffectVal(CGoods::GAP_REFINE,1,dwVal);
			}
		}
		break;
	case MSG_S2C_DELETEEQUIP:
		{
			CGUID guid;
			msg->GetGUID(guid);
			int npos = msg->GetByte();
			if(guid == m_pMainPlayer->GetExID())
			{
				CGoods* pEquip = m_pMainPlayer->GetGoodsMgr()->GetEquipMgr()->GetEquip(npos);
				if(pEquip)
				{
					SAFE_DELETE(pEquip);
					m_pMainPlayer->GetGoodsMgr()->GetEquipMgr()->SetEquip(npos,NULL);
				}
			}
			else
			{
				CPlayer* pPlayer = (CPlayer*)m_pClientObjMgr->FindObject(TYPE_PLAYER,guid);
				if(pPlayer)
				{
					CEquipMgr* pMgr = pPlayer->GetGoodsMgr()->GetEquipMgr();
					if( pMgr )
					{
						pMgr->SetEquipIndex(npos,NULL);
						pMgr->SetEquipLevel(npos,NULL);
					}
				}
			}
		}
		break;
	case MSG_S2C_DELETEGOODS:
		{
			BYTE lPos = msg->GetByte();
			BYTE lNum = msg->GetByte();
			list<tagGoods>::iterator it = m_pMainPlayer->GetGoodsMgr()->GetPacketMgr()->GetGoodsList()->begin();
			for(;it != m_pMainPlayer->GetGoodsMgr()->GetPacketMgr()->GetGoodsList()->end();it++)
			{
				if( it->lPos == lPos)
				{
					int nXSize= 1;
					int nYSize = 1;
					int nTmepPos = it->lPos;
					for(int i=0;i<nYSize;i++)
					{
						nTmepPos += 8;
					}
					it->lNum -= lNum;
					break;
				}
			}
		}
		break;
	case MSG_S2C_DELETEALLGOODS:
		{
			BYTE lPos = msg->GetByte();
			list<tagGoods>* pGoodsList = m_pMainPlayer->GetGoodsMgr()->GetPacketMgr()->GetGoodsList();
			if( pGoodsList )
			{
				list<tagGoods>::iterator it = pGoodsList->begin();
				for(;it != pGoodsList->end();it++)
				{
					if( it->lPos == lPos)
					{
						SAFE_DELETE(it->pGoods);
						pGoodsList->erase(it);
						break;
					}
				}
			}
		}
		break;
	case MSG_S2C_VIEWEQUIP:
		break;
	case MSG_S2C_OPENGOODSUPGRADE:
		break;
	case MSG_S2C_GOODSUPGRADE_UPGRADE:
		{
			CGoods* pUpgradeGoods = NULL;
			if( pUpgradeGoods )
			{
				;
			}
		}
		break;
	case MSG_S2C_GOODS_UPDATEPROPERTY:
		{
			CGUID playerGuid;
			msg->GetGUID(playerGuid);
			CGUID goodsGuid;
			msg->GetGUID(goodsGuid);

			CPlayer*pPlayer = (CPlayer*)m_pClientObjMgr->FindObject(TYPE_PLAYER,playerGuid);
			if(pPlayer)
			{
				CGoods*pGoods = pPlayer->GetGoodsMgr()->QueryGoodsByGuid(goodsGuid);
				if(pGoods)
				{
					long lDataSize = msg->GetLong();
					BYTE* pData = new BYTE[lDataSize];
					msg->GetEx(pData,lDataSize);
					long lPos = 0;
					pGoods->DecordFromByteArray(pData,lPos,false);
					SAFE_DELETE_ARRAY(pData);
				}
			}
		}
		break;
	case MSG_S2C_OPENGOODSENCHASE:
		this->OpenGoodsEnchase(msg);
		break;
	case MSG_S2C_ENCHASE_ARRANGE:
		break;
	case MSG_S2C_ENCHASE_OPEN_HOLE:
		{
			BYTE holeNum = msg->GetByte();
		}
		break;
	case MSG_S2C_ENCHASE_CLOSE_HOLE:
		break;
	case MSG_S2C_ENCHASE_ENCHASECARD:
		break;
	case MSG_S2C_ENCHASE_REMOVECARD:
		break;
	case MSG_S2C_GOODS_OPEN_CONTAINER:
		break;
	case MSG_S2C_GOODS_OPEN_WINDOWS:
		this->OpenGoodsHandleWindow(msg);
		break;
		//物品分解的最终结果
	case MSG_S2C_GOODS_RE_DISASSEMBLE_DO:
		break;
		//物品合成的结果
	case MSG_S2C_GOODS_RE_SYNTHESIZE_DO:
		{
			if( eGTC_Succeed == msg->GetLong())
			{
				long lNum = msg->GetLong();
				for(long c = 0;c<lNum;c++)
				{
					tagGoods stGoods;
					ZeroMemory(&stGoods,sizeof(stGoods));
					stGoods.lNum = msg->GetLong();
					CGUID goodsGuid;
					msg->GetGUID(goodsGuid);
					stGoods.pGoods = m_pMainPlayer->GetGoodsMgr()->QueryGoodsByGuid(goodsGuid,(int*)&stGoods.lPos,(long*)&stGoods.lType);
				}
			}
		}
		break;
	case MSG_S2C_GOODS_BIND:
		{
			break;
		}
	case MSG_S2C_DEPOT_RE_INPUT_PWD:
		this->AnswerDepotPwdInput(msg);
		break;
	case MSG_S2C_DEPOT_RE_CLOSE:
		this->OrderDepotClose(msg);
		break;
	case MSG_S2C_MEDAL_ADD_NOTIFY:
		{
			long GoodsIndex = msg->GetLong();
			m_pMainPlayer->GetMedalMgr()->AddMedal(GoodsIndex);
		}
	default:
		break;
	}
}

void CGameClient::OpenGoodsHandleWindow(CMessage* pMsg)
{
	long enumWndType		= pMsg->GetLong();
	long lPlayerMoveRange	= pMsg->GetLong();
	long lOpenedTileX		= pMsg->GetLong();
	long lOpenedTileY		= pMsg->GetLong();

	if( enumWndType >=0 && enumWndType < PT_Number )
	{
		SetCurOpendWindowsType(enumWndType);
	}
}
void CGameClient::OpenGoodsEnchase(CMessage* msg)
{
	CGUID SessionGuid;
	BYTE bPageType = msg->GetByte();
	msg->GetGUID(SessionGuid);
	long lPlayerID = msg->GetLong();

	if( bPageType>=0 && bPageType < CardEnchasePage_COUNT )
	{
		SetEnchasePageType(bPageType);
		SetEnchaseSessionGuid(SessionGuid);
		SetEnchasePlayerID(lPlayerID);
	}
}
void CGameClient::AnswerDepotPwdInput(CMessage *msg)
{
	//密码正确
	if( eDMC_Succeed == msg->GetLong() )
	{
		//设置通行证
		CDepotMgr* pDepotMgr = m_pMainPlayer->GetGoodsMgr()->GetDepotMgr();
		if( pDepotMgr == NULL )	return;

		pDepotMgr->SetDepotHasPwd(true);

		DBReadSet db_r;
		msg->GetDBReadSet(db_r);

		pDepotMgr->DecordDepotInfo(db_r);

		SetCurOpenPageType(PT_DepotWindow);
	}
}
void CGameClient::OrderDepotClose(CMessage* msg)
{
	char str[128];
	sprintf(str,"SetOpenDepot(%d,false);",msg->GetSocketID());
	CScriptSys::getInstance()->PerformString(str);
}


void CGameClient::AnswerNpcList(CMessage* msg)
{
	long lNpcNum = msg->GetLong();
	long lLen = msg->GetLong();
	BYTE* str = new BYTE[lLen+1];
	msg->GetEx(str,lLen);
	//
	long pos = 0;
	for( int i=0;i<lNpcNum;i++)
	{
		CClientRegion::tagNpc *Npc = new CClientRegion::tagNpc;
		_GetStringFromByteArray(str,pos,Npc->strName);
		Npc->lPosX = _GetDwordFromByteArray(str,pos);
		Npc->lPosY = _GetDwordFromByteArray(str,pos);
		m_pRegion->GetNpcList()->push_back(Npc);
		//		m_pRegion->GetMapNpcList()->insert(make_pair(Npc->strName,Npc));
	}

	//delete [] str;
	SAFE_DELETE_ARRAY(str);
}
// void CGameClient::S2CShape(CMessage* pMsg)
// {
// 	long lType = pMsg->GetType();
// 	switch( lType )
// 	{
// 	case MSG_S2C_SHAPE_MOVE:
// 		this->ShapeMove(pMsg);
// 		break;
// 	case MSG_S2C_SHAPE_SETPOS:
// 		this->ShapeSetPos(pMsg);
// 		break;
// 	case MSG_S2C_SHAPE_ATK_DIED:
// 		this->ShapeAtkDied(pMsg);
// 		break;
// 	case MSG_S2C_SHAPE_CHANGE_PROPERTIES:
// 		this->OnShapeUpdatePropertyMsg(pMsg);
// 		break;
// 	case MSG_S2C_SHAPE_ATK_BREAK:
// 		this->BeAtk(pMsg);
// 		break;
// 	case MSG_S2C_SHAPE_ATK_DAMAGE:
// 		this->ShapeAtkDamage(pMsg);
// 		break;
// 	case MSG_S2C_SHAPE_STOPMOVE:
// 		this->ShapeMoveStop(pMsg);
// 		break;
// 	default:
// 		break;
// 	}
// }
void CGameClient::S2CPlayer(CMessage* pMsg)
{
	long lType = pMsg->GetType();
	switch( lType)
	{
	case MSG_S2C_PLAYER_ADDSKILL:
		this->PlayerAddSkill(pMsg);
		break;
	case MSG_S2C_PLAYER_ANSWER_RELIVE:
		this->PlayerAnswerRelive(pMsg);
		break;
	case MSG_S2C_PLAYER_LEVELUP:
		this->PlayerLevelUp(pMsg);
		break;
	case MSG_S2C_PLAYER_CHANGE_PROPERTIES:
		this->PlayerUpdateProperty(pMsg);
		break;
	//case MSG_S2C_PLAYER_TRADEREQUEST:
	//	this->PlayerTradeRequest(pMsg);
	//	break;
	//case MSG_S2C_PLAYER_TRADEBEGIN:
	//	this->PlayerTradeBegin(pMsg);
	//	break;
	//case MSG_S2C_PLAYER_TRADELOCK:
	//	this->PlayerTradeLock(pMsg);
	//	break;
//	case MSG_S2C_PLAYER_WAITFORTRADE:
//		{
//			long flag = pMsg->GetLong();
//			if( pMsg->GetByte() == 1 )
//			{
////				m_pMainPlayer->SetbYouIsLocked(true);
//			}
//			else
//			{
//			//	m_pMainPlayer->SetbYouIsLocked(false);
//			//	m_pMainPlayer->SetbMeIsLocked(false);
//			}
//		}
//		break;
//	case MSG_S2C_PLAYER_TRADECANCEL:
//	case MSG_S2C_PLAYER_TRADERESULT:
//		{
////			m_pMainPlayer->SetTradeSessionID(CGUID::GUID_INVALID);
////			m_pMainPlayer->SetTradeRequestGUID(CGUID::GUID_INVALID);
////			m_pMainPlayer->SetbMeIsLocked(false);
////			m_pMainPlayer->SetbYouIsLocked(false);
////			m_pMainPlayer->SetlMPlugID(0);
////			m_pMainPlayer->SetlYPlugID(0);
////			m_pMainPlayer->SetCanTrade(false);
//			//			m_pMainPlayer->ClearTradeGoods();
//		}
//		break;
	case MSG_S2C_PLAYER_USEITEM:
		this->PlayerUseItem(pMsg);
		break;
	default:
		break;
	}
}

void CGameClient::PlayerUpdateProperty(CMessage* pMsg)
{
	CGUID guid;
	CGoods* pGoods = m_pMainPlayer->GetGoodsMgr()->QueryGoodsByGuid(guid);
}

//玩家使用消耗性物品后返回的S2C消息即处理结果,仅作调试用
void CGameClient::PlayerUseItem(CMessage* pMsg)
{
	int result=pMsg->GetChar();//result为49表示成功,50为失败
	//	m_pMainPlayer->SetUseItemResult(result);
	CGUID playerguid;
	pMsg->GetGUID(playerguid);//获取玩家的GUID
	//	m_pMainPlayer->SetUseItemPlayerGuid(playerguid);
	DWORD dwGoodsIndex = pMsg->GetDWord();//获取物品的物编编号
	//	m_pMainPlayer->SetUseItemGoodsIndex(dwGoodsIndex);
}

void CGameClient::PlayerLevelUp(CMessage* pMsg)
{
	CGUID guid;
	pMsg->GetGUID(guid);
	DWORD dwMaxHp=pMsg->GetDWord();
	WORD wMaxMp=pMsg->GetWord();
	WORD wLevel=pMsg->GetWord();
	DWORD dwExp=pMsg->GetDWord();
	DWORD dwUpExp=pMsg->GetDWord();
	DWORD dwPresentExp = pMsg->GetDWord();

	CPlayer* pPlayer =  (CPlayer*)m_pClientObjMgr->FindObject(TYPE_PLAYER,guid);
	if( pPlayer && pPlayer == m_pMainPlayer )
	{
//		pPlayer->SetMaxHp(dwMaxHp);
//		pPlayer->SetMaxMp(wMaxMp);

		pPlayer->SetHp(dwMaxHp);
//		pPlayer->SetMp(wMaxMp);

//		pPlayer->SetPresentExp(dwPresentExp);
//		pPlayer->SetLevel(wLevel);
	}
}
// void CGameClient::S2CRegion(CMessage* pMsg)
// {
// 	long lType = pMsg->GetType();
// 	switch( lType )
// 	{
// 	case MSG_S2C_RGN_ADDSHAPE:
// 		//	新加对象
// 		this->RegionAddShape(pMsg);
// 		break;
// 	case MSG_S2C_RGN_DELSHAPE:
// 		//	移出对象
// 		this->RegionDelShape(pMsg);
// 		break;
// 	case MSG_S2C_RGN_CHANGE:
// 		//	切换场景
// 		this->RegionChanged(pMsg);
// 		break;
// 	case MSG_S2C_RGN_CHANGESERVER:
// 		//	切换服务器
// 		this->RegionChangeServer(pMsg);
// 		break;
// 	default:
// 		break;
// 	}
// }

void CGameClient::S2CShop(CMessage* pMsg)
{
	long lType = pMsg->GetType();
	switch(lType)
	{
	case MSG_S2C_SHOP_OPEN:
		this->ShopOpen(pMsg);
		break;
	case MSG_S2C_BUSINESS_BUY:
	case MSG_S2C_SHOP_BUY_BAT:
		this->ShopBuy(pMsg);
		break;
	default:
		break;
	}
}

//打开NPC商店后服务器返回的消息
void CGameClient::ShopOpen(CMessage* pMsg)
{
	CGUID npcguid;
	pMsg->GetGUID(npcguid);				//获取NPC商店的NPCGUID
	long ShopType = pMsg->GetLong();	//设置当前的SHOP的类型
	BYTE CoinType = pMsg->GetByte();	//设置当前的货币的类型
	float BasePriceRate = pMsg->GetFloat();		//设置当前打开SHOP的基本折价率
	float TradeInRate = pMsg->GetFloat();	//设置当前打开SHOP的折价率
	DWORD dwnum = pMsg->GetDWord();		//NPC商店中物品的数量

//	m_pMainPlayer->SetShopNPCGuid(npcguid);
//	m_pMainPlayer->SetGoodsNum(dwnum);

// 	DWORD GoodsID = 0;
// 	DWORD num = 0;
// 	for(long i=0;i<dwnum;i++)
// 	{			
// 		GoodsID = pMsg->GetDWord();
// 		m_pMainPlayer->AddShopGoodsID(GoodsID);
// 		num = pMsg->GetDWord();//(区别叠加物品的种类的数量)
// 		m_pMainPlayer->AddEveryGoodsNum(num);
// 	}

}

//void CGameClient::OnShopGoodsChangeGoods(CMessage*pMsg)
//{
//	if (m_pMainPlayer !=NULL)
//	{
//		//扣掉玩家身上的一些物品
//		DWORD dwGoodsTypeNum = pMsg->GetDWord();	//要扣除物品种类的个数	
//		for(int i=0;i<dwGoodsTypeNum;i++)
//		{
//			DWORD dwContainerNum = pMsg->GetDWord();	//每种类型的物品所在的容器的个数
//			for(int j=0;j<dwContainerNum;j++)
//			{
//				DWORD dwContainerID = pMsg->GetDWord();	//每个容器的ID
//				DWORD dwGoodsPosNum = pMsg->GetDWord();	//要处理的位置个数
//				for(int k=0;k<dwGoodsPosNum;k++)
//				{
//					DWORD dwGoodsPos = pMsg->GetDWord();
//					DWORD dwGoodsNum = pMsg->GetDWord();
//					CGoods* pDelGoods = m_pMainPlayer->GetGoodsByPosition(dwContainerID,dwGoodsPos);
//					if (pDelGoods!=NULL)
//					{
//						if (dwGoodsNum==pDelGoods->GetNum())
//						{
//							CGUID guid = pDelGoods->GetExID();
//							m_pMainPlayer->RemoveGoods(guid,CGUID::GUID_INVALID,dwGoodsPos,dwContainerID,dwGoodsNum);
//						}else if (pDelGoods->GetNum()>dwGoodsNum)
//						{
//							CGUID guid = pDelGoods->GetExID();
//							m_pMainPlayer->ChangeObjectAmount(dwContainerID,TYPE_GOODS,guid,pDelGoods->GetNum()-dwGoodsNum);
//						}
//					}
//				}
//			}
//		}
//		//添加一些物品到玩家身上
//		DWORD dwTotalGoodsNum = pMsg->GetDWord();
//		DWORD dwDataSize = pMsg->GetDWord();
//		BYTE* pData = new BYTE[dwDataSize];
//		memset(pData,0,dwDataSize);
//		pMsg->GetEx(pData,dwDataSize);
//		long pos = 0;
//		for (int i = 0;i<dwTotalGoodsNum;i++)
//		{
//			BYTE type = _GetCharFromByteArray(pData,pos);
//			if (type==OT_CHANGE_AMOUNT)
//			{
//				DWORD dwContainerID = _GetDwordFromByteArray(pData,pos);
//				DWORD dwGoodsPos = _GetDwordFromByteArray(pData,pos);
//				DWORD dwGoodsNum = _GetDwordFromByteArray(pData,pos);
//				CGoods* pChangeGoods = m_pMainPlayer->GetGoodsByPosition(dwContainerID,dwGoodsPos);
//				if (pChangeGoods!=NULL)
//				{
//					CGUID guid = pChangeGoods->GetExID();
//					DWORD dwOldGoodsNum = pChangeGoods->GetNum();
//					DWORD dwCurGoodsNum = dwOldGoodsNum+dwGoodsNum;
//					m_pMainPlayer->ChangeObjectAmount(dwContainerID,TYPE_GOODS,guid,dwCurGoodsNum);
//
//				}
//			}else if (type==OT_NEW_OBJECT)
//			{
//				DWORD dwContainerID = _GetDwordFromByteArray(pData,pos);
//				CGoods* pNewGoods = new CGoods;
//				pNewGoods->DecordFromByteArray(pData,pos);
//				DWORD dwGoodsPos = _GetDwordFromByteArray(pData,pos);
//				m_pMainPlayer->AddGoods(pNewGoods,dwGoodsPos,dwContainerID);
//			}
//		}
//		SAFE_DELETE_ARRAY(pData);
//	}
//}

//在NPC商店操作中点击购买商品后服务器返回的S2C消息
void CGameClient::ShopBuy(CMessage* pMsg)
{
	DWORD dwMsgType = pMsg->GetType();

	DWORD dwExtendID = 0;
	DWORD dwPos =0;
	if( dwMsgType == MSG_S2C_BUSINESS_BUY)
	{
		dwExtendID = pMsg->GetDWord();
	}

	WORD wNum = pMsg->GetWord();
	BYTE GoodsInfo[102400];
	for(WORD i=0;i<wNum;i++)
	{
		long pos = 0;
		memset(GoodsInfo,0,102400);
		pMsg->GetEx(GoodsInfo,102400);
		CGoods* pGoods = new CGoods;
		pGoods->DecordFromByteArray(GoodsInfo,pos);
		if( dwMsgType != MSG_S2C_BUSINESS_BUY )
		{
			dwExtendID = pMsg->GetDWord();
		}
		dwPos = pMsg->GetDWord();
		m_pMainPlayer->GetGoodsMgr()->AddGoods(pGoods,dwExtendID,dwPos);
	}
}

void CGameClient::OccuskillStudyBegin(CMessage* msg)
{
	CSkillMgr* pSkillMgr = m_pMainPlayer->GetSkillMgr();
	if( pSkillMgr == NULL )	return ;

	pSkillMgr->ClearOccuSkill();	//清空PLAYER中保存可以学习的副职业生产技能的技能ID容器
	CGUID npcguid;
	msg->GetGUID(npcguid);	//npc的GUID

	long num = msg->GetDWord();	//num为所有的生产技能的数目
	for(DWORD i=0; i<num; ++i)
	{
		DWORD dwSkillID = msg->GetDWord();	//技能ID
		DWORD dwSkillCost = msg->GetDWord();	//学习技能所需要花费的金币

		pSkillMgr->AddOccuSkill(dwSkillID);
	}
}

//是否成功的学习到了副职业生产技能
void CGameClient::OccuskillStudySuccess(CMessage* msg)
{
	long lBeSucceed = msg->GetLong();	//是否成功,0为成功
	DWORD dwFactureID = msg->GetDWord();	//生产技能的id
	if (lBeSucceed==1)
	{
//		m_pMainPlayer->SetOccuSkillHaveStudied();
	}
}


void CGameClient::PlayerAnswerRelive(CMessage* msg)
{
	WORD dwAction = msg->GetWord();
	WORD wState =  msg->GetWord();
	DWORD dwHp = msg->GetDWord();
	WORD wMp = msg->GetWord();

	if (m_pMainPlayer->GetState() == CShape::STATE_DIED)
		m_pMainPlayer->SetState(CShape::STATE_PEACE);

	char str[128];
	sprintf(str,"%s:<-------------->复活",m_pMainPlayer->GetName());
	//PutDebugString(str);
	//
	m_pMainPlayer->SetState(wState);
	m_pMainPlayer->SetAction(dwAction);
	m_pMainPlayer->SetHp((DWORD)10000);
//	m_pMainPlayer->SetMp((WORD)10000);
}

void CGameClient::PlayerAddSkill(CMessage* msg)
{
	CGUID guid;
	msg->GetGUID(guid);

	long lSkillID = msg->GetLong();
	WORD wSkillLevel = msg->GetWord();
	DWORD dwRestoreTime = msg->GetDWord();
	DWORD dwRangeMin = msg->GetDWord();
	DWORD dwRangeMax = msg->GetDWord();
	//
	if( m_pMainPlayer->GetExID() == guid)
	{
		CCDKey* pCDKey = CCDKey::GetCDKey(m_strCDkey.c_str());
		if( pCDKey )
		{
			pCDKey->SaveSkill(m_pMainPlayer);
		}
		//m_pMainPlayer->AddSkill(lSkillID,wSkillLevel,dwRestoreTime,dwRangeMin,dwRangeMax);
	}
}

//Region msg
//void CGameClient::RegionChanged(CMessage* msg)
//{
//	long lSocketid = msg->GetSocketID();
//
//	long lType = msg->GetLong();
//	CGUID PlayerGuid;
//	msg->GetGUID(PlayerGuid);
//	long lUseGoods = msg->GetLong();
//	//
//	if( m_pMainPlayer->GetExID() == PlayerGuid )
//	{
//		CGUID RegionGuid;
//		msg->GetGUID(RegionGuid);
//
//		long lRegionID = msg->GetLong();
//		long lPlayerX = msg->GetLong();
//		long lPlayerY = msg->GetLong();
//		long lPlayerDir = msg->GetLong();
//
//		char strRegionName[256] = "";
//		msg->GetStr(strRegionName,256);
//		long lTaxRate = msg->GetLong();
//
//		REGION_TYPE eWarRegionType = (REGION_TYPE)msg->GetByte();
//		BYTE bWaterRegion = msg->GetByte();
//
//		bool bChangeEquip = msg->GetByte() == 0 ?false:true;
//
//		long lResourceID = msg->GetLong();
//		float fExScale = msg->GetFloat();
//
//		//	场景PK类型 0：未开启 1：开启，但不可PK 2：开启并可PK
//		char cPkFlags = msg->GetByte();
//
//		assert(lPlayerX>=0&&lPlayerY>=0);
//
//		//	分线解码
//		{
//			DBReadSet db_read;
//			msg->GetDBReadSet(db_read);
//			this->DecordLineFromDataBlock(db_read);
//		}
//
//		CPlayer* pPlayer = GetMainPlayer();
//
//		// -- 请求进入场景
//		
//		  //@清除原客户端数据
//		if( m_pRegion )
//		{
//			CMapRgnObj* mapObjectList = m_pClientObjMgr->GetObjectList();
//			if( mapObjectList == NULL )	return;
//
//			CMapRgnObj::iterator it = mapObjectList->begin();
////			list<CBaseObject*>::iterator it = m_listClientObject.begin();
//			for(; it != mapObjectList->end();++it)
//			{
//				CMoveShape* pShape = (CMoveShape*)it->second;
//				//	更新引用计数
//				pShape->ReduUsedTimes();
////				m_listClientObject.erase(it++);
//
//				m_pRegion->RemoveObject(pShape);
//
//				if( pShape->GetUsedTimes() == 0 )
//				{
//					if( pShape != m_pMainPlayer )
//					{
//						SAFE_DELETE( pShape );
//					}
//				}
//			}
//			mapObjectList->clear();
//		}
//
//		//	获取一个对应的新场景
//		m_pRegion = CScriptSys::getInstance()->GetTestDialog()->FindClientRegion(lResourceID,RegionGuid);
//		m_pRegion->SetName(strRegionName);
//		//
//		pPlayer->SetPosXY(lPlayerX + 0.5f,lPlayerY + 0.5f);
//		pPlayer->SetRegionID(lRegionID);
//
//		pPlayer->SetDestXY(lPlayerX + 0.5f,lPlayerY + 0.5f);
//		pPlayer->SetAction(CShape::ACT_STAND);
//
//
//		pPlayer->SetFather(m_pRegion);
//
//		this->AddObjectToClientRgn(pPlayer);
////		m_pClientObjMgr->AddObject(pPlayer);
////		m_pRegion->AddObject(pPlayer);
////		AddObject(pPlayer);
//
//		if( m_pRegion )
//		{
//			m_pRegion->SetTaxRate(lTaxRate);
//			m_pRegion->SetWarRegionType(eWarRegionType);
//			m_pRegion->SetExpScale(fExScale);
//		}
//
//		char s[256];
//		memset(s,0,sizeof(s));
//		sprintf(s,"ChangeState(%d,\"%s\")",lSocketid,"run");
//		CScriptSys::getInstance()->PerformString(s);
//	}
//	else
//	{
//		CMoveShape* pShape = (CMoveShape*)m_pClientObjMgr->FindObject(lType,PlayerGuid);
//		if( pShape )
//		{
//			m_pClientObjMgr->RemoveObject(pShape);
//			m_pRegion->RemoveObject(pShape);
////			RemoveObject(pShape);
//			if( pShape->GetUsedTimes() == 0 )
//				SAFE_DELETE(pShape);
//		}
//	}
//}
void CGameClient::DecordLineFromDataBlock(DBReadSet& db_read)
{
	m_vecTemplateID.clear();
	m_vecRegionGuid.clear();
	m_lLineNum = 0;

	BYTE m_nLineNote = db_read.GetByteFromByteArray();
	if( m_nLineNote == 1)
	{
		///*long*/ m_lLineID = db_read.GetLongFromByteArray();
		//long lineNum = db_read.GetLongFromByteArray();
		//for(int i=0;i<lineNum;i++)
		//{
		//	long templateID = db_read.GetLongFromByteArray();
		//	m_vecTemplateID.push_back(templateID);

		//	CGUID regionExGuid;
		//	db_read.GetBufferFromByteArray(regionExGuid);
		//	m_vecRegionGuid.push_back(regionExGuid);
		//}
		//m_lLineNum = lineNum;
	}
}


/*
brief：当前客户端添加一个对象
如果该对象不存在，先在场景对象列表中查找
如果也不存在，则新创建一个对象，添加到客户端对象列表

注意：客户端对象列表存在的对象必然存在于场景对象列表中
*/
//void CGameClient::RegionAddShape(CMessage* msg)
//{
//	long lType = msg->GetLong();
//	CGUID guid;
//	msg->GetGUID(guid);
//
//	DBReadSet db_r;
//	msg->GetDBReadSet(db_r);
//
//	//	客户端对象链表中查找
//	CBaseObject* pObject = m_pClientObjMgr->FindObject(lType,guid);
////	CBaseObject* pObject = FindClientObject(lType,guid);
//	if( pObject )
//	{
//		bool isSelf = false;
//		switch(lType)
//		{
//		case TYPE_PET:
//			{
//				msg->GetLong();	//重新接收宠物类型
//				isSelf = msg->GetChar();	//重新接收是否是自己的
//			}
//			break;
//		}
//
//		pObject->DecordFromDataBlock(db_r,isSelf);
//		CShape* pShape = dynamic_cast<CShape*>(pObject);
//		if(pShape)
//		{
//			CMoveShape* pMoveShape = dynamic_cast<CMoveShape*>(pShape);
//			if(pMoveShape)
//				pMoveShape->SetDestXY(pMoveShape->GetPosX(),pMoveShape->GetPosY());
//		}
//		return;
//	}
//
//	//  新加入对象
//	//	在场景对象链表中查找
//
//	CBaseObject* pShape = m_pRegion->FindChildObject(lType,guid);
//	if( pShape )
//	{
//		this->AddObjectToClientRgn(pShape);
//		//	添加到客户端对象列表
////		m_pClientObjMgr->AddObject(pShape);
//		//	添加到场景
////		m_pRegion->AddObject(pShape);
//	}
//	else
//	{
//		bool bSelf = false;
//		switch(lType)
//		{
//		case TYPE_PLAYER:
//			pShape = new CPlayer;
//			break;
//		case TYPE_MONSTER:
//			pShape = new CMonster;
//			break;
//		case TYPE_GOODS:
//			//return;
//			pShape = new CGoods;
//			bSelf = false;
//			pShape->SetExID(guid);
//			break;
//		case TYPE_COLLECTION:
//			pShape = new CCollectionGoods;
//			break;
//		case TYPE_PET:
//			{
//				long l = db_r.GetLongFromByteArray();
//				switch(l)
//				{
//				case PET_TYPE_ELF:
//					pShape = new CFairy;
//					break;
//				case PET_TYPE_HUNTER:
//					pShape = new CFighterPet;
//					break;
//				case PET_TYPE_CONJURED:
//					pShape = new CSummonPet;
//					break;
//				case PET_TYPE_TRAP:
//					pShape = new CTrap;
//					break;
//				default:
//					break;
//				}
//				bSelf = db_r.GetCharFromByteArray() == 0 ? false:true;
//			}
//			break;
//		}
//
//		if(pShape)
//		{
//			((CMoveShape*)pShape)->SetUsedTimes(0);
//			if(lType == TYPE_PET)
//			{
//				if( bSelf )
//					((CPet*)pShape)->DecodePetFromDataBlock(db_r,true,m_pMainPlayer,m_pRegion);
//				else
//					((CPet*)pShape)->DecodePetFromDataBlock(db_r,false);
//			}
//			else
//			{
//				if( bSelf )
//					pShape->DecordFromDataBlock(db_r,true);
//				else
//					pShape->DecordFromDataBlock(db_r,false);
//			}
//
//			if( lType == TYPE_PET && bSelf )
//			{
//				CPet* pPet = dynamic_cast<CPet*>(pShape);
//				if( pPet )
//				{
//					pPet->SetPetSocketID(msg->GetSocketID());
//					pPet->SetMaster( m_pMainPlayer );
//					pPet->SetMasterGuid(m_pMainPlayer->GetExID());
//					pPet->SetPetRegion( m_pRegion );
//					pPet->EnterRegion((bool)bSelf);
//				}
//			}
//
//			this->AddObjectToClientRgn(pShape);
////			m_pClientObjMgr->AddObject(pShape);
////			m_pRegion->AddObject(pShape);
//
//			if( pShape->GetType() == TYPE_MONSTER )
//			{
//				CMonster* pMonster = dynamic_cast<CMonster*>(pShape);
//				if(pMonster)
//				{
//					pMonster->SetBlock(pMonster->GetTileX(),pMonster->GetTileY(),pMonster->GetBlockType());
//				}
//			}
//		}
//	}
//}

//void CGameClient::RegionDelShape(CMessage* msg)
//{
//	long lType = msg->GetLong();
//	CGUID guid;
//	msg->GetGUID(guid);
//
//	long lUseGoods = msg->GetLong();
//
//	//For DriveClient
//	//************************************************************
//	if ( guid == m_pMainPlayer->GetSelectedShapeGuid())
//	{
//		m_pMainPlayer->SetSelectedShapeGuid(CGUID::GUID_INVALID);
//		return ;
//	}
//	//*************************************************************
//	CBaseObject* pObject = m_pClientObjMgr->FindObject(lType,guid);
//	if( pObject )
//	{
//		if( pObject != m_pMainPlayer )
//		{
//			m_pClientObjMgr->RemoveObject(pObject);
//			m_pRegion->RemoveObject(pObject);
//		}
//	}
//}

//void CGameClient::RegionChangeServer(CMessage* msg)
//{
//	//获取验证码
//	long lChangeServerCode = msg->GetLong();
//	long lSocketID = msg->GetSocketID();
//	char strIP[256];
//	msg->GetStr(strIP,256);
//	DWORD dwPort = msg->GetDWord();
//
//	char s[256];
//	memset(s,0,sizeof(s));
//	sprintf(s,"S2CChangeServer(%d,\"%s\",%d,%d)",lSocketID,strIP,dwPort,lChangeServerCode);
//
//	CScriptSys::getInstance()->PerformString(s);
//}

//客户端请求交易后,服务器返回给另一个客户端的消息,可以获得向自己请求交易的玩家的GUID
void CGameClient::PlayerTradeRequest(CMessage* msg)
{
	CGUID PlayerID;
	msg->GetGUID(PlayerID);
//	m_pMainPlayer->SetTradeRequestGUID(PlayerID);
}

//当客户端同意交易请求后,服务器发送给交易双方的交易开始的S2C消息
void CGameClient::PlayerTradeBegin(CMessage* msg)
{
	CGUID SessionID,RequestPlayerID,ResponsePlayerID;
	msg->GetGUID(SessionID);	//获得交易会话ID
	msg->GetGUID(RequestPlayerID);		//获得邀请方玩家的ID
	long lRequestPlugID = msg->GetLong();		//获得邀请方玩家的PLUGID
	msg->GetGUID(ResponsePlayerID);		//获得响应方玩家的ID
	long lResponsePlugID = msg->GetLong();		//获得响应方玩家的PLUGID


// 	m_pMainPlayer->SetTradeSessionID(SessionID);
// 	if(m_pMainPlayer->GetExID() == RequestPlayerID)
// 	{
// 		m_pMainPlayer->SetMyID(RequestPlayerID);
// 		m_pMainPlayer->SetYourID(ResponsePlayerID);
// 		m_pMainPlayer->SetlMPlugID(lRequestPlugID);
// 		m_pMainPlayer->SetlYPlugID(lResponsePlugID);
// 	}
// 	else if(m_pMainPlayer->GetExID() == ResponsePlayerID)
// 	{
// 		m_pMainPlayer->SetMyID(ResponsePlayerID);
// 		m_pMainPlayer->SetYourID(RequestPlayerID);
// 		m_pMainPlayer->SetlMPlugID(lResponsePlugID);
// 		m_pMainPlayer->SetlYPlugID(lRequestPlugID);
// 	}
}

//对方锁定后,会返回给自己一个S2C消息
void CGameClient::PlayerTradeLock(CMessage* msg)
{
//	m_pMainPlayer->SetbYouIsLocked(true);
}

//-------------------------------------------------------------------------------------------------------
//物品合成接口（新加）
//by tanhaowe 2008.11.26
//-------------------------------------------------------------------------------------------------------

void CGameClient::SetSynthesizeGoods(tagGoods *pstGoods, int index)
{	
	if ((index<0) || (index>MAX_STUFF_NUM+5))
	{
		return ; 
	}
	m_tagSynthesizeGoods.arrGoodsGuid[index]	 = pstGoods->pGoods->GetExID();
	m_tagSynthesizeGoods.arrContainerFlag[index] = pstGoods->lType;
	m_tagSynthesizeGoods.arrNum[index]			 = pstGoods->lNum;
	m_tagSynthesizeGoods.arrPos[index]			 = pstGoods->lPos;
}

void CGameClient::ClearSynthesizeGoods()
{
	for (int i=0; i!=MAX_STUFF_NUM+5; ++i)
	{
		m_tagSynthesizeGoods.arrGoodsGuid[i]	 = CGUID::GUID_INVALID;
		m_tagSynthesizeGoods.arrContainerFlag[i] = 0;
		m_tagSynthesizeGoods.arrPos[i]		     = 0;
		m_tagSynthesizeGoods.arrNum[i]		   	 = 0;
	}
}

//合成材料
void CGameClient::SettagEvilStuff(tagGoods *taggoods)
{
	m_tagEvilStuff.GoodsGuid		= taggoods->pGoods->GetExID();
	m_tagEvilStuff.lContainerFlag	= taggoods->lType;
	m_tagEvilStuff.lPos				= taggoods->lPos;
	m_tagEvilStuff.lUseNum			= taggoods->lNum;
}

void CGameClient::CleartagEvilStuff()
{
	m_tagEvilStuff.GoodsGuid		= CGUID::GUID_INVALID;
	m_tagEvilStuff.lContainerFlag	= 0;
	m_tagEvilStuff.lPos				= 0;
	m_tagEvilStuff.lUseNum			= 0;
}

//-------------------------------------------------------------------------
//  [12/17/2008 Square]
//	玩家商店消息
//-------------------------------------------------------------------------
// void CGameClient::OnPlayerShopToBuyer(CMessage* pMsg)
// {
// 	long lExtendID = -1;
// 	DWORD dwPos =0;
// 	DWORD dwMoney = 0;
// 	DWORD dwYuanBao = 0;
// 	DWORD dwNumber = pMsg->GetDWord();
// 	for(int i=0;i<dwNumber;i++)
// 	{
// 		CGoods* pGoods = NULL;
// 		BYTE bType = pMsg->GetByte();
// 		if(bType == OT_NEW_OBJECT)
// 		{
// 			pGoods = new CGoods();
// 			BYTE ByteData[102400];
// 			long lBytePos=0;
// 			pMsg->GetEx(ByteData,102400);
// 			pGoods->DecordFromByteArray(ByteData,lBytePos,false);
// 			lExtendID = pMsg->GetLong();
// 			dwPos = pMsg->GetDWord();
// 			m_pMainPlayer->AddGoods(pGoods,dwPos,lExtendID);
// 		}
// 		else if(OT_CHANGE_AMOUNT)
// 		{
// 			DWORD dwAddNum = pMsg->GetDWord();
// 			lExtendID = pMsg->GetLong();
// 			dwPos = pMsg->GetDWord();
// 			pGoods = m_pMainPlayer->GetGoodsByPosition(lExtendID,dwPos);
// 			if(pGoods!=NULL)
// 			{
// 				CGUID guid = pGoods->GetExID();
// 				m_pMainPlayer->ChangeObjectAmount(lExtendID,TYPE_GOODS,guid,pGoods->GetNum()+dwAddNum);
// 			}
// 		}
// 	}
// 
// 	dwMoney = pMsg->GetDWord();
// 	dwYuanBao = pMsg->GetDWord();
// 	m_pMainPlayer->SetPlayerMoneyNum(dwMoney);
// 	m_pMainPlayer->SetPlayerYuanBaoNum(dwYuanBao);
// }

//-------------------------------------------------------------------------------------------------------
// 家族 行会[12/22/2008 Square]
void CGameClient::OnFamilyMessage(CMessage* pMsg)
{
	switch(pMsg->GetType())
	{
	case MSG_S2C_Quest_Add:
		{
			DWORD dwQuestID = pMsg->GetDWord();
			long lStep = pMsg->GetLong();

//			m_pMainPlayer->AddOneRoleRecord(dwQuestID,lStep);
		}
		break;
	case MSG_S2C_Faction_ApplyBoardOpen_Notify:
		{	
			CGUID m_guidFacApply;
			// 清空之前的数据
// 			hash_map<CGUID,FamilyInfo,hash_guid_compare> *hmapfuc = m_pMainPlayer->GethmapFamily();
// 			hmapfuc->clear();
// 
// 			pMsg->GetLong();
// 			pMsg->GetGUID( m_guidFacApply );
// 			// 行会数量
// 			long	numFactions = pMsg->GetLong();
// 
// 			// 获取新行会列表
// 			for( long c = 0; c < numFactions; c++ )
// 			{
// 				tagApplyInfo	stFacInfo;
// 				pMsg->GetEx( &stFacInfo, sizeof(tagApplyInfo) );
// 				FamilyInfo info;
// 				info.FamilyGuid = stFacInfo.guid;
// 				strcpy(info.strName,stFacInfo.szName);
// 				info.arrFunLevel[FUN_FACTION] = stFacInfo.lLevel;
// 
// 				m_pMainPlayer->AddFamily(&info);
//			}
 		}
		break;
	case MSG_S2C_ORGSYS_InitOrgSys_Notify:
		{
			return;
			long lDataSize = pMsg->GetLong();
			LPBYTE pData = new BYTE[lDataSize+1];
			pMsg->GetEx(pData,lDataSize);
			long pos = 0;
			DecordFamilySetup(pData,pos);

			long crcNew = _GetLongFromByteArray(pData,pos);
			SAFE_DELETE_ARRAY(pData);
		}
		break;
	case MSG_S2C_ORGSYS_CreateFaction_Notify:
		{
			pMsg->GetLong();
		}
		break;
	case MSG_S2C_ORGSYS_CreateFaction_Re:
		{
			if(pMsg->GetLong() == PLAYER_OPT_SUCCEED)
			{
//				m_pMainPlayer->SetFamilyJobLev(Exceptive_JobLvl);
			}
		}
		break;
//	case MSG_S2C_ORGSYS_UpgradeFaction_Notify:
//		{
//			long lNewLevel = pMsg->GetLong();
//			ENUM_FUN eFun = (ENUM_FUN)pMsg->GetDWord();
////			FamilyInfo* familyInfo = m_pMainPlayer->GetFamilyInfoByGuid(m_pMainPlayer->GetFamilyGuid());
////			familyInfo->arrFunLevel[eFun] = lNewLevel;
//		}
//		break;
	//case MSG_S2C_ORGSYS_UpgradeFaction_Re:
	//	{
	//		ENUM_FUN eFun = (ENUM_FUN)pMsg->GetDWord();
	//	}
	//	break;
	case MSG_S2C_ORGSYS_DisbandFaction_Notify:
		{
			//解散帮会的guid
			CGUID familyGuid;
			pMsg->GetGUID(familyGuid);

			CMapRgnObj* mapObject = m_pClientObjMgr->GetObjectList();
			if( mapObject == NULL )	return;

			CMapRgnObj::iterator it = mapObject->begin();

			for(;it != mapObject->end();++it)
			{
				if(it->second->GetType() == TYPE_PLAYER)
				{
					if(CPlayer* pPlayer = dynamic_cast<CPlayer*>(it->second))
					{
//						if( pPlayer->GetFamilyGuid() == familyGuid )
// 						{
// 							pPlayer->SetFamilyGuid(CGUID::GUID_INVALID);
// 							pPlayer->SetUnionID(CGUID::GUID_INVALID);
// 							pPlayer->SetFamilyName("");
// 							pPlayer->SetFamilyTitle("");
// 						}
					}
				}
			}

//			m_pMainPlayer->DelFamilyByGuid(familyGuid);
		}
		break;
	case MSG_S2C_Faction_PublicData_Notify:
		{
			tagFacPublicData data;
			pMsg->GetEx(&data,sizeof(data));
		}
		break;

    case MSG_S2C_ANSWER_PLAYERRANKS:
        {
            // rank info
            CGUID guid;
            pMsg->GetGUID(guid);
        }
        break;
	}
}
void CGameClient::DecordFamilySetup(BYTE*pData,long& lPos)
{
	//全局配置
	tagGlobalSetup globeSetup;
	_GetBufferFromByteArray(pData,lPos,&globeSetup,sizeof(globeSetup));
	//相关的等级参数配置
	long lLevelSize = _GetLongFromByteArray(pData,lPos);
	for(long i=0;i<lLevelSize;i++)
	{
		long lLevel			= _GetLongFromByteArray(pData,lPos);
		long lMaxMemNum		= _GetLongFromByteArray(pData,lPos);
		long lConditionSize	= _GetLongFromByteArray(pData,lPos);
		for(long j=0;j<lConditionSize;++j)
		{
			tagUpgradeCondition cond;
			_GetBufferFromByteArray(pData,lPos,&cond,sizeof(tagUpgradeCondition));
		}
	}
	//创建联盟条件
	long lConditionSize = _GetLongFromByteArray(pData,lPos);
	for(long j=0;j<lConditionSize;j++)
	{
		tagUpgradeCondition con;
		_GetBufferFromByteArray(pData,lPos,&con,sizeof(tagUpgradeCondition));
	}
	//公会功能等级配置
	long arrFamLevel[MAXNUM_FactionPurview];
	_GetBufferFromByteArray(pData,lPos,&arrFamLevel,sizeof(long)*MAXNUM_FactionPurview);

	//帮会经验奖励等级的配置
	lLevelSize = _GetLongFromByteArray(pData,lPos);
	for(long i=0;i<lLevelSize;i++)
	{
		long lLevel			= _GetLongFromByteArray(pData,lPos);
		long fBaseExp			= _GetLongFromByteArray(pData,lPos);
		long lConditionSize	= _GetLongFromByteArray(pData,lPos);
		for(long j=0;j<lConditionSize;++j)
		{
			tagUpgradeCondition con;
			_GetBufferFromByteArray(pData,lPos,&con,sizeof(tagUpgradeCondition));
		}
	}

	//! 帮会仓库等级的配置
	lLevelSize = _GetLongFromByteArray(pData,lPos);
	for(long i=0;i<lLevelSize;i++)
	{
		long lLevel			= _GetLongFromByteArray(pData,lPos);
		long lCellNum			= _GetLongFromByteArray(pData,lPos);
		long lConditionSize	= _GetLongFromByteArray(pData,lPos);
		for(long j=0;j<lConditionSize;j++)
		{
			tagUpgradeCondition con;
			_GetBufferFromByteArray(pData,lPos,&con,sizeof(tagUpgradeCondition));
		}
	}
	//! 锻造等级配置
	lLevelSize = _GetLongFromByteArray(pData,lPos);
	for (LONG i = 0; i < lLevelSize; ++i)
	{
		long lLevel				= _GetLongFromByteArray(pData, lPos);
		LONG lConditionSize		= _GetLongFromByteArray(pData, lPos);
		for (LONG j = 0; j < lConditionSize; ++j)
		{
			tagUpgradeCondition UpgradeCondition;
			_GetBufferFromByteArray(pData, lPos, &UpgradeCondition, sizeof(tagUpgradeCondition));
		}
	}
	//! 炼金等级配置
	lLevelSize = _GetLongFromByteArray(pData, lPos);
	for (LONG i = 0; i < lLevelSize; ++i)
	{
		long lLevel					= _GetLongFromByteArray(pData, lPos);
		LONG lConditionSize			= _GetLongFromByteArray(pData, lPos);
		for (LONG j = 0; j < lConditionSize; ++j)
		{
			tagUpgradeCondition UpgradeCondition;
			_GetBufferFromByteArray(pData, lPos, &UpgradeCondition, sizeof(tagUpgradeCondition));
		}
	}
	//! 缝纫等级配置
	lLevelSize = _GetLongFromByteArray(pData, lPos);
	for (LONG i = 0; i < lLevelSize; ++i)
	{
		long lLevel				= _GetLongFromByteArray(pData, lPos);
		LONG lConditionSize		= _GetLongFromByteArray(pData, lPos);
		for (LONG j = 0; j < lConditionSize; ++j)
		{
			tagUpgradeCondition UpgradeCondition;
			_GetBufferFromByteArray(pData, lPos, &UpgradeCondition, sizeof(tagUpgradeCondition));
		}
	}
}

void CGameClient::OnPetMessage(CMessage* pMsg)
{
	//switch(pMsg->GetType())
	//{
	//case MSG_S2C_PET_CHANGE_MASTER:
	//case MSG_S2C_PET_CHANGE_MODE:
	//	break;
	//case MSG_S2C_PET_AI:
	//	{
	//		CGUID PetGuid;
	//		pMsg->GetGUID(PetGuid);

	//		long lOperType = pMsg->GetLong();
	//		CPet* pPet = dynamic_cast<CPet*>(m_pClientObjMgr->FindObject(TYPE_PET,PetGuid));
	//		if( pPet == NULL)
	//			return;
	//		switch(lOperType)
	//		{
	//		case PET_OT_AI_SET_TARGET:		//自动寻敌  (主动攻击模式)
	//			{
	//				long lAimType = pMsg->GetLong();
	//				CGUID aimGuid;
	//				pMsg->GetGUID(aimGuid);
	//				if( lAimType ==0 || m_pRegion == NULL )
	//				{
	//					pPet->SetCurAtkShape(NULL);
	//					return;
	//				}

	//				CMoveShape* pAimShape = dynamic_cast<CMoveShape*>(m_pClientObjMgr->FindObject(lAimType,aimGuid));
	//				pPet->SetCurAtkShape(pAimShape);
	//				if(pAimShape)
	//				{
	//					if( pAimShape!= pPet->GetCurAtkShape())
	//					{

	//						pPet->SetDestXY( pPet->GetTileX()+0.5f, pPet->GetTileY()+0.5f);
	//					}
	//				}
	//			}
	//			break;
	//		case PET_OT_AI_ADD_SKILL:
	//			{}//PET_OT_AI_ADD_SKILL
	//		}
	//	}
	//}
}

//Ping Server add bu MartySa
DWORD CGameClient::GetPingSrvSendVal()
{
	DWORD dwCRC32 = 0;
	//CCrc32Static::DataCrc32(&m_dwLocalIP,sizeof(DWORD),dwCRC32Value);
	CGUID guid = m_pRegion->GetExID();
	CCrc32Static::DataCrc32(&guid,sizeof(CGUID),dwCRC32);
	DWORD dwValue = m_pMainPlayer->GetHotKey(0);
	CCrc32Static::DataCrc32(&dwValue,sizeof(DWORD),dwCRC32);
	dwValue = m_pMainPlayer->GetHotKey(12);
	CCrc32Static::DataCrc32(&dwValue,sizeof(DWORD),dwCRC32);
	return dwCRC32;
}

//  [8/11/2009 Square]
void CGameClient::AddObjectToClientRgn(CBaseObject* pObject)
{
	if( m_pClientObjMgr->AddObject(pObject) )
		if( ((CMoveShape*)pObject)->GetUsedTimes() <= 1 )
			m_pRegion->AddObject(pObject);
}

//	@ 从单个客户端对象链表中删除一个对象，并更新该对象的引用计数
// @ 当计数 == 0 ，则从整个场景对象链表中清除该对象
// # 在这里不做删除指针的操作，对象的销毁在该接口的调用处做销毁处理
void CGameClient::RemoveObjectFromRgn(CBaseObject* pObject)
{
	m_pClientObjMgr->RemoveObject(pObject);

	if( ((CMoveShape*)pObject)->GetUsedTimes() == 0 )
		m_pRegion->RemoveObject(pObject);
}

void CGameClient::RemoveAllObject()
{
	CMapRgnObj* mapObj = m_pClientObjMgr->GetObjectList();
	if( mapObj == NULL )	return;

	CMapRgnObj::iterator it = mapObj->begin();
	for(; it != mapObj->end();++it)
	{
		CMoveShape* pObj = (CMoveShape*)(it->second);
		pObj->ReduUsedTimes();

		m_pRegion->RemoveObject(pObj);

		if( pObj->GetUsedTimes() == 0 )
		{
#ifdef _DEBUG
			if( pObj->GetType() == TYPE_PLAYER )
			{
				char szInfo[1024] = {};
				char szGuid[128] = {};
				pObj->GetExID().tostring(szGuid);
				sprintf(szInfo,"RemoveAllObject(%0x-%s-%s)",pObj,pObj->GetName(),szGuid);
				PutStringToFile("DelShape",szInfo);
			}
#endif // _DEBUG
			SAFE_DELETE( pObj );
		}
	}
	mapObj->clear();
}

CBaseObject* CGameClient::FindObjectFromClientRgn(long lType,const CGUID& guid)
{
	return m_pClientObjMgr->FindObject(lType,guid);
}

CBaseObject* CGameClient::FindObjectFromClientRgn(const CGUID& guid)
{
	return m_pClientObjMgr->FindObject(guid);
}

//增加一个物品
void CGameClient::AddGoods(CGoods *pGoods,DWORD dwPos,LONG lExtendID)
{
	this->AddObjectToClientRgn(pGoods);

	long x = dwPos%( m_pRegion->GetWidth());
	long y = dwPos%( m_pRegion->GetWidth());
	pGoods->SetPosXY(x+0.5f,y+0.5f);
}

CGoods * CGameClient::RemoveGoods(CGUID& guID,DWORD dwPos,LONG lExtendID,DWORD dwAmount)
{
	CGoods *pGoods=dynamic_cast<CGoods *>(m_pClientObjMgr->FindObject(TYPE_GOODS,guID));
	if(pGoods==NULL)
	{
		long x=dwPos%( m_pRegion->GetWidth()),y=dwPos/( m_pRegion->GetWidth());

		CMapRgnObj * mapObj = m_pClientObjMgr->GetObjectList();
		if( mapObj == NULL )	return NULL;

		CMapRgnObj::iterator it = mapObj->begin();
		for(; it != mapObj->end(); ++it)
		{
			pGoods=dynamic_cast<CGoods *>(it->second);
			if ( pGoods && pGoods->GetTileX() == x && pGoods->GetTileY()==y )
			{
				break;
			}
			pGoods=NULL;
		}
	}
	if( pGoods==NULL )
	{
		return NULL;
	}

	this->RemoveObjectFromRgn(pGoods);
	return pGoods;
}

// 获取一个点的指定范围内是否有Shape
CShape* CGameClient::GetShapeFromSpace(long x, long y, long lSize,OBJECT_TYPE eType, CShape* pShape)
{
	for (int i = x-lSize; i < x+lSize; ++i)
	{
		for (int j = y-lSize; j <y+lSize; ++j)
		{
			CShape *pOtherShape = GetShape(i,j);
			if(pOtherShape && pShape != pOtherShape && pOtherShape->GetType() == eType )
				return pOtherShape;
		}
	}
	return NULL;
}

// 获取一个点上是否有Shape
CShape* CGameClient::GetShape(long x, long y)
{
	CMapRgnObj * mapObj = m_pClientObjMgr->GetObjectList();
	if( mapObj == NULL )	return NULL;

	CMapRgnObj::iterator it = mapObj->begin();
	for(;it != mapObj->end();++it)
		//	for(list<CBaseObject*>::iterator it = m_listClientObject.begin(); it != m_listClientObject.end(); ++it)
	{
		CShape *pShape = (CShape*)(it->second);
		if (pShape &&
			(pShape->GetType() == TYPE_PLAYER || pShape->GetType() == TYPE_MONSTER || pShape->GetType() == TYPE_GOODS || pShape->GetType() == TYPE_COLLECTION) &&
			pShape->GetTileX() == x && pShape->GetTileY() == y )
		{
			return pShape;
		}
	}
	return NULL;
}