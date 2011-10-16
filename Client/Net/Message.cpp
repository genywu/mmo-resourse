#include "stdafx.h"
#include "message.h"
#include "../GameClient/Game.h"
#include "../GameClient/ClientApp/region.h"
#include "../GameClient/ClientApp/player.h"
#include "../GameClient/ClientApp/Goods/goods.h"
#include "../GameClient/ClientApp/message/sequencestring.h"
#include "../GameClient/ClientApp/Other/TestMsgType.h"

extern void OnLogMessage(CMessage* pMsg);				// 响应登陆消息
extern void OnOtherMessage(CMessage* pMsg);				// 响应其他消息
extern void OnWorldMessage(CMessage* pMsg);             // 响应World类型消息
extern void OnGoodsMessage(CMessage* pMsg);				// 响应物品类型消息
extern void OnTeamMessage(CMessage * pMsg);				// 响应组队消息
extern void OnShopMessage(CMessage* pMsg);				// 商店消息
extern void OnIncrementShopMessage(CMessage* pMsg);		// 商店消息
extern void OnDepotMessage(CMessage* pMsg);
extern void OnGMMessage(CMessage* pMsg);				// 响应GM消息
extern void OnSkillMessage(CMessage* pMsg);				// 相应技能消息
extern void OnPlayerShopMessage(CMessage* pMsg);
extern void OnPlayerTradeMessage(CMessage* pMsg);		// 响应玩家交易消息
extern void OnOrgasysMessage(CMessage* pMsg);
extern void OnContainerMessage(CMessage* pMsg);
extern void OnPetMessage(CMessage* pMsg);
extern void OnCostMessage(CMessage *pMsg);
extern void OnLinkmanMessage(CMessage *pMsg);
extern void OnSpriteMessage(CMessage *pMsg);
//extern void OnGameCardSaleMessage(CMessage *pMsg);

extern void OnCountryMessage(CMessage *pMsg);

extern void OnMailMessage(CMessage *pMsg);			// 邮件相关消息
extern void OnFriendHuntMessage(CMessage *pMsg);	// 好友查询消息

extern void OnCheckCheatMessage(CMessage *pMsg);    // 防外挂相关消息

extern void OnBounsMessage(CMessage* pMsg);			// 游戏外赠送系统相关消息

extern void OnAuctionHouseMessage(CMessage* pMsg);  // 仙贝寄售系统相关消息

extern void PhaseMessage(CMessage* pMsg);           ///> 游戏相位模块

extern void OnQuestMessage(CMessage* pMsg);			///> 从行会（OrgasysMessage）中独立出来的任务消息

extern void OnGeniusMessage(CMessage* pMsg);		/// 天赋消息

//--------------------------------------------------------------------------------------
extern void OnTestMouseMessage(CMessage *pMsg);			// 测试程序的模拟鼠标消息处理
extern void OnTestKeyMessage(CMessage *pMsg);			// 测试程序的模拟键盘消息处理
extern void OnTestLogicMessage(CMessage *pMsg);			// 测试程序的逻辑事件消息处理
//--------------------------------------------------------------------------------------

extern HWND g_hRetLb;

//#include "GameClient\DebugInfo\CrashHandler.h"
//#include "GameClient\DebugInfo\SystemInformation.h"
//#include "GameClient\DebugInfo\LogHtml.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CClient	*CMessage::m_pNetClient;
CClient* CMessage::s_pNetClient_ChatServer;
//CMapFile *CMessage::m_pMapFile = NULL;
CMemClient *CMessage::m_pMemClient = NULL;

CMessage::CMessage(void)
{
	m_pRegion = NULL;
	m_pPlayer = NULL;
}

CMessage::CMessage(long lType)
{
	m_pRegion = NULL;
	m_pPlayer = NULL;

	Init(lType);
}

CMessage::~CMessage(void)
{
}

 //重设消息相关变量
void CMessage::Reset(void)
{
	m_pRegion = NULL;
	m_pPlayer = NULL;
}

bool CMessage::Initial(CDataBlockAllocator* pDBAllocator,long nMaxFreeMsgNum)
{
	CBaseMessage::NewMsg = NewMessage;
	return CBaseMessage::Initial(pDBAllocator,nMaxFreeMsgNum);
}
bool CMessage::Release()
{
	CBaseMessage::NewMsg = NULL;
	return CBaseMessage::Release();
}

CBaseMessage* CMessage::NewMessage()
{
	return new CMessage();
}

