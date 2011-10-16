#include "StdAfx.h"
#include ".\confederation.h"
#include "OrganizingCtrl.h"
#include "..\dbaccess\worlddb\rsconfederation.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

long CConfederation::MAX_MemberNum = 6;		//最大成员数量

CConfederation::CConfederation(long lID,long lMastterID,string& strName /*=  string("")*/)
:m_lID(lID)
,m_lMastterID(lMastterID)
,m_strName(strName)
{
	Initial();
}

CConfederation::~CConfederation(void)
{
}

//初始化
bool CConfederation::Initial()
{
	m_ApplyPerson = 0;

	tagMemInfo MemInfo;
	MemInfo.lID = m_lMastterID;
	//帮主的默认职级最大1
	MemInfo.lJobLvl = MIN_JobLvl;
	//赋予帮助称号
	strcpy(MemInfo.strTitle,"盟主");

	MemInfo.listPV[PV_Disband] = PST_Permit;
	MemInfo.listPV[PV_Exit] = PST_No;
	MemInfo.listPV[PV_ConMem] = PST_Permit;
	MemInfo.listPV[PV_FireOut] = PST_Permit;
	MemInfo.listPV[PV_DubJobLvl] = PST_No;
	MemInfo.listPV[PV_Pronounce] = PST_No;
	MemInfo.listPV[PV_LeaveWord] = PST_No;
	MemInfo.listPV[PV_EditLeaveWord] = PST_No;
	MemInfo.listPV[PV_ObtainTax] = PST_No;
	MemInfo.listPV[PV_EndueROR] = PST_No;
	
	m_Members[m_lMastterID] = MemInfo;
	return true;
}

//负责保存数据
bool CConfederation::Save()
{
	GetGame()->GetRsConfederation()->SaveConfederation(this);
	return true;
}
//负责装载数据
bool CConfederation::Load()
{
	return true;
}

//设置成员信息
bool CConfederation::SetMembers(BYTE* pByte,long lSize)
{
	tagMemInfo* pMemInfo;
	long lCount = lSize/sizeof(tagMemInfo);
	assert(lCount>=1);
	for(long i=0; i < lCount;i++)
	{
		pMemInfo = (tagMemInfo*)pByte;
		m_Members[pMemInfo->lID] = *pMemInfo;
		pByte+=sizeof(tagMemInfo);
	}
	return true;
}

//得到成员信息数据
bool CConfederation::GetMembersData(vector<BYTE>& ByteArray)
{
	map<long,tagMemInfo>::iterator it = m_Members.begin();
	for(;it!=m_Members.end();it++)
	{
		_AddToByteArray(&ByteArray,&(*it).second,sizeof(tagMemInfo) );
	}
	return true;
}

// 添加到CByteArray
bool CConfederation::AddToByteArray()
{
	return true;
}

//设置同盟的名字
bool CConfederation::SetName(string& strName)
{
	m_strName = strName;
	return true;
}

//判断该该帮是否拥有主城
//返回值:0表示没有，否则返回该主城的ID
long CConfederation::IsOwnedCity(long lCityID)
{
	map<long,tagMemInfo>::iterator it = m_Members.begin();
	for(; it != m_Members.end(); it++)
	{
		COrganizing* pOrgFaction = GetOrganiCtrl()->GetFactionOrganizing((*it).first);
		if(pOrgFaction && pOrgFaction->IsOwnedCity(lCityID) > 0)
			return pOrgFaction->IsOwnedCity(lCityID);
	};
	return 0;
}

//设置该帮会的主城ID
void CConfederation::AddOwnedCity(long lCityID)
{
	map<long,tagMemInfo>::iterator it = m_Members.begin();
	for(; it != m_Members.end(); it++)
	{
		COrganizing* pOrgFaction = GetOrganiCtrl()->GetFactionOrganizing((*it).first);
		if(pOrgFaction)
		{
			//如果是盟主，把该城赋给盟帮
			if((*it).first == m_lMastterID)	pOrgFaction->AddOwnedCity(lCityID);
			//其余帮如果有该城，则删除该主城
			else pOrgFaction->DelOwnedCity(lCityID);
		}
	};
}

