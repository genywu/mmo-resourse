///////////////////////////////////////////////////////////////////////////////
///  NewSkillFactory.cpp
///
///  PATH: E:\alienbrainWork\Game_Program_Develop\guangmang\Server/Server/App/BuffSkill
///
///  CREATED: 08/07/2007 12:50:01 PM by 陈先进
///
///  PURPOSE:   技能工厂类
///
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "Procedure.h"
#include "Step.h"
#include "Do.h"
#include "OpRegister.h"
#include "OpSendMsg.h"
#include "OpPosEx.h"
#include "OpDamage.h"
#include "EffPoint.h"
#include "EffLine.h"
#include "EffLineFirst.h"
#include "EffCirc.h"
#include "EffRect.h"
#include "EffSector.h"
#include "EffTeam.h"
#include "EffAppObject.h"
#include "CondiGroup.h"
#include "Condition.h"
#include "Expressions.h"
#include "NewSkillFactory.h"
#include "../MoveShape.h"
//#include "../Script/VariableList.h"
//#include "../Script/Script.h"

#include "../../../../Public/Common/CStrConverter.h"
#include "../../../../Public/Setup/GlobalSetup.h"
#include "../../../../Public/ServerPublic/Server/Common/VariableList.h"
#include "../../../../Public/ServerPublic/Server/Common/VarListUtils.h"
#include "../LuaScript/Script.h"
#include "../LuaScript/ScriptSys.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
struct tagModuleKey;
struct tagKeycmp;
typedef list<CBaseModule*> listModules;
typedef list<stModuParam*> listModuParam;



listModules										CNewSkillFactory::g_BaseModules[EMAXSKILLSTATE];
listModuParam									CNewSkillFactory::g_lModuParam;
map<DWORD,CNewSkill*>							CNewSkillFactory::g_mNewSkillProperties;
stModuParam								        *CNewSkillFactory::m_pParam;
DWORD									        CNewSkillFactory::g_dwVersion;
DWORD											CNewSkillFactory::g_dwCurrentVersion;
map<DWORD,list<long>>					        CNewSkillFactory::g_mapOccuInitSkill;
DWORD									        CNewSkillFactory::g_dwCount;								//读数器

map<tagBKey,set<CBaseModule*>,tagBKeycmp>		CNewSkillFactory::m_UsingBaseModule;
map<tagModuleKey,long,tagKeycmp>			    CNewSkillFactory::g_mapModuleTimer;
map<tagModuleKey, long, tagKeycmp>				CNewSkillFactory::g_mapModuleCounter;
map<string,VariableList*>						CNewSkillFactory::g_mapVariableList;
CNewSkillFactory::tagSkillCommon                CNewSkillFactory::g_skillCommon;

//new MailTimer(1024,1000)

/*
NAME:DecodeNewSkillMessage
DESC:技能解码
PARA:pByte解码数据
	 pos位置
*/

