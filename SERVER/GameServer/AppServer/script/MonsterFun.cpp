=============================================================================
/**
 *  file: MonsterFun.cpp
 *
 *  Brief:定义跟怪物相关的脚本函数
 *			
 *
 *  Authtor:wangqiao
 *	
 *	Create Datae:2008-11-06
 */
=============================================================================

#include "StdAfx.h"
#include "mmsystem.h"
#include "script.h"
#include "variablelist.h"
#include "../monster.h"
#include "../Player.h"
#include "../RgnManager.h"
#include "../ai/MonsterAI.h"
#include "../ai/AIFactory.h"
#include "../ai/ScriptAI.h"
#include "../serverregion.h"
#include "../../../setup/LogSystem.h"
#include "../../GameServer/GameManager.h"
#include "../public/StrType.h"
#include "../public/StrUnifyInput.h"
#include "../public/kl_tp_utility.h"
#include "../AI/AIUtilities.h"


#define ADJUST_V( v ) v = ( v == INVILID_VALUE || v == ERROR_CODE ) ? 0 : v

double CScript::SetMonsterAttr(const char* CmdStr, char* retStr)
{
	CMonster* pMonster=dynamic_cast<CMonster*>(p_DesShape);
	if(!pMonster)
		return 0;

	//属性名
	char* pAttrName=GetStringParam(CmdStr,0);
	if(!pAttrName)
	{
		return 0;
	}

	if(strcmp(pAttrName, "OrginName") == 0)
	{
		char* szName = GetStringParam(CmdStr, 1);
		if(szName)
		{
			pMonster->SetOriginName(szName);
			M_FREE( pAttrName, sizeof(char)*MAX_VAR_LEN );
			M_FREE( szName, sizeof(char)*MAX_VAR_LEN );
			return 1.0f;
		}
		M_FREE( pAttrName, sizeof(char)*MAX_VAR_LEN );
		M_FREE( szName, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}
	else if(strcmp(pAttrName, "ViewName") == 0)
	{
		char* szName = GetStringParam(CmdStr, 1);
		if(szName)
		{
			pMonster->SetName(szName);
			M_FREE( pAttrName, sizeof(char)*MAX_VAR_LEN );
			M_FREE( szName, sizeof(char)*MAX_VAR_LEN );
			return 1.0f;
		}
		M_FREE( pAttrName, sizeof(char)*MAX_VAR_LEN );
		M_FREE( szName, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}

	int nAttrVal=GetIntParam(CmdStr,1);
	pMonster->SetAttribute((string)pAttrName,nAttrVal);
	pMonster->UpdateAttribute();

	M_FREE( pAttrName, sizeof(char)*MAX_VAR_LEN );
	return 1;
}

double CScript::GetMonsterAttr(const char* CmdStr, char* retStr)
{
	CMonster* pMonster=dynamic_cast<CMonster*>(p_DesShape);
	if(!pMonster)
		return 0;

	char* pAttrName=GetStringParam(CmdStr, 0);
	if(!pAttrName)
	{
		return 0;
	}

	char* pAttrValue = GetStringParam(CmdStr, 1);
	if(!pAttrValue)
	{
		M_FREE( pAttrName, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}

	long getFlag = GetIntParam(CmdStr, 2);

	CMoveShape* setShape = NULL;
	if(0 == getFlag)
		setShape = (CMoveShape*)pMonster;
	else if(1 == getFlag)
		setShape = (CMoveShape*)p_SrcShape;
	else
	{
		M_FREE( pAttrName, sizeof(char)*MAX_VAR_LEN );
		M_FREE( pAttrValue, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}

	if(strcmp(pAttrName, "OrginName") == 0)
	{
		SetScriptVarValue(setShape, pAttrValue, pMonster->GetOriginName());
		M_FREE( pAttrName, sizeof(char)*MAX_VAR_LEN );
		M_FREE( pAttrValue, sizeof(char)*MAX_VAR_LEN );
		return 1.0f;
	}
	else if(strcmp(pAttrName, "ViewName") == 0)
	{
		SetScriptVarValue(setShape, pAttrValue, pMonster->GetName());
		M_FREE( pAttrName, sizeof(char)*MAX_VAR_LEN );
		M_FREE( pAttrValue, sizeof(char)*MAX_VAR_LEN );
		return 1.0f;
	}

	int cmdLen = strlen(CmdStr);

	// 取第一个参数： 变量名字符串
	double varValue = pMonster->GetAttribute(string(pAttrName));
	SetScriptVarValue(setShape, pAttrValue, 0, varValue);

	M_FREE( pAttrName, sizeof(char)*MAX_VAR_LEN );
	M_FREE( pAttrValue, sizeof(char)*MAX_VAR_LEN );
	return 0;
}

double CScript::ChangeMonsterAttr(const char* CmdStr, char* retStr)
{
	CMonster* pMonster=dynamic_cast<CMonster*>(p_DesShape);
	if(!pMonster)
		return 0;

	//属性名
	char* pAttrName=GetStringParam(CmdStr,0);
	if(!pAttrName)
	{
		return 0;
	}

	if(strcmp(pAttrName, "OrginName") == 0)
	{
		char* szName = GetStringParam(CmdStr, 1);
		if(szName)
		{
			pMonster->SetOriginName(szName);
			M_FREE( pAttrName, sizeof(char)*MAX_VAR_LEN );
			M_FREE( szName, sizeof(char)*MAX_VAR_LEN );
			return 1.0f;
		}
		M_FREE( pAttrName, sizeof(char)*MAX_VAR_LEN );
		M_FREE( szName, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}
	else if(strcmp(pAttrName, "ViewName") == 0)
	{
		char* szName = GetStringParam(CmdStr, 1);
		if(szName)
		{
			pMonster->SetName(szName);
			M_FREE( pAttrName, sizeof(char)*MAX_VAR_LEN );
			M_FREE( szName, sizeof(char)*MAX_VAR_LEN );
			return 1.0f;
		}
		M_FREE( pAttrName, sizeof(char)*MAX_VAR_LEN );
		M_FREE( szName, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}

	int nAttrVal=GetIntParam(CmdStr,1);
	pMonster->ChangeAttribute((string)pAttrName, nAttrVal);
	pMonster->UpdateAttribute();

	M_FREE( pAttrName, sizeof(char)*MAX_VAR_LEN );
	return 1;
}


double CScript::AddHpBar(const char* cmd, char* retStr)
{
	if (p_SrcShape==NULL) return 0;
	CMonster *pMonster= dynamic_cast<CMonster*>(p_SrcShape);
	if (pMonster== NULL)  return 0;

	int AddHp = GetIntParam(cmd, 0);
	if (AddHp==ERROR_CODE || AddHp== INVILID_VALUE) return 0;
	char* Script= GetStringParam(cmd,1);
	pMonster->AddHpBar(AddHp,Script);
	M_FREE( Script, sizeof(char)*MAX_VAR_LEN );
	return 0;
}

double CScript::GetHpLevel(const char* cmd, char* retStr)
{
	if (p_SrcShape==NULL) return 0;
	CMonster *pMonster= dynamic_cast<CMonster*>(p_SrcShape);
	if (pMonster== NULL)  return 0;
	int AddHp = GetIntParam(cmd, 0);
	if (AddHp==ERROR_CODE || AddHp== INVILID_VALUE) return 0;
	return pMonster->GetHpLevel(AddHp);
}

double CScript::GetKillerID(const char* cmd, char* retStr)
{
	CMonster* pMonster= dynamic_cast<CMonster*>(p_SrcShape);
	if (pMonster== NULL)
	{
		return -1;
	}
	char* cGuidName= GetStringParam(cmd,0);
	if (cGuidName== NULL)	return -1;

	SetScriptGuidValue(pMonster, cGuidName,pMonster->GetKilledMeID());

	M_FREE( cGuidName, sizeof(char)*MAX_VAR_LEN );
	return 0;
}

double CScript::GetKillerType(const char* cmd, char* retStr)
{
	CMonster* pMonster= dynamic_cast<CMonster*>(p_SrcShape);
	if (pMonster== NULL)
	{
		return -1;
	}
	return pMonster->GetKilledMeType();
}

double CScript::SetBeneficiary(const char* cmd, char* retStr)
{
	////////////////////////////////////////////////////////////////////////
	char* szGUID = GetStringParam(cmd, 0); 
	int type=GetIntParam(cmd,1);
	char* szBeneficiaryGUID= GetStringParam(cmd,2);
	int  iBeneficiaryType= GetIntParam(cmd,3);
	int iBeneficiaryTime= GetIntParam(cmd,4);

	if (szGUID==NULL || szBeneficiaryGUID==NULL ||
		type== ERROR_CODE || type== INVILID_VALUE||
		iBeneficiaryType== ERROR_CODE || iBeneficiaryType== INVILID_VALUE||
		iBeneficiaryTime== ERROR_CODE || iBeneficiaryTime== INVILID_VALUE)
	{
		M_FREE( szGUID, sizeof(char)*MAX_VAR_LEN );
		M_FREE( szBeneficiaryGUID, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}

	const CGUID &monsterGUID = GetScriptGuidValue(static_cast<CMoveShape*>( this->p_SrcShape ), szGUID);
	CServerRegion* pRegion=dynamic_cast<CServerRegion*>(p_SrcShape->GetFather());
	if (pRegion!= NULL)
	{
		CMoveShape* pMoveShape= dynamic_cast<CMoveShape*>(pRegion->FindChildObject(type,monsterGUID));
		if (pMoveShape!= NULL)
		{
			CGUID BeneficiaryGUID= GetScriptGuidValue(static_cast<CMoveShape*>( this->p_SrcShape), szBeneficiaryGUID);
			pMoveShape->StartRecordBeneficiary(BeneficiaryGUID,(OBJECT_TYPE)iBeneficiaryType,iBeneficiaryTime);
		}
	}
	M_FREE( szGUID, sizeof(char)*MAX_VAR_LEN );
	M_FREE( szBeneficiaryGUID, sizeof(char)*MAX_VAR_LEN );
	return 1;
}

double CScript::GetBeneficiaryID(const char* cmd, char* retStr)
{
	CMonster* pMonster= dynamic_cast<CMonster*>(p_SrcShape);
	if (pMonster== NULL)
	{
		return -1;
	}
	char* cGuidName= GetStringParam(cmd,0);
	if (cGuidName== NULL)	return -1;

	SetScriptGuidValue(pMonster, cGuidName,pMonster->GetBeneficiaryID());

	M_FREE( cGuidName, sizeof(char)*MAX_VAR_LEN );
	return 0;
}

double CScript::GetBeneficiaryType(const char* cmd, char* retStr)
{
	CMonster* pMonster= dynamic_cast<CMonster*>(p_SrcShape);
	if (pMonster== NULL)
	{
		return -1;
	}
	return pMonster->GetBeneficiaryType();
}

double CScript::GetBeneTeamGuid(const char* CmdStr, char* retStr)
{
	int nRet = 0;
	DWORD dwObject = GetIntParam(CmdStr,0);
	char* strGuid = GetStringParam(CmdStr,1);
	if (dwObject!=ERROR_CODE && dwObject!=INVILID_VALUE && strGuid!=NULL)
	{
		CMonster *pMonster=(CMonster*)(dwObject);
		if (pMonster)
		{
			CPlayer* pBeneficial = pMonster->GetBeneficiary();
			if (pBeneficial)
			{
				SetScriptGuidValue(static_cast<CMoveShape*>( this->p_SrcShape ),strGuid,(pBeneficial->GetTeamID()));
				nRet = 1;
			} 
			else
			{
				SetScriptGuidValue(static_cast<CMoveShape*>( this->p_SrcShape ),strGuid,(NULL_GUID));
				nRet = 0;
			}				
		} 
		else
		{
			SetScriptGuidValue(static_cast<CMoveShape*>( this->p_SrcShape ),strGuid,(NULL_GUID));
			nRet = 0.0;
		}
	}
	else
	{
		SetScriptGuidValue(static_cast<CMoveShape*>( this->p_SrcShape ),strGuid,(NULL_GUID));
		nRet = 0.0;
	}
	M_FREE( strGuid, sizeof(char)*MAX_VAR_LEN );
	return nRet;
}

//! 删除一个地图上所有指定名称的怪物（等同怪物死亡），返回删除的数量
double CScript::DelRegionMonsterByName(const char* CmdStr, char* retStr)
{
	if(NULL == p_SrcShape) return 0;
	CPlayer* pPlayer=dynamic_cast<CPlayer*>(p_SrcShape);
	if(NULL == pPlayer) return 0;
	CServerRegion *pRegion = dynamic_cast<CServerRegion*>(pPlayer->GetFather());
	if(NULL == pRegion) return 0;

	const char* pMonsterName = GetStringParam(CmdStr,0);
	if(NULL == pMonsterName) return 0;

	double dRe = pRegion->DelMonster(pMonsterName);
	M_FREE( pMonsterName, sizeof(char)*MAX_VAR_LEN );
	return dRe;
}

double CScript::MonsterTalk(const char* CmdStr, char* retStr)
{
	if(p_SrcShape == NULL) return 0;

	char* name=GetStringParam(CmdStr,0);
	char* content=GetStringParam(CmdStr,1);
	if (content && name)
	{
		BEGIN_LOOP_AROUND_SHAPE((CServerRegion*)(p_SrcShape->GetFather()), p_SrcShape->GetArea()->GetX(), p_SrcShape->GetArea()->GetY())
		{
			if( pShape->GetType() == TYPE_MONSTER )
			{					
				CMonster* pMonster=(CMonster*)pShape;
				if(strcmp(pMonster->GetOriginName(),name)==0)				
				{
					pMonster->Talk(content);
				}
			}
		}
		END_LOOP_AROUND_SHAPE
	}

	M_FREE( name, sizeof(char)*MAX_VAR_LEN );
	M_FREE( content, sizeof(char)*MAX_VAR_LEN );

	return 0;
}

double CScript::MonsterTalkByIndex(const char* CmdStr, char* retStr)
{
	if(p_SrcShape == NULL) return 0;

	char* name=GetStringParam(CmdStr,0);
	LONG TextType= GetIntParam(CmdStr,1);
	LONG TextIndex=GetIntParam(CmdStr,2);
	if (TextIndex==ERROR_CODE || TextIndex==INVILID_VALUE || TextType == ERROR_CODE || TextType== INVILID_VALUE)
	{
		M_FREE( name, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}

	if (name)
	{
		BEGIN_LOOP_AROUND_SHAPE((CServerRegion*)(p_SrcShape->GetFather()), p_SrcShape->GetArea()->GetX(), p_SrcShape->GetArea()->GetY())
		{
			if( pShape->GetType() == TYPE_MONSTER )
			{					
				CMonster* pMonster=(CMonster*)pShape;
				if(strcmp(pMonster->GetOriginName(),name)==0)
				{
					pMonster->Talk(TextType,TextIndex);
				}
			}
		}
		END_LOOP_AROUND_SHAPE
	}

	M_FREE( name, sizeof(char)*MAX_VAR_LEN );
	return 0;
}

double CScript::MonsterTalkByGUID(const char* CmdStr, char* retStr)
{
	if(p_SrcShape == NULL) return 0;

	char* szGUID = GetStringParam(CmdStr, 0); 
	if(!szGUID) return 0;

	const CGUID &monsterGUID = GetScriptGuidValue(static_cast<CMoveShape*>( this->p_SrcShape ),szGUID);	
	char* str = GetStringParam(CmdStr, 1);
	if(str)
	{
		CServerRegion* pRegion=dynamic_cast<CServerRegion*>(p_SrcShape->GetFather());
		if(pRegion== NULL)
		{
			M_FREE( szGUID, sizeof(char)*MAX_VAR_LEN );
			M_FREE( str, sizeof(char)*MAX_VAR_LEN );
			return 0;
		}
		
		CMonster *pMonster = NULL;
		if( monsterGUID == NULL_GUID )
		{
			pMonster = static_cast<CMonster*>( this->p_SrcShape );
		}
		else
		{
			pMonster= dynamic_cast<CMonster*>(pRegion->FindChildObject(TYPE_MONSTER,monsterGUID));
		}
		if(pMonster== NULL)
		{
			M_FREE( szGUID, sizeof(char)*MAX_VAR_LEN );
			M_FREE( str, sizeof(char)*MAX_VAR_LEN );
			return 0;
		}

		pMonster->Talk(str);

		M_FREE( szGUID, sizeof(char)*MAX_VAR_LEN );
		M_FREE( str, sizeof(char)*MAX_VAR_LEN );
		return 1;
	}

	M_FREE( szGUID, sizeof(char)*MAX_VAR_LEN );
	return 0;
}

double CScript::MonsterTalkByGuidByIndex(const char* CmdStr, char* retStr)
{
	if(p_SrcShape == NULL) return -1;

	char* szGUID = GetStringParam(CmdStr, 0); 
	if(!szGUID) return 0;

	CGUID monsterGUID = GetScriptGuidValue(static_cast<CMoveShape*>( p_SrcShape),szGUID );	
	//char* str = GetStringParam(CmdStr, 1);
	int type= GetIntParam(CmdStr,1);
	int id= GetIntParam(CmdStr,2);

	CServerRegion* pRegion=dynamic_cast<CServerRegion*>(p_SrcShape->GetFather());
	if(pRegion== NULL)
	{
		M_FREE( szGUID, sizeof(char)*MAX_VAR_LEN );
		return -1;
	}

	CMonster* pMonster= dynamic_cast<CMonster*>(pRegion->FindChildObject(TYPE_MONSTER,monsterGUID));
	if(pMonster== NULL)
	{
		M_FREE( szGUID, sizeof(char)*MAX_VAR_LEN );
		return -1;
	}
	pMonster->Talk(type,id);

	M_FREE( szGUID, sizeof(char)*MAX_VAR_LEN );
	return 0;
}

double CScript::UseSkillByID(const char* CmdStr, char* retStr)
{
	CMonster* pMonster = (CMonster*)p_SrcShape;// monster
	if( pMonster ) 
	{
		double skillID = GetIntParam(CmdStr, 0);
		double skilllevel=GetIntParam(CmdStr,1);
		if(skillID != ERROR_CODE && skillID != INVILID_VALUE && skilllevel != ERROR_CODE && skilllevel != INVILID_VALUE)
		{
			char *strGUID = GetStringParam( CmdStr, 2 );
			if( strGUID == NULL || strlen( strGUID ) == 0 )
			{
				pMonster->GetAI()->UseSkillByID(skillID,skilllevel);
			}
			else
			{
				const CGUID &targetID = GetScriptGuidValue( pMonster, strGUID );
				pMonster->GetAI()->UseSkillByID( skillID, skilllevel, targetID );
			}
			M_FREE( strGUID, sizeof(char)*MAX_VAR_LEN );
		}
	}
	return 0;
}

double CScript::UseSkillByIDOnCell( const char *cmd, char *retStr )
{
	CMonster *pMonster = static_cast<CMonster*>( p_SrcShape );
	if( pMonster != NULL )
	{
		double skillID = GetIntParam( cmd, 0 );
		double skillLevel = GetIntParam( cmd, 1 );
		if( skillID == ERROR_CODE || skillLevel == ERROR_CODE )
		{
			return -1;
		}

		double posX = GetIntParam( cmd, 2 );
		double posY = GetIntParam( cmd, 3 );
		posX = posX == ERROR_CODE ? -1 : posX;
		posY = posY == ERROR_CODE ? -1 : posY;

		pMonster->GetAI()->UseSkillByIDOnCell( skillID, skillLevel, posX, posY );
		return 0;
	}

	return -1;
}

double CScript::UseSkillSync( const char *cmd, char *retStr )
{
	CMonster *pMonster = static_cast<CMonster*>( p_SrcShape );
	if( pMonster != NULL )
	{
		double skillType = GetIntParam( cmd, 0 );
		double skillID = GetIntParam( cmd, 1 );
		double skillLevel = GetIntParam( cmd, 2 );
		if( skillType == ERROR_CODE || skillID == ERROR_CODE || skillLevel == ERROR_CODE )
		{
			return -1;
		}
		char *strGUID = GetStringParam( cmd, 3 );
		bool ret = false;
		if( strGUID != NULL && strlen( strGUID ) != 0 )
		{
			ret = static_cast<CMonsterAI*>( pMonster->GetAI() )->UseSkillSyncSafe( skillType, skillID, skillLevel, 
				GetScriptGuidValue( pMonster, strGUID ) );
		}
		else
		{
			ret = static_cast<CMonsterAI*>( pMonster->GetAI() )->UseSkillSyncSafe( skillType, skillID, skillLevel );
		}

		// hang up the script until the act time expires.
		if( ret && pMonster->GetAIType() >= CAIFactory::SCRIPT_AI )
		{
			SetIsHang( true );
			SetHangFunc( SR_FUNC_USESKILLSYNC );
			CScriptAI *pAI = static_cast<CScriptAI*>( pMonster->GetAI() );
			pAI->SkillHangup( GetID(), pMonster->GetActModify( skillID ) );
		}
#ifdef _DEBUG
		else
		{
			char info[64];
			_snprintf( info, 64, "技能:%d 使用失败。\n", (int)skillID );
			OutputDebugString( info );
		}
#endif
		M_FREE( strGUID, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}

	return -1;
}

double CScript::UseSkillSyncWithTrace( const char *cmd, char *retStr )
{
	using namespace kl_common;
	CMonster *pMonster = static_cast<CMonster*>( p_SrcShape );
	if( pMonster != NULL && pMonster->GetAIType() >= CAIFactory::SCRIPT_AI )
	{
		double skillType = GetIntParam( cmd, 0 );
		double skillID = GetIntParam( cmd, 1 );
		double skillLevel = GetIntParam( cmd, 2 );
		if( skillType == ERROR_CODE || skillID == ERROR_CODE || skillLevel == ERROR_CODE )
		{
			return -1;
		}
		char* strGUID = GetStringParam( cmd, 3 );
		double trace_time = GetIntParam( cmd, 4 );
		char* strRet = GetStringParam( cmd, 5 );
		bool ret = false;
		if( trace_time == ERROR_CODE )
		{
			M_FREE(strGUID, sizeof(char)*MAX_VAR_LEN);
			M_FREE(strRet, sizeof(char)*MAX_VAR_LEN);
			return -1;
		}
		if( strRet && strGUID && strlen( strGUID ) != 0 )
		{
			const CGUID &target_id = GetScriptGuidValue( pMonster, strGUID );	

			SetIsHang( true );
			SetHangFunc( SR_FUNC_USESKILLSYNC );
			CScriptAI *pAI = static_cast<CScriptAI*>( pMonster->GetAI() );
			pAI->SkillHangupWithTrace( GetID(), trace_time, std::string( strRet ), target_id, skillID, skillLevel, skillType );
		}
		M_FREE(strGUID, sizeof(char)*MAX_VAR_LEN);
		M_FREE(strRet, sizeof(char)*MAX_VAR_LEN);
		return 0;
	}

	return -1;
}

double CScript::UseSkillOnCellSync( const char *cmd, char *retStr )
{
	CMonster *pMonster = static_cast<CMonster*>( p_SrcShape );
	if( pMonster != NULL )
	{
		double skillType = GetIntParam( cmd, 0 );
		double skillID = GetIntParam( cmd, 1 );
		double skillLevel = GetIntParam( cmd, 2 );
		double targetX = GetIntParam( cmd, 3 );
		double targetY = GetIntParam( cmd, 4 );
		if( skillType == ERROR_CODE || skillID == ERROR_CODE || skillLevel == ERROR_CODE )
		{
			return -1;
		}

		bool ret = false;
		ret = static_cast<CMonsterAI*>( pMonster->GetAI() )->UseSkillOnCellSyncSafe( skillType, skillID, skillLevel, 
			targetX == ERROR_CODE ? 0 : targetX,
			targetY == ERROR_CODE ? 0 : targetY );

		// hang up the script until the act time expires.
		if( ret && pMonster->GetAIType() >= CAIFactory::SCRIPT_AI )
		{
			SetIsHang( true );
			SetHangFunc( SR_FUNC_USESKILLSYNCONCELL );
			CScriptAI *pAI = static_cast<CScriptAI*>( pMonster->GetAI() );
			pAI->SkillHangup( GetID(), pMonster->GetActModify( skillID ) );
		}
#ifdef _DEBUG
		else
		{
			char info[64];
			_snprintf( info, 64, "技能:%d 使用失败。\n", (int)skillID );
			OutputDebugString( info );
		}
#endif
		return 0;
	}

	return -1;
}

double CScript::MoveToCellOnPeace	(const char* CmdStr, char* retStr)
{
	CMonster* pMonster= (CMonster*)p_SrcShape;
	if (pMonster)
	{
		LONG lX=INVILID_VALUE;
		LONG lY=INVILID_VALUE;
		lX=GetIntParam(CmdStr,0);
		lY=GetIntParam(CmdStr,1);
		if (lX!=INVILID_VALUE && lX!=ERROR_CODE&&lY!=INVILID_VALUE && lY!=ERROR_CODE)
		{
			pMonster->GetAI()->MoveToCellOnPeace(lX,lY);
		}
	}
	return 0;
}
double CScript::MoveToCellOnFight	(const char* CmdStr, char* retStr)
{
	CMonster* pMonster= (CMonster*)p_SrcShape;
	if (pMonster)
	{
		LONG lX=INVILID_VALUE;
		LONG lY=INVILID_VALUE;
		lX=GetIntParam(CmdStr,0);
		lY=GetIntParam(CmdStr,1);
		if (lX!=INVILID_VALUE && lX!=ERROR_CODE&&lY!=INVILID_VALUE && lY!=ERROR_CODE)
		{
			pMonster->GetAI()->MoveToCellOnFight(lX,lY);
		}
	}
	return 0;
}
double CScript::MoveToCellAround	(const char* CmdStr, char* retStr)
{
	CMonster* pMonster= (CMonster*)p_SrcShape;
	if (pMonster)
	{
		LONG lX=INVILID_VALUE;
		LONG lY=INVILID_VALUE;
		lX=GetIntParam(CmdStr,0);
		lY=GetIntParam(CmdStr,1);
		if (lX!=INVILID_VALUE && lX!=ERROR_CODE&&lY!=INVILID_VALUE && lY!=ERROR_CODE)
		{
			pMonster->GetAI()->MoveToCellAround(lX,lY);
		}
	}
	return 0;
}

double CScript::MoveByStepOnPeace	(const char* CmdStr, char* retStr)
{
	CMonster* pMonster= (CMonster*)p_SrcShape;
	if (pMonster)
	{
		LONG lDir=INVILID_VALUE;
		LONG lStep=INVILID_VALUE;
		lDir=GetIntParam(CmdStr,0);
		lStep=GetIntParam(CmdStr,1);
		if (lDir!=INVILID_VALUE && lDir!=ERROR_CODE && lStep!=INVILID_VALUE && lStep!=ERROR_CODE)
		{
			pMonster->GetAI()->MoveByStepOnPeace(lDir,lStep);
		}
	}
	return 0;
}
double CScript::MoveByStepOnFight	(const char* CmdStr, char* retStr)
{
	CMonster* pMonster= (CMonster*)p_SrcShape;
	if (pMonster)
	{
		LONG lDir=INVILID_VALUE;
		LONG lStep=INVILID_VALUE;
		lDir=GetIntParam(CmdStr,0);
		lStep=GetIntParam(CmdStr,1);
		if (lDir!=INVILID_VALUE && lDir!=ERROR_CODE && lStep!=INVILID_VALUE && lStep!=ERROR_CODE)
		{
			pMonster->GetAI()->MoveByStepOnFight(lDir,lStep);
		}
	}
	return 0;
}

double CScript::SendExpActOnPeace	(const char* cmd, char* retStr)
{
	CMonster* pMonster= dynamic_cast<CMonster*>(p_SrcShape);
	if (pMonster==NULL || pMonster->GetAI()==NULL)
	{
		return 0;
	}

	LONG lActID=GetIntParam(cmd,0);
	LONG lActTime=GetIntParam(cmd,1);
	LONG lAimType=GetIntParam(cmd,2);
	if (lActID==INVILID_VALUE || lActID==ERROR_CODE || lActTime==INVILID_VALUE || lActTime==ERROR_CODE )
	{
		return 0;
	}
	if (lAimType==INVILID_VALUE || lAimType==ERROR_CODE)
	{
		lAimType=0;
	}

	char* cGuidName= GetStringParam(cmd,3);
	CGUID guid=NULL_GUID;
	if (cGuidName!= NULL)
	{
		SetScriptGuidValue(static_cast<CMoveShape*>( this->p_SrcShape ),cGuidName,guid);
	}
	M_FREE( cGuidName, sizeof(char)*MAX_VAR_LEN );
	pMonster->GetAI()->SendExpActOnPeace(lActID,lActTime,lAimType,guid);
	return 1;
}
double CScript::SendExpActOnFight	(const char* cmd, char* retStr)
{
	CMonster* pMonster= dynamic_cast<CMonster*>(p_SrcShape);
	if (pMonster==NULL || pMonster->GetAI()==NULL)
	{
		return 0;
	}

	LONG lActID=GetIntParam(cmd,0);
	LONG lActTime=GetIntParam(cmd,1);
	LONG lAimType=GetIntParam(cmd,2);
	if (lActID==INVILID_VALUE || lActID==ERROR_CODE || lActTime==INVILID_VALUE || lActTime==ERROR_CODE )
	{
		return 0;
	}
	if (lAimType==INVILID_VALUE || lAimType==ERROR_CODE)
	{
		lAimType=0;
	}

	char* cGuidName= GetStringParam(cmd,3);
	CGUID guid=NULL_GUID;
	if (cGuidName!= NULL)
	{
		SetScriptGuidValue(static_cast<CMoveShape*>( this->p_SrcShape ),cGuidName,guid);
	}
	M_FREE( cGuidName, sizeof(char)*MAX_VAR_LEN );
	pMonster->GetAI()->SendExpActOnFight(lActID,lActTime,lAimType,guid);
	return 1;
}

double CScript::MoveByTimeOnPeace	(const char* CmdStr, char* retStr)
{
	CMonster* pMonster= (CMonster*)p_SrcShape;
	if (pMonster)
	{
		LONG lDir=INVILID_VALUE;
		LONG lTime=INVILID_VALUE;
		lDir=GetIntParam(CmdStr,0);
		lTime=GetIntParam(CmdStr,1);
		if (lDir!=INVILID_VALUE && lDir!=ERROR_CODE && lTime!=INVILID_VALUE && lTime!=ERROR_CODE)
		{
			pMonster->GetAI()->MoveByTimeOnPeace(lDir,lTime);
		}
	}
	return 0;
}

double CScript::MovebyTimeOnFight	(const char* CmdStr, char* retStr)
{
	CMonster* pMonster= (CMonster*)p_SrcShape;
	if (pMonster)
	{
		LONG lDir=INVILID_VALUE;
		LONG lTime=INVILID_VALUE;
		lDir=GetIntParam(CmdStr,0);
		lTime=GetIntParam(CmdStr,1);
		if (lDir!=INVILID_VALUE && lDir!=ERROR_CODE && lTime!=INVILID_VALUE && lTime!=ERROR_CODE)
		{
			pMonster->GetAI()->MovebyTimeOnFight(lDir,lTime);
		}
	}
	return 0;
}
double CScript::CallFriendAround	(const char* CmdStr, char* retStr)
{
	CMonster* pMonster= (CMonster*)p_SrcShape;
	if (pMonster== NULL) return 0;

	CServerRegion* pRegion = dynamic_cast<CServerRegion*>( pMonster -> GetFather() );
	if( pRegion == NULL ) return 0;

	if (pMonster->GetAI()== 0) return 0;

	CMoveShape* pTarget = pMonster->GetAI()->GetTarget();
	if (pTarget==NULL)	return 0;

	int distance= GetIntParam(CmdStr,0);
	int state	= GetIntParam(CmdStr,1);
	if (distance==INVILID_VALUE || distance==ERROR_CODE || state==INVILID_VALUE || state==ERROR_CODE)
		return 0;

	vector<CShape*> vOut;
	pRegion -> FindAroundObject( pMonster, TYPE_MONSTER, vOut );

	for( size_t i = 0; i < vOut.size(); i ++ )
	{
		CMonster* pFriend = dynamic_cast<CMonster*>( vOut[i] );
		if( pFriend == NULL || pFriend->GetAI()==NULL ) continue;

		if (pFriend->GetNpcType()== NT_Guard ||
			pFriend->GetIndex()!=pMonster->GetIndex())
		{
			continue;
		}

		if (pFriend->GetState()== CShape::STATE_PEACE && state!= 2 && pFriend -> RealDistance(pMonster) <= distance) 
		{
			pFriend->GetAI()->MoveToCellOnPeace(pTarget->GetTileX(),pTarget->GetTileY());
		}
		else if (pFriend->GetState()==CShape::STATE_FIGHT && state!=1 &&  pFriend -> RealDistance(pMonster) <= distance)
		{
			pFriend->GetAI()->MoveToCellOnFight(pTarget->GetTileX(),pTarget->GetTileY());
		}
	}
	return 0;
}
double CScript::CallMonsterAround	(const char* CmdStr, char* retStr)
{
	CMonster* pMonster= (CMonster*)p_SrcShape;
	if (pMonster== NULL) return 0;

	CServerRegion* pRegion = dynamic_cast<CServerRegion*>( pMonster -> GetFather() );
	if( pRegion == NULL ) return 0;

	if (pMonster->GetAI()== 0) return 0;

	CMoveShape* pTarget = pMonster->GetAI()->GetTarget();
	if (pTarget==NULL)	return 0;

	int distance= GetIntParam(CmdStr,0);
	int state	= GetIntParam(CmdStr,1);
	if (distance==INVILID_VALUE || distance==ERROR_CODE || state==INVILID_VALUE || state==ERROR_CODE)
		return 0;

	vector<CShape*> vOut;
	pRegion -> FindAroundObject( pMonster, TYPE_MONSTER, vOut );

	for( size_t i = 0; i < vOut.size(); i ++ )
	{
		CMonster* pFriend = dynamic_cast<CMonster*>( vOut[i] );
		if( pFriend == NULL || pFriend->GetAI()==NULL ) continue;

		if (pFriend->GetNpcType()== NT_Guard)
		{
			continue;
		}

		//if (state!=0 && pFriend->GetState()!= state) continue;
		if (pFriend->GetState()== CShape::STATE_PEACE && state!= 2 && pFriend -> RealDistance(pMonster) <= distance) 
		{
			pFriend->GetAI()->MoveToCellOnPeace(pTarget->GetTileX(),pTarget->GetTileY());
		}
		else if (pFriend->GetState()==CShape::STATE_FIGHT && state!=1 &&  pFriend -> RealDistance(pMonster) <= distance)
		{
			pFriend->GetAI()->MoveToCellOnFight(pTarget->GetTileX(),pTarget->GetTileY());
		}
	}
	return 0;
}
double CScript::Stand				(const char* CmdStr, char* retStr)
{
	CMonster* pMonster= (CMonster*)p_SrcShape;
	if (pMonster)
	{
		LONG lTimes=GetIntParam(CmdStr,0);
		LONG lTime=GetIntParam(CmdStr,1);
		if (lTime!=INVILID_VALUE && lTime!=ERROR_CODE&&lTimes!=INVILID_VALUE && lTimes!=ERROR_CODE)
		{
			pMonster->GetAI()->Stand(lTimes,lTime);
		}
	}
	return 0;
}

double CScript::StandOnce( const char *cmd, char *ret )
{
	double time = GetIntParam( cmd, 0 );
	if( time == ERROR_CODE )
	{
		return -1;
	}
	CMonster *pMonster = static_cast<CMonster*>( p_SrcShape );
	if( pMonster != NULL && pMonster->GetAI() != NULL )
	{
		pMonster->GetAI()->Stand( time );
	}

	return 0;
}

double CScript::RandomMoveByTime		(const char* CmdStr, char* retStr)
{

	CMonster* pMonster= (CMonster*)p_SrcShape;
	if (pMonster)
	{
		LONG lTime=GetIntParam(CmdStr,0);
		if (lTime!=INVILID_VALUE && lTime!=ERROR_CODE)
		{
			pMonster->GetAI()->RandomMoveByTime(lTime);
		}
	}
	return 0;
}

double CScript::ChangeAI(const char* CmdStr, char* retStr)
{
	CMonster* pMonster= (CMonster*)p_SrcShape;
	if (pMonster)
	{
		LONG lAIType=GetIntParam(CmdStr,0);
		if (lAIType!=INVILID_VALUE && lAIType!=ERROR_CODE && pMonster->GetAI()!=NULL)
		{
			//dynamic_cast<CMonsterAI*>(pMonster->GetAI())->ChangeAI(lAIType);
			((CMonsterAI*)(pMonster->GetAI ()))->AddChangeAIEvent (lAIType);
		}
	}
	return 0;
}

//*************************************************************
double CScript::StartCycle(const char* cmd, char* retStr)
{
	CMonster* pMonster= (CMonster*)p_SrcShape;
	if (pMonster)
	{
		LONG lStartTime=GetIntParam(cmd,0);
		LONG lCycleTime=GetIntParam(cmd,1);
		if (lStartTime!=INVILID_VALUE && lStartTime!=ERROR_CODE && 
			lCycleTime!=INVILID_VALUE && lCycleTime!=ERROR_CODE &&
			pMonster->GetAI()!=NULL)
		{
			pMonster->GetAI()->StartCycle(lStartTime,lCycleTime);
		}
	}
	return 0;
}

double CScript::EndCycle(const char* cmd, char* retStr)
{
	CMonster* pMonster= (CMonster*)p_SrcShape;
	if (pMonster && pMonster->GetAI()!=NULL)
	{
		pMonster->GetAI()->EndCycle();
	}
	return 0;
}
double CScript::EscapeByTimeOnFight(const char* cmd, char* retStr)
{
	CMonster* pMonster= (CMonster*)p_SrcShape;
	if (pMonster)
	{
		LONG lTime=GetIntParam(cmd,0);
		if (lTime!=INVILID_VALUE && lTime!=ERROR_CODE && pMonster->GetAI()!=NULL)
		{
			pMonster->GetAI()->EscapeByTimeOnFight(lTime);
		}
	}
	return 0;
}
double CScript::DeleteSpring(const char* cmd, char* retStr)
{
	CMonster* pMonster= (CMonster*)p_SrcShape;
	if (pMonster && pMonster->GetAI()!=NULL)
	{
		pMonster->GetAI()->DeleteSpring();
	}
	return 0;
}
double CScript::EndEscape(const char* cmd, char* retStr)
{
	CMonster* pMonster= (CMonster*)p_SrcShape;
	if (pMonster && pMonster->GetAI()!=NULL)
	{
		pMonster->GetAI()->EndEscape();
	}
	return 0;
}
double CScript::EndEscapeByGUID(const char* cmd, char* retStr)
{
	char* szGUID = GetStringParam(cmd, 0); 
	if(!szGUID) return 0;
	CGUID monsterGUID = GetScriptGuidValue(static_cast<CMoveShape*>( p_SrcShape),szGUID);
	CServerRegion* pRegion=dynamic_cast<CServerRegion*>(p_SrcShape->GetFather());
	if(pRegion== NULL)
	{
		M_FREE( szGUID, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}
	CMonster* pMonster= dynamic_cast<CMonster*>(pRegion->FindChildObject(TYPE_MONSTER,monsterGUID));
	if (pMonster== NULL || pMonster->GetAI()==NULL)
	{
		M_FREE( szGUID, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}
	pMonster->GetAI()->EndEscape();
	M_FREE( szGUID, sizeof(char)*MAX_VAR_LEN );
	//
	return 0;
}
double CScript::SetBefoolID(const char* cmd, char* retStr)
{
	CPlayer*  pPlayer = (CPlayer*)p_SrcShape;
	CMonster* pMonster= (CMonster*)p_DesShape;
	if (pMonster)
	{
		pMonster->SetBefoolID(pPlayer->GetExID());
	}
	return 0;
}
double CScript::CleanBefoolID(const char* cmd, char* retStr)
{
	CMonster* pMonster= (CMonster*)p_SrcShape;
	if (pMonster)
	{
		pMonster->SetBefoolID(NULL_GUID);
	}
	return 0;
}
double CScript::SetForceTarget(const char* cmd, char* retStr)
{
	CMoveShape* pSrcShape = (CMoveShape*)p_SrcShape;
	CMoveShape* pDesShape=	(CMoveShape*)p_DesShape;
	if(pSrcShape!=NULL && pDesShape!=NULL && pDesShape->GetAI()!=NULL)
	{
		pDesShape->GetAI()->SetForceTarget(pSrcShape->GetType(),pSrcShape->GetExID());
	}
	return 0;
}

double CScript::CleanForceTatget(const char* cmd, char* retStr)
{
	CMoveShape* pSrcShape = (CMoveShape*)p_SrcShape;
	if(pSrcShape!=NULL && pSrcShape->GetAI()!=NULL)
	{
		pSrcShape->GetAI()->CleanForceTarget();
	}
	return 0;
}

//脚本接口
//改变虚弱状态
double CScript::ChangeWeaknessState(const char* cmd, char* retStr)
{
	if (p_SrcShape==NULL) return -1;
	int state= GetIntParam(cmd,0);
	if (state== ERROR_CODE || state== INVILID_VALUE) return -1;
	CMoveShape* pMoveShape= dynamic_cast<CMoveShape*>(p_SrcShape);
	if (pMoveShape== NULL) return -1;
	pMoveShape->ChangeWeaknessState(state!=0);
	return 1;
}

//执行者周围的怪逃跑自己的目标一段时间
double CScript::EscapeByAround(const char* cmd, char* retStr)
{
	//CMoveShape * pMoveShape= dynamic_cast<CMonster*>(p_SrcShape);
	if (p_SrcShape== NULL) return 0;

	CServerRegion* pRegion = dynamic_cast<CServerRegion*>( p_SrcShape -> GetFather() );
	if( pRegion == NULL ) return 0;

	DWORD dwRange= GetIntParam(cmd,0);
	DWORD dwTime= GetIntParam(cmd,1);
	DWORD dwIsSameType= GetIntParam(cmd,2);
	DWORD dwState= GetIntParam(cmd,3);

	if (dwRange==INVILID_VALUE		|| dwRange==ERROR_CODE ||
		dwTime==INVILID_VALUE		|| dwTime==ERROR_CODE ||
		dwIsSameType==INVILID_VALUE || dwIsSameType==ERROR_CODE ||
		dwState==INVILID_VALUE		|| dwState==ERROR_CODE )
	{
		return 0;
	}

	vector<CShape*> vOut;
	pRegion -> FindAroundObject( p_SrcShape, TYPE_MONSTER, vOut );

	for (int a=0; a< vOut.size(); a++)
	{
		CMonster* pOther= dynamic_cast<CMonster*>(vOut[a]);
		if (pOther== NULL) continue;
		if (p_SrcShape->Distance(pOther)> dwRange) continue;
		//if (dwState!=0 && dwState!= pOther->GetState()) continue;
		//if (dwIsSameType!=0 && p_SrcShape->GetIndex()!= pOther->GetIndex())	continue;
		if (dwIsSameType!=0)
		{
			CMonster* pMonster= dynamic_cast<CMonster*>(p_SrcShape);
			if (pMonster==0 || pMonster->GetIndex()!= pOther->GetIndex())	continue;
		}
		if (pOther->GetAI()== NULL) continue;
		if ( pOther->GetState()== CShape::STATE_PEACE &&dwState!=2)
		{
			LONG dir= ::GetLineDir(p_SrcShape->GetTileX(),p_SrcShape->GetTileY(),pOther->GetTileX(),pOther->GetTileY());
			pOther->GetAI()->EscapeByTimeOnPeace(dir,(LONG)dwTime);
		}
		else if (pOther->GetState()== CShape::STATE_FIGHT && dwState!=1)
		{
			pOther->GetAI()->EscapeByTimeOnFight((LONG)dwTime);
		}

	}
	return 0;
}

//执行者周围怪物执行一个脚本					(ben)
double	CScript::MonsterRunScriptsAround(const char* cmd, char* retStr)
{
	if (p_SrcShape==NULL)
	{
		return -1;
	}
	DWORD dwRange= GetIntParam(cmd,0);
	if (dwRange==INVILID_VALUE && dwRange==ERROR_CODE )
	{
		return -1;
	}
	char* cScriptName= GetStringParam(cmd,1);
	if (cScriptName== NULL)
	{
		return -1;
	}


	int iIsKindred= GetIntParam(cmd,2);
	if (iIsKindred==INVILID_VALUE && iIsKindred==ERROR_CODE )
	{
		M_FREE( cScriptName, sizeof(char)*MAX_VAR_LEN );
		iIsKindred= -1;
	}



	CServerRegion* pRegion = dynamic_cast<CServerRegion*>( p_SrcShape -> GetFather() );
	if( pRegion == NULL )
	{
		M_FREE( cScriptName, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}

	vector<CShape*> vOut;
	pRegion -> FindAroundObject( p_SrcShape, TYPE_MONSTER, vOut );

	if (vOut.size()== -1)
	{
		M_FREE( cScriptName, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}

	if (p_SrcShape->GetType() == TYPE_MONSTER && iIsKindred!=0)
	{
		for (int a=0; a<vOut.size(); a++)
		{
			if (vOut[a]== NULL)		continue;
			if (p_SrcShape->Distance(vOut[a])> dwRange) continue;
			if (((CMonster*)p_SrcShape)->GetIndex()!= ((CMonster*)vOut[a])->GetIndex()) continue;

			stRunScript st;
			st.pszFileName = cScriptName;
			st.srcShape = vOut[a];
			st.pRegion = (CRegion*)(p_SrcShape->GetFather());
			st.desShape = NULL;
			RunScript(&st, (char*)GetGame()->GetScriptFileData(cScriptName));
		}
	}
	else
	{
		for (int a=0; a<vOut.size(); a++)
		{
			if (vOut[a]== NULL)		continue;
			if (p_SrcShape->Distance(vOut[a])> dwRange) continue;
			//if (((CMonster*)p_SrcShape)->GetIndex()!= ((CMonster*)vOut[a])->GetIndex()) continue;

			stRunScript st;
			st.pszFileName = cScriptName;
			st.srcShape = vOut[a];
			st.pRegion = (CRegion*)(p_SrcShape->GetFather());
			st.desShape = NULL;
			RunScript(&st, (char*)GetGame()->GetScriptFileData(cScriptName));
		}
	}
	M_FREE( cScriptName, sizeof(char)*MAX_VAR_LEN );
	return 0;
}
//怪物是否是在战斗状态				(ben)
double	CScript::MonsterIsFight(const char* cmd, char* retStr)
{
	CMonster* pMonster= dynamic_cast<CMonster*>(p_SrcShape);
	if (pMonster== NULL)
	{
		return -1;
	}

	WORD state= pMonster->GetState();
	if ( state== CShape::STATE_PEACE)
	{
		return 0;
	}
	else if (state == CShape::STATE_FIGHT)
	{
		return 1.0;
	}
	return -1;
}
//设置怪物的状态		//0= 和平， 1= 战斗			(ben)
double    CScript::SetMonsterFight(const char* cmd, char* retStr)
{
	CMonster* pMonster= dynamic_cast<CMonster*>(p_SrcShape);
	if (pMonster== NULL)
	{
		return -1;
	}

	DWORD dwState= GetIntParam(cmd,0);//pMonster->GetState();
	if (dwState==INVILID_VALUE && dwState==ERROR_CODE )
	{
		return -1;
	}
	if ( dwState== CShape::STATE_PEACE || dwState ==CShape::STATE_FIGHT)
	{
		CMonsterAI *pAI = static_cast<CMonsterAI*>( pMonster->GetAI() );
		if( pAI != NULL )
		{
			pAI->EnterFight();
		}
	}
	return 0;
}
//呼唤周围的同类怪物攻击当前目标		限制状态 0=无限制， 1= 和平， 2= 战斗			(ben)
double	CScript::CallFriendAttack(const char* cmd, char* retStr)
{

	CMonster* pMonster = dynamic_cast<CMonster*>(p_SrcShape);
	if (pMonster== NULL) return -1;
	if (pMonster->GetAI()==NULL) return -1;

	CServerRegion* pRegion = dynamic_cast<CServerRegion*>( p_SrcShape -> GetFather() );
	if( pRegion == NULL ) return -1;

	CMoveShape* pTarget = pMonster->GetAI()->GetTarget();
	if ( pTarget== NULL)	return -1;

	int dwRange= GetIntParam(cmd,0);
	int dwState= GetIntParam(cmd,1);

	if (dwRange==INVILID_VALUE || dwRange==ERROR_CODE ||
		dwState==INVILID_VALUE || dwState==ERROR_CODE)
	{
		return -1;
	}

	vector<CShape*> vOut;
	pRegion -> FindAroundObject( p_SrcShape, TYPE_MONSTER, vOut );

	for (int a=0; a<vOut.size(); a++)
	{
		CMonster* pOther= dynamic_cast<CMonster*>(vOut[a]);
		if (pOther== NULL || pOther->GetAI()== NULL)	continue;
		if (pOther->GetIndex()!= pMonster->GetIndex()) continue;
		if ((pOther->GetState()== CShape::STATE_PEACE && dwState!= 2)||
			(pOther->GetState()== CShape::STATE_FIGHT && dwState!= 1))
		{
			pOther->GetAI()->EnterFight();
			pOther->GetAI()->SetTarget(pTarget->GetType(),pTarget->GetExID());
		}
	}
	return 0;
}
//呼唤周围的所有怪物攻击当前目标		(ben)
double	CScript::CallMonsterAttack(const char* cmd, char* retStr)
{
	CMonster* pMonster = dynamic_cast<CMonster*>(p_SrcShape);
	if (pMonster== NULL) return -1;
	if (pMonster->GetAI()==NULL) return -1;

	CServerRegion* pRegion = dynamic_cast<CServerRegion*>( p_SrcShape -> GetFather() );
	if( pRegion == NULL ) return -1;

	CMoveShape* pTarget = pMonster->GetAI()->GetTarget();
	if ( pTarget== NULL)	return -1;

	int dwRange= GetIntParam(cmd,0);
	int dwState= GetIntParam(cmd,1);

	if (dwRange==INVILID_VALUE || dwRange==ERROR_CODE ||
		dwState==INVILID_VALUE || dwState==ERROR_CODE)
	{
		return -1;
	}

	vector<CShape*> vOut;
	pRegion -> FindAroundObject( p_SrcShape, TYPE_MONSTER, vOut );

	for (int a=0; a<vOut.size(); a++)
	{
		CMonster* pOther= dynamic_cast<CMonster*>(vOut[a]);
		if (pOther== NULL || pOther->GetAI()== NULL)	continue;
		if ((pOther->GetState()== CShape::STATE_PEACE && dwState!= 2)||
			(pOther->GetState()== CShape::STATE_FIGHT && dwState!= 1))
		{
			pOther->GetAI()->EnterFight();
			pOther->GetAI()->SetTarget(pTarget->GetType(),pTarget->GetExID());
		}
	}
	return 0;
}
//--------//执行者一定范围内的指定原始名怪物攻击指定GUID目标	//@目标GUID可能是玩家也可能是怪物，不填则默认为执行当前脚本的玩家或怪物。(ben)
double	CScript::MonsterAttackObjectByAround(const char* cmd, char* retStr)
{
	if (p_SrcShape== NULL) return -1;
	CServerRegion* pRegion = dynamic_cast<CServerRegion*>( p_SrcShape -> GetFather() );
	if( pRegion == NULL ) return -1;

	DWORD dwRange= GetIntParam(cmd, 0);
	if (dwRange==INVILID_VALUE && dwRange==ERROR_CODE )
	{
		return -1;
	}
	char* name= GetStringParam(cmd,1);
	if (name == NULL )
	{
		return -1;
	}
	DWORD dwIndex=CMonsterList::GetPropertyByName( name)->dwIndex;

	DWORD dwState= GetIntParam(cmd,2);
	if (dwState==INVILID_VALUE && dwState==ERROR_CODE )
	{
		M_FREE( name, sizeof(char)*MAX_VAR_LEN );
		return -1;
	}

	CMoveShape* pTarGet=0;

	char* pGuidName = GetStringParam( cmd, 3 );
	if (pGuidName)
	{
		if( strlen( pGuidName ) == 0 )
		{
			pTarGet = static_cast<CMoveShape*>( p_SrcShape );
		}
		else
		{
			CGUID Guid = GetScriptGuidValue(static_cast<CMoveShape*>( p_SrcShape ),pGuidName);
			pTarGet= GetGame()->FindShape(TYPE_MONSTER,Guid);
			if (pTarGet== NULL)
			{
				pTarGet = GetGame() ->FindShape(TYPE_PLAYER, Guid);
			}
		}
		if (pTarGet == NULL)
		{
			M_FREE( pGuidName, sizeof(char)*MAX_VAR_LEN );
			M_FREE( name, sizeof(char)*MAX_VAR_LEN );
			return -1;
		}
	}
	else 
	{
		pTarGet = dynamic_cast<CMoveShape*>(p_SrcShape);
		if ( pTarGet == NULL)	
		{
			M_FREE( pGuidName, sizeof(char)*MAX_VAR_LEN );
			M_FREE( name, sizeof(char)*MAX_VAR_LEN );
			return -1;
		}
	}	

	vector<CShape*> vOut;
	pRegion -> FindAroundObject( p_SrcShape, TYPE_MONSTER, vOut );
	for (int a=0; a<vOut.size(); a++)
	{
		CMonster* pMonster= dynamic_cast<CMonster*>(vOut[a]);
		if (pMonster== NULL)	continue;
		if (pMonster->GetIndex()!= dwIndex)	continue;
		if (pMonster->GetAI()== NULL)	continue;
		// 加入距离过滤
		if( this->p_SrcShape->Distance( pMonster ) > dwRange )
		{
			continue;
		}
		if (dwState==0 || 
			(dwState==1 && pMonster->GetState()== CShape:: STATE_PEACE) || 
			(dwState==2 && pMonster->GetState()== CShape::STATE_FIGHT))
		{
			pMonster->GetAI()->SetTarget(pTarGet->GetType(),pTarGet ->GetExID());
		}
	}

	M_FREE( pGuidName, sizeof(char)*MAX_VAR_LEN );
	M_FREE( name, sizeof(char)*MAX_VAR_LEN );
	return 0;
}

//指定GUID怪物攻击指定GUID目标	//@攻击者GUID如果填空则默认当前执行者。
double	CScript::MonsterAttackObject(const char* cmd, char* retStr)
{
	//CMonster* pMonster = dynamic_cast<CMonster*>(p_SrcShape);
	//if (pMonster == NULL)	return 0;
	CMoveShape* pTarget=0;

	char * SrcGUIDName= GetStringParam(cmd,0);
	if (SrcGUIDName== NULL)	return -1;
	char * DesGUIDName= GetStringParam(cmd,1);
	if (DesGUIDName== NULL)	
	{
		M_FREE( SrcGUIDName, sizeof(char)*MAX_VAR_LEN );
		return -1;
	}

   const CGUID &Guid = GetScriptGuidValue(static_cast<CMoveShape*>( p_SrcShape),DesGUIDName);
	pTarget= GetGame()->FindShape(TYPE_MONSTER,Guid);
	if (pTarget== NULL)
	{
		pTarget = GetGame() ->FindShape(TYPE_PLAYER,Guid);
	}
	if (pTarget == NULL)
	{
		M_FREE( SrcGUIDName, sizeof(char)*MAX_VAR_LEN );
		M_FREE( DesGUIDName, sizeof(char)*MAX_VAR_LEN );
		return -1;
	}

	CMonster * pMonster=0;

	if (SrcGUIDName != NULL)
	{
		if( strlen( SrcGUIDName ) == 0 )
		{
			pMonster = static_cast<CMonster*>( this->p_SrcShape );
		}
		else
		{
			CGUID Guid = GetScriptGuidValue(static_cast<CMoveShape*>(p_SrcShape),SrcGUIDName);
			pMonster= dynamic_cast<CMonster*>( GetGame()->FindShape(TYPE_MONSTER,Guid));
		}
		if (pMonster == NULL )
		{
			M_FREE( SrcGUIDName, sizeof(char)*MAX_VAR_LEN );
			M_FREE( DesGUIDName, sizeof(char)*MAX_VAR_LEN );
			return -1;
		}
	}
	else 
	{
		pMonster= dynamic_cast<CMonster*>(p_SrcShape);
		if (pMonster == NULL)
		{
			M_FREE( SrcGUIDName, sizeof(char)*MAX_VAR_LEN );
			M_FREE( DesGUIDName, sizeof(char)*MAX_VAR_LEN );
			return -1;
		}
	}

	if (pMonster->GetAI()== NULL)
	{
		M_FREE( SrcGUIDName, sizeof(char)*MAX_VAR_LEN );
		M_FREE( DesGUIDName, sizeof(char)*MAX_VAR_LEN );
		return -1;
	}

	pMonster->GetAI()->SetTarget(pTarget->GetType(), pTarget ->GetExID());

	M_FREE( SrcGUIDName, sizeof(char)*MAX_VAR_LEN );
	M_FREE( DesGUIDName, sizeof(char)*MAX_VAR_LEN );
	return 0;
}

//改变指定GUID怪物的面向
double	CScript::SetMonsterDir(const char* cmd, char* retStr)
{
	char* cGuidName= GetStringParam(cmd,0);
	if(!cGuidName) return 0;

	int  iDir=	GetIntParam(cmd,1);
	if (iDir==INVILID_VALUE && iDir==ERROR_CODE )
	{
		M_FREE( cGuidName, sizeof(char)*MAX_VAR_LEN );
		return -1;
	}

	CMoveShape* pMoveShape=0;
	if (cGuidName != NULL)
	{	
		if( strlen( cGuidName ) == 0 )
		{
			pMoveShape = static_cast<CMoveShape*>( p_SrcShape );
		}
		else
		{
			CGUID guid=GetScriptGuidValue(static_cast<CMoveShape*>(p_SrcShape),cGuidName);
			pMoveShape= dynamic_cast<CMoveShape*>(GetGame()->FindShape(TYPE_MONSTER,guid ));
		}
		if (pMoveShape== NULL )
		{
			M_FREE( cGuidName, sizeof(char)*MAX_VAR_LEN );
			return -1;
		}
	}
	else
	{
		pMoveShape= dynamic_cast<CMoveShape*>(p_SrcShape);
		if (pMoveShape== NULL)
		{
			M_FREE( cGuidName, sizeof(char)*MAX_VAR_LEN );
			return -1;
		}
	}

	pMoveShape->SetDir(iDir%8);
	CMessage Msg( MSG_S2C_SHAPE_CHANGEDIR );
	Msg.Add( static_cast<BYTE>(pMoveShape->GetDir()) );
	Msg.Add( pMoveShape->GetDirEx());
	Msg.Add( pMoveShape->GetType() );
	Msg.Add( pMoveShape->GetExID() );

	Msg.SendToAround( pMoveShape );

	M_FREE( cGuidName, sizeof(char)*MAX_VAR_LEN );
	return 0;
}

//指定GUID怪物进行归位处理
double	CScript::ReLiveMonster(const char* cmd, char* retStr)
{
	char* cGuidName= GetStringParam(cmd,0);
	CMonster* pMonster=0;
	if (cGuidName!= NULL)
	{
		if( strlen( cGuidName ) == 0 )
		{
			pMonster = static_cast<CMonster*>( p_SrcShape );
		}
		else
		{
			const CGUID &Guid= GetScriptGuidValue(static_cast<CMoveShape*>(p_SrcShape),cGuidName);
			pMonster= dynamic_cast<CMonster*>(GetGame()->FindShape(TYPE_MONSTER, Guid));
		}
		if (pMonster== NULL )
		{
			M_FREE( cGuidName, sizeof(char)*MAX_VAR_LEN );
			return -1;
		}

	}
	else 
	{
		pMonster = dynamic_cast<CMonster*>(p_SrcShape);
		if (pMonster == NULL)
		{
			M_FREE( cGuidName, sizeof(char)*MAX_VAR_LEN );
			return -1;
		}

	}

	if (pMonster->GetAI()== NULL){
		M_FREE( cGuidName, sizeof(char)*MAX_VAR_LEN );
		return -1;
	}

	if (pMonster->GetState()== CShape::STATE_FIGHT)
	{
		pMonster->GetAI() ->OnReturnPeace();
	}
	M_FREE( cGuidName, sizeof(char)*MAX_VAR_LEN );
	return 0;
}

//返回指定GUID怪物目标的GUID
double	CScript::GetMonsterTarget(const char* cmd, char* retStr)
{
	char* cGuidName= GetStringParam(cmd,0);
	if(!cGuidName) return -1;

	char* cGuidRet=	GetStringParam(cmd,1);
	if (cGuidRet== NULL)	
	{
		M_FREE( cGuidName, sizeof(char)*MAX_VAR_LEN );
		return -1;
	}

	CMonster *pMonster = 0;
	if (cGuidName!= NULL)
	{
		if( strlen( cGuidName ) == 0 )
		{
			pMonster = static_cast<CMonster*>( p_SrcShape );
		}
		else
		{
			const CGUID &guid=GetScriptGuidValue(static_cast<CMoveShape*>(p_SrcShape),cGuidName);
			pMonster = dynamic_cast<CMonster*>(GetGame()->FindShape(TYPE_MONSTER,guid));
		}
		if (pMonster== NULL)
		{
			M_FREE( cGuidName, sizeof(char)*MAX_VAR_LEN );
			M_FREE( cGuidRet, sizeof(char)*MAX_VAR_LEN );
			return -1;
		}
	}
	else 
	{
		pMonster = dynamic_cast<CMonster*>(p_SrcShape);
		if ( pMonster== NULL) 
		{
			M_FREE( cGuidName, sizeof(char)*MAX_VAR_LEN );
			M_FREE( cGuidRet, sizeof(char)*MAX_VAR_LEN );
			return -1;
		}
	}

	SetScriptGuidValue(pMonster,cGuidRet,pMonster->GetAI()->GetTargetID());

	M_FREE( cGuidName, sizeof(char)*MAX_VAR_LEN );
	M_FREE( cGuidRet, sizeof(char)*MAX_VAR_LEN );
	return 0;
}

//设置指定GUID怪物的目标
double	CScript::SetMonsterTarget(const char* cmd, char* retStr)
{
	char * cSrcGuidName= GetStringParam(cmd,0);
	if(!cSrcGuidName) return -1;
	char * cTargetGuidName= GetStringParam(cmd,1);
	if (cTargetGuidName== NULL)	
	{
		M_FREE( cSrcGuidName, sizeof(char)*MAX_VAR_LEN );
		return -1;
	}
	const CGUID &Guid= GetScriptGuidValue(static_cast<CMoveShape*>(p_SrcShape),cTargetGuidName);
	CMoveShape* pTarget= dynamic_cast<CMoveShape*>(GetGame()->FindShape(TYPE_MONSTER,Guid));
	if (pTarget== NULL)
	{
		pTarget = dynamic_cast<CMoveShape*>(GetGame()->FindShape(TYPE_PLAYER,Guid));
	}
	if (pTarget == NULL)	
	{
		M_FREE( cSrcGuidName, sizeof(char)*MAX_VAR_LEN );
		M_FREE( cTargetGuidName, sizeof(char)*MAX_VAR_LEN );
		return -1;
	}

	CMonster* pMonster=0;

	if (cSrcGuidName!= NULL)
	{
		if( strlen( cSrcGuidName ) == 0 )
		{
			pMonster = static_cast<CMonster*>( p_SrcShape );
		}
		else
		{
			const CGUID &Guid= GetScriptGuidValue(static_cast<CMoveShape*>(p_SrcShape),cSrcGuidName);
			pMonster = dynamic_cast<CMonster*>(GetGame()->FindShape(TYPE_MONSTER,Guid));
		}
		if (pMonster == NULL)	
		{
			M_FREE( cSrcGuidName, sizeof(char)*MAX_VAR_LEN );
			M_FREE( cTargetGuidName, sizeof(char)*MAX_VAR_LEN );
			return -1;
		}
	}
	else 
	{
		pMonster= dynamic_cast<CMonster*>(p_SrcShape);
		if (pMonster == NULL )
		{
			M_FREE( cSrcGuidName, sizeof(char)*MAX_VAR_LEN );
			M_FREE( cTargetGuidName, sizeof(char)*MAX_VAR_LEN );
			return -1;
		}
	}

	if (pMonster->GetAI() == NULL)
	{
		M_FREE( cSrcGuidName, sizeof(char)*MAX_VAR_LEN );
		M_FREE( cTargetGuidName, sizeof(char)*MAX_VAR_LEN );
		return -1;
	}
	pMonster->GetAI()->SetTarget(pTarget->GetType(),pTarget-> GetExID());
	M_FREE( cSrcGuidName, sizeof(char)*MAX_VAR_LEN );
	M_FREE( cTargetGuidName, sizeof(char)*MAX_VAR_LEN );
	return 0;
}


double CScript::GetMonsterPos( const char *cmd, char *retStr )
{
	char *SelfID = GetStringParam( cmd, 0 );
	char *XName = GetStringParam( cmd, 1 );
	char *YName = GetStringParam( cmd, 2 );

	if( XName == NULL || YName == NULL )
	{
		// invalid parameters
		M_FREE( XName, sizeof(char)*MAX_VAR_LEN );
		M_FREE( YName, sizeof(char)*MAX_VAR_LEN );
		M_FREE( SelfID, sizeof(char)*MAX_VAR_LEN );
		return -1;
	}

	CShape *pSelf = NULL;
	if( SelfID != NULL && strlen( SelfID ) > 0 )
	{
		const CGUID &guid = GetScriptGuidValue( static_cast<CMoveShape*>( p_SrcShape ), SelfID );
		pSelf = GetGame()->FindShape( TYPE_MONSTER, guid );
	}
	else
	{
		pSelf = this->p_SrcShape;
	}

	if( pSelf == NULL )
	{
		// error
		M_FREE( XName, sizeof(char)*MAX_VAR_LEN );
		M_FREE( YName, sizeof(char)*MAX_VAR_LEN );
		M_FREE( SelfID, sizeof(char)*MAX_VAR_LEN );
		return -1;
	}

	double x = static_cast<double>( pSelf->GetPosX() );
	double y = static_cast<double>( pSelf->GetPosY() );

	// set return value
	SetScriptVarValue( static_cast<CMoveShape*>( pSelf ), XName, 0, x );
	SetScriptVarValue( static_cast<CMoveShape*>( pSelf ), YName, 0, y );

	M_FREE( XName, sizeof(char)*MAX_VAR_LEN );
	M_FREE( YName, sizeof(char)*MAX_VAR_LEN );
	M_FREE( SelfID, sizeof(char)*MAX_VAR_LEN );

	return 0;
}

double CScript::CreateMonster(const char* CmdStr, char* retStr)
{
	// 返回的guid变量名
	char* szRetGUID = GetStringParam(CmdStr, 0);

	//##获取lRegionID的编号，如果是0，则设置为当前场景的id
	double rgnID	= GetIntParam( CmdStr, 9 );

	char* p			= GetStringParam( CmdStr, 1 );
	if( p != NULL )	
	{
		double nCounts 	= GetIntParam( CmdStr, 2 );
		double nLeft		= GetIntParam( CmdStr, 3 );
		double nTop		= GetIntParam( CmdStr, 4 );
		double nRight		= GetIntParam( CmdStr, 5 );
		double nBottom	= GetIntParam( CmdStr, 6 );
		char* strScript = GetStringParam( CmdStr, 7 );
		char* strTalk	= GetStringParam(CmdStr,8);
		int   iDir		= GetIntParam(CmdStr,10);

		if( nCounts == ERROR_CODE || nLeft == ERROR_CODE || nRight == ERROR_CODE ||
			nTop == ERROR_CODE || nBottom == ERROR_CODE )
		{
			M_FREE( szRetGUID, sizeof(char)*MAX_VAR_LEN );
			M_FREE( p, sizeof(char)*MAX_VAR_LEN );
			M_FREE( strTalk, sizeof(char)*MAX_VAR_LEN );
			M_FREE( strScript, sizeof(char)*MAX_VAR_LEN );
			return (long)(&NULL_GUID);
		}
		if (iDir==ERROR_CODE || iDir== INVILID_VALUE )
		{
			iDir=-1;
		}

		CServerRegion* pRegion = NULL;
		
		pRegion = GameManager::GetInstance()->GetRgnManager()->FindRgnByTemplateID(RGN_NORMAL, rgnID);
		if(pRegion == NULL)
			pRegion = dynamic_cast<CServerRegion*>(m_pRegion);

		if( pRegion )
		{
			LONG lNewX = 0; 
			LONG lNewY = 0;
			pRegion -> GetRandomPosInRange( lNewX, lNewY, nLeft, nTop, nRight - nLeft, nBottom - nTop );
			CMonster* pLeader = pRegion -> AddMonster( p, lNewX, lNewY, iDir );
			if( pLeader == NULL ) 
			{
				M_FREE( szRetGUID, sizeof(char)*MAX_VAR_LEN );
				M_FREE( p, sizeof(char)*MAX_VAR_LEN );
				M_FREE( strTalk, sizeof(char)*MAX_VAR_LEN );
				M_FREE( strScript, sizeof(char)*MAX_VAR_LEN );
				return 0.0;
			}

			// 添加脚本
			if (strScript && strcmp(strScript,"0")!=0)
			{
				pLeader->SetScriptFile(strScript);
			}
			if (strTalk && strcmp(strTalk,"0")!=0)
			{
				pLeader->SetTalkScriptFile(strTalk);
			}

			//			ret = pLeader -> GetExID();

			for (int i=0; i<nCounts - 1; i++)
			{
				pRegion -> GetRandomPosInRange( lNewX, lNewY, nLeft, nTop, nRight - nLeft, nBottom - nTop );
				pLeader = pRegion -> AddMonster( p, lNewX, lNewY, iDir );
				if (pLeader)
				{
					if (strScript)
					{
						pLeader->SetScriptFile(strScript);
					}
					if (strTalk)
					{
						pLeader->SetTalkScriptFile(strTalk);
					}
				}
			}

			if( szRetGUID != NULL && strlen( szRetGUID) > 0 )
			{
				SetScriptGuidValue( static_cast<CMoveShape*>( this->p_SrcShape ), szRetGUID, pLeader->GetExID());
			}
		}
		else 
		{
			//##发送消息给World Server，发送后不管
			CMessage msg( MSG_S2W_SERVER_CREATEMONSTER );
			if(pRegion)
				msg.Add( pRegion->GetExID() );
			else
				msg.Add(NULL_GUID);
			msg.Add( p );
			msg.Add( (long)nCounts );
			msg.Add( (long)nLeft );
			msg.Add( (long)nTop );
			msg.Add( (long)nRight );
			msg.Add( (long)nBottom );
			if( strScript )
			{
				msg.Add( BYTE(1) );
				msg.Add( strScript );
			}
			else 
			{
				msg.Add( BYTE(0) );
			}					
			msg.Send();
		}

		M_FREE( p, sizeof(char)*MAX_VAR_LEN );
		M_FREE( strTalk, sizeof(char)*MAX_VAR_LEN );
		M_FREE( strScript, sizeof(char)*MAX_VAR_LEN );
	}

	M_FREE( szRetGUID, sizeof(char)*MAX_VAR_LEN );
	return 1.0f;
}

double CScript::CreateMonsterEx(const char* CmdStr, char* retStr)
{
	// 返回的guid变量名
	char* szRetGUID = GetStringParam(CmdStr, 0);

	//##获取lRegionID的编号，如果是0，则设置为当前场景的id
	char* rgnID	= GetStringParam( CmdStr, 9 );
	CGUID lRegionID;

	if(CScript::GetGeneralVariableList())
		lRegionID = CScript::GetGeneralVariableList()->GetGuidValue(rgnID);
	if(lRegionID == NULL_GUID && p_SrcShape)
		lRegionID = ((CPlayer*)p_SrcShape)->GetVariableList()->GetGuidValue(rgnID);
	if(lRegionID == NULL_GUID)
		lRegionID = GetScriptGuidValue(static_cast<CMoveShape*>(p_SrcShape),rgnID);

	if( lRegionID == NULL_GUID && m_pRegion )
	{
		lRegionID = m_pRegion->GetExID();
	}

	char* p			= GetStringParam( CmdStr, 1 );
	if( p != NULL )	
	{
		double nCounts 	= GetIntParam( CmdStr, 2 );
		double nLeft		= GetIntParam( CmdStr, 3 );
		double nTop		= GetIntParam( CmdStr, 4 );
		double nRight		= GetIntParam( CmdStr, 5 );
		double nBottom	= GetIntParam( CmdStr, 6 );
		char* strScript = GetStringParam( CmdStr, 7 );
		char* strTalk	= GetStringParam(CmdStr,8);
		double rgnType		= GetIntParam( CmdStr, 10 );
		int		iDir		= GetIntParam(CmdStr,11);

		if( nCounts == ERROR_CODE || nLeft == ERROR_CODE || nRight == ERROR_CODE ||
			nTop == ERROR_CODE || nBottom == ERROR_CODE )
		{
			M_FREE( szRetGUID, sizeof(char)*MAX_VAR_LEN );
			M_FREE( rgnID, sizeof(char)*MAX_VAR_LEN );
			M_FREE( p, sizeof(char)*MAX_VAR_LEN );
			M_FREE( strTalk, sizeof(char)*MAX_VAR_LEN );
			M_FREE( strScript, sizeof(char)*MAX_VAR_LEN );
			return 0.0f;
		}
		if (iDir== ERROR_CODE || iDir== INVILID_VALUE)
		{
			iDir=-1;
		}

		CServerRegion* pRegion	= NULL;

		switch((eRgnType)(long)rgnType)
		{
		case RGN_NORMAL:
			pRegion = GameManager::GetInstance()->GetRgnManager()->FindRgnByRgnGUID(RGN_NORMAL, lRegionID);
			break;
		case RGN_PERSONAL_HOUSE:
			pRegion = GameManager::GetInstance()->GetRgnManager()->FindRgnByRgnGUID(RGN_PERSONAL_HOUSE, lRegionID);
			break;
		case RGN_PERSONAL:
			pRegion = GameManager::GetInstance()->GetRgnManager()->FindRgnByRgnGUID(RGN_PERSONAL, lRegionID);
			break;
		case RGN_TEAM:
			pRegion = GameManager::GetInstance()->GetRgnManager()->FindRgnByRgnGUID(RGN_TEAM, lRegionID);
			break;
		}

		if( pRegion == NULL )
			pRegion = static_cast<CServerRegion*>( m_pRegion );

		if( pRegion )
		{
			LONG lNewX = 0; 
			LONG lNewY = 0;
			pRegion -> GetRandomPosInRange( lNewX, lNewY, nLeft, nTop, nRight - nLeft, nBottom - nTop );
			CMonster* pLeader = pRegion -> AddMonster( p, lNewX, lNewY, iDir );
			if( pLeader == NULL ) 
			{
				M_FREE( szRetGUID, sizeof(char)*MAX_VAR_LEN );
				M_FREE( rgnID, sizeof(char)*MAX_VAR_LEN );
				M_FREE( p, sizeof(char)*MAX_VAR_LEN );
				M_FREE( strTalk, sizeof(char)*MAX_VAR_LEN );
				M_FREE( strScript, sizeof(char)*MAX_VAR_LEN );
				return 0;
			}

			// 添加脚本
			if (strScript && strcmp(strScript,"0")!=0)
			{
				pLeader->SetScriptFile(strScript);
			}
			if (strTalk && strcmp(strTalk,"0")!=0)
			{
				pLeader->SetTalkScriptFile(strTalk);
			}					

			for (int i=0; i<nCounts - 1; i++)
			{
				pRegion -> GetRandomPosInRange( lNewX, lNewY, nLeft, nTop, nRight - nLeft, nBottom - nTop );
				pLeader = pRegion -> AddMonster( p, lNewX, lNewY, -1 );
				if (pLeader)
				{
					if (strScript)
					{
						pLeader->SetScriptFile(strScript);
					}
					if (strTalk)
					{
						pLeader->SetTalkScriptFile(strTalk);
					}
				}
			}

			if( szRetGUID != NULL && strlen( szRetGUID) > 0 )
			{
				SetScriptGuidValue( static_cast<CMoveShape*>( this->p_SrcShape ), szRetGUID, pLeader->GetExID());
			}

		}
		else 
		{
			//##发送消息给World Server，发送后不管
			CMessage msg( MSG_S2W_SERVER_CREATEMONSTER );
			msg.Add( lRegionID );
			msg.Add( p );
			msg.Add( (long)nCounts );
			msg.Add( (long)nLeft );
			msg.Add( (long)nTop );
			msg.Add( (long)nRight );
			msg.Add( (long)nBottom );
			if( strScript )
			{
				msg.Add( BYTE(1) );
				msg.Add( strScript );
			}
			else 
			{
				msg.Add( BYTE(0) );
			}					
			msg.Send();
		}

		
		M_FREE( p, sizeof(char)*MAX_VAR_LEN );
		M_FREE( strTalk, sizeof(char)*MAX_VAR_LEN );
		M_FREE( strScript, sizeof(char)*MAX_VAR_LEN );
	}

	M_FREE( szRetGUID, sizeof(char)*MAX_VAR_LEN );
	M_FREE( rgnID, sizeof(char)*MAX_VAR_LEN );
	return 0.0f;
}
double CScript::DelMonster(const char* CmdStr, char* retStr)
{
	if(p_SrcShape == NULL) return 0;

	double ret = 0;
	char *pGuidName = GetStringParam( CmdStr, 0 );
	if (NULL == pGuidName) return ret;

	CGUID guid = GetScriptGuidValue(static_cast<CMoveShape*>(p_SrcShape),pGuidName);

	if( guid == NULL_GUID ) 
	{
		M_FREE( pGuidName, sizeof(char)*MAX_VAR_LEN );
		return ret;
	}

	CServerRegion* pRegion = reinterpret_cast<CServerRegion*>( p_SrcShape -> GetFather() );
	CMoveShape* pShape = NULL;
	if( pRegion ) 
		pShape = dynamic_cast<CMoveShape*>( pRegion -> FindChildObject( TYPE_MONSTER, guid) );
	if( pRegion && pShape )
	{
		pRegion->DelShapeToAround(pShape);
		pShape->AddDelEvent(0);
		ret = 1;
	}

	M_FREE( pGuidName, sizeof(char)*MAX_VAR_LEN );
	return 1;
}
double CScript::DelMonsterEx(const char* cmd, char* retStr)
{
	if(p_SrcShape == NULL) return 0;

	double ret = 0;
	char *pGuidName = GetStringParam( cmd, 0 );
	if (NULL == pGuidName) return ret;

	CGUID guid = GetScriptGuidValue(static_cast<CMoveShape*>(p_SrcShape),pGuidName);

	if( guid == NULL_GUID ) 
	{
		M_FREE( pGuidName, sizeof(char)*MAX_VAR_LEN );
		return ret;
	}

	CServerRegion* pRegion = reinterpret_cast<CServerRegion*>( p_SrcShape -> GetFather() );
	CMonster* pShape = NULL;
	if( pRegion ) 
		pShape = dynamic_cast<CMonster*>( pRegion -> FindChildObject( TYPE_MONSTER, guid) );
	if( pRegion && pShape )
	{
		pRegion->DelShapeToAround(pShape);

		if (pShape->GetAI())
		{
			pShape->SetState(CShape::STATE_DIED);
			pShape->GetAI()->LogoutCurrentEvent();
			pShape->EndAllActiveSkill();
			pShape->GetAI()->StartDiedEvent(0);
		}
		else
			pShape->AddDelEvent(0);

	}

	M_FREE( pGuidName, sizeof(char)*MAX_VAR_LEN );
	return 1;
}



double CScript::KillMonster(const char* CmdStr, char* retStr)
{
	if(p_SrcShape == NULL) return 0;

	char* pPlayerGuidName = GetStringParam( CmdStr, 0 );
	if (NULL == pPlayerGuidName)
	{
		return 0;
	}
	CGUID guid = GetScriptGuidValue(static_cast<CMoveShape*>(p_SrcShape),pPlayerGuidName);
	if( NULL_GUID == guid )
	{
		M_FREE( pPlayerGuidName, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}

	CServerRegion* pRegion = reinterpret_cast<CServerRegion*>( p_SrcShape -> GetFather() );
	CMonster* pShape = NULL;
	if( pRegion )
		pShape = dynamic_cast<CMonster*>( pRegion -> FindChildObject( TYPE_MONSTER, guid) );
	if( pShape )
	{
		//pShape -> Talk( "靠。。耍无聊哈。" );
		/**## 通知客户端怪物杀死信息 */
		tagAttackInformation ai;
		ai.lAttackerType	= p_SrcShape -> GetType();
		ai.AttackerID		= p_SrcShape -> GetExID();
		tagAttackPower* pDamage	= MP_NEW tagAttackPower; /* memory leak, not be used ?*/
		if( pDamage )
		{
			pDamage -> atAttackType	= ATTACK_TYPE_PHYSICAL;
			pDamage -> lHPDamage	= 0x7fffffff;
			ai.vDamage.push_back( pDamage );
		}
		//pShape -> OnBeenAttacked( ai );
	}
	M_FREE( pPlayerGuidName, sizeof(char)*MAX_VAR_LEN );
	return 0;
}

double CScript::DelMonsterRect(const char* CmdStr, char* retStr)
{
	CServerRegion *pRegion = NULL;
	int templateID = GetIntParam( CmdStr, 0 );
	if( templateID == ERROR_CODE )
	{
		pRegion = static_cast<CServerRegion*>( this->m_pRegion );
	}
	else
	{
		pRegion = GameManager::GetInstance()->GetRgnManager()->FindRgnByTemplateID(RGN_NORMAL, templateID);
		if(pRegion == NULL)
			pRegion = GameManager::GetInstance()->GetRgnManager()->FindRgnByTemplateID(RGN_PERSONAL, templateID);
		if(pRegion == NULL)
			pRegion = GameManager::GetInstance()->GetRgnManager()->FindRgnByTemplateID(RGN_PERSONAL_HOUSE, templateID);
		if(pRegion == NULL)
			pRegion = GameManager::GetInstance()->GetRgnManager()->FindRgnByTemplateID(RGN_TEAM, templateID);
		if( pRegion == NULL ) 
		{
			pRegion = static_cast<CServerRegion*>( this->m_pRegion );
		}
	}
	if( pRegion )
	{
		//##获取四边形的顶点
		LONG lTopLeftX		= GetIntParam( CmdStr, 1 );
		LONG lTopLeftY		= GetIntParam( CmdStr, 2 );
		LONG lBottomRightX	= GetIntParam( CmdStr, 3 );
		LONG lBottomRightY	= GetIntParam( CmdStr, 4 );

		if( lTopLeftX == ERROR_CODE &&
			lTopLeftY == ERROR_CODE &&
			lBottomRightX == ERROR_CODE &&
			lBottomRightY == ERROR_CODE )
		{
			lTopLeftX		= 0;
			lTopLeftY		= 0;
			lBottomRightX	= pRegion -> GetWidth() - 1;
			lBottomRightY	= pRegion -> GetHeight() - 1;
		}

		//##验证顶点的有效性
		if( lTopLeftX < 0 )
		{
			lTopLeftX = 0;
		}
		if( lTopLeftY < 0 )
		{
			lTopLeftY = 0;
		}
		if( lBottomRightX < 0 )
		{
			lBottomRightX = 0;
		}
		if( lBottomRightY < 0 )
		{
			lBottomRightY = 0;
		}

		if( lTopLeftX >= pRegion -> GetWidth() )
		{
			lTopLeftX = pRegion -> GetWidth() - 1;
		}
		if( lTopLeftY >= pRegion -> GetHeight() )
		{
			lTopLeftY = pRegion -> GetHeight() - 1;
		}
		if( lBottomRightX >= pRegion -> GetWidth() )
		{
			lBottomRightX = pRegion -> GetWidth() - 1;
		}
		if( lBottomRightY >= pRegion -> GetHeight() )
		{
			lBottomRightY = pRegion -> GetHeight() - 1;
		}

		//##验证区域的有效性
		if( lBottomRightY <= lTopLeftY ||
			lBottomRightX <= lTopLeftX )
		{
			return 0;
		}

		BEGIN_LOOP_REGION_SHAPE_BY_TYPE( pRegion, TYPE_MONSTER )
		{
			CMonster* pMonster = dynamic_cast<CMonster*>( pShape );
			if( pMonster )
			{
				if( pMonster -> GetTileX() <= lBottomRightX &&
					pMonster -> GetTileX() >= lTopLeftX &&
					pMonster -> GetTileY() <= lBottomRightY &&
					pMonster -> GetTileY() >= lTopLeftY )
				{
					pRegion->DelShapeToAround(pMonster);	

					pMonster->AddDelEvent(0);
				}
			}
		}
		END_LOOP_REGION_SHAPE_BY_TYPE
	}
	return 1;
}

double CScript::GetMonsterRefeashTime(const char* CmdStr, char* retStr)
{
	char* szID = GetStringParam(CmdStr,0);
	if(!szID) return 0;

	CGUID lRegionID(szID);

	double lMonsterIndex = GetIntParam(CmdStr, 1);

	if (lRegionID == NULL_GUID || lMonsterIndex == 0)
	{
		M_FREE( szID, sizeof(char)*MAX_VAR_LEN );
		return -1;
	}

	CServerRegion* pRegion = GetGame()->FindRegion(lRegionID);
	if (pRegion)
	{
		CServerRegion::tagMonster *pMonsterRefeash = pRegion->GetMonsterRefeash(lMonsterIndex);
		if (pMonsterRefeash && pMonsterRefeash->lResetTime)
		{
			double lNextTime = pMonsterRefeash->lResetTime - (timeGetTime() - pMonsterRefeash->dwLastResetTime);
			if  (lNextTime<0)
				lNextTime = 0;
			M_FREE( szID, sizeof(char)*MAX_VAR_LEN );
			return lNextTime/1000;
		}
	}
	M_FREE( szID, sizeof(char)*MAX_VAR_LEN );
	return 0;
}

double CScript::GetPlayerByDistance( const char *cmd, char *retStr )
{
	double distance = GetIntParam( cmd, 0 );
	if( distance == ERROR_CODE )
	{
		return -1;
	}
	char *strGUID = GetStringParam( cmd, 1 );
	if( strGUID == NULL || strlen( strGUID ) == 0 )
	{
		M_FREE( strGUID, sizeof(char)*MAX_VAR_LEN );
		return -1;
	}


	// 死亡判定标志
	int iDiedFlag = GetIntParam( cmd, 2 );
	// 隐身判定标志
	int iHideFlag = GetIntParam( cmd, 3 );

	ADJUST_V( iDiedFlag );
	ADJUST_V( iHideFlag );

	vector<CPlayer*> player_list;
	CServerRegion *pRegion = static_cast<CServerRegion*>( this->p_SrcShape->GetFather() );
	pRegion->FindAroundPlayer( this->p_SrcShape, player_list );
	for( vector<CPlayer*>::iterator it = player_list.begin(); it != player_list.end(); ++ it )
	{
		if( this->p_SrcShape->Distance( *it ) == distance && 
			( iDiedFlag == 1 || !iDiedFlag && !(*it)->IsDied() ) &&
			( iHideFlag == 1 || !iHideFlag && (*it)->IsVisible( static_cast<CMoveShape*>( p_SrcShape ) ) && 
			!AIUtilities::IsInGodState( *it ) ) )
		{
			// find it.
			SetScriptGuidValue( static_cast<CMoveShape*>( this->p_SrcShape ), strGUID, (*it)->GetExID() );
			M_FREE( strGUID, sizeof(char)*MAX_VAR_LEN );
			return 0;
		}
	}

	// not found
	SetScriptGuidValue( static_cast<CMoveShape*>( this->p_SrcShape ), strGUID, NULL_GUID );
	M_FREE( strGUID, sizeof(char)*MAX_VAR_LEN );
	return -1;
}

double CScript::GetDistanceFromPlayer( const char *cmd, char *retStr )
{
	char *strPlayerID = GetStringParam( cmd, 0 );
	char *strRetDis = GetStringParam( cmd, 1 );
	if( strPlayerID == NULL || strlen( strPlayerID ) == 0 || strRetDis == NULL || strlen( strRetDis ) == 0 )
	{
		M_FREE( strRetDis, sizeof(char)*MAX_VAR_LEN );
		M_FREE( strPlayerID, sizeof(char)*MAX_VAR_LEN );
		return -1;
	}
	const CGUID &playerID = GetScriptGuidValue( static_cast<CMoveShape*>( this->p_SrcShape ), strPlayerID );
	CPlayer *pPlayer = GetGame()->FindPlayer( playerID );
	if( pPlayer == NULL )
	{
		// set the result
		SetScriptVarValue( static_cast<CMoveShape*>( this->p_SrcShape ), strRetDis, 0, -1 );
		M_FREE( strRetDis, sizeof(char)*MAX_VAR_LEN );
		M_FREE( strPlayerID, sizeof(char)*MAX_VAR_LEN );
		return -1;
	}

	float dis = this->p_SrcShape->Distance( pPlayer );
	SetScriptVarValue( static_cast<CMoveShape*>( this->p_SrcShape ), strRetDis, 0, dis );

	M_FREE( strRetDis, sizeof(char)*MAX_VAR_LEN );
	M_FREE( strPlayerID, sizeof(char)*MAX_VAR_LEN );
	return 0;
}

double CScript::GetAttackablePlayer( const char *cmd, char *retStr )
{
	if( this->p_SrcShape == NULL || this->p_SrcShape->GetFather() == NULL )
	{
		return -1;
	}
	double range = GetIntParam( cmd, 0 );
	if( range == ERROR_CODE )
	{
		return -1;
	}
	char *strGUID = GetStringParam( cmd, 1 );
	if( strGUID == NULL || strlen( strGUID ) == 0 )
	{
		M_FREE( strGUID, sizeof(char)*MAX_VAR_LEN );
		return -1;
	}

	// 死亡判定标志
	int iDiedFlag = GetIntParam( cmd, 2 );
	// 隐身判定标志
	int iHideFlag = GetIntParam( cmd, 3 );

	ADJUST_V( iDiedFlag );
	ADJUST_V( iHideFlag );

	vector<CPlayer*> player_list;
	CServerRegion *pRegion = static_cast<CServerRegion*>( this->p_SrcShape->GetFather() );
	pRegion->FindAroundPlayer( this->p_SrcShape, player_list );
	// 过滤不可攻击玩家
	for( vector<CPlayer*>::iterator it = player_list.begin(); it != player_list.end();  )
	{
		CPlayer *pPlayer = *it;
		if( !iDiedFlag && pPlayer->IsDied() )
		{
			it = player_list.erase( it );
			continue;
		}
		if( p_SrcShape->Distance( pPlayer ) > range )
		{
			it = player_list.erase( it );
			continue;
		}
		else if( !iHideFlag && !pPlayer->IsVisible( dynamic_cast<CMoveShape*>(p_SrcShape) ) )
		{
			it = player_list.erase( it );
			continue;
		}

		if( AIUtilities::IsInGodState( pPlayer ) )
		{
			it = player_list.erase( it );
			continue;
		}

		if( pPlayer->GetExID() == static_cast<CMoveShape*>( p_SrcShape )->GetBefoolID() )
		{
			it = player_list.erase( it );
		}
		else
		{
			++ it;
		}
	}
	// 随机选择一个玩家
	if( player_list.size() == 0 )
	{
		SetScriptGuidValue( static_cast<CMoveShape*>( this->p_SrcShape ), strGUID, NULL_GUID );
	}
	else
	{
		size_t index = ::random( player_list.size() );
		SetScriptGuidValue( static_cast<CMoveShape*>( this->p_SrcShape ), strGUID, player_list.at( index )->GetExID() );
	}

	M_FREE( strGUID, sizeof(char)*MAX_VAR_LEN );
	return 0;
}

double CScript::ChooseTargetByOccu( const char *cmd, char *retStr )
{
	if( this->p_SrcShape == NULL || this->p_SrcShape->GetFather() == NULL )
	{
		return 0;
	}
	double occu = GetIntParam( cmd, 0 );
	if( occu == ERROR_CODE )
	{
		return 0;
	}
	double range = GetIntParam( cmd, 1 );
	if( range == ERROR_CODE )
	{
		return 0;
	}

	char *strGUID = GetStringParam( cmd, 2 );
	if( strGUID == NULL || strlen( strGUID ) == 0 )
	{
		M_FREE( strGUID, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}

	// 死亡判定标志
	int iDiedFlag = GetIntParam( cmd, 3 );
	// 隐身判定标志
	int iHideFlag = GetIntParam( cmd, 4 );

	ADJUST_V( iDiedFlag );
	ADJUST_V( iHideFlag );

	vector<CPlayer*> player_list;
	// 满足职业条件的玩家列表
	vector<CPlayer*> occu_players;
	CServerRegion *pRegion = static_cast<CServerRegion*>( this->p_SrcShape->GetFather() );
	pRegion->FindAroundPlayer( this->p_SrcShape, player_list );
	// 过滤不可攻击玩家
	for( vector<CPlayer*>::iterator it = player_list.begin(); it != player_list.end();  )
	{
		CPlayer *pPlayer = *it;
		if( !iDiedFlag && pPlayer->IsDied() )
		{
			it = player_list.erase( it );
			continue;
		}
		if( p_SrcShape->Distance( pPlayer ) > range )
		{
			it = player_list.erase( it );
			continue;
		}
		else if( !iHideFlag && !pPlayer->IsVisible( dynamic_cast<CMoveShape*>(p_SrcShape) ) )
		{
			it = player_list.erase( it );
			continue;
		}

		if( AIUtilities::IsInGodState( pPlayer ) )
		{
			it = player_list.erase( it );
			continue;
		}

		if( pPlayer->GetExID() == static_cast<CMoveShape*>( p_SrcShape )->GetBefoolID() )
		{
			it = player_list.erase( it );
			continue;
		}

		if( pPlayer->GetOccupation() == occu )
		{
			occu_players.push_back( pPlayer );
			++ it;
		}
		else
		{
			++ it;
		}
	}

	double ret = 0;
	// 随机选择一个玩家
	if( occu_players.size() > 0 )
	{
		size_t index = ::random( occu_players.size() );
		SetScriptGuidValue( static_cast<CMoveShape*>( this->p_SrcShape ), strGUID, occu_players.at( index )->GetExID() );
		ret = 1;
	}
	else
	{
		if( player_list.size() == 0 )
		{
			SetScriptGuidValue( static_cast<CMoveShape*>( this->p_SrcShape ), strGUID, NULL_GUID );
			ret = 0;
		}
		else
		{
			size_t index = ::random( player_list.size() );
			SetScriptGuidValue( static_cast<CMoveShape*>( this->p_SrcShape ), strGUID, player_list.at( index )->GetExID() );
			ret = 2;
		}
	}

	M_FREE( strGUID, sizeof(char)*MAX_VAR_LEN );
	return ret;
}
