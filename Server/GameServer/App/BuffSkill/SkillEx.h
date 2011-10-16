//=============================================================================
/**
 *  file: SkillEx.h
 *
 *  Brief:实现了对技能的功能，主要包括有吟唱阶段和施放阶段的技能
 *
 *  Authtor:wangqiao
 *	
 *	Datae:2007-8-31
 */
//=============================================================================

#ifndef __SKILL_EX_H__
#define __SKILL_EX_H__

#pragma once
#include "buff.h"

class CSkillEx :
	public CBuff
{
public:
	CSkillEx(void);
	virtual ~CSkillEx(void);
};

#endif