void CNewSkillFactory::DecodeNewSkillMessage(BYTE* pByte, long& pos)
{
	ClearSkillCache();
	char strTemp[1024];
	g_dwCount = 0;
	//技能个数
	DWORD dwNum = _GetDwordFromByteArray(pByte,pos);
	//版本号
	g_dwVersion = _GetDwordFromByteArray(pByte,pos);
	//初始化技能职业个数
	long lOccuNum = _GetLongFromByteArray(pByte,pos);
	for (int index=0; index<lOccuNum; ++index)
	{
		DWORD dwOccu = _GetDwordFromByteArray(pByte,pos);
		long lSkillNum = _GetLongFromByteArray(pByte,pos);
		if (lSkillNum>0)
		{
			list<long> lInitSkill;
			for (int i=0; i<lSkillNum; ++i)
			{
				long lSkillID = _GetLongFromByteArray(pByte,pos);
				lInitSkill.push_back(lSkillID);
			}
			g_mapOccuInitSkill[dwOccu] = lInitSkill;
		}
	}

    /*
    //公式个数
    long lFNum = _GetLongFromByteArray(pByte,pos);
    for (int i=0; i<lFNum; ++i)
    {
        //公式ID
        DWORD dwID = _GetDwordFromByteArray(pByte,pos);

        _GetStringFromByteArray(pByte,pos,strTemp);
        char strContend[1024];
        _GetStringFromByteArray(pByte,pos,strContend);

        CNewSkill::tagExpression Express = {strTemp,strContend};

        g_skillCommon.m_mapFormat[dwID] = Express;
    }

    //条件组
    //个数
    long lCGNum = _GetLongFromByteArray(pByte,pos);
    for (int i=0; i<lCGNum; ++i)
    {
        DWORD dwConGroupID = _GetDwordFromByteArray(pByte,pos);

        CNewSkillProperties::tagConditionGroup *ptgCondition = OBJ_CREATE(CNewSkillProperties::tagConditionGroup);
        assert(ptgCondition);
        _GetStringFromByteArray(pByte,pos,strTemp);
        ptgCondition->strMode =strTemp;

        long lCNum = _GetLongFromByteArray(pByte,pos);		
        for (int j=0; j<lCNum; ++j)
        {
            DWORD dwConID = _GetDwordFromByteArray(pByte,pos);
            ptgCondition->lConditonGroup.push_back(dwConID);
        }

        g_skillCommon.m_mapConditionGroup[ dwConGroupID ] = ptgCondition;
    }

    //条件体
    long lConditionNum = _GetLongFromByteArray(pByte,pos);
    for (int i=0; i<lConditionNum; ++i)
    {

        DWORD dwID = _GetDwordFromByteArray(pByte,pos);
        CNewSkillProperties::tagCondition *ptgCondition = OBJ_CREATE(CNewSkillProperties::tagCondition);
        assert(ptgCondition);
        _GetStringFromByteArray(pByte,pos,strTemp);
        DWORD dwLength = _GetDwordFromByteArray(pByte,pos);	
        GetConditon(pByte,pos, strTemp,dwLength,ptgCondition);

        g_skillCommon.m_mapCondition[ dwID ] = ptgCondition;

    }
    //作用对象个数
    long lONum = _GetLongFromByteArray(pByte,pos);
    for (int j=0; j<lONum; ++j)
    {
        CNewSkillProperties::tagOperateObject *ptgOperateObject = OBJ_CREATE(CNewSkillProperties::tagOperateObject);
        assert(ptgOperateObject);
        ptgOperateObject->dwID = _GetDwordFromByteArray(pByte,pos);
        ptgOperateObject->distance = _GetFloatFromByteArray(pByte,pos);
        ptgOperateObject->radius = _GetFloatFromByteArray(pByte,pos);
        ptgOperateObject->angle = _GetFloatFromByteArray(pByte,pos);
        ptgOperateObject->width = _GetFloatFromByteArray(pByte,pos);
        ptgOperateObject->dwSkillID = _GetDwordFromByteArray(pByte,pos);
        ptgOperateObject->dwSkillLevel = _GetDwordFromByteArray(pByte,pos);
        ptgOperateObject->dwConGroupID = _GetDwordFromByteArray(pByte,pos);	
        ptgOperateObject->dwNumObject = _GetDwordFromByteArray(pByte,pos);

        _GetStringFromByteArray(pByte,pos,strTemp);
        ptgOperateObject->strType =  strTemp;

        _GetStringFromByteArray(pByte,pos,strTemp);
        ptgOperateObject->strPosition = strTemp;

        _GetStringFromByteArray(pByte,pos,strTemp);
        ptgOperateObject->strCenterObject = strTemp;

        _GetStringFromByteArray(pByte,pos,strTemp);
        ptgOperateObject->strObject = strTemp;

        _GetStringFromByteArray(pByte,pos,strTemp);
        ptgOperateObject->NumObjectName = strTemp;

        ptgOperateObject->IsWithIn = _GetByteFromByteArray(pByte,pos);

        g_skillCommon.m_mapOperateObject[ ptgOperateObject->dwID ] = ptgOperateObject ;

    }
    */

	for (DWORD i=0; i<dwNum; ++i)
	{
		CNewSkill *pProperties = OBJ_CREATE(CNewSkill);
		assert(pProperties);
		//ID
		DWORD dwID = _GetDwordFromByteArray(pByte,pos);
		pProperties->SetNewSkillID(dwID);
		//名称
		_GetStringFromByteArray(pByte,pos,strTemp);
		pProperties->SetNewSkillName(strTemp);
		//描述
		_GetStringFromByteArray(pByte,pos,strTemp);
		pProperties->SetDescription(strTemp);
		//元素类型
		DWORD dwElement = _GetDwordFromByteArray(pByte,pos);
		pProperties->SetElement(dwElement);
		//类型
		_GetStringFromByteArray(pByte,pos,strTemp);		
		pProperties->SetSkillType(strTemp);
		//类别
		BYTE bUseType = _GetByteFromByteArray(pByte,pos);
		pProperties->SetUseType(bUseType);

		//唯一标志符
		BYTE bOnly = _GetByteFromByteArray(pByte,pos);
		pProperties->SetOnly(bOnly);
		//唯一类型
		DWORD dwOnlyType = _GetDwordFromByteArray(pByte,pos);
		pProperties->SetOnlyType(dwOnlyType);
		//名称唯一标志符
		BYTE bNameOnly = _GetByteFromByteArray(pByte,pos);
		pProperties->SetNameOnly(bNameOnly);
		//角色唯一标志2符
		BYTE bRoleOnly = _GetByteFromByteArray(pByte,pos);
		pProperties->SetRoleOnly(bRoleOnly);
		//攻击标志符
		BYTE bAttack = _GetByteFromByteArray(pByte,pos);
		pProperties->SetAttack(bAttack);
		//目标标志符
		BYTE bTarget = _GetByteFromByteArray(pByte,pos);
		pProperties->SetTarget(bTarget);
		//冷却时间
		BYTE bCooldown = _GetByteFromByteArray(pByte,pos);
		pProperties->SetCoolDown(bCooldown);
		//死亡目标
		BYTE bDeadUse = _GetByteFromByteArray(pByte,pos);
		pProperties->SetDeadUse(bDeadUse);
		//死亡结束
		BYTE bDeadEnd = _GetByteFromByteArray(pByte,pos);
		pProperties->SetDeadEnd(bDeadEnd);
		BYTE bBreak = _GetByteFromByteArray(pByte,pos);
		pProperties->SetBreak(bBreak);
		//宠物技能类型
		BYTE bPetType = _GetByteFromByteArray(pByte,pos);
		pProperties->SetPetType(bPetType);
		//计时标志符
		BYTE bTime = _GetByteFromByteArray(pByte,pos);
		pProperties->SetTime(bTime);
		//系统标志符
		BYTE bSystem = _GetByteFromByteArray(pByte,pos);
		pProperties->SetSystem(bSystem);
		//累加数量
		DWORD dwTotalNum = _GetDwordFromByteArray(pByte,pos);
		pProperties->SetTotalNum(dwTotalNum);
        //CD时间控制
        DWORD dwCDConTrol = _GetDwordFromByteArray(pByte,pos);
        pProperties->SetCDControl(dwCDConTrol);
		//增益减益
		_GetStringFromByteArray(pByte,pos,strTemp);
		pProperties->SetAddSub(strTemp);
        //当前技能攻击类型(新增加)
        _GetStringFromByteArray(pByte,pos,strTemp);
        pProperties->SetSkillAttType(strTemp);

		//公式个数
		long lFNum = _GetLongFromByteArray(pByte,pos);
		for (int i=0; i<lFNum; ++i)
		{
			//公式ID
			DWORD dwID = _GetDwordFromByteArray(pByte,pos);
			_GetStringFromByteArray(pByte,pos,strTemp);
			char strContend[1024];
			_GetStringFromByteArray(pByte,pos,strContend);
			pProperties->SetFormat(dwID,string(strTemp),string(strContend));
		}
		//条件组
		//个数
		long lCGNum = _GetLongFromByteArray(pByte,pos);
		for (int i=0; i<lCGNum; ++i)
		{
			DWORD dwConGroupID = _GetDwordFromByteArray(pByte,pos);

			CNewSkillProperties::tagConditionGroup *ptgCondition = OBJ_CREATE(CNewSkillProperties::tagConditionGroup);
			assert(ptgCondition);
			_GetStringFromByteArray(pByte,pos,strTemp);
			ptgCondition->strMode =strTemp;

			long lCNum = _GetLongFromByteArray(pByte,pos);		
			for (int j=0; j<lCNum; ++j)
			{
				DWORD dwConID = _GetDwordFromByteArray(pByte,pos);
				ptgCondition->lConditonGroup.push_back(dwConID);
			}
			pProperties->SetConGroup(dwConGroupID,ptgCondition);
		}

		//条件体
		long lConditionNum = _GetLongFromByteArray(pByte,pos);
		for (int i=0; i<lConditionNum; ++i)
		{

			DWORD dwID = _GetDwordFromByteArray(pByte,pos);
			CNewSkillProperties::tagCondition *ptgCondition = OBJ_CREATE(CNewSkillProperties::tagCondition);
			assert(ptgCondition);
			_GetStringFromByteArray(pByte,pos,strTemp);
			DWORD dwLength = _GetDwordFromByteArray(pByte,pos);	
			GetConditon(pByte,pos, strTemp,dwLength,ptgCondition);
			pProperties->SetCondition(dwID,ptgCondition);		
		}
		//作用对象个数
		long lONum = _GetLongFromByteArray(pByte,pos);
		for (int j=0; j<lONum; ++j)
		{
			CNewSkillProperties::tagOperateObject *ptgOperateObject = OBJ_CREATE(CNewSkillProperties::tagOperateObject);
			assert(ptgOperateObject);
			ptgOperateObject->dwID = _GetDwordFromByteArray(pByte,pos);
			ptgOperateObject->distance = _GetFloatFromByteArray(pByte,pos);
			ptgOperateObject->radius = _GetFloatFromByteArray(pByte,pos);
            ptgOperateObject->angle = _GetFloatFromByteArray(pByte,pos);
			ptgOperateObject->width = _GetFloatFromByteArray(pByte,pos);
			ptgOperateObject->dwSkillID = _GetDwordFromByteArray(pByte,pos);
			ptgOperateObject->dwSkillLevel = _GetDwordFromByteArray(pByte,pos);
			ptgOperateObject->dwConGroupID = _GetDwordFromByteArray(pByte,pos);	
			ptgOperateObject->dwNumObject = _GetDwordFromByteArray(pByte,pos);

			_GetStringFromByteArray(pByte,pos,strTemp);
			ptgOperateObject->strType =  strTemp;

			_GetStringFromByteArray(pByte,pos,strTemp);
			ptgOperateObject->strPosition = strTemp;

			_GetStringFromByteArray(pByte,pos,strTemp);
			ptgOperateObject->strCenterObject = strTemp;

			_GetStringFromByteArray(pByte,pos,strTemp);
			ptgOperateObject->strObject = strTemp;

            _GetStringFromByteArray(pByte,pos,strTemp);
            ptgOperateObject->NumObjectName = strTemp;

            ptgOperateObject->IsWithIn = _GetByteFromByteArray(pByte,pos);

			pProperties->SetOperateObject(ptgOperateObject->dwID,ptgOperateObject);

		}


		//各个等级的技能个数
		long lSkillNum = _GetLongFromByteArray(pByte,pos);

		for (int i=0; i<lSkillNum; ++i)
		{
			CNewSkillProperties::tagSkill *ptgSkill = OBJ_CREATE(CNewSkillProperties::tagSkill);	
			assert(ptgSkill);

			//ptgSkill->pStaticParam = NULL;
			//是否有静态参数		
            /*
                BYTE bStatic = _GetByteFromByteArray(pByte,pos);
			    if(bStatic)
			    {
				    CNewSkillProperties::stStaticParam* pParam = OBJ_CREATE(CNewSkillProperties::stStaticParam);	
				    assert(pParam);
				    pParam->lCoolDownTime=_GetLongFromByteArray(pByte,pos);
				    pParam->lConsMp=_GetLongFromByteArray(pByte,pos);
				    pParam->MinAtkDistance=_GetFloatFromByteArray(pByte,pos);
				    pParam->MaxAtkDistance=_GetFloatFromByteArray(pByte,pos);
				    pParam->lActTime = _GetLongFromByteArray(pByte,pos);
				    _GetStringFromByteArray(pByte,pos,strTemp);
				    CStrConverter::parseList(strTemp,pParam->lBuffID);
				    _GetStringFromByteArray(pByte,pos,strTemp);
				    CStrConverter::parseList(strTemp,pParam->lNBuffID);
				    _GetStringFromByteArray(pByte,pos,strTemp);
				    CStrConverter::parseList(strTemp,pParam->lScriptParam);
				    ptgSkill->pStaticParam = pParam;
			    }
            */

			//dwTemplateId
			ptgSkill->dwTemplateId = _GetDwordFromByteArray(pByte,pos);

			long lCondNum = _GetLongFromByteArray(pByte,pos);
			for (int i=0; i<lCondNum; ++i)
			{

				DWORD dwID = _GetDwordFromByteArray(pByte,pos);
				CNewSkillProperties::tagCondition *ptgCondition = OBJ_CREATE(CNewSkillProperties::tagCondition);	
				_GetStringFromByteArray(pByte,pos,strTemp);
				DWORD dwLength = _GetDwordFromByteArray(pByte,pos);	
				GetConditon(pByte,pos, strTemp,dwLength,ptgCondition);
				ptgSkill->mapCondition[dwID] = ptgCondition;			
			}

			//步骤个数
			long lSNum = _GetLongFromByteArray(pByte,pos);
			for (int j=0; j<lSNum; ++j)
			{

				CNewSkillProperties::tagProcess *ptgProcess = OBJ_CREATE(CNewSkillProperties::tagProcess);
				_GetStringFromByteArray(pByte,pos,strTemp);
				ptgProcess->strName = strTemp;


				//执行个数
				long lEPNum = _GetLongFromByteArray(pByte,pos);
				for (int m=0; m<lEPNum; ++m)
				{
					CNewSkillProperties::tagStep *ptgStep = OBJ_CREATE(CNewSkillProperties::tagStep);		
					_GetStringFromByteArray(pByte,pos,strTemp);
					ptgStep->strName = strTemp;
					//DO个数
					long lDNum = _GetLongFromByteArray(pByte,pos);
					for (int n=0; n<lDNum; ++n)
					{


						CNewSkillProperties::tagDo *ptgDo = OBJ_CREATE(CNewSkillProperties::tagDo);
						ptgDo->dwOperobjid =_GetDwordFromByteArray(pByte,pos);
						_GetStringFromByteArray(pByte,pos,strTemp);
						ptgDo->strRet = strTemp;

						long lCNum = _GetLongFromByteArray(pByte,pos);
						for (int index=0; index<lCNum; ++index)
						{
							CNewSkillProperties::tagCDo *ptgCDo = OBJ_CREATE(CNewSkillProperties::tagCDo);				
							_GetStringFromByteArray(pByte,pos,strTemp);
							ptgCDo->tgCondition.strTrueRet = strTemp;
							_GetStringFromByteArray(pByte,pos,strTemp);
							ptgCDo->tgCondition.strFalseRet = strTemp;

							ptgCDo->tgCondition.dwConditionGroup = _GetDwordFromByteArray(pByte,pos);						

							//真处理个数
							long lDPNum = _GetLongFromByteArray(pByte,pos);
							for (int x=0; x<lDPNum; ++x)
							{
								CNewSkillProperties::tagDoProcess *ptgTDoProcess = DecodeDoProcess(pByte,pos);;

								ptgCDo->ltgTrueDoProcess.push_back(ptgTDoProcess);

							}
							//假处理个娄
							long lFDPNum = _GetLongFromByteArray(pByte,pos);
							for (int x=0; x<lFDPNum; ++x)
							{
								CNewSkillProperties::tagDoProcess *ptgFDoProcess = DecodeDoProcess(pByte,pos);

								ptgCDo->ltgFalseDoProcess.push_back(ptgFDoProcess);
							}
							ptgDo->ltgCDo.push_back(ptgCDo);

						}
						ptgStep->ltgDo.push_back(ptgDo);						//ptgcd

					}					

					ptgProcess->ltgStep.push_back(ptgStep);

				}

				ptgSkill->ltgExecuteStep.push_back(ptgProcess);

			}
			pProperties->AddNewSkill(ptgSkill);
		}	

		if (pProperties)
		{
			g_mNewSkillProperties[dwID] = pProperties;
		}		
	}
}
/*
NAME:DecodeDoProcess
DESC:操作过程解码
PARA:pByte解码数据
pos位置
*/
CNewSkillProperties::tagDoProcess *CNewSkillFactory::DecodeDoProcess(BYTE* pByte, long& pos)
{
	char strTemp[1024];

	CNewSkillProperties::tagDoProcess *ptgDoProcess = OBJ_CREATE(CNewSkillProperties::tagDoProcess);

	long lCNum = _GetLongFromByteArray(pByte,pos);
	for (int index=0; index<lCNum; ++index)
	{
		CNewSkillProperties::tagCDo *ptgCDo = OBJ_CREATE(CNewSkillProperties::tagCDo);				
		_GetStringFromByteArray(pByte,pos,strTemp);
		ptgCDo->tgCondition.strTrueRet = strTemp;
		_GetStringFromByteArray(pByte,pos,strTemp);
		ptgCDo->tgCondition.strFalseRet = strTemp;

		ptgCDo->tgCondition.dwConditionGroup = _GetDwordFromByteArray(pByte,pos);						

		//真处理个数
		long lDPNum = _GetLongFromByteArray(pByte,pos);
		for (int x=0; x<lDPNum; ++x)
		{
			CNewSkillProperties::tagDoProcess *ptgTDoProcess = DecodeDoProcess(pByte,pos);;

			ptgCDo->ltgTrueDoProcess.push_back(ptgTDoProcess);

		}
		//假处理个娄
		long lFDPNum = _GetLongFromByteArray(pByte,pos);
		for (int x=0; x<lFDPNum; ++x)
		{
			CNewSkillProperties::tagDoProcess *ptgFDoProcess = DecodeDoProcess(pByte,pos);

			ptgCDo->ltgFalseDoProcess.push_back(ptgFDoProcess);
		}
		ptgDoProcess->ltgCDo.push_back(ptgCDo);

	}


	//个数
	long lNum = _GetLongFromByteArray(pByte,pos);
	for (int i=0; i<lNum; ++i)
	{
		_GetStringFromByteArray(pByte,pos,strTemp);
		string strType = strTemp;
		_GetStringFromByteArray(pByte,pos,strTemp);
		string strObj = strTemp;
		if (strType=="Script")
		{
			//脚本
			CNewSkillProperties::tagScriptID *ptgScriptID = OBJ_CREATE(CNewSkillProperties::tagScriptID);
			ptgScriptID->dwScriptID = _GetDwordFromByteArray(pByte,pos);
			ptgScriptID->strChangeType = strType;
			ptgScriptID->strObj = strObj;				
			ptgDoProcess->ltgBaseChange.push_back(ptgScriptID);

		} 
		else if (strType=="ObjectPropertyChange")
		{
			//基本属性
			CNewSkillProperties::tagChangeProperty *ptgChangeProperty = OBJ_CREATE(CNewSkillProperties::tagChangeProperty);
			ptgChangeProperty->dwValue = _GetDwordFromByteArray(pByte,pos);
			_GetStringFromByteArray(pByte,pos,strTemp);
			ptgChangeProperty->strName = strTemp;
			_GetStringFromByteArray(pByte,pos,strTemp);
			ptgChangeProperty->strOperator = strTemp;
			_GetStringFromByteArray(pByte,pos,strTemp);
			ptgChangeProperty->strScriptVarName = strTemp;
            _GetStringFromByteArray(pByte,pos,strTemp);
            ptgChangeProperty->paramname = strTemp;

			ptgChangeProperty->strChangeType = strType;
			ptgChangeProperty->strObj = strObj;
			ptgDoProcess->ltgBaseChange.push_back(ptgChangeProperty);
		}
		else if (strType=="BuffChange")
		{
			//状态
			CNewSkillProperties::tagChangeBuff *ptgChangeBuff = OBJ_CREATE(CNewSkillProperties::tagChangeBuff);
			ptgChangeBuff->dwID = _GetDwordFromByteArray(pByte,pos);
			ptgChangeBuff->dwLevel = _GetDwordFromByteArray(pByte,pos);

			ptgChangeBuff->dwCount = _GetDwordFromByteArray(pByte,pos);
			_GetStringFromByteArray(pByte,pos,strTemp);
			ptgChangeBuff->strSIDName = strTemp;
			_GetStringFromByteArray(pByte,pos,strTemp);
			ptgChangeBuff->strSLvName = strTemp;
			_GetStringFromByteArray(pByte,pos,strTemp);
			ptgChangeBuff->strSCountName = strTemp;

			_GetStringFromByteArray(pByte,pos,strTemp);
			ptgChangeBuff->strOperator = strTemp;
            _GetStringFromByteArray(pByte,pos,strTemp);
            ptgChangeBuff->strSkillType = strTemp;

			ptgChangeBuff->strChangeType=strType;
			ptgChangeBuff->strObj = strObj;				
			ptgDoProcess->ltgBaseChange.push_back(ptgChangeBuff);
		} 
		else if (strType=="PosChange")
		{
			//位置改变
			CNewSkillProperties::tagChangePos *ptgChangePos = OBJ_CREATE(CNewSkillProperties::tagChangePos);
			_GetStringFromByteArray(pByte,pos,strTemp);
			ptgChangePos->strDir = strTemp;			
			_GetStringFromByteArray(pByte,pos,strTemp);
			ptgChangePos->strRefObject = strTemp;
			_GetStringFromByteArray(pByte,pos,strTemp);
			ptgChangePos->strStyle = strTemp;
			_GetStringFromByteArray(pByte,pos,strTemp);
			ptgChangePos->strSpeed = strTemp;
            ptgChangePos->dis = _GetFloatFromByteArray(pByte, pos);

			ptgChangePos->strChangeType = strType;
			ptgChangePos->strObj = strObj;			
			ptgDoProcess->ltgBaseChange.push_back(ptgChangePos);
		}
		else if (strType=="AttakObjectChange")
		{
			CNewSkillProperties::tagAttackChangeObject *pTemp = OBJ_CREATE(CNewSkillProperties::tagAttackChangeObject);
			_GetStringFromByteArray(pByte,pos,strTemp);
			pTemp->strAttackChangeObject=strTemp;			
			pTemp->strChangeType = strType;
			pTemp->strObj;
			ptgDoProcess->ltgBaseChange.push_back(pTemp);

		}
		else if (strType=="CooldownChange")
		{
			//时间
			CNewSkillProperties::tagCoolDownChange *ptgCoolDownChange = OBJ_CREATE(CNewSkillProperties::tagCoolDownChange);
			ptgCoolDownChange->dwValue = _GetDwordFromByteArray(pByte,pos);
			_GetStringFromByteArray(pByte,pos,strTemp);
			ptgCoolDownChange->strType = strTemp;
			_GetStringFromByteArray(pByte,pos,strTemp);
			ptgCoolDownChange->strSVarianame = strTemp;
            _GetStringFromByteArray(pByte,pos,strTemp);
            ptgCoolDownChange->paramname = strTemp;
			ptgCoolDownChange->strChangeType = strType;// "CoolDown";
			ptgCoolDownChange->strObj = strObj;
			ptgDoProcess->ltgBaseChange.push_back(ptgCoolDownChange);			
		}
		else if (strType=="DestObjsChange")
		{
			//目标对象
			CNewSkillProperties::tagDestObjChange *pTemp = OBJ_CREATE(CNewSkillProperties::tagDestObjChange);
			_GetStringFromByteArray(pByte,pos,strTemp);
			pTemp->strDestObjChange = strTemp;			
			pTemp->strChangeType = strType;
			pTemp->strObj = strObj;
			ptgDoProcess->ltgBaseChange.push_back(pTemp);
		}
		else if (strType=="RegisterTimer")
		{
			//注册定时器
			CNewSkillProperties::tagRegisterTimer *ptgRegistTimer = OBJ_CREATE(CNewSkillProperties::tagRegisterTimer);
			ptgRegistTimer->dwCount = _GetDwordFromByteArray(pByte,pos);
			ptgRegistTimer->dwFlag = _GetDwordFromByteArray(pByte,pos);
			ptgRegistTimer->dwFuture = _GetDwordFromByteArray(pByte,pos);
			ptgRegistTimer->dwIntelTime = _GetDwordFromByteArray(pByte,pos);

			_GetStringFromByteArray(pByte,pos,strTemp);
			ptgRegistTimer->strSCountName = strTemp;
			_GetStringFromByteArray(pByte,pos,strTemp);
			ptgRegistTimer->strSFutvarname = strTemp;
			_GetStringFromByteArray(pByte,pos,strTemp);
			ptgRegistTimer->strSIntelName = strTemp;
            _GetStringFromByteArray(pByte,pos,strTemp);
            ptgRegistTimer->strVFuturename = strTemp;
            _GetStringFromByteArray(pByte,pos,strTemp);
            ptgRegistTimer->strVIntername = strTemp;
            _GetStringFromByteArray(pByte,pos,strTemp);
            ptgRegistTimer->strVCountname = strTemp;

			ptgRegistTimer->strChangeType = strType;
			ptgRegistTimer->strObj = strObj;

			ptgDoProcess->ltgBaseChange.push_back(ptgRegistTimer);
		}		
		else if (strType=="RegisterEvent")
		{
			//注册事件
			CNewSkillProperties::tagRegisterEvent *ptgRegisterEvent = OBJ_CREATE(CNewSkillProperties::tagRegisterEvent);

			ptgRegisterEvent->dwFlag = _GetDwordFromByteArray(pByte,pos);
			_GetStringFromByteArray(pByte,pos,strTemp);
			ptgRegisterEvent->strName = strTemp;
			ptgRegisterEvent->strChangeType = strType;
			ptgRegisterEvent->strObj = strObj;
			ptgDoProcess->ltgBaseChange.push_back(ptgRegisterEvent);

		}
		else if (strType=="UnRegisterTimer")
		{			
			//注消时间
			CNewSkillProperties::tagUnRegisterTimer *pTemp = OBJ_CREATE(CNewSkillProperties::tagUnRegisterTimer);
			pTemp->dwUnRegisterTimer =_GetDwordFromByteArray(pByte,pos);
			pTemp->strChangeType = strType;
			pTemp->strObj = strObj;

			ptgDoProcess->ltgBaseChange.push_back(pTemp);
		}
		else if (strType=="UnRegisterEvent")
		{
			//注消事件
			CNewSkillProperties::tagUnRegisterEvent *pTemp = OBJ_CREATE(CNewSkillProperties::tagUnRegisterEvent);
			pTemp->dwUnRegisterEvent = _GetDwordFromByteArray(pByte,pos);
			pTemp->strChangeType = strType;
			pTemp->strObj = strObj;
			ptgDoProcess->ltgBaseChange.push_back(pTemp);				
		}
		else if (strType=="SendMsg")
		{
			//发送消息
			CNewSkillProperties::tagSendMsg *ptgSendMsg = OBJ_CREATE(CNewSkillProperties::tagSendMsg);
			ptgSendMsg->dwValue = _GetDwordFromByteArray(pByte,pos);

			_GetStringFromByteArray(pByte,pos,strTemp);
			ptgSendMsg->strType = strTemp;

			_GetStringFromByteArray(pByte,pos,strTemp);
			ptgSendMsg->strRange = strTemp;

			_GetStringFromByteArray(pByte,pos,strTemp);
			ptgSendMsg->strSVName = strTemp;

			ptgSendMsg->nAdd = _GetByteFromByteArray(pByte,pos);

			ptgSendMsg->strChangeType =strType;
			ptgSendMsg->strObj = strObj;

			ptgDoProcess->ltgBaseChange.push_back(ptgSendMsg);
		}
		else if (strType=="DamageChange")
		{
			//伤害计算
			CNewSkillProperties::tagDamageChange *ptgDamageChange = OBJ_CREATE(CNewSkillProperties::tagDamageChange);
			ptgDamageChange->dwValue = _GetDwordFromByteArray(pByte,pos);			

			_GetStringFromByteArray(pByte,pos,strTemp);
			ptgDamageChange->strOperator = strTemp;

			_GetStringFromByteArray(pByte,pos,strTemp);
			ptgDamageChange->strType = strTemp;

			_GetStringFromByteArray(pByte,pos,strTemp);
			ptgDamageChange->strSVarianame = strTemp;

            _GetStringFromByteArray(pByte,pos,strTemp);/// Damagename
            ptgDamageChange->strDamagename = strTemp;

			ptgDamageChange->strChangeType = strType;
			ptgDamageChange->strObj = strObj;			
			ptgDoProcess->ltgBaseChange.push_back(ptgDamageChange);
		}
		else if (strType=="DoDamage")
		{
			//伤害处理
			CNewSkillProperties::tagDoDamage *ptgDoDamage = OBJ_CREATE(CNewSkillProperties::tagDoDamage);
			ptgDoDamage->dwBlastAttack = _GetDwordFromByteArray(pByte,pos);
			ptgDoDamage->dwResult = _GetDwordFromByteArray(pByte,pos);

			_GetStringFromByteArray(pByte,pos,strTemp);
			ptgDoDamage->strSBlastVariName = strTemp;

			_GetStringFromByteArray(pByte,pos,strTemp);
			ptgDoDamage->strSResVariName = strTemp;		

            _GetStringFromByteArray(pByte,pos,strTemp);
            ptgDoDamage->strDamagename = strTemp;

            _GetStringFromByteArray(pByte,pos,strTemp);
            ptgDoDamage->strBlastname = strTemp;	

			ptgDoDamage->strChangeType = strType;
			ptgDoDamage->strObj = strObj;
			ptgDoProcess->ltgBaseChange.push_back(ptgDoDamage);
		}
		else if (strType=="GoodChange")
		{
			//物品
			CNewSkillProperties::tagGoodChange *ptgGoodChange = OBJ_CREATE(CNewSkillProperties::tagGoodChange);
			ptgGoodChange->dwPlace = _GetDwordFromByteArray(pByte,pos);
			ptgGoodChange->dwPos = _GetDwordFromByteArray(pByte,pos);
			ptgGoodChange->dwValue = _GetDwordFromByteArray(pByte,pos);
			_GetStringFromByteArray(pByte,pos,strTemp);
			ptgGoodChange->strName = strTemp;
			_GetStringFromByteArray(pByte,pos,strTemp);
			ptgGoodChange->strOperator = strTemp;				
			ptgGoodChange->strChangeType = strType;
			ptgGoodChange->strObj = strObj;
			ptgGoodChange->dwGoodsType = _GetDwordFromByteArray(pByte,pos);
			ptgGoodChange->dwWeaponType = _GetDwordFromByteArray(pByte,pos);
			ptgDoProcess->ltgBaseChange.push_back(ptgGoodChange);
		}
		else if (strType=="DirecChange")
		{
			//方向
			CNewSkillProperties::tagDirection *pTemp = OBJ_CREATE(CNewSkillProperties::tagDirection);
			_GetStringFromByteArray(pByte,pos,strTemp);
			pTemp->strDir = strTemp;
			pTemp->strChangeType = strType;
			pTemp->strObj = strObj;
			ptgDoProcess->ltgBaseChange.push_back(pTemp);
		}
		else if (strType=="SkillChange")
		{
			//技能
			CNewSkillProperties::tagSkillChange *pTemp = OBJ_CREATE(CNewSkillProperties::tagSkillChange);
			_GetStringFromByteArray(pByte,pos,strTemp);
			pTemp->strObj = strTemp;
			_GetStringFromByteArray(pByte,pos,strTemp);
			pTemp->strType = strTemp;
			_GetStringFromByteArray(pByte,pos,strTemp);
			pTemp->strOpera = strTemp;
			pTemp->lID = _GetLongFromByteArray(pByte,pos);
			pTemp->lLv = _GetLongFromByteArray(pByte,pos);
			pTemp->strChangeType = strType;
			ptgDoProcess->ltgBaseChange.push_back(pTemp);

		}
        else if (strType=="GeniusChange")
        {
            CNewSkillProperties::tagGeniusChange *pTemp = OBJ_CREATE(CNewSkillProperties::tagGeniusChange);
            _GetStringFromByteArray(pByte,pos,strTemp);
            pTemp->strGenius = strTemp;

            pTemp->strChangeType = strType;
            ptgDoProcess->ltgBaseChange.push_back(pTemp);
        }

	}	
	return ptgDoProcess;
}
/*
NAME:GetConditon()
DESC:根据职业得到相对应初始化技能列表
PARA:dwOccu职业
*/
list<long>	CNewSkillFactory::GetInitOccuSkill(DWORD dwOccu)
{
	list<long> lSkill;
	map<DWORD,list<long>>::iterator it = g_mapOccuInitSkill.find(dwOccu);
	if (it!=g_mapOccuInitSkill.end())
	{
		return it->second;
	}
	return lSkill;
}
/*
NAME:GetConditon()
DESC:解码条件
PARA:pByte解码数据
pos位置
strType条件类型
dwLength长度
ptgCondition条件
*/
void CNewSkillFactory::GetConditon(BYTE* pByte, long& pos,string strType,DWORD dwLength,CNewSkillProperties::tagCondition *ptgCondition)
{
	char strTemp[1024];
	ptgCondition->dwLength = dwLength;
	ptgCondition->strType = strType;
	if (strType=="BaseProperty")
	{
		//基本属性
		CNewSkillProperties::tagConProperty *ptgConProperty = OBJ_CREATE(CNewSkillProperties::tagConProperty);
		for (DWORD j=0;j<dwLength;++j)
		{
			CNewSkillProperties::tagPropertyCondition *ptgPropertyCondition = OBJ_CREATE(CNewSkillProperties::tagPropertyCondition);
			ptgPropertyCondition->dwValue = _GetDwordFromByteArray(pByte,pos);

			_GetStringFromByteArray(pByte,pos,strTemp);
			ptgPropertyCondition->cmpareOp = strTemp;

			_GetStringFromByteArray(pByte,pos,strTemp);
			ptgPropertyCondition->strName = strTemp;

			_GetStringFromByteArray(pByte,pos,strTemp);
			ptgPropertyCondition->strObject = strTemp;		

            _GetStringFromByteArray(pByte,pos,strTemp);
            ptgPropertyCondition->paramvalue = strTemp;	

			ptgConProperty->ltgProtertyConditon.push_back(ptgPropertyCondition);
		}
		ptgCondition->pVoidCondition = reinterpret_cast<void*>(ptgConProperty);
	} 
	else if(strType=="Good")
	{
		//物品
		CNewSkillProperties::tagConGood *ptgConGood = OBJ_CREATE(CNewSkillProperties::tagConGood);
		for (DWORD j=0;j<dwLength;++j)
		{
			CNewSkillProperties::tagGoodCondition *ptgGoodCondition = OBJ_CREATE(CNewSkillProperties::tagGoodCondition);
			ptgGoodCondition->dwPlace = _GetDwordFromByteArray(pByte,pos);
			ptgGoodCondition->dwPos = _GetDwordFromByteArray(pByte,pos);
			ptgGoodCondition->dwValue = _GetDwordFromByteArray(pByte,pos);

			_GetStringFromByteArray(pByte,pos,strTemp);
			ptgGoodCondition->strCmpFun = strTemp;

			_GetStringFromByteArray(pByte,pos,strTemp);
			ptgGoodCondition->strName = strTemp;

			ptgGoodCondition->dwType = _GetDwordFromByteArray(pByte,pos);

			_GetStringFromByteArray(pByte,pos,strTemp);
			ptgGoodCondition->strObject = strTemp;

			long lGNum = _GetLongFromByteArray(pByte,pos);
			for (long l=0;l<lGNum;++l)
			{
				CNewSkillProperties::tagGoodAddProperty *ptgGoodAddProperty = OBJ_CREATE(CNewSkillProperties::tagGoodAddProperty);
				_GetBufferFromByteArray(pByte,pos,ptgGoodAddProperty,sizeof(CNewSkillProperties::tagGoodAddProperty));
				ptgGoodCondition->ltgGoodAddProperty.push_back(ptgGoodAddProperty);
			}	

			ptgConGood->ltgGoodCondition.push_back(ptgGoodCondition);
		}
		ptgCondition->pVoidCondition = reinterpret_cast<void*>(ptgConGood);	
	}
	else if (strType=="Buff")
	{
		//状态
		CNewSkillProperties::tagConBuff *ptgConBuff = OBJ_CREATE(CNewSkillProperties::tagConBuff);
		for (DWORD j=0;j<dwLength;++j)
		{
			CNewSkillProperties::tagBuffCondition *ptgBuffCondition = OBJ_CREATE(CNewSkillProperties::tagBuffCondition);
			ptgBuffCondition->dwID = _GetDwordFromByteArray(pByte,pos);
			ptgBuffCondition->dwLevel = _GetDwordFromByteArray(pByte,pos);
			ptgBuffCondition->dwNum = _GetDwordFromByteArray(pByte,pos);
            _GetStringFromByteArray(pByte,pos,strTemp);
            ptgBuffCondition->strOper = strTemp;
			_GetStringFromByteArray(pByte,pos,strTemp);
			ptgBuffCondition->strObject = strTemp;
            _GetStringFromByteArray(pByte,pos,strTemp);
            ptgBuffCondition->strBufObject = strTemp;
            _GetStringFromByteArray(pByte,pos,strTemp);
            ptgBuffCondition->strLevel = strTemp;
			ptgConBuff->ltgBuffCondition.push_back(ptgBuffCondition);
		}
		ptgCondition->pVoidCondition = reinterpret_cast<void*>(ptgConBuff);	
	}
    else if (strType=="Odds")
    {
        CNewSkillProperties::tagConOdds* ptgConOdds = OBJ_CREATE(CNewSkillProperties::tagConOdds);
        for (DWORD j=0;j<dwLength;++j)
        {
            CNewSkillProperties::tagOddsCondition *ptgOddsCondition =  OBJ_CREATE( CNewSkillProperties::tagOddsCondition );
            ptgOddsCondition->value = _GetShortFromByteArray(pByte,pos);
            _GetStringFromByteArray(pByte,pos,strTemp);
            ptgOddsCondition->strCmpFun = strTemp;
            _GetStringFromByteArray(pByte,pos,strTemp);
            ptgOddsCondition->strValueName = strTemp;

            ptgConOdds->ltgOddsCondition.push_back(ptgOddsCondition);
        }
        ptgCondition->pVoidCondition = reinterpret_cast<void*>(ptgConOdds);	
    }
	
	else if (strType=="Distance")
	{
		//距离
		CNewSkillProperties::tagConDistance *ptgConDistance = OBJ_CREATE(CNewSkillProperties::tagConDistance);
		for (DWORD j=0;j<dwLength;++j)
		{
			CNewSkillProperties::tagDistanceCondition *ptgDistanceCondition = OBJ_CREATE(CNewSkillProperties::tagDistanceCondition);
			ptgDistanceCondition->value = _GetFloatFromByteArray(pByte,pos);
			_GetStringFromByteArray(pByte,pos,strTemp);
			ptgDistanceCondition->strCmpFun = strTemp;
			_GetStringFromByteArray(pByte,pos,strTemp);
			ptgDistanceCondition->strRefObj = strTemp;
            _GetStringFromByteArray(pByte,pos,strTemp);
            ptgDistanceCondition->strValueName = strTemp;

			ptgConDistance->ltgDistanceCondition.push_back(ptgDistanceCondition);
		}
		ptgCondition->pVoidCondition = reinterpret_cast<void*>(ptgConDistance);	
	}
	else if (strType=="ValidObject")
	{
		//有效对象
		CNewSkillProperties::tagConValidObject *ptgConValidObject = OBJ_CREATE(CNewSkillProperties::tagConValidObject);
		for (DWORD j=0;j<dwLength;++j)
		{
			CNewSkillProperties::tagValidObjectCondition *ptgValidObjectCondition = OBJ_CREATE(CNewSkillProperties::tagValidObjectCondition);
			_GetStringFromByteArray(pByte,pos,strTemp);
			ptgValidObjectCondition->strType = strTemp;
			long lNum = _GetLongFromByteArray(pByte,pos);
			for (int i=0; i<lNum; ++i)
			{
				_GetStringFromByteArray(pByte,pos,strTemp);
				ptgValidObjectCondition->lObjType.push_back(strTemp);
			}
			ptgConValidObject->ltgValidObjectCondition.push_back(ptgValidObjectCondition);

		}
		ptgCondition->pVoidCondition  = reinterpret_cast<void*>(ptgConValidObject);
	}
	else if (strType=="Event")
	{
		//事件
		CNewSkillProperties::tagConEvent *ptgConEvent = OBJ_CREATE(CNewSkillProperties::tagConEvent);
		for(DWORD j=0; j<dwLength; ++j)
		{

			CNewSkillProperties::tagEventCondition *ptgEventCondition = OBJ_CREATE(CNewSkillProperties::tagEventCondition);
			ptgEventCondition->dwFlag = _GetDwordFromByteArray(pByte,pos);
			_GetStringFromByteArray(pByte,pos,strTemp);
			ptgEventCondition->strEventName = strTemp;

			ptgEventCondition->dwVar1 = _GetDwordFromByteArray(pByte,pos);
			ptgEventCondition->dwVar2 =_GetDwordFromByteArray(pByte,pos);
			ptgEventCondition->dwVar3 = _GetDwordFromByteArray(pByte,pos);
			ptgEventCondition->dwVar4 = _GetDwordFromByteArray(pByte,pos);

            _GetStringFromByteArray(pByte,pos,strTemp);
            ptgEventCondition->dwSkillId = strTemp;
			_GetStringFromByteArray(pByte,pos,strTemp);
			ptgEventCondition->strName = strTemp;

			ptgConEvent->ltgEventCondition.push_back(ptgEventCondition);
		}
		ptgCondition->pVoidCondition = reinterpret_cast<void*>(ptgConEvent);
	}
	else if (strType=="Timer")
	{
		//时间
		CNewSkillProperties::tagConTimer *ptgConTimer = OBJ_CREATE(CNewSkillProperties::tagConTimer);
		for (DWORD j=0; j<dwLength; ++j)
		{
			DWORD dwFlag = _GetDwordFromByteArray(pByte,pos);
			ptgConTimer->ldwConTimer.push_back(dwFlag);
		}
		ptgCondition->pVoidCondition = reinterpret_cast<void*>(ptgConTimer);
	}
    else if (strType=="Dispel")
    {
        CNewSkillProperties::tagConDispel* ptgConDispel = OBJ_CREATE(CNewSkillProperties::tagConDispel);
        for (DWORD j=0;j<dwLength;++j)
        {
            CNewSkillProperties::tagDispelCondition *ptgDispelCondition =  OBJ_CREATE( CNewSkillProperties::tagDispelCondition );
            ptgDispelCondition->Num = _GetShortFromByteArray(pByte,pos);
            _GetStringFromByteArray(pByte,pos,strTemp);
            ptgDispelCondition->strDispelNum = strTemp;
            _GetStringFromByteArray(pByte,pos,strTemp);
            ptgDispelCondition->strDispelType = strTemp;

            ptgConDispel->ltgDispelCondition.push_back(ptgDispelCondition);
        }
        ptgCondition->pVoidCondition = reinterpret_cast<void*>(ptgConDispel);
    }
	else if (strType=="Script")
	{
		//脚本
		CNewSkillProperties::tagConScript *ptgConScript = OBJ_CREATE(CNewSkillProperties::tagConScript);
		_GetStringFromByteArray(pByte,pos,strTemp);
		ptgConScript->strScript = strTemp;
		ptgCondition->pVoidCondition = reinterpret_cast<void*>(ptgConScript);
	}
	else if (strType=="Block")
	{
		//阻挡
		CNewSkillProperties::tagConBlock *ptgConBlock = OBJ_CREATE(CNewSkillProperties::tagConBlock);
		for(DWORD j=0; j<dwLength;++j)
		{
			CNewSkillProperties::tagBlockCondition *ptgBlockCondition = OBJ_CREATE(CNewSkillProperties::tagBlockCondition);
			_GetStringFromByteArray(pByte,pos,strTemp);
			ptgBlockCondition->strType = strTemp;
			_GetStringFromByteArray(pByte,pos,strTemp);
			ptgBlockCondition->strDir = strTemp;
			ptgBlockCondition->dwBlockType = _GetDwordFromByteArray(pByte,pos);
			ptgBlockCondition->dwShapeType = _GetDwordFromByteArray(pByte,pos);
			ptgConBlock->ltgBlockCondition.push_back(ptgBlockCondition);
		}
		ptgCondition->pVoidCondition = reinterpret_cast<void*>(ptgConBlock);
	}
	else if (strType=="Face")
	{
		//面向
		CNewSkillProperties::tagConFace *ptgConFace = OBJ_CREATE(CNewSkillProperties::tagConFace);
		for (DWORD j=0; j<dwLength; ++j)
		{
			CNewSkillProperties::tagFaceCondition *ptgFaceCondition = OBJ_CREATE(CNewSkillProperties::tagFaceCondition);
			_GetStringFromByteArray(pByte,pos,strTemp);
			ptgFaceCondition->strFaceType = strTemp;//_GetDwordFromByteArray(pByte,pos);
			ptgConFace->ltgFaceCondition.push_back(ptgFaceCondition);
		}
		ptgCondition->pVoidCondition = reinterpret_cast<void*>(ptgConFace);
	}
	else if(strType=="Region")
	{
		//场景
		CNewSkillProperties::tagConRegion *ptgConRegion = OBJ_CREATE(CNewSkillProperties::tagConRegion);
		for (DWORD i=0; i<dwLength; ++i)
		{
			CNewSkillProperties::tagRegionCondition *ptgRegionCondition = OBJ_CREATE(CNewSkillProperties::tagRegionCondition);
			_GetStringFromByteArray(pByte,pos,strTemp);
			ptgRegionCondition->strName = strTemp;
			ptgRegionCondition->dwType =  _GetDwordFromByteArray(pByte,pos);
			ptgConRegion->ltgRegionCondition.push_back(ptgRegionCondition);
		}
		ptgCondition->pVoidCondition = reinterpret_cast<void*>(ptgConRegion);
	}
}

