#include "StdAfx.h"
#include "Organizingctrl.h"
#include "Faction.h"
#include "Union.h"
#include "NoOrganizing.h"
#include "Player.h"
#include "Message.h"
#include "../../../Public/Common/Public.h"
#include "../GameClient/Game.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

COrganizingCtrl* COrganizingCtrl::instance = NULL;

COrganizingCtrl::COrganizingCtrl(void)
{
	m_FactionOrganizing = NULL;
	m_ConfedeOrganizing=NULL;
	m_bIsSendGetOrgaData = false;

	m_lFactinNum=0;
	m_lCurMaxPage=0;

	
	m_AllFactions.clear();
}

COrganizingCtrl::~COrganizingCtrl(void)
{
	if(m_FactionOrganizing)
	{
		SAFE_DELETE(m_FactionOrganizing);
	}
}

//得到唯一实例句柄
//该函数不支持多线程操作
inline COrganizingCtrl*COrganizingCtrl:: getInstance()
{
	if(instance == NULL)
	{
		instance = new COrganizingCtrl;
	}
	return instance;
}

//初始化
bool   COrganizingCtrl::Initialize()
{

	SYSTEMTIME sysTime;
	GetSystemTime(&sysTime);
	tagTime Time;
	memcpy(&Time,&sysTime,sizeof(Time));
	return true;
}

//释放资源
void COrganizingCtrl::Release()
{
	SAFE_DELETE(m_FactionOrganizing);
	SAFE_DELETE(m_ConfedeOrganizing);

	if(instance != NULL)
	{
		SAFE_DELETE(instance);
	}
}

void   COrganizingCtrl::ReNew()
{
	SAFE_DELETE(m_FactionOrganizing);
	SAFE_DELETE(m_ConfedeOrganizing);
	m_bIsSendGetOrgaData=false;
	m_FactionList.clear();
}

//解压行会列表
void COrganizingCtrl::DecordFactionListFromByteArray(BYTE* pByte, long& pos,long lPage,long lFactionNum)
{
	//第一次得，清除以前的列表
	if(lPage==1)
	{
		m_FactionList.clear();
	}

	m_lCurMaxPage = lPage;
	m_lFactinNum=lFactionNum;

	long lSize = _GetLongFromByteArray(pByte, pos);
	for(long l=0;l<lSize;l++)
	{
		tagApplyFaction ApplyFaction;
		_GetBufferFromByteArray(pByte, pos,ApplyFaction.ID);
		_GetStringFromByteArray(pByte, pos, ApplyFaction.strName);
		m_FactionList.push_back(ApplyFaction);
	}
}

//解呀宣战行会列表
void COrganizingCtrl::DecordDeclareFactionInfoFromByteArray(BYTE* pByte, long& pos,long lPage,long lFactionNum)
{
	if(lPage == 1)
	{
		m_DecWarFationList.clear();
	}

	m_lDecWarCurMaxPage = lPage;
	m_lDecWarFactionNum = lFactionNum;

	long lSize = _GetLongFromByteArray(pByte, pos);
	for(long l=0;l<lSize;l++)
	{
		DeclareWarFacInfo FactionInfo;
		_GetBufferFromByteArray(pByte, pos,FactionInfo.FactionID);
		FactionInfo.btCountry	= _GetByteFromByteArray(pByte, pos);
		_GetStringFromByteArray(pByte, pos, FactionInfo.strName);
		FactionInfo.lRelations = _GetLongFromByteArray(pByte, pos);
		m_DecWarFationList.push_back(FactionInfo);
	}
}

//解压行会数据
void COrganizingCtrl::DecordAllFactionsFromByteArray(BYTE* pByte,long& pos)
{
	m_AllFactions.clear();
	long lSize = _GetLongFromByteArray(pByte, pos);
	for(long l=0;l<lSize;l++)
	{
		tagAllFacState Faction;
		_GetBufferFromByteArray(pByte, pos,Faction.FactionID);
		Faction.btCountry = _GetByteFromByteArray(pByte, pos);
		_GetStringFromByteArray(pByte, pos, Faction.strFactionName);
		Faction.bUionRelation = false;
		Faction.bFactWarEnemyRela = false;
		Faction.bCityWarEnemyRela = false;
		m_AllFactions.insert(Faction);
	}
}

