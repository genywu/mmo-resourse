/*Union.cpp
*修改者: 王峰
*修改日期:2008-11-5
*/

#include "StdAfx.h"
#include "Union.h"
#include "Faction.h"

#include "Player.h"
#include "Message.h"
#include "ClientRegion.h"
#include "Other/AudioList.h"
#include "../GameClient/Game.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************
// 联盟逻辑类
//**************************************************************************************

/* 
*功能：构造函数
*细节描述： 将成员指针置为NULL。
*参数： 无
*返回值： 无
*修改时间：2008-11-5
*/
CUnion::CUnion()
{
	ZeroMemory( &m_stUnionInfo, sizeof(m_stUnionInfo) );
}

/* 
*功能：构造函数
*细节描述： 将成员指针置为NULL。
*参数： 无
*返回值： 无
*修改时间：2008-11-5
*/
CUnion::CUnion( SUnionInfo* pstUnionInfo )
{
	this->SetUnionInfo( pstUnionInfo );
}


/*
*功能：析构函数
*细节描述： 释放资源。
*参数： 无
*返回值： 无
*修改时间：2008-11-5
*/
CUnion::~CUnion()
{
	this->Release();
}



/* 
*功能：初始化函数
*细节描述： 
*参数： 无
*返回值： bool
*修改时间：2008-11-5
*/
bool	CUnion::SetUnionInfo( SUnionInfo* pstUnionInfo )
{
	if( pstUnionInfo )
	{
		m_stUnionInfo = *pstUnionInfo;
		return true;
	}

	return false;
}


/* 
*功能：释放函数
*细节描述： 
*参数： bool
*返回值： 无
*修改时间：2008-11-5
*/
bool	CUnion::Release()
{
	ZeroMemory( &m_stUnionInfo, sizeof(m_stUnionInfo) );

	m_listFactions.clear();

	return true;
}




/* 
*功能：添加一个家族
*细节描述： 
*参数： pFaction
*返回值： bool
*修改时间：2008-11-5
*/
bool	CUnion::AddFaction( CFaction* pFaction )
{
	if( pFaction )
	{
		// 遍历更新场景内所有的玩家
		std::list<CBaseObject*>*	pListOfObjects = GetGame()->GetRegion()->GetObjectList();
		for( std::list<CBaseObject*>::iterator p = pListOfObjects->begin(); p != pListOfObjects->end(); p++ )
		{
			if( (*p)->GetType() == TYPE_PLAYER )
			{
				if( CPlayer* pPlayer = dynamic_cast<CPlayer*>(*p) )
				{
					// 找到该行会的会员进行处理
					if( GetGame()->GetFactionManager()->IsPlayerJoinedFaction( pPlayer, pFaction->GetGUID() ) )
					{
						pPlayer->SetUnionID( this->GetID() );
					}
				}
			}
		}

		// 检测该家族是否已经在联盟内
		if( ! this->HasFaction( pFaction->GetGUID() ) )
		{
			// 向联盟添加新家族
			m_listFactions.push_back( pFaction );
		}

		pFaction->SetUnionGUID( this->GetID() );

		return true;
	}

	return false;
}


/* 
*功能：删除一个联盟家族
*细节描述： 
*参数： guid 行会GUID
*返回值： bool
*修改时间：2008-11-5
*/
bool	CUnion::DelFaction( CGUID guid )
{
	// 遍历处理场景内所有的玩家
	std::list<CBaseObject*>*	pListOfObjects = GetGame()->GetRegion()->GetObjectList();
	for( std::list<CBaseObject*>::iterator p = pListOfObjects->begin(); p != pListOfObjects->end(); p++ )
	{
		if( (*p)->GetType() == TYPE_PLAYER )
		{
			if( CPlayer* pPlayer = dynamic_cast<CPlayer*>(*p) )
			{
				// 找到该家族的会员进行处理
				if( GetGame()->GetFactionManager()->IsPlayerJoinedFaction( pPlayer, guid ) )
				{
					pPlayer->SetUnionID( NULL_GUID );
				}
			}
		}
	}

	// 从联盟删除该家族
	for( std::list<CFaction*>::iterator p = m_listFactions.begin(); p != m_listFactions.end(); p++ )
	{
		if( (*p)->GetGUID()  ==  guid )
		{
			(*p)->SetUnionGUID( NULL_GUID );
			m_listFactions.erase( p );

			return true;
		}
	}

	return false;
}



