#include "StdAfx.h"
#include "script.h"
#include "variablelist.h"
#include "..\player.h"
#include "..\ServerRegion.h"
#include "../goods/cgoods.h"
#include "../monster.h"
#include "../moveshape.h"
#include "../public/StrType.h"
#include "../public/StrUnifyInput.h"

#include <vector>
#include <string>






CScript::tagStrValueInfo::tagStrValueInfo(void)
:pStrValue(NULL),lStrValueLen(0)
{

}
CScript::tagStrValueInfo::~tagStrValueInfo(void)
{

}


/************************************************
*	运行一个脚本文件 RunScript(...);			*
************************************************/

listScript g_Scripts;				// 脚本链表表头
FreeScript g_FreeScriptList;     // 预分配的空闲脚本内存
map<CScript*,string> g_UsingScript;     // 正在使用的Script

CVariableList* CScript::g_StackVarList = NULL;
list<CGUID>	CScript::m_DelScripts;

CVariableList* CScript::GetStackVarListInst(void)
{
    if(NULL == g_StackVarList)
        g_StackVarList = MP_NEW CVariableList;
    return g_StackVarList;
}
void CScript::ReleaseStackVarListInst(void)
{
	MP_DELETE(g_StackVarList);
}
///////////////////
CScript* PopFreeScript(const char* pszName)
{
    CScript* tempScript = NULL;
    if( g_FreeScriptList.size() > 0 ) //有空闲Script内存块
    {
        tempScript = g_FreeScriptList.front();
        g_FreeScriptList.pop_front();	
    }
    else
    {
        tempScript = MP_NEW CScript;
    }
    tempScript->ResetVariableList();
    tempScript->ResetChildScriptInfo();
	tempScript->SetSrcShape(NULL);
	tempScript->SetDesShape(NULL);
	tempScript->SetRegion(NULL);

	if(pszName)
		g_UsingScript[tempScript]=pszName;
	else
		g_UsingScript[tempScript]="";
    return tempScript;
}
void	 PushFreeScript(CScript* freeScript)
{
    if(NULL == freeScript)	return;

	g_UsingScript.erase(freeScript);
    // 如果空闲队列没有达到最大数量
    if(g_FreeScriptList.size() <= CGlobeSetup::GetSetup()->lMaxPreAllocedScriptNum)
    {
        freeScript->GetSelfVariableList()->Release();
        g_FreeScriptList.push_back(freeScript);
    }
    else //清除多余空闲内存
    {
        MP_DELETE(freeScript);
    }
}

//输出脚本信息
void	OutputScriptInfo()
{
	char pszInfo[1024]="";
	_snprintf(pszInfo,1024,"g_UsingScript:%d,g_Scripts:%d",g_UsingScript.size(),g_Scripts.size());
	PutStringToFile("ScriptInfo",pszInfo);
	map<CScript*,string>::iterator it = g_UsingScript.begin();
	for(;it != g_UsingScript.end();it++)
	{
		bool bFlag = false;
		char pszSrcGuid[50]="";
		const CGUID& guid = (*it).first->GetID();
		(*it).first->GetSrcID().tostring(pszSrcGuid);
		if(guid != NULL_GUID)
			bFlag = ( g_Scripts.find(guid )  != g_Scripts.end() );
		_snprintf(pszInfo,1024,"(SrcType:%d,SrcGuid:%s)flag:%d,IsHang:%d,HangFun:%d,fileame:%s",(*it).first->GetSrcType(),pszSrcGuid,bFlag,(*it).first->IsHang(),(*it).first->GetHangFunc(),(*it).second.c_str());
		PutStringToFile("ScriptInfo",pszInfo);
	}
}
// 客户端选择有效性判断
bool ClientOptIsValid(const CGUID& scriptid, const char* szClientOpt)
{
	if(scriptid == NULL_GUID) return true;

	listScript::iterator itr = g_Scripts.find(scriptid);
	if(itr != g_Scripts.end())
	{
		if( !itr->second->GetClientOptInfo()
			|| itr->second->GetClientOptInfo()[0] == '\0') return true;

		if(strstr(itr->second->GetClientOptInfo(), szClientOpt))
			return true;
		else
			return false;
	}
	return false;
}
// 清除脚本的客户端选择检查数据
void ClearClientOptData(const CGUID& scriptid)
{
	listScript::iterator itr = g_Scripts.find(scriptid);
	if(itr != g_Scripts.end())
	{
		if(itr->second)
			itr->second->SetClientOptInfo("");
	}
}
// 外部调用脚本
double RunScript(stRunScript *p, char* pData, int paramA, int paramB)
{
    if(p==NULL || pData==NULL)
        return false;

#ifdef _RUNSTACKINFO_
    char pszStatckInfo[10240]="";
    _snprintf(pszStatckInfo,10240,"RunScript(FileName:%s) Start",p->pszFileName?p->pszFileName:"");
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStatckInfo);
#endif


    CScript* tempScript = NULL;
    double lIndexID = 0; //全局的唯一脚本ID

    tempScript = PopFreeScript(p->pszFileName);
#ifdef _SCRIPT_INFO_
	DWORD BeginTime = timeGetTime();
	tempScript->m_RunTime = 0;
#endif
    // 初始化script对象
    tempScript->m_dwLastTime=0;
    if(p->srcShape && (OBJECT_TYPE)p->srcShape->GetType() == TYPE_PLAYER)
        tempScript->SetPlayerID(p->srcShape->GetExID());
    else
        tempScript->SetPlayerID(NULL_GUID);
    tempScript->SetRegion(p->pRegion);
    tempScript->SetSrcShape(p->srcShape);
    tempScript->SetDesShape(p->desShape);
    tempScript->SetUsedItemID(p->guUsedItemID);
    POINT pointXY;
    pointXY.x = -1;
    pointXY.y = -1;
    tempScript->SetDropGoodsPoint(pointXY);
    CGUID guid;
    CGUID::CreateGUID(guid);
    tempScript->SetID(guid);
    // 加载脚本内容
    double ret = 0; // 返回值
    if( tempScript->LoadScript(p->pszFileName, pData) )
    {
        tempScript->GetVariableList()->AddVar("$ParamA", paramA);
        tempScript->GetVariableList()->AddVar("$ParamB", paramB);
        tempScript->GetVariableList()->AddVar("$Ret", 0.0f);
        // 执行脚本
        if( tempScript->RunStep() == CScript::SR_HANG )
        {
            g_Scripts[guid] = tempScript;
#ifdef _RUNSTACKINFO_
            _snprintf(pszStatckInfo,10240,"RunScript(FileName:%s) Hang",p->pszFileName?p->pszFileName:"");
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStatckInfo);
#endif
#ifdef _SCRIPT_INFO_
			tempScript->m_RunTime += timeGetTime() - BeginTime;
#endif
            return 0;
        }
        else
        {
            ret = tempScript->GetVariableList()->GetVarValue("$Ret", 0);
        } 
    }
    
#ifdef _SCRIPT_INFO_
	GetGame()->AddScriptInfo((p->pszFileName?p->pszFileName:""), timeGetTime() - BeginTime);
	tempScript->m_RunTime = 0;
#endif

	PushFreeScript(tempScript);

#ifdef _RUNSTACKINFO_
	_snprintf(pszStatckInfo,10240,"RunScript(FileName:%s) End",p->pszFileName?p->pszFileName:"");
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStatckInfo);
#endif

    return ret;
}

// 往脚本里添加参数
static void _AddParamList( CScript *pScript, const VarPairListType &var_list )
{
	CVariableList *pVarList = pScript->GetVariableList();
	for( VarPairListType::const_iterator it = var_list.begin(); it != var_list.end(); ++ it )
	{
		const stVarPair &vp = *it;
		switch( vp.ValueType )
		{
		case stVarPair::VT_NUMBER:
			pVarList->AddVar( vp.Name.c_str(), vp.NumValue );
			break;

		case stVarPair::VT_STRING:
			pVarList->AddVar( vp.Name.c_str(), vp.StrValue.c_str() );
			break;

		case stVarPair::VT_GUID:
			pVarList->AddGuid( vp.Name.c_str(), vp.GuidValue );
			break;

		default:
			// error
			break;
		}
	}

	// add ret
	pVarList->AddVar( "$Ret", (double)0 );
}

double RunScript( stRunScript *p, char *pData, const VarPairListType &var_list )
{
    if( p == NULL || pData == NULL )
	{
        return false;
	}
    CScript* tempScript = NULL;
    double lIndexID = 0; //全局的唯一脚本ID

    tempScript = PopFreeScript(p->pszFileName);
    tempScript->m_dwLastTime=0;
    if(p->srcShape && (OBJECT_TYPE)p->srcShape->GetType() == TYPE_PLAYER)
        tempScript->SetPlayerID(p->srcShape->GetExID());
    else
        tempScript->SetPlayerID(NULL_GUID);
    tempScript->SetRegion(p->pRegion);
    tempScript->SetSrcShape(p->srcShape);
    tempScript->SetDesShape(p->desShape);
    tempScript->SetUsedItemID(p->guUsedItemID);
    POINT pointXY;
    pointXY.x = -1;
    pointXY.y = -1;
    tempScript->SetDropGoodsPoint(pointXY);
    CGUID guid;
    CGUID::CreateGUID(guid);
    tempScript->SetID(guid);
    // 加载脚本内容
    double ret = 0; // 返回值
    if( tempScript->LoadScript(p->pszFileName, pData) )
    {
		_AddParamList( tempScript, var_list );
        // 执行脚本
        if( tempScript->RunStep() == CScript::SR_HANG )
        {
            g_Scripts[guid] = tempScript;
            return 0;
        }
        else
        {
            ret = tempScript->GetVariableList()->GetVarValue("$Ret", 0);
        } 
    }
    PushFreeScript(tempScript);

    return ret;
}

long DelectAllScripByShape(const CGUID& PlayerGuid)
{
    for(itScript it=g_Scripts.begin(); it!=g_Scripts.end(); it++)
    {
        CScript* p = (CScript*)(*it).second;
        if( p && p->GetSrcID() == PlayerGuid )
        {
			CScript::PushDelScript((*it).first);
        }
    }
    return true;
}

//删除玩家的Talkbox脚本
long DelPlayerTalkBoxScript(const CGUID& PlayerGuid, SCRIPT_FUNC_DEF funcDef)
{

    for(itScript it=g_Scripts.begin(); it!=g_Scripts.end(); it++)
    {
        CScript* p = (CScript*)(*it).second;
        if( p && p->GetSrcID() == PlayerGuid &&
            p->IsHang() &&
            (p->GetHangFunc() == funcDef) )
        {
			CScript::PushDelScript((*it).first);
        }
    }

    return true;
}


//删除某个ID对应的脚本
long DelectPlayerScript(const CGUID &ScriptID,const CGUID& PlayerGuid)
{
    itScript it = g_Scripts.find(ScriptID);

    if(it != g_Scripts.end())
    {
        CScript* p = (CScript*)(*it).second;
        if(p && p->GetSrcID() == PlayerGuid)
        {
			CScript::PushDelScript(ScriptID);
        }
    }
    return true;
}

//在脚本链表中清除PLAYER对应的脚本
long DelectPlayerScript(CPlayer* pPlayer,char* strFile)
{
    if(pPlayer==NULL || strFile == NULL)
        return false;

    for(itScript it=g_Scripts.begin(); it!=g_Scripts.end(); it++)
    {
        CScript* p = (CScript*)(*it).second;
        if( p && p->GetSrcID() == pPlayer->GetExID() && p->GetFileName() &&
            strcmp(strFile,p->GetFileName()) == 0 )
        {
			CScript::PushDelScript((*it).first);
        }
    }
    return true;
}

//判断某个万家的脚本是否正在执行
bool ScriptIfExit(CPlayer* pPlayer,char* strFile)
{
    if(pPlayer==NULL || strFile == NULL)
        return false;

    for(itScript it=g_Scripts.begin(); it!=g_Scripts.end();it++)
    {
        CScript* p = (CScript*)(*it).second;
        if( p && p->GetPlayerID() == pPlayer->GetExID() && p->GetFileName() &&
            strcmp(strFile,p->GetFileName()) == 0 )
        {
            return true;
        }
    }

    return false;
}

//继续执行一个脚本
long ContinueRunScript(const CGUID& ScriptID, CVariableList* stackVarList)
{
    itScript it = g_Scripts.find(ScriptID);
    if(it != g_Scripts.end())
    {
        CScript* pScript = (*it).second;
        if(pScript)
        {
            pScript->ResetShape();
            if( pScript->ContinueRun(stackVarList) != CScript::SR_HANG )
            {
                CScript::PushDelScript(ScriptID);
                return 1;
            }
            else
            {
                return 0;
            }
        }
        return 1;
    }
    return 0;
}
//得到一个脚本的全局ID
long GetScriptIndex()
{
    static double lID = 0;
    return ++lID;
}
/************************************************
*	-= 脚本类 =-								*
*	读入并执行脚本文件							*
*	（１）用以控制NPC的行为						*
*	（２）编写特定的情节						*
*	（３）制作任务								*
*	（４）动态改变游戏中的各种数据［巫师］		*
************************************************/

////////////////
// 函数列表

double CScript::m_LoadFunctionNum=0;

CScript::STRING2PFUNC* CScript::s_FunctionList2=0;

double CScript::m_MaxFunction=0;

////////////////
// 全局变量
CVariableList* CScript::m_GeneralVariableList=NULL;
CVariableList* CScript::m_BuffSkillVariableList=NULL;

//////////////////////////////////////
// 操作符列表 (数值越大，优先级越大)
CScript::stOperation CScript::m_OperationList[MAX_OPERATION]=
{
    "", 0,
    ";",1,
    "||",2,
    "&&",3,
    "==",4, "!=",4, ">",4,  "<",4,  ">=",4, "<=",4,
    "=",5,  "+=",5, "-=",5, "*=",5, "/=",5, "%=",5,
    "&",6,  "|",6,
    "+",7,  "-",7,
    "*",8,  "/",8,  "%",8
};

///////////////
// 构建
CScript::CScript(CShape* srcShape, CRegion *Region, CShape *desShape, CGUID& guUsedItemID, DWORD dwDiedMonsterIndex)
:m_strRunScriptName("")
{
    m_FileName[0] = 0;
    m_Data=NULL;
    m_DataLen=0;
    m_Block=0;

	m_szClientUpdateVarName[0] = '\0';
	m_szClientOptInfo = NULL;
	m_lClientOptInfoLen = 0;

	m_VarNum = 0;

    m_dwLastTime=0;
    m_guIndexID = NULL_GUID;
    if(srcShape && (OBJECT_TYPE)srcShape->GetType() == TYPE_PLAYER)
        m_PlayerGuid=srcShape->GetExID();
    else
        m_PlayerGuid=NULL_GUID;

    m_pRegion = Region;
    p_SrcShape = srcShape;
    p_DesShape = desShape;

    m_guUsedItemID = guUsedItemID;
    m_ptDropGoods.x = -1;
    m_ptDropGoods.y = -1;
    m_pChildScriptInfo = NULL;

    m_pSelfVariableList = MP_NEW CVariableList;
    m_pVariableList = NULL;
}

/////////////////////
// 释放
CScript::~CScript()
{
	M_FREE(m_szClientOptInfo, sizeof(char)*MAX_VAR_LEN);

    M_FREE(m_Data, sizeof(char)*(m_DataLen+1));		//释放
    m_DataLen=0;

    for (int i=0;i<MAX_VAR;i++)
	{
		M_FREE(m_strValueInfo[i].pStrValue, m_strValueInfo[i].lStrValueLen);
		m_strValueInfo[i].pStrValue = NULL;
		m_strValueInfo[i].lStrValueLen = 0;
	}

    MP_DELETE(m_pSelfVariableList);

    if(m_pChildScriptInfo)
    {
        MP_DELETE(m_pChildScriptInfo->pChildScript);
        MP_DELETE(m_pChildScriptInfo);
    }
}
void CScript::SetClientOptInfo(const char* szInfo) 
{ 
	if(szInfo)
	{
		M_FREE(m_szClientOptInfo, m_lClientOptInfoLen);
		long lLen = strlen(szInfo);
		m_szClientOptInfo = (char*)M_ALLOC(sizeof(char)*(lLen+1));
		strcpy_s(m_szClientOptInfo, m_lClientOptInfoLen, szInfo);
		m_lClientOptInfoLen = lLen+1;
	}
}
//释放子脚本
void CScript::ReleaseChildScript()
{
    if(m_pChildScriptInfo)
    {
        if(m_pChildScriptInfo->pChildScript)
            m_pChildScriptInfo->pChildScript->ReleaseChildScript();
        PushFreeScript(m_pChildScriptInfo->pChildScript);
        MP_DELETE(m_pChildScriptInfo);
    }
}

void CScript::PushDelScript(const CGUID& guid)
{
	m_DelScripts.push_back(guid);
	return;
}
//删除标记的脚本
void CScript::DelScript()
{
	list<CGUID>::iterator it = m_DelScripts.begin();
	for(;it != m_DelScripts.end();it++)
	{
		itScript itScript = g_Scripts.find((*it));
		if(itScript != g_Scripts.end())
		{
			CScript* pScript = (*itScript).second;
			if(pScript)
			{
				pScript->ReleaseChildScript();
				PushFreeScript((*itScript).second);
			}
			g_Scripts.erase(itScript);
		}
	}
	m_DelScripts.clear();
	return;
}


// ------------------------------ 初始化 ------------------------------

///////////////////////
// 读取函数列表