/*
NAME:ClearSkillCache
DESC:清除技能缓冲
PARA:无
*/

void CNewSkillFactory::ClearSkillCache()
{
    g_skillCommon.m_mapFormat.clear();

    map<DWORD,CNewSkill::tagConditionGroup*>::iterator  GroupItr = g_skillCommon.m_mapConditionGroup.begin();
    for ( ; GroupItr != g_skillCommon.m_mapConditionGroup.end() ; GroupItr++ )
    {
        OBJ_RELEASE( CNewSkill::tagConditionGroup , GroupItr->second );
    }
    g_skillCommon.m_mapConditionGroup.clear();

    map<DWORD,CNewSkill::tagCondition*>::iterator  CondItr = g_skillCommon.m_mapCondition.begin();
    for ( ; CondItr != g_skillCommon.m_mapCondition.end() ; CondItr++ )
    {
        OBJ_RELEASE( CNewSkill::tagCondition,CondItr->second );
    }
    g_skillCommon.m_mapCondition.clear();

    map<DWORD,CNewSkill::tagOperateObject*>::iterator  ObjectItr = g_skillCommon.m_mapOperateObject.begin();
    for ( ; ObjectItr != g_skillCommon.m_mapOperateObject.end() ; ObjectItr++ )
    {
        OBJ_RELEASE( CNewSkill::tagOperateObject,ObjectItr->second );
    }
    g_skillCommon.m_mapOperateObject.clear();

	DWORD dwParamCount = 0;
	for( map<DWORD, CNewSkill*>::iterator it = g_mNewSkillProperties.begin();
		it != g_mNewSkillProperties.end(); it++ )
	{		
		OBJ_RELEASE(CNewSkill, it -> second );
	}
	g_mNewSkillProperties.clear();

	Log4c::Trace(ROOT_MODULE,"技能实例个数:%d",g_dwCount);
	for (int i=0; i<EMAXSKILLSTATE; ++i)
	{
		listModules &lBaseModule = g_BaseModules[i];

		for (itModule bit=lBaseModule.begin(); bit!=lBaseModule.end(); ++bit)
		{	
			if (g_dwCount>0)
			{
				--g_dwCount;	
			}	
			OBJ_RELEASE(CBaseModule, *bit);
		}
		g_BaseModules[i].clear();		
	}		

	

	itVariableList it = g_mapVariableList.begin();
	for (; it!=g_mapVariableList.end();++it)
	{
		OBJ_RELEASE(VariableList, (*it).second);
	}
	g_mapVariableList.clear();

	itModulerCounter mit = g_mapModuleCounter.begin();
	for (; mit!=g_mapModuleCounter.end(); ++ mit)
	{
		dwParamCount += (*mit).second;
		Log4c::Trace(ROOT_MODULE,"技能ID:%d,等级:%d,:次数:%d",(*mit).first.dwID,(*mit).first.dwLvl,(*mit).second);
	}
	
	Log4c::Trace(ROOT_MODULE,"技能总次数:%d",dwParamCount);

}