//删除一个拥有的主城ID
bool CConfederation::DelOwnedCity(long lCityID)
{
	//主城在盟主所在的帮
	COrganizing* pOrgFaction = GetOrganiCtrl()->GetFactionOrganizing(m_lMastterID);
	if(pOrgFaction)
	{
		pOrgFaction->DelOwnedCity(lCityID);
		return true;
	}
	return false;
}
//对敌对帮会列表作处理
//得到好友组织列表
list<long> CConfederation::GetFriendList()
{
	list<long> TemptList;
	TemptList.clear();
	map<long,tagMemInfo>::iterator it = m_Members.begin();
	for(; it != m_Members.end(); it++)
	{
		TemptList.push_back((*it).first);
	}
	return TemptList;
}
//得到敌对组织列表
list<long> CConfederation::GetEnemyList()
{
	//得到同盟任意一个帮派的敌对列表就可以，这里取盟主的帮派
	COrganizing* pOrga = GetOrganiCtrl()->GetFactionOrganizing(m_lMastterID);
	if(pOrga)
	{
		return pOrga->GetEnemyList();
	}
	else
	{
		list<long> TemptList;
		TemptList.clear();
		return TemptList;
	}
}

//添加一个敌对组织列表
void CConfederation::AddEnemyOrganizing(list<long>& TemptList)
{
	//给每个成员组织添加
	//以下循环最大为6*6=36次
	COrganizing* pOrga = NULL;
	list<long>::iterator itList = TemptList.begin();
	for(; itList != TemptList.begin(); itList++)
	{
		map<long,tagMemInfo>::iterator itMem = m_Members.begin();
		for(;itMem != m_Members.end(); itMem++)
		{
			if(pOrga = GetOrganiCtrl()->GetFactionOrganizing((*itMem).first))
			{
				pOrga->AddEnemyOrganizing((*itList));
			}
		}
	}
}
//删除一个敌对组织列表
void CConfederation::DelEnemyOrganizing(list<long>& TemptList)
{
	//给从每个成员组织删除
	//以下循环最大为6*6=36次
	COrganizing* pOrga = NULL;
	list<long>::iterator itList = TemptList.begin();
	for(; itList != TemptList.begin(); itList++)
	{
		map<long,tagMemInfo>::iterator itMem = m_Members.begin();
		for(;itMem != m_Members.end(); itMem++)
		{
			if(pOrga = GetOrganiCtrl()->GetFactionOrganizing((*itMem).first))
			{
				pOrga->DelEnemyOrganizing((*itList));
			}
		}
	}
}
//是否有敌对帮会存在
bool CConfederation::IsHaveEnymyFaction()
{
	map<long,tagMemInfo>::iterator it = m_Members.begin();
	for(; it != m_Members.end(); it++)
	{
		COrganizing* pOrgFaction = GetOrganiCtrl()->GetFactionOrganizing((*it).first);
		if(pOrgFaction &&	pOrgFaction->IsHaveEnymyFaction() ) return true;
	}
	return false;
}
//添加一个敌对帮会,给每个成员都添加敌对帮会
void CConfederation::AddEnemyOrganizing(long lFactionID)
{
	//如果该帮会是该同盟的成员则退出
	if(IsMember(lFactionID) > 0)	return;

	map<long,tagMemInfo>::iterator it = m_Members.begin();
	for(; it != m_Members.end(); it++)
	{
		COrganizing* pOrgFaction = GetOrganiCtrl()->GetFactionOrganizing((*it).first);
		if(pOrgFaction)	pOrgFaction->AddEnemyOrganizing(lFactionID);
	}
}
//删除一个敌对帮会
//删除每个帮会成员的敌对帮会
void CConfederation::DelEnemyOrganizing(long lFactionID)
{
	map<long,tagMemInfo>::iterator it = m_Members.begin();
	for(; it != m_Members.end(); it++)
	{
		COrganizing* pOrgFaction = GetOrganiCtrl()->GetFactionOrganizing((*it).first);
		if(pOrgFaction)	pOrgFaction->DelEnemyOrganizing(lFactionID);
	}
}


//清除敌对帮会列表
//清除每个帮会成员的敌对帮会列表
void CConfederation::ClearEnemyFation()
{
	map<long,tagMemInfo>::iterator it = m_Members.begin();
	for(; it != m_Members.end(); it++)
	{
		COrganizing* pOrgFaction = GetOrganiCtrl()->GetFactionOrganizing((*it).first);
		if(pOrgFaction)	pOrgFaction->ClearEnemyFation();
	}
}

