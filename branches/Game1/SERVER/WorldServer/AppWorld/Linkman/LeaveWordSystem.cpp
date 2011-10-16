

#include "StdAfx.h"
#include "LeaveWordSystem.h"
#include "LinkmanSystem.h"
#include "..\nets\networld\message.h"


LeaveWordSystem::LeaveWordSystem(void)
:m_pTimerQueue(NULL),
m_BufIdSeed(0)
{
	memset(&m_LeaveWordSetup, 0xff, sizeof(tagLeaveWordSetup));
}

LeaveWordSystem::~LeaveWordSystem(void)
{
	Release();
}

LeaveWordSystem	g_LeaveWordSystem;

LeaveWordSystem& LeaveWordSystem::GetInstance(void)
{
	return g_LeaveWordSystem;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//! 初始化
BOOL LeaveWordSystem::Init(const char *pSetupFile)
{
	if (!LoadSetup(pSetupFile))
	{
		AddLogText("留言系统初始化错误：读取留言配置失败！");
		return FALSE;
	}
	if (!StartupTimer())
	{
		AddLogText("留言系统初始化错误：初始化定时器失败！");
		return FALSE;
	}

	//! 预分配内存
	GetGame()->GetMemPool().SetInitialTileNum(sizeof(tagLeaveWord_W2DB), 128);

	return TRUE;
}

//! 发送配置到DBS
void LeaveWordSystem::SendSetupToDBS(void)
{
	CMessage msg(MSG_W2DB_LEAVEWORD_SETUP);
	msg.AddEx(&m_LeaveWordSetup, sizeof(tagLeaveWordSetup));
	msg.SendToSocket(GetGame()->GetDBSocketID());	
}

//! 释放
void LeaveWordSystem::Release(void)
{
	if (NULL != m_pTimerQueue)
	{
		//! 取消任务定时
		m_pTimerQueue->Cancel(this);
		//! 删除对象
		SAFE_DELETE(m_pTimerQueue);
		
		//! 正常情况，退出时m_LeaveWriteBuf应为空
		for (map<LONG, tagLeaveWord_W2DB*>::iterator ite = m_LeaveWriteBuf.begin(); ite != m_LeaveWriteBuf.end(); ++ite)
		{
			ReleaseWord_W2DB(ite->second);
			ite->second = NULL;
		}
		m_LeaveWriteBuf.clear();
	}	
}

//! 玩家留言
BOOL LeaveWordSystem::LeaveWord(const CGUID &SenderGuid, const char *pReceiverName, char *pWord)
{	
	CPlayer *pSender = GetGame()->GetMapPlayer(SenderGuid);
	if (NULL == pSender)return FALSE;

	map<CGUID, LONG>::iterator ite = m_PlayerLvwdCount.find(SenderGuid);
	if (m_PlayerLvwdCount.end() != ite)
	{
		if(m_LeaveWordSetup.lMaxLeaveWordNum <= ite->second)
		{
			//! 发送消息
			CMessage msg(MSG_W2S_LEAVEWORD_SAVE_RESULT);
			msg.Add((LONG)eLWR_SendMore);
			msg.Add(pReceiverName);
			msg.Add(SenderGuid);
			msg.SendToSocket(pSender->GetGsSocket());
			return FALSE;
		}
	}
	//! 获得留言优先级、接收者GUID
	CGUID ReceiverGuid;
	ULONG uPRI = LinkmanSystem::GetInstance().CanLeaveWord(SenderGuid, pReceiverName, ReceiverGuid);
	
	
	if (eLWPRI_Cant == uPRI)
	{
		RePlayerSendFailed(pSender, pReceiverName);
		return FALSE;
	}
	assert(NULL_GUID != ReceiverGuid);
	if (NULL_GUID == ReceiverGuid) return FALSE;

	tagLeaveWord_W2DB* pLeaveWord_W2DB = CreateLeaveWord_W2DB();
	pLeaveWord_W2DB->dwSenderType	= eSender_Player;
	pLeaveWord_W2DB->ReceiverGuid	= ReceiverGuid;
	pLeaveWord_W2DB->SenderGuid		= SenderGuid;
	pLeaveWord_W2DB->uPRI			= uPRI;
	strcpy(pLeaveWord_W2DB->szSenderName, pSender->GetName());

	LONG lBufID = GetBufID();

	CMessage msg(MSG_W2DB_LEAVEWORD_SAVE);
	msg.Add(lBufID);
	msg.AddEx(pLeaveWord_W2DB, sizeof(tagLeaveWord_W2DB));
	msg.Add(pWord);
	msg.SendToSocket(GetGame()->GetDBSocketID());

	
	assert(m_LeaveWriteBuf.end() == m_LeaveWriteBuf.find(lBufID));
	strcpy(pLeaveWord_W2DB->szReceiverName, pReceiverName);
	m_LeaveWriteBuf[lBufID] = pLeaveWord_W2DB;

	return TRUE;	
}

//! 响应玩家上线
void LeaveWordSystem::OnPlayerLogin(CPlayer *pReceiver)
{
	if(m_WaitPlayerGuid.find(pReceiver->GetExID()) == m_WaitPlayerGuid.end())
	{
		m_WaitPlayerGuid.insert(pReceiver->GetExID());
		CMessage msg(MSG_W2DB_LEAVEWORD_POP);
		msg.Add(pReceiver->GetExID());
		msg.SendToSocket(GetGame()->GetDBSocketID());
	}
}

//! 响应玩家留言取出
void LeaveWordSystem::OnLeaveWordPop(CMessage *pMsg)
{
	CGUID PlayerGuid;
	pMsg->GetGUID(PlayerGuid);
	LONG lDataSize = pMsg->GetLong();

	CPlayer *pPlayer = GetGame()->GetMapPlayer(PlayerGuid);
	if (NULL != pPlayer && sizeof(LONG) < lDataSize)
	{
		pMsg->SetType(MSG_W2S_LEAVEWORD_POP);
		pMsg->SendToSocket(pPlayer->GetGsSocket());
	}
	
	m_WaitPlayerGuid.erase(PlayerGuid);
}

//! 响应留言保存结果
void LeaveWordSystem::OnLeaveWordResult(CMessage *pMsg)
{
	LONG lBufID		= pMsg->GetLong();
	LONG lResult	= pMsg->GetLong();

	map<LONG, tagLeaveWord_W2DB*>::iterator ite = m_LeaveWriteBuf.find(lBufID);
	assert(ite != m_LeaveWriteBuf.end());
	if (ite == m_LeaveWriteBuf.end())return;

	tagLeaveWord_W2DB* pLeaveWord_W2DB = ite->second;
	m_LeaveWriteBuf.erase(lBufID);

	CPlayer *pSender = GetGame()->GetMapPlayer(pLeaveWord_W2DB->SenderGuid);
	if (NULL != pSender)
	{
		const CGUID &SenderGuid = pSender->GetExID();
		//! 发送消息
		CMessage msg(MSG_W2S_LEAVEWORD_SAVE_RESULT);
		msg.Add(lResult);
		msg.Add(pLeaveWord_W2DB->szReceiverName);
		msg.Add(SenderGuid);
		msg.SendToSocket(pSender->GetGsSocket());

		//! 记录留言次数
		if (m_PlayerLvwdCount.end() == m_PlayerLvwdCount.find(SenderGuid))
		{
			m_PlayerLvwdCount[SenderGuid] = 1;
		}
		else
		{
			m_PlayerLvwdCount[SenderGuid] += 1;
		}
	}

	ReleaseWord_W2DB(pLeaveWord_W2DB);
}

//! 定时器超时响应
long LeaveWordSystem::OnTimeOut(DWORD timerid,DWORD curtime,const void* var)
{
	m_PlayerLvwdCount.clear();
	CMessage msg(MSG_W2DB_LEAVEWORD_CLEAR_OUTTIME);
	msg.SendToSocket(GetGame()->GetDBSocketID());
	return 0;
}
//! 当定时器被取消的调用
void LeaveWordSystem::OnTimerCancel(DWORD timerid,const void* var)
{
	
}

//! 定时器检测触发
void LeaveWordSystem::TimerExpire(LONG lCurrTime)
{
	if(NULL != m_pTimerQueue)
		m_pTimerQueue->Expire(lCurrTime);
}



//! private
////////////////////////////////////////////////////////////////////////////////////////////////////
//! 读取配置
BOOL LeaveWordSystem::LoadSetup(const char *pSetupFile)
{
	if (NULL == pSetupFile)return FALSE;

	//! 打开根节点
	TiXmlNode *pNode = NULL; 		

	CRFile *prfile = rfOpen(pSetupFile);
	if(NULL == prfile)
	{
		//输出错误信息
		return FALSE;
	}

	TiXmlDocument m_Tdoc(pSetupFile);
	//载入技能配置文件
	if( !m_Tdoc.LoadData(prfile->GetData(),prfile->GetDatalen()) )
	{
		//输出错误信息
		rfClose(prfile);
		return FALSE;
	}
	rfClose(prfile);

	pNode=m_Tdoc.FirstChild("LeaveWordSetup");
	if (pNode==NULL) return FALSE;

	TiXmlElement* pLeaveWordSetup = pNode->ToElement();//获取node 的指针
	if (pLeaveWordSetup==NULL) return FALSE;

	const char *pValue = NULL;
	//! 操作时段内最大发送留言数
	if(NULL == (pValue = pLeaveWordSetup->Attribute("MaxLeaveWordNum"))) return FALSE;
	m_LeaveWordSetup.lMaxLeaveWordNum = atoi(pValue);
	//! 操作时段(把配置的分转换成秒)
	if(NULL == (pValue = pLeaveWordSetup->Attribute("OptPeriodOfTime"))) return FALSE;
	m_LeaveWordSetup.lOptPeriodOfTime = 60 * atoi(pValue);
	//! 最大发送到单一角色留言数
	if(NULL == (pValue = pLeaveWordSetup->Attribute("MaxToPlayerLeaveWordNum"))) return FALSE;
	m_LeaveWordSetup.lMaxToPlayerLeaveWordNum = atoi(pValue);
	//! 最大保存留言数
	if(NULL == (pValue = pLeaveWordSetup->Attribute("MaxSaveLeaveWordNum"))) return FALSE;
	m_LeaveWordSetup.lMaxSaveLeaveWordNum = atoi(pValue);
	//! 最大保存期限（把配置的小时转换成秒）
	if(NULL == (pValue = pLeaveWordSetup->Attribute("MaxSaveTime"))) return FALSE;
	m_LeaveWordSetup.lMaxSaveTime = 60 * 60 * atoi(pValue);

	return TRUE;
}

//! 启动工作定时器
BOOL LeaveWordSystem::StartupTimer(void)
{
	//! 创建定时器，预计4个节点，解析度为分
	m_pTimerQueue = new CTimerQueue<LeaveWordSystem*>(4, 1000 * 60);

	//! 创建留言周期任务定时器（2分钟以后开始第一次执行）
	m_pTimerQueue->Schedule(this, NULL, 2 * 60 * 1000, m_LeaveWordSetup.lOptPeriodOfTime * 1000);

	return TRUE;
}

//! 通知玩家密聊发送失败
void LeaveWordSystem::RePlayerSendFailed(CPlayer *pSender, const char *pReceiverName)
{
	CMessage msg(MSG_W2S_OTHER_TALK);
	msg.Add((BYTE)9);		// 失败
	msg.Add((LONG)CHAT_PRIVATE);
	msg.Add(pSender->GetType());
	msg.Add(pSender->GetExID());
	msg.Add(0L);
	msg.Add(pReceiverName);
	msg.SendToSocket(pSender->GetGsSocket());
}

//! 获得一个缓存的ID
LONG LeaveWordSystem::GetBufID(void)
{
	return ++m_BufIdSeed;
}

//! 创建WSDB结构
tagLeaveWord_W2DB* LeaveWordSystem::CreateLeaveWord_W2DB(void)
{
	void* pRe = GetGame()->GetMemPool().Multi_Alloc(sizeof(tagLeaveWord_W2DB), _FILE_AND_LINE_);
	new(pRe)tagLeaveWord_W2DB();
	return (tagLeaveWord_W2DB*)pRe;
}
//! 释放WSDB结构
void LeaveWordSystem::ReleaseWord_W2DB(tagLeaveWord_W2DB* pLeaveWord_W2DB)
{
	GetGame()->GetMemPool().Multi_Free(pLeaveWord_W2DB, sizeof(tagLeaveWord_W2DB));
}