void CScript::LoadFunction(const char *filename, char* pData)
{
	SAFE_DELETE(s_FunctionList2);
    s_FunctionList2 = new STRING2PFUNC;


    //-----------------------------------------------------
	//基本语法函数
    (*s_FunctionList2)["goto"] = &CScript::Goto;
    (*s_FunctionList2)["if"] = &CScript::If;	
	(*s_FunctionList2)["wait"] = &CScript::wait;
	(*s_FunctionList2)["random"] = &CScript::random;
	(*s_FunctionList2)["Int"] = &CScript::Int;
	(*s_FunctionList2)["Round"] = &CScript::RoundNum;
    (*s_FunctionList2)["RGB"] = &CScript::Rgb;
    (*s_FunctionList2)["localvar"] = &CScript::LocalVar;    
    (*s_FunctionList2)["define"] = &CScript::define;
    (*s_FunctionList2)["undefine"] = &CScript::undefine;
    (*s_FunctionList2)["ifdef"] = &CScript::Ifdef;
	 (*s_FunctionList2)["destdefine"] = &CScript::DestDefine;
    (*s_FunctionList2)["destundefine"] = &CScript::DestUndefine;
    (*s_FunctionList2)["ifdestdef"] = &CScript::IfDestDef;
    (*s_FunctionList2)["Time"] = &CScript::Time;
    (*s_FunctionList2)["Year"] = &CScript::Year;
    (*s_FunctionList2)["Month"] = &CScript::Month;
    (*s_FunctionList2)["Day"] = &CScript::Day;
    (*s_FunctionList2)["Hour"] = &CScript::Hour;
    (*s_FunctionList2)["Minute"] = &CScript::Minute;
    (*s_FunctionList2)["DayOfWeek"] = &CScript::DayOfWeek;
    (*s_FunctionList2)["HourDiff"] = &CScript::HourDiff;
    (*s_FunctionList2)["MinuteDiff"] = &CScript::MinuteDiff;
    (*s_FunctionList2)["Second"] = &CScript::Second;
	(*s_FunctionList2)["CheckGuidIsNull"] = &CScript::CheckGuidIsNull;
	(*s_FunctionList2)["GetCurTick"] = &CScript::GetCurTick;
	(*s_FunctionList2)["GetPow"] = &CScript::GetPow;
	(*s_FunctionList2)["GetSubString"] = &CScript::GetSubString;

	//-----------------------------------------------------
	//系统全局函数
	(*s_FunctionList2)["FormatGUID"] = &CScript::FormatGUID;
    (*s_FunctionList2)["ListOnlineGM"] = &CScript::ListOnlineGM;
    (*s_FunctionList2)["ListSilencePlayer"] = &CScript::ListSilencePlayer;
    (*s_FunctionList2)["ListBanedPlayer"] = &CScript::ListBanedPlayer;
    (*s_FunctionList2)["GetOnlinePlayers"] = &CScript::GetOnlinePlayers;
    (*s_FunctionList2)["PostRegionInfo"] = &CScript::PostRegionInfo;
    (*s_FunctionList2)["PostRegionInfoByIndex"] = &CScript::PostRegionInfoByIndex;
    (*s_FunctionList2)["PostWorldInfo"] = &CScript::PostWorldInfo;
    (*s_FunctionList2)["PostWorldInfoByIndex"] = &CScript::PostWorldInfoByIndex;
    (*s_FunctionList2)["PostCountryInfo"] = &CScript::PostCountryInfo;
    (*s_FunctionList2)["KickAllPlayer"] = &CScript::KickAllPlayer;
    (*s_FunctionList2)["KickRegionPlayer"] = &CScript::KickRegionPlayer;
	(*s_FunctionList2)["RegisterGlobalTimer"] = &CScript::RegisterGlobalTimer;
    (*s_FunctionList2)["UnRegisterGlobalTimer"] = &CScript::UnRegisterGlobalTimer;
	(*s_FunctionList2)["KickPlayer"] = &CScript::KickPlayer;
	(*s_FunctionList2)["KickAround"] = &CScript::KickAround;
    (*s_FunctionList2)["BanPlayer"] = &CScript::BanPlayer;
    (*s_FunctionList2)["SilencePlayer"] = &CScript::SilencePlayer;
	(*s_FunctionList2)["GetTextRes"] = &CScript::GetTextRes;
	(*s_FunctionList2)["SetPlayerRegion"] = &CScript::SetPlayerRegion;
	(*s_FunctionList2)["SetPlayerRegionEx"] = &CScript::SetPlayerRegionEx;
	(*s_FunctionList2)["RollRegionInfo"] = &CScript::RollRegionInfo;
	(*s_FunctionList2)["RollWorldInfo"] = &CScript::RollWorldInfo;
	(*s_FunctionList2)["GetPlayerRegion"] = &CScript::GetPlayerRegion;
	(*s_FunctionList2)["ChangePlayerRegion"] = &CScript::ChangePlayerRegion;
	(*s_FunctionList2)["ChangePlayerName"] = &CScript::ChangePlayerName;


	//-----------------------------------------------------
	//所有对象(怪物、采集物、角色)基本使用函数
	(*s_FunctionList2)["SetAttrInt"] = &CScript::SetAttrInt;
	(*s_FunctionList2)["SetAttrStr"] = &CScript::SetAttrStr;
	(*s_FunctionList2)["SetAttrGuid"] = &CScript::SetAttrGuid;
    (*s_FunctionList2)["GetAttr"] = &CScript::GetAttr;
	(*s_FunctionList2)["GetScriptAttr"] = &CScript::GetScriptAttr;
    (*s_FunctionList2)["ChangeAttr"] = &CScript::ChangeAttr;
	(*s_FunctionList2)["RegisterTimer"] = &CScript::RegisterTimer;
    (*s_FunctionList2)["UnRegisterTimer"] = &CScript::UnRegisterTimer;
	(*s_FunctionList2)["PlayMotion"] = &CScript::PlayMotion;
	(*s_FunctionList2)["DesRunScript"] = &CScript::DesRunScript;
	(*s_FunctionList2)["GetPlayerNumByAround"] = &CScript::GetPlayerNumByAround;
    (*s_FunctionList2)["GetMonsterNumByAround"] = &CScript::GetMonsterNumByAround;
    (*s_FunctionList2)["GetSameMonsterNumByAround"] = &CScript::GetSameMonsterNumByAround;
	(*s_FunctionList2)["SetDelTime"] = &CScript::SetDelTime;
	(*s_FunctionList2)["SetDelTimeByGuid"] = &CScript::SetDelTimeByGuid;
	(*s_FunctionList2)["GetSrcShapeGUID"] = &CScript::GetSrcShapeGUID;
	(*s_FunctionList2)["GetDesShapeGUID"] = &CScript::GetDesShapeGUID;
	(*s_FunctionList2)["ChangeMoveSpeed"] = &CScript::ChangeMoveSpeed;
    (*s_FunctionList2)["SetMoveSpeed"] = &CScript::SetMoveSpeed;
	(*s_FunctionList2)["ChangeBaseSpeed"] = &CScript::ChangeBaseSpeed;
	(*s_FunctionList2)["GetCurrentSpeed"] = &CScript::GetCurrentSpeed;

	(*s_FunctionList2)["GetDestIntVar"] = &CScript::GetDestIntVar;
	(*s_FunctionList2)["SetDestIntVar"] = &CScript::SetDestIntVar;
	(*s_FunctionList2)["GetDestStrVar"] = &CScript::GetDestStrVar;
	(*s_FunctionList2)["SetDestStrVar"] = &CScript::SetDestStrVar;
	(*s_FunctionList2)["GetDestGuidVar"] = &CScript::GetDestGuidVar;
	(*s_FunctionList2)["SetDestGuidVar"] = &CScript::SetDestGuidVar;
	(*s_FunctionList2)["IsShapeValid"] = &CScript::IsShapeValid;

	//-----------------------------------------------------
	//怪物相关函数
	//1、基本函数
	(*s_FunctionList2)["SetMonsterAttr"] = &CScript::SetMonsterAttr;
    (*s_FunctionList2)["GetMonsterAttr"] = &CScript::GetMonsterAttr;
    (*s_FunctionList2)["ChangeMonsterAttr"] = &CScript::ChangeMonsterAttr;
	(*s_FunctionList2)["AddHpBar"] = &CScript::AddHpBar;
    (*s_FunctionList2)["GetHpLevel"] = &CScript::GetHpLevel;
	(*s_FunctionList2)["GetKillerID"] = &CScript::GetKillerID;
    (*s_FunctionList2)["GetKillerType"] = &CScript::GetKillerType;
	(*s_FunctionList2)["SetBeneficiary"] = &CScript::SetBeneficiary;
    (*s_FunctionList2)["GetBeneficiaryID"] = &CScript::GetBeneficiaryID;
    (*s_FunctionList2)["GetBeneficiaryType"] = &CScript::GetBeneficiaryType;
	(*s_FunctionList2)["GetBeneTeamGuid"] = &CScript::GetBeneTeamGuid;
	(*s_FunctionList2)["DelRegionMonsterByName"] = &CScript::DelRegionMonsterByName;
	//2、怪物AI相关函数
	(*s_FunctionList2)["MonsterTalk"] = &CScript::MonsterTalk;
    (*s_FunctionList2)["MonsterTalkByIndex"] = &CScript::MonsterTalkByIndex;
    (*s_FunctionList2)["MonsterTalkByGUID"] = &CScript::MonsterTalkByGUID;
    (*s_FunctionList2)["MonsterTalkByGuidByIndex"] = &CScript::MonsterTalkByGuidByIndex;
	(*s_FunctionList2)["UseSkillByID"] = &CScript::UseSkillByID;
	(*s_FunctionList2)["UseSkillByIDOnCell"] = &CScript::UseSkillByIDOnCell;
	(*s_FunctionList2)["UseSkillSync"] = &CScript::UseSkillSync;
	(*s_FunctionList2)["UseSkillSyncWithTrace"] = &CScript::UseSkillSyncWithTrace;
	(*s_FunctionList2)["UseSkillOnCellSync"] = &CScript::UseSkillOnCellSync;
    (*s_FunctionList2)["MoveToCellOnPeace"] = &CScript::MoveToCellOnPeace;
    (*s_FunctionList2)["MoveToCellOnFight"] = &CScript::MoveToCellOnFight;
    (*s_FunctionList2)["MoveToCellAround"] = &CScript::MoveToCellAround;
    (*s_FunctionList2)["MoveByStepOnPeace"] = &CScript::MoveByStepOnPeace;
    (*s_FunctionList2)["MoveByStepOnFight"] = &CScript::MoveByStepOnFight;
    (*s_FunctionList2)["SendExpActOnPeace"] = &CScript::SendExpActOnPeace;
    (*s_FunctionList2)["SendExpActOnFight"] = &CScript::SendExpActOnFight;
    (*s_FunctionList2)["MoveByTimeOnPeace"] = &CScript::MoveByTimeOnPeace;
    (*s_FunctionList2)["MovebyTimeOnFight"] = &CScript::MovebyTimeOnFight;
    (*s_FunctionList2)["CallFriendAround"] = &CScript::CallFriendAround;
    (*s_FunctionList2)["CallMonsterAround"] = &CScript::CallMonsterAround;
    (*s_FunctionList2)["Stand"] = &CScript::Stand;
	(*s_FunctionList2)["StandOnce"] = &CScript::StandOnce;
    (*s_FunctionList2)["ChangeAI"] = &CScript::ChangeAI;
    (*s_FunctionList2)["StartCycle"] = &CScript::StartCycle;
    (*s_FunctionList2)["EndCycle"] = &CScript::EndCycle;
    (*s_FunctionList2)["EscapeByTimeOnFight"] = &CScript::EscapeByTimeOnFight;
    (*s_FunctionList2)["DeleteSpring"] = &CScript::DeleteSpring;
    (*s_FunctionList2)["EndEscape"] = &CScript::EndEscape;
    (*s_FunctionList2)["EndEscapeByGUID"] = &CScript::EndEscapeByGUID;
    (*s_FunctionList2)["SetBefoolID"] = &CScript::SetBefoolID;
    (*s_FunctionList2)["CleanBefoolID"] = &CScript::CleanBefoolID;
    (*s_FunctionList2)["SetForceTarget"] = &CScript::SetForceTarget;
    (*s_FunctionList2)["CleanForceTatget"] = &CScript::CleanForceTatget;
	(*s_FunctionList2)["ChangeWeaknessState"] = &CScript::ChangeWeaknessState;
	(*s_FunctionList2)["EscapeByAround"] = &CScript::EscapeByAround;
    (*s_FunctionList2)["MonsterRunScriptsAround"] = &CScript::MonsterRunScriptsAround;
    (*s_FunctionList2)["MonsterIsFight"] = &CScript::MonsterIsFight;
    (*s_FunctionList2)["SetMonsterFight"] = &CScript::SetMonsterFight;
    (*s_FunctionList2)["CallFriendAttack"] = &CScript::CallFriendAttack;
    (*s_FunctionList2)["CallMonsterAttack"] = &CScript::CallMonsterAttack;
    (*s_FunctionList2)["MonsterAttackObjectByAround"] = &CScript::MonsterAttackObjectByAround;
    (*s_FunctionList2)["MonsterAttackObject"] = &CScript::MonsterAttackObject;
    (*s_FunctionList2)["SetMonsterDir"] = &CScript::SetMonsterDir;
    (*s_FunctionList2)["ReLiveMonster"] = &CScript::ReLiveMonster;
    (*s_FunctionList2)["GetMonsterTarget"] = &CScript::GetMonsterTarget;
    (*s_FunctionList2)["SetMonsterTarget"] = &CScript::SetMonsterTarget;
	(*s_FunctionList2)["GetMonsterPos"] = &CScript::GetMonsterPos;
	(*s_FunctionList2)["CreateMonster"] = &CScript::CreateMonster;
    (*s_FunctionList2)["CreateMonsterEx"] = &CScript::CreateMonsterEx;
    (*s_FunctionList2)["DelMonster"] = &CScript::DelMonster;
    (*s_FunctionList2)["DelMonsterEx"] = &CScript::DelMonsterEx;
	(*s_FunctionList2)["DelMonsterRect"] = &CScript::DelMonsterRect;
	(*s_FunctionList2)["KillMonster"] = &CScript::KillMonster;
	(*s_FunctionList2)["GetMonsterRefeashTime"] = &CScript::GetMonsterRefeashTime;
	(*s_FunctionList2)["GetPlayerByDistance"] = &CScript::GetPlayerByDistance;
	(*s_FunctionList2)["GetDistanceFromPlayer"] = &CScript::GetDistanceFromPlayer;
	(*s_FunctionList2)["GetAttackablePlayer"] = &CScript::GetAttackablePlayer;
	(*s_FunctionList2)["ChooseTargetByOccu"] = &CScript::ChooseTargetByOccu;

	//-----------------------------------------------------
	//采集物相关函数
	(*s_FunctionList2)["CreateCollection"] = &CScript::CreateCollection;
    (*s_FunctionList2)["CreateCollectionEx"] = &CScript::CreateCollectionEx;
    (*s_FunctionList2)["DelCollection"] = &CScript::DelCollection;
	
	//-----------------------------------------------------
	//建物相关函数
	//对门的操作
	(*s_FunctionList2)["SetDoor"] = &CScript::SetDoor;
	(*s_FunctionList2)["GetDoor"] = &CScript::GetDoor;
	//-----------------------------------------------------
	//角色相关函数
	//1.基本函数
	(*s_FunctionList2)["ChangeMe"] = &CScript::ChangeMe;
    (*s_FunctionList2)["GetMe"] = &CScript::GetMe;
    (*s_FunctionList2)["SetMe"] = &CScript::SetMe;
	(*s_FunctionList2)["ChangeDest"] = &CScript::ChangeDest;
    (*s_FunctionList2)["GetDest"] = &CScript::GetDest;
    (*s_FunctionList2)["SetDest"] = &CScript::SetDest;
	(*s_FunctionList2)["SetPlayerPos"] = &CScript::SetPlayerPos;
    (*s_FunctionList2)["SetPlayerDir"] = &CScript::SetPlayerDir;
	(*s_FunctionList2)["AddAreaCreditByAreaId"] = &CScript::AddAreaCreditByAreaId;
    (*s_FunctionList2)["AddAreaCredit"] = &CScript::AddAreaCredit;
    (*s_FunctionList2)["SetAreaCredit"] = &CScript::SetAreaCredit;
    (*s_FunctionList2)["SetAreaCreditByAreaId"] = &CScript::SetAreaCreditByAreaId;
    (*s_FunctionList2)["GetAreaCredit"] = &CScript::GetAreaCredit;
	(*s_FunctionList2)["GetAreaCreditLevel"] = &CScript::GetAreaCreditLevel;
	(*s_FunctionList2)["GetOrganCreditLevel"] = & CScript::GetOrganCreditLevel;
	(*s_FunctionList2)["AddOrganCredit"] = & CScript::AddOrganCredit;
	(*s_FunctionList2)["SetOrganCredit"] = & CScript::SetOrganCredit;
	(*s_FunctionList2)["GetCountry"] = &CScript::GetCountry;
    (*s_FunctionList2)["ChangeCountry"] = &CScript::ChangeCountry;
    (*s_FunctionList2)["GetContribute"] = &CScript::GetContribute;
    (*s_FunctionList2)["SetContribute"] = &CScript::SetContribute;
    (*s_FunctionList2)["IsCharged"] = &CScript::IsCharged;
    (*s_FunctionList2)["SetCharged"] = &CScript::SetCharged;
	(*s_FunctionList2)["GetName"] = &CScript::GetName;
	(*s_FunctionList2)["ChangePlayer"] = &CScript::ChangePlayer;
    (*s_FunctionList2)["SetPlayer"] = &CScript::SetPlayer;
    (*s_FunctionList2)["GetPlayer"] = &CScript::GetPlayer;
    (*s_FunctionList2)["SetPlayerLevel"] = &CScript::SetPlayerLevel;
    (*s_FunctionList2)["IsPlayerOnline"] = &CScript::IsPlayerOnline; 
	(*s_FunctionList2)["ChangeMoneyByName"] = &CScript::ChangeMoneyByName;
    (*s_FunctionList2)["GetMoneyByName"] = &CScript::GetMoneyByName;
    (*s_FunctionList2)["SetMoneyByName"] = &CScript::SetMoneyByName;
    (*s_FunctionList2)["ChangeSilverByName"] = &CScript::ChangeSilverByName;
    (*s_FunctionList2)["GetSilverByName"] = &CScript::GetSilverByName;
    (*s_FunctionList2)["SetSilverByName"] = &CScript::SetSilverByName;
	(*s_FunctionList2)["AddTicketByName"] = &CScript::AddTicketByName;
	(*s_FunctionList2)["GetTicketByName"] = &CScript::GetTicketByName;
	(*s_FunctionList2)["SetPlayerExp"] = &CScript::SetPlayerExp;
	(*s_FunctionList2)["ChangePlayerExp"] = &CScript::ChangePlayerExp;
	(*s_FunctionList2)["SetPlayerOccuExp"] = &CScript::SetPlayerOccuExp;
	(*s_FunctionList2)["ChangePlayerOccuExp"] = &CScript::ChangePlayerOccuExp;
	(*s_FunctionList2)["GetPlayerOccuExp"] = &CScript::GetPlayerOccuExp;
	(*s_FunctionList2)["SetPlayerOccuLvl"] = &CScript::SetPlayerOccuLvl;
	(*s_FunctionList2)["GetPlayerOccuLvl"] = &CScript::GetPlayerOccuLvl;
	(*s_FunctionList2)["SetPlayerOccuSp"] = &CScript::SetPlayerOccuSp;
	(*s_FunctionList2)["ChangePlayerOccuSp"] = &CScript::ChangePlayerOccuSp;
	(*s_FunctionList2)["GetPlayerOccuSp"] = &CScript::GetPlayerOccuSp;
	(*s_FunctionList2)["ChangePlayerOccuExpSp"] = &CScript::ChangePlayerOccuExpSp;
	(*s_FunctionList2)["IsGM"] = &CScript::IsGM;
    (*s_FunctionList2)["ShowPlayerInfo"] = &CScript::ShowPlayerInfo;
    (*s_FunctionList2)["ShowPlayerGoods"] = &CScript::ShowPlayerGoods;
    (*s_FunctionList2)["ShowGeneralVariable"] = &CScript::ShowGeneralVariable;
    (*s_FunctionList2)["DurancePlayer"] = &CScript::DurancePlayer;
	(*s_FunctionList2)["CheckReliveByOrigin"] = &CScript::CheckReliveByOrigin;
	(*s_FunctionList2)["CheckReliveGoods"] = &CScript::CheckReliveGoods;
    (*s_FunctionList2)["Onstate"] = &CScript::Onstate;
	(*s_FunctionList2)["GetActive"] = &CScript::GetActive;
	(*s_FunctionList2)["AddActive"] = &CScript::AddActive;
	(*s_FunctionList2)["TakeActive"] = &CScript::TakeActive;
	(*s_FunctionList2)["ResetActive"] = &CScript::ResetActive;
	(*s_FunctionList2)["GetServer"] = &CScript::GetServer;
	(*s_FunctionList2)["GetOtherOneInMarriageTeam"] = &CScript::GetOtherOneInMarriageTeam;
	(*s_FunctionList2)["GetPlayerMarriageStep"] = &CScript::GetPlayerMarriageStep;
	(*s_FunctionList2)["SetPlayerMarriageStep"] = &CScript::SetPlayerMarriageStep;
	(*s_FunctionList2)["SetPlayerSpouse"] = &CScript::SetPlayerSpouse;
	(*s_FunctionList2)["DelPlayerSpouse"] = &CScript::DelPlayerSpouse;
	(*s_FunctionList2)["GetPlayerSpouse"] = &CScript::GetPlayerSpouse;
	(*s_FunctionList2)["GetPlayerSpouseName"] = &CScript::GetPlayerSpouseName;
	(*s_FunctionList2)["GetElementProperty"] = &CScript::GetElementProperty;
	(*s_FunctionList2)["SetElementProperty"] = &CScript::SetElementProperty;
	(*s_FunctionList2)["SetActiveRatio"] = &CScript::SetActiveRatio;
	(*s_FunctionList2)["GetActiveRatio"] = &CScript::GetActiveRatio;
	(*s_FunctionList2)["GetCostYuanBao"] = &CScript::GetCostYuanBao;
    //稳定挂机时间
    (*s_FunctionList2)["SetStableHookTime"] = &CScript::SetStableHookTime;
    (*s_FunctionList2)["AddStableHookTime"] = &CScript::AddStableHookTime;
    (*s_FunctionList2)["GetStableHookTime"] = &CScript::GetStableHookTime;

	
	

	

	//2.扩展函数
	(*s_FunctionList2)["GetOccuValueRatio"] = &CScript::GetOccuValueRatio;
    (*s_FunctionList2)["GetOccuLvlCoe"] = &CScript::GetOccuLvlCoe;
	(*s_FunctionList2)["ChangeOccu"] = &CScript::ChangeOccu;
	(*s_FunctionList2)["CheckLevel"] = &CScript::CheckLevel;
	(*s_FunctionList2)["CheckOccuLvl"] = &CScript::CheckOccuLvl;
	(*s_FunctionList2)["ReLive"] = &CScript::ReLive;
	(*s_FunctionList2)["ScriptIsRun"] = &CScript::ScriptIsRun;
    (*s_FunctionList2)["RemoveScript"] = &CScript::RemoveScript;
	(*s_FunctionList2)["AutoMove"] = &CScript::AutoMove;
	(*s_FunctionList2)["MovePlayer"] = &CScript::MovePlayer;
	(*s_FunctionList2)["GodMode"] = &CScript::GodMode;
	(*s_FunctionList2)["ResidentMode"] = &CScript::ResidentMode;

	//-----------------------------------------------------
	//物品相关函数
	(*s_FunctionList2)["AddGoods"] = &CScript::AddGoods;
	(*s_FunctionList2)["AddArmament"] = &CScript::AddArmament;
	(*s_FunctionList2)["AddArmamentForGM"] = &CScript::AddArmamentForGM;
	(*s_FunctionList2)["DelGoods"] = &CScript::DelGoods;
	(*s_FunctionList2)["AddGoodsToRgn"] = &CScript::AddGoodsToRgn;
	(*s_FunctionList2)["CheckGoods"] = &CScript::CheckGoods;
	(*s_FunctionList2)["CheckGoodsInBank"] = &CScript::CheckGoodsInBank;
	(*s_FunctionList2)["OpenGoodsExWindow"] = &CScript::OpenGoodsExWindow;
	(*s_FunctionList2)["BindGoods"] = &CScript::BindGoods;
    (*s_FunctionList2)["FrostGoods"] = &CScript::FrostGoods;
	(*s_FunctionList2)["CheckSpace"] = &CScript::CheckSpace;
    (*s_FunctionList2)["GetGoodsNum"] = &CScript::GetGoodsNum;
    (*s_FunctionList2)["GetGoodsFree"] = &CScript::GetGoodsFree;
    (*s_FunctionList2)["GetEquipIDByPos"] = &CScript::GetEquipIDByPos;
    (*s_FunctionList2)["GetEquipNameByPos"] = &CScript::GetEquipNameByPos;
	(*s_FunctionList2)["GetPlayerGoodsAddonProperty"] = &CScript::GetPlayerGoodsAddonProperty;
	(*s_FunctionList2)["SetPlayerGoodsAddonProperty"] = &CScript::SetPlayerGoodsAddonProperty;
	(*s_FunctionList2)["GetGoodsBaseType"] = &CScript::GetGoodsBaseType;
    (*s_FunctionList2)["OpenGoodsEnchase"] = &CScript::OpenGoodsEnchase;
    (*s_FunctionList2)["DelPlayerGoods"] = &CScript::DelPlayerGoods;
    (*s_FunctionList2)["DelUsedGoods"] = &CScript::DelUsedGoods;
    (*s_FunctionList2)["CheckUsedGoods"] = &CScript::CheckUsedGoods;
    (*s_FunctionList2)["GetUsedGoodsPropertiy1"] = &CScript::GetUsedGoodsPropertiy1;
    (*s_FunctionList2)["GetUsedGoodsPropertiy2"] = &CScript::GetUsedGoodsPropertiy2;
    (*s_FunctionList2)["SetUsedGoodsPropertiy1"] = &CScript::SetUsedGoodsPropertiy1;
    (*s_FunctionList2)["SetUsedGoodsPropertiy2"] = &CScript::SetUsedGoodsPropertiy2;
    (*s_FunctionList2)["BuyGoods"] = &CScript::BuyGoods;
	(*s_FunctionList2)["GetGoodsAddonProperty"] = &CScript::GetGoodsAddonProperty;
	(*s_FunctionList2)["FullRefine"] = &CScript::FullRefine;
	(*s_FunctionList2)["GetWeaponStr"] = &CScript::GetWeaponStr;
	(*s_FunctionList2)["AddMedal"] = &CScript::AddMedal;
    (*s_FunctionList2)["DelMedal"] = &CScript::DelMedal;
    (*s_FunctionList2)["GetMedal"] = &CScript::GetMedal;
    (*s_FunctionList2)["GetMedalNum"] = &CScript::GetMedalNum;
	(*s_FunctionList2)["GetShowMedalName"] = &CScript::GetShowMedalName;
	(*s_FunctionList2)["IsEquipEnchased"] = &CScript::IsEquipEnchased;
	(*s_FunctionList2)["GetGoodsDisplayName"] = &CScript::GetGoodsDisplayName;
	(*s_FunctionList2)["BeginMake"] = &CScript::BeginMake;
	(*s_FunctionList2)["FinishMake"] = &CScript::FinishMake;
	(*s_FunctionList2)["BackToCity"] = &CScript::BackToCity;

	(*s_FunctionList2)["BeginContainerGoodsOpt"] = &CScript::BeginContainerGoodsOpt;
	(*s_FunctionList2)["GetOptGoodsInContainer"] = &CScript::GetOptGoodsInContainer;
	(*s_FunctionList2)["PutOptGoodsInContainer"] = &CScript::PutOptGoodsInContainer;
	(*s_FunctionList2)["RemoveOptGoodsInContainer"] = &CScript::RemoveOptGoodsInContainer;
	(*s_FunctionList2)["RefreshContainerTxt"] = &CScript::RefreshContainerTxt;
	(*s_FunctionList2)["RefreshContainerGold"] = &CScript::RefreshContainerGold;
	(*s_FunctionList2)["EndContainerGoodsOpt"] = &CScript::EndContainerGoodsOpt;

	(*s_FunctionList2)["GetGoodsOriginalName"] = &CScript::GetGoodsOriginalName;
	(*s_FunctionList2)["GetGoodsAddonPropertyValues"] = &CScript::GetGoodsAddonPropertyValues;
	(*s_FunctionList2)["SetGoodsAddonPropertyValues"] = &CScript::SetGoodsAddonPropertyValues;
	(*s_FunctionList2)["ResetGoodsStochastic"] = &CScript::ResetGoodsStochastic;
	(*s_FunctionList2)["DelGoodsByGuid"] = &CScript::DelGoodsByGuid;
	(*s_FunctionList2)["AddGoodsReturnGuid"] = &CScript::AddGoodsReturnGuid;
	(*s_FunctionList2)["IsFrostGoods"] = &CScript::IsFrostGoods;
	(*s_FunctionList2)["GetUsedGoodsOriginalName"] = &CScript::GetUsedGoodsOriginalName;
	(*s_FunctionList2)["GetUsedGoodsGuid"] = &CScript::GetUsedGoodsGuid;
    (*s_FunctionList2)["UploadGoods"] = &CScript::UploadGoods;
	(*s_FunctionList2)["SetAddonDesc"] = &CScript::SetAddonDesc;
	(*s_FunctionList2)["GetAddonDesc"] = &CScript::GetAddonDesc;
	(*s_FunctionList2)["AddGoodsToServer"] = &CScript::AddGoodsToServer;
	(*s_FunctionList2)["AddGoodsToClient"] = &CScript::AddGoodsToClient;
	

	//-----------------------------------------------------
	//任务相关函数
	(*s_FunctionList2)["AddQuest"] = &CScript::AddQuest;
	(*s_FunctionList2)["GetCurrentMasterRoleID"] = &CScript::GetCurrentMasterRoleID;
    (*s_FunctionList2)["CompleteQuest"] = &CScript::CompleteQuest;
    (*s_FunctionList2)["DisbandQuest"] = &CScript::DisbandQuest;
    (*s_FunctionList2)["GetQuestState"] = &CScript::GetQuestState;
	(*s_FunctionList2)["GetValidQuestNum"] = &CScript::GetValidQuestNum;
    (*s_FunctionList2)["UpdateQuestStep"] = &CScript::UpdateQuestStep;
    (*s_FunctionList2)["UpdateQuestParamData"] = &CScript::UpdateQuestParamData;
    (*s_FunctionList2)["UpdateQuestData"] = &CScript::UpdateQuestData;
	(*s_FunctionList2)["GetMercQuest"] = &CScript::GetMercQuest;
    (*s_FunctionList2)["SetMercQuest"] = &CScript::SetMercQuest;
    (*s_FunctionList2)["GetRoleStepNum"] = &CScript::GetRoleStepNum;
    (*s_FunctionList2)["GetParam"] = &CScript::GetParam;
    (*s_FunctionList2)["GetRoleAccParam"] = &CScript::GetRoleAccParam;
    (*s_FunctionList2)["SetRoleAccParam"] = &CScript::SetRoleAccParam;
    (*s_FunctionList2)["AddRoleAccParam"] = &CScript::AddRoleAccParam;
    (*s_FunctionList2)["GetRoleRegionID"] = &CScript::GetRoleRegionID;
    (*s_FunctionList2)["SetRoleRegionID"] = &CScript::SetRoleRegionID;
    (*s_FunctionList2)["GetRoleRegionX"] = &CScript::GetRoleRegionX;
    (*s_FunctionList2)["SetRoleRegionX"] = &CScript::SetRoleRegionX;
    (*s_FunctionList2)["GetRoleRegionY"] = &CScript::GetRoleRegionY;
    (*s_FunctionList2)["SetRoleRegionY"] = &CScript::SetRoleRegionY;
    (*s_FunctionList2)["GetRoleSum"] = &CScript::GetRoleSum;
    (*s_FunctionList2)["GetChapter"] = &CScript::GetChapter;
    (*s_FunctionList2)["SetChapter"] = &CScript::SetChapter;
    (*s_FunctionList2)["AddFinishedRoleRecord"] = &CScript::AddFinishedRoleRecord;
    (*s_FunctionList2)["DelFinishedRoleRecord"] = &CScript::DelFinishedRoleRecord;
    (*s_FunctionList2)["GetRoleFinishedRecord"] = &CScript::GetRoleFinishedRecord;
    (*s_FunctionList2)["GetStepText"] = &CScript::GetStepText;
    (*s_FunctionList2)["GetText"] = &CScript::GetText;
    (*s_FunctionList2)["GetTextType"] = &CScript::GetTextType;
    (*s_FunctionList2)["GetAttributeRoleName"] = &CScript::GetAttributeRoleName;
    (*s_FunctionList2)["GetAttributeBeRecord"] = &CScript::GetAttributeBeRecord;
    (*s_FunctionList2)["GetAttributeRoleType"] = &CScript::GetAttributeRoleType;
    (*s_FunctionList2)["GetAttributeEmbracerNPCID"] = &CScript::GetAttributeEmbracerNPCID;
    (*s_FunctionList2)["GetAttributeShowAble"] = &CScript::GetAttributeShowAble;
	(*s_FunctionList2)["GetAttributeExpRatio"] = &CScript::GetAttributeExpRatio;
    (*s_FunctionList2)["GetAttributeRepeated"] = &CScript::GetAttributeRepeated;
    (*s_FunctionList2)["GetAttributeDeadLost"] = &CScript::GetAttributeDeadLost;
    (*s_FunctionList2)["GetAttributeLostRepeated"] = &CScript::GetAttributeLostRepeated;
    (*s_FunctionList2)["GetAttributeMinGrade"] = &CScript::GetAttributeMinGrade;
	(*s_FunctionList2)["GetAttributeMaxGrade"] = &CScript::GetAttributeMaxGrade;
    (*s_FunctionList2)["GetAttributeOccupationRestrict"] = &CScript::GetAttributeOccupationRestrict;
    (*s_FunctionList2)["GetAttributeEngageArmsRepute"] = &CScript::GetAttributeEngageArmsRepute;
    (*s_FunctionList2)["GetAttributeChapterNum"] = &CScript::GetAttributeChapterNum;
	(*s_FunctionList2)["GetAttributeNextRoleID"] = &CScript::GetAttributeNextRoleID;
    (*s_FunctionList2)["GetLabelPrefix"] = &CScript::GetLabelPrefix;
    (*s_FunctionList2)["GetLabelSum"] = &CScript::GetLabelSum;
    (*s_FunctionList2)["GetAimAMTType"] = &CScript::GetAimAMTType;
    (*s_FunctionList2)["GetAimType"] = &CScript::GetAimType;
    (*s_FunctionList2)["GetAimId"] = &CScript::GetAimId;
    (*s_FunctionList2)["GetAimNum"] = &CScript::GetAimNum;
    (*s_FunctionList2)["GetAimParameIndex"] = &CScript::GetAimParameIndex;
    (*s_FunctionList2)["GetDropFrom"] = &CScript::GetDropFrom;
    (*s_FunctionList2)["GetDropOdds"] = &CScript::GetDropOdds;
    (*s_FunctionList2)["GetAimName"] = &CScript::GetAimName;
    (*s_FunctionList2)["GetPrepareParam"] = &CScript::GetPrepareParam;
    (*s_FunctionList2)["GetStepEncouragementSum"] = &CScript::GetStepEncouragementSum;
    (*s_FunctionList2)["GetQuestEncouragementSum"] = &CScript::GetQuestEncouragementSum;
    (*s_FunctionList2)["GetSetpEncouragementName"] = &CScript::GetSetpEncouragementName;
    (*s_FunctionList2)["GetQuestEncouragementName"] = &CScript::GetQuestEncouragementName;
    (*s_FunctionList2)["GetSetpEncouragementType"] = &CScript::GetSetpEncouragementType;
    (*s_FunctionList2)["GetQuestEncouragementType"] = &CScript::GetQuestEncouragementType;
    (*s_FunctionList2)["GetSetpEncouragementID"] = &CScript::GetSetpEncouragementID;
    (*s_FunctionList2)["GetQuestEncouragementID"] = &CScript::GetQuestEncouragementID;
    (*s_FunctionList2)["GetSetpEncouragementNum"] = &CScript::GetSetpEncouragementNum;
    (*s_FunctionList2)["GetQuestEncouragementNum"] = &CScript::GetQuestEncouragementNum;
    (*s_FunctionList2)["GetStepEncouragementCell"] = &CScript::GetStepEncouragementCell;
    (*s_FunctionList2)["GetQuestEncouragementCell"] = &CScript::GetQuestEncouragementCell;
    (*s_FunctionList2)["GetStepSum"] = &CScript::GetStepSum;
    (*s_FunctionList2)["GetStepAimNum"] = &CScript::GetStepAimNum;
    (*s_FunctionList2)["FindRoleIsInSystem"] = &CScript::FindRoleIsInSystem;
    (*s_FunctionList2)["FindRoleStepIsInSystem"] = &CScript::FindRoleStepIsInSystem;
    (*s_FunctionList2)["ResetMercQuestUpdateTime"] = &CScript::ResetMercQuestUpdateTime;
    (*s_FunctionList2)["AcquireMercQuest"] = &CScript::AcquireMercQuest;
    (*s_FunctionList2)["SendMercQuestToClient"] = &CScript::SendMercQuestToClient;
    (*s_FunctionList2)["OpenMercQuestInterface"] = &CScript::OpenMercQuestInterface;
    (*s_FunctionList2)["GetMercQuestData"] = &CScript::GetMercQuestData;
    (*s_FunctionList2)["SetMercQuestData"] = &CScript::SetMercQuestData;
    (*s_FunctionList2)["GetQuestMassPoint"] = &CScript::GetQuestMassPoint;
    (*s_FunctionList2)["UpdateAllMercQuestList"] = &CScript::ResetAllMercQuestData;
    (*s_FunctionList2)["IsInMercQuestList"] = &CScript::IsInMercQuestList;
	(*s_FunctionList2)["SetMasterQuest"] = &CScript::SetMasterQuest;
	(*s_FunctionList2)["SetRoleProtect"] = &CScript::SetRoleProtect;
	(*s_FunctionList2)["OpenShareQuestInterface"] = &CScript::OpenShareQuestInterface;
	(*s_FunctionList2)["GetTeamCurrQuest"] = &CScript::GetTeamCurrQuest;
	(*s_FunctionList2)["SetTeamCurrQuest"] = &CScript::SetTeamCurrQuest;
	(*s_FunctionList2)["DelTeamShareQuest"] = &CScript::DelTeamShareQuest;
	(*s_FunctionList2)["IsCompleteQuest"] = &CScript::IsCompleteQuest;
	(*s_FunctionList2)["GetQuestOdds"] = &CScript::GetQuestOdds;
	(*s_FunctionList2)["CanAcceptMercQuest"] = &CScript::CanAcceptMercQuest;
	(*s_FunctionList2)["OpenIndexQuestInterface"] = &CScript::OpenIndexQuestInterface;
	(*s_FunctionList2)["SetIndexQuestParamNum"] = &CScript::SetIndexQuestParamNum;
	(*s_FunctionList2)["SetIndexQuestState"] = &CScript::SetIndexQuestState;
	(*s_FunctionList2)["ResetIndexQuest"] = &CScript::ResetIndexQuest;

	//-----------------------------------------------------
	//副职业相关函数  
	(*s_FunctionList2)["CanCollect"] = &CScript::CanCollect;
    (*s_FunctionList2)["CanCheckCollect"] = &CScript::CanCheckCollect;
    (*s_FunctionList2)["CheckCollect"] = &CScript::CheckCollect;
	(*s_FunctionList2)["LockCollection"] = &CScript::LockCollection;
    (*s_FunctionList2)["OutlockCollection"] = &CScript::OutlockCollection;
    (*s_FunctionList2)["CanFacture"] = &CScript::CanFacture;
    (*s_FunctionList2)["Facture"] = &CScript::Facture;
    (*s_FunctionList2)["FactureSkillStudyBegin"] = &CScript::FactureSkillStudyBegin;
    (*s_FunctionList2)["ClearFactureList"] = &CScript::ClearFactureList;
	(*s_FunctionList2)["AddFacture"] = &CScript::AddFacture;
	(*s_FunctionList2)["DelFacture"] = &CScript::DelFacture;
	(*s_FunctionList2)["FindFacture"] = &CScript::FindFacture;
	(*s_FunctionList2)["GetFactureNeedsLevel"] = &CScript::GetFactureNeedsLevel;
	(*s_FunctionList2)["GetFactureNeedsDOccu"] = &CScript::GetFactureNeedsDOccu;
	(*s_FunctionList2)["UpdateEffect"] = &CScript::UpdateEffect;
	(*s_FunctionList2)["AddFactureExp"] = &CScript::AddFactureExp;
	(*s_FunctionList2)["AddCollectionExp"] = &CScript::AddCollectionExp;
	//-----------------------------------------------------
	//技能状态相关函数
	//1、基本技能相关的函数
	(*s_FunctionList2)["AddState"] = &CScript::AddState;       
	(*s_FunctionList2)["AddSkill"] = &CScript::AddSkill;
    (*s_FunctionList2)["DelSkill"] = &CScript::DelSkill;
    (*s_FunctionList2)["SetSkillLevel"] = &CScript::SetSkillLevel;
    (*s_FunctionList2)["GetSkillLevel"] = &CScript::GetSkillLevel;
	(*s_FunctionList2)["CheckStudySkill"] = &CScript::CheckStudySkill;	
	(*s_FunctionList2)["GetAllStateNum"] = &CScript::GetAllStateNum;
	(*s_FunctionList2)["DelNewState"] = &CScript::DelNewState;
	(*s_FunctionList2)["DelDeBuff"] = &CScript::DelDeBuff;
	(*s_FunctionList2)["AddStateToShape"] = &CScript::AddStateToShape;
	(*s_FunctionList2)["DelAllBuff"] = &CScript::DelAllBuff;
	(*s_FunctionList2)["Modulus"] = &CScript::Modulus;
	
	//2、技能脚本中用到的函数
	
	(*s_FunctionList2)["AddNewState"] = &CScript::AddNewState;
	(*s_FunctionList2)["RemoverNewState"] = &CScript::RemoverNewState;	
	(*s_FunctionList2)["SetMoveShapeAttrEx"] = &CScript::SetMoveShapeAttrEx;
    (*s_FunctionList2)["GetMoveShapeAttrEx"] = &CScript::GetMoveShapeAttrEx;
    (*s_FunctionList2)["ChangeMoveShapeAttrEx"] = &CScript::ChangeMoveShapeAttrEx;
	(*s_FunctionList2)["AddDeBuffToObj"] = &CScript::AddDeBuffToObj;
    (*s_FunctionList2)["ReLiveSkill"] = &CScript::ReLiveSkill;
    (*s_FunctionList2)["SetEventDestObj"] = &CScript::SetEventDestObj;
    (*s_FunctionList2)["GetBaseModuleLv"] = &CScript::GetBaseModuleLv;
	(*s_FunctionList2)["GetDistance"] = &CScript::GetDistance;
    (*s_FunctionList2)["GetPkResult"] = &CScript::GetPkResult;
	(*s_FunctionList2)["GetObjectType"] = &CScript::GetObjectType;    
    (*s_FunctionList2)["GetShieldNO"] = &CScript::GetShieldNO;
    (*s_FunctionList2)["SendSkillList"] = &CScript::SendSkillList;
	(*s_FunctionList2)["SetTarget"] = &CScript::SetTarget;
	(*s_FunctionList2)["GetDesEnergyRatio"] = &CScript::GetDesEnergyRatio;
	(*s_FunctionList2)["GetEnergyRatio"] = &CScript::GetEnergyRatio;
    (*s_FunctionList2)["DeductEnergy"] = &CScript::DeductEnergy;
    (*s_FunctionList2)["GetSkillParam"] = &CScript::GetSkillParam;
	(*s_FunctionList2)["GetSkillElement"] = &CScript::GetSkillElement;
	(*s_FunctionList2)["GetSkillUseType"] = &CScript::GetSkillUseType;
    (*s_FunctionList2)["GetStateParamByID"] = &CScript::GetStateParamByID;
    (*s_FunctionList2)["GetStateParamByIndex"] = &CScript::GetStateParamByIndex;
	(*s_FunctionList2)["CheckStateFirer"] = &CScript::CheckStateFirer;
    (*s_FunctionList2)["BeedAttackEvent"] = &CScript::BeedAttackEvent;
    (*s_FunctionList2)["GetSkillLv"] = &CScript::GetSkillLv;
    (*s_FunctionList2)["ReLiveInScript"] = &CScript::ReLiveInScript;
    (*s_FunctionList2)["BeginUseSkill"] = &CScript::BeginUseSkill;
	(*s_FunctionList2)["GetGuidByParam"] = &CScript::GetGuidByParam;
    (*s_FunctionList2)["RunScriptByDest"] = &CScript::RunScriptByDest;
	(*s_FunctionList2)["GetGoodsAddProperty"] = &CScript::GetGoodsAddProperty;	
	(*s_FunctionList2)["GetGoodsPlacePos"] = &CScript::GetGoodsPlacePos;	
	(*s_FunctionList2)["DelGoodsByPlacePos"] = &CScript::DelGoodsByPlacePos;	
	(*s_FunctionList2)["SucceedUseItem"] = &CScript::SucceedUseItem;
	(*s_FunctionList2)["GetActModify"] = &CScript::GetActModify;
	(*s_FunctionList2)["SendUnLockGoodsMsg"] = &CScript::SendUnLockGoodsMsg;	
	(*s_FunctionList2)["GetBeneficiaryInfo"] = &CScript::GetBeneficiaryInfo;		
	(*s_FunctionList2)["GetKillerInfo"] = &CScript::GetKillerInfo;	
	(*s_FunctionList2)["GetModType"] = &CScript::GetModType;	
	(*s_FunctionList2)["GetAttackerType"] = &CScript::GetAttackerType;
	(*s_FunctionList2)["GetFunMoHua"] = &CScript::GetFunMoHua;
	(*s_FunctionList2)["GetBModifier"] = &CScript::GetBModifier;
	(*s_FunctionList2)["GetPrevent"] = &CScript::GetPrevent;
	(*s_FunctionList2)["GetPartDamageOut"] = &CScript::GetPartDamageOut;
	(*s_FunctionList2)["OnCostYuanBao"] = &CScript::OnCostYuanBao;
	
	
	//-----------------------------------------------------
	//定义了和客户端交互，客户端可视相关的脚本函数
	(*s_FunctionList2)["AddInfo"] = &CScript::AddInfo;
	(*s_FunctionList2)["AddDestInfo"] = &CScript::AddDestInfo;
    (*s_FunctionList2)["AddInfoByIndex"] = &CScript::AddInfoByIndex;
    (*s_FunctionList2)["GameMessage"] = &CScript::GameMessage;
    (*s_FunctionList2)["TalkBox"] = &CScript::TalkBox;
    (*s_FunctionList2)["PicTalkBox"] = &CScript::PicTalkBox;
	(*s_FunctionList2)["EndTalkBox"] = &CScript::EndTalkBox;
	(*s_FunctionList2)["EndPixyTalkBox"] = &CScript::EndPixyTalkBox;
    (*s_FunctionList2)["TalkBoxByIndex"] = &CScript::TalkBoxByIndex;
    (*s_FunctionList2)["StoryTalkBox"] = &CScript::StoryTalkBox;
    (*s_FunctionList2)["StoryTalkBoxByIndex"] = &CScript::StoryTalkBoxByIndex;
	(*s_FunctionList2)["PixyTalkBox"] = &CScript::PixyTalkBox;
	(*s_FunctionList2)["PixyTalkBoxByIndex"] = &CScript::PixyTalkBoxByIndex;
    (*s_FunctionList2)["PlayEffectByIndex"] = &CScript::PlayEffectByIndex;
    (*s_FunctionList2)["PlayerTalk"] = &CScript::PlayerTalk;
    (*s_FunctionList2)["PlayerTalkByIndex"] = &CScript::PlayerTalkByIndex;
    (*s_FunctionList2)["PackGlint"] = &CScript::PackGlint;
	(*s_FunctionList2)["PostPlayerInfo"] = &CScript::PostPlayerInfo;
	(*s_FunctionList2)["PostDestPlayerInfo"] = &CScript::PostDestPlayerInfo;
    (*s_FunctionList2)["PostPlayerInfoByIndex"] = &CScript::PostPlayerInfoByIndex;
	(*s_FunctionList2)["PostDestPlayerInfoByIndex"] = &CScript::PostDestPlayerInfoByIndex;
    (*s_FunctionList2)["ShowString"] = &CScript::ShowString;
	(*s_FunctionList2)["Weather"] = &CScript::Weather;
    (*s_FunctionList2)["PlayEffect"] = &CScript::PlayEffect;
    (*s_FunctionList2)["PlayAction"] = &CScript::PlayAction;
	(*s_FunctionList2)["PlaySound"] = &CScript::PlaySound;
	(*s_FunctionList2)["Play3DSound"] = &CScript::Play3DSound;
	(*s_FunctionList2)["PlayBkMusic"] = &CScript::PlayBkMusic;
	(*s_FunctionList2)["StopBkMusic"] = &CScript::StopBkMusic;
    (*s_FunctionList2)["AddGoodToShortCutBar"] = &CScript::AddGoodToShortCutBar;
    (*s_FunctionList2)["AddSkillToShortCutBar"] = &CScript::AddSkillToShortCutBar;
    (*s_FunctionList2)["TransforCamera"] = &CScript::TransforCamera;
    (*s_FunctionList2)["OperaInterface"] = &CScript::OperaInterface;
    (*s_FunctionList2)["HelpHint"] = &CScript::HelpHint;
    (*s_FunctionList2)["InterfaceHint"] = &CScript::InterfaceHint;
	(*s_FunctionList2)["PlayVideo"] = &CScript::PlayVideo;
    (*s_FunctionList2)["StopVideo"] = &CScript::StopVideo;
	(*s_FunctionList2)["UpdateScriptVarValue"] = &CScript::UpdateScriptVarValue;
	(*s_FunctionList2)["SendExpAction"] = &CScript::SendExpAction;
	(*s_FunctionList2)["GetPlayerAllProperties"] = &CScript::GetPlayerAllProperties;
    (*s_FunctionList2)["GetPlayerAllVariables"] = &CScript::GetPlayerAllVariables;
	(*s_FunctionList2)["ViewPlayerAllVariableList"] = &CScript::ViewPlayerAllVariableList;
	(*s_FunctionList2)["OpenReliveWindow"] = &CScript::OpenReliveWindow;
	(*s_FunctionList2)["OpenReLiveByGoodsWindow"] = &CScript::OpenReLiveByGoodsWindow;




	//-----------------------------------------------------
	//队伍帮会组织相关函数
	//1、队伍函数
	(*s_FunctionList2)["GetTeamNum"] = &CScript::GetTeamNum;
	(*s_FunctionList2)["GetTeamNumInRgn"] = &CScript::GetTeamNumInRgn;
    (*s_FunctionList2)["GetTeamerName"] = &CScript::GetTeamerName;
    (*s_FunctionList2)["GetTeamGUID"] = &CScript::GetTeamGUID;
    (*s_FunctionList2)["GetPlayerTeamGUID"] = &CScript::GetPlayerTeamGUID;
    (*s_FunctionList2)["IsTeamCaptain"] = &CScript::IsTeamCaptain;
    (*s_FunctionList2)["CheckRgnOnlyTeam"] = &CScript::CheckRgnOnlyTeam;
    (*s_FunctionList2)["GetTeamCaptainPos"] = &CScript::GetTeamCaptainPos;   
    (*s_FunctionList2)["SetRegionForTeam"] = &CScript::SetRegionForTeam;
    (*s_FunctionList2)["SetTeamRegion"] = &CScript::SetTeamRegion;
    (*s_FunctionList2)["IsTeammatesAroundMe"] = &CScript::IsTeammatesAroundMe;
    (*s_FunctionList2)["InLvlDifference"] = &CScript::InLvlDifference;
    (*s_FunctionList2)["InLvlSpace"] = &CScript::InLvlSpace;
	(*s_FunctionList2)["RunScriptForTeam"] = &CScript::RunScriptForTeam;
	//2.帮会组织
	(*s_FunctionList2)["CreateFaction"] = &CScript::CreateFaction;
	(*s_FunctionList2)["CreateUnion"] = &CScript::CreateUnion;
    (*s_FunctionList2)["OpenApplyBoard"] = &CScript::OpenApplyBoard;
    (*s_FunctionList2)["OpenFacIconBoard"] = &CScript::OpenFacIconBoard;
    (*s_FunctionList2)["SetFactionRecruitState"] = &CScript::SetFactionRecruitState;
    (*s_FunctionList2)["GetFactionLevel"] = &CScript::GetFactionLevel;
    (*s_FunctionList2)["GetFactionIDByPlayerName"] = &CScript::GetFactionIDByPlayerName;
    (*s_FunctionList2)["GetUinionIDByPlayerName"] = &CScript::GetUinionIDByPlayerName;
	(*s_FunctionList2)["AddFacRes"] = &CScript::AddFacRes;
	(*s_FunctionList2)["AddFacExp"] = &CScript::AddFacExp;
	(*s_FunctionList2)["AddFacMemberContribute"] = &CScript::AddFacMemberContribute;
	(*s_FunctionList2)["GetMemContribute"]=&CScript::GetMemContribute;
	(*s_FunctionList2)["GetFacRes"]=&CScript::GetFacRes;
	(*s_FunctionList2)["GetFacExp"]=&CScript::GetFacExp;
    (*s_FunctionList2)["IsFacMaster"] = &CScript::IsFacMaster;
	(*s_FunctionList2)["AddFacTickets"]=&CScript::AddFacTickets;
	(*s_FunctionList2)["AddFacBaseBattle"]=&CScript::AddFacBaseBattle;
	(*s_FunctionList2)["ChangeBaseInfo"]=&CScript::ChangeBaseInfo;
	(*s_FunctionList2)["AddBattleVal"]=&CScript::AddBattleVal;
	(*s_FunctionList2)["AddArmyInvest"]=&CScript::AddArmyInvest;
	(*s_FunctionList2)["GetFacTickets"]=&CScript::GetFacTickets;
	(*s_FunctionList2)["GetBattleVal"]=&CScript::GetBattleVal;
	(*s_FunctionList2)["GetArmyInvest"]=&CScript::GetArmyInvest;
	(*s_FunctionList2)["GetJoinBattle"]=&CScript::GetJoinBattle;
	(*s_FunctionList2)["AddTech"]=&CScript::AddTech;
	(*s_FunctionList2)["GetTechLv"]=&CScript::GetTechLv;
	(*s_FunctionList2)["GetFactionName"]=&CScript::GetFactionName;
	(*s_FunctionList2)["ChangeEnemy"]=&CScript::ChangeEnemy;
	(*s_FunctionList2)["GetFacBaseBattle"]=&CScript::GetFacBaseBattle;
	(*s_FunctionList2)["SendBaseInfo"]=&CScript::SendBaseInfo;
	(*s_FunctionList2)["GetFacLevel"]=&CScript::GetFacLevel;
	(*s_FunctionList2)["FacDisplay"]=&CScript::FacDisplay;
	//队伍帮会组织相关函数
	//1、场景相关函数
	(*s_FunctionList2)["GetRgnAttr"] = &CScript::GetRgnAttr; 
	(*s_FunctionList2)["SetRgnAttr"] = &CScript::SetRgnAttr; 
	(*s_FunctionList2)["GetCellType"] = &CScript::GetCellType; 
	(*s_FunctionList2)["RegionIsExit"] = &CScript::RegionIsExit; 
	(*s_FunctionList2)["GetRegionID"] = &CScript::GetRegionID;         
    (*s_FunctionList2)["GetMapInfo"] = &CScript::GetMapInfo;
    (*s_FunctionList2)["GetCountryOwningRegion"] = &CScript::GetCountryOwningRegion;
    (*s_FunctionList2)["SetBlock"] = &CScript::SetBlock;
    (*s_FunctionList2)["GetRegionRandomPos"] = &CScript::GetRegionRandomPos;
    (*s_FunctionList2)["GetRandomPosInRange"] = &CScript::GetRandomPosInRange;
	(*s_FunctionList2)["GetLineDir"] = &CScript::GetLineDir;
	(*s_FunctionList2)["GetNextPosByDir"] = &CScript::GetNextPosByDir;
    (*s_FunctionList2)["GetRegionPlayerNum"] = &CScript::GetRegionPlayerNum;
    (*s_FunctionList2)["RunScriptForAllPlayerInRgn"] = &CScript::RunScriptForAllPlayerInRgn;
	//1、副本相关函数
	(*s_FunctionList2)["CreatePRgn"] = &CScript::CreatePRgn;
    (*s_FunctionList2)["CreatePHRgn"] = &CScript::CreatePHRgn;
    (*s_FunctionList2)["CreateTRgn"] = &CScript::CreateTRgn;
    (*s_FunctionList2)["GetPlayerPDupRgnID"] = &CScript::GetPlayerPDupRgnID;
    (*s_FunctionList2)["GetPlayerPHDupRgnID"] = &CScript::GetPlayerPHDupRgnID;
    (*s_FunctionList2)["GetTeamDupRgnID"] = &CScript::GetTeamDupRgnID;
    (*s_FunctionList2)["GetNormalDupRgnID"] = &CScript::GetNormalDupRgnID;
    (*s_FunctionList2)["ChangeRgn"] = &CScript::ChangeRgnByTemplateID;
    (*s_FunctionList2)["ChangeDupRgn"] = &CScript::ChangeDupRgn;
    (*s_FunctionList2)["AddRgnVar"] = &CScript::AddRgnVar;
    (*s_FunctionList2)["SetRgnVar"] = &CScript::SetRgnVar;
    (*s_FunctionList2)["DelRgnVar"] = &CScript::DelRgnVar;
    (*s_FunctionList2)["GetRgnVar"] = &CScript::GetRgnVar;
    (*s_FunctionList2)["DeleteRgn"] = &CScript::DeleteRgn;
    (*s_FunctionList2)["GetTemplateIDByGUID"] = &CScript::GetTemplateIDByGUID;
    (*s_FunctionList2)["GetCurRgnGUID"] = &CScript::GetCurRgnGUID;
    (*s_FunctionList2)["GetDupTypeByGUID"] = &CScript::GetDupTypeByGUID;
    (*s_FunctionList2)["GetDupRgnOnwerGUID"] = &CScript::GetDupRgnOnwerGUID;
    (*s_FunctionList2)["CreatePHGoods"] = &CScript::CreatePHGoods;
    (*s_FunctionList2)["DeletePHGoods"] = &CScript::DeletePHGoods;
    (*s_FunctionList2)["GetDupRgnMaxNum"] = &CScript::GetDupRgnMaxNum;
    (*s_FunctionList2)["GetDupRgnGUIDByOnwerGUID"] = &CScript::GetPHRgnGUIDByOnwerName;
    (*s_FunctionList2)["DeleteMonster"] = &CScript::DeleteMonster;
    (*s_FunctionList2)["CreateDupRgnEntity"] = &CScript::CreateDupRgnEntity;
    (*s_FunctionList2)["RunDupRgnScript"] = &CScript::RunDupRgnScript;
    (*s_FunctionList2)["GetDupRgnNum"] = &CScript::GetDupRgnNum;
    (*s_FunctionList2)["GetDupRgnMaxNum"] = &CScript::GetDupRgnMaxNum;
    (*s_FunctionList2)["SetDupRgnOperFlag"] = &CScript::SetDupRgnOperFlag;
    (*s_FunctionList2)["GetTeamMaterIDByPos"] = &CScript::GetTeamMaterIDByPos;
    (*s_FunctionList2)["EditDupRgnName"] = &CScript::EditDupRgnName;
    (*s_FunctionList2)["GetPlayerGUID"] = &CScript::GetPlayerGUID;
	(*s_FunctionList2)["GetDestGUID"] = &CScript::GetDestGUID;
    (*s_FunctionList2)["GetCurPlayerPDupRgnID"] = &CScript::GetCurPlayerPDupRgnID;
    (*s_FunctionList2)["GetCurPlayerPHDupRgnID"] = &CScript::GetCurPlayerPHDupRgnID;

    (*s_FunctionList2)["GetLocalGSID"] = &CScript::GetLocalGSID;
    (*s_FunctionList2)["GetGUIDByPlayerName"] = &CScript::GetGUIDByPlayerName;
    (*s_FunctionList2)["GetPGUIDByPlayerName"] = &CScript::GetPGUIDByPlayerName;
    (*s_FunctionList2)["GetPHGUIDByPlayerName"] = &CScript::GetPHGUIDByPlayerName;
    (*s_FunctionList2)["GetTGUIDByPlayerName"] = &CScript::GetTGUIDByPlayerName;
    (*s_FunctionList2)["ChangeDupRgnByRgnID"] = &CScript::ChangeDupRgnByRgnID;
    (*s_FunctionList2)["SetPlayerPDupRgnID"] = &CScript::SetPlayerPDupRgnID;
    (*s_FunctionList2)["SetPlayerPHDupRgnID"] = &CScript::SetPlayerPHDupRgnID;
    (*s_FunctionList2)["AddTeamRgnID"] = &CScript::AddTeamRgnID;
	(*s_FunctionList2)["DelTeamRgnID"] = &CScript::DelTeamRgnID;
	(*s_FunctionList2)["ClearTeamRgnID"] = &CScript::ClearTeamRgnID;
	(*s_FunctionList2)["SwapTeamRgnID"] = &CScript::SwapTeamRgnID;
	(*s_FunctionList2)["GetTeamRgnIDNum"] = &CScript::GetTeamRgnIDNum;
	(*s_FunctionList2)["GetTeamRgnIDByIndex"] = &CScript::GetTeamRgnIDByIndex;
	(*s_FunctionList2)["GetTeamRgnTemplatIDByIndex"] = &CScript::GetTeamRgnTemplatIDByIndex;
    (*s_FunctionList2)["ViewRgnVar"] = &CScript::ViewRgnVar;
    (*s_FunctionList2)["EvaluateGUID"] = &CScript::EvaluateGUID;
    (*s_FunctionList2)["ViewRgnValidByGUID"] = &CScript::ViewRgnValidByGUID;
    (*s_FunctionList2)["GetDupRgnPlayerMaxNum"] = &CScript::GetDupRgnPlayerMaxNum;
    (*s_FunctionList2)["GetDupRgnOperFlag"] = &CScript::GetDupRgnOperFlag; 
    (*s_FunctionList2)["GetCallerGUID"] = &CScript::GetCallerGUID;
	(*s_FunctionList2)["ViewDupRgnData"] = &CScript::ViewDupRgnData;
	(*s_FunctionList2)["CallScriptByGUID"] = &CScript::CallScriptByGUID;    
    (*s_FunctionList2)["ClearCurGSDupRgn"] = &CScript::ClearCurGSDupRgn;
	(*s_FunctionList2)["ReFreshPlayerDupRgnList"] = &CScript::ReFreshPlayerDupRgnList;
	(*s_FunctionList2)["SetRgnStrongPointState"] = &CScript::SetRgnStrongPointState;
	(*s_FunctionList2)["GetRgnStrongPointState"] = &CScript::GetRgnStrongPointState;
	(*s_FunctionList2)["GetRegionCampPlayerNum"] = &CScript::GetRegionCampPlayerNum;

	//其他的脚本相关函数
	//1、国家相关函数
	//............................................
	//2.战斗系统相关函数
	(*s_FunctionList2)["SentaiQueue"] = &CScript::SentaiQueue;
	(*s_FunctionList2)["GetPlayerSentaiPoints"] = &CScript::GetPlayerSentaiPoints;
	(*s_FunctionList2)["ChangePlayerSentaiPoints"] = &CScript::ChangePlayerSentaiPoints;
	(*s_FunctionList2)["GetTeamSentaiPoints"] = &CScript::GetTeamSentaiPoints;
	(*s_FunctionList2)["EndMatch"] = &CScript::EndMatch;
	(*s_FunctionList2)["EndTeamMatch"] = &CScript::EndTeamMatch;
	(*s_FunctionList2)["IsSentaiSys"] = &CScript::IsSentaiSys;

	//............................................
	//3.跑商系统相关函数    
	(*s_FunctionList2)["OpenBusiness"] = &CScript::OpenBusiness;
	(*s_FunctionList2)["IsBusinessState"] = &CScript::IsBusinessState;
	(*s_FunctionList2)["UseHiggleSkill"] = &CScript::UseHiggleSkill;
	(*s_FunctionList2)["UseCheckingSkill"] = &CScript::UseCheckingSkill;
    (*s_FunctionList2)["UseChafferSkill"] = &CScript::UseChafferSkill;
    (*s_FunctionList2)["OpenInvest"] = &CScript::OpenInvest;
	(*s_FunctionList2)["RemoveBusinessState"] = &CScript::RemoveBusinessState;
	(*s_FunctionList2)["OpenFBusiness"] = &CScript::OpenFBusiness;

	//4.邮件系统函数
	(*s_FunctionList2)["SendSysMail"] = &CScript::SendSysMail;
	(*s_FunctionList2)["ReceiveSysMail"] = &CScript::ReceiveSysMail;
	(*s_FunctionList2)["ReceiveMail"] = &CScript::ReceiveMail;
	//5.奖励相关
	(*s_FunctionList2)["OpenBouns"] = &CScript::OpenBouns;
	

	// Fox@20090104 宠物
	(*s_FunctionList2)["CreatePet"] = &CScript::CreatePet;
	(*s_FunctionList2)["DelPet"] = &CScript::DelPet;
	(*s_FunctionList2)["RelivePet"] = &CScript::RelivePet;
	(*s_FunctionList2)["AddPetSkill"] = &CScript::AddPetSkill;
	(*s_FunctionList2)["GetFocusPet"] = &CScript::GetFocusPet;

	// 反外挂相关函数
	(*s_FunctionList2)["ChangeActivity"] = &CScript::ChangeActivity;
	(*s_FunctionList2)["ACKickPlayer"] = &CScript::ACKickPlayer;
	(*s_FunctionList2)["StopCheckAC"] = &CScript::StopCheckAC;
	(*s_FunctionList2)["GetACAttr"] = &CScript::GetACAttr;
	(*s_FunctionList2)["StartCheackAC"] =&CScript::StartCheackAC;

	// 隐匿功能相关函数
	(*s_FunctionList2)["GetRegionHideFlagById"] = &CScript::GetRegionHideFlagById;
	(*s_FunctionList2)["GetPlayerCanHideByGuid"] = &CScript::GetPlayerCanHideByGuid;
	(*s_FunctionList2)["SetPlayerCanHideByGuid"] = &CScript::SetPlayerCanHideByGuid;
	(*s_FunctionList2)["GetPlayerHideFlagByGuid"] = &CScript::GetPlayerHideFlagByGuid;
	(*s_FunctionList2)["SetPlayerHideFlagByGuid"] = &CScript::SetPlayerHideFlagByGuid;
	(*s_FunctionList2)["GetHideName"] = &CScript::GetHideName;
	(*s_FunctionList2)["GMViewHideNameAround"] = &CScript::GMViewHideNameAround;
	//GM脚本接口
	(*s_FunctionList2)["GMAddSkill"] = &CScript::GMAddSkill;
	(*s_FunctionList2)["GMDelSkill"] = &CScript::GMDelSkill;
	// 定时器功能相关函数
	(*s_FunctionList2)["SetTimer"] = &CScript::SetTimer;
	(*s_FunctionList2)["CloseTimer"] = &CScript::CloseTimer;
	(*s_FunctionList2)["GetPlayerTimer"] = &CScript::GetPlayerTimer;
	(*s_FunctionList2)["GetTimer"] = &CScript::GetTimer;
	//彩票系统
	(*s_FunctionList2)["OpenLotteryPage"] = &CScript::OpenLotteryPage;
	(*s_FunctionList2)["DoLottery"] = &CScript::DoLottery;
	(*s_FunctionList2)["SetWorldCredit"] = &CScript::SetWorldCredit;
	(*s_FunctionList2)["GetPersonalCredit"] = &CScript::GetPersonalCredit;
	(*s_FunctionList2)["SetCurHighestCredit"] = &CScript::SetCurHighestCredit;
	(*s_FunctionList2)["GetPersonalCreditTotal"] = &CScript::GetPersonalCreditTotal;
	(*s_FunctionList2)["SetWorldCreditVelue"] = &CScript::SetWorldCreditVelue;
	(*s_FunctionList2)["GetWorldCreditMax"] = &CScript::GetWorldCreditMax;
	(*s_FunctionList2)["SendWorldCredit"] = &CScript::SendWorldCredit;
	(*s_FunctionList2)["SendLotteryChat"] = &CScript::SendLotteryChat;
	(*s_FunctionList2)["GetLotteryRe"] = &CScript::GetLotteryRe;
	//获得玩家的升级经验
	(*s_FunctionList2)["GetNextTotalExp"] = &CScript::GetNextTotalExp;

    // 结婚日志
    (*s_FunctionList2)["MarriageLog"] = &CScript::MarriageLog;
}