/* 
*功能：检测一个行会成员是否存在
*细节描述： 
*参数： guid 行会GUID
*返回值： bool
*修改时间：2008-11-5
*/
bool	CUnion::HasFaction( CGUID guid )
{
	return this->GetFaction( guid )  !=  NULL;
}




/* 
*功能：获取一个行会
*细节描述： 
*参数： guid 行会GUID
*返回值： 行会成员
*修改时间：2008-11-5
*/
CFaction*	CUnion::GetFaction( CGUID guid )
{
	for( std::list<CFaction*>::iterator p = m_listFactions.begin(); p != m_listFactions.end(); p++ )
	{
		if( (*p)->GetGUID()  ==  guid )
		{
			return *p;
		}
	}

	return NULL;
}



/* 
*功能：获取一个行会
*细节描述： 
*参数： szName 行会名字
*返回值： 行会成员
*修改时间：2008-11-5
*/
CFaction*	CUnion::GetFaction( LPCTSTR szName )
{
	for( std::list<CFaction*>::iterator p = m_listFactions.begin(); p != m_listFactions.end(); p++ )
	{
		if( ! lstrcmp( (*p)->GetName(), szName ) )
		{
			return *p;
		}
	}

	return NULL;
}



/* 
*功能：设置一个行会的联盟权限
*细节描述： 
*参数： guid 行会GUID
*返回值： bool
*修改时间：2008-11-5
*/
bool	CUnion::SetFacPurview( CGUID guid, long lPurview )
{
	return false;
}



/* 
*功能：获取一个行会的联盟权限
*细节描述： 
*参数： guid 行会GUID
*返回值： long
*修改时间：2008-11-5
*/
long	CUnion::GetFacPurview( CGUID guid )
{
	return NULL;
}



/*
*功能：获取联盟人数
*细节描述： 
*参数： 
*返回值： 
*修改时间：2008-11-5
*/
long	CUnion::GetNumOfMembers() const
{
	long num = 0;

	for( std::list<CFaction*>::const_iterator p = m_listFactions.begin(); p != m_listFactions.end(); p++ )
	{
		num += (*p)->GetNumOfMembers();
	}

	return num;
}



/*
*功能：获取在线盟友数量
*细节描述： 
*参数： 无
*返回值： 无
*修改时间：2008-11-5
*/
long	CUnion::GetNumOfOnlineMembers() const
{
	long num = 0;

	for( std::list<CFaction*>::const_iterator p = m_listFactions.begin(); p != m_listFactions.end(); p++ )
	{
		num += (*p)->GetNumOfOnlineMembers();
	}

	return num;
}



/*
*功能： 按照指定规则排序 家族
*细节描述： 
*参数： eSort	排序规则枚举
*返回值： 
*修改时间：2008-4-16
*/
void	CUnion::SortFactions( CFaction::ENUM_SORT eSort )
{
	switch( eSort )
	{
		case CFaction::SORT_BY_FACIOTN:			m_listFactions.sort( CFaction::CompareByFaction );			break;
		case CFaction::SORT_BY_CHAIRMAN:		m_listFactions.sort( CFaction::CompareByChairman );			break;
		case CFaction::SORT_BY_ONLINE:			m_listFactions.sort( CFaction::CompareByOnline );			break;
		case CFaction::SORT_BY_LEVEL:			m_listFactions.sort( CFaction::CompareByLevel );			break;
	}
}








//**************************************************************************************
// 联盟逻辑管理类
//**************************************************************************************

/* 
*功能：构造函数
*细节描述： 将成员指针置为NULL。
*参数： 无
*返回值： 无
*修改时间：2008-11-5
*/
CUnionManager::CUnionManager()
{
}


/*
*功能：析构函数
*细节描述： 释放资源。
*参数： 无
*返回值： 无
*修改时间：2008-11-5
*/
CUnionManager::~CUnionManager()
{
	this->Release();
}



/* 
*功能：释放函数
*细节描述： 
*参数： 无
*返回值： 无
*修改时间：2008-11-5
*/
void	CUnionManager::Release()
{
	for( std::map<CGUID,CUnion*>::iterator p = m_mapUnions.begin(); p != m_mapUnions.end(); p++ )
	{
		SAFE_DELETE( p->second );
	}

	m_mapUnions.clear();
}



