#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_SKILL_INCLUDED
#define _INC_SKILL_INCLUDED
#include "..\..\..\..\Public\SkillRelated.h"
#include <vector>
using namespace std;

class CSkill
{
public:

	struct tagUsage
	{
		DWORD dwUsage;
		DWORD dwCost;
	};

private:

	vector<tagUsage>			m_vUsages;
	char						m_strSkillName[256];
	char						m_strDescription[256];
	//##技能的类型，攻击，防御，召唤，状态
	DWORD						m_dwSkillType;
	DWORD						m_dwSkillID;
	DWORD						m_dwLevel;
	DWORD						m_dwIconID;
	BOOL						m_bIsTargetSelf;

public:

	CSkill						();
	CSkill						( LONG lSkillID );
	~CSkill						();

	VOID	SetSkillType		( LONG lSkillType );
	VOID	SetSkillID			( LONG lSkillID );
	VOID	SetLevel			( LONG lLevel );
	VOID	SetSkillName		( char* pName );
	VOID	SetDescription		( char* pDescription );
	VOID	SetIconID			( DWORD dwIcon );
	VOID	SetTargetType		( BOOL bIsTargetSelf );
	VOID	AddUsage			( tagUsage& rUsage );
	BOOL	Serialize			( BYTE** ppBuffer, size_t* pLength );
	VOID	Clear				();
};

#endif