//初始化基本模块
void CNewSkillFactory::InitBaseModule(CBaseModule *pBaseModule,DWORD dwID,DWORD dwLevel,CNewSkill *pSkillProperties,stModuParam *pParam)
{

	CNewSkillProperties::tagSkill *ptgLevelSkill = pSkillProperties->GetNewSkill(dwLevel);
	if(ptgLevelSkill==NULL)	
		return;
	/*if(ptgLevelSkill->pStaticParam)
	{
		stStaticModuParam *pStaticParam = OBJ_CREATE(stStaticModuParam);
		pStaticParam->lCoolDownTime=ptgLevelSkill->pStaticParam->lCoolDownTime;
		pStaticParam->lConsMp=ptgLevelSkill->pStaticParam->lConsMp;
		pStaticParam->MinAtkDistance=ptgLevelSkill->pStaticParam->MinAtkDistance;
		pStaticParam->MaxAtkDistance=ptgLevelSkill->pStaticParam->MaxAtkDistance;		
		pStaticParam->lScriptParam=ptgLevelSkill->pStaticParam->lScriptParam;
		pStaticParam->lActTime = ptgLevelSkill->pStaticParam->lActTime;
		pBaseModule->SetStaticParam(pStaticParam);
	}
    */
	vector<CProcedure*> vProcedure;
	for (list<CNewSkillProperties::tagProcess*>::iterator it=ptgLevelSkill->ltgExecuteStep.begin();
		it!=ptgLevelSkill->ltgExecuteStep.end();++it)
	{ 
		CProcedure *pProcedure = OBJ_CREATE_PVOID(CProcedure, (void*)pParam);

		InitProcedure(dwID,dwLevel, pProcedure,*it);		
		vProcedure.push_back(pProcedure);
	}	
	pBaseModule->InitBaseData(vProcedure);

}

//初始化过程
void CNewSkillFactory::InitProcedure(DWORD dwID,DWORD dwLevel,CProcedure *pProcedure,CNewSkillProperties::tagProcess* ptgProcess)
{
	//处理步骤
	for (list<CNewSkillProperties::tagStep*>::iterator it=ptgProcess->ltgStep.begin();
		it!=ptgProcess->ltgStep.end();++it)
	{
		CStep *pStep = OBJ_CREATE(CStep);
		InitStep(dwID,dwLevel,pStep,*it);

		pProcedure->InitBaseData((*it)->strName,pStep);
	}
}

//初始化步骤
void CNewSkillFactory::InitStep(DWORD dwID,DWORD dwLevel,CStep *pStep,CNewSkillProperties::tagStep *ptgStep)
{
	//执行操作
	for (list<CNewSkillProperties::tagDo*>::iterator it=ptgStep->ltgDo.begin();
		it!=ptgStep->ltgDo.end();++it)
	{
		CDo	*pDo = OBJ_CREATE_PVOID(CDo, (void*)m_pParam);		
		InitDo(dwID,dwLevel, pDo,*it);
		pStep->InitBaseData(pDo);
	}	  
}

//属性条件
CConProperty *CNewSkillFactory::InitBasePropertyCondition(CNewSkillProperties::tagCondition *ptgConditon)
{
	CConProperty *pConProperty = OBJ_CREATE_PVOID(CConProperty, (void*)m_pParam);
	list<CConProperty::tagCheckes> ltgCheck;

	CNewSkillProperties::tagConProperty *pProperty = reinterpret_cast<CNewSkillProperties::tagConProperty*>(ptgConditon->pVoidCondition);
	for (list<CNewSkillProperties::tagPropertyCondition*>::iterator pit=pProperty->ltgProtertyConditon.begin();
		pit!=pProperty->ltgProtertyConditon.end();++pit)
	{
		CConProperty::tagCheckes tgChecks;
		string str;		
		tgChecks.Init(GetObj((*pit)->strObject),(*pit)->strName,&GetCmpFun((*pit)->cmpareOp),(*pit)->dwValue,(*pit)->paramvalue);
		ltgCheck.push_back(tgChecks);
	}
	pConProperty->InitBaseData(ltgCheck);
	return pConProperty;
}
//物品
CConGoods *CNewSkillFactory::InitGoodCondittion(CNewSkillProperties::tagCondition *ptgConditon)
{
	CConGoods *pConGood = OBJ_CREATE_PVOID(CConGoods, (void*)m_pParam);
	list<CConGoods::tagCheck> ltgCheck;
	CNewSkillProperties::tagConGood *pProperty = reinterpret_cast<CNewSkillProperties::tagConGood*>(ptgConditon->pVoidCondition);
	for (list<CNewSkillProperties::tagGoodCondition*>::iterator pit=pProperty->ltgGoodCondition.begin();
		pit!=pProperty->ltgGoodCondition.end();++pit)
	{
		CConGoods::tagCheck tgCheck;
		string str;

		list<CConGoods::tagAddProperty*> ltgAddProperty;
		for (list<CNewSkillProperties::tagGoodAddProperty*>::iterator ait=(*pit)->ltgGoodAddProperty.begin();ait!=(*pit)->ltgGoodAddProperty.end();++ait)
		{
			CConGoods::tagAddProperty *ptgAddProperty = OBJ_CREATE(CConGoods::tagAddProperty);
			ptgAddProperty->dwAddProType = (*ait)->dwAddProType;
			ptgAddProperty->dwValue1 = (*ait)->dwValue1;
			ptgAddProperty->dwValue2 = (*ait)->dwValue2;
			ltgAddProperty.push_back(ptgAddProperty);
		}
		tgCheck.Init(GetObj((*pit)->strObject),
			(*pit)->dwPlace,
			(*pit)->dwPos,
			(*pit)->dwType,
			(*pit)->strName,
			&GetCmpFun((*pit)->strCmpFun),
			(*pit)->dwValue,
			str,
			ltgAddProperty);	
		ltgCheck.push_back(tgCheck);
	}
	pConGood->InitBaseData(ltgCheck);
	return pConGood;
}
//状态
CConBuff *CNewSkillFactory::InitBufffCondition(CNewSkillProperties::tagCondition *ptgConditon)
{

	CConBuff *pConBuff = OBJ_CREATE_PVOID(CConBuff, (void*)m_pParam);
	list<CConBuff::tagCheck> ltgCheck;
	CNewSkillProperties::tagConBuff *pProperty = reinterpret_cast<CNewSkillProperties::tagConBuff*>(ptgConditon->pVoidCondition);

	for (list<CNewSkillProperties::tagBuffCondition*>::iterator pit=pProperty->ltgBuffCondition.begin();
		pit!=pProperty->ltgBuffCondition.end();++pit)
	{
		CConBuff::tagCheck tgCheck;
		eObj obj = GetObj((*pit)->strObject);
		tgCheck.Init(obj,
            (*pit)->strBufObject,
			(*pit)->dwID,
			(*pit)->dwLevel,
			(*pit)->dwNum,
            (*pit)->strLevel,
            GetCmpFun((*pit)->strOper,1));
		ltgCheck.push_back(tgCheck);
	}
	pConBuff->InitBaseData(ltgCheck);
	return pConBuff;
}

//距离
CConDistance *CNewSkillFactory::InitDistanceConditon(CNewSkillProperties::tagCondition *ptgConditon)
{

	CNewSkillProperties::tagConDistance *pProperty = reinterpret_cast<CNewSkillProperties::tagConDistance*>(ptgConditon->pVoidCondition);
	for (list<CNewSkillProperties::tagDistanceCondition*>::iterator pit=pProperty->ltgDistanceCondition.begin();
		pit!=pProperty->ltgDistanceCondition.end();++pit)
	{

		CConDistance *pConDistance = OBJ_CREATE_PVOID(CConDistance, (void*)m_pParam);
		string str;
		pConDistance->InitBaseData(GetCmpFun((*pit)->strCmpFun),(*pit)->value,(*pit)->strValueName,(*pit)->strRefObj);
		return pConDistance;
	}
	return NULL;
}
//有效目标
CConValidObj *CNewSkillFactory::InitValidObjectCondition(CNewSkillProperties::tagCondition *ptgConditon)
{
	CNewSkillProperties::tagConValidObject *pProperty = reinterpret_cast<CNewSkillProperties::tagConValidObject*>(ptgConditon->pVoidCondition);
	CConValidObj *pConValidObj = OBJ_CREATE_PVOID(CConValidObj, (void*)m_pParam);
	for (list<CNewSkillProperties::tagValidObjectCondition*>::iterator it=pProperty->ltgValidObjectCondition.begin();
		it!=pProperty->ltgValidObjectCondition.end();++it)
	{			
		pConValidObj->InitBaseData(GetEType((*it)->strType), GetValidType((*it)->lObjType));
		return pConValidObj;		
	} 
	return  NULL;

}
//脚本
CConScript *CNewSkillFactory::InitScriptCondition(CNewSkillProperties::tagCondition *ptgConditon)
{
	CNewSkillProperties::tagConScript *pProperty = reinterpret_cast<CNewSkillProperties::tagConScript*>(ptgConditon->pVoidCondition);
	if (pProperty)
	{
		CConScript *pConScript = OBJ_CREATE_PVOID(CConScript, (void*)m_pParam);
		pConScript->InitBaseData(pProperty->strScript);
		return pConScript;
	}
	return NULL;
}
//冷却
CConCooldown *CNewSkillFactory::InitCoolDownCondition(CNewSkillProperties::tagCondition *ptgConditon)
{
	return NULL;
}
//时间
CConTimer *CNewSkillFactory::InitTimerCondtion(CNewSkillProperties::tagCondition *ptgConditon)
{
	CNewSkillProperties::tagConTimer *pProperty = reinterpret_cast<CNewSkillProperties::tagConTimer*>(ptgConditon->pVoidCondition);
	for (list<DWORD>::iterator it=pProperty->ldwConTimer.begin();
		it!=pProperty->ldwConTimer.end();++it)
	{		
		CConTimer *pConTimer = OBJ_CREATE_PVOID(CConTimer, (void*)m_pParam);
		pConTimer->InitBaseData(*it);
		return pConTimer;
	}
	return NULL;
}
//事件
CConEvent *CNewSkillFactory::InitEventCondition(CNewSkillProperties::tagCondition *ptgConditon)
{
	CNewSkillProperties::tagConEvent *pProperty = reinterpret_cast<CNewSkillProperties::tagConEvent*>(ptgConditon->pVoidCondition);

	for (list<CNewSkillProperties::tagEventCondition*>::iterator it=pProperty->ltgEventCondition.begin();
		it!=pProperty->ltgEventCondition.end();++it)
	{
		CConEvent *pConEvent = OBJ_CREATE_PVOID(CConEvent, (void*)m_pParam);
		pConEvent->InitBaseData((*it)->dwFlag,GetEventType(m_pParam->nID,
            (*it)->strEventName),(*it)->dwVar1,(*it)->dwVar2,(*it)->dwVar3,
            (*it)->dwVar4,(*it)->dwSkillId,(*it)->strName);
		return pConEvent;
	}
	return NULL;

}
//阻挡
CConBlock  *CNewSkillFactory::InitBlockCondition(CNewSkillProperties::tagCondition *ptgConditon)
{
	CNewSkillProperties::tagConBlock *pBlock = reinterpret_cast<CNewSkillProperties::tagConBlock*>(ptgConditon->pVoidCondition);
	list<CNewSkillProperties::tagBlockCondition*>::iterator it=pBlock->ltgBlockCondition.begin();
	for (;it!=pBlock->ltgBlockCondition.end();++it)
	{
		CConBlock *pConBlock = OBJ_CREATE_PVOID(CConBlock, (void*)m_pParam);
		pConBlock->InitBaseData((*it)->dwBlockType,(*it)->dwShapeType,(*it)->strType,(*it)->strDir);
		return pConBlock;
	}
	return NULL;
}
//面向
CConDir *CNewSkillFactory::InitDirCondition(CNewSkillProperties::tagCondition *ptgConditon)
{
	CNewSkillProperties::tagConFace *pFace = reinterpret_cast<CNewSkillProperties::tagConFace*>(ptgConditon->pVoidCondition);
	list<CNewSkillProperties::tagFaceCondition*>::iterator it=pFace->ltgFaceCondition.begin();
	for (;it!=pFace->ltgFaceCondition.end();++it)
	{
		CConDir *pConDir = OBJ_CREATE_PVOID(CConDir, (void*)m_pParam);
		pConDir->InitBaseData((*it)->strFaceType);
		return pConDir;
	}
	return NULL;
}
//场景
CConRegion *CNewSkillFactory::InitRegionCondition(CNewSkillProperties::tagCondition *ptgCondition)
{
	CNewSkillProperties::tagConRegion *pRegion = reinterpret_cast<CNewSkillProperties::tagConRegion*>(ptgCondition->pVoidCondition);
	list<CNewSkillProperties::tagRegionCondition*>::iterator it=pRegion->ltgRegionCondition.begin();
	for (;it!=pRegion->ltgRegionCondition.end();++it)
	{
		CConRegion *pConRegion = OBJ_CREATE_PVOID(CConRegion, (void*)m_pParam);
		pConRegion->InitBaseData((*it)->strName,(*it)->dwType);
		return pConRegion;
	}
	return NULL;
}
//几率
CConOdds *CNewSkillFactory::InitOddsCondition(CNewSkillProperties::tagCondition *ptgCondition)
{
    CNewSkillProperties::tagConOdds *pOdds = reinterpret_cast<CNewSkillProperties::tagConOdds*>(ptgCondition->pVoidCondition);
    list<CNewSkillProperties::tagOddsCondition*>::iterator it=pOdds->ltgOddsCondition.begin();
    for (;it!=pOdds->ltgOddsCondition.end();++it)
    {
        CConOdds *pConOdds = OBJ_CREATE_PVOID(CConOdds, (void*)m_pParam);
        pConOdds->InitBaseData((*it)->value,(*it)->strValueName,GetCmpFun((*it)->strCmpFun));
        return pConOdds;
    }
    return NULL;
}
//驱散
CConDispel *CNewSkillFactory::InitDispelCondition(CNewSkillProperties::tagCondition *ptgCondition)
{
    CNewSkillProperties::tagConDispel *pDispel = reinterpret_cast<CNewSkillProperties::tagConDispel*>(ptgCondition->pVoidCondition);
    list<CNewSkillProperties::tagDispelCondition*>::iterator it=pDispel->ltgDispelCondition.begin();
    for (;it!=pDispel->ltgDispelCondition.end();++it)
    {
        CConDispel *pConDispel = OBJ_CREATE_PVOID(CConDispel, (void*)m_pParam);
        pConDispel->InitBaseData((*it)->Num,GetSkillAttType((*it)->strDispelType),(*it)->strDispelType);
        return pConDispel;
    }
    return NULL;
}