/*
*功能：判断是否拥有联盟
*细节描述： 
*参数： guid	联盟GUID
*返回值： 无
*修改时间：2008-11-5
*/
bool	CUnionManager::HasUnion( CGUID guid )
{
	return m_mapUnions.find( guid )  !=  m_mapUnions.end();
}



/*
*功能：判断自己是否在此联盟
*细节描述： 
*参数： 
*返回值： 无
*修改时间：2008-11-5
*/
bool	CUnionManager::AmIJoinedUnion()
{
	return this->IsPlayerJoinedUnion( GetGame()->GetMainPlayer() );
}


/*
*功能：判断自己是否在此联盟
*细节描述： 
*参数： guidUnion	联盟GUID
*返回值： 无
*修改时间：2008-11-5
*/
bool	CUnionManager::AmIJoinedUnion( CGUID guidUnion )
{
	return this->IsPlayerJoinedUnion( GetGame()->GetMainPlayer(), guidUnion );
}



/*
*功能：判断Player是否有联盟
*细节描述： 
*参数： guidUnion	联盟GUID
*返回值： 无
*修改时间：2008-11-5
*/
bool	CUnionManager::IsPlayerJoinedUnion( CPlayer* pPlayer )
{
	if( pPlayer )
		return pPlayer->GetUnionID() != NULL_GUID;

	return false;
}



/*
*功能：判断Player是否在此联盟
*细节描述： 
*参数： guidUnion	联盟GUID
*返回值： 无
*修改时间：2008-11-5
*/
bool	CUnionManager::IsPlayerJoinedUnion( CPlayer* pPlayer, CGUID guidUnion )
{
	if( pPlayer )
		return pPlayer->GetUnionID() == guidUnion;

	return false;
}




/* 
*功能：我是否盟主
*细节描述： 
*参数： 
*返回值： bool
*修改时间：2008-11-5
*/
bool	CUnionManager::AmIChairman()
{
	return this->IsPlayerChairman( GetGame()->GetMainPlayer() );
}



/* 
*功能：玩家是否盟主
*细节描述： 
*参数： guidPlayer
*返回值： bool
*修改时间：2008-11-5
*/
bool	CUnionManager::IsPlayerChairman( CGUID guidUnion, CGUID guidPlayer )
{
	if( CUnion* pUnion = this->GetUnion( guidUnion ) )
	{
		return pUnion->GetMasterID() == guidPlayer;
	}

	return false;
}



/* 
*功能：玩家是否盟主
*细节描述： 
*参数： pPlayer
*返回值： bool
*修改时间：2008-11-5
*/
bool	CUnionManager::IsPlayerChairman( CPlayer* pPlayer )
{
	if( pPlayer )
	{
		if( CUnion* pUnion = this->GetUnion( pPlayer->GetUnionID() ) )
		{
			return pUnion->GetMasterID() == pPlayer->GetExID();
		}
	}

	return false;
}



/* 
*功能：添加一个联盟
*细节描述： 
*参数： pstUnionInfo
*返回值： 无
*修改时间：2008-11-5
*/
bool	CUnionManager::AddUnion( CUnion::SUnionInfo* pstUnionInfo )
{
	if( pstUnionInfo )
	{
		// 检测联盟是否已经存在
		if( this->HasUnion( pstUnionInfo->guidUnion ) )
		{
			// 更新数据
			m_mapUnions[ pstUnionInfo->guidUnion ]->SetUnionInfo( pstUnionInfo );
		}
		else
		{
			// 添加新联盟
			m_mapUnions[ pstUnionInfo->guidUnion ] = new CUnion( pstUnionInfo );
		}

		return true;
	}

	return false;
}


