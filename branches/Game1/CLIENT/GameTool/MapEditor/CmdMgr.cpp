#include "StdAfx.h"

#include "..\..\frostengine\utility.h"
#include "..\..\frostengine\console.h"
#include "cmdmgr.h"
#include "cmd.h"

IMPLEMENT_SINGLETON(CmdMgr);

CmdMgr::CmdMgr(void)
{
	m_pCurCmd = NULL;
}

CmdMgr::~CmdMgr(void)
{
	Clear();
}

void CmdMgr::Do(Cmd * pCmd)
{
	//判断是否超出了长度 (未实现)
	//MAX_COMMAND_SAVED

	//运行pCmd,
	//并把pCmd设为当前运行
	//插入到队列中
	pCmd->Execute();

	pCmd->SetPrev(m_pCurCmd);

	Cmd *pPreCmd = m_pCurCmd;
	Cmd *pNextCmd = (pPreCmd)? pPreCmd->GetNext() : NULL;

	pCmd->SetPrev(pPreCmd);
	pCmd->SetNext(pNextCmd);

	if (pPreCmd)
	{
		pPreCmd->SetNext(pCmd);
	}

	if (pNextCmd)
	{
		pNextCmd->SetPrev(pCmd);
	}

	m_pCurCmd = pCmd;
}

void CmdMgr::UnDo()
{
	//Un运行当前
	if (m_pCurCmd)
	{
		if (m_pCurCmd->GetExecuted())
		{
			m_pCurCmd->UnExecute();
		
			//退回上一个
			Cmd *pCmd = m_pCurCmd->GetPrev();
			if (pCmd)
			{
				m_pCurCmd = pCmd;
			}
		}
	}
	
}

void CmdMgr::ReDo()
{
    if (m_pCurCmd)
	{	
		if (!m_pCurCmd->GetExecuted())
		{
			m_pCurCmd->Execute();
		}
		else
		{
			//前进到下一个
			Cmd *pNextCmd = m_pCurCmd->GetNext();

			if (pNextCmd)
			{
				//下一个可以运行
				if (!pNextCmd->GetExecuted())
				{
					pNextCmd->Execute();
					m_pCurCmd = m_pCurCmd->GetNext();
				}
			}
		}
	}
}

void CmdMgr::Dump()
{
	if (m_pCurCmd)
	{
		OutputConsole("cur Cmd: ");
		m_pCurCmd->Dump();
		OutputConsole("\n");

		Cmd *pCmd = m_pCurCmd->GetPrev();
		while(pCmd)
		{
			OutputConsole("prev Cmd: ");
			pCmd->Dump();
			OutputConsole("\n");
			pCmd = pCmd->GetPrev();
		}
		pCmd = m_pCurCmd->GetNext();
		while(pCmd)
		{
			OutputConsole("next Cmd: ");
			pCmd->Dump();
			OutputConsole("\n");
			pCmd = pCmd->GetNext();
		}
	}
}

void CmdMgr::Clear()
{
	if (m_pCurCmd)
	{
		//向前
		Cmd *pPrev = m_pCurCmd->GetPrev();
		while(pPrev)
		{
			Cmd *pCmd = pPrev->GetPrev();
			SAFEDELETE(pPrev);
			pPrev = pCmd;
		}
		//向后
		Cmd *pNext = m_pCurCmd->GetNext();
		while(pNext)
		{
			Cmd *pCmd = pNext->GetNext();
			SAFEDELETE(pNext);
			pNext = pCmd;
		}
		//中间
		SAFEDELETE(m_pCurCmd);
	}
}