/*
NAME:InitConcreateCondition
DESC:初始化具体条件
PARA:ptgCondition条件
*/
CCondition *CNewSkillFactory::InitConcreateCondition(CNewSkillProperties::tagCondition *ptgConditon)
{
	if(ptgConditon->strType=="BaseProperty")
	{
		//属性条件			
		return InitBasePropertyCondition(ptgConditon);
	}
	else if (ptgConditon->strType=="Good")
	{
		//物品
		return InitGoodCondittion(ptgConditon);

	}
	else if (ptgConditon->strType=="Buff")
	{
		//状态
		return InitBufffCondition(ptgConditon);
	}		
	else if(ptgConditon->strType=="Distance")
	{
		//距离
		return InitDistanceConditon(ptgConditon);
	}
	else if(ptgConditon->strType=="ValidObject")
	{
		//有效对象
		return InitValidObjectCondition(ptgConditon);
	} 
	else if(ptgConditon->strType=="Script")
	{
		//脚本
		return InitScriptCondition(ptgConditon);
	}
    else if (ptgConditon->strType=="Odds")
    {
        //几率
        return InitOddsCondition(ptgConditon);
    }
    else if (ptgConditon->strType=="Dispel")
    {
        //驱散
        return InitDispelCondition(ptgConditon);
    }
	else if(ptgConditon->strType=="CoolDown")
	{
		//时间
		return InitCoolDownCondition(ptgConditon);
	} 
	else if(ptgConditon->strType=="Timer")
	{
		//定时器
		return InitTimerCondtion(ptgConditon);
	}
	else if (ptgConditon->strType=="Event")
	{
		//事件
		return InitEventCondition(ptgConditon);
	}
	else if (ptgConditon->strType=="Block")
	{
		//阻挡
		return InitBlockCondition(ptgConditon);
	}
	else if (ptgConditon->strType=="Face")
	{
		//面向
		return InitDirCondition(ptgConditon);
	}
	else if (ptgConditon->strType=="Region")
	{
		return InitRegionCondition(ptgConditon);
	}
	/*else if (ptgConditon->strType=="Occupation")
	{
		return InitOccupationCondition(ptgConditon);
	}*/
	return NULL;
}
//初始2化条件
CCondition *CNewSkillFactory::InitCondition(DWORD dwID,DWORD dwLv, DWORD dwConditionID,bool vRet)
{    
    if( !vRet )
    {
       map<DWORD, CNewSkillProperties::tagCondition*>::iterator it  = g_skillCommon.m_mapCondition.find( dwConditionID );
       if ( it != g_skillCommon.m_mapCondition.end() )
       {
           return InitConcreateCondition(it->second);
       }
       return NULL;
    }

    map<DWORD, CNewSkillProperties::tagCondition*> mpInCondition = QueryNewSkillProperty(dwID)->GetNewSkill(dwLv)->mapCondition;

    map<DWORD, CNewSkillProperties::tagCondition*>::iterator it = mpInCondition.find(dwConditionID);
    if (it == mpInCondition.end())
    {
        it = g_skillCommon.m_mapCondition.find( dwConditionID );

        if ( it == g_skillCommon.m_mapCondition.end() )
        {
            map<DWORD, CNewSkillProperties::tagCondition*> mpOutCondition = QueryNewSkillProperty(dwID)->GetCondition();	
            map<DWORD, CNewSkillProperties::tagCondition*>::iterator Oit= mpOutCondition.find(dwConditionID);
            if (Oit!=mpOutCondition.end())
            {
                return InitConcreateCondition(Oit->second);
            }
           Log4c::Trace(ROOT_MODULE,FormatText("STR_WS_SKILL_NINETH",dwID,dwConditionID));
            return NULL;
        }
    }
    return InitConcreateCondition(it->second); 
}

//初始化条件组
CCondiGroup *CNewSkillFactory::InitConditionGroup(DWORD dwID,DWORD dwLv,DWORD dwGroupID)
{
    if (0!=dwGroupID)
    {	
        map<DWORD,CNewSkillProperties::tagConditionGroup*> mpConditionGroup = QueryNewSkillProperty(dwID)->GetConditionGroup();

        map<DWORD,CNewSkillProperties::tagConditionGroup*>::iterator it = mpConditionGroup.find(dwGroupID);

        bool flag = true;
        if ( it == mpConditionGroup.end() )
        {
            it = g_skillCommon.m_mapConditionGroup.find( dwGroupID );
            if ( it == g_skillCommon.m_mapConditionGroup.end() )
            {
                return NULL;
            }
            flag = false;
        }
        CCondiGroup *pCondiGroup = OBJ_CREATE(CCondiGroup);

        vector<CCondition*> vCondition;
        for (list<DWORD>::iterator cit= (it->second)->lConditonGroup.begin();
            cit!=(it->second)->lConditonGroup.end();++cit)
        {
            DWORD dwConID = (DWORD)(*cit);
            CCondition *pCondition = InitCondition(dwID,dwLv,dwConID,flag);
            if (NULL==pCondition)
            {
                continue;
            }
            vCondition.push_back(pCondition);				
        }
        CCondiGroup::eGrouMode eGoupMode = GetGroupMode(it->second->strMode);
        pCondiGroup->InitBaseData(vCondition,eGoupMode);
        return pCondiGroup;
    }		
    else
    {
        CCondiGroup *pCondiGroup = OBJ_CREATE(CCondiGroup);

        vector<CCondition*> vCondition;		
        CCondiGroup::eGrouMode eGoupMode = GetGroupMode("");
        pCondiGroup->InitBaseData(vCondition,eGoupMode);
        return pCondiGroup;
    }
    return NULL;
}

//初始2化影响对象
CEffectObjects *CNewSkillFactory::InitEffectObject(DWORD dwID,DWORD dwLv,DWORD dwOperobjID)
{
	map<DWORD , CNewSkillProperties::tagOperateObject*> mpOperateObject = QueryNewSkillProperty(dwID)->GetOperateObject();
	map<DWORD , CNewSkillProperties::tagOperateObject*>::iterator it = mpOperateObject.find(dwOperobjID);
	if(it == mpOperateObject.end() ) 
	{
		/// 从全局的对象查找
        it = g_skillCommon.m_mapOperateObject.find( dwOperobjID );
        if ( it == g_skillCommon.m_mapOperateObject.end() )
        {
            return NULL;
        }
	}

    {
        CCondiGroup* pCondiGroup = InitConditionGroup(dwID,dwLv,it->second->dwConGroupID);
        if (it->second->strType=="point")
        {
            //点
            CEffPoint::tagTmpParam TmpParam;
            TmpParam.pParam = m_pParam;
            TmpParam.pCondiGroup = pCondiGroup;
            CEffPoint *pEffPoint = OBJ_CREATE_PVOID(CEffPoint, &TmpParam);
            pEffPoint->InitBaseData(it->second->strPosition);
            return pEffPoint;
        }
        else if (it->second->strType=="line")
        {
            //线
            CEffLine::tagTmpParam param;
            param.pCondiGroup = pCondiGroup;
            param.pParam = m_pParam;
            CEffLine *pEffLine = OBJ_CREATE_PVOID(CEffLine, (void*)&param);
            pEffLine->InitBaseData(it->second->strPosition,(int)it->second->distance,it->second->dwNumObject,
                it->second->IsWithIn=='1',it->second->NumObjectName);
            return pEffLine;
        } 
        else if(it->second->strType=="linefirst")
        {
            //线上第一个
            CEffLineFirst::tagTmpParam param;
            param.pCondiGroup = pCondiGroup;
            param.pParam = m_pParam;
            CEffLineFirst *pEffLineFirst = OBJ_CREATE_PVOID(CEffLineFirst, (void*)&param);
            pEffLineFirst->InitBaseData(it->second->strPosition,(int)it->second->distance);
            return pEffLineFirst;
        }
        else if (it->second->strType=="circ")
        {
            //圆
            CEffCirc::tagTmpParam param;
            param.pCondiGroup = pCondiGroup;
            param.pParam = m_pParam;
            CEffCirc *pEffCirc = OBJ_CREATE_PVOID(CEffCirc, (void*)&param);
            pEffCirc->InitBaseData(it->second->strCenterObject,it->second->radius,it->second->dwNumObject,
                it->second->IsWithIn=='1',it->second->NumObjectName);
            return pEffCirc;
        } 
        else if (it->second->strType=="rect")
        {
            //矩形（带宽度的直线）
            EffRect::tagTmpParam param;
            param.pCondiGroup = pCondiGroup;
            param.pParam = m_pParam;
            EffRect *effRect = OBJ_CREATE_PVOID(EffRect, (void*)&param);
            effRect->InitBaseData(it->second->strPosition,it->second->distance,it->second->width/2.0f,
                it->second->dwNumObject,it->second->IsWithIn=='1',it->second->NumObjectName);
            return effRect;
        } 
        else if (it->second->strType=="sector")
        {
            //扇形
            EffSector::tagTmpParam param;
            param.pCondiGroup = pCondiGroup;
            param.pParam = m_pParam;
            EffSector *effSector = OBJ_CREATE_PVOID(EffSector, (void*)&param);
            effSector->InitBaseData(it->second->strPosition,it->second->distance,it->second->angle,
                it->second->dwNumObject,it->second->IsWithIn=='1',it->second->NumObjectName);
            return effSector;
        } 
        else if(it->second->strType=="team")
        {	//队伍
            CEffTeam::tagTmpParam param;
            param.pCondiGroup = pCondiGroup;
            param.pParam = m_pParam;
            CEffTeam *pEffTeam = OBJ_CREATE_PVOID(CEffTeam, (void*)&param);
            pEffTeam->InitBaseData(it->second->strCenterObject,(int)it->second->distance,
                it->second->dwNumObject,it->second->NumObjectName);
            return pEffTeam;
        }	
        else if (it->second->strType=="eobject")
        {
            //已存在作用对象
            CEffAppObject::tagTmpParam param;
            param.pCondiGroup = pCondiGroup;
            param.pParam = m_pParam;
            CEffAppObject *pEffAppObject = OBJ_CREATE_PVOID(CEffAppObject, (void*)&param);
            return pEffAppObject;			
        }
        else if (it->second->strType=="eventobject")
        {
            //根据事件参数设置对象
            CEffEventByParam::tagTmpParam param;
            param.pCondiGroup = pCondiGroup;
            param.pParam = m_pParam;
            CEffEventByParam *pEffEventObj = OBJ_CREATE_PVOID(CEffEventByParam, (void*)&param);
            return pEffEventObj;
        }
    }
    return NULL;
}


//初始化操作
void CNewSkillFactory::InitDo(DWORD dwID,DWORD dwLevel, CDo *pDo,CNewSkillProperties::tagDo *ptgDo)
{
	DWORD dwOperobjid = ptgDo->dwOperobjid;	
	CEffectObjects *pEffObjects = InitEffectObject(dwID,dwLevel, dwOperobjid); 

	vector<CDo::tagGroup*> vtgGroup;

	for (list<CNewSkillProperties::tagCDo*>::iterator it=ptgDo->ltgCDo.begin();
		it!=ptgDo->ltgCDo.end();++it)
	{	
		CDo::tagGroup *tgGroup = OBJ_CREATE(CDo::tagGroup);
		tgGroup->pTrueGroup = NULL;
		tgGroup->pFalseGroup = NULL;

		DWORD dwConditionGroup = (*it)->tgCondition.dwConditionGroup;
		tgGroup->pCondiGroup = InitConditionGroup(dwID,dwLevel,dwConditionGroup);
		eModuleRet tret = GetModuleRet((*it)->tgCondition.strTrueRet);
		eModuleRet fret = GetModuleRet((*it)->tgCondition.strFalseRet);

		tgGroup->Init(tret,fret);	
		if((*it)->ltgTrueDoProcess.size()>0)
		{
			for (list<CNewSkillProperties::tagDoProcess*>::iterator Tit=(*it)->ltgTrueDoProcess.begin();
				Tit!=(*it)->ltgTrueDoProcess.end();++Tit)
			{
				InitOperator(dwID,*Tit,tgGroup->pTrueOperators);	
				//tgGroup->pTrueGroup = NULL;
				if ((*Tit)->ltgCDo.size()>0)
				{
					tgGroup->pTrueGroup = OBJ_CREATE(CDo::tagGroup);
					InitGroup(dwID,dwLevel,*Tit,tgGroup->pTrueGroup);
				}				
			}
		}

		if((*it)->ltgFalseDoProcess.size()>0)
		{
			for (list<CNewSkillProperties::tagDoProcess*>::iterator Fit=(*it)->ltgFalseDoProcess.begin();
				Fit!=(*it)->ltgFalseDoProcess.end();++Fit)
			{
				InitOperator(dwID, *Fit,tgGroup->pFalseOperators);	
				//tgGroup->pFalseGroup = NULL;
				if ((*Fit)->ltgCDo.size()>0)
				{
					tgGroup->pFalseGroup = OBJ_CREATE(CDo::tagGroup);
					InitGroup(dwID,dwLevel,*Fit,tgGroup->pFalseGroup);
				}
			}	
		}

		vtgGroup.push_back(tgGroup);
	}

	pDo->InitBaseData(pEffObjects,vtgGroup,GetModuleRet(ptgDo->strRet));
}

