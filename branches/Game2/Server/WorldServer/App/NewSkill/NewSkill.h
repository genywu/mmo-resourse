//////////////////////////////////////////////////////////////////////////////////////////////////
///  NewSkill.h
///
///  PATH: E:\alienbrainWork\Game_Program_Develop\guangmang\Server/WorldServer\App/New Skill
///
///  CREATED: 08/06/2007 13:49:01 PM by 陈先进
///
///  PURPOSE:   技能类
///////////////////////////////////////////////////////////////////////////////////////////////////
#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif

#ifndef _INC_NEWSKILL_INCLUDED
#define _INC_NEWSKILL_INCLUDED

#include "../../../../Public/Setup/NewSkillList.h"

class CNewSkill : public CNewSkillProperties
{
public:
	
	CNewSkill(void);
	CNewSkill(DWORD dwSkillID);
	virtual ~CNewSkill(void);
	
	void	AddNewSkill			( tagSkill *pSkill);													//添加技能
	void	AddDoProcessToByteArray( vector<BYTE>& vRet,list<tagDoProcess*> &ltgDoProcess);				//过程编码
	bool	AddNewSkillToByteArray( vector<BYTE>& vRet);												//压入技能
	static  void	AddConditionLength( vector<BYTE>& vRet,tagCondition *ptgCondition);							//条件长度
	void	Clear();																					//释放
	
};


#endif//_INC_NEWSKILL_INCLUDED
