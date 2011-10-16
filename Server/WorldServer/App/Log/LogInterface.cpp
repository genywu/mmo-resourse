#include "StdAfx.h"

#include "../GlobalRgnManager.h"
#include "../OrganizingSystem/WSTeam.h"

#include "../../Net/Message.h"

#include "LogicLogInterface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//<!--帮会创建/解散日志-->
//<Table name="t150_faction_log" log_no="150" log_pos="1">
//<item type="client_head"     head_name=""></item>
//<item type="player"   head_name=""></item>
//<item type="faction"  head_name=""></item>
//<item type="alone"    Item_name="act_type"        data_type="int32"></item><!--动作类型（0：创建；1解散）-->
//</Table>
//*/
bool LogicLogInterface::LogT150_faction_log(LPCSTR pOprName, const CGUID &pOprGUID, CFaction *pFaction, LONG lAct)
{
    if(0 == GetNetHandler().GetServerSetup().useLogServer) return true;
    if(NULL == m_pLogClient)return false;
    if(NULL == pOprName || NULL == pFaction) return false;

    string strTime;
    GetCurrTimeString(strTime);


    return m_pLogClient->SendLogToServer(150, strTime.c_str(),
        &pOprGUID,	pOprName,
        &(pFaction->GetExID()),	pFaction->GetName(),
        lAct
        );
}
//
///* --------------------------------------------------------------------
//<!--成员申请加入/退出日志-->
//<Table name="t160_faction_join_leave_log" log_no="160" log_pos="1">
//<item type="client_head"     head_name=""></item>
//<item type="player"   head_name=""></item>
//<item type="faction"  head_name=""></item>
//<item type="alone"    Item_name="act_type"        data_type="int32"></item><!--动作类型（0：申请；1退出）-->
//</Table>
//*/
bool LogicLogInterface::LogT160_faction_join_leave_log(const char *pOpterName, const CGUID *pOpterGuid, CFaction *pFaction, LONG lAct)
{
    if(0 == GetNetHandler().GetServerSetup().useLogServer) return true;
    if(NULL == m_pLogClient)return false;
    if(NULL == pOpterGuid || NULL == pOpterName || NULL == pFaction) return false;

    string strTime;
    GetCurrTimeString(strTime);

    return m_pLogClient->SendLogToServer( 
        160, strTime.c_str(),
        pOpterGuid,	pOpterName,
        &(pFaction->GetExID()),	pFaction->GetName(),
        lAct
        );
}
//
//
///* --------------------------------------------------------------------
//<!--同意进入/开除成员日志-->
//<Table name="t170_faction_agree_kick_log" log_no="170" log_pos="1">
//<item type="client_head"     head_name=""></item>
//<item type="player"   head_name=""></item>
//<item type="player"   head_name="aim"></item>
//<item type="faction"  head_name=""></item>
//<item type="alone"    Item_name="act_type"        data_type="int32"></item><!--动作类型（0：同意；1开除）-->
//</Table>
//*/
bool LogicLogInterface::LogT170_faction_agree_kick_log(const char *pOpterName, const CGUID *pOpterGuid, const char *pAimName, const CGUID *pAimGuid, CFaction *pFaction, LONG lAct)
{
    if(0 == GetNetHandler().GetServerSetup().useLogServer) return true;
    if(NULL == m_pLogClient)return false;
    if(NULL == pOpterName || NULL == pOpterGuid || NULL == pAimName || NULL == pAimGuid || NULL == pFaction) return false;

    string strTime;
    GetCurrTimeString(strTime);

    return m_pLogClient->SendLogToServer( 
        170, strTime.c_str(),
        pOpterGuid, pOpterName,
        pAimGuid,	pAimName,
        &(pFaction->GetExID()),	pFaction->GetName(),
        lAct
        );
}