////////////////////////
// 释放函数列表

void CScript::ReleaseFunction()
{
    if(s_FunctionList2)
        s_FunctionList2->clear();

    SAFE_DELETE(s_FunctionList2);
}

///////////////////////
// 读入全局变量列表
void CScript::LoadGeneralVariable(BYTE* pByte,long lPos)
{
    ReleaseGeneralVariable();
    m_GeneralVariableList = MP_NEW CVariableList;
    GetGeneralVariableList()->DecordFromByteArray(pByte,lPos, false/*非玩家脚本*/);
}

void CScript::IniBuffSkillVariableList()
{
    m_BuffSkillVariableList = MP_NEW CVariableList;
}

////////////////////////
// 释放全局变量列表
void CScript::ReleaseGeneralVariable()
{
    MP_DELETE(m_GeneralVariableList);
}

// ------------------------------ 工具 ------------------------------

/////////////////////
// 转到下一行
int CScript::GotoNextLine()
{
    char tmp=m_Data[m_Point];
    while( 1 ) 
    {
        m_Point++;
        tmp=m_Data[m_Point];

        if (tmp == '\n' || tmp == '\r')
        {
            m_Point++;
            break;
        }
        if( tmp == EOF || tmp == '\0') break;
    }

    m_Line++;
    m_Word=0;
    return m_Point;
}

