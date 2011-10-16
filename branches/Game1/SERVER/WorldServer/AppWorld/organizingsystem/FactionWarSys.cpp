//////////////////////////////////////////////////////////////////////////
//FactionWarSys.cpp
//Function:帮战系统相关功能
//Author:Joe
//Create Time:2005.6.27
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include ".\factionwarsys.h"
#include "OrganizingCtrl.h"
#include "Faction.h"
#include "Union.h"
#include "AttackCitySys.h"
#include "..\public\Timer.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//CFactionWarSys*		CFactionWarSys::pInstance = NULL;
//
//CFactionWarSys::CFactionWarSys(void)
//{
//	m_dwStartTime = timeGetTime();
//	m_FactionWars.clear();
//}
//
//CFactionWarSys::~CFactionWarSys(void)
//{
//}
//
//CFactionWarSys* CFactionWarSys::GetInstance()
//{
//	if(pInstance == NULL)
//	{
//		pInstance = new CFactionWarSys;
//	}
//	return pInstance;
//}
//
//void CFactionWarSys::Release()
//{
//	if(pInstance)
//	{
//		SAFE_DELETE(pInstance);
//	}
//}
//
//bool   CFactionWarSys::Initialize()
//{
//	AddLogText("发送读取敌对帮会列表消息...");
//	//	GetGame()->GetRsEnemyFactions()->LoadAllEnemyFactions();
//	GetGame()->SendLoadAllEnemyFactions();
//	//	AddLogText("成功装载敌对帮会列表...");
//
//	list<tagEnemyFaction>::iterator it = EnemyFactionList.begin();
//	for(;it != EnemyFactionList.end();it++)
//	{
//		GetOrganiCtrl()->SetEnemyFactionRelation((*it).FactionGuid1,(*it).FactionGuid2);
//	}
//
//	//读取配置文件
//	LoadIni();
//	return true;
//}
//
//bool	CFactionWarSys::LoadIni()
//{
//	m_FactionWars.clear();
//
//	CRFile* prfile = rfOpen("data/FactionWarSys.ini");
//	if(prfile == NULL)
//	{
//		char str[]="data/FactionWarSys.txt can't found!";
//		AddLogText(str);
//		return false;
//	}
//
//	stringstream stream;
//	prfile->ReadToStream(stream);
//	rfClose(prfile);
//
//	while(ReadTo(stream, "#"))
//	{
//		tagWarType WarType;
//		stream>>WarType.lType
//			>>WarType.lFightTime
//			>>WarType.lMoney;
//		WarType.lFightTime *= 60000;//把分钟转换成毫秒
//		m_FactionWars[WarType.lType] = WarType;
//	}
//	return true;
//}
//
////帮战系统处理
//void	CFactionWarSys::Run()
//{
//	static list<tagEnemyFaction> TemptEnemyFactionList;
//	DWORD dwCurTime = timeGetTime();
//	//一分钟处理一次
//	if( (dwCurTime-m_dwStartTime) < 60000)	return;
//	DWORD dwDifTime = dwCurTime-m_dwStartTime;
//	m_dwStartTime = dwCurTime;
//	TemptEnemyFactionList.clear();
//	list<tagEnemyFaction>::iterator it = EnemyFactionList.begin();
//	for(;it != EnemyFactionList.end();it++)
//	{
//		if((*it).dwDisandTime <= dwDifTime)
//		{
//			TemptEnemyFactionList.push_back((*it));
//		}
//		else
//		{
//			(*it).dwDisandTime -= dwDifTime;
//		}
//	}
//
//	//处理帮战时间已到的帮会
//	it = TemptEnemyFactionList.begin();
//	for(;it != TemptEnemyFactionList.end();it++)
//	{
//		OnTimeOut( (*it).FactionGuid1,(*it).FactionGuid2 );
//	}
//}
//
////得到宣战某个类型需要的金钱
//long CFactionWarSys::GetDecWarMoneyByType(long lType)
//{
//	if(m_FactionWars.find(lType) == m_FactionWars.end())
//		return 0;
//	return m_FactionWars[lType].lMoney;
//}
//
////判断两个帮会是否是敌对关系
//bool CFactionWarSys::IsEnemyRelation(const CGUID& FactionGuid1,const CGUID& FactionGuid2)
//{
//	list<tagEnemyFaction>::iterator it = EnemyFactionList.begin();
//	for(;it != EnemyFactionList.end();it++)
//	{
//		if( ((*it).FactionGuid1 == FactionGuid1 && (*it).FactionGuid2 == FactionGuid2) ||
//			((*it).FactionGuid1 == FactionGuid2 && (*it).FactionGuid2 == FactionGuid1) )
//		{
//			return true;
//		}
//	}
//	return false;
//}
//
//void CFactionWarSys::AddOneEnmeyFaction(const CGUID& FactionGuid1,const CGUID& FactionGuid2,DWORD dwLeaveTime)
//{
//	//如果有则更新时间值
//	list<tagEnemyFaction>::iterator it = EnemyFactionList.begin();
//	for(;it != EnemyFactionList.end();it++)
//	{
//		if( ((*it).FactionGuid1 == FactionGuid1 && (*it).FactionGuid2 == FactionGuid2) ||
//			((*it).FactionGuid1 == FactionGuid2 && (*it).FactionGuid2 == FactionGuid1) )
//		{
//			(*it).dwDisandTime = dwLeaveTime;
//			return;
//		}
//	}
//
//	tagEnemyFaction EnemyFaction;
//	EnemyFaction.FactionGuid1 = FactionGuid1;
//	EnemyFaction.FactionGuid2 = FactionGuid2;
//	EnemyFaction.dwDisandTime = dwLeaveTime;
//	EnemyFactionList.push_back(EnemyFaction);
//}
//
////清楚两个帮会间的敌对关系
//void	CFactionWarSys::ClearEnemyFaction(const CGUID& FactionGuid1,const CGUID& FactionGuid2)
//{
//	list<tagEnemyFaction>::iterator it = EnemyFactionList.begin();
//	for(;it != EnemyFactionList.end();)
//	{
//		if( ((*it).FactionGuid1 == FactionGuid1 && (*it).FactionGuid2 == FactionGuid2) ||
//			((*it).FactionGuid1 == FactionGuid2 && (*it).FactionGuid2 == FactionGuid1) )
//		{
//			it = EnemyFactionList.erase(it);
//		}
//		else
//		{
//			it++;
//		}
//	}
//}
//
////得到一个帮会的敌对帮会列表
//void CFactionWarSys::GetEnemyFactionsByFactionID(const CGUID& FactionGuid,set<COrganizing*>& FactionList,long& DisTime)
//{
//	list<tagEnemyFaction>::iterator it = EnemyFactionList.begin();
//	for(;it != EnemyFactionList.end();it++)
//	{
//		if( (*it).FactionGuid1 == FactionGuid )
//		{
//			COrganizing* pOrg = GetOrganiCtrl()->GetFactionOrganizing((*it).FactionGuid2);
//			if(pOrg && FactionList.find(pOrg) != FactionList.end())
//				FactionList.insert(pOrg);
//			if((*it).dwDisandTime > DisTime)
//				(*it).dwDisandTime = DisTime;
//		}
//		else if( (*it).FactionGuid2 == FactionGuid )
//		{
//			COrganizing* pOrg = GetOrganiCtrl()->GetFactionOrganizing((*it).FactionGuid1);
//			if(pOrg && FactionList.find(pOrg) != FactionList.end())
//				FactionList.insert(pOrg);
//			if((*it).dwDisandTime > DisTime)
//				(*it).dwDisandTime = DisTime;
//		}
//	}
//}
//
////帮会宣战
//bool CFactionWarSys::DigUpTheHatchet(const CGUID& DigGuid,const CGUID& BeDigedFactinID,long lLvl,tagTime& Time)
//{
//	//察看是否找得到此帮战类型
//	if(m_FactionWars.find(lLvl) == m_FactionWars.end())	return false;
//	long lFightTime = m_FactionWars[lLvl].lFightTime;
//	long lMoney     = m_FactionWars[lLvl].lMoney;
//
//	const CGUID& DigFactionID =  GetOrganiCtrl()->IsFactionMaster(DigGuid);
//	//只有帮主才能宣战
//	if(DigFactionID == NULL_GUID)
//	{
//		GetOrganiCtrl()->SendOrgaInfoToClient(DigGuid,string("只有帮主才能宣战。"),string("系统提示"));
//		return false;
//	}
//
//	//找不到帮会的实例
//	COrganizing* pDigFactin = GetOrganiCtrl()->GetFactionOrganizing(DigFactionID);
//	COrganizing* pBeDigedFaction = GetOrganiCtrl()->GetFactionOrganizing(BeDigedFactinID);
//
//	//找不到帮会实例
//	if(pDigFactin == NULL || pBeDigedFaction == NULL) return false;
//
//	//如果属于同一个盟则退出 
//	if(pDigFactin->IsSuperiorOrganizing() != NULL_GUID && 
//		pDigFactin->IsSuperiorOrganizing() == pBeDigedFaction->IsSuperiorOrganizing())
//	{
//		GetOrganiCtrl()->SendOrgaInfoToClient(DigGuid,
//											string("你和对方帮会属于同一个盟，不能宣战。"),
//											string("系统提示"));
//		return false;
//	}
//
//	CPlayer* pPlayer = GetGame()->GetMapPlayer(DigGuid);
//	if(pPlayer == NULL)	return false;
//
//	if(pPlayer->GetMoney() < lMoney)
//	{
//		char strSend[256]="";
//		sprintf(strSend,"宣战失败，你没有%d的金钱。",lMoney);
//		GetOrganiCtrl()->SendOrgaInfoToClient(DigGuid,string(strSend),string("系统提示"));
//		return false;
//	}
//
//
//	list<COrganizing*>	DigFactionList;DigFactionList.clear();
//	list<COrganizing*>  BeDigedFactionList;BeDigedFactionList.clear();
//
//	//得到宣战方所有的帮会列表
//    const CGUID& DigUnionID = GetOrganiCtrl()->IsFreeFaction(pDigFactin->GetExID());
//	if(DigUnionID == NULL_GUID)
//	{
//		DigFactionList.push_back(pDigFactin);
//	}
//	else
//	{
//		COrganizing* pDigUnion = GetOrganiCtrl()->GetUnionOrganizing(DigUnionID);
//		if(pDigUnion)
//			pDigUnion->GetMemberList(DigFactionList);
//	}
//	//得到被宣战方所有帮会列表
//	const CGUID& BeDigUnionID = GetOrganiCtrl()->IsFreeFaction(pBeDigedFaction->GetExID());
//	if(BeDigUnionID == NULL_GUID)
//	{
//		BeDigedFactionList.push_back(pBeDigedFaction);
//	}
//	else
//	{
//		COrganizing* pBeDigedUnion = GetOrganiCtrl()->GetUnionOrganizing(BeDigUnionID);
//		if(pBeDigedUnion)
//			pBeDigedUnion->GetMemberList(BeDigedFactionList);
//	}
//
//	list<COrganizing*>::iterator itDig = DigFactionList.begin();
//	for(;itDig != DigFactionList.end();itDig++)
//	{
//		COrganizing* pDigOrg = (*itDig);
//		list<COrganizing*>::iterator itBeDig = BeDigedFactionList.begin();
//		for(;itBeDig != BeDigedFactionList.end();itBeDig++)
//		{
//			COrganizing* pBeDigedOrg = (*itBeDig);
//			//添加敌对关系
//			AddOneEnmeyFaction(pDigOrg->GetExID(),pBeDigedOrg->GetExID(),lFightTime);
//
//			//向一个帮会添加敌对组织
//			pDigOrg->AddEnemyOrganizing(pBeDigedOrg->GetExID());
//			pBeDigedOrg->AddEnemyOrganizing(pDigOrg->GetExID());
//		}
//	}
//
//	string strFacNames;
//	string strFacNames1;
//	//更新相关敌对帮会信息到玩家和客户端
//	itDig = DigFactionList.begin();
//	for(;itDig != DigFactionList.end();itDig++)
//	{
//		COrganizing* pDigOrg = (*itDig);
//		if(pDigOrg)
//		{
//			pDigOrg->UpdateEnemyFaction();
//
//			strFacNames += pDigOrg->GetName();
//			strFacNames += ",";
//		}
//	}
//	strFacNames = strFacNames.substr(0,strFacNames.length()-1);
//
//	list<COrganizing*>::iterator itBeDig = BeDigedFactionList.begin();
//	for(;itBeDig != BeDigedFactionList.end();itBeDig++)
//	{
//		COrganizing* pBeDigedOrg = (*itBeDig);
//		if(pBeDigedOrg)
//		{
//			pBeDigedOrg->UpdateEnemyFaction();
//
//			strFacNames1 += pBeDigedOrg->GetName();
//			strFacNames1 += ",";
//		}
//	}
//	strFacNames1 = strFacNames1.substr(0,strFacNames1.length()-1);
//
//	//系统公告
//	char strMsg[4096]="";
//	sprintf(strMsg,"系统时间%s,{%s}向{%s}宣战了。",Time.GetDateFormatStr(),strFacNames.c_str(),strFacNames1.c_str());
//
//	GetOrganiCtrl()->SendOrgaInfoToClient(string(strMsg),
//		MAKELONG(MAKEWORD(146,254), MAKEWORD(255,255)),MAKELONG(MAKEWORD(0,0), MAKEWORD(255,255)));
//
//	//输出战斗调试信息
//	PutWarDebugString(strMsg);
//
//	return true;
//}
//
//
////玩家死亡时触发
//void CFactionWarSys::OnPlayerDied(const CGUID& PlayerID, const CGUID& Killer)
//{
//	//检查被杀死者是否是帮主或盟主
//	const CGUID& FactionID = GetOrganiCtrl()->IsFactionMaster(PlayerID);
//	if(FactionID == NULL_GUID)	return;
//	CGUID UnionID = GetOrganiCtrl()->IsFreeFaction(FactionID);
//	if(UnionID != NULL_GUID)
//	{
//		UnionID = GetOrganiCtrl()->IsConferationMaster(FactionID);
//		if(UnionID == NULL_GUID)	return;
//	}
//	
//	//检查杀人者是否有帮会
//	const CGUID& KillFactionID = GetOrganiCtrl()->IsFreePlayer(Killer);
//	if(KillFactionID == NULL_GUID)	return;
//
//
//	COrganizing* pFaction = GetOrganiCtrl()->GetFactionOrganizing(FactionID);
//	COrganizing* pKillFaction = GetOrganiCtrl()->GetFactionOrganizing(KillFactionID);
//
//	if(pFaction == NULL || pKillFaction == NULL)	return;
//
//	//检查这个两个帮会是否是敌对状态
//	if(IsEnemyRelation(FactionID,KillFactionID) == false)	return;
//
//	list<COrganizing*>	FactionList;FactionList.clear();
//	list<COrganizing*>  KillFactionList;KillFactionList.clear();
//
//	//得到宣战方所有的帮会列表
//    UnionID = GetOrganiCtrl()->IsFreeFaction(pFaction->GetExID());
//	if(UnionID == NULL_GUID)
//	{
//		FactionList.push_back(pFaction);
//	}
//	else
//	{
//		COrganizing* pUnion = GetOrganiCtrl()->GetUnionOrganizing(UnionID);
//		if(pUnion)
//			pUnion->GetMemberList(FactionList);
//	}
//	//得到被宣战方所有帮会列表
//	const CGUID& KillUnionID = GetOrganiCtrl()->IsFreeFaction(pKillFaction->GetExID());
//	if(KillUnionID == NULL_GUID)
//	{
//		KillFactionList.push_back(pKillFaction);
//	}
//	else
//	{
//		COrganizing* pKillUnion = GetOrganiCtrl()->GetUnionOrganizing(KillUnionID);
//		if(pKillUnion)
//			pKillUnion->GetMemberList(KillFactionList);
//	}
//
//	//消除帮会间的敌对状态
//	list<COrganizing*>::iterator it = FactionList.begin();
//	for(;it != FactionList.end();it++)
//	{
//		COrganizing* pOrg = (*it);
//		list<COrganizing*>::iterator itKill = KillFactionList.begin();
//		for(;itKill != KillFactionList.end();itKill++)
//		{
//			COrganizing* pKillOrg = (*itKill);
//			//清除敌对关系
//			ClearEnemyFaction(pOrg->GetExID(),pKillOrg->GetExID());
//
//			//向一个帮会添加敌对组织
//			pOrg->DelEnemyOrganizing(pKillOrg->GetExID());
//			pKillOrg->DelEnemyOrganizing(pOrg->GetExID());
//		}
//	}
//
//	string strFacNames;
//	string strKillFacNames;
//	//更新相关敌对帮会信息到玩家和客户端
//	it = FactionList.begin();
//	for(;it != FactionList.end();it++)
//	{
//		COrganizing* pOrg = (*it);
//		if(pOrg)
//		{
//			pOrg->UpdateEnemyFaction();
//			strFacNames += pOrg->GetName();
//			strFacNames += ",";
//		}
//	}
//	strFacNames = strFacNames.substr(0,strFacNames.length()-1);
//	list<COrganizing*>::iterator itKill = KillFactionList.begin();
//	for(;itKill != KillFactionList.end();itKill++)
//	{
//		COrganizing* pKillOrg = (*itKill);
//		if(pKillOrg)
//		{
//			pKillOrg->UpdateEnemyFaction();
//			strKillFacNames += pKillOrg->GetName();
//			strKillFacNames += ",";
//		}
//	}
//	strKillFacNames = strKillFacNames.substr(0,strKillFacNames.length()-1);
//
//	//系统公告
//	char strMsg[4096]="";
//	sprintf(strMsg,"{%s}在帮会战中战胜了{%s}。",strKillFacNames.c_str(),strFacNames.c_str());
//
//	GetOrganiCtrl()->SendOrgaInfoToClient(string(strMsg),
//		MAKELONG(MAKEWORD(146,254), MAKEWORD(255,255)),MAKELONG(MAKEWORD(0,0), MAKEWORD(255,255)));
//
//	//输出战斗调试信息
//	PutWarDebugString(strMsg);
//}
//
//
////终止帮战
//void CFactionWarSys::StopFactionWar(const CGUID& FactionGuid1,const CGUID& FactionGuid2)
//{
//	//检查这个两个帮会是否是敌对状态
//	if(IsEnemyRelation(FactionGuid1,FactionGuid2) == false)	return;
//
//	COrganizing* pFaction1 = GetOrganiCtrl()->GetFactionOrganizing(FactionGuid1);
//	COrganizing* pFaction2 = GetOrganiCtrl()->GetFactionOrganizing(FactionGuid2);
//
//	if(pFaction1 == NULL || pFaction2 == NULL)
//	{
//		ClearEnemyFaction(FactionGuid1,FactionGuid2);
//		return;
//	}
//
//	list<COrganizing*>	FactionList1;FactionList1.clear();
//	list<COrganizing*>  FactionList2;FactionList2.clear();
//
//	//得到宣战方所有的帮会列表
//	CGUID UnionID = GetOrganiCtrl()->IsFreeFaction(FactionGuid1);
//	if(UnionID == NULL_GUID)
//	{
//		FactionList1.push_back(pFaction1);
//	}
//	else
//	{
//		COrganizing* pUnion = GetOrganiCtrl()->GetUnionOrganizing(UnionID);
//		if(pUnion)
//			pUnion->GetMemberList(FactionList1);
//	}
//	//得到被宣战方所有帮会列表
//	UnionID = GetOrganiCtrl()->IsFreeFaction(FactionGuid2);
//	if(UnionID == NULL_GUID)
//	{
//		FactionList2.push_back(pFaction2);
//	}
//	else
//	{
//		COrganizing* pUnion = GetOrganiCtrl()->GetUnionOrganizing(UnionID);
//		if(pUnion)
//			pUnion->GetMemberList(FactionList2);
//	}
//
//	//消除帮会间的敌对状态
//	list<COrganizing*>::iterator it = FactionList1.begin();
//	for(;it != FactionList1.end();it++)
//	{
//		COrganizing* pOrg = (*it);
//		list<COrganizing*>::iterator it1 = FactionList2.begin();
//		for(;it1 != FactionList2.end();it1++)
//		{
//			COrganizing* pOrg1 = (*it1);
//			//清除敌对关系
//			ClearEnemyFaction(pOrg->GetExID(),pOrg1->GetExID());
//
//			//向一个帮会添加敌对组织
//			pOrg->DelEnemyOrganizing(pOrg1->GetExID());
//			pOrg1->DelEnemyOrganizing(pOrg->GetExID());
//		}
//	}
//
//	string strFacNames;
//	string strFacNames1;
//	//更新相关敌对帮会信息到玩家和客户端
//	it = FactionList1.begin();
//	for(;it != FactionList1.end();it++)
//	{
//		COrganizing* pOrg = (*it);
//		if(pOrg)
//		{
//			pOrg->UpdateEnemyFaction();
//			strFacNames += pOrg->GetName();
//			strFacNames += ",";
//		}
//	}
//	strFacNames = strFacNames.substr(0,strFacNames.length()-1);
//	it = FactionList2.begin();
//	for(;it != FactionList2.end();it++)
//	{
//		COrganizing* pOrg = (*it);
//		if(pOrg)
//		{
//			pOrg->UpdateEnemyFaction();
//			strFacNames1 += pOrg->GetName();
//			strFacNames1 += ",";
//		}
//	}
//	strFacNames1 = strFacNames1.substr(0,strFacNames1.length()-1);
//
//	//系统公告
//	char strMsg[4096]="";
//	sprintf(strMsg,"{%s}和{%s}的帮会战斗结束。",strFacNames1.c_str(),strFacNames.c_str());
//
//	GetOrganiCtrl()->SendOrgaInfoToClient(string(strMsg),
//		MAKELONG(MAKEWORD(146,254), MAKEWORD(255,255)),MAKELONG(MAKEWORD(0,0), MAKEWORD(255,255)));
//
//	//输出战斗调试信息
//	PutWarDebugString(strMsg);
//}
////帮战时间已到触发
//void CFactionWarSys::OnTimeOut(const CGUID& FactionGuid1,const CGUID& FactionGuid2)
//{
//	StopFactionWar(FactionGuid1,FactionGuid2);
//}
//
//
//void CFactionWarSys::GenerateSaveData()
//{
////	list<tagEnemyFaction*> ListEnemyFactions;
////	list<tagEnemyFaction>::iterator it  = EnemyFactionList.begin();
////	for(;it != EnemyFactionList.end();it++)
////	{
////		tagEnemyFaction* pEnemyFaction =
////			new tagEnemyFaction((*it).FactionGuid1,(*it).FactionGuid2,(*it).dwDisandTime);
////		ListEnemyFactions.push_back(pEnemyFaction);
////	}
////	GetGame()->SetEnemyFactions(ListEnemyFactions);
//}
//
//CFactionWarSys* GetFactionWarSys()
//{
//	return CFactionWarSys::GetInstance();
//}