//属性
void CNewSkillFactory::InitChangeProperty(CNewSkillProperties::tagChangeProperty *ptgChangePropety, vector<COperator*> &vOperator)
{	
	COpProperty *pOpProperty = OBJ_CREATE_PVOID(COpProperty, (void*)m_pParam);
	OPFUN *Op = GetOpFun(ptgChangePropety->strOperator,m_pParam->nID);	
	pOpProperty->InitBaseData(GetObj(ptgChangePropety->strObj),Op,ptgChangePropety->strName,
            ptgChangePropety->dwValue,ptgChangePropety->strScriptVarName,ptgChangePropety->paramname);
	vOperator.push_back(pOpProperty);
}
//注册事件
void CNewSkillFactory::InitEventChange(DWORD dwID,CNewSkillProperties::tagRegisterEvent *ptgEvent,vector<COperator*> &vOperator)
{	
	COpRegisterEvent *pOperator = OBJ_CREATE_PVOID(COpRegisterEvent, (void*)m_pParam);
	eEventType etype =GetEventType(m_pParam->nID, ptgEvent->strName);
	eChildObjType type = GetChildObjType(QueryNewSkillProperty(dwID)->GetSkillType());
	pOperator->InitDataBase(GetObj(ptgEvent->strObj),type,ptgEvent->dwFlag,etype);
	vOperator.push_back(pOperator);
}
//时间
void CNewSkillFactory::InitTimerChange(DWORD dwID, CNewSkillProperties::tagRegisterTimer *ptgTimer,vector<COperator*> &vOperator)
{	
	COpRegisterTimer *pOperator = OBJ_CREATE_PVOID(COpRegisterTimer, (void*)m_pParam);
	eChildObjType type = GetChildObjType(QueryNewSkillProperty(dwID)->GetSkillType());
	pOperator->InitBaseData(GetObj(ptgTimer->strObj),type,ptgTimer->dwFlag,ptgTimer->dwFuture,
        ptgTimer->strSFutvarname,ptgTimer->dwIntelTime,ptgTimer->strSIntelName,
        ptgTimer->dwCount,ptgTimer->strSCountName,ptgTimer->strVFuturename,
        ptgTimer->strVIntername,ptgTimer->strVCountname);
	vOperator.push_back(pOperator);	
}
//发送消息
void CNewSkillFactory::InitSendMsg(CNewSkillProperties::tagSendMsg *ptgSendMsg ,vector<COperator*> &vOperator)
{	
	COpSendMsg *pOperator = OBJ_CREATE_PVOID(COpSendMsg, (void*)m_pParam);	
	ModuleType eModuleType = QueryNewSkillProperty(m_pParam->nID)->GetSkillType();
	pOperator->InitBaseData(GetObj(ptgSendMsg->strObj),eModuleType,GetMsgType( m_pParam->nID, ptgSendMsg->strType), GetRange(ptgSendMsg->strRange),ptgSendMsg->dwValue,ptgSendMsg->strSVName,ptgSendMsg->nAdd);
	vOperator.push_back(pOperator);		
}
//脚本
void CNewSkillFactory::InitChangeScriptID(DWORD dwID,CNewSkillProperties::tagScriptID *ptgScriptID,vector<COperator*> &vOperator)
{	
	COpScript *pOperator = OBJ_CREATE_PVOID(COpScript, (void*)m_pParam);	
	map<DWORD,CNewSkillProperties::tagExpression> mapScript = QueryNewSkillProperty(dwID)->GetFormat();

	vector<CExpressions*> vExpression;	
	
	map<DWORD,CNewSkillProperties::tagExpression>::iterator cit=mapScript.find(ptgScriptID->dwScriptID);
	if (cit!=mapScript.end())
	{	
		CExpressions *pExpression = OBJ_CREATE(CExpressions);
		pExpression->InitBaseData(cit->second.strFileName,cit->second.strContent);
		vExpression.push_back(pExpression);
	}
    else 
    {
        cit = g_skillCommon.m_mapFormat.find( ptgScriptID->dwScriptID );
        if ( cit != g_skillCommon.m_mapFormat.end() )
        {
            CExpressions *pExpression = OBJ_CREATE(CExpressions);
            pExpression->InitBaseData(cit->second.strFileName,cit->second.strContent);
            vExpression.push_back(pExpression); 
        }
    }

	pOperator->InitBaseData(GetObj(ptgScriptID->strObj),vExpression);	
	vOperator.push_back(pOperator);
}
//攻击对象
void CNewSkillFactory::InitAttackChangeObject(CNewSkillProperties::tagAttackChangeObject *ptgAttackChangeObject,vector<COperator*> &vOperator)
{

}
//目标对象
void CNewSkillFactory::InitDestObjChange(CNewSkillProperties::tagDestObjChange *ptgDest, vector<COperator*> &vOperator)
{	
	COpDestObjs *pOperator = OBJ_CREATE_PVOID(COpDestObjs, (void*)m_pParam);
	pOperator->InitBaseData(GetObj(ptgDest->strObj),ptgDest->strDestObjChange);
	vOperator.push_back(pOperator);	
}
//状态
void CNewSkillFactory::InitBuffChange(CNewSkillProperties::tagChangeBuff *ptgBuff, vector<COperator*> &vOperator)
{	
	COpBuff *pOperator = OBJ_CREATE_PVOID(COpBuff, (void*)m_pParam);	
	eModuleOper op = GetOperator(m_pParam->nID, ptgBuff->strOperator);
    eSkillAttackType type = GetSkillAttType( ptgBuff->strSkillType);
	pOperator->InitBaseData(GetObj(ptgBuff->strObj),op,type,ptgBuff->dwID,ptgBuff->dwLevel,
        ptgBuff->dwCount,ptgBuff->strSIDName,ptgBuff->strSLvName,ptgBuff->strSCountName);
	vOperator.push_back(pOperator);			
}
//位置
void CNewSkillFactory::InitChangePos(CNewSkillProperties::tagChangePos *ptgPos, vector<COperator*> &vOperator)
{	
	//COpPosition *pOperator = OBJ_CREATE_PVOID(COpPosition, (void*)m_pParam);		
	//eObj obj = GetObj(ptgPos->strObj);
	//eObj robj = GetObj(ptgPos->strRefObject);
	//pOperator->InitBaseData(GetObj(ptgPos->strObj),obj,robj,ptgPos->strDir,ptgPos->strStyle,ptgPos->strSpeed);				
	//vOperator.push_back(pOperator);			
    OpPositionEx *op = OBJ_CREATE_PVOID( OpPositionEx, (void*) m_pParam );
    op->InitBaseData( GetObj( ptgPos->strObj ), GetObj( ptgPos->strRefObject ),
        ptgPos->strDir, ptgPos->strStyle, ptgPos->strSpeed, ptgPos->dis );
    vOperator.push_back( op );
}
//冷却时间
void CNewSkillFactory::InitCooldownChange(CNewSkillProperties::tagCoolDownChange *ptgCool, vector<COperator*> &vOperator)
{	
	COpTimeChange *pOperator = OBJ_CREATE_PVOID(COpTimeChange, (void*)m_pParam);
	pOperator->InitBaseData(GetObj(ptgCool->strObj),ptgCool->strType,ptgCool->dwValue,ptgCool->strSVarianame,ptgCool->paramname);
	vOperator.push_back(pOperator);	
}
//伤害
void CNewSkillFactory::InitDamageChange(CNewSkillProperties::tagDamageChange *ptgDamage, vector<COperator*> &vOperator)
{	
	COpDamage *pOperator = OBJ_CREATE_PVOID(COpDamage, (void*)m_pParam);
	eObj obj = GetObj(ptgDamage->strObj);
	eModuleOper oper = GetOperator(m_pParam->nID,ptgDamage->strOperator);
	tagDamageType type = GetDamageType(m_pParam->nID, ptgDamage->strType);
	pOperator->InitBaseData(obj,ptgDamage->strOperator,type,ptgDamage->dwValue,
        ptgDamage->strSVarianame,ptgDamage->strDamagename);
	vOperator.push_back(pOperator);

}
//DO伤害
void CNewSkillFactory::InitDoDamage(CNewSkillProperties::tagDoDamage *ptgDoDamage, vector<COperator*> &vOperator)
{

	COpDoDamage *pOperator = OBJ_CREATE_PVOID(COpDoDamage, (void*)m_pParam);
	pOperator->InitBaseData(GetObj(ptgDoDamage->strObj),ptgDoDamage->dwResult,ptgDoDamage->strSResVariName,
             ptgDoDamage->dwBlastAttack,ptgDoDamage->strSBlastVariName,ptgDoDamage->strDamagename,ptgDoDamage->strBlastname);
	vOperator.push_back(pOperator);

}
//物品
void CNewSkillFactory::InitGoodChange(CNewSkillProperties::tagGoodChange *ptgGoodChange,vector<COperator*> &vOperator)
{

	COpGood *pOperator = OBJ_CREATE_PVOID(COpGood, (void*)m_pParam);
	eModuleOper op = GetOperator(m_pParam->nID,ptgGoodChange->strOperator);
	pOperator->InitBaseData(GetObj(ptgGoodChange->strObj),op,ptgGoodChange->strName,ptgGoodChange->dwValue,ptgGoodChange->dwPlace,ptgGoodChange->dwPos,ptgGoodChange->dwGoodsType,ptgGoodChange->dwWeaponType);
	vOperator.push_back(pOperator);

}
//注消时间
void CNewSkillFactory::InitUnRegisterTimer(CNewSkillProperties::tagUnRegisterTimer *ptgUnRegisterTimer, vector<COperator*> &vOperator)								
{	
	COpUnRegisterTimer::tagParam param;
	param.pParam = m_pParam;
	param.lFlag = ptgUnRegisterTimer->dwUnRegisterTimer;
	COpUnRegisterTimer *pOperator = OBJ_CREATE_PVOID(COpUnRegisterTimer, (void*)&param);
	pOperator->InitDataBase(GetObj(ptgUnRegisterTimer->strObj));
	vOperator.push_back(pOperator);

}
//注消事件
void CNewSkillFactory::InitUnRegisterEvent(CNewSkillProperties::tagUnRegisterEvent *ptgUnRegisterEvent, vector<COperator*> &vOperator)									
{
	COpUnRegisterEvent::tagParam param;
	param.pParam = m_pParam;
	param.lFlag = ptgUnRegisterEvent->dwUnRegisterEvent;
	COpUnRegisterEvent *pOperator = OBJ_CREATE_PVOID(COpUnRegisterEvent, (void*)&param);
	pOperator->InitDataBase(GetObj(ptgUnRegisterEvent->strObj));
	vOperator.push_back(pOperator);

}
//技能操作
void CNewSkillFactory::InitSkillChange(CNewSkillProperties::tagSkillChange  *ptgSkillChange,vector<COperator*> &vOperator)
{
   COpSkill *pOperator = OBJ_CREATE_PVOID(COpSkill, (void*)m_pParam);
   pOperator->InitBaseData(GetObj(ptgSkillChange->strObj),GetModuleType(ptgSkillChange->strType),ptgSkillChange->lID,ptgSkillChange->lLv,ptgSkillChange->strOpera);
   vOperator.push_back(pOperator);
}
//天赋
void CNewSkillFactory::InitGeniusChange(CNewSkillProperties::tagGeniusChange*ptgGeniusChange,vector<COperator*>&vOperator)
{
    COpGenius* pOperator = OBJ_CREATE_PVOID(COpGenius, (void*)m_pParam);
    pOperator->InitBaseData(ptgGeniusChange->strGenius);
    vOperator.push_back( pOperator );
}

//方向
void  CNewSkillFactory::InitDirection(CNewSkillProperties::tagDirection *ptgDirection,vector<COperator*> &vOperator)									
{
	COpDirec *POperator = OBJ_CREATE_PVOID(COpDirec, (void*)m_pParam);
	POperator->InitBaseData(GetObj(ptgDirection->strObj),GetDirec(ptgDirection->strDir));
	vOperator.push_back(POperator);
}


void	CNewSkillFactory::InitGroup(DWORD dwID,DWORD dwLevel,CNewSkillProperties::tagDoProcess *ptgDoProcess,CDo::tagGroup *pGroup)
{
	
	list<CNewSkillProperties::tagCDo*>::iterator it=ptgDoProcess->ltgCDo.begin();
	for (;it!=ptgDoProcess->ltgCDo.end();++it)
	{
		DWORD dwConditionGroup = (*it)->tgCondition.dwConditionGroup;	
		pGroup->pCondiGroup = InitConditionGroup(dwID,dwLevel,dwConditionGroup);
		eModuleRet tret = GetModuleRet((*it)->tgCondition.strTrueRet);
		eModuleRet fret = GetModuleRet((*it)->tgCondition.strFalseRet);
		pGroup->pTrueGroup = NULL;
		pGroup->pFalseGroup = NULL;

		pGroup->Init(tret,fret);	
		if((*it)->ltgTrueDoProcess.size()>0)
		{
			for (list<CNewSkillProperties::tagDoProcess*>::iterator Tit=(*it)->ltgTrueDoProcess.begin();
				Tit!=(*it)->ltgTrueDoProcess.end();++Tit)
			{
				InitOperator(dwID,*Tit,pGroup->pTrueOperators);
				

				if ((*Tit)->ltgCDo.size()>0)
				{
					pGroup->pTrueGroup = OBJ_CREATE(CDo::tagGroup);
					InitGroup(dwID,dwLevel, *Tit,pGroup->pTrueGroup);
				}
			}
		}

		if((*it)->ltgFalseDoProcess.size()>0)
		{
			for (list<CNewSkillProperties::tagDoProcess*>::iterator Fit=(*it)->ltgFalseDoProcess.begin();
				Fit!=(*it)->ltgFalseDoProcess.end();++Fit)
			{
				InitOperator(dwID, *Fit,pGroup->pFalseOperators);	
				
				if ((*Fit)->ltgCDo.size()>0)
				{
					pGroup->pFalseGroup = OBJ_CREATE(CDo::tagGroup);
					InitGroup(dwID,dwLevel,*Fit,pGroup->pFalseGroup);
				}
			}	
		}
		
		break;
	}
}

//初始化操作符

