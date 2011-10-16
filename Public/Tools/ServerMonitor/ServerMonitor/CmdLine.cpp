#include "StdAfx.h"
#include "CmdLine.h"

CCmdLine* CCmdLine::m_Instance;
CCmdLine::CCmdLine(void)
{
	m_Instance = NULL;
}

CCmdLine::~CCmdLine(void)
{
	DeleteCriticalSection(&m_CSection);
}

CCmdLine* CCmdLine::GetInstance()
{
	if (m_Instance == NULL)
	{
		m_Instance = new CCmdLine;
	}
	return m_Instance;
}

bool CCmdLine::Init()
{
	InitializeCriticalSection(&m_CSection);
	return true;
}

bool CCmdLine::Release()
{
	if (m_Instance!=NULL)
	{
		delete m_Instance;
		m_Instance = NULL;
	}
	return true;
}
bool CCmdLine::InsertCmd(const char* cmd)
{
	EnterCriticalSection(&m_CSection);
	m_qCmdLines.push_back(cmd);
	LeaveCriticalSection(&m_CSection);
	return true;
}

bool CCmdLine::GetAllCmd(deque<string>& dequecmd)
{
	EnterCriticalSection(&m_CSection);
	dequecmd.resize(m_qCmdLines.size());
	std::copy(m_qCmdLines.begin(),m_qCmdLines.end(),dequecmd.begin());
	m_qCmdLines.clear();
	LeaveCriticalSection(&m_CSection);
	return true;
}