//解压行会排行榜
void COrganizingCtrl::DecordFactionBillboardFromByteArray(BYTE* pByte,long& pos,string& str)
{
	m_FactionBillboard.clear();

	m_strFactionBillboardTitle = str;
	tagFacBillboard FacBillboard;
	long lSize = _GetLongFromByteArray(pByte, pos);
	for(long l=0;l<lSize;l++)
	{
		FacBillboard.lFactionID = _GetLongFromByteArray(pByte, pos);
		char strName[17]="";
		_GetStringFromByteArray(pByte, pos, strName);
		FacBillboard.strName = strName;
		FacBillboard.lNum	= _GetLongFromByteArray(pByte,pos);

		m_FactionBillboard.push_back(FacBillboard);
	}
}

const list<COrganizingCtrl::tagApplyFaction>& COrganizingCtrl::GetFactionList( long lPage,LONG64 lNetSessionID,long lPassword)
{
	static long lTemptPage = 0;
	static list<tagApplyFaction> FactionList;


	if(lPage < 1)	lPage = 1;

	if(lPage>m_lCurMaxPage)
	{
		if((lPage-1)*11 < m_lFactinNum)
		{
			FactionList.clear();
			//CMessage msg(MSG_C2S_ORGSYS_GETFACTIONLIST);
			//msg.Add(lNetSessionID);
			//msg.Add(lPassword);
			//msg.Send();
		}
	}
	else
	{	
		lTemptPage = lPage;
		FactionList.clear();

		long lBeginPos = (lPage-1)*11;
		long lSize = 11;

		list<tagApplyFaction>::iterator it = m_FactionList.begin();
		for(long l=0; it != m_FactionList.end(); it++,l++)
		{
			if( l >= lBeginPos && l < lBeginPos+lSize)
			{
				FactionList.push_back((*it));
			}
			if(l >= lBeginPos+lSize)	break;
		}
	}
	return FactionList;
}

bool COrganizingCtrl::IsNextFactionPage(long lPage)
{
	if(lPage < m_lCurMaxPage) return true;

	if(lPage*11 < m_lFactinNum) return true;

	return false;
}

list<DeclareWarFacInfo>& COrganizingCtrl::GetDecWarFactionList(long lPage,LONG64 lNetSessionID,long lPassword)
{
	static long lTemptPage = 0;
	static list<DeclareWarFacInfo> DecWarFactionList;

	if(lPage < 1)	lPage = 1;

	if(lPage>m_lDecWarCurMaxPage)
	{
		if((lPage-1)*11 < m_lDecWarFactionNum)
		{
			DecWarFactionList.clear();
			//CMessage msg(MSG_C2S_FACTIONWAR_GETFACTIONLIST);
			//msg.Add(lNetSessionID);
			//msg.Add(lPassword);
			//msg.Send();
		}
	}
	else
	{	
		lTemptPage = lPage;
		DecWarFactionList.clear();

		long lBeginPos = (lPage-1)*11;
		long lSize = 11;

		list<DeclareWarFacInfo>::iterator it = m_DecWarFationList.begin();
		for(long l=0; it != m_DecWarFationList.end(); it++,l++)
		{
			//该成员是某个组织的成员，返回该组织的ID
			if( l >= lBeginPos && l < lBeginPos+lSize)
			{
				DecWarFactionList.push_back((*it));
			}
			if(l >= lBeginPos+lSize)	break;
		}
	}
	return DecWarFactionList;
}


bool COrganizingCtrl::IsNetDecWarFactionPage(long lPage)
{
	if(lPage < m_lDecWarCurMaxPage) return true;

	if(lPage*11 < m_lDecWarFactionNum) return true;

	return false;
}