///////////////////////
// 跳到指定位置
int CScript::JumpTo(const char *Prompt)
{
    int lp=m_Point;	//保存现场
    m_Point=0;
    char CmdStr[MAX_VAR_LEN];
    while(1)
    {
        ReadCmd( CmdStr );	//读一行指令

        if( CmdStr[strlen(CmdStr)-2] == ':' )	//返回
        {
            int p=(int)strlen(CmdStr);
            CmdStr[p-1] = 0;
            CmdStr[p-2] = 0;
            if( strcmp( CmdStr, Prompt ) == 0 )
            {
                return m_Point;
            }
        }

        if( m_Point >= m_DataLen-1 )
        {
            char str[10240];
            _snprintf(str, 10240, "Prompt <%s> in file %s is not found!", Prompt, m_FileName);
            PutDebugString(str);

            m_Point=lp;
            return 0;
        }
    }
}

///////////////////////
// 跳到下一块 { }
int CScript::JumpToNextBlock()
{
    int lp=m_Point;	//保存现场
    char CmdStr[MAX_VAR_LEN]="";
    int tmp=0;

    ReadCmd( CmdStr );
    if( CmdStr[0] != '{' )
    {
        return m_Point;
    }

    m_Point=lp;
    while(1)
    {
        ReadCmd( CmdStr );	//读一行指令


        if( CmdStr[0] == '{' )	//
        {
            tmp++;
        }

        if( CmdStr[0] == '}' )	//
        {
            tmp--;

            // 块结束

            if( tmp == 0 )
            {
                lp=m_Point;
                ReadCmd( CmdStr );
                if( stricmp( CmdStr, "else;" ) != 0 )
                    m_Point = lp;
                return m_Point;
            }
        }

        if( m_Point >= m_DataLen-1 )
        {
            m_Point = m_DataLen-1;
            return 0;
        }
    }
}