//判断某ID是否能使用此权限
//传入玩家的ID
bool CConfederation::IsUsingPV(long lID,ePurview ePV)
{
	//权限不存在
	if(ePV < PV_Disband || ePV > PV_EndueROR)	return false;

	long lFactionID = GetOrganiCtrl()->IsFactionMaster(lID);
	//该玩家不是帮主,则没有权限操作盟
	if(lFactionID == 0)	return false;
	//如果该盟里没有此帮派,退出
	if( m_Members.find(lFactionID) == m_Members.end() )	return false;

	//该帮是否有此权限并且该权限是否打开
	if( m_Members[lFactionID].listPV[ePV] != PST_Permit )	return false;
	//拥有此权限
	return true;
}
//给某ID设置一个权限
//传入帮会ID
void CConfederation::SetMemPV(long lID,ePurview ePV)
{
	//权限不存在
	if(ePV < PV_Disband || ePV > PV_EndueROR)	return;
	//找到此成员的信息
	if(m_Members.find(lID) == m_Members.end()) return;
	//拥有此权限退出
	if( m_Members[lID].listPV[ePV] != PST_No)	return;
	//赋予一个权限给该成员,默认为打开次权限
	m_Members[lID].listPV[ePV] = PST_Permit;
}

//取消某个组织的权限
//传入帮会的ID
void CConfederation::AbolishMemPV(long lID,ePurview ePV)
{
	//权限不存在
	if(ePV < PV_Disband || ePV > PV_EndueROR)	return;
	//找到此成员的信息
	if(m_Members.find(lID) == m_Members.end()) return;
	//已经没有此权限退出
	if(m_Members[lID].listPV[ePV] == PST_No)	return;
	//取消此权限
	m_Members[lID].listPV[ePV] = PST_No;
	return ;
}
//检查某ID执行某个操作的合法性
//传入玩家ID
bool CConfederation::CheckOperValidate(long lExecuteID,long lBeExecutedID,ePurview ePV)
{
	//自己不能对自己进行操作
	if(lExecuteID == lBeExecutedID )	return false;
	//被操作者不能是盟主
	if(lBeExecutedID == m_lMastterID)	return false;
	//得到所在的帮并且他们是帮主
	long lExeFactionID		= GetOrganiCtrl()->IsFactionMaster(lExecuteID);
	long lBeExedFactionID	= GetOrganiCtrl()->IsFactionMaster(lBeExecutedID);
	if(lExeFactionID == 0 || lBeExedFactionID == 0)	return false;

	//不是该盟的帮派则不能做此操作
	if( IsMember(lExeFactionID) == 0 || IsMember(lBeExedFactionID) == 0) return false;
	//操作者没有此操作权限或者两人都有此操作权限则不能执行此操作
	bool lResult = IsUsingPV(lExeFactionID,ePV);
	if( lResult == false || lResult == IsUsingPV(lBeExedFactionID,ePV) ) return false;
	
	return true;
}
//检查某ID执行某个操作的合法性
//传入玩家ID
bool CConfederation::CheckOperValidate(long lExecuteID,ePurview ePV)
{
	//得到所在的帮并且他们是帮主
	long lFactionID		= GetOrganiCtrl()->IsFactionMaster(lExecuteID);
	if(lFactionID == 0)	return false;
	//不是本帮成员则不能执行此操作
	if(IsMember(lFactionID)==0)	return false;
	//没有此操作权限则不能执行此操作
	if(IsUsingPV(lFactionID,ePV)== false)	return false;
	
	return true;
}
//从变量中删除一个成员
//传入帮会ID
bool CConfederation::DelMember(long lID)
{
	m_Members.erase(lID);
	//删除该帮会的敌对组织,设置上级组织为0
	COrganizing* pFaction = GetOrganiCtrl()->GetFactionOrganizing(lID);
	if(pFaction)
	{
		pFaction->ClearEnemyFation();
		pFaction->SetSuperiorOrganizing(0);
	}
	return true;
}
//移出一个申请列表中的成员
long CConfederation::RemoveApplyMember(long lID)
{
	return 0;
}
//邀请自由帮派成员加入
bool CConfederation::ApplyForJoin(long lPlayerID,long lLvl,long lOrgMemID /*= 0*/)
{
	//如果当前是攻城时间，则不能邀请帮派加入同盟
	if(GetOrganiCtrl()->IsAggressCityTime() == true)	return false;

	//当前有申请帮派,退出
	if(m_ApplyPerson > 0)	return false;
	//检查玩家所在的帮会,并且是帮主
	long lFactionID = GetOrganiCtrl()->IsFactionMaster(lPlayerID);
	if(lFactionID == 0)	return false;
	//检查被申请的player是否具有接受新成员的权限
	if(CheckOperValidate(lOrgMemID,PV_ConMem) == false)	return false;
	//该帮必须是自由帮派
	long lConfederationID = GetOrganiCtrl()->IsFreeFaction(lFactionID);
	if(lConfederationID > 0)	return false;

	//保存要加入的帮派ID
	m_ApplyPerson = lFactionID;

	//建立异步回调类
	class PlayerApplyForJoinConfeder:public IAsyncCaller,public IAsyncCallback
	{
	public:
		PlayerApplyForJoinConfeder(long lID)
			:m_lID(lID)
		{
		}
		//释放自己
		void Release(){	delete this; }

		void DoAsyncCall(long lID,long lPassWord,va_list va)
		{
			long lAppPlayerID = va_arg(va,long);
			long lFactinoMemID	 = va_arg(va,long);
			//发送消息给lFactinoMemID处理,等待异步返回调用			
		}

		void OnAsyncCallback(tagAsyncResult& Result)
		{
			if(Result.Result == AR_OK)
			{
				long lExecuteID = va_arg(Result.Arguments,long);
				long lBeExecutedID	 = va_arg(Result.Arguments,long);
				tagTime Time = va_arg(Result.Arguments,tagTime);

				COrganizing* pConfederation = GetOrganiCtrl()->GetConfederationOrganizing(m_lID);
				if(pConfederation)
				{
					pConfederation->DoJoin(lExecuteID,lBeExecutedID,Time);
				}
			}
		}
	private:
		//保存一个该盟的ID,以便回调时候处理
		long  m_lID;
	};

	//建立一个Session
	CNetSession* pSession = GetNetSessionManager()->CreateSession(lOrgMemID);
	//建立并设置异步调用接口
	PlayerApplyForJoinConfeder* pInstance = new PlayerApplyForJoinConfeder(m_lID);
	pSession->SetCallbackHandle(pInstance);
	//赋予超时时间,并开始异步处理
	pSession->Beging(1000,pInstance,lPlayerID,lOrgMemID);

	return true;
}
//处理要加入联盟的组织
//传入PlayerID
bool CConfederation::DoJoin(long lExecuteID,long lBeExecutedID,tagTime& Time)
{
	//检查玩家所在的帮会,并且是帮主
	long lFactionID = GetOrganiCtrl()->IsFactionMaster(lBeExecutedID);
	if(lFactionID == 0)	return false;
	//检查申请加入的帮派是否合法
	if(m_ApplyPerson != lFactionID)	return false;
	//清除
	m_ApplyPerson = 0;
	//该帮必须是自由帮派
	long lConfederationID = GetOrganiCtrl()->IsFreeFaction(lFactionID);
	if(lConfederationID > 0)	return false;
	//检查操作的合法性
	if(CheckOperValidate(lExecuteID,PV_ConMem) == false)	return false;

	//互加敌对帮会
	COrganizing* pFaction = GetOrganiCtrl()->GetFactionOrganizing(lFactionID);
	if(pFaction)
	{
		AddEnemyOrganizing(pFaction->GetEnemyList());
		pFaction->AddEnemyOrganizing(GetEnemyList());
	}

	//把该成员加入帮会
	tagMemInfo MemInfo;
	MemInfo.lID=lFactionID;
	//赋予最大职级
	MemInfo.lJobLvl = MAX_JobLvl;
	//赋予成员默认称号
	strcpy(MemInfo.strTitle, "盟员");

	//赋予成员有退出帮会的权限
	MemInfo.listPV[PV_Disband] = PST_No;
	MemInfo.listPV[PV_Exit] = PST_Permit;
	MemInfo.listPV[PV_ConMem] = PST_No;
	MemInfo.listPV[PV_FireOut] = PST_No;
	MemInfo.listPV[PV_DubJobLvl] = PST_No;
	MemInfo.listPV[PV_Pronounce] = PST_No;
	MemInfo.listPV[PV_LeaveWord] = PST_No;
	MemInfo.listPV[PV_EditLeaveWord] = PST_No;
	MemInfo.listPV[PV_ObtainTax] = PST_No;
	MemInfo.listPV[PV_EndueROR] = PST_No;


	m_Members[lFactionID] = MemInfo;

	//设置该帮会的上级组织
	if(pFaction)	pFaction->SetSuperiorOrganizing(m_lID);
	return true;
}

