#include "StdAfx.h"
#include "SMClient.h"
#include "../Net/Message.h"
#include "../App/GlobalRgnManager.h"
#include "../../../Public/Common/ManageToolMsgType.h"

CSMClient* CSMClient::g_SMInstance;
#define MAPFILEOK()\
	if (!m_bMapFile)\
	return;
CSMClient::CSMClient(void)
:m_bMapFile(false)
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

bool CSMClient::LoadMapFileSetup(CDataBlockAllocator *pDBAllocator)
{
    bool res = true;
	string strTempt("");
	string strID("");
	string strMapObjName("");
	ifstream stream;
	stream.open("MapFileSetup.ini");
	if( stream.is_open() )
	{
		stream  >> strTempt>> strID
			>> strTempt >> strMapObjName;
		m_bMapFile=GetInst(CMemClient).Initial(strMapObjName.c_str(),
            pDBAllocator);
		if (!m_bMapFile)
		{
            res = false;
			Log4c::Warn(ROOT_MODULE, GetText("GAME_SYS_SMDISABLE"));
		}
		stream.close();
	}	
	return res;
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
	msg.Add((long)GetNetHandler().GetSetup().gappID);
	//是否是第一次发送
	if (b)
	{
		msg.Add(BYTE(1));
	}
	else
		msg.Add(BYTE(0));
	msg.Add(GetGame().GetAITick());
	msg.SendToSM();
}

void CSMClient::ExitOk()
{
	MAPFILEOK();
	CMessage msg(MSG_GAPP2SM_SHUTDOWN);
	msg.Add((long)GetNetHandler().GetSetup().gappID);
	msg.SendToSM();
}

void CSMClient::InitOk()
{
	MAPFILEOK();
	CMessage msg(MSG_GAPP2SM_GAPP_INITOK);
	char szTitle[MAX_PATH];
    AppFrame::GetTitle(szTitle);
	msg.AddEx(szTitle,MAX_PATH);
	msg.Add((long)GetNetHandler().GetSetup().gappID);
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