// ----------------------------------------------------------------------------
// 语句处理

//-----------------------------------------------------------------------------

////////////////////////
// 读入一个脚本
bool CScript::LoadScript(const char *filename, char* pData)
{
    // 非文件读入和文件读入
    if( pData )
    {
        M_FREE( m_Data, sizeof(char)*(m_DataLen+1) );

        m_DataLen=(int)strlen(pData);
        m_Data=(char*)M_ALLOC(sizeof(char)*(m_DataLen+1));
        memcpy(m_Data, pData, m_DataLen);
        m_Data[m_DataLen] = 0;
        //初始化指针
        m_Point=0;				//当前指针
        m_Line=m_Word=0;		//当前行列
        m_bHang = false;
		if(filename)
			strcpy_s(m_FileName, MAX_PATH, filename);
        return true;
    }

    //文件读入
    else if(filename != NULL)
    {
        strcpy_s(m_FileName, MAX_PATH, filename);
        strlwr(m_FileName);
         M_FREE( m_Data, sizeof(char)*(m_DataLen+1) );

        //复制内存
        {
            FILE *fp;
            if( (fp=fopen(m_FileName, "rb")) == NULL )
            {
                return false;
            }

            m_DataLen=GetFileLength(m_FileName);	//文件长度
            m_Data=(char*)M_ALLOC(sizeof(char)*(m_DataLen+1));
            fread(m_Data, m_DataLen, 1, fp);
            m_Data[m_DataLen] = 0;
            fclose(fp);
        }

        //初始化指针
        m_Point=0;				//当前指针
        m_Line=m_Word=0;		//当前行列
        m_bHang=false;

        return true;
    }

    return false;
}
bool CScript::ReadForCmd(long& beginPos, long& endPos)	//读入for循环体内的全部脚本
{
    //临时变量
    char chr;
    beginPos = -1;
    long blockNum = 0;
    for(int i=m_Point; i<m_DataLen; i++)
    {
        chr=m_Data[i];

        //注释 "//... && /*....*/"
        if( m_Data[i] == '/' && m_Data[i+1] == '/' )
        {
            m_Point++;
            i=GotoNextLine()-1;
            continue;
        }

        if( m_Data[i] == '/' && m_Data[i+1] == '*' )
        {
            i++;
            while( 1 )
            {
                i++;
                if( m_Data[i] == '*' && m_Data[i+1] == '/' )
                {
                    i+=2;
                    chr=m_Data[i];
                    break;
                }
                if( i>=m_DataLen-1 ) 
                {
                    m_Point=m_DataLen-1;				
                    return false;
                }
            }
        }

        //程序块 { ... }
        //指令开始
        if( chr == '{' )
        {
            for(int j=i; j<m_DataLen; j++)
            {
                chr=m_Data[j];

                //注释 "//... && /*....*/"
                if( m_Data[j] == '/' && m_Data[j+1] == '/' )
                {
                    m_Point++;
                    j=GotoNextLine()-1;
                    continue;
                }
                if( m_Data[j] == '/' && m_Data[j+1] == '*' )
                {
                    j++;
                    while( 1 )
                    {
                        j++;
                        if( m_Data[j] == '*' && m_Data[j+1] == '/' )
                        {
                            j+=2;
                            chr=m_Data[j];
                            break;
                        }
                        if( j>=m_DataLen-1 ) 
                        {
                            m_Point=m_DataLen-1;				
                            return false;
                        }
                    }
                }

                if(chr == '{')
                {
                    if(beginPos == -1) beginPos = j;
                    blockNum++;
                }

                if( chr == '}' )	//指令结束
                {
                    m_Point=j;
                    endPos = m_Point;
                    blockNum--;
                    if(blockNum == 0)
                        return true;	//返回
                    else
                    {
                        m_Point++;
                        continue;
                    }
                }
                m_Point++;
            }

            m_Point=m_DataLen-1;			
        }
    }

    m_Point=m_DataLen-1;
    return false;
}
/////////////////////
// 读取一行指令

bool CScript::ReadCmd(char *Ret, int MaxLen)
{
    //临时变量
    register char chr;

    for(int i=m_Point; i<m_DataLen; i++)
    {
        chr=m_Data[i];

        //注释 "//... && /*....*/"
        if( m_Data[i] == '/' && m_Data[i+1] == '/' )
        {
            m_Point++;
            i=GotoNextLine()-1;
            continue;
        }
        if( m_Data[i] == '/' && m_Data[i+1] == '*' )
        {
            i++;
            while( 1 )
            {
                i++;
                if( m_Data[i] == '*' && m_Data[i+1] == '/' )
                {
                    i+=2;
                    chr=m_Data[i];
                    break;
                }
                if( i>=m_DataLen-1 ) 
                {
                    m_Point=m_DataLen-1;					
                    strcpy_s(Ret, MAX_VAR_LEN, "");					
                    return false;
                }
            }
        }

        //程序块 { ... }
        //if( chr == '{' ) m_Block++;
        //if( chr == '}' ) m_Block--;

        //指令开始
        if( chr == '{' || chr == '}' || chr == '<' || chr == '>' || chr == '#' || chr == '$' || chr == '@' || chr >= 'a' && chr <= 'z' || chr >= 'A' && chr <= 'Z' )
        {
            int m=0;
            bool bInString = false;
            for(int j=i; j<m_DataLen; j++)
            {
                chr=m_Data[j];

                if ( chr == '\"' ) bInString=!bInString;

                if( bInString == false && 
                    (chr == ';' || chr == '\t' || chr == '\n' || chr == '\r' ))	//指令结束
                {
                    m_Point=j;
                    Ret[m]=';';
                    Ret[m+1]=0;
                    return true;	//返回
                }

                // 跳过字符串提行后的制表符
                if (bInString && (chr == '\n' || chr == '\r'))
                {
                    int n = j+1;
                    while(m_Data[n] == '\t')
                    {
                        n++;
                    }
                    j = n-1;
                }

                Ret[m]=chr;
                m++;

                if( m >= MaxLen )	//指令超过长度
                {
                    m_Point=j;
                    Ret[m]=';';
                    Ret[m+1]=0;

                    char str[10240];
                    _snprintf(str, 10240, "Script Script Command '%s' is too long", Ret);
                    PutDebugString(str);
                    return true;
                }
            }

            m_Point=m_DataLen-1;			
            strcpy_s(Ret, MAX_VAR_LEN, "");		
        }
    }

    m_Point=m_DataLen-1;
    strcpy_s(Ret, MAX_VAR_LEN, "");
    return false;
}

///////////////////////
// 运行一行脚本
void CScript::RunLine(const char *cmdline)
{

    memset(m_Operation, 0, MAX_VAR * sizeof(int));
    memset(m_Value, 0, MAX_VAR * sizeof(double));
    for (int i=0;i<MAX_VAR;i++)
	{
		M_FREE(m_strValueInfo[i].pStrValue, m_strValueInfo[i].lStrValueLen);
		m_strValueInfo[i].pStrValue = NULL;
		m_strValueInfo[i].lStrValueLen = 0;
	}

    m_VarNum = 0;
    Check(cmdline);			//分解

    //求出所有变量的值
    for(int i=0; i<m_VarNum; i++)
    {
        ComputeVar(i);

        //如果是IF语句

        if( m_Var[i].length() > 1 && m_Var[i][0]=='i' && m_Var[i][1]=='f' && m_Value[i]==(int)false )
        {
            if( m_VarNum == 1 )
            {
                JumpToNextBlock();	// 跳到下一块{ } 
                break;
            }
            else
                break;
        }

        //如果是Next语句直接跳过程序块

        if( m_Var[i] == "else" )
        {
            JumpToNextBlock();	// 跳到下一块{ } 
            break;
        }

    }

    //计算

    Count();
}

/////////////////////
// 分解