// [客户端] 发送消息
long CMessage::Send(CClient* pClient)
{
		// 检测消息类型是c to s 把从C到S的消息封装 时间效验 和 序列效验
	if(pClient == NULL)
		pClient = m_pNetClient;
	if(pClient == NULL)	return false;
	// 网络延时模拟测试
	*((long*)Base()) = GetSize()+HEAD_SIZE;
	CBaseMessage *pMsg = CBaseMessage::AllocMsg();
	vector<CDataBlock*>& MsgData = GetMsgData();
	pMsg->SetMsgData(MsgData);
	MsgData.clear();
	pClient->ASend(pMsg);
	return true;
}

// [客户端] 发送消息给聊天服务器
bool CMessage::SendToChatServer()
{
    if (s_pNetClient_ChatServer == NULL)
        return false;
    // 网络延时模拟测试
    *((long*)Base()) = GetSize()+HEAD_SIZE;
    CBaseMessage *pMsg = CBaseMessage::AllocMsg();
    vector<CDataBlock*>& MsgData = GetMsgData();
    pMsg->SetMsgData(MsgData);
    MsgData.clear();
    s_pNetClient_ChatServer->ASend(pMsg);
    return true;
}

//打开文件
void CMessage::AsyOpenFile(const char* pszFileName)
{
	if(m_pMemClient)
	{
		CMessage msg(MSG_FILE_OPEN);
		msg.Add(pszFileName);
		m_pMemClient->Send(&msg);
	}
	return;
}
//关闭文件
void CMessage::AsyCloseFile(const char* pszFileName)
{
	if(m_pMemClient)
	{
		CMessage msg(MSG_FILE_CLOSE);
		msg.Add(pszFileName);
		m_pMemClient->Send(&msg);
	}
	return;
}
//清空文件内容
void CMessage::AsyClearFile(const char* pszFileName)
{
	if(m_pMemClient)
	{
		CMessage msg(MSG_FILE_CLEAR);
		msg.Add(pszFileName);
		m_pMemClient->Send(&msg);
	}
	return;
}

//异步写文件的接口
void CMessage::AsyWriteFile(const char* pszFileName,const char* pszContent)
{
	if(m_pMemClient)
	{
		CMessage msg(MSG_FILE_WRITE);
		msg.Add(pszFileName);
		msg.Add(pszContent);
		m_pMemClient->Send(&msg);
	}
}


// 向测试程序发送消息
long CMessage::SendMemToTest(CMemClient *pMemClient)
{
#ifdef _CLENTTOTEST_
	if(pMemClient == NULL)
		pMemClient = m_pMemClient;
	if(pMemClient == NULL)
		return false;
	//分配一个消息
	*((long*)Base()) = GetSize()+HEAD_SIZE;
	CBaseMessage *pMsg = CBaseMessage::AllocMsg();
	Clone(pMsg);
	pMemClient->Send(pMsg);
	/*char str[256]="";
	sprintf(str,"Send Msg To Test:%d，Time:%d",GetType(),timeGetTime());
	PutStringToFile("TestMsg",str);*/
#endif
	return true;
}

////////////////////////////////////////////////////////////////////////
//	消息处理
////////////////////////////////////////////////////////////////////////

long CMessage::DefaultRun()
{
    switch (GetType())
    {
    case BASEMSG_Socket_Close:
        {
            if (GetNetFlag() & NF_CS_Client)
            {
                ResetClient4Cs();
            }
            else if (GetNetFlag() & NF_LS_Client)
            {
                GetGame()->OnServerLost();		
            }
            else if (GetNetFlag() & NF_GS_Client)
            {
                GetGame()->OnServerLost();
            }
        }
        break;
    }
    return true;
}

// 客户端消息处理
long CMessage::Run()
{
	// 获取region和player指针
	if (m_pPlayer==NULL)
	{
		m_pPlayer = GetGame()->GetMainPlayer();
	}
	if (m_pRegion==NULL)
	{
		m_pRegion = (CRegion*)GetGame()->GetRegion();
	}

	char str[1024] = "";
#ifdef _RUNSTACKINFO_
	sprintf_s(str,"CMessage::Run() Start.消息类型：%d",GetType());
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),str);
#endif
	//----------------------------------


	// 转发到Test程序
#ifdef _CLENTTOTEST_
	if(m_pMemClient)
	{
		SendMemToTest();
	}
