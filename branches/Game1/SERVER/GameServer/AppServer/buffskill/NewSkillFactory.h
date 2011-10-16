///////////////////////////////////////////////////////////////////////////////
///  NewSkillFactory.h
///
///  PATH: E:\alienbrainWork\Game_Program_Develop\guangmang\SERVER\GameServer\AppServer\BuffSkill
///
///  CREATED: 08/07/2007 12:50:01 PM by 陈先进
///
///  PURPOSE:   技能工厂类
///
//////////////////////////////////////////////////////////////////////
#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_NEW_SKILL_FACTORY_INCLUDED
#define _INC_NEW_SKILL_FACTORY_INCLUDED

#include <map>
#include <string>

#include "Operator.h"
#include "Condition.h"
#include "CondiGroup.h"
#include "OpSendMsg.h"
#include "BaseModule.h"
#include "Do.h"
#include "NewSkillBaseProperties.h"
#include "../../public/GameEvent.h"
#include "../../../setup/NewSkillList.h"


enum EMAXBASEMODLE
{
	EMAXSKILLSTATE = 10000,
};
class CStep;

class CEffectObjects;

using namespace std;
struct  tagModuleKey
{
	DWORD dwID;
	DWORD dwLvl;
};
struct tagBKey
{
	long lFlag;
	long lID;
};

struct  tagBKeycmp
{
	bool operator()(const tagBKey& left,const tagBKey& right) const
	{
		if(left.lID<right.lID)
		{
			return true;
		}		
		return false;
	}
};
struct tagKeycmp
{
	bool operator()(const tagModuleKey& left,const tagModuleKey& right) const
	{
		if(left.dwID<right.dwID)
		{
			return true;
		}
		else if (left.dwID==right.dwID)
		{
			if (left.dwLvl < right.dwLvl)
			{
				return true;
			} 
			else
			{
				return false;
			}
		}
		return false;
	}
};


class CNewSkillFactory 
{
private:

	typedef list<CBaseModule*>									listModules;
	typedef list<CBaseModule*>::iterator						itModule;	
	static  listModules											g_BaseModules[EMAXSKILLSTATE];			

	typedef list<stModuParam*>									listModuParam;
	typedef list<stModuParam*>::iterator						itModuParam;
	static	listModuParam										g_lModuParam;
	static stModuParam											*m_pParam;								//参数
	static  map<DWORD, CNewSkill*>								g_mNewSkillProperties;					//属性列表
	static DWORD												g_dwVersion;							//配置文件的版本
	static map<DWORD,list<long>>								g_mapOccuInitSkill;
	static DWORD												g_dwCurrentVersion;						//当前版本
	static DWORD												g_dwCount;								//器
	static map<tagBKey,  set<CBaseModule*>,tagBKeycmp>			m_UsingBaseModule;

	static map<tagModuleKey, long, tagKeycmp>					g_mapModuleCounter;		
	typedef map<tagModuleKey, long, tagKeycmp>::iterator		itModulerCounter;


	//  [4/26/2009 chenxianj]
	typedef map<tagModuleKey,long,tagKeycmp>::iterator			itModuleTimer;	
	static map<tagModuleKey,long,tagKeycmp>						g_mapModuleTimer;