void CNewSkillFactory::InitOperator(DWORD dwID,CNewSkillProperties::tagDoProcess *ptgDoProcess, vector<COperator*> &vOperator)
{
	list<CNewSkillProperties::tagBaseChange*>::iterator it = ptgDoProcess->ltgBaseChange.begin();
	for (;it!=ptgDoProcess->ltgBaseChange.end();++it)
	{
		if ((*it)->strChangeType=="Script")
		{
			CNewSkillProperties::tagScriptID *ptgScriptID = dynamic_cast<CNewSkillProperties::tagScriptID*>(*it);
			InitChangeScriptID(dwID, ptgScriptID,vOperator);
		}
		else if ((*it)->strChangeType=="ObjectPropertyChange")
		{
			CNewSkillProperties::tagChangeProperty *ptgChangePropety = dynamic_cast<CNewSkillProperties::tagChangeProperty*>(*it);
			InitChangeProperty(ptgChangePropety,vOperator);			
		}	
		else if ((*it)->strChangeType=="BuffChange")
		{
			CNewSkillProperties::tagChangeBuff *ptgBuff = dynamic_cast<CNewSkillProperties::tagChangeBuff*>(*it);
			InitBuffChange(ptgBuff,vOperator);
		}
		else if ((*it)->strChangeType=="PosChange")
		{
			//位置5InitChangePos
			CNewSkillProperties::tagChangePos *ptgPos = dynamic_cast<CNewSkillProperties::tagChangePos*>(*it);
			InitChangePos(ptgPos,vOperator);
		}
		else if((*it)->strChangeType=="AttackChangeObject")
		{
			CNewSkillProperties::tagAttackChangeObject *ptgAttackChangeObject = dynamic_cast<CNewSkillProperties::tagAttackChangeObject*>(*it);
			InitAttackChangeObject(ptgAttackChangeObject,vOperator);
		}
		else if ((*it)->strChangeType=="CooldownChange")
		{
			//时间变化InitCooldownChange
			CNewSkillProperties::tagCoolDownChange *ptgCool = dynamic_cast<CNewSkillProperties::tagCoolDownChange*>(*it);
			InitCooldownChange(ptgCool,vOperator);
		}
		else if ((*it)->strChangeType=="DestObjsChange")
		{
			//目标对象8InitDestObjChange
			CNewSkillProperties::tagDestObjChange *ptgDest = dynamic_cast<CNewSkillProperties::tagDestObjChange*>(*it);
			InitDestObjChange(ptgDest,vOperator);
		}
		else if ((*it)->strChangeType=="RegisterTimer")
		{
			//定时器时间
			CNewSkillProperties::tagRegisterTimer *ptgTimer = dynamic_cast<CNewSkillProperties::tagRegisterTimer*>(*it);
			InitTimerChange(dwID, ptgTimer,vOperator);
		}
		else if ((*it)->strChangeType=="RegisterEvent")
		{
			//注册事件9
			CNewSkillProperties::tagRegisterEvent *ptgEvent = dynamic_cast<CNewSkillProperties::tagRegisterEvent*>(*it);
			InitEventChange(dwID, ptgEvent,vOperator);
		}
		else if ((*it)->strChangeType=="UnRegisterTimer")
		{
			////注消时间	
			CNewSkillProperties::tagUnRegisterTimer *ptgUnRegisterTimer = dynamic_cast<CNewSkillProperties::tagUnRegisterTimer*>(*it);
			InitUnRegisterTimer(ptgUnRegisterTimer,vOperator);
		}
		else if ((*it)->strChangeType=="UnRegisterEvent")
		{
			////注消事件		
			CNewSkillProperties::tagUnRegisterEvent *ptgUnRegisterEvent = dynamic_cast<CNewSkillProperties::tagUnRegisterEvent*>(*it);
			InitUnRegisterEvent(ptgUnRegisterEvent,vOperator);
		}
		else if ((*it)->strChangeType=="SendMsg")
		{
			//发送消息11InitSendMsg
			CNewSkillProperties::tagSendMsg *ptgSendMsg = dynamic_cast<CNewSkillProperties::tagSendMsg*>(*it);
			InitSendMsg(ptgSendMsg,vOperator);	
		}
		else if ((*it)->strChangeType=="DamageChange")
		{
			//伤害
			CNewSkillProperties::tagDamageChange *ptgDamage = dynamic_cast<CNewSkillProperties::tagDamageChange*>(*it);
			InitDamageChange(ptgDamage,vOperator);
		}	
		else if ((*it)->strChangeType=="DoDamage")
		{
			//DO伤害
			CNewSkillProperties::tagDoDamage *ptgDoDamage = dynamic_cast<CNewSkillProperties::tagDoDamage*>(*it);
			InitDoDamage(ptgDoDamage,vOperator);
		}
		else if ((*it)->strChangeType=="GoodChange")
		{
			//物品
			CNewSkillProperties::tagGoodChange *ptgGoodChange = dynamic_cast<CNewSkillProperties::tagGoodChange*>(*it);
			InitGoodChange(ptgGoodChange,vOperator);
		}

		else if ((*it)->strChangeType=="DirecChange")
		{
			//方向
			CNewSkillProperties::tagDirection *ptgTemp = dynamic_cast<CNewSkillProperties::tagDirection*>(*it);
			InitDirection(ptgTemp,vOperator);
		}
		else if ((*it)->strChangeType=="SkillChange")
		{
			//技能操作
			CNewSkillProperties::tagSkillChange *ptgTemp = dynamic_cast<CNewSkillProperties::tagSkillChange*>(*it);
			InitSkillChange(ptgTemp,vOperator);

		}
        else if ((*it)->strChangeType=="GeniusChange")
        {
            //天赋
            CNewSkillProperties::tagGeniusChange *ptgTemp = dynamic_cast<CNewSkillProperties::tagGeniusChange*>(*it);
            InitGeniusChange(ptgTemp,vOperator);
        }
	}		
}


//创建基本模块	

CBaseModule	*CNewSkillFactory::CreateModule(DWORD dwID, DWORD dwLevel,stModuParam *pModuParam,DWORD dwGlobleID ,long lGoodsIndex,long lType)
{


	CNewSkill *pSkillProperties = QueryNewSkillProperty(dwID);
	if (NULL==pSkillProperties)
	{
		return NULL;
	}
	CNewSkill::tagSkill *ptgSkill = pSkillProperties->GetNewSkill(dwLevel);

	if(NULL == ptgSkill)
	{
		return NULL;
	}
	pModuParam->SetGlobleID(dwGlobleID);
	CBaseModule *pBaseModule  = GetBaseModule(pModuParam);

	g_dwCurrentVersion = g_dwVersion;	

	tagModuleKey tgMKey;
	tgMKey.dwID = dwID;
	tgMKey.dwLvl = dwLevel;
	g_mapModuleTimer[tgMKey] = timeGetTime();

	tagBKey tgKey;
	tgKey.lFlag = lType;
	tgKey.lID   = dwID;


	m_UsingBaseModule[tgKey].insert(pBaseModule);


	return pBaseModule;
}
//获取变量列表
VariableList	*CNewSkillFactory::GetVariableList( const string &strFileName)
{
	VariableList *pTempVariableList = NULL;

	itVariableList it = g_mapVariableList.find(strFileName);
	if (it!=g_mapVariableList.end())
	{
		pTempVariableList = (*it).second;
	}
	else if(strFileName.size()>0)
	{
		pTempVariableList = OBJ_CREATE(VariableList);
		//CScript *pTempScript =  PopFreeScript(strFileName.c_str());
		//if (pTempScript)
		//{
		//	pTempScript->SetVariableList(pTempVariableList);		

		//	if( pTempScript->LoadScript(m_pParam->strFileName.c_str(), (char*)GetGame()->GetScriptFileData(m_pParam->strFileName.c_str())) )
		//		pTempScript->RunStep();
		//	PushFreeScript(pTempScript);
		//}
		Script::RunEnv env( NULL, NULL, NULL, pTempVariableList );
		GetInst( ScriptSys ).RunScript( env, m_pParam->strFileName.c_str() );

		AddVariableList(strFileName,pTempVariableList);
	}


	return pTempVariableList;
}
void	CNewSkillFactory::AddVariableList( const string &strFileName, VariableList *pVariableList)
{
	g_mapVariableList[strFileName] = pVariableList;
}

//初始化基本参数
void CNewSkillFactory::InitModuParam(CNewSkill *pSkillProperties,stModuParam *pParam,DWORD dwLevel)
{
	if (pSkillProperties && pParam)
	{	

		DWORD dwOldTime = timeGetTime();
		m_pParam = pParam;
		pParam->nLvl = dwLevel;
		pParam->nID = pSkillProperties->GetNewSkillID();	
		pParam->eType =pSkillProperties->GetSkillType();
		pParam->eUseType = GetUseType(pSkillProperties->GetUseType());
		pParam->eAddSubType = GetAddSub(pSkillProperties->GetAddSub());
		pParam->eTagAttack = eAttack(pSkillProperties->GetAttack());
		pParam->eTagTarget = eTarget(pSkillProperties->GetTarget());
		pParam->bTime		= BOOL(pSkillProperties->GetTime());	
		pParam->nElement = pSkillProperties->GetElement();

		pParam->strFileName = pSkillProperties->GetInitScriptName();
		pParam->pEventVar = NULL;

        /*
		CNewSkillProperties::stStaticParam *pStacicParam = pSkillProperties->GetStaticParam(dwLevel);

		if (pStacicParam)
		{
			pParam->lCoolDownTime = pStacicParam->lCoolDownTime;
		}
		else
		{
			pParam->lCoolDownTime = 0;
		}
        */

		pParam->pVariableList = OBJ_CREATE(VariableList);		
		VariableList *pTempVariableList = GetVariableList(pParam->strFileName);
		if (pTempVariableList)
		{
			VarList::Copy( pTempVariableList, pParam->pVariableList );
			//const CVariableList::VarList *pVarList = pTempVariableList->GetVarList();
			//pParam->pVariableList->SetVarList(pVarList);
			//// 设置GUIDLIST
			//const CVariableList::GuidList *pGuidList = pTempVariableList->GetGuidList();
			//if(pGuidList) pParam->pVariableList->SetGuidList(pGuidList);
		}
		#ifdef _DEBUG
				DWORD dwDiffTime = timeGetTime() - dwOldTime;
				Log4c::Trace(ROOT_MODULE,"初始化动态参数时间:%d",dwDiffTime);
		#endif

		tagModuleKey tgMKey;
		tgMKey.dwID = pParam->nID;
		tgMKey.dwLvl = pParam->nLvl;

		itModulerCounter it = g_mapModuleCounter.find(tgMKey);
		if (it!=g_mapModuleCounter.end())
		{
			++(*it).second;
		}
		else
		{
			g_mapModuleCounter[tgMKey] = 1;
		}
//#ifdef _DEBUG
		//char strTemp1[256];
		//_snprintf(strTemp1,256,"创建动态参数ID:%d,LV:%d",pParam->nID,pParam->nLvl);
		//PutStringToFile("NewSkillLog",strTemp1);	
//#endif
	}
}

//暂存入缓存
void CNewSkillFactory::RealeaseBaseMoudule(stModuParam *pModuParam)
{	
	if (pModuParam && !CheckListModule(g_lModuParam,pModuParam))
	{
		g_lModuParam.push_back(pModuParam);
	}	
}
//及时释放动态参数
void	CNewSkillFactory::RealeaseModuParam(stModuParam *pModuParam)
{
	if (pModuParam)
	{
		//#ifdef _DEBUG
		//			char strTemp[256];
		//			_snprintf(strTemp,256,"释放动态参数ID:%d,LV:%d",pModuParam->nID,pModuParam->nLvl);
		//			PutStringToFile("NewSkillLog",strTemp);	
		//#endif
		pModuParam->Clear();	
		OBJ_RELEASE(stModuParam,pModuParam);
	}
}
//周期释动态参数
void	CNewSkillFactory::RealeaseModuParam()
{
	itModuParam it = g_lModuParam.begin();
	for (; it!=g_lModuParam.end(); ++it)
	{
		stModuParam *pModuParam = (*it);
		if (pModuParam)
		{
//#ifdef _DEBUG
//			char strTemp[256];
//			_snprintf(strTemp,256,"释放动态参数ID:%d,LV:%d",pModuParam->nID,pModuParam->nLvl);
//			PutStringToFile("NewSkillLog",strTemp);	
//#endif
			pModuParam->Clear();	
			OBJ_RELEASE(stModuParam,pModuParam);
		}
	}
	g_lModuParam.clear();
}


