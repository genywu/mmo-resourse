///
/// @file ScriptAI.cpp
/// @author Kevin Lynx
/// @brief execute script for monster ai.
///
#include "stdafx.h"
#include "ScriptAI.h"
#include "AIUtilities.h"
#include "..\Monster.h"
#include "..\ServerRegion.h"
#include "..\..\GameServer\GameManager.h"

#define SCRIPT_ERROR (-1)
#define SCRIPT_DISABLE (-2)

void CSkillWithTraceHelper::Reset()
{
	m_lSkillID = 0;
	m_lSkillType = 0;
	m_lSkillLevel = 0;
	m_lSkillMaxDis = 0;
	m_lSkillMinDis = 0;
	m_targetID = NULL_GUID;
	m_ScriptID = NULL_GUID;
	m_RetVarName.clear();
	m_dwEndTime = 0;
}

bool CSkillWithTraceHelper::Run()
{
	if( timeGetTime() >= m_dwEndTime )
	{
		SetRetValue( 2 );
		Reset();
		m_pFather->SkillContinue();
		return false;
	}

	CMonster *pMonster = static_cast<CMonster*>( m_pFather->m_pOwner );
	CMoveShape *pTarget = AIUtilities::FindShape( static_cast<CServerRegion*>( pMonster->GetFather() ), m_targetID );
	
	if( pTarget == NULL )
	{
		SetRetValue( -1 );
		m_pFather->SkillContinue();
		Reset();
		return false;
	}

	long lDistance = Distance( pMonster->GetTileX(), pMonster->GetTileY(), pTarget->GetTileX(), pTarget->GetTileY() );
	if( lDistance > m_lSkillMaxDis )
	{
		m_pFather->CBaseAI::MoveTo( pTarget->GetTileX(), pTarget->GetTileY() );
	}
	else if( lDistance < m_lSkillMinDis )
	{
		long lDir = GetLineDir( pTarget->GetTileX(), pTarget->GetTileY(), pMonster->GetTileX(), pMonster->GetTileY() );
		m_pFather->MoveTo( lDir );
	}
	else
	{
		bool ret = m_pFather->UseSkillSyncSafe( m_lSkillType, m_lSkillID, m_lSkillLevel, m_targetID );
		if( ret )
		{
			m_pFather->SkillHangup( m_ScriptID, pMonster->GetActModify( m_lSkillID ) );
			SetRetValue( 1 );
		}
		else
		{
			m_pFather->SkillContinue();
			SetRetValue( -1 );
		}
		Reset();
		return ret;
	}

	return false;
}

void CSkillWithTraceHelper::PrepareSkillProper()
{
	CNewSkill *pSkillProperty = CNewSkillFactory::QueryNewSkillProperty( m_lSkillID );
	if( pSkillProperty != NULL )
	{
		m_lSkillMaxDis = pSkillProperty->GetStaticParam( m_lSkillLevel )->lMaxAtkDistance;
		m_lSkillMinDis = pSkillProperty->GetStaticParam( m_lSkillLevel )->lMinAtkDistance;
	}
}