#endif

	unsigned long lType = GetType() & 0xffffff00;
	// 消息分类处理
	switch( lType )	// GetType() - GetType() % 256
	{
	case 0:
		DefaultRun();
		break;
	//----------------------------------------------------/登陆消息/
	case MSG_C2C_LOG:
	case MSG_L2C_LOG:
	case MSG_S2C_LOG:
		OnLogMessage(this);
		break;

	//----------------------------------------------------/场景消息/
	case MSG_S2C_RGN:
		if( m_pRegion )
		{
			m_pRegion->OnMessage(this);
		}
		break;

	//----------------------------------------------------/SHAPE消息/
	case MSG_S2C_SHAPE:
		if( m_pPlayer )
		{
			m_pPlayer->CMoveShape::OnMessage(this);
		}
		break;
	//----------------------------------------------------/玩家消息/
	case MSG_S2C_PLAYER:
		if( m_pPlayer )
		{
			m_pPlayer->OnMessage(this);
		}
		break;

	//---------------------------------------------------/物品消息/
	case MSG_S2C_GOODS:
		if(m_pPlayer)
		{
			OnGoodsMessage(this);
		}
		break;
	//-----------------------------------------------------/好友查询/
	case MSG_S2C_PM:
		if(m_pPlayer)
		{
			OnFriendHuntMessage(this);
		}
		break;
	case MSG_S2C_SHOP:
		OnShopMessage(this);
		break;

	case MSG_S2C_INCREMENTSHOP:
		OnIncrementShopMessage(this);
		break;

	case MSG_S2C_OTHER:
    case MSG_CS2C:
		OnOtherMessage(this);
		break;

	case MSG_S2C_TEAM:
		OnTeamMessage(this);
		break;

	case MSG_S2C_DEPOT:
		OnDepotMessage(this);
		break;

	case MSG_S2C_GM:
		/**## 服务器端对GM命令的响应返回处理 */
		/**## Added By Jar @ 2004-4-6 */
		OnGMMessage(this);
		break;

	case MSG_S2C_SKILL:
		if( m_pRegion )
		{
			OnSkillMessage(this);
		}
		break;

	case MSG_S2C_ORGSYS:
		{
			OnOrgasysMessage(this);
		}
		break;

	case MSG_S2C_PLAYERSHOP:
		{
			OnPlayerShopMessage(this);
		}
		break;
	case MSG_S2C_CONTAINER:
		{
			OnContainerMessage(this);
		}
		break;
	case MSG_S2C_PET:
		{
			OnPetMessage(this);
		}
			break;
	case MSG_S2C_MAIL:
		{
			OnMailMessage(this);
		}

	case MSG_S2C_COST:
		{
			//OnCostMessage(this);
		}
	case MSG_S2C_CARD:
		{
			//OnGameCardSaleMessage(this);
		}
		break;
	case MSG_S2C_COUNTRY:
		OnCountryMessage(this);
		break;
	case MSG_S2C_LINKMAN:
		OnLinkmanMessage(this);
		break; 
	case MSG_S2C_SPRITE:
		OnSpriteMessage(this);
		break;
		/* *****************************
		功能:   防外挂消息处理
		作者:zhaohang(kaba)
		创建日期: 2009/5/26
		修改日志:
		修改日期:
		修改内容:
		修改人  :
		* *****************************/
	case MSG_S2C_ANTICHEAT://这里消息头根据服务器确定
		OnCheckCheatMessage(this);
		break;

	case MSG_S2C_BOUNS:
		OnBounsMessage(this);
		break;
		/////////////////////////////////////////////////
		// zhaohang  2010/5/6 
		// 仙贝寄售消息处理
		/////////////////////////////////////////////////
	case MSG_S2C_AH:
		OnAuctionHouseMessage(this);
		break;

    case MSG_S2C_PHASE:
        PhaseMessage(this);
        break;
	case MSG_S2C_QUEST:
		OnQuestMessage(this);
		break;
	case MSG_S2C_PLAYERTRADE:
		OnPlayerTradeMessage(this);
		break;
		/***********************************************************************/
		/*  zhaohang 2010-10-18 天赋消息*/
		/***********************************************************************/
	case MSG_S2C_GENIUS:
		OnGeniusMessage(this);
		break;
	}
#ifdef _RUNSTACKINFO_
	sprintf_s(str,"CMessage::Run() End.消息类型：%d",GetType());
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),str);
#endif
	return true;
}

// 测试程序发送的消息处理
long CMessage::TestRun()
{
	// 获取region和player指针
	if (m_pPlayer==NULL)
		m_pPlayer = GetGame()->GetMainPlayer();
	if (m_pRegion==NULL)
		m_pRegion = (CRegion*)GetGame()->GetRegion();

	unsigned long lType = GetType() & 0xffffff00;

	// 转发到Test程序
#ifdef _CLENTTOTEST_
	// 消息分类处理
	switch( lType )	// GetType() - GetType() % 256
	{
	case 0:
		break;
	case MSG_T2C_MOUSE:
		OnTestMouseMessage(this);
		break;
	case MSG_T2C_KEY:
		OnTestKeyMessage(this);
		break;
	case MSG_T2C_LOGIC:
		OnTestLogicMessage(this);
	}
#endif
	return 1;
}