//是否可以向这个行会宣战
bool COrganizingCtrl::IsDecWarFaction(const CGUID& FactionID)
{
	list<DeclareWarFacInfo>::iterator it = m_DecWarFationList.begin();
	for(; it != m_DecWarFationList.end(); it++)
	{
		//该成员是某个组织的成员，返回该组织的ID
		if( FactionID == (*it).FactionID)
		{
			if( it->lRelations == 0 || it->lRelations == 3 /*|| it->lRelations == 4*/)
				return true;
			else
				return false;
		}
	}
	return false;
}

//设置一个宣战行会的关系
void COrganizingCtrl::SetDecWarFactionRelation(const CGUID& FactionID,long lRel)
{
	list<DeclareWarFacInfo>::iterator it = m_DecWarFationList.begin();
	for(; it != m_DecWarFationList.end(); it++)
	{
		//该成员是某个组织的成员，返回该组织的ID
		if( FactionID == (*it).FactionID)
		{
			(*it).lRelations = lRel;
			return;
		}
	}
}

//得到一页行会状态列表
void COrganizingCtrl::GetFactionStatInfo(set<tagAllFacState,KeyCompareByFaction>& Factions,long lPage,long lPageSize)
{
	Factions.clear();
	if((long)m_AllFactions.size() <= lPage* lPageSize )
		return;

	set<tagAllFacState,KeyCompareByFaction>::iterator it = m_AllFactions.begin();
	for(int i=0; it != m_AllFactions.end();it++,i++)
	{
		//if( m_FactionOrganizing && m_FactionOrganizing->GetID() == (*it).FactionID )
		//{
		//	i--;
		//	continue;
		//}
		if(i >= lPage*lPageSize && i < (lPage+1)*lPageSize)		
		{
			tagAllFacState FacState = (*it);
			if( IsUnionRelation(FacState.FactionID) )
			{
				FacState.bUionRelation = true;
				FacState.bFactWarEnemyRela = false;
				FacState.bCityWarEnemyRela = false;
			}
			else
			{
				FacState.bUionRelation = false;
				FacState.bFactWarEnemyRela = false;
				FacState.bCityWarEnemyRela = false;
				if(IsEnemyRelationWithFactionWar(FacState.FactionID))
				{
					FacState.bFactWarEnemyRela = true;
				}
				if( IsEnemyRelationWithCityWar(FacState.FactionID) )
				{
					FacState.bCityWarEnemyRela = true;
				}
			}

			Factions.insert(FacState);
		}
		if(i >= (lPage+1)*lPageSize)
			break;
	}
}

bool COrganizingCtrl::IsNextFactionFromAllFaction(long lPage,long lPageSize)
{
	if(lPage*lPageSize < (long)m_AllFactions.size())	return true;
	return false;
}

void COrganizingCtrl::SetFactionOrganizing(CFaction* pOrgan)
{
	if(m_FactionOrganizing)
	{
		SAFE_DELETE(m_FactionOrganizing);
	}
	m_FactionOrganizing = pOrgan;
}

CFaction* COrganizingCtrl::GetFactionOrganizing()						//根据组织ID得到相应的组织实例
{
	if(m_FactionOrganizing==NULL && m_bIsSendGetOrgaData==false)
	{
		CPlayer* pPlayer =GetGame()->GetMainPlayer();
		if(pPlayer)
		{
			if(pPlayer->GetFactionID()!= CGUID::GUID_INVALID)
			{
				//CMessage msg(MSG_C2S_ORGSYS_GetOrganizing);
				//msg.Send();
				m_bIsSendGetOrgaData = true;
			}
		}
	}
	return m_FactionOrganizing;
}

//添加一同盟组织
void COrganizingCtrl::SetConfederationOrganizing(CUnion* pOrgan)
{
	if(m_ConfedeOrganizing)
		SAFE_DELETE(m_ConfedeOrganizing);
	m_ConfedeOrganizing = pOrgan;

	//m_FactionOrganizing->SetSuperiorOrganizing(pOrgan->GetID());
}

