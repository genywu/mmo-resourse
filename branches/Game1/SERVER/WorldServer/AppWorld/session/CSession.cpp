//CSession.cpp/////////////////////////////////////////////////////////////////////
//对象类:会话对象抽象基类
//Author:安海川
//Create Time:2008.11.03
#include "stdafx.h"
#include "MMSystem.h"
#include "CSession.h"
#include "CSessionFactory.h"
#include "../dbentity/entitygroup.h"

using namespace std;

CSession::tagSessionStep CSession::g_NullSessionStep;

//##ModelId=41DCD1EF008C
CSession::CSession(DWORD dwLifeTime)
{
	// TODO: Add your specialized code here.
	m_dwSessionType		= ST_NORMAL_SESSION;
	m_dwLifeTime		= dwLifeTime;
	m_bSessionAborted	= FALSE;
	m_bSessionStarted	= FALSE;
	m_bSessionEnded		= FALSE;
	m_bRemoveFlag		= FALSE;
}

//##ModelId=41DCD1F60157
CSession::CSession(const CGUID& guid, DWORD dwLifeTime)
{
	// TODO: Add your specialized code here.
	m_dwSessionType		= ST_NORMAL_SESSION;
	m_dwLifeTime		= dwLifeTime;
	m_bSessionAborted	= FALSE;
	m_bSessionStarted	= FALSE;
	m_bSessionEnded		= FALSE;
	m_bRemoveFlag		= FALSE;
	SetExID( guid );
	memset(m_szOwnerName, 0, sizeof(m_szOwnerName));
	m_CurSessionStepItr = m_vecSessionSteps.begin();
}

//##ModelId=41DCD2070109
CSession::~CSession()
{

}



//##ModelId=421158C70330
VOID CSession::SendNotification(CHAR* strMsg, DWORD dwColor)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	if( strMsg )
	{
	
	}
}

CSession::tagSessionStep& CSession::AddOneSessionStep(long step)
{
	for(int i=0; i<m_vecSessionSteps.size(); i++)
	{
		if (m_vecSessionSteps[i].lStep == step)
		{
			return m_vecSessionSteps[i];
		}
	}

	tagSessionStep stSessionStep;
	stSessionStep.lStep = step;
	m_vecSessionSteps.push_back(stSessionStep);
	m_CurSessionStepItr = m_vecSessionSteps.begin();
	return m_vecSessionSteps[m_vecSessionSteps.size()-1];
}
CSession::tagSessionStep& CSession::GetCurSessionStep(void)
{
	if(m_CurSessionStepItr!= m_vecSessionSteps.end())
	{
		return *m_CurSessionStepItr;
	}
	return g_NullSessionStep;
}