#include "StdAfx.h"
#include "Expressions.h"
#include "../script/Script.h"

CExpressions::CExpressions(void)
{
}

CExpressions::~CExpressions(void)
{	
}

//Ö´ÐÐ½á¹û
int CExpressions::Execute(CScript* pScript,CMoveShape* pDestShape)
{		
	if(NULL == pScript)	
		return 0;

#ifdef _DEBUG
	DWORD dwStartTime = timeGetTime();
#endif

	const char *pScriptText = m_strContent.c_str();
	if(m_strContent == "")
		pScriptText = const_cast<CHAR*>( GetGame()->GetScriptFileData(m_strFileName.c_str()));

	if(NULL == pScriptText || 0 >= strlen(pScriptText))
		return 0;

#ifdef _RUNSTACKINFO_
	char pszStatckInfo[10240]="";
	_snprintf(pszStatckInfo,10240,"RunStr(FileName:%s) Start", m_strFileName.c_str());
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStatckInfo);
#endif
#ifdef _SCRIPT_INFO_
	DWORD BeginTime = timeGetTime();
	pScript->m_RunTime = 0;
#endif
	if (CScript::SR_HANG == pScript->RunStr(pDestShape, pScriptText))
	{
#ifdef _RUNSTACKINFO_
		_snprintf(pszStatckInfo,10240,"RunStr(FileName:%s) Hang", m_strFileName.c_str());
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStatckInfo);
#endif
#ifdef _SCRIPT_INFO_
		pScript->m_RunTime += timeGetTime() - BeginTime;
#endif
	}
	else
	{
#ifdef _RUNSTACKINFO_
		_snprintf(pszStatckInfo,10240,"RunStr(FileName:%s) End", m_strFileName.c_str());
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStatckInfo);
#endif
#ifdef _SCRIPT_INFO_
		GetGame()->AddScriptInfo(m_strFileName.c_str(), timeGetTime() - BeginTime);
		pScript->m_RunTime = 0;
#endif
	}
	return 0;
}