///* --------------------------------------------------------------------
//<!--帮主变更-->
//<Table name="t250_faction_master_log" log_no="250" log_pos="1">
//<item type="client_head"    head_name=""></item>
//<item type="faction"		head_name=""></item>
//<item type="player"			head_name="old"></item>
//<item type="player"			head_name="new"></item>
//</Table>
//*/
bool LogicLogInterface::LogT250_faction_master_log(CFaction *pFaction, tagFacMemInfo *pOld, tagFacMemInfo *pNew)
{
    if(0 == GetNetHandler().GetServerSetup().useLogServer) return true;
    if(NULL == m_pLogClient)return false;
    if(NULL == pFaction || NULL == pOld || NULL == pNew) return true;

    string strTime;
    GetCurrTimeString(strTime);

    return m_pLogClient->SendLogToServer( 
        250, strTime.c_str(),							
        &(pFaction->GetExID()),			pFaction->GetName(), 
        &(pOld->MemberGuid),			pOld->szName,
        &(pNew->MemberGuid),			pNew->szName
        );
}
//
///*
//<!--================================================================-->
//<!--帮会经验变化-->
//<Table name="t260_faction_experience_log" log_no="260" log_pos="1">
//<item type="client_head"     head_name=""></item>
//<item type="faction"  head_name=""></item>
//<item type="alone"    Item_name="before_exp"      data_type="int32"></item><!--之前经验-->
//<item type="alone"    Item_name="curr_exp"        data_type="int32"></item><!--当前经验-->
//</Table>
//*/
bool LogicLogInterface::LogT260_faction_experience_log(CFaction *pFaction, long lBeforeExp)
{
    if(0 == GetNetHandler().GetServerSetup().useLogServer) return true;
    if(NULL == m_pLogClient)return false;
    if(NULL == pFaction) return true;

    string strTime;
    GetCurrTimeString(strTime);

    return m_pLogClient->SendLogToServer(260, strTime.c_str(),
        &(pFaction->GetExID()),			pFaction->GetName(), 
        lBeforeExp,						pFaction->GetExp()
        );
}
//
///*
//<!--================================================================-->
//<!--帮会等级变化-->
//<Table name="t270_faction_level_log" log_no="270" log_pos="1">
//<item type="client_head"     head_name=""></item>
//<item type="faction"  head_name=""></item>
//<item type="alone"    Item_name="before_level"    data_type="int32"></item><!--之前等级-->
//<item type="alone"    Item_name="curr_level"      data_type="int32"></item><!--当前等级-->
//<item type="alone"    Item_name="level_type"      data_type="int32"></item><!--等级类型 0=工会 1=福利 2=仓库 3=锻造 4炼金 5=缝纫-->
//</Table>
//*/
bool LogicLogInterface::LogT270_faction_level_log(CFaction *pFaction, long lCurrLevel, long lLvlType)
{
    if(0 == GetNetHandler().GetServerSetup().useLogServer) return true;
    if(NULL == m_pLogClient)return false;
    if(NULL == pFaction) return true;

    string strTime;
    GetCurrTimeString(strTime);

    return m_pLogClient->SendLogToServer(270, strTime.c_str(),
        &(pFaction->GetExID()),			pFaction->GetName(), 
        pFaction->GetLvl(lLvlType),		lCurrLevel,
        lLvlType
        );
}

////! 日志记录接口定义
////////////////////////////////////////////////////////////////////////////////////////////////
///* 
//日志配置：
//<!--队伍组建-->
//<Table name="t340_team_create" log_no="34">
//<item type="date"     head_name=""></item>
//<item type="object"   head_name="team"></item>
//<item type="player"   head_name="sponsor"></item>
//<item type="player"   head_name="participator"></item>
//</Table>
//*/
////! 记录队伍组建日志
bool LogicLogInterface::LogT340_team_create(	
    WSTeam *pTeam,			//! 队伍
    CPlayer *pSponsor,		//! 发起人
    CPlayer *pParticipator	//! 参与者
    )
{
    if(NULL == m_pLogClient) return true;

    string strTime;
    GetCurrTimeString(strTime);

    return m_pLogClient->SendLogToServer( 340, strTime.c_str(),
        &(pTeam->GetExID()),			"", 
        &(pSponsor->GetExID()),			pSponsor->GetName(),
        &(pParticipator->GetExID()),	pParticipator->GetName()
        );
}
//
///* 
//日志配置：
//<!--队伍解散-->
//<Table name="t370_team_disband" log_no="37">
//<item type="date"     head_name=""></item>
//<item type="object"   head_name="team"></item>
//</Table>
//*/
////! 记录队伍解散日志
bool LogicLogInterface::LogT370_team_disband(WSTeam *pTeam)
{
	if(NULL == m_pLogClient) return true;

    string strTime;
    GetCurrTimeString(strTime);

	return m_pLogClient->SendLogToServer( 370, strTime.c_str(),
		&(pTeam->GetExID()), ""
		);
}