/* 
*功能：删除一个联盟
*细节描述： 
*参数： guid
*返回值： 无
*修改时间：2008-11-5
*/
bool	CUnionManager::DelUnion( CGUID guid )
{
	// 遍历处理场景内所有的玩家
	std::list<CBaseObject*>*	pListOfObjects = GetGame()->GetRegion()->GetObjectList();
	for( std::list<CBaseObject*>::iterator p = pListOfObjects->begin(); p != pListOfObjects->end(); p++ )
	{
		if( (*p)->GetType() == TYPE_PLAYER )
		{
			if( CPlayer* pPlayer = dynamic_cast<CPlayer*>(*p) )
			{
				// 找到该联盟的Player进行处理
				if( this->IsPlayerJoinedUnion( pPlayer, guid ) )
				{
					pPlayer->SetUnionID( NULL_GUID );
				}
			}
		}
	}

	// 清除联盟内的家族的联盟信息
	list<CFaction*>*	plistFactions = GetGame()->GetFactionManager()->GetListOfFactions();
	for( list<CFaction*>::iterator p = plistFactions->begin(); p != plistFactions->end(); p++ )
	{
		CFaction* pFaction = *p;

		if( pFaction->GetUnionGUID() == guid )
			pFaction->SetUnionGUID( NULL_GUID );
	}

	// 查找要删除的联盟
	std::map<CGUID,CUnion*>::iterator	p = m_mapUnions.find( guid );

	if( p  !=  m_mapUnions.end() )
	{
		// 删除一个联盟
		SAFE_DELETE( (*p).second );
		m_mapUnions.erase( p );

		return true;
	}

	return false;
}



/* 
*功能：获取一个联盟
*细节描述： 
*参数： guid
*返回值： 无
*修改时间：2008-11-5
*/
CUnion*	CUnionManager::GetUnion( CGUID guid )
{
	std::map<CGUID,CUnion*>::iterator	p = m_mapUnions.find( guid );

	if( p  !=  m_mapUnions.end() )
	{
		return (*p).second;
	}

	return NULL;
}



/* 
*功能：获取我的官阶
*细节描述： 
*参数： 
*返回值： 无
*修改时间：2008-11-5
*/
long	CUnionManager::GetMyJob()
{
	//return GetGame()->GetMainPlayer()->GetFactionJob();
	return 0;
}



/* 
*功能：设置我的官阶
*细节描述： 
*参数： lJob
*返回值： 无
*修改时间：2008-11-5
*/
bool	CUnionManager::SetMyJob( long lJob )
{
	//if( lJob >= -1  &&  lJob <= MAXNUM_JobLvl )
	//	GetGame()->GetMainPlayer()->SetFactionJob( lJob );
	//else
	//	GetGame()->GetMainPlayer()->SetFactionJob( 0 );

	return true;
}




/* 
*功能：获取我的联盟公告
*细节描述： 
*参数： 
*返回值： 无
*修改时间：2008-11-5
*/
LPCTSTR	CUnionManager::GetMyPlacard()
{
	if( CUnion*	pMyUnion = this->GetMyUnion() )
	{
		return pMyUnion->GetPlacard();
	}

	return "";
}



/* 
*功能：设置我的联盟公告
*细节描述： 
*参数： szPlacard
*返回值： 无
*修改时间：2008-11-5
*/
bool	CUnionManager::SetMyPlacard( LPCTSTR szPlacard )
{
	if( CUnion*	pMyUnion = this->GetMyUnion() )
	{
		pMyUnion->SetPlacard( szPlacard );
		return true;
	}

	return false;
}



/* 
*功能：获取我的联盟据点信息
*细节描述： 
*参数： 
*返回值： 无
*修改时间：2008-11-5
*/
LPCTSTR	CUnionManager::GetMyBeachhead()
{
	if( CUnion*	pMyUnion = this->GetMyUnion() )
	{
		return pMyUnion->GetBeachhead();
	}

	return "";
}



/* 
*功能：设置我的联盟据点信息
*细节描述： 
*参数： szBeachhead
*返回值： 无
*修改时间：2008-11-5
*/
bool	CUnionManager::SetMyBeachhead( LPCTSTR szBeachhead )
{
	if( CUnion*	pMyUnion = this->GetMyUnion() )
	{
		pMyUnion->SetBeachhead( szBeachhead );
		return true;
	}

	return false;
}





/* 
*功能：获取我的联盟
*细节描述： 
*参数： guid
*返回值： 无
*修改时间：2008-11-5
*/
CUnion*	CUnionManager::GetMyUnion()
{
	if( CPlayer* pPlayer = GetGame()->GetMainPlayer() )
	{
		return this->GetUnion( pPlayer->GetUnionID() );
	}

	return NULL;
}


/* 
*功能：获取家族的联盟
*细节描述： 
*参数： guid
*返回值： 无
*修改时间：2008-11-5
*/
CUnion*	CUnionManager::GetFacUnion( CGUID guidFaction )
{
	for( std::map<CGUID,CUnion*>::iterator p = m_mapUnions.begin(); p != m_mapUnions.end(); p++ )
	{
		if( CUnion* pUnion = p->second )
		{
			if( pUnion->HasFaction( guidFaction ) )
				return pUnion;
		}
	}

	return NULL;
}