	typedef map<string,CVariableList*>::iterator				itVariableList; 
	static map<string,CVariableList*>							g_mapVariableList;	


public:
	//根据ID查询属性
	static  CNewSkill*								QueryNewSkillProperty(DWORD dwID);									
	//对技能解码
	static  void									DecodeNewSkillMessage(BYTE* pByte, long& pos);						
	//过程解码
	static  CNewSkill::tagDoProcess					*DecodeDoProcess(BYTE* pByte, long& pos);							
	//清空技能缓冲
	static  void									ClearSkillCache();													
	//初始2化模块参数
	static  void									InitModuParam(CNewSkill *pSkillProperties,stModuParam *pParam,DWORD dwLevel);
	//初始化基本模块
	static  void									InitBaseModule(CBaseModule *pBaseModule,DWORD dwID,DWORD dwLevel,CNewSkill *pSkillProperties,stModuParam *pParam);
	//初始化过程
	static  void									InitProcedure(DWORD dwID,DWORD dwLevel,CProcedure *pProcedure,CNewSkillProperties::tagProcess* ptgProcess);
	//初始化步骤
	static  void									InitStep(DWORD dwID,DWORD dwLevel,CStep	*pStep,CNewSkillProperties::tagStep *ptgStep);	
	//初始化Do	
	static  void									InitDo(DWORD dwID,DWORD dwLevel,CDo *pDo,CNewSkillProperties::tagDo *ptgDo);					
	//初始化DO下条件组
	static  void									InitGroup(DWORD dwID,DWORD dwLevel,CNewSkillProperties::tagDoProcess *ptgDoProcess, CDo::tagGroup *pGroup);
	//初始化操作
	static  void									InitOperator(DWORD dwID,CNewSkillProperties::tagDoProcess *ptgDoProcess, vector<COperator*> &vOperator);
	//初始改变属性
	static  void									InitChangeProperty(CNewSkillProperties::tagChangeProperty *ptgChangePropety, vector<COperator*> &vOperator);
	//初始化脚本
	static  void									InitChangeScriptID(DWORD dwID,CNewSkillProperties::tagScriptID *ptgScriptID,vector<COperator*> &vOperator);
	static  void									InitAttackChangeObject(CNewSkillProperties::tagAttackChangeObject *ptgAttackChangeObject,vector<COperator*> &vOperator);
	//初始化状态
	static  void									InitBuffChange(CNewSkillProperties::tagChangeBuff *ptgBuff, vector<COperator*> &vOperator);
	//初始化改变位置
	static  void									InitChangePos(CNewSkillProperties::tagChangePos *ptgPos, vector<COperator*> &vOperator);
	//初始化改变时间
	static  void									InitCooldownChange(CNewSkillProperties::tagCoolDownChange *ptgCool, vector<COperator*> &vOperator);
	//初始化伤害
	static  void									InitDamageChange(CNewSkillProperties::tagDamageChange *ptgDamage, vector<COperator*> &vOperator);
	//初始化目标对象
	static  void									InitDestObjChange(CNewSkillProperties::tagDestObjChange *ptgDest, vector<COperator*> &vOperator);
	//初始化注册事件
	static  void									InitEventChange(DWORD dwID, CNewSkillProperties::tagRegisterEvent *ptgEvent,vector<COperator*> &vOperator);
	//事间
	static  void									InitTimerChange(DWORD dwID, CNewSkillProperties::tagRegisterTimer *ptgTimer,vector<COperator*> &vOperator);
	//发送消息
	static  void									InitSendMsg(CNewSkillProperties::tagSendMsg *ptgSendMsg ,vector<COperator*> &vOperator);
	//初始化DO伤害
	static  void									InitDoDamage(CNewSkillProperties::tagDoDamage *ptgDoDamage, vector<COperator*> &vOperator);
	//初始化物品
	static  void									InitGoodChange(CNewSkillProperties::tagGoodChange *ptgGoodChange,vector<COperator*> &vOperator);
	//初始化注消时间
	static  void									InitUnRegisterTimer(CNewSkillProperties::tagUnRegisterTimer *ptgUnRegisterTimer, vector<COperator*> &vOperator);
	//初始化注消事件
	static  void									InitUnRegisterEvent(CNewSkillProperties::tagUnRegisterEvent *ptgUnRegisterEvent, vector<COperator*> &vOperator);
	//初始化方向
	static  void									InitDirection(CNewSkillProperties::tagDirection *ptgDirection,vector<COperator*> &vOperator);
	//初始化技能操作
	static  void									InitSkillChange(CNewSkillProperties::tagSkillChange  *ptgSkillChange,vector<COperator*> &vOperator);
	//初始化具体条件
	static  CCondition               			    *InitConcreateCondition(CNewSkillProperties::tagCondition *ptgConditon);