//清除申请人员列表
bool CConfederation::ClearApplyList(long lPlayerID)
{
	//检查操作的合法性
	if(CheckOperValidate(lPlayerID,PV_ConMem) == false)	return false;
	//清除申请人员
	m_ApplyPerson = 0;
	return true;
}

//邀请人员加入
bool CConfederation::Invite(long lInvitedID, long lBeInvitedID)
{
	return true;
}
//退出
bool CConfederation::Exit(long lPlayerID)
{
	//如果当前是攻城时间，则不能建立同盟
	if(GetOrganiCtrl()->IsAggressCityTime() == true)	return false;
	//检查操作的合法性
	if(CheckOperValidate(lPlayerID,PV_Exit) == false)	return false;
	//得到该玩家所在的帮派
	long lFactionID = GetOrganiCtrl()->IsFactionMaster(lPlayerID);
	if(lFactionID == 0)	return false;
	
	//删除该帮会
	DelMember(lFactionID);
	return true;
}
//解散
//传入PlayerID
bool CConfederation::Disband(long lID)
{
	//检查操作的合法性
	if(CheckOperValidate(lID,PV_Disband) == false)	return false;

	//清除成员
	map<long,tagMemInfo>::iterator it=m_Members.begin();
	for(; it != m_Members.end(); it++)
	{
		DelMember((*it).first);
	}
	//清0
	m_ApplyPerson = 0;
	return true;
}