/* 
*功能：获取我的权利
*细节描述： 
*参数： guid
*返回值： bool
*修改时间：2008-11-5
*/
bool	CUnionManager::GetMyRights( eFactionPurviewIndex ePV )
{
	CUnion*	pMyUnion = this->GetMyUnion();
	CPlayer*	pMainPlayer = GetGame()->GetMainPlayer();

	// 是否有联盟
	if( pMyUnion && pMainPlayer )
	{
		return true;
	}

	return false;
}



/*
*功能： 接收创建联盟脚本 协议
*细节描述： 
*参数： 无
*返回值： 无
*修改时间：2008-11-5
*/
void	CUnionManager::RecvScriptCreationMsg( CMessage* pMsg )
{
	//CUnionPageEx*	pUnionPageEx = GetGame()->GetCGuiEx()->GetUnionPageEx();

	//// 检测 成功失败
	//if( PLAYER_OPT_SUCCEED == pMsg->GetLong() )
	//{
	//	// 打开联盟创建UI
	//	pUnionPageEx->OpenCreationUI();
	//}
	//else
	//{
	//	// 处理错误
	//}
}



/*
*功能： 接收 创建联盟协议
*细节描述： 
*参数： pMsg	协议结构
*返回值： 无
*修改时间：2008-4-16
*/
void	CUnionManager::RecvCreationMsg( CMessage* pMsg )
{
	// 检测 成功失败
	if( PLAYER_OPT_SUCCEED == pMsg->GetLong() )
	{
		// "联盟创建成功！"
		/*GetGame()->GetCGuiEx()->GetMessageBoxPageEx()->m_lCallerID = -1;
		GetGame()->GetCGuiEx()->GetMessageBoxPageEx()->Open( GetGame()->GetCGuiEx()->GetFactionPageEx(), "联盟创建成功！", CMessageBoxPageEx::MSG_OK, true );*/
        GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-19.wav"); 
	}
	else
	{
		// 处理错误
	}
}



/*
*功能： 接收 创建联盟通知
*细节描述： 
*参数： pMsg	协议结构
*返回值： 无
*修改时间：2008-4-16
*/
void	CUnionManager::RecvCreationNotifyMsg( CMessage* pMsg )
{
	//// 行会 GUID
	//CGUID	guidFaction;
	//pMsg->GetGUID( guidFaction );

	//// 联盟 GUID
	//CGUID	guidUnion;
	//pMsg->GetGUID( guidUnion );

	//// 联盟 名字
	//TCHAR	szUnionName[ MAX_MEMBER_NAME_SIZE ];
	//pMsg->GetStr( szUnionName, MAX_MEMBER_NAME_SIZE );


	//// 添加一个联盟
	//tagUnionBaseData	stUnionInfo;
	//if( CUnion* pUnion = this->GetUnion( guidUnion ) )
	//{
	//	stUnionInfo = *pUnion->GetUnionInfo();
	//}

	//stUnionInfo.guid = guidUnion;
	//stUnionInfo.MasterFacGuid = guidFaction;
	//lstrcpy( stUnionInfo.szName, szUnionName );

	//this->AddUnion( &stUnionInfo );

	//// 在盟内添加 主行会
	//if( ! this->GetUnion( guidUnion )->HasFaction( guidFaction ) )
	//{
	//	tagUnionMemInfo	stFacInfo;
	//	stFacInfo.MemberGuid = guidFaction;

	//	this->GetUnion( guidUnion )->AddFaction( &stFacInfo );
	//}


	//// 如果是主角的行会创建了联盟
	//if( this->AmIJoinedUnion( guidUnion ) )
	//{
	//	// 提示信息
	//	//TCHAR	szPrompt[ 1024 ];
	//	//wsprintf( "您所在的行会创建了联盟：%s", szUnionName );
	//	//GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText( eNOTIFYPOS_CENTER, szPrompt, 0xffffffff );
	//}
}