void CScript::Check(const char *cmdline)
{

    int oper_num=0, str_num=0, now=0;	//(0=变量 1=操作符);
    int nBracket=0, nFountion=0;		//括号匹配

    register char tmp;
	const long lVarLen = 3072;
    char strVar[lVarLen]="";
    bool bArray=false;	//是否在数组型变量中
    bool bInString=false;	//是否在字符串中
    int arrayNum = 0; // 数组括号个数'['

    int j=0;
    while( cmdline[j]==' ' || cmdline[j] == '\t' )	{j++;}

	long lCmdLineLen = strlen(cmdline);  
    for(unsigned int i=j; i<lCmdLineLen; i++)
    {
		if(str_num>= lVarLen)
		{
			char pszInfo[2048]="";
			_snprintf(pszInfo,2048,"脚本变量名长度超过%d(cmdline:%s)",lVarLen,cmdline);
			AddLogText(pszInfo);
			break;
		}

        tmp=cmdline[i];

        if (tmp=='\"')
            bInString=!bInString;
        if( bInString ){
            strVar[str_num++]=tmp;
            continue;
        }


        if( now==0 )	//变量
        {
            if( tmp == ';' )	//完成
            {
				strVar[str_num]='\0';
                m_Var[oper_num] = strVar;
                if( strVar[0] == '$' )	m_VarClass[oper_num]=VC_VAR;	//变量
                else if( strVar[0] == '#' )	m_VarClass[oper_num]=VC_STRING;	//字符串
                else if( strVar[0] == '\"' )
                {
                    m_VarClass[oper_num]=VC_CONST;	//字符串常量
                    m_Var[oper_num] = strVar;
                }
                else if(strVar[0] == '@')   //GUID变量
                    m_VarClass[oper_num]=VC_GUID;
                else m_VarClass[oper_num]=VC_CONST;					//常量

                m_Operation[oper_num]=1;
                m_VarNum=oper_num+1;
                return;
            }

            //小括号
            if( nBracket == 0 && tmp == '(' && str_num == 0 )
            {
                nBracket++;
            }
            else if( nBracket > 0 )
            {
                if( tmp == '(' ) nBracket++;
                if( tmp == ')' ) nBracket--;

                if( nBracket == 0 )
                {
                    strVar[str_num++]=tmp;
                    now=1;
					strVar[str_num]='\0';
                    m_Var[oper_num] = strVar;
                    m_VarClass[oper_num]=VC_BRACKET;
                    str_num=0;
                    i++;
                    while( cmdline[i]==' ' || cmdline[i] == '\t' )	{i++;}
                    i--;
                    continue;
                }
                else
                {
                    strVar[str_num++]=tmp;
                    continue;
                }
            }

            //函数型变量
            if( nFountion == 0 && tmp == '(' && str_num > 0 && !bArray )
            {
                nFountion++;
            }
            else if( nFountion > 0 )
            {
                if( tmp == '(' ) nFountion++;
                else if( tmp == ')' ) nFountion--;
                if( nFountion ==0 )
                {
                    strVar[str_num++]=tmp;
					strVar[str_num]='\0';
                    now=1;
                    m_Var[oper_num] = strVar;
                    m_VarClass[oper_num]=VC_FUNCTION;
                    str_num=0;
                    i++;
                    while( cmdline[i]==' ' || cmdline[i] == '\t' )	{i++;}
                    i--;
                    continue;
                }
                else
                {
                    strVar[str_num++]=tmp;
                    continue;
                }
            }

            if( tmp == '[' )
            {
                bArray=true;
                arrayNum++;
            }
            else if( tmp == ']' )
            {
                arrayNum--;
                strVar[str_num++]=tmp;	
            }

            if( arrayNum == 0 && bArray == true) // 到了最后一个匹配的']'
            {
                now=1;
                bArray=false;

				strVar[str_num]='\0';
                m_Var[oper_num] = strVar;
                m_VarClass[oper_num]=VC_ARRAY;
                str_num=0;
                i++;
                while( cmdline[i]==' ' || cmdline[i] == '\t' )	{i++;}
                i--;
                continue;
            }


            //普通变量和常量
            if( !bArray && ((IsOperation(tmp) && str_num >0 )|| tmp==' ' || tmp == '\t' ) )
            {
                now=1;
				strVar[str_num]='\0';
                m_Var[oper_num] = strVar;
                if( strVar[0] == '$' )	m_VarClass[oper_num]=VC_VAR;	//变量
                else if( strVar[0] == '#' )	m_VarClass[oper_num]=VC_STRING;	//字符串
                else if( strVar[0] == '\"' )
                {
                    m_VarClass[oper_num]=VC_CONST;	//字符串常量
                    m_Var[oper_num] = strVar;
                }
                else if(strVar[0] == '@')   //GUID变量
                    m_VarClass[oper_num]=VC_GUID;
                else	
                    m_VarClass[oper_num]=VC_CONST;					//常量
                str_num=0;
                while( cmdline[i]==' ' || cmdline[i] == '\t' )	{i++;}
                i--;
            }
            else
            {
                strVar[str_num++]=tmp;
            }
        }
        else if( now==1 )	//操作符
        {
            if( tmp == ';' )	//完成
            {
                m_Operation[oper_num]=1;
                m_VarNum=oper_num+1;
                return;
            }

            if( IsChar(tmp) || tmp=='\"' || tmp==' ' || tmp == '\t' || tmp == '(' || ( tmp=='-' && str_num > 0 ))
            {
				strVar[str_num]='\0';
                now=0;
                str_num=0;
                m_Operation[oper_num] = OperationNum(strVar);

                if( m_Operation[oper_num] == -1 )
                {
                    char str[10240];
                    _snprintf(str, 10240, "\r\n脚本语法错误: %s", cmdline);
                    PutDebugString(str);

                    m_Operation[oper_num] = 1;	// 付值为 ';'
                }
                while( cmdline[i]==' ' || tmp == '\t' )	{i++;}
                oper_num++;
                i--;
            }
            else
            {
                strVar[str_num++]=tmp;
            }
        }
    }
}

/////////////////////////
// 计算一个变量的值
bool CScript::ComputeVar(int num)
{
    if( num > m_VarNum )
    {
        AddLogText("欲求的变量超出范围");
        return false;
    }

    if( m_Operation[num] == 10 )	//如果该变量后面是 '=' (赋值)就直接返回，后面再进行操作
    {
        return true;
    }

    switch( m_VarClass[num] )
    {
    case VC_CONST:		//常量（直接转换）
        if (VC_STRING==GetValueType(m_Var[num].c_str()))
        {
            int pos=DumpString(m_strValueInfo[num].pStrValue, m_strValueInfo[num].lStrValueLen, &m_Var[num][1]);
			m_strValueInfo[num].lStrValueLen = pos+1;
            (m_strValueInfo[num].pStrValue)[pos-1]=0;
        }
        else
        {
            m_Value[num]=atof(m_Var[num].c_str());
        }
        break;

    case VC_VAR:		//变量（查表求值）
        {
            m_Value[num] = GetScriptVarValue((CMoveShape*)p_SrcShape, m_Var[num].c_str(),0);
        }
        break;

    case VC_ARRAY:		//数组（查表求值）
		{
			// 数组下标值
			int lArray = CVariableList::VAR_NOT_FOUND;
			char MainVarName[256];
			char InnVarName[256];

			char* MainVarNamePtr = MainVarName;
			char* InnVarNamePtr = InnVarName;
			char* varTypeFlag = (char*)m_Var[num].c_str();
			long tempPos = 0;

			double ret = CheckParamStringForArray(varTypeFlag, strlen(varTypeFlag), &MainVarNamePtr, 256, &InnVarNamePtr, 256, tempPos);
			if(ret == 1.0f) // 是数组
			{
				if(InnVarNamePtr[0] == '$') // 下标是变量
					lArray = GetScriptVarValue((CMoveShape*)p_SrcShape, InnVarName, 0);
				else
					lArray = atoi(InnVarName);
				if(varTypeFlag[0] == '$') // 整数数组
				{
					if(lArray != CVariableList::VAR_NOT_FOUND)
						m_Value[num]=GetScriptVarValue( (CMoveShape*)p_SrcShape, MainVarName, lArray );
				}
				else if(varTypeFlag[0] == '#') // 字符串数组
				{
					if(lArray != CVariableList::VAR_NOT_FOUND)
					{
						// 找到对应字符串
						char arrayStr[256];
						_snprintf(arrayStr, 256, "%s%d", MainVarName, lArray);
						const char* strPtr = GetScriptVarValue((CMoveShape*)p_SrcShape, arrayStr);
						long lLen = DumpString(m_strValueInfo[num].pStrValue, m_strValueInfo[num].lStrValueLen, strPtr);
						m_strValueInfo[num].lStrValueLen = lLen+1;
					}
				}
				else if(varTypeFlag[0] == '@')
				{
					if(lArray != CVariableList::VAR_NOT_FOUND)
					{
						// 找到对应字符串
						char arrayStr[256];
						_snprintf(arrayStr, 256, "%s%d", MainVarName, lArray);
						m_guidValue[num] = GetScriptGuidValue((CMoveShape*)p_SrcShape, arrayStr);
					}
				}
			}
			else
			{
				char outStr[1024];
				_snprintf(outStr, 1024, "computvar() array oper Err, var[%s]!", m_Var[num].c_str());
				AddLogText(outStr);
			}
		}
        break;
    case VC_STRING:		//字符串
        {
                const char* strPtr = GetScriptVarValue((CMoveShape*)p_SrcShape, m_Var[num].c_str());
                long lLen = DumpString(m_strValueInfo[num].pStrValue, m_strValueInfo[num].lStrValueLen, strPtr);
				m_strValueInfo[num].lStrValueLen = lLen+1;
            }
        break;


    case VC_FUNCTION:	//函数（运行该语句并返回值）
        {
                // 返回字符串
                if (num>0 && m_Operation[num-1]==10 && m_VarClass[num-1]==VC_STRING)
                {
                    char* pstr = NULL;
					long lLen = 0;
                    RunFunction(m_Var[num].c_str(), pstr);
                    if (!pstr)
                        lLen = DumpString(m_strValueInfo[num].pStrValue, m_strValueInfo[num].lStrValueLen, "");
                    else
                        lLen = DumpString(m_strValueInfo[num].pStrValue, m_strValueInfo[num].lStrValueLen, pstr);
					m_strValueInfo[num].lStrValueLen = lLen+1;
                    break;
                }

                m_Value[num]=RunFunction( m_Var[num].c_str() );
            
        }
        break;

    case VC_BRACKET:	//小括号（递归）
        {
            m_Var[num][0]=' ';
            m_Var[num][m_Var[num].length()-1]=';';

            CScript *pTemptScript = PopFreeScript(m_Var[num].c_str());
            InitChildScriptData(pTemptScript);

            //设置变量	
            pTemptScript->SetVariableList(m_pVariableList);
            pTemptScript->RunLine(m_Var[num].c_str());	
            m_Value[num] = pTemptScript->m_Value[0];
            PushFreeScript(pTemptScript);
        }  
        break;
    case VC_GUID:
        {
                m_guidValue[num] = GetScriptGuidValue((CMoveShape*)p_SrcShape, m_Var[num].c_str());
            }
        break;
    }

    return true;
}

/////////////////////
// 是否是操作符

bool CScript::IsOperation(char chr)
{
    for(int i=0; i<MAX_OPERATION; i++)
    {
        if( chr == m_OperationList[i].Name[0] )
            return true;
    }
    return false;
}

///////////////////////
// 获取操作符编号
int CScript::OperationNum(const char *string)
{
    for(int i=0; i<MAX_OPERATION; i++)
    {
        if( strcmp( string,  m_OperationList[i].Name ) == 0 )
            return i;
    }
    return -1;
}

/////////////////////
// 是否是变量
bool CScript::IsChar(char chr)
{
    if( chr == '$' || chr == '#' || chr >= 'a' && chr <= 'z' || chr >='A' && chr <= 'Z' || chr >='0' && chr <= '9' )
        return true;

    return false;
}

//////////////////////
// 找出最高优先级
int CScript::FindHighest()
{
    int level=-1, ret=0;
    for(int i=0; i<m_VarNum; i++)
    {
        if( level < m_OperationList[ m_Operation[i] ].Level )
        {
            level=m_OperationList[ m_Operation[i] ].Level;
            ret=i;
        }
    }
    return ret;
}

////////////////////////
// 前一个可用的变量
int CScript::Prew(int num)
{
    for(int i=num; i>=0; i--)
    {
        if( m_Value[i] != ERROR_CODE )
        {
            return i;
        }
    }
    return 0;
}

//前一个可用的字符串变量
int CScript::PrewString(int num)
{
    for(int i=num; i>=0; i--)
    {
        if( m_strValueInfo[i].pStrValue != 0 )
        {
            return i;
        }
    }
    return -1;
}

//////////////////
// 计算出结果

int CScript::Count()
{
    int n, p;
    if( m_VarNum <=0 ) return 0;







    /*
    0	"", 0,
    1	";",1,
    2	"||",2,
    3	"&&",3,
    4	"==",4, "!=",4, ">",4,  "<",4,  ">=",4, "<=",4,
    10	"=",5,  "+=",5, "-=",5, "*=",5, "/=",5, "%=",5,
    16	"+",6,  "-",6,
    18	"*",7,  "/",7,  "%",7
    */

    while(1)
    {
        n=FindHighest();
        switch( m_Operation[n] )
        {
        case 0:
        case 1:		// ';'	结束

            return m_Value[0];

        case 2:		// '||'
            p=Prew(n);
            m_Value[p] = m_Value[p] || m_Value[n+1];
            m_Value[n+1]=ERROR_CODE;
            m_Operation[n]=0;
            break;

        case 3:		// '&&'
            p=Prew(n);
            m_Value[p] = m_Value[p] && m_Value[n+1];
            m_Value[n+1]=ERROR_CODE;
            m_Operation[n]=0;
            break;

        case 4:	// '=='
            p=Prew(n);
            if ( m_VarClass[p]==VC_STRING || (m_Var[p][0] == '#' && m_VarClass[p] == VC_ARRAY) )
            {
				if(m_strValueInfo[p].pStrValue && m_strValueInfo[n+1].pStrValue)
                m_Value[p] = ( strcmp(m_strValueInfo[p].pStrValue, m_strValueInfo[n+1].pStrValue)==0)?true:false;
				else
					m_Value[p] = false;
            }
            else if(m_VarClass[p] == VC_GUID || (m_Var[n][0] == '@' && m_VarClass[n] == VC_ARRAY))
            {
                    m_Value[p] = ( m_guidValue[p] == m_guidValue[n+1] )?true:false;
                }
            else // 其它类型
            {
                if(VC_STRING == GetValueType(m_Var[p].c_str()))
                {
					if(m_strValueInfo[p].pStrValue && m_strValueInfo[n+1].pStrValue)
                    m_Value[p] = ( strcmp(m_strValueInfo[p].pStrValue,m_strValueInfo[n+1].pStrValue)==0)?true:false;
					else
						m_Value[p] = false;
                }
                else
                {
                    if( (m_Value[p] - m_Value[n+1]) >= -0.0001f && (m_Value[p] - m_Value[n+1]) <= 0.0001f )
                        m_Value[p] = true;
                    else
                        m_Value[p] = false;
                }
            }

            m_Value[n+1]=ERROR_CODE;
            m_Operation[n]=0;
            break;

        case 5:	// '!='
            p=Prew(n);
            if ( m_VarClass[p]==VC_STRING || (m_Var[p][0] == '#' && m_VarClass[p] == VC_ARRAY) )
            {
                if(m_strValueInfo[p].pStrValue && m_strValueInfo[n+1].pStrValue)
                m_Value[p] = ( strcmp(m_strValueInfo[p].pStrValue, m_strValueInfo[n+1].pStrValue)==0)?false:true;
				else
					m_Value[p] = false;
            }
            else if(m_VarClass[p] == VC_GUID || (m_Var[n][0] == '@' && m_VarClass[n] == VC_ARRAY))
            {
                    m_Value[p] = ( m_guidValue[p] == m_guidValue[n+1] )?false:true;
                }
            else
            {
                if(VC_STRING == GetValueType(m_Var[p].c_str()))
                {
					if(m_strValueInfo[p].pStrValue && m_strValueInfo[n+1].pStrValue)
                    m_Value[p] = ( strcmp(m_strValueInfo[p].pStrValue, m_strValueInfo[n+1].pStrValue)==0)?false:true;
					else
						m_Value[p] = false;
                }
                else
                {
                    if( fabs(m_Value[p] - m_Value[n+1]) >= 0.00001f )
                        m_Value[p] = true;
                    else
                        m_Value[p] = false;
                }
            }

            m_Value[n+1]=ERROR_CODE;
            m_Operation[n]=0;
            break;

        case 6:	// '>'
            p=Prew(n);
            if(m_VarClass[p] == VC_GUID)
            {
                TestLog(CStringReading::LoadString(IDS_GS_SCRIPT,STR_GS_SCRIPT_CANTOPERWITHGUIDVAR), ">");
                break;
            }
            if( (m_Value[p] - m_Value[n+1]) > 0 )
                m_Value[p] = true;
            else
                m_Value[p] = false;

            m_Value[n+1]=ERROR_CODE;
            m_Operation[n]=0;
            break;

        case 7:	// '<'
            p=Prew(n);
            if(m_VarClass[p] == VC_GUID)
            {
                TestLog(CStringReading::LoadString(IDS_GS_SCRIPT,STR_GS_SCRIPT_CANTOPERWITHGUIDVAR), "<");
                break;
            }
            
            if( (m_Value[p] - m_Value[n+1]) < 0 )
                m_Value[p] = true;
            else
                m_Value[p] = false;

            m_Value[n+1]=ERROR_CODE;
            m_Operation[n]=0;
            break;

        case 8:	// '>='
            p=Prew(n);
            if(m_VarClass[p] == VC_GUID)
            {
                TestLog(CStringReading::LoadString(IDS_GS_SCRIPT,STR_GS_SCRIPT_CANTOPERWITHGUIDVAR), ">=");
                break;
            }
            
            if( ((m_Value[p] - m_Value[n+1]) > 0) || (fabs(m_Value[p] - m_Value[n+1]) < 0.0001f) )
                m_Value[p] = true;
            else
                m_Value[p] = false;

            m_Value[n+1]=ERROR_CODE;
            m_Operation[n]=0;
            break;

        case 9:	// '<='
            p=Prew(n);
            if(m_VarClass[p] == VC_GUID)
            {
                TestLog(CStringReading::LoadString(IDS_GS_SCRIPT,STR_GS_SCRIPT_CANTOPERWITHGUIDVAR), "<=");
                break;
            }
           
            if( ((m_Value[p] - m_Value[n+1]) < 0) || (fabs(m_Value[p] - m_Value[n+1]) < 0.0001f) )
                m_Value[p] = true;
            else
                m_Value[p] = false;

            m_Value[n+1]=ERROR_CODE;
            m_Operation[n]=0;
            break;

        case 10:		// '='

            if ( m_VarClass[n]==VC_STRING || (m_Var[n][0] == '#' && m_VarClass[n] == VC_ARRAY) )
            {
                long lLen = DumpString(m_strValueInfo[n].pStrValue, m_strValueInfo[n].lStrValueLen, m_strValueInfo[n+1].pStrValue);
				m_strValueInfo[n].lStrValueLen = lLen+1;
                M_FREE(m_strValueInfo[n+1].pStrValue, sizeof(char)*m_strValueInfo[n+1].lStrValueLen);
				m_strValueInfo[n+1].pStrValue = NULL;
				m_strValueInfo[n+1].lStrValueLen = 0;
                m_Value[n+1]=ERROR_CODE;
                m_Operation[n]=0;
                goto _SetVar;
            }
            else if( m_VarClass[n]==VC_GUID || (m_Var[n][0] == '@' && m_VarClass[n] == VC_ARRAY) )
            {
                m_guidValue[n]=m_guidValue[n+1];
                m_guidValue[n+1]=NULL_GUID;
                m_Operation[n]=0;
                goto _SetVar;
            }
            else
            {
                m_Value[n]=m_Value[n+1];
                m_Value[n+1]=ERROR_CODE;
                m_Operation[n]=0;
                goto _SetVar;
            }

        case 11:		// '+='
            m_Value[n]+=m_Value[n+1];
            m_Value[n+1]=ERROR_CODE;
            m_Operation[n]=0;
            goto _SetVar;

        case 12:		// '-='
            m_Value[n]-=m_Value[n+1];
            m_Value[n+1]=ERROR_CODE;
            m_Operation[n]=0;
            goto _SetVar;

        case 13:		// '*='
            m_Value[n]*=m_Value[n+1];
            m_Value[n+1]=ERROR_CODE;
            m_Operation[n]=0;
            goto _SetVar;

        case 14:		// '/='
            m_Value[n]/=m_Value[n+1];
            m_Value[n+1]=ERROR_CODE;
            m_Operation[n]=0;
            goto _SetVar;

        case 15:		// '%='
            m_Value[n] = ((long)m_Value[n] % (long)m_Value[n+1]);
            m_Value[n+1]=ERROR_CODE;
            m_Operation[n]=0;
            goto _SetVar;

        case 16:		// '&'
            p=Prew(n);
            m_Value[p] = ((long)m_Value[p] & (long)m_Value[n+1]);
            m_Value[n+1]=ERROR_CODE;
            m_Operation[n]=0;
            break;

        case 17:		// '|'
            p=Prew(n);
            m_Value[p] = ((long)m_Value[p] | (long)m_Value[n+1]);
            m_Value[n+1]=ERROR_CODE;
            m_Operation[n]=0;
            break;

        case 18:	// '+'
            {
                int nPrewNum = Prew(n);
                int nPrewStr = PrewString(n);

                // 字符串连接
                if ((nPrewStr >=0 && nPrewNum<=nPrewStr) || m_strValueInfo[n+1].pStrValue!=0)
                {
                    if (nPrewNum<=nPrewStr)
                        p = nPrewStr;
                    else
                        p = nPrewNum;

                   
                    size_t len=1;
                    if(m_strValueInfo[p].pStrValue!=0)
                        len+=strlen(m_strValueInfo[p].pStrValue);
                    else
                        len+=32;

                    if (m_strValueInfo[n+1].pStrValue!=0)
                        len+=strlen(m_strValueInfo[n+1].pStrValue);
                    else
                        len+=32;

                    char* temp=(char*)M_ALLOC(sizeof(char)*len);
                    //char* str1;
                    //char* str2;
                    //int len1,len2;
                    //处理第一个参数

                    if (m_strValueInfo[p].pStrValue!=0){
                        strcpy_s(temp, len, m_strValueInfo[p].pStrValue);
                    }
                    else
						itoa(m_Value[p],temp,10);

                    if (m_strValueInfo[n+1].pStrValue!=0)
                        strcat_s(temp, len, m_strValueInfo[n+1].pStrValue);
                    else
						itoa(m_Value[n+1],&temp[strlen(temp)],10);

					
                    M_FREE(m_strValueInfo[p].pStrValue, sizeof(char)*m_strValueInfo[p].lStrValueLen);
					m_strValueInfo[p].pStrValue = NULL;
					m_strValueInfo[p].lStrValueLen = 0;

                    m_strValueInfo[p].pStrValue=temp;
					m_strValueInfo[p].lStrValueLen = (long)(sizeof(char)*len);

                    M_FREE(m_strValueInfo[n+1].pStrValue, sizeof(char)*m_strValueInfo[n+1].lStrValueLen);
					m_strValueInfo[n+1].pStrValue = NULL;
					m_strValueInfo[n+1].lStrValueLen = 0;

                    m_Value[n+1]=ERROR_CODE;
                    m_Operation[n]=0;
                }
                else{
                    p = nPrewNum;

                    m_Value[p]=m_Value[p] + m_Value[n+1];
                    m_Value[n+1]=ERROR_CODE;
                    m_Operation[n]=0;
                }
            }
            break;

        case 19:		// '-'
            p=Prew(n);
            m_Value[p]=m_Value[p] - m_Value[n+1];
            m_Value[n+1]=ERROR_CODE;
            m_Operation[n]=0;
            break;

        case 20:		// '*'
            p=Prew(n);
            m_Value[p]=m_Value[p] * m_Value[n+1];
            m_Value[n+1]=ERROR_CODE;
            m_Operation[n]=0;
            break;

        case 21:		// '/'
            p=Prew(n);

            if (m_Value[n+1]!=0)
                m_Value[p]=m_Value[p] / m_Value[n+1];

            m_Value[n+1]=ERROR_CODE;
            m_Operation[n]=0;
            break;

        case 22:		// '%'
            p=Prew(n);
            m_Value[p]=((long)m_Value[p] % (long)m_Value[n+1]);
            m_Value[n+1]=ERROR_CODE;
            m_Operation[n]=0;
            break;
        }
    }

_SetVar:
    //给变量付值
    if( m_VarClass[n] == VC_VAR )
    {
        double result = (double)SetScriptVarValue((CMoveShape*)p_SrcShape, m_Var[n].c_str(), 0, m_Value[n]);
    }
    else if( m_VarClass[n] == VC_STRING ) // 字符串
    {
        double result = (double)SetScriptVarValue((CMoveShape*)p_SrcShape, m_Var[n].c_str(), m_strValueInfo[n].pStrValue);
    }
    else if(m_VarClass[n] == VC_ARRAY ) // 数组(字符串和整数)
	{
		// 数组下标值
		int lArray = CVariableList::VAR_NOT_FOUND;
		char MainVarName[256];
		char InnVarName[256];

		char* MainVarNamePtr = MainVarName;
		char* InnVarNamePtr = InnVarName;
		char* varTypeFlag = (char*)m_Var[n].c_str();
		long tempPos = 0;

		double ret = CheckParamStringForArray(varTypeFlag, strlen(varTypeFlag), &MainVarNamePtr, 256, &InnVarNamePtr, 256, tempPos);
		if(ret == 1.0f)
		{
			if(InnVarNamePtr[0] == '$') // 下标是变量
				lArray = GetScriptVarValue((CMoveShape*)p_SrcShape, InnVarName, 0);
			else
				lArray = atoi(InnVarName);

			if(varTypeFlag[0] == '$') // 整数数组
			{
				if(lArray != CVariableList::VAR_NOT_FOUND)
					SetScriptVarValue( (CMoveShape*)p_SrcShape, MainVarName, lArray, m_Value[n] );
			}
			else if(varTypeFlag[0] == '#') // 字符串数组
			{
				if(lArray != CVariableList::VAR_NOT_FOUND)
				{
					// 找到对应字符串
					char arrayStr[256];
					_snprintf(arrayStr, 256, "%s%d", MainVarName, lArray);
					SetScriptVarValue((CMoveShape*)p_SrcShape, arrayStr, m_strValueInfo[n].pStrValue);
				}
			}
			else if(varTypeFlag[0] == '@') // GUID数组
			{
				if(lArray != CVariableList::VAR_NOT_FOUND)
				{
					// 找到对应字符串
					char arrayStr[256];
					_snprintf(arrayStr, 256, "%s%d", MainVarName, lArray);
					SetScriptGuidValue((CMoveShape*)p_SrcShape, arrayStr, m_guidValue[n]);
				}
			}
		}
		else
		{
			char outStr[1024];
			_snprintf(outStr, 1024, "count() array oper Err, var[%s]!", m_Var[n].c_str());
			AddLogText(outStr);
		}
	}
    else if( m_VarClass[n] == VC_GUID )
    {
        double result = (double)CVariableList::VAR_NOT_FOUND;
            SetScriptGuidValue((CMoveShape*)p_SrcShape, m_Var[n].c_str(), m_guidValue[n]);
        }
    return 0;
}

