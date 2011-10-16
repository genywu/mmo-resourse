///
/// @file ScriptAI.h
/// @author Kevin Lynx
/// @brief execute script for monster ai.
///
#ifndef ___SCRIPT_AI_H_
#define ___SCRIPT_AI_H_

#include "../../../setup/ScriptAILoader.h"
#include "../Script/Script.h"
#include "MonsterAI.h"


class CScriptAI;

///
/// 当脚本释放某个技能时，怪物自动朝目标靠近直到释放距离条件满足 
///
class CSkillWithTraceHelper
{
public:
	CSkillWithTraceHelper() : m_pFather( 0 )
	{
		Reset();
	}

	void Reset();

	bool IsValid() const
	{
		return m_lSkillID != 0 && m_dwEndTime != 0;
	}

	void SetFather( CScriptAI *pFather )
	{
		m_pFather = pFather;
	}

	void SetRetValue( int ret );

	void PrepareSkillProper();

	/// 逻辑处理完毕返回true
	bool Run();
public:
	long m_lSkillID;
	long m_lSkillType;
	long m_lSkillLevel;
	long m_lSkillMaxDis;
	long m_lSkillMinDis;
	CGUID m_targetID;
	CGUID m_ScriptID;
	std::string m_RetVarName;
	DWORD m_dwEndTime;
private:
	CScriptAI *m_pFather;
};

class CScriptAI : public CMonsterAI
{
private:
	friend class CSkillWithTraceHelper;
public:
	CScriptAI() 
	{
		m_SkillTraceHelper.SetFather( this );
		m_pFuncTable = 0;
		m_bFightActive = true;
	}

	/// build the function table by its type
	void BuildFuncTable();

	virtual DWORD BornSpring();
	virtual BOOL SearchEnemy();
	virtual BOOL WhenBeenHurted( LONG lAttackerType, const CGUID& AttackerID, DWORD dwHurt );
	virtual INT Tracing( CMoveShape *pTarget );
	virtual BOOL OnFightState();
	virtual BOOL CheckReturnPeace();
	virtual BOOL OnSkillEnd( long lAddSub );

	/// When the script use some skills, it will hang up this script until the act time expires.
	void SkillHangup( const CGUID &scriptID, DWORD dwHangupTime );
	
	/// for traceing skills, use some skill and the monster will trace automatically.
	void SkillHangupWithTrace( const CGUID &scriptID, DWORD dwTraceTime, const std::string &ret_var, 
		const CGUID &target_id, long lSkillID, long lSkillLev, long lSkillType );

	/// continue run the script
	void SkillContinue();

private:
	/// call script
	double CallScript( const char *funcName, const VarPairListType &var_list = VarPairListType() );
private:
	CScriptAILoader::FuncTableType *m_pFuncTable;
	/// only used for OnFightState script
	CGUID m_FightScript;
	/// this flag is only set by the skill-timer
	bool m_bFightActive;
	/// for tracing skill
	CSkillWithTraceHelper m_SkillTraceHelper;
};

#endif // ___SCRIPT_AI_H_