/*
*功能： 一个行会成员加入联盟通知
*细节描述： 
*参数： 无
*返回值： 无
*修改时间：2008-11-5
*/
void	CUnionManager::RecvJoinMsg( CMessage* pMsg )
{
	//// 行会 GUID
	//CGUID	guidFaction;
	//pMsg->GetGUID( guidFaction );

	//// 联盟 GUID
	//CGUID	guidUnion;
	//pMsg->GetGUID( guidUnion );

	//// 联盟 名字
	//TCHAR	szUnionName[ MAX_MEMBER_NAME_SIZE ];
	//pMsg->GetStr( szUnionName, MAX_MEMBER_NAME_SIZE );

	//// 添加一个联盟
	//tagUnionBaseData	stUnionInfo;
	//if( CUnion* pUnion = this->GetUnion( guidUnion ) )
	//{
	//	stUnionInfo = *pUnion->GetUnionInfo();
	//}

	//stUnionInfo.guid = guidUnion;
	//lstrcpy( stUnionInfo.szName, szUnionName );

	//this->AddUnion( &stUnionInfo );


	//// 在盟内添加 行会
	//if( ! this->GetUnion( guidUnion )->HasFaction( guidFaction ) )
	//{
	//	tagUnionMemInfo	stFacInfo;
	//	stFacInfo.MemberGuid = guidFaction;

	//	this->GetUnion( guidUnion )->AddFaction( &stFacInfo );
	//}

	//// 如果是主角的行会加入了联盟
	//if( this->AmIJoinedUnion( guidUnion ) )
	//{
	//	// 提示信息
	//	TCHAR	szPrompt[ 1024 ];
	//	wsprintf( "您所在的行会加入了联盟：%s", szUnionName );
	//	GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText( eNOTIFYPOS_CENTER, szPrompt, 0xffffffff );
	//}
}



/*
*功能： 一个联盟的基本数据通知
*细节描述： 
*参数： 无
*返回值： 无
*修改时间：2008-11-5
*/
void	CUnionManager::RecvUnionInfoMsg( CMessage* pMsg )
{
	//// 保存联盟基本信息
	//tagUnionBaseData	stUnionInfo;
	//pMsg->GetEx( &stUnionInfo, sizeof(stUnionInfo) );

	//// 如果存在则更新，否则新加
	//this->AddUnion( &stUnionInfo );

	// 刷新UI数据
}



/*
*功能： 一个联盟的所有成员数据通知
*细节描述： 
*参数： 无
*返回值： 无
*修改时间：2008-11-5
*/
void	CUnionManager::RecvUnionMemberMsg( CMessage* pMsg )
{
	//// 联盟 GUID
	//CGUID	guidUnion;
	//pMsg->GetGUID( guidUnion );

	//// 保存行会成员信息
	//tagUnionMemInfo	stFacInfo;
	//pMsg->GetEx( &stFacInfo, sizeof(stFacInfo) );

	////
	//if( CUnion* pUnion = this->GetUnion( guidUnion ) )
	//{
	//	pUnion->AddFaction( &stFacInfo );
	//}
}



/*
*功能： 发送 创建联盟协议
*细节描述： 
*参数： strFactionName	行会名称
*返回值： 无
*修改时间：2008-4-16
*/
void	CUnionManager::SendCreationMsg( LPCTSTR szName )
{
	// 创建联盟协议
	CMessage msg( MSG_C2S_UNION_Request_Creation );

		// 联盟名称
		msg.Add( szName );

	msg.Send();
}



/*
*功能： 发送 解散联盟协议
*细节描述： 
*参数： 
*返回值： 无
*修改时间：2008-4-16
*/
void	CUnionManager::SendDisbandMsg()
{
	// 解散联盟协议
	CMessage msg( MSG_C2S_UNION_Request_Disband );

	msg.Send();
}



/*
*功能： 接收 解散联盟回复
*细节描述： 
*参数： pMsg	协议结构
*返回值： 无
*修改时间：2008-4-16
*/
void	CUnionManager::RecvDisbandMsg( CMessage* pMsg )
{
	// 联盟 GUID
	CGUID	guidUnion;
	pMsg->GetGUID( guidUnion );

	// 如果是主角的联盟解散了
	if( this->AmIJoinedUnion( guidUnion ) )
	{
		// 提示信息
		//TCHAR	szPrompt[ 1024 ];
		//wsprintf( "您所在的联盟：%s 解散了！", szUnionName );
		//GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText( eNOTIFYPOS_CENTER, szPrompt, 0xffffffff );
	}

	// 解散
	this->DelUnion( guidUnion );
}



/*
*功能： 发送 退出联盟协议
*细节描述： 
*参数： 
*返回值： 无
*修改时间：2008-4-16
*/
void	CUnionManager::SendQuitMsg()
{
	CMessage msg( MSG_C2S_UNION_Request_Quit );

	msg.Send();
}