	static  CEffectObjects           				*InitEffectObject(DWORD dwID,DWORD dwLv,DWORD dwOperobjID);					//作用对象
	static  CCondiGroup								*InitConditionGroup(DWORD dwID,DWORD dwLv,DWORD dwGroupID);					//条件组
	static  CCondition								*InitCondition(DWORD dwID,DWORD dwLv,DWORD dwConditionID);					//条件
	static  CConProperty							*InitBasePropertyCondition(CNewSkillProperties::tagCondition *ptgConditon); //基本属性
	static  CConGoods								*InitGoodCondittion(CNewSkillProperties::tagCondition *ptgConditon);		//物品
	static  CConBuff								*InitBufffCondition(CNewSkillProperties::tagCondition *ptgConditon);		//状态
	static  CConDistance							*InitDistanceConditon(CNewSkillProperties::tagCondition *ptgConditon);		//距离
	static  CConValidObj							*InitValidObjectCondition(CNewSkillProperties::tagCondition *ptgConditon);	//有效目标	
	static  CConScript								*InitScriptCondition(CNewSkillProperties::tagCondition *ptgConditon);		//脚本	
	static	CConCooldown							*InitCoolDownCondition(CNewSkillProperties::tagCondition *ptgConditon);		//时间变化	
	static	CConTimer								*InitTimerCondtion(CNewSkillProperties::tagCondition *ptgConditon);			//时间	
	static	CConEvent								*InitEventCondition(CNewSkillProperties::tagCondition *ptgConditon);		//事件	
	static	CConBlock								*InitBlockCondition(CNewSkillProperties::tagCondition *ptgConditon);		//阻挡	
	static	CConDir									*InitDirCondition(CNewSkillProperties::tagCondition *ptgConditon);			//面向	
	static  CConRegion								*InitRegionCondition(CNewSkillProperties::tagCondition *ptgCondition);		//场景


	static list<long>								GetInitOccuSkill(DWORD dwOccu);
	static eEventType								GetEventType(long nID, string strName);
	static eModuleOper								GetOperator(long nID,string strName);
	static eObj										GetObj(string strName);
	static eChildObjType							GetChildObjType(ModuleType type);	
	static tagDamageType							GetDamageType(long nID, string strName);
	static list<CConValidObj::eValidObjType>		GetValidType(list<string> lStr);
	static CConValidObj::eType						GetEType(string strType);
	static CCondiGroup::eGrouMode					GetGroupMode(string);
	static COpSendMsg::eSendMsgType					GetMsgType(long nID, string strName);
	static COpSendMsg::eMsgRange					GetRange(string strName);
	static eModuleRet								GetModuleRet(string str);
	static OPFUN									*GetOpFun(string name,long nID);
	static CMPFUN									*GetCmpFun(string name);
	static COpDirec::eDirec							GetDirec(string str);
	static CBaseModule								*CreateModule(DWORD dwID, DWORD dwLevel,stModuParam *pModuParam=NULL, DWORD dwGlobleID=0,long lGoodsIndex=0,long lType=0);
	static void										RealeaseBaseMoudule(stModuParam *pModuParam);	
	static CBaseModule								*GetBaseModule(stModuParam *pParam);
	static void										GetConditon(BYTE* pByte, long& pos, string strType,DWORD dwLength,CNewSkillProperties::tagCondition *ptgCondition);
	static ModuleType								GetModuleType(string strName);
	static eSkillUseType							GetUseType(BYTE bUseType);
	static eAddSub									GetAddSub(string strAddSub);
	static BOOL										CheckListModule(listModuParam lModuParam,stModuParam *pModuParam);
	static void										OutputBasemodelInfo(const char *pszFileName);
	static void										CheckBaseModuleTimeOut(long lNowTime);
	static CVariableList						    *GetVariableList(string strFileName);
	static void										AddVariableList(string strFileName, CVariableList *pVariableList);
	//计算hash值
	static long										CacuHashValue(long lID,long lLevle);

	static void										RealeaseModuParam();
	static void										RealeaseModuParam(stModuParam *pModuParam);
};
#endif