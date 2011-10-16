//=============================================================================
/**
 *  file: DeputyOccuFun.cpp
 *
 *  Brief:定义了跟副职业相关的脚本函数
 *			
 *
 *  Authtor:wangqiao
 *	
 *	Create Datae:2008-11-06
 */
//=============================================================================

#include "StdAfx.h"
#include "mmsystem.h"
#include "script.h"
#include "variablelist.h"
#include "../player.h"
#include "../monster.h"
#include "../serverregion.h"
#include "../Collection.h"
#include "../../../setup/LogSystem.h"
#include "../public/StrType.h"
#include "../public/StrUnifyInput.h"



//副职业
double CScript::CanCollect(const char* cmd, char* retStr)
{
	//double ret=0;
	CPlayer* player=dynamic_cast<CPlayer*>(p_SrcShape);
	if (player== NULL)
	{
		return -1;
	}

	DWORD dwSkillParam = GetIntParam(cmd,0);
	stModuParam *pSkill = (stModuParam*)dwSkillParam;
	if (pSkill==NULL)
	{
		return -1;
	}
	DWORD dwDesShapeParam= GetIntParam(cmd,1);
	CMoveShape* pMoveShape= (CCollection*)dwDesShapeParam;
	if (pMoveShape==NULL)
	{
		return -1;
	}

	int result = (player->CanCollect(pMoveShape));
	if (result!= SKILL_USE_NO_FAILED)
	{
		player->SendDOccuSKillFailMsg(result,pSkill->GetID(),pSkill->GetLv());
#ifdef _DEBUG
		char str[64]="";
		_snprintf(str, 64, "副职业采集result%d\n",result);
		OutputDebugString(str);
#endif
		return result;
	}

	//执行脚本
	const CCollectionLayout* pCollectionLayout= CDeputyOccuSystem::GetCollectionLayout(pMoveShape->GetCollectionName().c_str());
	if (pCollectionLayout==NULL)
	{
#ifdef _DEBUG
		char str[64]="";
		_snprintf(str, 64, "副职业采集ret%d\n",result);
		OutputDebugString(str);
#endif
		const string& strScrpt = pCollectionLayout->GetStartScriptName();
		if (strScrpt!="")
		{
			stRunScript st;
			st.pszFileName   = (char*)(strScrpt.c_str());
			st.srcShape  = player;
			st.desShape	 = pMoveShape;
			st.pRegion   = dynamic_cast<CRegion*>(player->GetFather());
			RunScript(&st,(char*)GetGame()->GetScriptFileData(st.pszFileName));	
		}
	}
	
#ifdef _DEBUG
	char str[64]="";
	_snprintf(str, 64, "---------副职业采集ret%d\n",result);
	OutputDebugString(str);
#endif
	return result;
}

double CScript::CanCheckCollect(const char* cmd, char* retStr)
{
	int ret=0;
	CPlayer* player=dynamic_cast<CPlayer*>(p_SrcShape);
	if (player== NULL)
	{
		return -1;
	}

	DWORD dwSkillParam = GetIntParam(cmd,0);
	stModuParam *pSkill = (stModuParam*)dwSkillParam;
	if (pSkill==NULL)
	{
		return -1;
	}
	DWORD dwDesShapeParam= GetIntParam(cmd,1);
	CMoveShape* pMoveShape=(CCollection*)dwDesShapeParam;
	if (pMoveShape==NULL)
	{
		return -1;
	}

	ret = player->CheckCanCollect(pMoveShape);
	return ret==SKILL_USE_NO_FAILED? 1:0;
}