/*
*功能： 接收 退出联盟回复
*细节描述： 
*参数： pMsg	协议结构
*返回值： 无
*修改时间：2008-4-16
*/
void	CUnionManager::RecvQuitMsg( CMessage* pMsg )
{
	// 行会 GUID
	CGUID	guidFaction;
	pMsg->GetGUID( guidFaction );

	// 如果是主角的行会退出联盟了
	//if( this->AmIJoinedUnion( guidUnion ) )
	{
		// 提示信息
		//TCHAR	szPrompt[ 1024 ];
		//wsprintf( "您所在的联盟：%s 解散了！", szUnionName );
		//GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText( eNOTIFYPOS_CENTER, szPrompt, 0xffffffff );
	}

	// 解散
	//this->DelUnion( guidUnion );
}




/*
*功能： 发送 踢出联盟协议
*细节描述： 
*参数： guidFaction
*返回值： 无
*修改时间：2008-4-16
*/
void	CUnionManager::SendKickMsg( CGUID guidFaction )
{
	CMessage msg( MSG_C2S_UNION_Request_Kick );

		// 踢出行会的 GUID
		msg.Add( guidFaction );

	msg.Send();
}



/*
*功能： 接收 踢出联盟回复
*细节描述： 
*参数： pMsg	协议结构
*返回值： 无
*修改时间：2008-4-16
*/
void	CUnionManager::RecvKickMsg( CMessage* pMsg )
{
	// 行会 GUID
}



/*
*功能： 发送 盟主转让请求
*细节描述： 
*参数： guidChairman
*返回值： 无
*修改时间：2008-4-16
*/
void	CUnionManager::SendSetChairmanMsg( CGUID guidChairman )
{
	CMessage msg( MSG_C2S_UNION_Request_SetChairman );

	msg.Send();
}



/*
*功能： 接收 盟主转让回复
*细节描述： 
*参数： pMsg	协议结构
*返回值： 无
*修改时间：2008-4-16
*/
void	CUnionManager::RecvSetChairmanMsg( CMessage* pMsg )
{
}



/*
*功能： 发送 盟主设置行会权限请求
*细节描述： 
*参数： guidFaction
*返回值： 无
*修改时间：2008-4-16
*/
void	CUnionManager::SendSetRightsMsg( CGUID guidFaction )
{
	CMessage msg( MSG_C2S_UNION_Request_SetRights );

	msg.Send();
}



/*
*功能： 盟主接收 权限设置结果回复
*细节描述： 
*参数： pMsg	协议结构
*返回值： 无
*修改时间：2008-4-16
*/
void	CUnionManager::RecvSetRightsMsg( CMessage* pMsg )
{
}



/*
*功能： 会长接收 盟主设置的权限
*细节描述： 
*参数： pMsg	协议结构
*返回值： 无
*修改时间：2008-4-16
*/
void	CUnionManager::RecvGetRightsMsg( CMessage* pMsg )
{
}



/*
*功能： 发送 盟主 邀请行会会长 加入联盟的发送请求
*细节描述： 
*参数： guidFaction
*返回值： 无
*修改时间：2008-4-16
*/
void	CUnionManager::SendInviteMsg( LPCTSTR szName )
{
	CMessage msg( MSG_C2S_UNION_Request_Invite );

		// 玩家名字
		msg.Add( szName );

	msg.Send();
}



/*
*功能： 行会会长 收到 盟主邀请加入联盟的请求
*细节描述： 
*参数： pMsg	协议结构
*返回值： 无
*修改时间：2008-4-16
*/
void	CUnionManager::RecvInviteMsg( CMessage* pMsg )
{
}


/*
*功能： 行会会长 发送回复盟主的邀请
*细节描述： 
*参数： guidFaction
*返回值： 无
*修改时间：2008-4-16
*/
void	CUnionManager::SendResponseMsg( CGUID guidFaction )
{
	CMessage msg( MSG_C2S_UNION_Request_Invite );

	msg.Send();
}



/*
*功能： 盟主 收到 行会会长 回复的邀请结果
*细节描述： 
*参数： pMsg	协议结构
*返回值： 无
*修改时间：2008-4-16
*/
void	CUnionManager::RecvResponseMsg( CMessage* pMsg )
{
}