//开除
bool CConfederation::FireOut(long lFireID,long lBeFiredID)
{	
	//如果当前是攻城时间，则不能开除盟员 
	if(GetOrganiCtrl()->IsAggressCityTime() == true)	return false;
	//检查操作的合法性
	if(CheckOperValidate(lFireID,lBeFiredID,PV_FireOut) == false)	return false;
	//得到被开除者的帮会
	long lFactionID = GetOrganiCtrl()->IsFactionMaster(lBeFiredID);
	if(lFactionID == 0)	return false;
	//删除该帮派
	DelMember(lFactionID);
	return true;
}

//传盟主之位
bool CConfederation::Demise(long lMasterID,long lID)
{
	//如果当前是攻城时间，则不能建盟主之位
	if(GetOrganiCtrl()->IsAggressCityTime() == true)	return false;
	//自己不能对自己进行操作
	if(lMasterID == lID )	return false;
	//得到所在的帮并且他们是帮主
	long lMasterFactionID		= GetOrganiCtrl()->IsFactionMaster(lMasterID);
	long lFactionID				= GetOrganiCtrl()->IsFactionMaster(lID);
	if(lMasterFactionID == 0 || lFactionID == 0)	return false;

	//不是该盟的盟主或帮派则不能做此操作
	if( IsMaster(lMasterFactionID) == 0 || IsMember(lFactionID) == 0) return false;

	//改变主人
	m_lMastterID = lFactionID;
	//改变两人成员信息
	m_Members[lFactionID] = m_Members[lMasterFactionID];
	m_Members[lFactionID].lID = lFactionID;

	//改变原盟主的称号和权限
	m_Members[lMasterFactionID].lJobLvl = 99;
	m_Members[lMasterFactionID].lID = lMasterFactionID;
	strcpy(m_Members[lMasterFactionID].strTitle,"成员");
;
	//赋予成员有退出帮会的权限
	m_Members[lMasterFactionID].listPV[PV_Disband] = PST_No;
	m_Members[lMasterFactionID].listPV[PV_Exit] = PST_Permit;
	m_Members[lMasterFactionID].listPV[PV_ConMem] = PST_No;
	m_Members[lMasterFactionID].listPV[PV_FireOut] = PST_No;
	m_Members[lMasterFactionID].listPV[PV_DubJobLvl] = PST_No;
	m_Members[lMasterFactionID].listPV[PV_Pronounce] = PST_No;
	m_Members[lMasterFactionID].listPV[PV_LeaveWord] = PST_No;
	m_Members[lMasterFactionID].listPV[PV_EditLeaveWord] = PST_No;
	m_Members[lMasterFactionID].listPV[PV_ObtainTax] = PST_No;
	m_Members[lMasterFactionID].listPV[PV_EndueROR] = PST_No;
	return true;
}

//操作税金
bool CConfederation::OperatorTax(long lPlayerID)
{
	//只有盟主的帮派才拥有操作税金的权限
	COrganizing* pOrga = GetOrganiCtrl()->GetFactionOrganizing(m_lMastterID);
	if(pOrga == NULL)	return false;

    return pOrga->OperatorTax(lPlayerID);
}
//授予称号
bool CConfederation::DubAndSetJobLvl(long lDubID,long lBeDubedID,string& strTitle,long lLvl)
{	
	return true;
}
//取消成员某个权利
bool CConfederation::AbolishRightToMember(long lEndueID,long lBeEnduedID,ePurview ePV)
{
	return true;
}
//赋予成员权利
bool CConfederation::EndueRightToMember(long lEndueID,long lBeEnduedID,ePurview ePV)
{
	return true;
}

//宣告
bool CConfederation::Pronounce(long lPlayerID,string& strPro,tagTime& Time)
{	
	return true;
}
//留言
bool CConfederation::LeaveWord(long lPlayerID,string& strWords,tagTime& Time)
{
	return true;
}
