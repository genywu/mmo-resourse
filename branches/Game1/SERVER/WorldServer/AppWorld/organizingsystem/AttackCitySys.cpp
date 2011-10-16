///////////////////////////////////////////////////////////////////////////////////////
//AttackCitySys.cpp
//攻城战系统
//Function:完成攻城战系统的时间控制等等
//Author:Joe
//Create Time:2004.7.29
#include "StdAfx.h"
#include ".\attackcitysys.h"
#include ".\villagewarsys.h"
#include "..\nets\networld\message.h"

#include "OrganizingCtrl.h"
#include "..\WorldRegion.h"
#include "..\Player.h"
#include "Organizing.h"
#include "organizingparam.h"
#include "Faction.h"

#include "..\Country\Country.h"
#include "..\Country\CountryHandler.h"
#include "../GlobalRgnManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//map<long,tagACT> CAttackCitySys::m_Attacks;
//CAttackCitySys::tagGetTodayTaxTime CAttackCitySys::m_GetTodayTaxTime;
//CAttackCitySys* CAttackCitySys::instance = NULL;
//CAttackCitySys::CAttackCitySys(void)
//{
//}
//
//CAttackCitySys::~CAttackCitySys(void)
//{
//}
//
////得到唯一实例句柄
//CAttackCitySys* CAttackCitySys::getInstance()
//{
//	if(instance==NULL)
//	{
//		instance=new CAttackCitySys;
//	}
//	return instance;
//}
////初始化
//bool   CAttackCitySys::Initialize()
//{
//	return true;
//	m_Attacks.clear();
//
//	char filename[]="setup/CityWarSys.ini";
//
//	ifstream stream;
//	stream.open(filename);
//	if( !stream.is_open() )
//	{
//		char str[]="setup/CityWarSys.ini can't found!";
//		AddLogText(str);
//		return false;
//	}
//
//	tagTime CurrTime;
//	string strDate;
//	long lTime = 0;
//	map<long,tagTime> LastEndWarTime;LastEndWarTime.clear();
//	while(ReadTo(stream, "*"))
//	{
//		lTime++;
//		tagAttackCityTime	AttackCityTime;
//		AttackCityTime.lTime=lTime;
//		long lStartInfoTime,lDecTime,lEndInfoTime,lLastTime,lMassTime,lClearTime,lRefreshTime;
//		stream>>AttackCityTime.lCityRegionID
//			>>strDate
//			>>lStartInfoTime
//			>>lDecTime
//			>>lEndInfoTime
//			>>lLastTime
//			>>lMassTime
//			>>lClearTime
//			>>lRefreshTime;
//
//		AttackCityTime.RegionState = CIS_NO;
//		AttackCityTime.DecWarFactions.clear();
//		tagTime Time(strDate.c_str());
//		AttackCityTime.AttackCityStartTime=Time;
//
//		//开始提示时间
//		tagTime TemptTime = Time;
//		TemptTime.AddMinute(lStartInfoTime);
//		AttackCityTime.AttackCityStartInfoTime=TemptTime;
//
//		TemptTime = Time;
//		TemptTime.AddMinute(lDecTime);
//		AttackCityTime.DeclarWarTime=TemptTime;
//
//		//结束提示时间
//		TemptTime = Time;
//		TemptTime.AddMinute(lEndInfoTime);
//		AttackCityTime.AttackCityEndInfoTime=TemptTime;
//
//		TemptTime = Time;
//		TemptTime.AddMinute(lLastTime);
//		AttackCityTime.AttackCityEndTime=TemptTime;
//
//		TemptTime = Time;
//		TemptTime.AddMinute(lMassTime);
//		AttackCityTime.MassTime=TemptTime;
//
//		TemptTime = Time;
//		TemptTime.AddSecond(lClearTime);
//		AttackCityTime.ClearPlayerTime=TemptTime;
//
//		TemptTime = Time;
//		TemptTime.AddSecond(lRefreshTime);
//		AttackCityTime.RefreshRegionTime=TemptTime;
//
//		//错误检查
//		if( AttackCityTime.DeclarWarTime >= AttackCityTime.MassTime  ||
//			AttackCityTime.DeclarWarTime >= AttackCityTime.AttackCityStartTime ||
//			AttackCityTime.DeclarWarTime >= AttackCityTime.AttackCityEndTime )
//		{
//			AddLogText("Load ACityWarSys.ini Error!");
//			return false;
//		}
//
//		if( AttackCityTime.MassTime >= AttackCityTime.AttackCityStartTime ||
//			AttackCityTime.MassTime >= AttackCityTime.AttackCityEndTime )
//		{
//			AddLogText("Load CityWarSys.ini Error!");
//			return false;
//		}
//
//		if( AttackCityTime.AttackCityStartTime >= AttackCityTime.AttackCityEndTime )
//		{
//			AddLogText("Load CityWarSys.ini Error!");
//			return false;
//		}
//
//		if(AttackCityTime.AttackCityStartInfoTime >= AttackCityTime.AttackCityStartTime)
//		{
//			AddLogText("Load CityWarSys.ini Error!");
//			return false;
//		}
//
//		if(AttackCityTime.AttackCityEndInfoTime >= AttackCityTime.AttackCityEndTime)
//		{
//			AddLogText("Load CityWarSys.ini Error!");
//			return false;
//		}
//
//
//		//错误检查
//		m_Attacks[lTime]=AttackCityTime;
//		map<long,tagTime>::iterator itEndTime = LastEndWarTime.find( AttackCityTime.lCityRegionID );
//		if( itEndTime  != LastEndWarTime.end() )
//		{
//			if( AttackCityTime.AttackCityStartTime <= (*itEndTime).second )
//			{
//				AddLogText("Load CityWarSys.ini Error!");
//				return false;
//			}
//		}
//		LastEndWarTime[AttackCityTime.lCityRegionID] = AttackCityTime.AttackCityEndTime;
//	}
//	stream.close();
//
//
//	// 注册时间事件
//	itAttack it = m_Attacks.begin();
//	for(;it != m_Attacks.end();it++)
//	{
//		long lTemptTime = (*it).first;
//		if((*it).second.AttackCityEndTime >= CurrTime)
//		{
//			(*it).second.lEndEventID = GetTimer()->SetTimeEvent((*it).second.AttackCityEndTime,
//				(CallBackFun)CAttackCitySys::OnAttackCityEnd,lTemptTime);
//
//			//注册结束提示时间
//			if( (*it).second.AttackCityEndInfoTime >= CurrTime )
//			{
//				(*it).second.lEndInfoEventID = GetTimer()->SetTimeEvent((*it).second.AttackCityEndInfoTime,
//				(CallBackFun)CAttackCitySys::OnAttackCityEndnfo,lTemptTime);
//			}
//			else
//			{
//				(*it).second.lEndInfoEventID = GetTimer()->SetTimeEvent(CurrTime,
//				(CallBackFun)CAttackCitySys::OnAttackCityEndnfo,lTemptTime);
//			}
//
//			if((*it).second.AttackCityStartTime>=CurrTime )
//			{
//				(*it).second.lClearEventID = GetTimer()->SetTimeEvent((*it).second.ClearPlayerTime,
//					(CallBackFun)CAttackCitySys::OnClearOtherPlayer,lTemptTime);
//
//				(*it).second.lStartEventID =GetTimer()->SetTimeEvent((*it).second.AttackCityStartTime,
//				(CallBackFun)CAttackCitySys::OnAttackCityStart,lTemptTime);
//
//				(*it).second.lRefreshEventID = GetTimer()->SetTimeEvent((*it).second.RefreshRegionTime,
//					(CallBackFun)CAttackCitySys::OnRefreshRegion,lTemptTime);
//
//				//注册开始提示时间
//				if( (*it).second.AttackCityStartInfoTime >= CurrTime )
//				{
//					(*it).second.lStartInfoEventID = GetTimer()->SetTimeEvent((*it).second.AttackCityStartInfoTime,
//					(CallBackFun)CAttackCitySys::OnAttackCityStartInfo,lTemptTime);
//				}
//				else
//				{
//					(*it).second.lStartInfoEventID = GetTimer()->SetTimeEvent(CurrTime,
//					(CallBackFun)CAttackCitySys::OnAttackCityStartInfo,lTemptTime);
//				}
//
//				if((*it).second.MassTime>=CurrTime )
//				{
//					(*it).second.lMassEventID = GetTimer()->SetTimeEvent((*it).second.MassTime,
//					(CallBackFun)CAttackCitySys::OnMass,lTemptTime);
//
//					if( (*it).second.DeclarWarTime >= CurrTime )
//					{
//						(*it).second.lDeclarWarEventID = GetTimer()->SetTimeEvent((*it).second.DeclarWarTime,
//							(CallBackFun)CAttackCitySys::OnDeclarWar,lTemptTime);
//					}
//					else
//					{
//						(*it).second.RegionState = CIS_DUTH;
//					}
//				}
//				else
//				{
//					(*it).second.RegionState = CIS_Mass;
//				}
//			}
//			else
//			{
//				(*it).second.RegionState = CIS_Fight;
//			}
//		}							
//	}
//	return true;
//}
//
//void	CAttackCitySys::SetDecWarFactionsFromDB(long lRegionID,tagTime& WarStartTime,list<CGUID>& DecWarFactions)
//{
//	itAttack it = m_Attacks.begin();
//	for(;it != m_Attacks.end();it++)
//	{
//		if( (*it).second.lCityRegionID == lRegionID && WarStartTime == (*it).second.AttackCityStartTime &&
//			(*it).second.RegionState != CIS_NO)
//		{
//			(*it).second.DecWarFactions = DecWarFactions;
//			return;
//		}
//	}
//}
//
////添加到CByteArray
//bool CAttackCitySys::AddToByteArray(vector<BYTE>& ByteArray)
//{
//	_AddToByteArray(&ByteArray,(long)m_Attacks.size());
//	itAttack it = m_Attacks.begin();
//	for(;it != m_Attacks.end();it++)
//	{
//		tagACT &AttackCityTime = (*it).second;
//		_AddToByteArray(&ByteArray,&AttackCityTime,sizeof(tagACT)-sizeof(list<long>) );
//		//添加宣战帮会列表
//		_AddToByteArray(&ByteArray,(long)AttackCityTime.DecWarFactions.size());
//		list<CGUID>::iterator itFac= AttackCityTime.DecWarFactions.begin();
//		for(;itFac != AttackCityTime.DecWarFactions.end();itFac++)
//		{
//			_AddToByteArray(&ByteArray,(*itFac));
//		}
//	}
//	return true;
//}
//
////添加宣战帮会到CByteArray
//bool CAttackCitySys::UpdateApplyWarFacsToGameServer(long lWarID,vector<BYTE>& ByteArray)
//{
//	list<CGUID>	DecFacs;
//	itAttack it = m_Attacks.find(lWarID);
//	if(it != m_Attacks.end())
//	{
//		DecFacs = (*it).second.DecWarFactions;
//	}
//	_AddToByteArray(&ByteArray,lWarID);
//	_AddToByteArray(&ByteArray,(long)DecFacs.size());
//	list<CGUID>::iterator itFac = DecFacs.begin();
//	for(;itFac != DecFacs.end();itFac++)
//	{
//		_AddToByteArray(&ByteArray,(CGUID)(*itFac));
//	}
//	return true;
//}
//
//void CAttackCitySys::GenerateSaveData()
//{
///*	list<tagAttackCityTime*> TemptCityWarSetupList;
//	itAttack it = m_Attacks.begin();
//	for(;it != m_Attacks.end();it++)
//	{
//		tagAttackCityTime* pSetup = new tagAttackCityTime;
//		pSetup->lCityRegionID = (*it).second.lCityRegionID;
//		pSetup->AttackCityStartTime = (*it).second.AttackCityStartTime;
//		if((*it).second.RegionState != CIS_NO)
//			pSetup->DecWarFactions = (*it).second.DecWarFactions;
//		else
//			pSetup->DecWarFactions.clear();
//		TemptCityWarSetupList.push_back(pSetup);
//	}
//	GetGame()->SetCityWarParam(TemptCityWarSetupList);*/
//}
//
//bool  CAttackCitySys::Reload()
//{
//	itAttack it = m_Attacks.begin();
//	for(;it != m_Attacks.end();it++)
//	{
//		tagAttackCityTime&	AttackCityTime=(*it).second;
//		GetTimer()->KillTimeEvent(AttackCityTime.lDeclarWarEventID);
//		GetTimer()->KillTimeEvent(AttackCityTime.lMassEventID);
//		GetTimer()->KillTimeEvent(AttackCityTime.lClearEventID);
//		GetTimer()->KillTimeEvent(AttackCityTime.lRefreshEventID);
//		GetTimer()->KillTimeEvent(AttackCityTime.lStartInfoEventID);
//		GetTimer()->KillTimeEvent(AttackCityTime.lStartEventID);
//		GetTimer()->KillTimeEvent(AttackCityTime.lEndInfoEventID);
//		GetTimer()->KillTimeEvent(AttackCityTime.lEndEventID);
//
//		GetTimer()->KillTimeEvent(m_GetTodayTaxTime.lEventID);
//	}
//
//	map<long,tagACT> TemptAttacks = m_Attacks;
//	itAttack itAttack = TemptAttacks.begin();
//	for(;itAttack != TemptAttacks.end();itAttack++)
//	{
//		if( (*itAttack).second.RegionState != CIS_NO )
//		{
//			GetAttackCitySys()->OnCityWarEnd( (*itAttack).first );
//			GetAttackCitySys()->OnCityWarEndToGameServer((*itAttack).second.lCityRegionID,(*itAttack).first);
//		}
//	}
//
//	Initialize();
//	return true;
//}
////释放资源
//void   CAttackCitySys::Release()
//{
//	SAFE_DELETE(instance);
//}
//
//long	CAttackCitySys::GetWarRegionIDByTime(long lTime)
//{
//	itAttack it = m_Attacks.find(lTime);
//	if(it == m_Attacks.end())	return 0;
//	return (*it).second.lCityRegionID;
//}
//
////攻城开始
//void CALLBACK CAttackCitySys::OnAttackCityStart(LPARAM lparam)
//{
//	long lTime = (long)lparam;
//	if(m_Attacks.find(lTime) == m_Attacks.end())	return;
//
//	m_Attacks[lTime].RegionState=CIS_Fight;
//	CMessage msg(MSG_W2S_ATTACKCITY_Start);
//	msg.Add(lTime);
//	msg.SendAll();
//	
//	long lRegionID = GetAttackCitySys()->GetWarRegionIDByTime(lTime);
//	if(lRegionID == 0)	return;
//
//	//CGame::tagRegion* pRegion = GetGame()->GetRegion(lRegionID);
//	CGlobalRgnManager::tagRegion* pRegion = GetGame()->GetGlobalRgnManager()->GetTemplateNormalRegion(lRegionID);
//	if(pRegion == NULL || pRegion->pRegion == NULL)	return;
//	char strSend[256]="";
//	sprintf(strSend,"%s城战已开始。",pRegion->pRegion->GetName());
//	GetOrganiCtrl()->SendOrgaInfoToClient(string(strSend),
//		MAKELONG(MAKEWORD(146,254), MAKEWORD(255,255)),MAKELONG(MAKEWORD(0,0), MAKEWORD(255,255)));
//
//	//输出战斗调试信息
//	char strWarInfo[256];
//	sprintf(strWarInfo,"(Num:%d){%s}城战已经开始。",lTime,pRegion->pRegion->GetName());
//	PutWarDebugString(strWarInfo);
//}
////攻城结束
//void CALLBACK CAttackCitySys::OnAttackCityEnd(LPARAM lparam)
//{	
//	long lTime = (long)lparam;
//	if(m_Attacks.find(lTime) == m_Attacks.end())	return;
//	if(m_Attacks[lTime].RegionState != CIS_Fight)	return;
//
//	m_Attacks[lTime].RegionState = CIS_NO;
//	CMessage msg(MSG_W2S_ATTACKCITY_TimeOut);
//	msg.Add(lTime);
//	msg.SendAll();
//
//	//输出战斗调试信息
//	char strWarInfo[256];
//	sprintf(strWarInfo,"(Num:%d) 城战时间结束。",lTime);
//	PutWarDebugString(strWarInfo);
//}
//
////主城争霸战结束
//void	CAttackCitySys::OnCityWarEnd(long lWarNum)
//{
//	long lTime = lWarNum;
//	m_Attacks[lTime].DecWarFactions.clear();
//
//	//在城战结束后更新帮会城战敌对列表
//	UpdateCityAllFactionEnemyRelation();
//}
//
////发送结束消息到Gameserver
//void    CAttackCitySys::OnCityWarEndToGameServer(long lRegionID,long lTime)
//{
//	CMessage msg(MSG_W2S_ATTACKCITY_End);
//	msg.Add(lTime);
//	msg.SendAll();
//}
//
////攻城宣战
//void CALLBACK CAttackCitySys::OnDeclarWar(LPARAM lparam)
//{
//	//逻辑处理
//	long lTime = (long)lparam;
//	if(m_Attacks.find(lTime) == m_Attacks.end())	return;
//
//	m_Attacks[lTime].RegionState=CIS_DUTH;
//	CMessage msg(MSG_W2S_ATTACKCITY_Declar);
//	msg.Add(lTime);
//	msg.SendAll();
//
//
//	//相关提示信息
//	long lRegionID = GetAttackCitySys()->GetWarRegionIDByTime(lTime);
//	if(lRegionID == 0)	return;
//
//	//CGame::tagRegion* pRegion = GetGame()->GetRegion(lRegionID);
//	CGlobalRgnManager::tagRegion* pRegion = GetGame()->GetGlobalRgnManager()->GetTemplateNormalRegion(lRegionID);
//
//	if(pRegion == NULL || pRegion->pRegion == NULL)	return;
//	char strSend[256]="";
//	sprintf(strSend,"%s城战已开始报名。",pRegion->pRegion->GetName());
//	GetOrganiCtrl()->SendOrgaInfoToClient(string(strSend),
//		MAKELONG(MAKEWORD(146,254), MAKEWORD(255,255)),MAKELONG(MAKEWORD(0,0), MAKEWORD(255,255)));
//
//	//输出战斗调试信息
//	char strWarInfo[256];
//	sprintf(strWarInfo,"(Num:%d){%s}城战已开始报名。",lTime,pRegion->pRegion->GetName());
//	PutWarDebugString(strWarInfo);
//}
////攻城集结
//void CALLBACK CAttackCitySys::OnMass(LPARAM lparam)
//{
//	//逻辑处理
//	long lTime = (long)lparam;
//	if(m_Attacks.find(lTime) == m_Attacks.end())	return;
//
//	m_Attacks[lTime].RegionState = CIS_Mass;
//	CMessage msg(MSG_W2S_ATTACKCITY_Mass);
//	msg.Add(lTime);
//	msg.SendAll();
//
//	//相关提示信息
//	long lRegionID = GetAttackCitySys()->GetWarRegionIDByTime(lTime);
//	if(lRegionID == 0)	return;
//
//	//CGame::tagRegion* pRegion = GetGame()->GetRegion(lRegionID);
//	CGlobalRgnManager::tagRegion* pRegion = GetGame()->GetGlobalRgnManager()->GetTemplateNormalRegion(lRegionID);
//
//	if(pRegion == NULL || pRegion->pRegion == NULL)	return;
//	char strSend[256]="";
//	sprintf(strSend,"%s城战已开始集结。",pRegion->pRegion->GetName());
//	GetOrganiCtrl()->SendOrgaInfoToClient(string(strSend),
//		MAKELONG(MAKEWORD(146,254), MAKEWORD(255,255)),MAKELONG(MAKEWORD(0,0), MAKEWORD(255,255)));
//
//	//输出战斗调试信息
//	char strWarInfo[256];
//	sprintf(strWarInfo,"(Num:%d){%s}城战已开始集结。",lTime,pRegion->pRegion->GetName());
//	PutWarDebugString(strWarInfo);
//}
////清除其它人员
//void CALLBACK CAttackCitySys::OnClearOtherPlayer(LPARAM lparam)
//{
//	long lTime = (long)lparam;
//	if(m_Attacks.find(lTime) == m_Attacks.end())	return;
//
//	CMessage msg(MSG_W2S_ATTACKCITY_ClearOtherPlayer);
//	msg.Add(lTime);
//	msg.SendAll();
//}
////刷新场景
//void CALLBACK CAttackCitySys::OnRefreshRegion(LPARAM lparam)
//{
//	long lTime = (long)lparam;
//	if(m_Attacks.find(lTime) == m_Attacks.end())	return;
//
//	CMessage msg(MSG_W2S_ATTACKCITY_RefreshRegion);
//	msg.Add(lTime);
//	msg.SendAll();
//}
//
////攻城开始倒计时提示信息
//void CALLBACK CAttackCitySys::OnAttackCityStartInfo(LPARAM lparam)
//{
//	long lTime = (long)lparam;
//	if(m_Attacks.find(lTime) == m_Attacks.end())	return;
//
//	long lRegionID = GetAttackCitySys()->GetWarRegionIDByTime(lTime);
//	if(lRegionID == 0)	return;
//	//CGame::tagRegion* pRegion = GetGame()->GetRegion(lRegionID);
//	CGlobalRgnManager::tagRegion* pRegion = GetGame()->GetGlobalRgnManager()->GetTemplateNormalRegion(lRegionID);
//	if(pRegion == NULL || pRegion->pRegion == NULL)	return;
//	char strTempt[256]="";
//	sprintf(strTempt,"距%s——主城争霸战[[[开战]]]还有",pRegion->pRegion->GetName());
//
//	tagTime CurrTime;
//	tagACT Attack = m_Attacks[lTime];
//	if(Attack.AttackCityStartTime <= CurrTime) return;
//	tagTime StartTime = Attack.AttackCityStartTime;
//
//	tagTime DifTime = StartTime.GetTimeDiff(CurrTime);
//	long lDifTime = (DifTime.Minute()*60+DifTime.Second())*1000;
//
//	long lTopInfoID = GetOrganiCtrl()->AddOneTopInfo(2,lDifTime,string(strTempt));
//	GetOrganiCtrl()->SendTopInfoToClient(lTopInfoID,2,lDifTime,string(strTempt));
//}
//
////攻城结束倒计时提示信息
//void CALLBACK CAttackCitySys::OnAttackCityEndnfo(LPARAM lparam)
//{
//	long lTime = (long)lparam;
//	if(m_Attacks.find(lTime) == m_Attacks.end())	return;
//
//	long lRegionID = GetAttackCitySys()->GetWarRegionIDByTime(lTime);
//	if(lRegionID == 0)	return;
//	//CGame::tagRegion* pRegion = GetGame()->GetRegion(lRegionID);
//	CGlobalRgnManager::tagRegion* pRegion = GetGame()->GetGlobalRgnManager()->GetTemplateNormalRegion(lRegionID);
//
//	if(pRegion == NULL || pRegion->pRegion == NULL)	return;
//	char strTempt[256]="";
//	sprintf(strTempt,"距%s——主城争霸战[[[结束]]]还有",pRegion->pRegion->GetName());
//
//	tagTime CurrTime;
//	tagACT Attack = m_Attacks[lTime];
//	if(Attack.AttackCityEndTime <=  CurrTime)	return;
//	tagTime EndTime = Attack.AttackCityEndTime;
//
//	if(Attack.RegionState != CIS_Fight)	return;
//
//	tagTime DifTime = EndTime.GetTimeDiff(CurrTime);
//	long lDifTime = (DifTime.Minute()*60+DifTime.Second())*1000;
//
//	long lTopInfoID = GetOrganiCtrl()->AddOneTopInfo(2,lDifTime,string(strTempt));
//	GetOrganiCtrl()->SendTopInfoToClient(lTopInfoID,2,lDifTime,string(strTempt));
//}
//
//
////得到一个场景的当前状态
//eCityState CAttackCitySys::GetCityState(long lRegionID)
//{
//	tagTime CurrTime;
//	itAttack it = m_Attacks.begin();
//	for(;it != m_Attacks.end();it++)
//	{
//		if( (*it).second.lCityRegionID == lRegionID )
//		{
//			if( CurrTime >= (*it).second.DeclarWarTime && CurrTime <= (*it).second.AttackCityEndTime )
//				return (*it).second.RegionState;
//		}
//	}
//	return CIS_NO;
//}
//
////得到战斗标示当前状态
//eCityState CAttackCitySys::GetCityStateByWarNum(long lWarTime)
//{
//
//	itAttack it = m_Attacks.find(lWarTime);
//	if(it != m_Attacks.end() )
//		return (*it).second.RegionState;
//	return CIS_NO;
//}
//
////得到当前时间的一个场景的攻城标示
//long CAttackCitySys::GetWarNumByRegionIDAndCurTime(long lRegionID)
//{
//	tagTime CurrTime;
//	itAttack it = m_Attacks.begin();
//	for(;it != m_Attacks.end();it++)
//	{
//		if( (*it).second.lCityRegionID == lRegionID )
//		{
//			if( CurrTime >= (*it).second.DeclarWarTime && CurrTime <= (*it).second.AttackCityEndTime )
//				return (*it).first;
//		}
//	}
//	return 0;
//}
//
//
////得到一个场景的宣战帮会
//void CAttackCitySys::GetDecWarFactions(long lRegionID,list<CGUID>& Factions)
//{
//	itAttack it = m_Attacks.begin();
//	for(;it != m_Attacks.end();it++)
//	{
//		if( (*it).second.lCityRegionID == lRegionID )
//		{
//			Factions =  (*it).second.DecWarFactions;
//			return;
//		}
//	}
//}
//
////得到一次战斗的宣战帮会
//void CAttackCitySys::GetDecWarFactionsByWarNum(long lWarNum,list<CGUID>& Factions)
//{
//	itAttack it = m_Attacks.find(lWarNum);
//	if( it != m_Attacks.end() )
//	{
//		Factions = (*it).second.DecWarFactions;
//	}
//}
//
////判断一个帮会是否宣战
//bool	CAttackCitySys::IsAlreadyDeclarForWar(long lWarNum,const CGUID& FactionGuid)
//{
//	if(m_Attacks.find(lWarNum) == m_Attacks.end() )	return false;
//	tagACT Attack = m_Attacks[lWarNum];
//	if( find(Attack.DecWarFactions.begin(),Attack.DecWarFactions.end(),FactionGuid) == Attack.DecWarFactions.end() )
//		return false;
//	return true;
//}
//
////判断一个帮会是否宣战
//bool	CAttackCitySys::IsAlreadyDeclarForWar(const CGUID& FactionGuid)
//{
//	list<CGUID>	DecFacs;
//	itAttack it = m_Attacks.begin();
//	for(;it != m_Attacks.end();it++)
//	{
//		if( (*it).second.RegionState != CIS_NO )
//		{
//			DecFacs = (*it).second.DecWarFactions;
//			if( find(DecFacs.begin(),DecFacs.end(),FactionGuid) != DecFacs.end() )
//				return true;
//		}
//	}
//	return false;
//}
//
////是否处于敌对关系
//bool	CAttackCitySys::IsEnemyRelation(const CGUID& FactionGuid1,const CGUID& FactionGuid2)
//{
//	const CGUID& UnionGuid1 = GetOrganiCtrl()->IsFreeFaction(FactionGuid1);
//	const CGUID& UnionGuid2 = GetOrganiCtrl()->IsFreeFaction(FactionGuid2);
//	itAttack it = m_Attacks.begin();
//	for(;it != m_Attacks.end();it++)
//	{
//		//CGame::tagRegion* pRegion = GetGame()->GetRegion( (*it).second.lCityRegionID );
//		CGlobalRgnManager::tagRegion* pRegion = GetGame()->GetGlobalRgnManager()->GetTemplateNormalRegion((*it).second.lCityRegionID);
//
//		if(pRegion == NULL || pRegion->pRegion == NULL)	continue;
//		if( find((*it).second.DecWarFactions.begin(),(*it).second.DecWarFactions.end(),FactionGuid1 ) != 
//			(*it).second.DecWarFactions.end() )
//		{
//			if(FactionGuid2 == pRegion->pRegion->GetOwnedCityFaction() ||
//				(NULL_GUID != UnionGuid1 && UnionGuid2 ==  pRegion->pRegion->GetOwnedCityUnion()) )
//				return true;
//		}
//		else if( find((*it).second.DecWarFactions.begin(),(*it).second.DecWarFactions.end(),FactionGuid2 ) != 
//			(*it).second.DecWarFactions.end() )
//		{
//			if(FactionGuid1 == pRegion->pRegion->GetOwnedCityFaction() ||
//				(NULL_GUID != UnionGuid1 && UnionGuid1 ==  pRegion->pRegion->GetOwnedCityUnion()) )
//				return true;
//		}
//	}
//	return false;
//}
//
//
////list查找函数
//bool	CAttackCitySys::FindInList(list<long>& TemptList,long lID)
//{
//	return find(TemptList.begin(),TemptList.end(),lID)  != TemptList.end();
//}
////当一个玩家向一个城宣战时
//bool	CAttackCitySys::OnPlayerDeclareWar(const CGUID& PlayerGuid,long lWarID,long lMoney)
//{
///*ZKZ20080408
//	const CGUID& FactionGuid = GetOrganiCtrl()->IsFactionMaster(PlayerGuid);
//	if(FactionGuid == NULL_GUID)	return false;
//	COrganizing* pDecWarOrg = GetOrganiCtrl()->GetFactionOrganizing(FactionGuid);
//	if(pDecWarOrg == NULL)	return false;
//	long lWarRegionID = GetWarRegionIDByTime(lWarID);
//	if(lWarRegionID == 0)	return false;
//	//CGame::tagRegion* pRegion = GetGame()->GetRegion(lWarRegionID);
//	CGlobalRgnManager::tagRegion* pRegion = GetGame()->GetGlobalRgnManager()->GetTemplateNormalRegion(lWarRegionID);
//	if(pRegion == NULL || pRegion->pRegion == NULL)	return false;
//
//	DWORD dwColor = MAKELONG(MAKEWORD(255,255), MAKEWORD(255,255));
//	DWORD dwBkColor = MAKELONG(MAKEWORD(0,0), MAKEWORD(255,255));
//	//判断帮会等级是否满足要求
//	if( pDecWarOrg->GetLvl()  < GetOrganizingParam()->GetAttackCityMinLvl() )
//	{
//		GetOrganiCtrl()->SendOrgaInfoToClient(PlayerGuid,string("你的帮会没达到足够的等级，不能参加国王争夺战。"),string("系统提示"),-1,dwColor,dwBkColor);
//		return false;
//	}
//
//	//判断是否可以宣战
//	if( GetCityStateByWarNum(lWarID) == CIS_NO )	return false;
//
//	//判断是否和城主是同盟
//	const CGUID& OwnedFactionGuid = pRegion->pRegion->GetOwnedCityFaction();
//	if(OwnedFactionGuid == FactionGuid)	return false;
//	const CGUID& UnionGuid = GetOrganiCtrl()->IsFreeFaction(FactionGuid);
//	const CGUID& OwnedUnionGuid = GetOrganiCtrl()->IsFreeFaction(OwnedFactionGuid);
//	if(UnionGuid != NULL_GUID && OwnedUnionGuid == UnionGuid)	return false;
//
//	//判断是否已经拥有主城或据点
//	if(pDecWarOrg->GetOwnedCities().size() > 0)	return false;
//
//	//判断是否已报名参加了一个城战
//	if( IsAlreadyDeclarForWar(FactionGuid) == true)	return false;
//	 //判断是否已经报名参加了一个据点争霸战
//	if(GetVilWarSys()->IsAlreadyDeclarForWar(FactionGuid) == true) return false;
//	
//	COrganizing* pOwnedFac = NULL;
//	//得到城主所有帮会列表
//	if(OwnedFactionGuid != NULL_GUID)
//	{	
//		COrganizing* pOwnedFac = GetOrganiCtrl()->GetFactionOrganizing(OwnedFactionGuid);
//	}
//
//	//添加宣战帮会列表
//	m_Attacks[lWarID].DecWarFactions.push_back( pDecWarOrg->GetExID() );	
//	
//	//更新申请的帮会列表
//	vector<BYTE>	vecWarFacData;
//	UpdateApplyWarFacsToGameServer(lWarID,vecWarFacData);
//	CMessage msg(MSG_W2S_CITYWAR_UpdateDecWarFacs);
//	if(vecWarFacData.size() > 0)
//		msg.AddEx(&vecWarFacData[0],vecWarFacData.size());
//	msg.SendAll();
//
//	//在宣战之后更新帮会城战敌对列表
//	UpdateCityAllFactionEnemyRelation();
//
//
//	char strSend[1024]="参加了国王争夺战。";
//	BYTE bCountry = pRegion->pRegion->GetCountry();
//	if(bCountry>=0 && bCountry<=4)
//	{		
//		sprintf(strSend,"%s的%s帮会决定参加%s国王争夺战。",CountryName[bCountry],
//		pDecWarOrg->GetName().c_str(),CountryName[bCountry]);
//	}
//	GetOrganiCtrl()->SendOrgaInfoToClient(string(strSend),
//			MAKELONG(MAKEWORD(146,254), MAKEWORD(255,255)),MAKELONG(MAKEWORD(0,0), MAKEWORD(255,255)));
//	//输出战斗调试信息
//	char strWarInfo[1024];
//	sprintf(strWarInfo,"(Num:%d){%s}帮会向主城{%s}宣战了.",
//				lWarID,pDecWarOrg->GetName().c_str(),pRegion->pRegion->GetName() );
//	PutWarDebugString(strWarInfo);
//*/
//	return true;
//}
//
////当个一个帮会赢得了城战
//void	CAttackCitySys::OnFacWinCity(long lWarNum,long lWarRegionID,const CGUID& FactionGuid,const CGUID& UnionGuid)
//{
//	char strWarInfo[256];
//	char szFactionGuid[40] = {0};
//	char szUnionGuid[40] = {0};
//	FactionGuid.tostring(szFactionGuid);
//	UnionGuid.tostring(szUnionGuid);
//	sprintf(strWarInfo,
//		"lWarNum:%d,lWarRegionID:%d,FactionGuid:%s,lUnionID:%s",
//		lWarNum,lWarRegionID,szFactionGuid,szUnionGuid);
//	PutWarDebugString(strWarInfo);
//
//	if(lWarNum == 0 || lWarRegionID == 0)	return;
//
//	itAttack it = m_Attacks.find(lWarNum);
//	if( it == m_Attacks.end() )	return;
//	tagACT Attack = (*it).second;
//	if(Attack.lCityRegionID != lWarRegionID)	return;
//
//	//CGame::tagRegion* pRegion = GetGame()->GetRegion(lWarRegionID);
//	CGlobalRgnManager::tagRegion* pRegion = GetGame()->GetGlobalRgnManager()->GetTemplateNormalRegion(lWarRegionID);
//
//	if(pRegion == NULL || pRegion->pRegion == NULL)	return;
//
//	//删除定时器里边的触发时间
//	GetTimer()->KillTimeEvent(Attack.lEndInfoEventID);
//	GetTimer()->KillTimeEvent(Attack.lEndEventID);
//
//	//结束 
//	OnCityWarEnd(lWarNum);
//	OnCityWarEndToGameServer(lWarRegionID,lWarNum);
//
//	//第一次城战没有任何帮会获胜
//	if(FactionGuid == NULL_GUID)
//	{	
//		char strSend[256]="";
//		sprintf(strSend,"%s主城争霸战结束。",pRegion->pRegion->GetName());
//		GetOrganiCtrl()->SendOrgaInfoToClient(string(strSend),
//			MAKELONG(MAKEWORD(146,254), MAKEWORD(255,255)),MAKELONG(MAKEWORD(0,0), MAKEWORD(255,255)));
//
//		//添加顶端提示信息
//		char strTopInfo[256]="";
//		sprintf(strTopInfo,"%s——主城争霸战结束，此次城战没有获胜帮会。",pRegion->pRegion->GetName());
//		GetOrganiCtrl()->SendTopInfoToClient(-1,1,2,string(strTopInfo));
//
//		GetOrganiCtrl()->SendOrgaInfoToClient(string(strTopInfo),
//			MAKELONG(MAKEWORD(146,254), MAKEWORD(255,255)),MAKELONG(MAKEWORD(0,0), MAKEWORD(255,255)));
//
//		//输出战斗调试信息
//		sprintf(strWarInfo,"(Num:%d){%s}主城争霸战结束，此次没有获胜方。",lWarNum,pRegion->pRegion->GetName());
//		PutWarDebugString(strWarInfo);
//		return;
//	}
//
//	const CGUID& OrgOwnedFactionGuid = pRegion->pRegion->GetOwnedCityFaction();
//	COrganizing* pWinFaction = GetOrganiCtrl()->GetFactionOrganizing(FactionGuid);
//	if(pWinFaction == NULL)	return;
//
//	const CGUID& TmpUnionGuid = GetOrganiCtrl()->IsFreeFaction(FactionGuid);
//
//	//是否是防守方获得胜利
//	bool bDefSide = true;
//
//	//如果攻城方胜利
//	if(OrgOwnedFactionGuid != FactionGuid)
//	{
//		bDefSide = false;
//		GetGame()->RefreshOwnedCityOrg(lWarRegionID,FactionGuid,TmpUnionGuid);
//
//		//给获胜帮会添加拥有的主城
//		pWinFaction->AddOwnedCity(lWarRegionID);
//
//		// 设置该帮帮主为该国国王
//		CFaction *pWinFac = (CFaction*)pWinFaction;
//		if(pWinFac)
//		{
//			GetCountryHandler()->GetCountry(pWinFac->GetCountry())->SetKing(pWinFaction->GetMasterID());
//		}
//		//给原来的帮会删去拥有的主城
//		COrganizing* pOwnedFaction =
//			GetOrganiCtrl()->GetFactionOrganizing(OrgOwnedFactionGuid);
//		if(pOwnedFaction)
//		{
//			pOwnedFaction->DelOwnedCity(lWarRegionID);
//		}
//		
//	}
//
//	//统计帮会排行榜
//	GetOrganiCtrl()->StatBillboard();
//
//	char strSend[256]="";
//	sprintf(strSend,"%s主城争霸战结束。%s帮会在%s城战中获胜，获得对%s的税收管理权力。",
//		pRegion->pRegion->GetName(),pWinFaction->GetName().c_str(),
//		pRegion->pRegion->GetName(),pRegion->pRegion->GetName());
//	GetOrganiCtrl()->SendOrgaInfoToClient(string(strSend),
//		MAKELONG(MAKEWORD(146,254), MAKEWORD(255,255)),MAKELONG(MAKEWORD(0,0), MAKEWORD(255,255)));
//
//	//添加顶端提示信息
//	char strTopInfo[256]="";
//	sprintf(strTopInfo,"%s取得了%s——主城争霸战的最终胜利",pWinFaction->GetName().c_str(),pRegion->pRegion->GetName());
//	GetOrganiCtrl()->SendTopInfoToClient(-1,1,2,string(strTopInfo));
//
//	//输出战斗调试信息
//	sprintf(strWarInfo,"(Num:%d){%s}帮会在{%s}城战中获胜，获得对{%s}城的税收管理权力。",lWarNum,
//		pWinFaction->GetName().c_str(),pRegion->pRegion->GetName(),pRegion->pRegion->GetName());
//	PutWarDebugString(strWarInfo);
//}
//
////更新所有的宣战帮会到GameServer
//void CAttackCitySys::UpdateAllDecFacToGameServer()
//{
//	itAttack it = m_Attacks.begin();
//	for(;it != m_Attacks.end();it++)
//	{
//		//CGame::tagRegion* pRegion = GetGame()->GetRegion( (*it).second.lCityRegionID );
//		CGlobalRgnManager::tagRegion* pRegion = GetGame()->GetGlobalRgnManager()->GetTemplateNormalRegion((*it).second.lCityRegionID);
//		if(pRegion == NULL || pRegion->pRegion == NULL)	continue;
//
//		//更新申请的帮会列表
//		vector<BYTE>	vecWarFacData;
//		UpdateApplyWarFacsToGameServer((*it).first,vecWarFacData);
//		CMessage msg(MSG_W2S_CITYWAR_UpdateDecWarFacs);
//		if(vecWarFacData.size() > 0)
//			msg.AddEx(&vecWarFacData[0],vecWarFacData.size());
//		msg.SendAll();
//	}
//}
//
////根据宣战情况更新帮会的敌对关系
//void	CAttackCitySys::InitialCityAllFactionEnemyRelation()
//{
//	//先清除所有的城战敌对帮会
//	GetOrganiCtrl()->ClearAllCityEneFacRelation();
//
//	list<CGUID>	DecFacs;
//	//添加城战敌对帮会
//	itAttack it = m_Attacks.begin();
//	for(;it != m_Attacks.end();it++)
//	{
//		if( (*it).second.RegionState == CIS_NO) continue;
//	
//		DecFacs = (*it).second.DecWarFactions;
//		SetCityWarEnemyFactions((*it).second.lCityRegionID,DecFacs);
//		//设置数据
//	}
//}
////根据宣战情况更新帮会的敌对关系
//void CAttackCitySys::UpdateCityAllFactionEnemyRelation()
//{
//	//设置所有帮会的城战敌对帮会的标志为false
//	GetOrganiCtrl()->SetAllCityFacEnemyChanged(false);
//	
//	InitialCityAllFactionEnemyRelation();
//
//	//最后更新所有变化了的城战敌对关系列表
//	GetOrganiCtrl()->UpdateAllCityEneFacRelation();
//}
//
////根据装载的宣战帮会设置敌对关系
//void	CAttackCitySys::SetCityWarEnemyFactions(long lRegionID,list<CGUID>& DecWarFactions)
//{
//	if(DecWarFactions.size() == 0)	return;
//
//	//CGame::tagRegion* pRegion = GetGame()->GetRegion( lRegionID );
//	CGlobalRgnManager::tagRegion* pRegion = GetGame()->GetGlobalRgnManager()->GetTemplateNormalRegion(lRegionID);
//	if( pRegion == NULL || pRegion->pRegion == NULL )	return;
//	const CGUID& OwnedFaction = pRegion->pRegion->GetOwnedCityFaction();
//	if( OwnedFaction == NULL_GUID )	return;
//
//	//得到城主方
//	list<COrganizing*>	OwnedFactiosList;OwnedFactiosList.clear();
//	const CGUID& OwnedUnionID = GetOrganiCtrl()->IsFreeFaction(OwnedFaction);
//	if( OwnedUnionID == NULL_GUID )
//	{
//		COrganizing* pOwnedFaction = GetOrganiCtrl()->GetFactionOrganizing(OwnedFaction);
//		if(pOwnedFaction)
//			OwnedFactiosList.push_back(pOwnedFaction);
//	}
//	else
//	{
//		COrganizing* pOwnedUnion = GetOrganiCtrl()->GetUnionOrganizing(OwnedUnionID);
//		if(pOwnedUnion)
//			pOwnedUnion->GetMemberList(OwnedFactiosList);
//	}
//
//	//得到宣战方所有成员
//	list<COrganizing*>	DecWarFactionList;DecWarFactionList.clear();
//	list<CGUID>::iterator it = DecWarFactions.begin();
//	for(; it != DecWarFactions.end();it++)
//	{
//		const CGUID& FactionGuid = (*it);
//		const CGUID& UnionID = GetOrganiCtrl()->IsFreeFaction(FactionGuid);
//		if(UnionID == NULL_GUID )
//		{
//			COrganizing* pFaction = GetOrganiCtrl()->GetFactionOrganizing(FactionGuid);
//			if(pFaction)
//				DecWarFactionList.push_back(pFaction);
//		}
//		else
//		{
//			COrganizing* pUnion = GetOrganiCtrl()->GetUnionOrganizing(UnionID);
//			if(pUnion)
//				pUnion->GetMemberList(DecWarFactionList);
//		}
//	}
//
//	//帮会互加城战敌对列表
//	list<COrganizing*>::iterator itDec = DecWarFactionList.begin();
//	for(;itDec != DecWarFactionList.end();itDec++)
//	{
//		COrganizing* pDecOrg = (*itDec);
//		list<COrganizing*>::iterator itOwned= OwnedFactiosList.begin();
//		for(;itOwned != OwnedFactiosList.end();itOwned++)
//		{
//			COrganizing* pOwnedOrg = (*itOwned);		
//			pDecOrg->AddCityWarEnemyOrganizing(pOwnedOrg->GetExID());
//			pOwnedOrg->AddCityWarEnemyOrganizing(pDecOrg->GetExID());
//		}
//	}
//}
//
//CAttackCitySys* GetAttackCitySys()
//{
//	return CAttackCitySys::getInstance();
//}