void COrganizingCtrl::DelConfederationOrganizing()
{
	if(m_ConfedeOrganizing)
		SAFE_DELETE(m_ConfedeOrganizing);
	//if(m_FactionOrganizing)
	//	m_FactionOrganizing->SetSuperiorOrganizing(CGUID::GUID_INVALID);
}

CUnion* COrganizingCtrl::GetConfederationOrganizing()				//根据组织ID得到相应的组织实例
{
	if(m_FactionOrganizing==NULL && m_bIsSendGetOrgaData==false)
	{
		CPlayer* pPlayer =GetGame()->GetMainPlayer();
		if(pPlayer)
		{
			if(pPlayer->GetUnionID() != CGUID::GUID_INVALID)
			{
				//CMessage msg(MSG_C2S_ORGSYS_GetOrganizing);
				//msg.Send();
				m_bIsSendGetOrgaData = true;
			}
		}
	}
	return m_ConfedeOrganizing;
}


void COrganizingCtrl::DelFactionOrganizing()
{
	if(m_FactionOrganizing)
	{
		SAFE_DELETE(m_FactionOrganizing);
	}
}
//无行会成员申请加入
bool COrganizingCtrl::ApplyForJoin(const CGUID& FactionID)
{
	CPlayer* pPlayer = GetGame()->GetMainPlayer();
	if(pPlayer->GetFactionID()!= CGUID::GUID_INVALID)	return false;

	//CMessage msg(MSG_C2S_ORGSYS_APPLYJOINFACTION);
	//msg.Add(FactionID);
	//msg.Send();
	return true;
}
//处理要加入组织的人员
bool COrganizingCtrl::DoJoin(const CGUID& BeExecutedID)
{	
	return true;
}
//创建行会
bool COrganizingCtrl::CreateFaction(long lPlayerID,long lLvl,tagTime& Time,string& strName /* = string */)
{
	
	return true;
}
//解散行会
bool COrganizingCtrl::DisbandFaction(const CGUID& PlayerID,const CGUID& FactionID)
{
	//如果当前是攻城时间，则不能解散行会
	if(GetOrganiCtrl()->IsAggressCityTime() == true)	return false;
	
	
	//if(m_FactionOrganizing->Disband(PlayerID) == false)	return false;


	SAFE_DELETE(m_FactionOrganizing);

	return true;
}
//创建同盟
bool COrganizingCtrl::CreateConfederation(long lTargFactionID,string& strName /*=  string("")*/)
{
	
	//判断该玩家是否是自由人员,不是，返回所在的行会
	CPlayer* pPlayer = GetGame()->GetMainPlayer();
	if(pPlayer==NULL)	return false;
	
	if(pPlayer->GetFactionID()==NULL_GUID) return false;
	
	//如果该玩家不是该帮的帮主
	//if( m_FactionOrganizing->IsMaster(pPlayer->GetExID()) == 0) return false;

	//CMessage msg(MSG_C2S_ORGSYS_RequestEstaUnion);
	//msg.Add(lTargFactionID);
	//msg.Add((char*)strName.c_str());
	//msg.Send();
		
	return true;
}
//解散同盟
bool COrganizingCtrl::DisbandConferation(const CGUID& PlayerID,const CGUID& ConfeID)
{
	//如果当前是攻城时间，则不能解散同盟
	if(GetOrganiCtrl()->IsAggressCityTime() == true)	return false;
		
	//if(m_ConfedeOrganizing->Disband(PlayerID) == false)	return false;

	
	SAFE_DELETE(m_ConfedeOrganizing);
	return true;
}

//转让主城
//lOwnedID：拥有主城的行会的帮主的ID,lBeTeransferedID:被转让的行会的帮主,lCityID,主城ID
bool COrganizingCtrl::TransferIOwnerCity(long lOwnedID,long lBeTransferedID,long lCiytID)
{
	//判断该玩家是否是自由人员,不是，返回所在的行会
	CPlayer* pPlayer = GetGame()->GetMainPlayer();
	if(pPlayer==NULL)	return false;

	//CMessage msg(MSG_C2S_CITY_Transfer);
	//msg.Add(lBeTransferedID);
	//msg.Add(lCiytID);
	//msg.Send();
	return true;
}