void CSkillWithTraceHelper::SetRetValue( int ret )
{
	CScript *pScript = NULL;
	itScript it = g_Scripts.find( m_ScriptID );
	if( it != g_Scripts.end() && (( pScript = it->second ) != NULL ) )
	{
		pScript->SetScriptVarValue( static_cast<CMoveShape*>( m_pFather->m_pOwner ), m_RetVarName.c_str(), 0, ret );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
void CScriptAI::BuildFuncTable()
{
	m_pFuncTable = CScriptAILoader::Instance().QueryFuncTable( GetAIType() );
	m_FightScript = NULL_GUID;
	m_bFightActive = true;
}

double CScriptAI::CallScript( const char *funcName, const VarPairListType &var_list )
{
	if( m_pFuncTable == NULL )
	{
		// when someone configed error
		return SCRIPT_ERROR;
	}
	CScriptAILoader::FuncTableType::iterator it = m_pFuncTable->find( funcName );
	if( it != m_pFuncTable->end() )
	{
		const std::string &file = it->second ;
		if( file.size() == 0 )
		{
			return SCRIPT_DISABLE;
		}
		stRunScript st;
		st.pszFileName = (char*)( file.c_str() );
		st.srcShape = m_pOwner;
		st.desShape = m_pOwner;
		st.pRegion = static_cast<CRegion*>( m_pOwner->GetFather() );
		double ret = RunScript( &st,(char*)GetGame()->GetScriptFileData( st.pszFileName ), var_list );	

		return ret;
	}

	return SCRIPT_ERROR;
}

DWORD CScriptAI::BornSpring()
{
  	CallScript( "Init" );
	return CMonsterAI::BornSpring();
}

BOOL CScriptAI::SearchEnemy()
{
 	if( CallScript( "SearchEnemy" ) != SCRIPT_ERROR )
	{
		return TRUE;
	}
	else
	{
		return CMonsterAI::SearchEnemy();
	}
}

BOOL CScriptAI::WhenBeenHurted( LONG lAttackerType, const CGUID& AttackerID, DWORD dwHurt )
{
	// argument list
	VarPairListType var_list;
	var_list.push_back( stVarPair( "$AttackerType", (double)lAttackerType ) );
	var_list.push_back( stVarPair( "@AttackerID", AttackerID ) );
	var_list.push_back( stVarPair( "$Hurt", (double)dwHurt ) );

	if( CallScript( "WhenBeenHurted", var_list ) != SCRIPT_ERROR )
	{
		// 多血条脚本触发处理
		static_cast<CMonster*>( m_pOwner )->OnHpChanged( m_pOwner->GetHP(), dwHurt );
		GBHSpring( dwHurt );
		return TRUE;
	}
	else
	{
		return CMonsterAI::WhenBeenHurted( lAttackerType, AttackerID, dwHurt );
	}
}

INT CScriptAI::Tracing( CMoveShape *pTarget )
{
	VarPairListType var_list;
	var_list.push_back( stVarPair( "$TargetType", (double)pTarget->GetType() ) );
	var_list.push_back( stVarPair( "@TargetID", pTarget->GetExID() ) );

	double ret = CallScript( "Tracing", var_list );
	if( ret == SCRIPT_ERROR )
	{
		return CMonsterAI::Tracing( pTarget );
	}

	return ret;
}

BOOL CScriptAI::OnFightState()
{
	if( m_bFightActive )
	{
		if( m_FightScript != NULL_GUID )
		{
			long ret = ContinueRunScript( m_FightScript, 0 );
			if( ret == 1 ) // execute finished.
			{
				m_FightScript = NULL_GUID;
				// AI可能终止，添加AI事件以驱动AI
				AddAIEvent( AET_MOVE, 0 );
			}
			return TRUE;
		}
		else
		{
			double ret = CallScript( "OnFightState" );
			if( ret != SCRIPT_ERROR )
			{
				return ret != 0;
			}
		}
		return CMonsterAI::OnFightState();
	}
	else if( m_SkillTraceHelper.IsValid() )
	{
		// 处理UseSkillSyncWithTrace脚本请求
		m_SkillTraceHelper.Run(); 
	}

	return FALSE;
}

BOOL CScriptAI::OnSkillEnd( long lAddSub )
{
	double ret = CallScript( "OnSkillEnd" );
	if( ret != SCRIPT_ERROR )
	{
		return ret != SCRIPT_DISABLE ? ret != 0 : FALSE;
	}

	return CMonsterAI::OnSkillEnd( lAddSub );
}

BOOL CScriptAI::CheckReturnPeace()
{
	double ret = CallScript( "CheckReturnPeace" );
	if( ret != SCRIPT_ERROR )
	{
		return ret != SCRIPT_DISABLE ? ret != 0 : FALSE;
	}

	return CMonsterAI::CheckReturnPeace();
}

void CScriptAI::SkillHangup( const CGUID &scriptID, DWORD dwHangupTime )
{
	m_FightScript = scriptID;
	m_bFightActive = false;
	AddAIEvent( AET_SKILL_CONTINUE, dwHangupTime );	
}

void CScriptAI::SkillHangupWithTrace( const CGUID &scriptID, DWORD dwTraceTime, const std::string &ret_var, 
		const CGUID &target_id, long lSkillID, long lSkillLev, long lSkillType )
{
	m_FightScript = scriptID;
	m_bFightActive = false;
	
	m_SkillTraceHelper.m_dwEndTime = timeGetTime() + dwTraceTime;
	m_SkillTraceHelper.m_RetVarName = ret_var;
	m_SkillTraceHelper.m_targetID = target_id;
	m_SkillTraceHelper.m_lSkillID = lSkillID;
	m_SkillTraceHelper.m_lSkillLevel = lSkillLev;
	m_SkillTraceHelper.m_lSkillType = lSkillType;
	m_SkillTraceHelper.m_ScriptID = scriptID;

	m_SkillTraceHelper.PrepareSkillProper();
	AddAIEvent( AET_MOVE, 0 );
}

void CScriptAI::SkillContinue()
{
	if( m_aesAIState == AES_FIGHT_NORMAL && m_pOwner->GetState()== CShape::STATE_FIGHT )
	{
		m_bFightActive = true;
		AddAIEvent( AET_MOVE, 0 );
	}
}