double CScript::CheckCollect(const char* cmd, char* retStr)
{
	CPlayer* player=dynamic_cast<CPlayer*>(p_SrcShape);
	if (player== NULL)
	{
		return -1;
	}

	DWORD dwSkillParam = GetIntParam(cmd,0);
	stModuParam *pSkill = (stModuParam*)dwSkillParam;
	if (pSkill==NULL)
	{
		return -1;
	}
	DWORD dwDesShapeParam= GetIntParam(cmd,1);
	CMoveShape* pMoveShape=(CCollection*)dwDesShapeParam;
	if (pMoveShape==NULL)
	{
		return -1;
	}

	double ret= player->CheckCollect(pMoveShape);
	//执行脚本
	const CCollectionLayout* pCollectionLayout= CDeputyOccuSystem::GetCollectionLayout(pMoveShape->GetCollectionName().c_str());
	if (pCollectionLayout==NULL)
	{
		return ret;
	}
	const string& strScrpt = pCollectionLayout->GetEndScriptName();
	if (strScrpt!="")
	{
		stRunScript st;
		st.pszFileName   = (char*)(strScrpt.c_str());
		st.srcShape  = player;
		st.desShape	 = pMoveShape;
		st.pRegion   = dynamic_cast<CRegion*>(player->GetFather());
		RunScript(&st,(char*)GetGame()->GetScriptFileData(st.pszFileName));	
	}

	const string& strSpring = CDeputyOccuSystem::GetCollectionSuccSpring();
	if (strSpring!="")
	{
		stRunScript st;
		st.pszFileName   = (char*)(strSpring.c_str());
		st.srcShape  = player;
		st.desShape	 = pMoveShape;
		st.pRegion   = dynamic_cast<CRegion*>(player->GetFather());
		RunScript(&st,(char*)GetGame()->GetScriptFileData(st.pszFileName));	
	}
	return ret;
}

double CScript::LockCollection(const char* cmd,char* retStr/* =NULL */)
{
	CPlayer* player=dynamic_cast<CPlayer*>(p_SrcShape);
	if (player== NULL)
	{
		return 0;
	}

	DWORD dwDesShapeParam= GetIntParam(cmd,0);
	CMoveShape* pMoveShape=(CCollection*)dwDesShapeParam;
	if (pMoveShape==NULL)
	{
		return 0;
	}
	player->LockCollection(pMoveShape);
	return 1;
}
double CScript::OutlockCollection(const char* cmd, char* retStr)
{
	CPlayer* player=dynamic_cast<CPlayer*>(p_SrcShape);
	if (player== NULL)
	{
		return -1;
	}

	/*DWORD dwSkillParam = GetIntParam(cmd,0);
	stModuParam *pSkill = (stModuParam*)dwSkillParam;
	if (pSkill==NULL)
	{
		return -1;
	}*/
	DWORD dwDesShapeParam= GetIntParam(cmd,0);
	CMoveShape* pMoveShape=(CCollection*)dwDesShapeParam;
	if (pMoveShape==NULL)
	{
		return -1;
	}

	
	player->OutlockCollection(pMoveShape);
	return (double)0;
}

double CScript::CanFacture(const char* cmd, char* retStr)
{
	CPlayer* player=dynamic_cast<CPlayer*>(p_SrcShape);
	if (player== NULL)
	{
		return -1;
	}
	DWORD dwSkillParam = GetIntParam(cmd,0);
	stModuParam *pSkill = (stModuParam*)dwSkillParam;
	if (pSkill==NULL)
	{
		return -1;
	}

	int result = player->CanFacture();
	if (result!= SKILL_USE_NO_FAILED)
	{
		player->SendDOccuSKillFailMsg(result,pSkill->GetID(),pSkill->GetLv());
		return -1;
	}

	return (double)1;
}

double CScript::Facture(const char* cmd, char* retStr)
{
	CPlayer* player=dynamic_cast<CPlayer*>(p_SrcShape);
	if (player== NULL)
	{
		return -1;
	}
	DWORD dwSkillParam = GetIntParam(cmd,0);
	stModuParam *pSkill = (stModuParam*)dwSkillParam;
	if (pSkill==NULL)
	{
		return -1;
	}
	BOOL result = player->Facture();
	if (result!= TRUE)
	{
		player->SendDOccuSKillFailMsg(result,pSkill->GetID(),pSkill->GetLv());
		return -1;
	}
	const string& strSpring = CDeputyOccuSystem::GetFactureSuccSpring();
	if (strSpring!="")
	{
		stRunScript st;
		st.pszFileName   = (char*)(strSpring.c_str());
		st.srcShape  = player;
		st.desShape	 = player;
		st.pRegion   = dynamic_cast<CRegion*>(player->GetFather());
		RunScript(&st,(char*)GetGame()->GetScriptFileData(st.pszFileName));	
	}

	return	(double)1;
}
double CScript::FactureSkillStudyBegin(const char* cmd, char* retStr)
{
	CMonster* pMonster= dynamic_cast<CMonster*>(p_DesShape);
	CPlayer*  pPlayer = dynamic_cast<CPlayer*>(p_SrcShape);
	if (pMonster== NULL || pPlayer== NULL)	return -1;
	pPlayer->FactureSkillStudyBegin(pMonster);
	return -1;
}