///*
//<!--联系人操作日志-->
//<Table name="t450_linkman_opt_log" log_no="50" log_pos="1">
//<item type="client_head"     head_name=""></item>
//<item type="player"   head_name="opter"></item><!--操作者-->
//<item type="player"   head_name="aim"></item><!--操作目标-->
//<item type="alone"    Item_name="opt_type"      data_type="int32"></item><!--操作类型：1=添加、2=删除-->
//<item type="alone"    Item_name="group_id"      data_type="int32"></item><!--所操作的联系人组的ID-->
//</Table>
//*/
bool LogicLogInterface::logT450_linkman_opt_log(CPlayer *pOpter, const CGUID &AimGuid, const char *pAimName, long lOptType, long lGroupID)
{
    if(0 == GetNetHandler().GetServerSetup().useLogServer) return true;

    if(NULL == m_pLogClient)return false;
    if (NULL == pOpter || NULL == pAimName) return false;

    string strTime;
    GetCurrTimeString(strTime);

    return m_pLogClient->SendLogToServer( 
        450,strTime.c_str(),
        &(pOpter->GetExID()),	pOpter->GetName(),
        &AimGuid,				pAimName,
        lOptType,				lGroupID
        );
}

///*
//<!--客户端上下线日志-->
//<Table name="t550_client_in_game_log" log_no="550" log_pos="1">
//	<item type="client_head"     head_name=""></item>
//	<item type="alone"    Item_name="cdkey"         data_type="str128"></item><!--IP-->
//	<item type="alone"    Item_name="ip"            data_type="int32"></item><!--CDKEY-->
//	<item type="player"   head_name=""></item>
//	<item type="pos"      head_name=""></item>
//	<item type="alone"    Item_name="region_guid"   data_type="int32"></item><!--level-->
//	<item type="alone"    Item_name="level"         data_type="int32"></item><!--level-->
//	<item type="alone"    Item_name="gold"          data_type="int32"></item><!--gold-->
//	<item type="alone"    Item_name="silver"        data_type="int32"></item><!--silver-->
//	<item type="alone"    Item_name="flag"          data_type="int32"></item><!--操作类型：1=上线、2=下线-->
//</Table>
//*/
bool LogicLogInterface::logT550_client_in_game_log(CPlayer *pPlayer, LONG lState)
{
    if(0 == GetNetHandler().GetServerSetup().useLogServer) return true;
    if(NULL == m_pLogClient)return false;
    if(NULL == pPlayer) return false;

    const char *pRegionName = "";
    map<LONG, CGlobalRgnManager::tagRegion> &mapRegionList = *(GetGame().GetGlobalRgnManager()->GetTemplateRegionListMap());
    map<LONG, CGlobalRgnManager::tagRegion>::iterator ite = mapRegionList.find(pPlayer->GetRegionID());
    if (ite != mapRegionList.end())
    {
        CWorldRegion *pWorldRegion = ite->second.pRegion;
        if (NULL != pWorldRegion)
        {
            pRegionName = pWorldRegion->GetName();
        }
    }

    float fPosX = pPlayer->GetPosX();
    float fPosY = pPlayer->GetPosY();

    //! 转换IP
    char szTmp[128] = {0};
    UCHAR arrIp[4] = {0};
    LONG lIP = pPlayer->GetIP();
    memmove(arrIp, &lIP, sizeof(LONG));
    sprintf(szTmp, "%u.%u.%u.%u", (ULONG)arrIp[0], (ULONG)arrIp[1], (ULONG)arrIp[2], (ULONG)arrIp[3]);

    string strTime;
    GetCurrTimeString(strTime);


    LONG lBankMoney = 0;
    CGoodsContainer* pBank = pPlayer->m_PlayerDepot.FindContainer(eDCT_Gold);
    if(NULL != pBank)
        lBankMoney = pBank->GetGoodsAmount();

    return m_pLogClient->SendLogToServer( 
        550, strTime.c_str(),
        pPlayer->GetAccount(),
        szTmp,
        &(pPlayer->GetExID()),	pPlayer->GetName(),
        pPlayer->GetRegionID(),
        pRegionName,
        &fPosX,
        &fPosY,
        &(pPlayer->GetRegionExID()),
        pPlayer->GetLevel(),
        pPlayer->GetMoney() + lBankMoney,
        pPlayer->GetSilver(),
        lState
        );
}