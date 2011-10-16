///////////////////////////////////////////////////////////////////////////////
///  NewSkillBaseProperties.h
///
///  PATH: E:\alienbrainWork\Game_Program_Develop\guangmang\Server/Server/App/BuffSkill
///
///  CREATED: 08/07/2007 12:50:01 PM by 陈先进
///
///  PURPOSE:   技能属性类
///
//////////////////////////////////////////////////////////////////////

#pragma once

#include "BuffSkillDef.h"
#include "../../../../Public/Setup/NewSkillList.h"

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
    //stStaticParam					*GetStaticParam(DWORD dwLevel);
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
