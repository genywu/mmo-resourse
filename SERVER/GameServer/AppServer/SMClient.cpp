#include "StdAfx.h"
#include "SMClient.h"
#include "../nets/Servers.h"
#include "..\GameServer\GameManager.h"
#include "../AppServer/RgnManager.h"
#include "../AppServer/script/Script.h"
class CMessage;
extern HWND g_hWnd;
#define MAPFILEOK()\
	if (!m_bMapFile)\
	return;
CSMClient* CSMClient::g_SMInstance;
CSMClient::CSMClient(void)
{
	g_SMInstance = NULL;
}

CSMClient::~CSMClient(void)
{
}

CSMClient* CSMClient::GetSM()
{
	if (g_SMInstance == NULL)
	{
		g_SMInstance = new CSMClient;
	}
	return g_SMInstance;
}

void CSMClient::ReleaseSM()
{
	GetInst(CMemClient).Release();
	if (g_SMInstance!=NULL)
	{
		delete g_SMInstance;
		g_SMInstance =NULL;
	}
}

bool CSMClient::LoadMapFileSetup(CDataBlockAllocator *pDBAllocator)
{
	string strTempt("");
	string strID("");
	string strMapObjName("");
	ifstream stream;
	stream.open("MapFileSetup.ini");
	if( stream.is_open() )
	{
		stream  >> strTempt>> strID
			>> strTempt >> strMapObjName;
		GetGame()->GetSetup()->dwGappID = atoi(strID.c_str());
		m_bMapFile=GetInst(CMemClient).Initial(
			strMapObjName.c_str(), pDBAllocator);
		if (!m_bMapFile)
		{
			AddLogText("没有启动SM,不能记录相关日志!");
		}
	}
	return false;
}

void CSMClient::SendAITick(bool b /* = false */)
{
	MAPFILEOK();
	CMessage msg(MSG_GAPP2SM_SEND_MAINAITICK);
	msg.Add(GetGame()->GetSetup()->dwGappID);
	//是否是第一次发送
	if (b)
	{
		msg.Add(BYTE(1));
	}
	else
		msg.Add(BYTE(0));
	msg.Add(GetGame()->GetAITick());
	msg.SendToSM();
}

void CSMClient::ExitOk()
{
	MAPFILEOK();
	CMessage msg(MSG_GAPP2SM_SHUTDOWN);
	msg.Add(GetGame()->GetSetup()->dwGappID);
	msg.SendToSM();
}

void CSMClient::InitOk()
{
	MAPFILEOK();
	CMessage msg(MSG_GAPP2SM_GAPP_INITOK);
	char szTitle[MAX_PATH]={0};
	::GetWindowText(g_hWnd,szTitle,MAX_PATH);
	msg.AddEx(szTitle,MAX_PATH);
	msg.Add(GetGame()->GetSetup()->dwGappID);
	msg.SendToSM();
}

void CSMClient::ProcessMessage()
{
	MAPFILEOK();
	static msgQueue TemptMsgQueue;
	GetInst(CMemClient).GetRecvMsgs()->GetAllMessage(TemptMsgQueue);
	msgQueue::iterator it = TemptMsgQueue.begin();
	for(;it != TemptMsgQueue.end();it++)
	{
		CMessage* pMsg = (CMessage*)(*it);
		if( pMsg )
		{
			pMsg->Run();
			CBaseMessage::FreeMsg(pMsg);
		}
	}
	TemptMsgQueue.clear();
}

void CSMClient::ChangeWatchedPlayer(const char* name,BOOL bDel)
{
	if (!bDel)
	{
		set<string>::iterator it=WatchedSet.find(name);
		if (it!=WatchedSet.end())
			WatchedSet.erase(it);	
	}
	else
		WatchedSet.insert(name);
}

bool CSMClient::FindWatchedPlayer(const char* name)
{
	set<string>::iterator it=WatchedSet.find(name);
	if (it!=WatchedSet.end())
	{
		return true;
	}
	return false;
}

bool CSMClient::IsSendTo(BYTE byType)
{
	switch (byType)
	{
	case eGGC_Move:
		{
			static DWORD now=timeGetTime(),old=now;
			now=timeGetTime();
			if (now-old>5000)
			{
				old=now;
				return true;
			}
		}
		break;
	default:
		return true;
	}
	return false;
}

void CSMClient::SendWatchResult(const char* name,BYTE byType,
								char* szInfo)
{
	if (!FindWatchedPlayer(name))
		return;
	if (!IsSendTo(byType))
		return;
	CMessage msg(MSG_S2W_OTHER_WATCH_INFO);
	msg.Add((DWORD)byType);
	msg.Add(szInfo);
	msg.Send();
}