//检查缓存中listModules中的BaseModule
BOOL CNewSkillFactory::CheckListModule(listModuParam lModuParam,stModuParam *pModuParam)
{
	itModuParam it = find(lModuParam.begin(),lModuParam.end(),pModuParam);
	if (it!=lModuParam.end())
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

//计算hash值
long CNewSkillFactory::CacuHashValue(long lID,long lLevle)
{
	long lValue;

	lValue = (lID>>3) + lLevle;
	if (lID>9999)
	{
		lValue = (lID/10000)*1000 +lID%10000 + ((lLevle<<3) + 2);
	}
	else if (lID>999)
	{
		lValue = (lID/1000)*100 +lID%1000 + ((lLevle<<3) + 2);
	}
	else
	{
		lValue = lID%100 + ((lLevle<<3) + 2);
	}

	lValue = lValue&((2<<12)-1);



	return lValue;
}


//从技能列表中取得技能
CBaseModule *CNewSkillFactory::GetBaseModule(stModuParam *pParam)
{	
	DWORD dwOldTime = timeGetTime();
	DWORD dwID = pParam->GetID();
	DWORD dwLevel = pParam->GetLv();
	long lHashValue = CacuHashValue(dwID,dwLevel);
	CBaseModule *pModule = NULL;

	listModules &lBaseModule = g_BaseModules[lHashValue];

	for(itModule it=lBaseModule.begin(); it!=lBaseModule.end(); ++it)
	{		
		
		if ((*it) && (*it)->GetID() == dwID && (*it)->GetLv()==dwLevel)
		{			
			(*it)->SetParam(pParam);
			pModule = (*it);
			break;			
		}
	}	
	
	if (NULL ==pModule)
	{
		CNewSkill *pSkillProperties = QueryNewSkillProperty(dwID);

		m_pParam = pParam;			
		CBaseModule::tagCBaseModuleParam CBaseModuleParam;
		CBaseModuleParam.pParam = m_pParam;
		CBaseModuleParam.pStatiParam = NULL;
		pModule = OBJ_CREATE_PVOID(CBaseModule, (void*)&CBaseModuleParam);

		InitBaseModule(pModule,dwID,dwLevel,pSkillProperties,pParam);
		lBaseModule.push_back(pModule);	
		++g_dwCount;
	}	
#ifdef _DEBUG
	DWORD dwDiffTime = timeGetTime() - dwOldTime;
	Log4c::Trace(ROOT_MODULE,"技能状态实例化时间:%d",dwDiffTime);
#endif
	return  pModule;
}

//获取技能属性
CNewSkill* CNewSkillFactory::QueryNewSkillProperty(DWORD dwID)
{
	map<DWORD,CNewSkill*>::iterator it=g_mNewSkillProperties.find(dwID);
	if (it!=g_mNewSkillProperties.end())
	{
		return it->second;
	}
	else 
	{
		return NULL;
	}
}

eModuleRet CNewSkillFactory::GetModuleRet(string str)
{	
	if (str=="MR_No")
	{
		return MR_No;
	} 
	else if(str=="MR_Continue")
	{
		return MR_Continue;
	}
	else if (str=="MR_Return")
	{
		return MR_Return;
	}
	else if (str=="MR_Step_End")
	{
		return MR_Step_End;
	}
	else if (str=="MR_Proc_End")
	{
		return MR_Proc_End;
	}
	else if (str=="MR_Modu_End")
	{
		return MR_Modu_End;
	}
	else if (str=="MR_Proc_Next")
	{
		return MR_Proc_Next;
	}
	else
	{
#ifdef _DEBUG
		Log4c::Trace(ROOT_MODULE,"未找到匹配的eModuleRet，返回默认的是%s","MR_No");		
#endif	
		return MR_No;
	}

}
OPFUN *CNewSkillFactory::GetOpFun(string name,long nID)
{
	OPFUN *Op;;
	if (name=="Add")
	{
		Op = AddOper;
	}
	else if (name=="Sub")
	{
		Op = SubOper;
	}
	else if (name=="Set")
	{
		Op = SetOper;
	}
	else if (name=="Update")
	{
		Op = UpdateOper;
	}
	else if (name=="Bool")
	{
		Op = BoolOper;
	}
    else if (name=="Reset")
    {
        Op = ResetOper;
    }
	else 
	{
#ifdef _DEBUG
		Log4c::Trace(ROOT_MODULE, AppFrame::GetText("STR_WS_SKILL_TENTH"),nID,name.c_str());		
#endif	
		Op = NULL;
	}

	return Op;
}
CMPFUN *CNewSkillFactory::GetCmpFun(string name,long type)
{
	if (name=="Less")
	{
		return Less;
	}
	else if (name=="Equal")
	{
		return Equal;
	}
	else if (name=="LessEqual")
	{
		return LessEqual;
	}
	else if (name=="Greater")
	{
		return Greater;
	}
	else if (name=="GreaterEqual")
	{
		return GreaterEqual;
	} 
	else if(name=="NotEqual")
	{
		return NotEqual;
	}
	else if(type==0)
	{
#ifdef _DEBUG
		Log4c::Trace(ROOT_MODULE, "未找到匹配的CMPFUN，返回默认的是%s","Less");		
#endif	
		return Less;
	}
    
    return NULL;
}

eSkillAttackType  CNewSkillFactory::GetSkillAttType(string type)
{
     if (type=="SKILL_ATTTYPE_OUTENERGY")
     {
         return SKILL_ATTTYPE_OUTENERGY;
     }
     else if (type=="SKILL_ATTTYPE_ENERGY")
     {
         return SKILL_ATTTYPE_ENERGY;
     }
     else if (type=="SKILL_ATTTYPE_CURSE")
     {
         return SKILL_ATTTYPE_CURSE;
     }
     else if (type=="SKILL_ATTTYPE_POX")
     {
         return SKILL_ATTTYPE_POX;
     }
     else if (type=="SKILL_ATTTYPE_ILLNESS")
     {
         return SKILL_ATTTYPE_ILLNESS;
     }
     else if (type=="SKILL_ATTTYPE_POISON")
     {
         return SKILL_ATTTYPE_POISON;
     }
     else if (type=="SKILL_ATTTYPE_FIRE")
     {
         return SKILL_ATTTYPE_FIRE;
     }
     else if (type=="SKILL_ATTTYPE_SHADOW")
     {
         return SKILL_ATTTYPE_SHADOW;
     }
     else if (type=="SKILL_ATTTYPE_FROST")
     {
         return SKILL_ATTTYPE_FROST;
     }
     else if (type=="SKILL_ATTTYPE_NATURE")
     {
         return SKILL_ATTTYPE_NATURE;
     }
     else if (type=="SKILL_ATTTYPE_HOLINESS")
     {
         return SKILL_ATTTYPE_HOLINESS;
     }
     else
     {
#ifdef _DEBUG
         Log4c::Trace(ROOT_MODULE,"GetSkillAttType函数,未找到匹配的eSkillAttackType返回默认的是%s","SKILL_ATTTYPE_NO");		
#endif	
         return SKILL_ATTTYPE_NO;
     }
}

COpSendMsg::eSendMsgType CNewSkillFactory::GetMsgType(long nID,string strName)
{		
	if (strName=="SMT_ProcBegin")
	{
		return COpSendMsg::SMT_ProcBegin;
	} 
	else if(strName=="SMT_ProcEnd")
	{
		return COpSendMsg::SMT_ProcEnd;
	}
	else if (strName=="SMT_ProcFail")
	{
		return COpSendMsg::SMT_ProcFail;
	}
	else
	{
#ifdef _DEBUG
		Log4c::Trace(ROOT_MODULE,FormatText("STR_WS_SKILL_ELEVENTH",nID,strName.c_str(),"SMT_ProcNo"));		
#endif	
		return COpSendMsg::SMT_ProcNo;
	}

}
COpSendMsg::eMsgRange CNewSkillFactory::GetRange(string strName)
{
	if (strName=="MsgRange_Single")
	{
		return COpSendMsg::MsgRange_Single;
	} 
	else if(strName=="MsgRange_Around")
	{
		return COpSendMsg::MsgRange_Around;
	}
	else if (strName=="MsgRange_Region")
	{
		return COpSendMsg::MsgRange_Region;
	}
	else
	{
#ifdef _DEBUG
		Log4c::Warn(ROOT_MODULE, "未找到匹配的eMsgRange，返回默认的是%s","MsgRange_Single");		
#endif	
		return COpSendMsg::MsgRange_Single;	
	}

}


tagDamageType CNewSkillFactory::GetDamageType(long nID,string strName)
{
	if (strName=="DAMAGE_TYPE_HP")
	{
		return DAMAGE_TYPE_HP;
	} 
	else if(strName=="DAMAGE_TYPE_MP")
	{
		return DAMAGE_TYPE_MP;
	}
    else if(strName=="DAMAGE_TYPE_TAUNT")
    {
        return DAMAGE_TYPE_TAUNT;
    }
	else
	{
#ifdef _DEBUG
		Log4c::Trace(ROOT_MODULE,FormatText("STR_WS_SKILL_TWELFTH",nID, strName.c_str(), "DAMAGE_TYPE_NO "));		
#endif	
		return DAMAGE_TYPE_NO;
	}

}
CConValidObj::eType	CNewSkillFactory::GetEType(string strType)
{
	if (strType=="Player_Type")
	{
		return CConValidObj::ePlayer_Type;
	}
	else if (strType=="Monster_Type")
	{
		return CConValidObj::eMonster_Type;
	}
	else if (strType=="Gate_Type")
	{
		return CConValidObj::eGate_Type;
	}
	else if (strType=="Pet_Type")
	{
		return CConValidObj::ePet_Type;
	}
	else
	{
		return CConValidObj::ePlayer_Type;
	}

}
list<CConValidObj::eValidObjType> CNewSkillFactory::GetValidType(list<string> lStr)
{
	list<CConValidObj::eValidObjType> lValidObjType;
	for (list<string>::iterator it=lStr.begin(); it!=lStr.end();++it)
	{
		if ((*it)=="SelfPlayer_Type")
		{
			lValidObjType.push_back(CConValidObj::SelfPlayer_Type) ;
			continue;
		} 
		else if ((*it)=="EnemyPlayer_Type")
		{
			lValidObjType.push_back(CConValidObj::EnemyPlayer_Type) ;
			continue;
		} 
		else if((*it)=="Monster_Type")
		{
			lValidObjType.push_back(CConValidObj::Monster_Type) ;
			continue;

		}
		else if((*it)=="Npc_Type")
		{
			lValidObjType.push_back(CConValidObj::Npc_Type) ;
			continue;

		}
		else if((*it)=="SMonster_Type")
		{
			lValidObjType.push_back(CConValidObj::SMonster_Type) ;
			continue;			
		}
		else if ((*it)=="Guard_Type")
		{
			lValidObjType.push_back(CConValidObj::Guard_Type);
			continue;
		}
		else if((*it)=="NormalPlayer_Type")
		{
			lValidObjType.push_back(CConValidObj::NormalPlayer_Type) ;
			continue;

		}
		else if((*it)=="RedPlayer_Type")
		{
			lValidObjType.push_back(CConValidObj::RedPlayer_Type) ;
			continue;

		}
		else if((*it)=="PurplePlayer_Type")
		{
			lValidObjType.push_back(CConValidObj::PurplePlayer_Type) ;
			continue;

		}
		else if ((*it)=="TeamPlayer_Type")
		{
			lValidObjType.push_back(CConValidObj::TeamPlayer_Type);
			continue;
		}
		else if ((*it)=="TongPlayer_Type")
		{
			lValidObjType.push_back(CConValidObj::TongPlayer_Type);
			continue;
		}
		else if ((*it)=="LeaguePlayer_Type")
		{
			lValidObjType.push_back(CConValidObj::LeaguePlayer_Type);
			continue;
		}
		else
		{
			lValidObjType.push_back(CConValidObj::SelfPlayer_Type) ;
			continue;				
		}
	}
	return lValidObjType;


}

eModuleOper CNewSkillFactory::GetOperator(long nID, string strName)
{	
	if (strName=="Add")
	{
		return MODOP_Add;
	}
	else if (strName=="Sub")
	{
		return MODOP_Sub;
	}
	else if (strName=="Set")
	{
		return MODOP_Set;
	}
	else if (strName=="Clear")
	{
		return MODOP_Clear;
	}
	else if (strName=="Inc")
	{
		return MODOP_Inc;
	}
	else
	{
#ifdef _DEBUG
		Log4c::Trace(ROOT_MODULE,FormatText(("STR_WS_SKILL_THIRTEENTH",nID,strName.c_str(),"MODOP_Add")));			
#endif
		return MODOP_Add;
	}
}	

eEventType CNewSkillFactory::GetEventType(long nID, string strName)
{
	//移动
	if (strName=="ET_Move")
	{
		return ET_Move;
	}
	else if (strName=="ET_BeAttack")
	{
		return ET_BeAttack;
	}
	else if (strName=="ET_Attack")
	{
		return ET_Attack;
	}
	else if (strName=="ET_Damage")
	{
		return ET_Damage;
	}
	else if (strName=="ET_EquipMount")
	{
		return ET_EquipMount;
	}
	else if (strName=="ET_EquipUnmount")
	{
		return ET_EquipUnmount;
	}
	else if (strName=="ET_ExitGame")
	{
		return ET_ExitGame;
	}
	else if (strName=="ET_ExitRegion")
	{
		return ET_ExitRegion;
	}
	else if (strName=="ET_AddState")
	{
		return ET_AddState;
	}
	else if (strName=="ET_UseSkill")
	{
		return ET_UseSkill;
	}
	else if (strName=="ET_EnterGame")
	{
		return ET_EnterGame;
	}
	else if (strName=="ET_ChangeOccu")
	{
		return ET_ChangeOccu;
	}	
	else if (strName=="ET_ResetBaseFightProperty")
	{
		return ET_ResetBaseFightProperty;
	}
	else if (strName=="ET_Trade")
	{
		return ET_Trade;	//交易
	}
	else if (strName=="ET_OpenShop")
	{
		return ET_OpenShop;//开店
	}
	else if (strName=="ET_Kill")
	{
		return ET_Kill;//杀死目标			
	}
	else if (strName=="ET_BeenKilled")
	{
		return ET_BeenKilled;//被杀死
	}
	else if (strName=="ET_Blast")
	{
		return ET_Blast;//爆击目标
	}
	else if (strName=="ET_BeenBlast")
	{
		return ET_BeenBlast;//被爆击
	}
	else if (strName=="ET_Block")
	{
		return ET_Block;//格挡目标
	}
	else if (strName=="ET_BeenBlocked")
	{
		return ET_BeenBlocked;//被目标格挡
	}
	else if (strName=="ET_Dodge")
	{
		return ET_Dodge;//闪避目标
	}
	else if (strName=="ET_BeenDodged")
	{
		return ET_BeenDodged;//被目标闪避
	}
	else if (strName=="ET_Parry")
	{
		return ET_Parry;//招架目标
	}
	else if (strName=="ET_BeenParried")
	{
		return ET_BeenParried;//被招架
	}
	else if (strName=="ET_Resist")
	{
		return ET_Resist;//抵抗目标
	}
	else if (strName=="ET_BeenResisted")
	{
		return ET_BeenResisted;//被抵抗
	}	
	else if (strName=="ET_Fighting")
	{
		return  ET_Fighting; //进入战斗状态
	}
	else if (strName=="ET_Peace")
	{
		return ET_Peace; //进入和平状态
	}
	else if (strName=="ET_BreakSkill")
	{
		return ET_BreakSkill; //进入中断技能
	}
	else if (strName=="ET_StateBreak") //状态中断
	{
		return ET_StateBreak;
	}
	else if (strName=="ET_EventBreak") //事件中断
	{	
		return ET_EventBreak;
	}
	else if (strName=="ET_EnterRegion")//进入场景事件
	{
		return ET_EnterRegion;
	}
	else if (strName=="ET_SuccessSkill")//施放技能成功
	{
		return ET_SuccessSkill;
	}
	else if (strName=="ET_BeenDamage")//被伤害
	{
		return ET_BeenDamage;
	}

	else if (strName=="ET_DamageEx")//额外伤害
	{
		return ET_DamageEx;
	}
	else if (strName=="ET_BeenDamageEx")//额外被伤害
	{
		return ET_BeenDamageEx;
	}
	else if (strName=="ET_Homing")//归位事件
	{
		return ET_Homing;
	}
	else if (strName=="ET_SAttack")//DOTA伤害
	{
		return ET_SAttack;
	}
	else if (strName=="ET_BeSAttack")//被DOTA伤害
	{
		return ET_BeSAttack;
	}		
	else 
	{
#ifdef _DEBUG
		Log4c::Trace(ROOT_MODULE,FormatText("STR_WS_SKILL_FOURTEENTH",nID,strName.c_str(), "ET_Move"));		
#endif	
		return ET_Move;
	}

}

eObj CNewSkillFactory::GetObj(string strName)
{
	if (strName=="Obj_Self")
	{
		return Obj_Self;
	} 
	else if(strName=="Obj_Dest")
	{
		return Obj_Dest;
	}
	else if (strName=="Obj_Mouse")
	{
		return Obj_Mouse;
	}
	else if (strName=="Obj_Firer")
	{
		return Obj_Firer;
	}
	else
	{
		return Obj_Self;
	}

}
COpDirec::eDirec CNewSkillFactory::GetDirec(string str)
{
	if (str=="Dir_Mouse")
	{
		return COpDirec::eDir_Mouse;
	}
	else if (str=="Dir_Self")
	{
		return COpDirec::eDir_Self;
	}
	else if ("Dir_Dest")
	{
		return COpDirec::eDir_Dest;
	}
	else 
	{
		return COpDirec::eDir_Dest;
	}
}


CCondiGroup::eGrouMode CNewSkillFactory::GetGroupMode(string str)
{
	if (str=="Mode_And")
	{
		return CCondiGroup::Mode_And;
	}
	else if (str=="Mode_Or")
	{
		return CCondiGroup::Mode_Or;
	}
	else
	{
		return CCondiGroup::Mode_And;
	}


}

eChildObjType  CNewSkillFactory::GetChildObjType(ModuleType type)
{	 
	if (type==ModuType_Buff)
	{
		return ChildObjType_Buff;
	}
	else if (type==ModuType_DeBuff)
	{
		return ChildObjType_DeBuff;
	}
	else if (type==ModuType_Skill_Active)
	{
		return ChildObjType_ActiveSkill;
	}
	else if (type==ModuType_Skill_Passive)
	{
		return ChildObjType_PassiveSkill;
	}
	else if (type==ModuType_Skill_OnOff)
	{
		return ChildObjType_OnOffSkill;
	}			   		
	else
	{
		return ChildObjType_Buff;
	}

}

ModuleType	CNewSkillFactory::GetModuleType(string strName)
{	
	if (strName=="ModuType_Buff")
	{
		return ModuType_Buff;
	} 
	else if (strName=="ModuType_DeBuff")
	{
		return ModuType_DeBuff;
	} 
	else if (strName=="ModuType_Skill_Active")
	{
		return ModuType_Skill_Active;
	} 
	else if (strName=="ModuType_Skill_Passive")
	{
		return ModuType_Skill_Passive;
	} 
	else if (strName=="ModuType_Skill_OnOff")
	{
		return ModuType_Skill_OnOff;
	}
	else
	{
		return ModuType_Skill_Active;
	}
}

eSkillUseType	CNewSkillFactory::GetUseType(BYTE bUseType)
{
	if (bUseType==SKILL_TYPE_PHYSICAL)
	{
		return SKILL_TYPE_PHYSICAL;
	} 
	else if (bUseType==SKILL_TYPE_MAGIC)
	{
		return SKILL_TYPE_MAGIC;
	}
	else
	{
		return SKILL_TYPE_NO;
	}
}

eAddSub			CNewSkillFactory::GetAddSub(string strAddSub)
{
	if (strAddSub=="Add")
	{
		return eAdd_Type;
	} 
	else if (strAddSub=="Sub")
	{
		return eSub_Type;
	}
	else
	{
		return eNo_Type;
	}
}


void CNewSkillFactory::OutputBasemodelInfo(const char *pszFileName)
{
	long lTotalCount = 0;
	Log4c::Trace(ROOT_MODULE,"UseBaseModelInfo:");
	map<tagBKey,  set<CBaseModule*>,tagBKeycmp>::iterator it = m_UsingBaseModule.begin();
	for(;it != m_UsingBaseModule.end();it++)
	{
		lTotalCount += (long)(*it).second.size();
		if((*it).second.size() > 0)
		{
			Log4c::Trace(ROOT_MODULE, "BaseModelFlag:%d,ID:%d Count:%d",(*it).first.lFlag,(*it).first.lID,(*it).second.size() );		
		}
	}
}

void CNewSkillFactory::CheckBaseModuleTimeOut(long lNowTime)
{
	itModuleTimer it = g_mapModuleTimer.begin();
	for (;it!=g_mapModuleTimer.end();)
	{
		if (lNowTime-(*it).second > GlobalSetup::GetSetup()->lBaseModuleTime)
		{	
			//long lHashValue = CacuHashValue((*it).first.dwID,(*it).first.dwLvl);

			//listModules &lBaseModule = g_BaseModules[lHashValue];

			//for(itModule bit=lBaseModule.begin(); bit!=lBaseModule.end();)
			//{		
			//	if ((*bit)->GetID() == (*it).first.dwID && (*bit)->GetLv()==(*it).first.dwLvl)
			//	{
			//		if (g_dwCount>0)
			//		{
			//			--g_dwCount;	
			//		}	
			//		CBaseModule *pBaseModule = (*bit);
			//		bit = lBaseModule.erase(bit);
			//		OBJ_RELEASE(CBaseModule, pBaseModule);
			//	}
			//	else
			//	{
			//		++bit;
			//	}
			//}
			it = g_mapModuleTimer.erase(it);
			continue;				
		}
		++it;
	}
}



