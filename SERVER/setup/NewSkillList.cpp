///////////////////////////////////////////////////////////////////////////////
///  NewSkillList.cpp
///
///  PATH: E:\alienbrainWork\Game_Program_Develop\guangmang\SERVER\setup
///
///  CREATED: 08/06/2007 13:49:01 PM by 陈先进
///
///  PURPOSE:   技能类
//////////////////////////////////////////////////////////////////////--

#include "stdafx.h"
#include "NewSkillList.h"
#include "../../public/StrUnifyInput.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//CNewSkillProperties::tagChangeProperty::~tagChangeProperty()
//{
//	strName.clear();
//	strOperator.clear();
//	strScriptVarName.clear();
//}

/*
NAME:CState()
DESC:默认构造函数
PARA:无
*/

CNewSkillProperties::CNewSkillProperties()
{
	m_dwID = 0;	
}


/*
NAME:CState(long lStateID)
DESC:构造函数
PARA:lStateID为状态的ID
*/

CNewSkillProperties::CNewSkillProperties(DWORD dwID)
{
	m_dwID = dwID;
}

/*
NAME:~CNewSkill()
DESC:析造函数
PARA:无
*/

CNewSkillProperties::~CNewSkillProperties()
{
	//Clear();
}

//CNewSkillProperties::tagChangeProperty::~tagChangeProperty()
//{
//	delete strName.c_str();
//}
/*
NAME:AddNewSkill()
DESC:添加技能的主要结构
PARA:pSkill技能的主要结构
*/
//
void CNewSkillProperties::AddNewSkill( tagSkill *pSkill)
{
	m_lSkill.push_back(pSkill);
}


//脚本
void	CNewSkillProperties::SetFormat( DWORD dwID, string& strFileName,string& strContend)	
{
	tagExpression Express = {strFileName,strContend};
	m_mapFormat[dwID] = Express;
} 
/*
NAME:SetCondition()
DESC:添加条件
PARA:dwID条件ID
	 tgConditon条件体
*/
void	CNewSkillProperties::SetCondition( DWORD dwID,tagCondition *tgCondtition)
{
	map<DWORD, tagCondition*>::iterator it = m_mapCondition.find(dwID);
	if (it!=m_mapCondition.end())
	{
		char str[256];
		_snprintf(str, 256, CStringReading::LoadString(IDS_WS_SKILL,STR_WS_SKILL_SECOND),this->GetNewSkillID(),dwID);	
		PutDebugString(str);
		return;
	}
	else
	{
		m_mapCondition[dwID] = tgCondtition;
	}
}

/*
NAME:SetConGroup()
DESC:添加条件组
PARA:dwGroupID条件组ID
	 ptgConditionGroup条件组
*/
void CNewSkillProperties::SetConGroup(DWORD dwGroupID,tagConditionGroup *ptgConditionGroup)
{
	map<DWORD,tagConditionGroup*>::iterator it = m_mapConditionGroup.find(dwGroupID);
	if (it!=m_mapConditionGroup.end())
	{
		char str[256];		
		_snprintf(str, 256, CStringReading::LoadString(IDS_WS_SKILL,STR_WS_SKILL_FIRST),this->GetNewSkillID(),dwGroupID);	
		PutDebugString(str);
		return;
		
	} 
	else
	{
		m_mapConditionGroup[dwGroupID]=ptgConditionGroup;
	}
 
}

/*
NAME:GetNewSkill()
DESC:通过等级获取相应技能的主要结构
PARA:dwLevel技能等级
*/
CNewSkillProperties::tagSkill *CNewSkillProperties::GetNewSkill(DWORD dwLevel)
{
	if (m_lSkill.size()==0)
	{
		return NULL;
	}
	list<tagSkill*>::iterator it=m_lSkill.begin();
	for (; it!=m_lSkill.end(); ++it)
	{
		if (dwLevel == (*it)->dwLevel)
		{
			tagSkill *ptgSkill = *it;
			return ptgSkill;
		}
	}
	return NULL;
}

CNewSkillProperties::stStaticParam	*CNewSkillProperties::GetStaticParam(DWORD dwLevel)	
{
	CNewSkillProperties::tagSkill *ptgSkill = GetNewSkill(dwLevel);
	if (ptgSkill)
	{
		return ptgSkill->pStaticParam;
	}
	return NULL;
}				

