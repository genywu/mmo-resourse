#include "StdAfx.h"
#include "Expressions.h"
//#include "../Script/Script.h"
#include "../LuaScript/Script.h"
#include "../LuaScript/ScriptSys.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CExpressions::CExpressions(void)
{
}

CExpressions::~CExpressions(void)
{	
}

//Ö´ÐÐ½á¹û
int CExpressions::Execute(Script* pScript,CMoveShape* pDestShape)
{		
	if(NULL == pScript)	
		return 0;

#ifdef _DEBUG
	DWORD dwStartTime = timeGetTime();
#endif

	//const char *pScriptText = m_strContent.c_str();
	//if(m_strContent == "")
	//	pScriptText = const_cast<CHAR*>( GetGame()->GetScriptFileData(m_strFileName.c_str()));

	//if(NULL == pScriptText || 0 >= strlen(pScriptText))
	//	return 0;
	bool runStr = m_strContent.length() > 0;

#ifdef _RUNSTACKINFO_
	char pszStatckInfo[10240]="";
	_snprintf(pszStatckInfo,10240,"RunStr(FileName:%s) Start", m_strFileName.c_str());
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStatckInfo);
#endif
#ifdef _SCRIPT_INFO_
	//DWORD BeginTime = timeGetTime();
	//pScript->m_RunTime = 0;
#endif
	int ret = 0;
	if( runStr )
	{
		ret = pScript->RunStr( m_strContent.c_str() );
	}
	else
	{
		ret = pScript->RunScript( m_strFileName.c_str() );
	}

	return ret;
	//if (CScript::SR_HANG == pScript->RunStr(pDestShape, pScriptText))
//	{
//#ifdef _RUNSTACKINFO_
//		_snprintf(pszStatckInfo,10240,"RunStr(FileName:%s) Hang", m_strFileName.c_str());
//		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStatckInfo);
//#endif
//#ifdef _SCRIPT_INFO_
//		pScript->m_RunTime += timeGetTime() - BeginTime;
//#endif
//	}
//	else
//	{
//#ifdef _RUNSTACKINFO_
//		_snprintf(pszStatckInfo,10240,"RunStr(FileName:%s) End", m_strFileName.c_str());
//		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStatckInfo);
//#endif
//#ifdef _SCRIPT_INFO_
//		GetGame()->AddScriptInfo(m_strFileName.c_str(), timeGetTime() - BeginTime);
//		pScript->m_RunTime = 0;
//#endif
//	}

	//int nRet = m_pParam->pVariableList->GetVarValue("$Ret",0);	
#ifdef _DEBUG
	GetGame()->AddTimeOutScript(timeGetTime()-dwStartTime);
#endif
	return 0;
}