// -------------------------------------------------------
// 函数
// -------------------------------------------------------

//////////////////////////
// 获取函数名称
char *CScript::GetFunctionName(const char *CmdStr, long& lLen)
{
    int Len=(int)strlen(CmdStr);
    int n=0;
    char chr, *Ret;

    Ret=(char*)M_ALLOC(sizeof(char)*(Len+1));
	Ret[0] = '\0';
	lLen = Len+1;
    //memset(Ret, 0, Len+1);

    for(int i=0; i<Len; i++)
    {
        chr=CmdStr[i];
        if( chr == '(' || chr == ' ' || chr == '\t' || chr == '\n' || chr == '\r' || chr == ';' )
        {
			Ret[n] = '\0';
            return Ret;
        }
        Ret[n]=chr;
        n++;
    }

    return Ret;
}

/////////////////////////
// 取得整形参数（如果没有该参数，那么返回ERROR_CODE）
double CScript::GetIntParam(const char *CmdStr, int num)
{
    int Len=(int)strlen(CmdStr), j, ParamNum=0, nBracket=0;
    char chr, *Ret;
    bool bInString=false;

    Ret=(char*)M_ALLOC(sizeof(char)*(Len+1));
	Ret[0] = '\0';

    //查找参数头
    int i=0;
    for(; i<Len; i++)
    {
        if( CmdStr[i] == '(' ) goto _Next;
    }

    goto _Error;

_Next:
    for(j=i+1; j<Len; j++)
    {
        chr=CmdStr[j];

        // 判断是否进入字符串
        if( chr == '\"' ) bInString=!bInString;

        // 如果不在字符串内部
        if( !bInString )
        {
            if( chr == '(' )
            {
                nBracket++;								// 进入一层括号
            }
            else if( chr == ')' )
            {
                if( nBracket > 0 )			// 退出一层括号
                {
                    nBracket--;
                }
                else if( nBracket == 0 )	// 完成-没有找到指定的参数
                {
                    goto _Error;
                }
            }
            else if( nBracket==0 && IsCompart(chr) )	// 参数加一
            {
                while( !IsCompart(CmdStr[j]) ) { j++;}	// 去掉逗号前面的空格
                chr=CmdStr[j];
                ParamNum++;
            }
        }

        //对应的参数
        if( ParamNum==num )
        {
            while( IsCompart(CmdStr[j]) ) { j++;}	// 去掉逗号

            int n = 0;
            bInString=false;
            nBracket=0;
            for(int k=j; k<Len; k++)
            {
                chr=CmdStr[k];
                if( chr == '\"' ) bInString=!bInString;
                if( !bInString )
                {
                    if( chr == '(' ) nBracket++;
                    else if( chr == ')' && nBracket>0 ) nBracket--;
                    else if( nBracket==0 && ( IsCompart(chr) || chr == ')' ))
                    {
                        Ret[n]=';';
						Ret[n+1] = '\0';
                        CScript *pTemptScript = PopFreeScript(Ret);
                        InitChildScriptData(pTemptScript);
                        //设置变量	
                        pTemptScript->SetVariableList(m_pVariableList);
                        pTemptScript->RunLine(Ret);					
                        double ret=pTemptScript->m_Value[0];
                        M_FREE(Ret, sizeof(char)*(Len+1));
						Ret = NULL;
                        if(pTemptScript->m_strValueInfo[0].pStrValue!=NULL)	//类型不对
                            ret = ERROR_CODE;
                        PushFreeScript(pTemptScript);
                        return ret;
                    }
                }
                Ret[n]=chr;
                n++;
            }
            goto _Error;
        }
    }

_Error:
    M_FREE(Ret, sizeof(char)*(Len+1));
    return ERROR_CODE;
}


///////////////////////////
// 取得字符串参数（如果没有该参数，那么返回一个空的字符串）
char *CScript::GetStringParam(const char *CmdStr, int num)
{
    char* pRet=NULL;		//存放返回值
    char* pCurrent;			//当前搜索的位置
    char* pParam=NULL;		//参数的起始位置

    bool bInString=false;	//是否在字符串中
    int ParamNum;
    int ParamLength=0;		//参数的字符串长度
    int nBracket;

    //找第一个'('
    for( pCurrent = (char*)CmdStr; *pCurrent!=0; pCurrent++ )
    {
        if (*pCurrent=='(') break;
    }

    //到参数开始的地方
    pCurrent++;
    ParamNum=0;
    nBracket=0;

    for(; *pCurrent!=0; pCurrent++)
    {
        //记录需要的参数
        if(ParamNum==num)
        {
            if (0==pParam)
            {
                pParam=pCurrent;
                ParamLength=0;
            }
            ParamLength++;
        }

        if( *pCurrent == '\"' )
        {
            bInString=!bInString;
        }

        if( !bInString )
        {
            if( *pCurrent == '(' ) nBracket++;
            else if(*pCurrent == ')') nBracket--;

            if((nBracket==0 && IsCompart(*pCurrent)) || nBracket<0)
            {
                if (ParamLength>0)
                    break;//如果参数已经找到,就直接跳出进行处理
                ParamNum++;
            }
        }
    }

    //有此参数 (包括两边的引号)
    if (ParamLength>=2)
    {
        pRet=(char*)M_ALLOC(sizeof(char)*MAX_VAR_LEN);
        memcpy(pRet,pParam,ParamLength);
        pRet[ParamLength-1]=';';
        pRet[ParamLength]=0;

        CScript *pTemptScript = PopFreeScript(pRet);
        InitChildScriptData(pTemptScript);
        //设置变量	
        pTemptScript->SetVariableList(m_pVariableList);
        pTemptScript->RunLine(pRet);	
        if (0 == pTemptScript->m_strValueInfo[0].pStrValue)
        {
            _snprintf(pRet, MAX_VAR_LEN, "%f", pTemptScript->m_Value[0]);
        }
        else
        {
            strcpy_s(pRet, MAX_VAR_LEN, pTemptScript->m_strValueInfo[0].pStrValue);
        }
        PushFreeScript(pTemptScript);

    }
    else
    {
        pRet=NULL;
        //pRet[0]=0;
    }

    return pRet;
}

char *CScript::GetOperation(const char *CmdStr, int num)
{
    int Len=(int)strlen(CmdStr), n=0, j, ParamNum=0, nBracket=0;
    char chr, *Ret;
    bool bInString=false;

    Ret=(char*)M_ALLOC(sizeof(char)*(Len+1));
	Ret[0] = '\0';


    //查找参数头
    int i=0;
    for(; i<Len; i++)
    {
        if( CmdStr[i] == '(' ) goto _Next;
    }
    goto _ERROR;

_Next:
    for(j=i+1; j<Len; j++)
    {
        chr=CmdStr[j];

        //变量数值参数
        if( ParamNum==num && IsCompart(chr) )
        {
            for(int k=j+1; k<Len; k++)
            {
                Ret[n]=chr;
                n++;

                chr=CmdStr[k];
                if( !IsCompart(chr) )
				{
					Ret[n] = '\0';
                    return Ret;
            }
            }
            goto _ERROR;
        }

        //第几参数
        if( chr == '\"' ) bInString=!bInString;
        if( !bInString )
        {
            if( chr == '(' ) nBracket++;
            else if( chr == ')' && nBracket>0 ) nBracket--;
            else if( nBracket==0 && IsCompart(chr) )
            {
                while( !IsCompart(CmdStr[j]) ) { j++; }
                chr=CmdStr[j];
                ParamNum++;
            }
        }
    }

_ERROR:
    char str[10240];
    _snprintf(str, 10240, "Command [%s]'s Operation %d not found!", CmdStr, num);
    PutDebugString(str);


	Ret[0] = '\0';
    return Ret;
}

////////////////////////////
// 某字符是否是分隔符

bool CScript::IsCompart(char chr)
{
    if( chr == ',' )
        return true;

    return false;
}

////////////////////////
// 获取实际的Npc ID
double CScript::GetTrueNpcID(double id)
{
    return 0;
}

////////////////////////
// 运行一步脚本
// soft 2006-12-26 改为一直把脚本执行完，除非遇到等待的函数
enum CScript::SCRIPTRETURN CScript::RunStep()
{
    char CmdStr[MAX_VAR_LEN];
    char *CmdName;

    do{
        //有等待函数正在执行
        if( m_bHang )
        {
            return SR_HANG;
        }

        ReadCmd(CmdStr, MAX_VAR_LEN);		//读一行指令

		long lCmdNameLen = 0;
        CmdName=GetFunctionName(CmdStr, lCmdNameLen);	//获取第一个函数名称
        //返回
        if( stricmp( CmdStr, "" ) == 0 )
        {
			M_FREE(CmdName, lCmdNameLen);
            return SR_END;
        }
        else if( stricmp( CmdStr, "return;" ) == 0 )
        {
            M_FREE(CmdName, lCmdNameLen);
            return SR_RETURN;
        }
        //For返回
        else if( stricmp( CmdStr, "break;" ) == 0 )
        {
            M_FREE(CmdName, lCmdNameLen);
            return SR_FORBREAK;
        }
        //For返回
        else if( stricmp( CmdStr, "continue;" ) == 0 )
        {
            M_FREE(CmdName, lCmdNameLen);
            return SR_FORCONTINUE;
        }		
        else if( stricmp( CmdName, "<begin>" ) == 0 )
        {
            M_FREE(CmdName, lCmdNameLen);
            continue;
        }
        else if( stricmp( CmdName, "<end>" ) == 0 )
        {
           M_FREE(CmdName, lCmdNameLen);
            continue;
        }
        //标记
        else if( CmdStr[strlen(CmdStr)-2] == ':' )
        {
            M_FREE(CmdName, lCmdNameLen);
            continue;
        }
        //调用其它脚本(递归调用)
        else if( stricmp( CmdName, "call" ) == 0 )
        {
            SCRIPTRETURN ret = call(CmdStr);
            M_FREE(CmdName, lCmdNameLen);
            if(CScript::SR_END == ret || CScript::SR_RETURN == ret)
                continue;
            return ret;		
        }
		//调用其它脚本(递归调用)
        else if( stricmp( CmdName, "callex" ) == 0 )
        {
            SCRIPTRETURN ret = callEx(CmdStr);
            M_FREE(CmdName, lCmdNameLen);
            if(CScript::SR_END == ret || CScript::SR_RETURN == ret)
                continue;
            return ret;		
        }
        // [070511 AHC]
        // 定义局部变量
        else if( stricmp( CmdName, "localvar" ) == 0 )
        {          
            LocalVar(CmdStr);

            M_FREE(CmdName, lCmdNameLen);
            continue;
        }
        // for
        else if( stricmp( CmdName, "for" ) == 0 )
        {
            SCRIPTRETURN ret = For(CmdStr);
            M_FREE(CmdName, lCmdNameLen);
            if(CScript::SR_END == ret)
                continue;
            return ret;
        }

        RunLine(CmdStr);		//执行
        M_FREE(CmdName, lCmdNameLen);

    }while( 1 );

    return SR_END;
}

//异步操作返回后继续执行
enum CScript::SCRIPTRETURN CScript::ContinueRun(CVariableList* stackVarList)
{
#ifdef _RUNSTACKINFO_
    char pszStatckInfo[256]="";
    _snprintf(pszStatckInfo,256,"CScript::ContinueRun(FileName:%s) Start",m_FileName);
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStatckInfo);
#endif
#ifdef _SCRIPT_INFO_
	DWORD BeginTime = timeGetTime();
	m_RunTime = 0;
#endif

    SCRIPTRETURN eRet = SR_END;
    m_bHang = false;
    if(m_pChildScriptInfo)
    {
        InitChildScriptData(m_pChildScriptInfo->pChildScript);
		if(m_pChildScriptInfo->pChildScript)
		{
        switch(m_pChildScriptInfo->eType)
        {
        case CST_FOR:
            {
                eRet = ContinueRunFor(stackVarList);
            }
            break;
        case CST_CALL:
            {
                eRet = ContinueRunCall(stackVarList);
            }
            break;
		 case CST_CALLEX:
            {
                eRet = ContinueRunCallEx(stackVarList);
            }
            break;
			}
			if(eRet == SR_HANG)
			{
				m_bHang = true;
				SetHangFunc(m_pChildScriptInfo->pChildScript->GetHangFunc());
			}
		}
    }
    else
    {
        // 根据stackVarList设置返回值
        if(stackVarList)
        {
            if(stackVarList->GetVarList())
            {
                CVariableList::VarList::iterator varItr = stackVarList->GetVarList()->begin();
                for(; varItr != stackVarList->GetVarList()->end(); varItr++)
                {
                    if(varItr->first[0] == '#')// 字符串
                        SetScriptVarValue((CMoveShape*)p_SrcShape, varItr->first.c_str(), varItr->second->strValue);
                    else if(varItr->first[0] == '$')
                        SetScriptVarValue((CMoveShape*)p_SrcShape, varItr->first.c_str(), varItr->second->Array, *varItr->second->Value);
                }
                // GUID 变量
                CVariableList::GuidList::iterator guidItr = stackVarList->GetGuidList()->begin();
                for(; guidItr != stackVarList->GetGuidList()->end(); guidItr++)
                {
                    SetScriptGuidValue((CMoveShape*)p_SrcShape, guidItr->first.c_str(), *(guidItr->second));
                }
            }
        }
    }

#ifdef _SCRIPT_INFO_
	if(eRet != SR_HANG)
	{
		m_RunTime += timeGetTime() - BeginTime;
		GetGame()->AddScriptInfo(m_FileName, m_RunTime);
		m_RunTime = 0;
	}
	else
	{
		m_RunTime += timeGetTime() - BeginTime;
	}
#endif

    if(eRet == SR_HANG || eRet == SR_RETURN || eRet == SR_ERROR)
    {
#ifdef _RUNSTACKINFO_
        _snprintf(pszStatckInfo,256,"CScript::ContinueRun() End.(ret1:%d)",eRet);
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStatckInfo);
#endif
        return eRet;
    }
    eRet  = RunStep();
#ifdef _RUNSTACKINFO_
	_snprintf(pszStatckInfo,256,"CScript::ContinueRun() End.(ret2:%d)",eRet);
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStatckInfo);
#endif
    return eRet;
}

//异步操作返回后继续执行call子句
enum CScript::SCRIPTRETURN CScript::ContinueRunCall(CVariableList* stackVarList)
{
#ifdef _RUNSTACKINFO_
	char pszStatckInfo[1024]="";
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CScript::ContinueRunCall() Start");
#endif
	SCRIPTRETURN eRet = SR_END;
	eRet = m_pChildScriptInfo->pChildScript->ContinueRun(stackVarList);
	if(eRet != SR_HANG)
	{
		//释放脚本资源
		PushFreeScript(m_pChildScriptInfo->pChildScript);
		LeaveChildScript(m_pChildScriptInfo->pChildScript);		
	}
	if(eRet == SR_RETURN)
		eRet = SR_END;
#ifdef _RUNSTACKINFO_
	_snprintf(pszStatckInfo,1024,"CScript::ContinueRunCall() End(ret:%d)",eRet);
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStatckInfo);
#endif
	return eRet;
}

