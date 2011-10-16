#include "StdAfx.h"
#include "SMClient.h"
#include "..\nets\networld\message.h"
#include "../AppWorld/GlobalRgnManager.h"
#include "..\nets\Servers.h"
#include "..\nets\MemClient.h"
#include "Player.h"
extern HWND g_hWnd;
CSMClient* CSMClient::g_SMInstance;
#define MAPFILEOK()\
	if (!m_bMapFile)\
	return;
CSMClient::CSMClient(void)
:m_bMapFile(false)
,m_WatchedGuid(NULL_GUID)
{
	ZeroMemory(m_WatchedName,32);
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
		stream.close();
	}	
	return false;
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


void CSMClient::ReloadFileNotify(char* szfile,BYTE bSuccess)
{
	/*CMessage msg(MSG_WS2SM_RELOADFILE);
	long lsize= (long)strlen(szfile);
	msg.Add(lsize);
	msg.AddEx(szfile,lsize);
	msg.Add(bSuccess);
	msg.SendToSM();*/
}

void CSMClient::SendInfo(char* szInfo,...)
{
	/*char szText[512]={0};
	va_list va;
	va_start(va,szInfo);
	vsprintf(szText,szInfo,va);
	va_end(va);
	CMessage msg(MSG_GAPP2SM_SENDINFO);
	msg.Add(GetGame()->GetSetup()->dwGappID);
	msg.Add((long)strlen(szText));
	msg.AddEx(szText,(long)strlen(szText));
	msg.SendToSM();*/
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

void CSMClient::SendReportInfo(CPlayer* pPlayer,CPlayer* pObjPlayer,WORD wType)
{
	CMessage msg(MSG_GAPP2SM_REPORTINFO);
	msg.Add(pObjPlayer->GetName());
	msg.Add(pPlayer->GetName());
	msg.Add(wType);
	msg.Add(time(NULL));
	msg.Add(GetGame()->GetSetup()->dwGappID);
	msg.SendToSM();
}

void CSMClient::SendBanResult(BYTE bSucc,CGUID& guid)
{
	CMessage Replymsg(MSG_GAPP2SM_REPORT_Result_Reply);
	Replymsg.Add(guid);
	Replymsg.Add((BYTE)bSucc);
	Replymsg.SendToSM();
}

void CSMClient::GetBroadCastInfo(REPORTPLAYER& tgInfo,char* szInfo)
{
	if (tgInfo._lBanTime>=0)
	{
		DWORD dwHours=tgInfo._lBanTime/3600;
		sprintf(szInfo,CStringReading::LoadString(IDS_WS_REPORT_INFO,
				(eReportType)tgInfo._dwCause + eTipInfo1),
				tgInfo._IndicteeName);
		
	}
	//解封
	else if (tgInfo._lBanTime<0)
	{
	}
}

bool CSMClient::OnProcessBanPlayer(const char* name,bool bSucc)
{
	map<string,REPORTPLAYER>::iterator it=mapBanPlayer.find(name);
	//在数据中找到该玩家
	if (it!=mapBanPlayer.end())
	{	
		//发送到LS封号
		CMessage msg(MSG_W2L_GM_BANPLAYER);
		msg.Add(name);
		msg.Add(it->second._lBanTime);
		msg.SendToSocket(GetGame()->GetLSSocketID());
		//返回结果给SCC,解封不发公告
		SendBanResult(bSucc?1:0,it->second._OptGuid); 
		if (it->second._lBanTime>=0)
		{	
			//发送到GS踢除玩家,解封时不发送
			CPlayer* pPlayer=GetGame()->GetMapPlayer(it->second._IndicteeName);
			//玩家在线
			if (pPlayer!=NULL)
			{
				CMessage msggs(MSG_W2S_GM_KICKPLAYER);
				msggs.Add(NULL_GUID);
				msggs.Add(it->second._IndicteeName);
				msggs.SendGSID(GetGame()->GetGameServerNumber_ByPlayerID(pPlayer->GetExID()));
			}
		}
		//发送世界公告
		if (bSucc)
		{
			CMessage msg(MSG_W2S_OTHER_BROADCAST);
			msg.Add((long)0);
			msg.Add((long)0);
			msg.Add(MAKELONG(MAKEWORD(0,0), MAKEWORD(255,255)));
			msg.Add(MAKELONG(MAKEWORD(0,0), MAKEWORD(0,50)));
			char szInfo[512]={0};
			GetBroadCastInfo(it->second,szInfo);
			msg.Add(szInfo);
			msg.SendAll();	
		}
		mapBanPlayer.erase(it);
	}
	return true;
}

bool  CSMClient::BanPlayer(tagBaseResultInfo& tgInfo,CGUID& guid)
{
	REPORTPLAYER tgBan;
	memcpy(&tgBan,&tgInfo,sizeof(tagBaseResultInfo));
	tgBan._OptGuid=guid;
	mapBanPlayer[tgBan._IndicteeName]=tgBan;
	//发送到DBS封号
	GetGame()->SendGMBanPlayerToDBSMsg(tgInfo._IndicteeName,tgInfo._lBanTime);
	return true;
}