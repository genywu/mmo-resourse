///////////////////////////////////////////////////////////////////////////////
///  NewSkillBaseProperties.h
///
///  PATH: E:\alienbrainWork\Game_Program_Develop\guangmang\SERVER\GameServer\AppServer\BuffSkill
///
///  CREATED: 08/07/2007 12:50:01 PM by 陈先进
///
///  PURPOSE:   技能属性类
///
//////////////////////////////////////////////////////////////////////

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_NEWSKILLBASEPROPERTIES_INCLUDED
#define _INC_NEWSKILLBASEPROPERTIES_INCLUDED
#include <vector>
#include <list>
#include <string>
#include <map>

#include "BuffSkillDef.h"
#include "..\..\..\setup\NewSkillList.h"
using namespace std;


class CNewSkill : public CNewSkillProperties
{
private:	
	ModuleType						m_MType;					//类型
public:
	CNewSkill(void);
	CNewSkill(DWORD dwSkillID);
	virtual ~CNewSkill(void);

	void							AddNewSkill( tagSkill *pSkill);														//添加技能	
	tagSkill						*GetNewSkill(DWORD dwLevel);														//获取技能
	void							Clear();																			//清空
	void							ClearCDo(tagCDo *ptgCDo);															//释放
	void							ClearBaseChange(tagBaseChange *ptgBaseChange);
	void							ClearCondition(void *pCondition,string strType);
	ModuleType						GetModuleType(string strName);														//设置类型
	void							SetSkillType( string strType ){m_MType = GetModuleType(strType);}					//获取类型	
	ModuleType						GetType()				  {return m_MType;}
	void							SetFormat(DWORD dwID, string& strFileName,string& strContend){tagExpression Express = {strFileName,strContend};m_mapFormat[dwID] = Express;}	//设置公式
	void							SetCondition(DWORD dwID,tagCondition *tgCondtition) {m_mapCondition[dwID] = tgCondtition;}														//设置条件
	void							SetConGroup(DWORD dwGroupID,tagConditionGroup *ptgConditionGroup){m_mapConditionGroup[dwGroupID]=ptgConditionGroup;}							//设置条件组
	void							SetOperateObject(DWORD dwID,tagOperateObject *ptgOperateObject){m_mapOperateObject[dwID]=ptgOperateObject;}										//设置作用对象
	map<DWORD,tagOperateObject*>	GetOperateObject()				{return m_mapOperateObject;}		//作用对象列表
	ModuleType						GetSkillType()					{return m_MType;}					//类型
	map<DWORD,tagExpression>&		GetFormat()						{return m_mapFormat;}				//获取公式
	map<DWORD,tagCondition*>		GetCondition()					{return m_mapCondition;}			//条件
	map<DWORD,tagConditionGroup*>	GetConditionGroup()				{return m_mapConditionGroup;}		//条件组
};

#endif//_INC_NEWSKILLBASEPROPERTIES_INCLUDED