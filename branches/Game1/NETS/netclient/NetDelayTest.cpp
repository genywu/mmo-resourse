#include "StdAfx.h"
#include "mmsystem.h"
#include "..\mysocket.h"
#include "message.h"
#include "netdelaytest.h"
#include "MyNetClient.h"
#include "../../GameClient/GameClient/Game.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma comment(lib,"winmm.lib")

// -------------------------------------------------------------------------------------

CMsgQueue* g_MsgQeue = NULL;
queue<int> g_MsgTime;
BOOL g_bNetDelay = false;
long g_lNetDelayInMin = 100;
long g_lNetDelayInRange = 100;
long g_lNetDelayOutMin = 100;
long g_lNetDelayOutRange = 100;

HANDLE hPushThread = NULL;

// 读取模拟设置
void InitNetDelay()
{
	ifstream stream;
	stream.open("netdelay.txt");
	if( stream.is_open() )
	{
		char str[256];
		stream >> str >> g_bNetDelay
			>> str >> g_lNetDelayInMin
			>> str >> g_lNetDelayInRange
			>> str >> g_lNetDelayOutMin
			>> str >> g_lNetDelayOutRange;

		stream.close();

		g_lNetDelayInRange -= g_lNetDelayInMin;
		g_lNetDelayOutRange -= g_lNetDelayOutMin;

		if (g_MsgQeue==NULL && g_bNetDelay != false)
		{
			g_MsgQeue = new CMsgQueue;
			unsigned threadID;
			hPushThread = (HANDLE)_beginthreadex(NULL,0,PushMessageThread,NULL,0,&threadID);
		}
	}
}

void ExitNetDelay()
{
	g_bNetDelay = false;
	WaitForSingleObject(hPushThread,INFINITE);
	SAFE_DELETE(g_MsgQeue);
}

// ---- TEST ----
// 压消息入队列线程
unsigned __stdcall PushMessageThread(void* pArguments)
{
	while(1)
	{
		Sleep(1);

		if( !g_MsgTime.empty() )
		{
			if( GetCurTickCount() >= (DWORD)g_MsgTime.front() )
			{
				CBaseMessage* pMsg = GetMsgQeue()->PopMessage();
				g_MsgTime.pop();
				if(pMsg)
				{
					GetGame()->GetNetClient()->GetRecvMessages()->PushMessage(pMsg);
				}
			}
		}

		if( g_bNetDelay == false )
		{
			break;
		}
	}

	_endthreadex( 0 );
	return 0;
}

CMsgQueue* GetMsgQeue()
{
	return g_MsgQeue;
}

// ---- TEST ----
// 发送消息线程
unsigned __stdcall SendThread(void* pArguments)
{
	extern int random(int);
	srand((unsigned)time(NULL));
	Sleep(random(g_lNetDelayOutRange)+g_lNetDelayOutMin);

	stSend* st = (stSend*)pArguments;
	(st->pSocket)->Send(st->pBuffer, st->size);
	delete st->pBuffer;
	delete st;

	_endthreadex( 0 );
	return 0;
}

// -------------------------------------------------------------------------------------