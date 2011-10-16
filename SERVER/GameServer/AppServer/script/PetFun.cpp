// Fox@20081230----------------------------------------------
// File:    PetFun.h
// Brief:   宠物相关脚本.
//
// Author:  Fox (yulefox@gmail.com)
// Date:    Dec.30, 2008
// Fox@20081230----------------------------------------------

#include "StdAfx.h"
#include "Script.h"
#include "../player.h"
#include "../Pet.h"


// 根据宠物索引ID创建宠物
double CScript::CreatePet(const char* pCmd, char* pRetStr)
{
	long lType = GetIntParam(pCmd, 0);
	long lID = GetIntParam(pCmd, 1);
	GetInst(CPetCtrl).CreatePet((CPlayer*)p_SrcShape, lType, lID);

	return 0;
}

// 根据宠物GUID删除宠物
double CScript::DelPet(const char* pCmd, char* pRetStr)
{
	char* szGUID = GetStringParam(pCmd, 0);
	if( szGUID != NULL )
	{
		const CGUID& rID = GetScriptGuidValue(NULL, szGUID);
		M_FREE( szGUID, sizeof(char)*MAX_VAR_LEN );
		GetInst(CPetCtrl).DelPet(rID);
	}

	return 0;
}

// 根据宠物类型创建宠物
double CScript::RelivePet(const char* pCmd, char* pRetStr)
{
	long lType = GetIntParam(pCmd, 0);
	long lID = GetIntParam(pCmd, 1);
	GetInst(CPetCtrl).RelivePet((CPlayer*)p_SrcShape, lType, lID);

	return 0;
}

// 为玩家当前宠物添加技能
double CScript::AddPetSkill(const char* pCmd, char* pRetStr)
{
	long lType = GetIntParam(pCmd, 0);
	long lID = GetIntParam(pCmd, 1);
	long lLevel = GetIntParam(pCmd, 2);
	long lPos = GetIntParam(pCmd, 3);
	GetInst(CPetCtrl).AddPetSkill((CPlayer*)p_SrcShape, lType, lID, lLevel, lPos);

	return 0;
}

// 获取当前焦点宠物GUID
double CScript::GetFocusPet(const char* pCmd, char* pRetStr)
{
	char* szGUID = GetStringParam(pCmd, 0);
	if( szGUID != NULL )
	{
		int isFind = INVILID_VALUE;
		if( p_SrcShape != NULL)
		{
			SetScriptGuidValue((CMoveShape*)p_SrcShape,
				szGUID, ((CPlayer*)p_SrcShape)->GetFocusPet());
		}
		M_FREE( szGUID, sizeof(char)*MAX_VAR_LEN );
	}

	return 0;
}

// 获取某类型宠物GUID
double CScript::GetPet(const char* pCmd, char* pRetStr)
{
	long lType = GetIntParam(pCmd, 0);
	long lID = GetIntParam(pCmd, 1);
	char* szGUID = GetStringParam(pCmd, 2);
	if( szGUID != NULL )
	{
		int isFind = INVILID_VALUE;
		if( p_SrcShape != NULL)
		{
			CGUID ID;
			CPet* pPet = ((CPlayer*)p_SrcShape)->GetActivePet(lType);
			if (pPet != NULL && pPet->GetDiffPro().lIndex == lID)
			{
				ID = pPet->GetExID();
			}
			else
			{
				ID = NULL_GUID;
			}
			SetScriptGuidValue((CMoveShape*)p_SrcShape,
				szGUID, ID);
		}

		M_FREE( szGUID, sizeof(char)*MAX_VAR_LEN );
	}

	return 0;
}