//异步操作返回后继续执行call子句
enum CScript::SCRIPTRETURN CScript::ContinueRunCallEx(CVariableList* stackVarList)
{
#ifdef _RUNSTACKINFO_
	char pszStatckInfo[1024]="";
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CScript::ContinueRunCallEx() Start");
#endif
	SCRIPTRETURN eRet = SR_END;
	eRet = m_pChildScriptInfo->pChildScript->ContinueRun(stackVarList);
	if(eRet != SR_HANG)
	{
		m_pChildScriptInfo->pChildScript->ResetVariableList();
		//释放脚本资源
		PushFreeScript(m_pChildScriptInfo->pChildScript);
		LeaveChildScript(m_pChildScriptInfo->pChildScript);		
	}
	if(eRet == SR_RETURN)
		eRet = SR_END;
#ifdef _RUNSTACKINFO_
	_snprintf(pszStatckInfo,1024,"CScript::ContinueRunCallEx() End(ret:%d)",eRet);
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStatckInfo);
#endif
	return eRet;
}

//异步操作返回后继续执行for子句
enum CScript::SCRIPTRETURN CScript::ContinueRunFor(CVariableList* stackVarList)
{
#ifdef _RUNSTACKINFO_
    char pszStatckInfo[1024]="";
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CScript::ContinueRunFor() Start");
#endif

    SCRIPTRETURN eRet = SR_END;
	eRet = m_pChildScriptInfo->pChildScript->ContinueRun(stackVarList);
	if(eRet == SR_HANG)
	{
		SetIsHang(true);
		SetHangFunc(m_pChildScriptInfo->pChildScript->GetHangFunc());
#ifdef _RUNSTACKINFO_
		_snprintf(pszStatckInfo,1024,"SCRIPTRETURN CScript::ContinueRunFor() End1(ret:%d)",eRet);
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStatckInfo);
#endif
		return eRet;
	}
	//离开子脚本
    CScript *forScript = m_pChildScriptInfo->pChildScript;
    char szCountCmd[MAX_STACK_CMD_LEN]="";
    char szMaxCmd[MAX_STACK_CMD_LEN]="";
    strncpy(szCountCmd,m_pChildScriptInfo->pszPreOp,MAX_STACK_CMD_LEN);
    strncpy(szMaxCmd,m_pChildScriptInfo->pszCon,MAX_STACK_CMD_LEN);

    LeaveChildScript(m_pChildScriptInfo->pChildScript);		
    if(eRet == SR_FORBREAK || eRet == SR_RETURN || eRet == SR_ERROR)
    {
        if(eRet == SR_FORBREAK)
            eRet= SR_END;
        PushFreeScript(forScript);
#ifdef _RUNSTACKINFO_
		_snprintf(pszStatckInfo,1024,"SCRIPTRETURN CScript::ContinueRunFor() End2(ret:%d)",eRet);
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStatckInfo);
#endif
        return eRet;
    }
    //如果eRet==SR_END or SR_FORCONTINUE,继续执行该脚本
    RunLine(szCountCmd);
    RunLine(szMaxCmd);
    while(1 == m_Value[0])
    {
        //重置当前指针
        forScript->ResetDataPos();
        //记录进入子脚本
        EnterChildScript(CST_FOR,forScript,szCountCmd,szMaxCmd);
        eRet =  forScript->RunStep();
        //只要该脚本不挂起，就记录离开子脚本
        if(eRet != CScript::SR_HANG)
			LeaveChildScript(forScript);
		//脚本挂起
		if(eRet == CScript::SR_HANG)
		{
			SetIsHang(true);
			SetHangFunc(forScript->GetHangFunc());
#ifdef _RUNSTACKINFO_
			_snprintf(pszStatckInfo,1024,"SCRIPTRETURN CScript::ContinueRunFor() End3(ret:%d)",eRet);
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStatckInfo);
#endif
			return eRet;
		}
		else if(eRet == CScript::SR_FORBREAK || eRet == CScript::SR_END)
		{
			break;
		}
		else if(eRet == CScript::SR_RETURN || eRet == CScript::SR_ERROR)
		{
			EndForScript(forScript);
#ifdef _RUNSTACKINFO_
			_snprintf(pszStatckInfo,1024,"SCRIPTRETURN CScript::ContinueRunFor() End4(ret:%d)",eRet);
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStatckInfo);
#endif
			return eRet;
		}

		//在retFlag ==  CScript::SR_FORCONTINUE或者其他况下，继续执行
		RunLine(szCountCmd);
		RunLine(szMaxCmd);
	}
    EndForScript(forScript);
#ifdef _RUNSTACKINFO_
	_snprintf(pszStatckInfo,1024,"SCRIPTRETURN CScript::ContinueRunFor() End5(ret:%d)",eRet);
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStatckInfo);
#endif
    return eRet;
}


//初始化子脚本数据
void CScript::InitChildScriptData(CScript *pChildScript)
{
    pChildScript->m_dwLastTime=0;
    pChildScript->SetPlayerID(GetPlayerID());
    pChildScript->SetRegion(GetRegion());
    pChildScript->SetSrcShape(GetSrcShape());
    pChildScript->SetDesShape(GetDesShape());
    pChildScript->SetUsedItemID(GetUsedItemID());
    pChildScript->SetDropGoodsPoint(GetDropGoodsPoint());
    pChildScript->SetID(GetID());
}

//记录进入子脚本
//必须和LeaveChildScript函数成对的调用 
void CScript::EnterChildScript(eCST eType,CScript* pChildScript,const char* pszPreOp,const char* pszCon)
{
    if(m_pChildScriptInfo != NULL)
    {
        //output the error info
        char strErrorInfo[1024]="";
        _snprintf(strErrorInfo,256,"error(scriptname:%s),enter the child script,the variable  is not null.",m_FileName);
        PutDebugString(strErrorInfo);
        MP_DELETE(m_pChildScriptInfo);
    }
    m_pChildScriptInfo = MP_NEW tagChilScriptInfo;
    m_pChildScriptInfo->eType = eType;
    m_pChildScriptInfo->pChildScript = pChildScript;
    strncpy(m_pChildScriptInfo->pszPreOp,pszPreOp,MAX_STACK_CMD_LEN);
    strncpy(m_pChildScriptInfo->pszCon,pszCon,MAX_STACK_CMD_LEN);
}
void CScript::LeaveChildScript(CScript* pChildScript)
{
    if(m_pChildScriptInfo == NULL || m_pChildScriptInfo->pChildScript != pChildScript)
    {
        //output the error info
        char strErrorInfo[1024]="";
        _snprintf(strErrorInfo,256,"error(scriptname:%s),leave the child script,the variable  is not equals.",m_FileName);
        PutDebugString(strErrorInfo);
    }
    MP_DELETE(m_pChildScriptInfo);
}

void CScript:: ResetChildScriptInfo()
{
    if(m_pChildScriptInfo != NULL)
    {
        //output the error info
        char strErrorInfo[1024]="";
        _snprintf(strErrorInfo,256,"error(scriptname:%s),reset the child script,the variable  is not equals.",m_FileName);
        PutDebugString(strErrorInfo);
        m_pChildScriptInfo = NULL;
    }
}


CScript* CScript::BeginForScript(char* pScriptData)
{
    CScript* tempScript = PopFreeScript("for");
    InitChildScriptData(tempScript);
    // 加载脚本内容
    if(tempScript)
    {
        tempScript->LoadScript(NULL, pScriptData);
        tempScript->SetVariableList(GetVariableList());
    }
    return tempScript;
}

void CScript::EndForScript(CScript* forScript)//外部调用脚本
{
    PushFreeScript(forScript);
}

void	CScript::SetSrcShapeEx(CShape* pShape)
{
   SetSrcShape(pShape);
    //设置脚本变量名值
    GetVariableList()->AddVar("$ParamA", (DWORD)p_SrcShape);
}


void	CScript::SetRegion(CRegion* r)
{
    m_pRegion = r;
    if(m_pRegion)
        m_guRegionID = m_pRegion->GetExID();
    else
        m_guRegionID = NULL_GUID;
}

void    CScript::SetSrcShape(CShape* shape)
{
    p_SrcShape = shape;
    if(p_SrcShape)
    {
        m_lSrcType = p_SrcShape->GetType();
        m_guSrcID = p_SrcShape->GetExID();
    }
    else
    {
        m_lSrcType = 0;
        m_guSrcID = NULL_GUID;
    }
}


void    CScript::SetDesShape(CShape* shape)
{
    p_DesShape = shape;
    if(p_DesShape)
    {
        m_lDesType = p_DesShape->GetType();
        m_guDesID = p_DesShape->GetExID();
    }
    else
    {
        m_lDesType = 0;
        m_guDesID = NULL_GUID;
    }
}

//重新设置
void CScript::ResetShape()
{
    CServerRegion *pRegion = GetGame()->FindRegion(m_guRegionID);
    if(pRegion)
    {
        m_pRegion = pRegion;
        p_SrcShape = dynamic_cast<CShape*>(pRegion->FindChildObject(m_lSrcType,m_guSrcID));
        p_DesShape = dynamic_cast<CShape*>(pRegion->FindChildObject(m_lDesType,m_guDesID));
    }
    else
    {
        m_pRegion = NULL;
        p_SrcShape = NULL;
        p_DesShape = NULL;
    }
}


void	CScript::SetSkillEx(CBaseModule *pSkill)
{
    //设置脚本变量名值
    GetVariableList()->AddVar("$ParamS",(DWORD)pSkill);
}
void	 CScript::SetFireShapeEx(CShape* shape)
{
   //设置脚本变量名值
   GetVariableList()->AddVar("$ParamF",(DWORD)shape);
}
//执行一段字符串
enum CScript::SCRIPTRETURN CScript::RunStr(CMoveShape* pDest,const char* pszContent)
{
    SetDesShape(pDest);
    if( LoadScript(NULL, (char*)pszContent) )		//打开文件
    {
        GetVariableList()->AddVar("$ParamB", (DWORD)pDest);
        //返回值
        GetVariableList()->AddVar("$Ret", 0.0f);
        return RunStep();
    }
    return SR_ERROR;
}


int CScript::GetValueType(const char* pValue){
    if (*pValue=='\"')
        return VC_STRING;
    else
        return VC_VAR;
}


int CScript::DumpString(char* &pDest, long lDesLen, const char* pSrc)
{
    if (pSrc==NULL)
        return 0;

    int len=lstrlen(pSrc);

    if(pDest!=0)
        M_FREE(pDest, lDesLen);
    pDest=(char*)M_ALLOC(sizeof(char)*(len+1));

    strcpy_s(pDest, len+1, pSrc);
    //	memcpy(pDest,pSrc,len+1);
    return len;
}


// 更新到WorldServer
bool CScript::UpdateToWorldServer(const char* strVarName, int pos, double nValue)
{
	if(!strVarName) return false;
	CMessage msg(MSG_S2W_SERVER_CHANGE_SCRIPT_VARIABLE);
	msg.Add((char*)strVarName);
	msg.Add((BYTE)eVarNumber); // 0 数值
	msg.Add((long)pos);
	msg.Add((LONG64)(*(LONG64*)&nValue));
	msg.Send();
	return true;
}
bool CScript::UpdateToWorldServer(const char* strVarName, const char* szValue)	// 更新到WorldServer
{
	if(!strVarName || !szValue) return false;
	CMessage msg(MSG_S2W_SERVER_CHANGE_SCRIPT_VARIABLE);
	msg.Add(strVarName);
	msg.Add((BYTE)eVarString); // 1 字符串
	msg.Add(szValue);
	msg.Send();
	return true;
}
bool CScript::UpdateToWorldServer(const char* strVarName, const CGUID& guid)		 // 更新"@"到WorldServer
{
	if(!strVarName) return false;
	CMessage msg(MSG_S2W_SERVER_CHANGE_SCRIPT_VARIABLE);
	msg.Add(strVarName);
	msg.Add((BYTE)eVarGuid); // 2 guid
	msg.Add(guid);
	msg.Send();
	return true;
}
// [070608 AHC]
bool CScript::SetAttr(OBJECT_TYPE type, const CGUID& entityID, std::string& attrName, double value)
{
    CMoveShape* shape = GetGame()->FindShape(type, entityID);
    if(shape)
        return shape->SetAttribute(attrName, value);

    return false;
}
double CScript::GetAttr(OBJECT_TYPE type, const CGUID& entityID, std::string& attrName)
{
    CMoveShape* shape = GetGame()->FindShape(type, entityID);
    if(shape)
        return shape->GetAttribute(attrName);

    return INVILID_VALUE;
}
bool CScript::ChangeAttr(OBJECT_TYPE type, const CGUID& entityID, std::string& attrName, double value)
{
    CMoveShape* shape = GetGame()->FindShape(type, entityID);
    if(shape)
    {
        shape->ChangeAttribute(attrName, value);
        shape->UpdateAttribute();
        return true;
    }

    return false;
}

bool CScript::SetMoveShapeAttrEx(DWORD entityPtr, std::string& attrName, double value)
{
		CMoveShape* pMoveShape = ((CMoveShape*)entityPtr);
        if(pMoveShape)
        {
			if(pMoveShape->GetType() == TYPE_PLAYER)
			{
				CPlayer *pPlayer = (CPlayer*)(pMoveShape);
				char pszTempt[32]="";
				if(attrName == "OccuLvl")
				{
					_snprintf(pszTempt,32,"OccuLvl%d",pPlayer->GetOccupation());
					attrName = pszTempt;
				}
				else if(attrName == "OccuExp")
				{
					_snprintf(pszTempt,32,"OccuExp%d",pPlayer->GetOccupation());
					attrName = pszTempt;
				}
				else if(attrName == "OccuSP")
				{
					_snprintf(pszTempt,32,"OccuSP%d",pPlayer->GetOccupation());
					attrName = pszTempt;
				}
			}

            bool ret = pMoveShape->SetAttribute(attrName, value);
            pMoveShape->UpdateAttribute();
            return ret;
        }
    return false;
}
double CScript::GetMoveShapeAttrEx(DWORD entityPtr, std::string& attrName)
{
		CMoveShape *pMoveShape = (CMoveShape*)entityPtr;
		if (pMoveShape)
		{
			if(pMoveShape->GetType() == TYPE_PLAYER)
			{
				CPlayer *pPlayer = (CPlayer*)(pMoveShape);
				char pszTempt[32]="";
				if(attrName == "OccuLvl")
				{
					_snprintf(pszTempt,32,"OccuLvl%d",pPlayer->GetOccupation());
					attrName = pszTempt;
				}
				else if(attrName == "OccuExp")
				{
					_snprintf(pszTempt,32,"OccuExp%d",pPlayer->GetOccupation());
					attrName = pszTempt;
				}
				else if(attrName == "OccuSP")
				{
					_snprintf(pszTempt,32,"OccuSP%d",pPlayer->GetOccupation());
					attrName = pszTempt;
				}
			}
			return pMoveShape->GetAttribute(attrName);
		}
    return INVILID_VALUE;
}
bool CScript::ChangeMoveShapeAttrEx(DWORD entityPtr, std::string& attrName, double value)
{
		CMoveShape* pMoveShape = ((CMoveShape*)entityPtr);
        if(pMoveShape)
        {
			if(pMoveShape->GetType() == TYPE_PLAYER)
			{
				CPlayer *pPlayer = (CPlayer*)(pMoveShape);
				char pszTempt[32]="";
				if(attrName == "OccuLvl")
				{
					_snprintf(pszTempt,32,"OccuLvl%d",pPlayer->GetOccupation());
					attrName = pszTempt;
				}
				else if(attrName == "OccuExp")
				{
					_snprintf(pszTempt,32,"OccuExp%d",pPlayer->GetOccupation());
					attrName = pszTempt;
				}
				else if(attrName == "OccuSP")
				{
					_snprintf(pszTempt,32,"OccuSP%d",pPlayer->GetOccupation());
					attrName = pszTempt;
				}
			}

            pMoveShape->ChangeAttribute(attrName, value);
            pMoveShape->UpdateAttribute();
            return true;
        }
    return false;
}

int CScript::GetVarValue(const char* str,int num)
{
    int nRet = GetVariableList()->GetVarValue(str, 0);
    if(nRet == CVariableList::VAR_NOT_FOUND)
        return 0;
    return nRet;
}
int CScript::SetScriptVarValue(CMoveShape* moveShapePtr, const char* varName, const char* value)
{
    int retFlag = GetVariableList()->SetVarValue(varName, value);
    if(retFlag == CVariableList::VAR_NOT_FOUND && moveShapePtr)
        retFlag = moveShapePtr->GetVariableList()->SetVarValue(varName, value);
	if(retFlag == CVariableList::VAR_NOT_FOUND)
	{
		if(CScript::GetGeneralVariableList())
			retFlag = CScript::GetGeneralVariableList()->SetVarValue(varName, value);
		if(1 == retFlag)
			CScript::UpdateToWorldServer(varName, value);
	}

    return retFlag;
}
int CScript::SetScriptVarValue(CMoveShape* moveShapePtr, const char* varName, long lArray, double value)
{
    int retFlag = GetVariableList()->SetVarValue(varName, lArray, value);
    if(retFlag == CVariableList::VAR_NOT_FOUND && moveShapePtr)
        retFlag = moveShapePtr->GetVariableList()->SetVarValue(varName, lArray, value);
    if(retFlag == CVariableList::VAR_NOT_FOUND)
	{
		if(CScript::GetGeneralVariableList())
			retFlag = CScript::GetGeneralVariableList()->SetVarValue(varName, lArray, value);
		if(1 == retFlag)
			CScript::UpdateToWorldServer(varName, lArray, value);
	}

    return retFlag;
}

const char* CScript::GetScriptVarValue(CMoveShape* moveShapePtr, const char* varName)
{
    const char* retStr = GetVariableList()->GetVarValue(varName);
    if(retStr == NULL && moveShapePtr)
        retStr = moveShapePtr->GetVariableList()->GetVarValue(varName);
    if(retStr == NULL)
	{
		if(CScript::GetGeneralVariableList())
			retStr = CScript::GetGeneralVariableList()->GetVarValue(varName);
	}

    return retStr;
}
double CScript::GetScriptVarValue(CMoveShape* moveShapePtr, const char* varName, long lArray)
{
    double retFlag = GetVariableList()->GetVarValue(varName, lArray);
    if(retFlag == CVariableList::VAR_NOT_FOUND && moveShapePtr)
        retFlag = moveShapePtr->GetVariableList()->GetVarValue(varName, lArray);
    if(retFlag == CVariableList::VAR_NOT_FOUND)
	{
		if(CScript::GetGeneralVariableList())
			retFlag = CScript::GetGeneralVariableList()->GetVarValue(varName, lArray);
	}

    return retFlag;
}
int CScript::SetScriptGuidValue(CMoveShape* moveShapePtr, const char* varName, const CGUID& guid)
{
    int retFlag = GetVariableList()->SetGuidValue(varName, &guid);
    if(retFlag == CVariableList::VAR_NOT_FOUND && moveShapePtr)
	{
		retFlag = moveShapePtr->GetVariableList()->SetGuidValue(varName, &guid);
		if(moveShapePtr->GetType() == TYPE_PLAYER)
			GetGame()->UpdateSavePlayer(moveShapePtr->GetExID(), SAVE_DETAIL_ATTRPROPERTY);
	}
    if(retFlag == CVariableList::VAR_NOT_FOUND)
	{
		if(CScript::GetGeneralVariableList())
			retFlag = CScript::GetGeneralVariableList()->SetGuidValue(varName, &guid);
		if(1 == retFlag)
			CScript::UpdateToWorldServer(varName, guid);
	}

    return retFlag;
}
const CGUID& CScript::GetScriptGuidValue(CMoveShape* moveShapePtr, const char* varName)
{	
	const CGUID &localGUID = GetVariableList()->GetGuidValue(varName);
	if( localGUID != NULL_GUID )
	{
		return localGUID;
	}
	if( moveShapePtr != NULL )
	{
		const CGUID &shapeGUID = moveShapePtr->GetVariableList()->GetGuidValue( varName );
		if( shapeGUID != NULL_GUID )
		{
			return shapeGUID;
		}
	}

	if(CScript::GetGeneralVariableList())
		return CScript::GetGeneralVariableList()->GetGuidValue(varName);

    return NULL_GUID;
}