double CScript::ClearFactureList(const char* cmd, char* retStr)
{
	CPlayer*  pPlayer = dynamic_cast<CPlayer*>(p_SrcShape);
	if (pPlayer== NULL)	return -1;
	pPlayer->CleanFacture();
	return 1;
}

double  CScript::AddFacture(const char* cmd, char* retStr)
{
	CPlayer*  pPlayer = dynamic_cast<CPlayer*>(p_SrcShape);
	if (pPlayer== NULL)	return -1;
	DWORD dwSkillParam = GetIntParam(cmd,0);
	if (dwSkillParam==ERROR_CODE || dwSkillParam == INVILID_VALUE)
	{
		return 0;
	}
	return (double)pPlayer->AddFacture(dwSkillParam);
}

double  CScript::DelFacture(const char* cmd, char* retStr)
{
	CPlayer*  pPlayer = dynamic_cast<CPlayer*>(p_SrcShape);
	if (pPlayer== NULL)	return -1;
	DWORD dwSkillParam = GetIntParam(cmd,0);
	if (dwSkillParam==ERROR_CODE || dwSkillParam == INVILID_VALUE)
	{
		return 0;
	}
	return (double)pPlayer->DeleteFacture(dwSkillParam);
}
double CScript::FindFacture(const char* cmd, char* retStr)
{
	CPlayer*  pPlayer = dynamic_cast<CPlayer*>(p_SrcShape);
	if (pPlayer== NULL)	return -1;
	DWORD dwSkillParam = GetIntParam(cmd,0);
	if (dwSkillParam==ERROR_CODE || dwSkillParam == INVILID_VALUE)
	{
		return 0;
	}
	return (double)pPlayer->FindFacture(dwSkillParam);
}

double CScript::GetFactureNeedsLevel(const char* cmd, char* retStr)
{
	CPlayer*  pPlayer = dynamic_cast<CPlayer*>(p_SrcShape);
	if (pPlayer== NULL)	return -1;
	DWORD dwSkillParam = GetIntParam(cmd,0);
	if (dwSkillParam==ERROR_CODE || dwSkillParam == INVILID_VALUE)
	{
		return 0;
	}
	return (double)pPlayer->GetFactureNeedsLevel(dwSkillParam);
}

double CScript::GetFactureNeedsDOccu(const char* cmd, char* retStr)
{
	CPlayer*  pPlayer = dynamic_cast<CPlayer*>(p_SrcShape);
	if (pPlayer== NULL)	return -1;
	DWORD dwSkillParam = GetIntParam(cmd,0);
	if (dwSkillParam==ERROR_CODE || dwSkillParam == INVILID_VALUE)
	{
		return 0;
	}
	return (double)pPlayer->GetFactureNeedsDOccu(dwSkillParam);
}

double CScript::UpdateEffect(const char* cmd, char* retStr)
{
	CPlayer*  pPlayer = dynamic_cast<CPlayer*>(p_SrcShape);
	if (pPlayer== NULL)	return -1;
	pPlayer->UpdateEffect();
	return 1;
}

double CScript::AddCollectionExp(const char* cmd,char* retstr/* =NULL */)
{
	long exp = GetIntParam(cmd,0);
	if (exp == ERROR_CODE || exp == INVILID_VALUE)
	{
		return -1;
	}
	CPlayer*  pPlayer = dynamic_cast<CPlayer*>(p_SrcShape);
	if (pPlayer== NULL)	return -1;
	pPlayer->AddCollectionExp(exp);
	return 1;
}

double CScript::AddFactureExp(const char* cmd,char* retstr/* =NULL */)
{
	long exp = GetIntParam(cmd,0);
	if (exp == ERROR_CODE || exp == INVILID_VALUE)
	{
		return -1;
	}
	CPlayer*  pPlayer = dynamic_cast<CPlayer*>(p_SrcShape);
	if (pPlayer== NULL)	return -1;
	pPlayer->AddFactureExp(exp);
	return 1;
}