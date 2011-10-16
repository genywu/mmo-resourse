//////////////////////////////////////////////////////////////////////////
//SocketCommands.cpp
//Fun:对Socket的操作命令列表
//Create Time:2004.12.17
//Author:wangqiao
//////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include ".\socketcommands.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSocketCommands::CSocketCommands(void)
{
	InitializeCriticalSection(&m_CriticalSectionCommans);
	m_hWait = CreateEvent(NULL,FALSE,FALSE,0);
	m_bWait = false;
}

CSocketCommands::~CSocketCommands(void)
{
	Clear();
	DeleteCriticalSection(&m_CriticalSectionCommans);
	CloseHandle(m_hWait);
}


// 压入命令到队列后段
bool CSocketCommands::Push_Back(tagSocketOper* pCommand)
{
	if(NULL == pCommand)	return false;

	EnterCriticalSection(&m_CriticalSectionCommans);
	if(m_Commands.size() == 0)
		SetEvent(m_hWait);
	m_Commands.push_back(pCommand);
	LeaveCriticalSection(&m_CriticalSectionCommans);

	return false;
}

// 得到命令队列长度
long CSocketCommands::GetSize()
{
	EnterCriticalSection(&m_CriticalSectionCommans);
	long lSize = (long)m_Commands.size();
	LeaveCriticalSection(&m_CriticalSectionCommans);
	return lSize;
}

// 清空命令队列
void CSocketCommands::Clear()
{
	EnterCriticalSection(&m_CriticalSectionCommans);
	m_Commands.clear();
	LeaveCriticalSection(&m_CriticalSectionCommans);
}

void CSocketCommands::CopyAllCommand(CommandsQueue& TemptCommandsQueue)
{
	EnterCriticalSection(&m_CriticalSectionCommans);
	while(m_Commands.size() == 0)
	{
		LeaveCriticalSection(&m_CriticalSectionCommans);
		//等待通知事件
		WaitForSingleObject(m_hWait,INFINITE);
		EnterCriticalSection(&m_CriticalSectionCommans);
	}
	TemptCommandsQueue = m_Commands;
	m_Commands.clear();
	LeaveCriticalSection(&m_CriticalSectionCommans);
}