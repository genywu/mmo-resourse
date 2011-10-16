///////////////////////////////////////////////////////////////////////////////
///  NewSkillFactory.h
///
///  PATH: E:\alienbrainWork\Game_Program_Develop\guangmang\Server/WorldServer\App/New Skill
///
///  CREATED: 08/07/2007 12:50:01 PM by 陈先进
///
///  PURPOSE:   技能工厂类
///
// NewSkillFactory.h: interface for the CNewSkillFactory class.
//
//////////////////////////////////////////////////////////////////////
#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_NEWSKILL_FACTORY_INCLUDED
#define _INC_NEWSKILL_FACTORY_INCLUDED
#include "NewSkill.h"

class CNewSkillFactory
{

public:

    //公共部分配置
    struct tagSkillCommon 
    {
        map<DWORD,CNewSkill::tagConditionGroup*>	m_mapConditionGroup;		//条件组
        map<DWORD,CNewSkill::tagCondition*>		    m_mapCondition;				//条件体 
        map<DWORD,CNewSkill::tagExpression>		    m_mapFormat;				//计算公式			
        map<DWORD,CNewSkill::tagOperateObject*>	    m_mapOperateObject;		    //操作对象 
    };

private:
	//状态的临时列表
	static map<DWORD, CNewSkill*>				g_mapNewSkill;
	static DWORD								g_dwVersion;
	static map<DWORD, TiXmlElement*>			g_mapTemplateElem;								
	static map<DWORD, TiXmlElement*>			g_mapComTemplateElem;
	static list<long>							g_lInitSkillList;
	static map<DWORD,list<long>>				g_mapOccuInitSkill;
    static tagSkillCommon                       g_skillCommon;

public:
	static list<long>		GetOccuSkillList(DWORD dwOccu);
	static list<long>		GetInitSkillList()	{return g_lInitSkillList;}
	//读取状态配置文件
	static HRESULT			LoadConfigration	( const string p );
	//通过节点载入
	static bool				LoadConfigration	( TiXmlElement *pElem );
	//载入各个等级的静态参数
	static void				LoadLevelStatiParam( TiXmlElement *pElem, CNewSkill::tagSkill *pSkill);
	static void				LoadCommonConfig( TiXmlElement *pElem, CNewSkill *pNewSkill,CNewSkill::tagSkill *ptgSkill);
	static void				LoadNewLevelConfigration( TiXmlElement *pElem, CNewSkill *pNewSkill);
	//载入各个等级的状态技能
	static void				LoadLevelConfigration( TiXmlElement *pElem, CNewSkill *pNewSkill);
	//载入公式配置
	static void				LoadExpresstionConfigration(TiXmlElement *pElem, CNewSkill *pNewSkill);
	//条件配置
	static CNewSkill::tagCondition *LoadConditionConfigration(TiXmlElement *pElem,string strType,DWORD dwID,CNewSkill::tagSkill *ptgSkill=NULL);
	//载入基本属性条件
	static void				LoadPropertyCondition(TiXmlElement *pElem,CNewSkill::tagConProperty *ptgConProperty);
	//载入物品条件
	static void				LoadGoodCondition(TiXmlElement *pElem,CNewSkill::tagConGood *ptgConGood);
	//状态条件
	static void				LoadBuffCondition(TiXmlElement *pElem,CNewSkill::tagConBuff *ptgConBuff);
    //状态驱散
    static void				LoadDispelCondition(TiXmlElement *pElem,CNewSkill::tagConDispel *ptgConDispel);
    //几率条件
    static void             LoadOddsCondition(TiXmlElement *pElem,CNewSkill::tagConOdds *ptgConOdds);
	//技能条件
	static void				LoadSkillCondition(TiXmlElement *pElem,CNewSkill::tagConSkill *ptgConSkill);
	//距离条件
	static void				LoadDistanceCondition(TiXmlElement *pElem,CNewSkill::tagConDistance *ptgConDistance);
	//载入事件
	static void				LoadEventCondition(TiXmlElement *pElem,CNewSkill::tagConEvent *ptgConEvent);
	//载入阻挡
	static void				LoadBlockCondition(TiXmlElement *pElem,CNewSkill::tagConBlock *ptgConBlock);
	//载入职业条件
	static void				LoadOccupationCondition(TiXmlElement *pElem,CNewSkill::tagConOccupation *ptgConOccupation);
	//载入时间条件
	static void				LoadTimerCondition(TiXmlElement *pElem,CNewSkill::tagConTimer *ptgTimer);
	//载入面条件
	static void				LoadFaceCondition(TiXmlElement *pElem,CNewSkill::tagConFace *ptgFace);
	//载入场景条件
	static void				LoadRegionCondition(TiXmlElement *pElem,CNewSkill::tagConRegion *ptgRegion);
	//载入对象
	static void				LoadValidObject(TiXmlElement *pElem,CNewSkill::tagConValidObject *ptgConValidObject);
	//载入脚本
	static void				LoadScript(TiXmlElement *pElem,CNewSkill::tagConScript *ptgConScript);
	//载入条件组
	static void				LoadConditionGroupfigration(TiXmlElement *pElem,CNewSkill *pNewSkill);
	//载入模版
	static void				LoadTemplateConfig(TiXmlElement *pElem);
	//公用模板
	static void				LoadComTemplate(TiXmlElement *pElem);
	//载入公共模板
	static void				LoadComTemplate(TiXmlElement *pElem,CNewSkill *pNewSkill);
	//载入操作对象
	static void				LoadOperatObjects(TiXmlElement *pElem,CNewSkill *pNewSkill);	
	//处理过程
	static void				LoadProcesseExecute(TiXmlElement *pElem,CNewSkill::tagSkill *ptgSkill,CNewSkill *pNewSkill);
	//载入步骤
	static void				LoadStep(TiXmlElement *pElem,CNewSkill::tagStep *ptgStep,CNewSkill *pNewSkill,CNewSkill::tagSkill *ptgSkill);
	//载入处理的具体操作
	static void				LoadStepProcess(TiXmlElement *pElem,CNewSkill::tagCDo *ptgCDo,CNewSkill::tagSkill *ptgSkill);
	//基本属性
	static void				LoadPropertyChange(TiXmlElement *pElem,CNewSkill::tagDoProcess *ptgDoProcess,CNewSkill::tagSkill *ptgSkill);
	//状态
	static void				LoadBuffChange(TiXmlElement *pElem,CNewSkill::tagDoProcess *ptgDoProcess,CNewSkill::tagSkill *ptgSkill);
	//位置
	static void				LoadPosChange(TiXmlElement *pElem,CNewSkill::tagDoProcess *ptgDoProcess);
	//攻击对象
	static void				LodAttackObject(TiXmlElement *pElem,CNewSkill::tagDoProcess *ptgDoProcess);	
	//脚本
	static void				LoadScriptChange(TiXmlElement *pElem,CNewSkill::tagDoProcess *ptgDoProcess);	
	//执行过程
	static void				LoadDoProcess(TiXmlElement *pElem,CNewSkill::tagDoProcess *ptgDoProcess,CNewSkill::tagSkill *ptgSkill);
	static void				LoadCDoProcess(TiXmlElement *pElem,CNewSkill::tagDo *ptgDo,CNewSkill::tagSkill *ptgSkill);
	//获取DWORD
	static DWORD			GetDwordAttribute(TiXmlElement *pElem,string strName);
    static float            GetFloatAttribute( TiXmlElement *elem, const std::string &attrName );
	//获取字符串
	static string			GetStringAttribute(TiXmlElement *pElem,string strName);
	//获取整型
	static BYTE				GetIntAttribute(TiXmlElement *pElem,string strName);
	//获取文本
	static string			GetTextAttribute(TiXmlElement *pElem);
	//冷却时间
	static void				LoadCooldownChange(TiXmlElement *pElem,CNewSkill::tagDoProcess *ptgDoProcess,CNewSkill::tagSkill *ptgSkill);
	//对象列表
	static void				LoadDestObjChange(TiXmlElement *pElem,CNewSkill::tagDoProcess *ptgDoProcess);
	//技能操作
	static void				LoadSkillChange(TiXmlElement *pElem,CNewSkill::tagDoProcess *ptgDoProcess,CNewSkill::tagSkill *ptgSkill);
    //天赋操作
    static void             LoadGeniusChange(TiXmlElement *pElem,CNewSkill::tagDoProcess *ptgDoProcess);
	//注册时间
	static void				LoadRegisterTimer(TiXmlElement *pElem,CNewSkill::tagDoProcess *ptgDoProcess,CNewSkill::tagSkill *ptgSkill);
	//注册事件
	static void				LoadRegisterEvent(TiXmlElement *pElem,CNewSkill::tagDoProcess *ptgDoProcess);
	//注消时间
	static void				LoadUnRegisterTimer(TiXmlElement *pElem,CNewSkill::tagDoProcess *ptgDoProcess);
	//注消事件
	static void				LoadUnRegisterEvent(TiXmlElement *pElem,CNewSkill::tagDoProcess *ptgDoProcess);
	//发送消息
	static void				LoadSendMsg(TiXmlElement *pElem,CNewSkill::tagDoProcess *ptgDoProcess,CNewSkill::tagSkill *ptgSkill);
	//伤害计算
	static void				LoadDamageChange(TiXmlElement *pElem,CNewSkill::tagDoProcess *ptgDoProcess,CNewSkill::tagSkill *ptgSkill);
	//do伤害
	static void				LoadDoDamage(TiXmlElement *pElem,CNewSkill::tagDoProcess *ptgDoProcess,CNewSkill::tagSkill *ptgSkill);
	//物品
	static void				LoadGoodChange(TiXmlElement *pElem,CNewSkill::tagDoProcess *ptgDoProcess,CNewSkill::tagSkill *ptgSkill);
	//方向
	static void				LoadDirection(TiXmlElement *pElem,CNewSkill::tagDoProcess *ptgDoProcess);
	//
	static void				LoadInitSkillConfigration(TiXmlElement *pElem);
	//
	static  HRESULT			LoadInitSkillConfig( const string pFileName );
	//技能列表
	static  HRESULT			LoadSkillList(const string pFileName, list<string>* listFileName);
	//
	static void				LoadSkillListConfigration(TiXmlElement *pElem,list<string>* listFileName);


	//清除状态缓冲
	static void				ClearNewSkillCache	();								
	//载入状态缓冲
	static bool				LoadNewSkillCache	();
	//编码
	static bool				CodeNewSkill		( vector<BYTE>& vRet );	

    //
    static bool             LoadSkillCommon(const char* filename);
    static bool             CodeSkillCommon(vector<BYTE>& vRet);

};

#endif//_INC_NEWSKILL_FACTORY_INCLUDED