//判断该玩家是否不属于任何行会
//返回值:0表示不属于任何行会，否则返回该行会的ID
long COrganizingCtrl::IsFreePlayer(long lPlayerID)
{
	return 0;
}
//判断该帮是否不属于任何同盟                                                                                                        
//返回值:0表示是自由的行会，否则返回同盟的ID
long COrganizingCtrl::IsFreeFaction(long lFactionID)
{
	return 0;
}

//判断该玩家是否是帮主
long COrganizingCtrl::IsFactionMaster(long lPlayerID)
{
	return 0;
}

//判断该帮是否是盟主
long COrganizingCtrl::IsConferationMaster(long lFactionID)
{
	return 0;
}

//得到当前是否是攻城战时间 
bool COrganizingCtrl::IsAggressCityTime()
{
	return false;
}
//从申请所有行会列表中移出一个申请人员
long COrganizingCtrl::RemovePersonFromApplyFactionList(long lPlayerID)
{
	return 0;
}
//从申请所有联盟列表中移出一个申请组织
long COrganizingCtrl::RemoveFactionFromApplyConfederList(long lFactionID)
{
	
	return 0;
}

//购买徽标
bool COrganizingCtrl::BuyFactionLogo(long lPlayerID,long lIconID,tagTime& CurrTime)
{
	return true;
}

//相应服务器的行会变化
void COrganizingCtrl::DelFaction(const CGUID& FactionID)
{
	set<tagAllFacState,KeyCompareByFaction>::iterator it = m_AllFactions.begin();
	for(; it != m_AllFactions.end();)
	{
		if((*it).FactionID == FactionID)
			it = m_AllFactions.erase(it);
		else
			it++;
	}
}
void COrganizingCtrl::AddFaction(const CGUID& FactionID,char* strFactionName)
{
	set<tagAllFacState,KeyCompareByFaction>::iterator it = m_AllFactions.begin();
	for(; it != m_AllFactions.end();it++)
	{
		if((*it).FactionID == FactionID)		
		{
			return;
		}
	}

	tagAllFacState Faction;
	Faction.FactionID = FactionID;
	strcpy(Faction.strFactionName,strFactionName);
	Faction.bUionRelation = false;
	Faction.bFactWarEnemyRela = false;
	Faction.bCityWarEnemyRela = false;
	m_AllFactions.insert(Faction);
}

//得到行会的名字
string COrganizingCtrl::GetFactionNameByID(const CGUID& FactionID)
{
	set<tagAllFacState,KeyCompareByFaction>::iterator it = m_AllFactions.begin();
	for(; it != m_AllFactions.end();it++)
	{
		if((*it).FactionID == FactionID)		
		{
			return (*it).strFactionName;
		}
	}
	return "";
}

BYTE COrganizingCtrl::GetFactionCountry(const CGUID& ID)
{
	set<tagAllFacState,KeyCompareByFaction>::iterator it = m_AllFactions.begin();
	for(; it != m_AllFactions.end();it++)
	{
		if((*it).FactionID == ID)		
		{
			return (*it).btCountry;
		}
	}

	return 0;
}

//得到自己行会与此行会是否是同盟关系
bool	COrganizingCtrl::IsUnionRelation(const CGUID& FactionID)
{
	if(m_ConfedeOrganizing)
	{
		//if(m_ConfedeOrganizing->IsMember(FactionID) > 0)
		//	return true;
	}
	return false;
}
//是否是帮战的敌对关系
bool	COrganizingCtrl::IsEnemyRelationWithFactionWar(const CGUID& FactionID)
{
	if(m_FactionOrganizing)
	{
		//return m_FactionOrganizing->IsEnemyFaction(FactionID);
	}
	return false;
}
//判断是否是城战的敌对关系
bool	COrganizingCtrl::IsEnemyRelationWithCityWar(const CGUID& FactionID)
{
	if(m_FactionOrganizing)
	{
		//return m_FactionOrganizing->IsCityWarEnemyFaction(FactionID);
	}
	return false;
}