/*Faction.cpp
*修改者: 王峰
*修改日期:2008-11-5
*/

#include "StdAfx.h"
#include "Faction.h"

#include "Player.h"
#include "Other/CountryList.h"
#include "Message.h"
#include "../GameClient/Game.h"
#include "ClientRegion.h"
#include "ClientWordsFilter.h"    
#include "../organizingsystem/Union.h"
#include "../MsgBoxSys/MsgEventManager.h"
#include "../../../Public/Common/OrganizingParam.h"
#include "../../../Public/Common/Public.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



//**************************************************************************************
// 族员的 逻辑功能类
//**************************************************************************************

/* 
*功能：构造函数
*细节描述： 将成员指针置为NULL。
*参数： 无
*返回值： 无
*修改时间：2008-11-5
*/
CFacMember::CFacMember()
{
	ZeroMemory( &m_stMemberInfo, sizeof(m_stMemberInfo) );
}


/*
*功能：析构函数
*细节描述： 释放资源。
*参数： 无
*返回值： 无
*修改时间：2008-11-5
*/
CFacMember::~CFacMember()
{
	ZeroMemory( &m_stMemberInfo, sizeof(m_stMemberInfo) );
}

/* 
*功能：构造函数
*细节描述： 
*参数： pstMemberInfo
*返回值： 无
*修改时间：2008-11-5
*/
CFacMember::CFacMember( SMemberInfo* pstMemberInfo )
{
	this->SetMemberInfo( pstMemberInfo );
}


/* 
*功能：获取职业字符串
*细节描述： 
*参数： 
*返回值： 无
*修改时间：2008-11-5
*/
LPCTSTR	CFacMember::GetOccuText() const
{
	if( this->GetOccupation() >= 0  &&  this->GetOccupation() < OCC_Max )
		return CPlayer::s_strOccupation[ this->GetOccupation() ].c_str();

	return "";
}



/*
*功能：强制排序规则
*细节描述： 
*参数： pFacMember1		族员1
*		pFacMember2		族员2
*返回值： -1 则在前面，1在后面，0不影响
*修改时间：2008-4-16
*/
int CFacMember::CompareForceRule( const CFacMember* pFacMember1, const CFacMember* pFacMember2 )
{
	// 族长第一
	if( Exceptive_JobLvl == pFacMember1->GetJob() )
	{
		return 1;
	}
	else if( Exceptive_JobLvl == pFacMember2->GetJob() )
	{
		return -1;
	}
	// 自己第二
	else if( GetGame()->GetMainPlayer()->GetExID() == pFacMember1->GetGUID() )
	{
		return 1;
	}
	else if( GetGame()->GetMainPlayer()->GetExID() == pFacMember2->GetGUID() )
	{
		return -1;
	}

	return 0;
}



/*
*功能：基本排序比较
*细节描述： 
*参数： pFacMember1		族员1
*		pFacMember2		族员2
*返回值： false 则在前面
*修改时间：2008-4-16
*/
bool CFacMember::CompareNormal( const CFacMember* pFacMember1, const CFacMember* pFacMember2 )
{
	// 先根据是否在线排序
	if( pFacMember1->GetOnline() > pFacMember2->GetOnline() )
	{
		return true;
	}
	else if( pFacMember1->GetOnline() < pFacMember2->GetOnline() )
	{
		return false;
	}
	// 最后对名字编码进行排序
	else
	{
		return lstrcmp( pFacMember1->GetName(), pFacMember2->GetName() ) < 0;
	}

	return true;
}



/*
*功能：名字 排序比较
*细节描述： 
*参数： pFacMember1		族员1
*		pFacMember2		族员2
*返回值： false 则在前面
*修改时间：2008-4-16
*/
bool CFacMember::CompareByName( const CFacMember* pFacMember1, const CFacMember* pFacMember2 )
{
	// 强制规则
	if( int iForceResult = CFacMember::CompareForceRule( pFacMember1, pFacMember2 ) )
	{
		return iForceResult > 0;
	}
	// 根据是否在线排序
	else if( pFacMember1->GetOnline() > pFacMember2->GetOnline() )
	{
		return true;
	}
	else if( pFacMember1->GetOnline() < pFacMember2->GetOnline() )
	{
		return false;
	}

	return lstrcmp( pFacMember1->GetName(), pFacMember2->GetName() ) < 0;
}



/*
*功能：官阶排序比较
*细节描述： 
*参数： pFacMember1		族员1
*		pFacMember2		族员2
*返回值： false 则在前面
*修改时间：2008-4-16
*/
bool CFacMember::CompareByJob( const CFacMember* pFacMember1, const CFacMember* pFacMember2 )
{
	// 按照官阶排列
	if( pFacMember1->GetJob()  !=  pFacMember2->GetJob() )
	{
		return pFacMember1->GetJob()  >  pFacMember2->GetJob();
	}

	// 最后根据等级排列
	return CFacMember::CompareByLevel( pFacMember1, pFacMember2 );
}



/*
*功能：称号排序比较
*细节描述： 
*参数： pFacMember1		族员1
*		pFacMember2		族员2
*返回值： false 则在前面
*修改时间：2008-4-16
*/
bool CFacMember::CompareByTitle( const CFacMember* pFacMember1, const CFacMember* pFacMember2 )
{
	// 按照称号排列
	if( int iTitleResult = lstrcmp( pFacMember1->GetTitle(), pFacMember2->GetTitle() ) )
	{
		return iTitleResult < 0;
	}

	// 最后根据等级排列
	return CFacMember::CompareByLevel( pFacMember1, pFacMember2 );
}




/*
*功能：族员等级排序比较
*细节描述： 
*参数： pFacMember1		族员1
*		pFacMember2		族员2
*返回值： false 则在前面
*修改时间：2008-4-16
*/
bool CFacMember::CompareByLevel( const CFacMember* pFacMember1, const CFacMember* pFacMember2 )
{
	// 按照等级排列
	if( pFacMember1->GetLevel()  !=  pFacMember2->GetLevel() )
	{
		return pFacMember1->GetLevel()  >  pFacMember2->GetLevel();
	}

	// 最后对名字编码进行排序
	return CFacMember::CompareByName( pFacMember1, pFacMember2 );
}




/*
*功能：族员状态排序比较
*细节描述： 
*参数： pFacMember1		族员1
*		pFacMember2		族员2
*返回值： false 则在前面
*修改时间：2008-4-16
*/
bool CFacMember::CompareByState( const CFacMember* pFacMember1, const CFacMember* pFacMember2 )
{
	// 按照状态排列
	if( pFacMember1->GetState()  !=  pFacMember2->GetState() )
	{
		return pFacMember1->GetState()  >  pFacMember2->GetState();
	}

	// 最后根据等级排列
	return CFacMember::CompareByLevel( pFacMember1, pFacMember2 );
}




/*
*功能：地图排序比较
*细节描述： 
*参数： pFacMember1		族员1
*		pFacMember2		族员2
*返回值： false 则在前面
*修改时间：2008-4-16
*/
bool CFacMember::CompareByRegion( const CFacMember* pFacMember1, const CFacMember* pFacMember2 )
{
	// 按照地图排列
	if( int iRegionResult = lstrcmp( pFacMember1->GetRegion(), pFacMember2->GetRegion() ) )
	{
		return iRegionResult < 0;
	}

	// 最后根据等级排列
	return CFacMember::CompareByLevel( pFacMember1, pFacMember2 );
}



/*
*功能：在线 排序比较
*细节描述： 
*参数： pFacMember1		族员1
*		pFacMember2		族员2
*返回值： false 则在前面
*修改时间：2008-4-16
*/
bool CFacMember::CompareByOnline( const CFacMember* pFacMember1, const CFacMember* pFacMember2 )
{
	// 根据是否在线排序
	if( pFacMember1->GetOnline() > pFacMember2->GetOnline() )
	{
		return true;
	}
	else if( pFacMember1->GetOnline() < pFacMember2->GetOnline() )
	{
		return false;
	}

	// 最后根据等级排列
	return CFacMember::CompareByLevel( pFacMember1, pFacMember2 );
}



/*
*功能：职业排序比较
*细节描述： 
*参数： pFacMember1		族员1
*		pFacMember2		族员2
*返回值： false 则在前面
*修改时间：2008-4-16
*/
bool CFacMember::CompareByOccupation( const CFacMember* pFacMember1, const CFacMember* pFacMember2 )
{
	// 按照职业排列
	if( pFacMember1->GetOccupation()  !=  pFacMember2->GetOccupation() )
	{
		return pFacMember1->GetOccupation()  <  pFacMember2->GetOccupation();
	}

	// 最后根据等级排列
	return CFacMember::CompareByLevel( pFacMember1, pFacMember2 );
}



/*
*功能：贡献排序比较
*细节描述： 
*参数： pFacMember1		族员1
*		pFacMember2		族员2
*返回值： false 则在前面
*修改时间：2008-4-16
*/
bool CFacMember::CompareByContribute( const CFacMember* pFacMember1, const CFacMember* pFacMember2 )
{
	// 按照贡献排列
	if( pFacMember1->GetContribute()  !=  pFacMember2->GetContribute() )
	{
		return pFacMember1->GetContribute()  <  pFacMember2->GetContribute();
	}

	// 最后根据等级排列
	return CFacMember::CompareByLevel( pFacMember1, pFacMember2 );
}


/*
*功能：选中排序比较
*细节描述： 
*参数： pFacMember1		族员1
*		pFacMember2		族员2
*返回值： false 则在前面
*修改时间：2008-4-16
*/
bool CFacMember::CompareBySelection( const CFacMember* pFacMember1, const CFacMember* pFacMember2 )
{
	// 按照选中排列
	if( pFacMember1->GetSelected() > pFacMember2->GetSelected() )
	{
		return true;
	}
	else if( pFacMember1->GetSelected() < pFacMember2->GetSelected() )
	{
		return false;
	}

	// 最后根据等级排列
	return CFacMember::CompareByLevel( pFacMember1, pFacMember2 );
}




//**************************************************************************************
// 家族的 逻辑功能类
//**************************************************************************************

/* 
*功能：构造函数
*细节描述： 将成员指针置为NULL。
*参数： 无
*返回值： 无
*修改时间：2008-11-5
*/
CFaction::CFaction()
{
}


/* 
*功能：构造函数
*细节描述： 将成员指针置为NULL。
*参数： pstFacInfo
*返回值： 无
*修改时间：2008-11-5
*/
CFaction::CFaction( SFacInfo* pstFacInfo )
{
	this->SetFacInfo( pstFacInfo );
}


/*
*功能：析构函数
*细节描述： 释放资源。
*参数： 无
*返回值： 无
*修改时间：2008-11-5
*/
CFaction::~CFaction()
{
	this->ClearMembers();
	this->ClearApply();
}



/*
*功能：清空族员函数
*细节描述： 释放资源。
*参数： 无
*返回值： 无
*修改时间：2008-11-5
*/
void	CFaction::ClearMembers()
{
	for( std::list<CFacMember*>::iterator p = m_listMembers.begin(); p != m_listMembers.end(); p++ )
	{
		SAFE_DELETE( *p );
	}

	m_listMembers.clear();
}



/*
*功能：清空申请人函数
*细节描述： 释放资源。
*参数： 无
*返回值： 无
*修改时间：2008-11-5
*/
void	CFaction::ClearApply()
{
	for( std::list<CFacMember*>::iterator p = m_listApply.begin(); p != m_listApply.end(); p++ )
	{
		SAFE_DELETE( *p );
	}

	m_listApply.clear();
}



/*
*功能：设置功能等级
*细节描述： 
*参数： eFun
*		level
*返回值： 无
*修改时间：2008-11-5
*/
void	CFaction::SetLevel( ENUM_FUN eFun, long level )
{
	if( eFun >= 0  &&  eFun < MAXNUM_FUN )
	{
		m_stFacInfo.arrLevel[ eFun ] = level;
	}
}


/*
*功能：获取功能等级
*细节描述： 
*参数： eFun
*返回值： 
*修改时间：2008-11-5
*/
long	CFaction::GetLevel( ENUM_FUN eFun ) const
{
	if( eFun >= 0  &&  eFun < MAXNUM_FUN )
	{
		return m_stFacInfo.arrLevel[ eFun ];
	}

	return -1;
}



/*
*功能：设置族长是否在线
*细节描述： 
*参数： 
*返回值： 
*修改时间：2008-11-5
*/
void	CFaction::SetOnline( bool bOnline )
{
	if( CFacMember* pMember = this->GetMember( this->GetMasterGUID() ) )
	{
		pMember->SetOnline( bOnline );
	}

	m_stFacInfo.bOnline = bOnline;
}



/*
*功能：获取族长是否在线
*细节描述： 
*参数： 
*返回值： 
*修改时间：2008-11-5
*/
bool	CFaction::GetOnline() const
{
	if( CFacMember* pMember = this->GetMember( this->GetMasterGUID() ) )
	{
		return pMember->GetOnline();
	}

	return m_stFacInfo.bOnline;
}



/*
*功能：设置官阶名字
*细节描述： 
*参数： lJobLvl
*		szJobName
*返回值： 无
*修改时间：2008-11-5
*/
void	CFaction::SetJobName( long lJobLvl, LPCTSTR szJobName )
{
	if( lJobLvl > 0  &&  lJobLvl <= MAXNUM_JobLvl && szJobName )
	{
		m_stFacInfo.arrJobName[ lJobLvl -1 ] = szJobName;
	}
}


/*
*功能：获取官阶名字
*细节描述： 
*参数： lJobLvl
*返回值： szJobName
*修改时间：2008-11-5
*/
LPCTSTR	CFaction::GetJobName( long lJobLvl ) const
{
	if( lJobLvl > 0  &&  lJobLvl <= MAXNUM_JobLvl )
	{
		return m_stFacInfo.arrJobName[ lJobLvl -1 ].c_str();
	}

	return "";
}



/*
*功能：设置官阶权限
*细节描述： 
*参数： lJobLvl
*		lJobPurview
*返回值： 无
*修改时间：2008-11-5
*/
void	CFaction::SetJobPurview( long lJobLvl, long lJobPurview )
{
	if( lJobLvl > 0  &&  lJobLvl <= MAXNUM_JobLvl )
	{
		m_stFacInfo.arrJobPurview[ lJobLvl -1 ] = lJobPurview;
	}
}


/*
*功能：获取官阶权限
*细节描述： 
*参数： lJobLvl
*		ePV
*返回值： 
*修改时间：2008-11-5
*/
bool	CFaction::GetJobPurview( long lJobLvl, eFactionPurviewIndex ePV ) const
{
	// 家族等级对 权限的影响
	if( this->GetLevel() < (long)(GetGame()->GetFactionManager()->GetFacLvlForPV( ePV )) )
	{
		return false;
	}
	// 族长拥有所有权限
	else if( Exceptive_JobLvl == lJobLvl )
	{
		return true;
	}
	// 官阶权限
	else if( lJobLvl > 0  &&  lJobLvl <= MAXNUM_JobLvl )
	{
		return ( m_stFacInfo.arrJobPurview[ lJobLvl -1 ] & ( 1 << ePV ) )  !=  0;
	}

	return false;
}



/*
*功能：设置敌对家族
*细节描述： 
*参数： eBT
*		pEnemy
*返回值： 无
*修改时间：2008-11-5
*/
void	CFaction::SetEnemy( eBattleType eBT, tagEnemy* pEnemy )
{
	if( eBT >= 0 && eBT < eMaxWar )
	{
		for( std::vector<tagEnemy>::iterator p = m_vecWarInfo[ eBT ].begin(); p != m_vecWarInfo[ eBT ].end(); p++ )
		{
			if( ! lstrcmp( (*p)._cName, pEnemy->_cName ) )
			{
				(*p) = *pEnemy;
				return;
			}
		}

		m_vecWarInfo[ eBT ].push_back( *pEnemy );
	}
}


/*
*功能：获取敌对家族
*细节描述： 
*参数： eBT
*返回值： 无
*修改时间：2008-11-5
*/
vector<tagEnemy>*	CFaction::GetEnemy( eBattleType eBT )
{
	if( eBT >= 0 && eBT < eMaxWar )
	{
		return &m_vecWarInfo[ eBT ];
	}

	return NULL;
}



/*
*功能：获取族员人数
*细节描述： 
*参数： 
*返回值： 
*修改时间：2008-11-5
*/
long	CFaction::GetNumOfMembers() const
{
	return m_stFacInfo.numMembers ? m_stFacInfo.numMembers : (long)m_listMembers.size();
}



/*
*功能：获取在线族员数量
*细节描述： 
*参数： 无
*返回值： 无
*修改时间：2008-11-5
*/
long	CFaction::GetNumOfOnlineMembers() const
{
	if( m_stFacInfo.numOnlineMembers )
		return m_stFacInfo.numOnlineMembers;

	// 
	long num = 0;

	for( std::list<CFacMember*>::const_iterator p = m_listMembers.begin(); p != m_listMembers.end(); p++ )
	{
		if( (*p)->GetOnline() )
			num++;
	}

	return num;
}



/*
*功能：获取最大族员数量
*细节描述： 
*参数： 无
*返回值： 无
*修改时间：2008-11-5
*/
long	CFaction::GetMaxNumOfMembers()
{
	return GetGame()->GetFactionManager()->GetMaxNumByFacLv( this->GetLevel() );
}



/* 
*功能：获取最低要求族员数量
*细节描述： 
*参数： 无
*返回值： 无
*修改时间：2008-11-5
*/
long	CFaction::GetRequireNumOfMembers()
{
	return GetGame()->GetFactionManager()->GetRequireNumByFacLv( this->GetLevel() );
}




/* 
*功能：添加一个族员
*细节描述： 
*参数： pstMemberInfo
*返回值： 无
*修改时间：2008-11-5
*/
bool	CFaction::AddMember( CFacMember::SMemberInfo* pstMemberInfo )
{
	if( pstMemberInfo )
	{
		// 检测族员是否已经存在
		if( CFacMember* pFacMember = this->GetMember( pstMemberInfo->guidMember ) )
		{
			// 更新数据
			pFacMember->SetMemberInfo( pstMemberInfo );
		}
		else
		{
			// 添加新族员
			m_listMembers.push_back( new CFacMember( pstMemberInfo ) );
		}

		// 遍历处理场景内所有的玩家
		std::list<CBaseObject*>*	pListOfObjects = GetGame()->GetRegion()->GetObjectList();
		for( std::list<CBaseObject*>::iterator p = pListOfObjects->begin(); p != pListOfObjects->end(); p++ )
		{
			if( (*p)->GetType() == TYPE_PLAYER )
			{
				if( CPlayer* pPlayer = dynamic_cast<CPlayer*>(*p) )
				{
					// 找到该Player进行处理
					if( pPlayer->GetExID() == pstMemberInfo->guidMember )
					{
						pPlayer->SetFactionID( this->GetGUID() );
						pPlayer->SetFactionName( string( this->GetName() ) );

						pPlayer->SetUnionID( this->GetUnionGUID() );

						break;
					}
				}
			}
		}

		return true;
	}

	return false;
}



/* 
*功能：删除一个族员
*细节描述： 
*参数： guid
*返回值： 无
*修改时间：2008-11-5
*/
bool	CFaction::DelMember( CGUID guid )
{
	// 遍历处理场景内所有的玩家
	std::list<CBaseObject*>*	pListOfObjects = GetGame()->GetRegion()->GetObjectList();
	for( std::list<CBaseObject*>::iterator p = pListOfObjects->begin(); p != pListOfObjects->end(); p++ )
	{
		if( (*p)->GetType() == TYPE_PLAYER )
		{
			if( CPlayer* pPlayer = dynamic_cast<CPlayer*>(*p) )
			{
				// 找到该Player进行处理
				if( pPlayer->GetExID() == guid )
				{
					pPlayer->SetFactionID( NULL_GUID );
					pPlayer->SetUnionID( NULL_GUID );
					pPlayer->SetFactionName( string("") );
					pPlayer->SetFactionTitle( string("") );

					break;
				}
			}
		}
	}

	for( std::list<CFacMember*>::iterator p = m_listMembers.begin(); p != m_listMembers.end(); p++ )
	{
		if( (*p)->GetGUID()  ==  guid )
		{
			SAFE_DELETE( *p );
			m_listMembers.erase( p );

			return true;
		}
	}

	return false;
}



/* 
*功能：是否拥有一个族员
*细节描述： 
*参数： guid
*返回值： 无
*修改时间：2008-11-5
*/
bool	CFaction::HasMember( CGUID guid ) const
{
	return this->GetMember( guid )  !=  NULL;
}



/* 
*功能：获取一个族员
*细节描述： 
*参数： guid
*返回值： 无
*修改时间：2008-11-5
*/
CFacMember*	CFaction::GetMember( CGUID guid ) const
{
	for( std::list<CFacMember*>::const_iterator p = m_listMembers.begin(); p != m_listMembers.end(); p++ )
	{
		if( (*p)->GetGUID()  ==  guid )
		{
			return *p;
		}
	}

	return NULL;
}



/* 
*功能：获取一个族员
*细节描述： 
*参数： szName
*返回值： 无
*修改时间：2008-11-5
*/
CFacMember*	CFaction::GetMember( LPCTSTR szName ) const
{
	for( std::list<CFacMember*>::const_iterator p = m_listMembers.begin(); p != m_listMembers.end(); p++ )
	{
		if( ! lstrcmp( (*p)->GetName(), szName ) )
		{
			return *p;
		}
	}

	return NULL;
}




/* 
*功能：添加一个申请者
*细节描述： 
*参数： pstMemberInfo
*返回值： 无
*修改时间：2008-11-5
*/
bool	CFaction::AddApply( CFacMember::SMemberInfo* pstMemberInfo )
{
	if( pstMemberInfo )
	{
		// 检测申请者是否已经存在
		if( CFacMember* pFacMember = this->GetApply( pstMemberInfo->guidMember ) )
		{
			// 更新数据
			pFacMember->SetMemberInfo( pstMemberInfo );
		}
		else
		{
			// 添加新申请者
			m_listApply.push_back( new CFacMember( pstMemberInfo ) );
		}

		return true;
	}

	return false;
}



/* 
*功能：删除一个申请者
*细节描述： 
*参数： guid
*返回值： 无
*修改时间：2008-11-5
*/
bool	CFaction::DelApply( CGUID guid )
{
	for( std::list<CFacMember*>::iterator p = m_listApply.begin(); p != m_listApply.end(); p++ )
	{
		if( (*p)->GetGUID()  ==  guid )
		{
			SAFE_DELETE( *p );
			m_listApply.erase( p );

			return true;
		}
	}

	return false;
}



/* 
*功能：是否拥有一个申请者
*细节描述： 
*参数： guid
*返回值： 无
*修改时间：2008-11-5
*/
bool	CFaction::HasApply( CGUID guid ) const
{
	return this->GetApply( guid )  !=  NULL;
}



/* 
*功能：获取一个申请者
*细节描述： 
*参数： guid
*返回值： 无
*修改时间：2008-11-5
*/
CFacMember*	CFaction::GetApply( CGUID guid ) const
{
	for( std::list<CFacMember*>::const_iterator p = m_listApply.begin(); p != m_listApply.end(); p++ )
	{
		if( (*p)->GetGUID()  ==  guid )
		{
			return *p;
		}
	}

	return NULL;
}





/* 
*功能：获取一个申请者
*细节描述： 
*参数： szName
*返回值： 无
*修改时间：2008-11-5
*/
CFacMember*	CFaction::GetApply( LPCTSTR szName ) const
{
	for( std::list<CFacMember*>::const_iterator p = m_listApply.begin(); p != m_listApply.end(); p++ )
	{
		if( ! lstrcmp( (*p)->GetName(), szName ) )
		{
			return *p;
		}
	}

	return NULL;
}



/*
*功能： 按照指定规则排序 族员
*细节描述： 
*参数： eSort	排序规则枚举
*返回值： 
*修改时间：2008-4-16
*/
void	CFaction::SortMembers( CFacMember::ENUM_SORT eSort )
{
	switch( eSort )
	{
		case CFacMember::SORT_BY_NORMAL:		m_listMembers.sort( CFacMember::CompareNormal );			break;
		case CFacMember::SORT_BY_NAME:			m_listMembers.sort( CFacMember::CompareByName );			break;
		case CFacMember::SORT_BY_JOB:			m_listMembers.sort( CFacMember::CompareByJob );				break;
		case CFacMember::SORT_BY_TITLE:			m_listMembers.sort( CFacMember::CompareByTitle );			break;
		case CFacMember::SORT_BY_LEVEL:			m_listMembers.sort( CFacMember::CompareByLevel );			break;
		case CFacMember::SORT_BY_STATE:			m_listMembers.sort( CFacMember::CompareByState );			break;
		case CFacMember::SORT_BY_REGION:		m_listMembers.sort( CFacMember::CompareByRegion );			break;
		case CFacMember::SORT_BY_ONLINE:		m_listMembers.sort( CFacMember::CompareByOnline );			break;
		case CFacMember::SORT_BY_OCCUPATION:	m_listMembers.sort( CFacMember::CompareByOccupation );		break;
		case CFacMember::SORT_BY_CONTRIBUTE:	m_listMembers.sort( CFacMember::CompareByContribute );		break;
		case CFacMember::SORT_BY_SELECTION:		m_listMembers.sort( CFacMember::CompareBySelection );		break;
	}
}



/*
*功能： 按照指定规则排序 申请者
*细节描述： 
*参数： eSort	排序规则枚举
*返回值： 
*修改时间：2008-4-16
*/
void	CFaction::SortApply( CFacMember::ENUM_SORT eSort )
{
	switch( eSort )
	{
		case CFacMember::SORT_BY_NORMAL:		m_listApply.sort( CFacMember::CompareNormal );				break;
		case CFacMember::SORT_BY_NAME:			m_listApply.sort( CFacMember::CompareByName );				break;
		case CFacMember::SORT_BY_JOB:			m_listApply.sort( CFacMember::CompareByJob );				break;
		case CFacMember::SORT_BY_TITLE:			m_listApply.sort( CFacMember::CompareByTitle );				break;
		case CFacMember::SORT_BY_LEVEL:			m_listApply.sort( CFacMember::CompareByLevel );				break;
		case CFacMember::SORT_BY_STATE:			m_listApply.sort( CFacMember::CompareByState );				break;
		case CFacMember::SORT_BY_REGION:		m_listApply.sort( CFacMember::CompareByRegion );			break;
		case CFacMember::SORT_BY_ONLINE:		m_listApply.sort( CFacMember::CompareByOnline );			break;
		case CFacMember::SORT_BY_OCCUPATION:	m_listApply.sort( CFacMember::CompareByOccupation );		break;
		case CFacMember::SORT_BY_CONTRIBUTE:	m_listApply.sort( CFacMember::CompareByContribute );		break;
		case CFacMember::SORT_BY_SELECTION:		m_listApply.sort( CFacMember::CompareBySelection );			break;
	}
}





/*
*功能：强制排序规则
*细节描述： 
*参数： CFaction		族员1
*		CFaction		族员2
*返回值： -1 则在前面，1在后面，0不影响
*修改时间：2008-4-16
*/
int CFaction::CompareForceRule( const CFaction* pFaction1, const CFaction* pFaction2 )
{
	// 盟主第一
	if( GetGame()->GetUnionManager()->IsPlayerChairman( pFaction1->GetUnionGUID(), pFaction1->GetMasterGUID() ) )
	{
		return 1;
	}
	else if( GetGame()->GetUnionManager()->IsPlayerChairman( pFaction2->GetUnionGUID(), pFaction2->GetMasterGUID() ) )
	{
		return -1;
	}
	// 自己第二
	else if( GetGame()->GetMainPlayer()->GetFactionID() == pFaction1->GetGUID() )
	{
		return 1;
	}
	else if( GetGame()->GetMainPlayer()->GetFactionID() == pFaction2->GetGUID() )
	{
		return -1;
	}
	// 根据是否在线排序
	else if( pFaction1->GetOnline() > pFaction2->GetOnline() )
	{
		return 1;
	}
	else if( pFaction1->GetOnline() < pFaction2->GetOnline() )
	{
		return -1;
	}

	return 0;
}




/*
*功能：家族名字排序比较
*细节描述： 
*参数： pFaction1		族员1
*		pFaction2		族员2
*返回值： false 则在前面
*修改时间：2008-4-16
*/
bool CFaction::CompareByFaction( const CFaction* pFaction1, const CFaction* pFaction2 )
{
	// 强制规则
	if( int iForceResult = CFaction::CompareForceRule( pFaction1, pFaction2 ) )
	{
		return iForceResult > 0;
	}

	// 最后对名字编码进行排序
	return lstrcmp( pFaction1->GetName(), pFaction2->GetName() ) < 0;
}



/*
*功能：族长名字排序比较
*细节描述： 
*参数： pFaction1		族员1
*		pFaction2		族员2
*返回值： false 则在前面
*修改时间：2008-4-16
*/
bool CFaction::CompareByChairman( const CFaction* pFaction1, const CFaction* pFaction2 )
{
	// 强制规则
	if( int iForceResult = CFaction::CompareForceRule( pFaction1, pFaction2 ) )
	{
		return iForceResult > 0;
	}

	// 最后对名字编码进行排序
	return lstrcmp( pFaction1->GetMasterName(), pFaction2->GetMasterName() ) < 0;
}



/*
*功能：家族在线人数排序比较
*细节描述： 
*参数： pFacMember1		族员1
*		pFacMember2		族员2
*返回值： false 则在前面
*修改时间：2008-4-16
*/
bool CFaction::CompareByOnline( const CFaction* pFaction1, const CFaction* pFaction2 )
{
	// 按照在线人数排列
	long numOnlineMembers1 = pFaction1->GetNumOfOnlineMembers();
	long numOnlineMembers2 = pFaction2->GetNumOfOnlineMembers();

	if(  numOnlineMembers1 !=  numOnlineMembers2 )
	{
		return numOnlineMembers1  >  numOnlineMembers2;
	}

	// 最后对名字编码进行排序
	return CFaction::CompareByFaction( pFaction1, pFaction2 );
}


/*
*功能：家族等级排序比较
*细节描述： 
*参数： pFacMember1		族员1
*		pFacMember2		族员2
*返回值： false 则在前面
*修改时间：2008-4-16
*/
bool CFaction::CompareByLevel( const CFaction* pFaction1, const CFaction* pFaction2 )
{
	// 按照等级排列
	if( pFaction1->GetLevel()  !=  pFaction2->GetLevel() )
	{
		return pFaction1->GetLevel()  >  pFaction2->GetLevel();
	}

	// 最后对名字编码进行排序
	return CFaction::CompareByFaction( pFaction1, pFaction2 );
}





//**************************************************************************************
// 家族 逻辑管理类
//**************************************************************************************

/* 
*功能：构造函数
*细节描述： 将成员指针置为NULL。
*参数： 无
*返回值： 无
*修改时间：2008-11-5
*/
CFactionManager::CFactionManager()
{
}


/*
*功能：析构函数
*细节描述： 释放资源。
*参数： 无
*返回值： 无
*修改时间：2008-11-5
*/
CFactionManager::~CFactionManager()
{
	this->ClearFactions();
}


/*
*功能：清空家族函数
*细节描述： 释放资源。
*参数： 无
*返回值： 无
*修改时间：2008-11-5
*/
void	CFactionManager::ClearFactions()
{
	for( std::list<CFaction*>::iterator p = m_listFactions.begin(); p != m_listFactions.end(); p++ )
	{
		SAFE_DELETE( *p );
	}

	m_listFactions.clear();
}





/* 
*功能：添加一个家族
*细节描述： 
*参数： pstFacInfo
*返回值： 无
*修改时间：2008-11-5
*/
bool	CFactionManager::AddFaction( CFaction::SFacInfo* pstFacInfo )
{
	if( pstFacInfo )
	{
		// 设置家族图标CRC
		pstFacInfo->dwIconCRC = GetGame()->GetFactionManager()->GetFacIconCRC( pstFacInfo->guidFac );

		// 检测家族是否已经存在
		if( CFaction* pFaction = this->GetFaction( pstFacInfo->guidFac ) )
		{
			// 更新数据
			pFaction->SetFacInfo( pstFacInfo );
		}
		else
		{
            CFaction * Faction = new CFaction( pstFacInfo );
   			// 添加新家族
			m_listFactions.push_back(Faction);
		}

		return true;
	}

	return false;
}



/* 
*功能：删除一个家族
*细节描述： 
*参数： guid
*返回值： 无
*修改时间：2008-11-5
*/
bool	CFactionManager::DelFaction( CGUID guid )
{
	// 遍历处理场景内所有的玩家
	std::list<CBaseObject*>*	pListOfObjects = GetGame()->GetRegion()->GetObjectList();
	for( std::list<CBaseObject*>::iterator p = pListOfObjects->begin(); p != pListOfObjects->end(); p++ )
	{
		if( (*p)->GetType() == TYPE_PLAYER )
		{
			if( CPlayer* pPlayer = dynamic_cast<CPlayer*>(*p) )
			{
				// 找到该家族的Player进行处理
				if( pPlayer->GetFactionID() == guid )
				{
					pPlayer->SetFactionID( NULL_GUID );
					pPlayer->SetUnionID( NULL_GUID );
					pPlayer->SetFactionName( string("") );
					pPlayer->SetFactionTitle( string("") );
				}
			}
		}
	}

	// 连带从联盟中删除
	if( CUnion* pUnion = GetGame()->GetUnionManager()->GetFacUnion( guid ) )
	{
		pUnion->DelFaction( guid );
	}

	// 删除家族
	for( std::list<CFaction*>::iterator p = m_listFactions.begin(); p != m_listFactions.end(); p++ )
	{
		if( (*p)->GetGUID()  ==  guid )
		{
			SAFE_DELETE( *p );
			m_listFactions.erase( p );

			return true;
		}
	}

	return false;
}



/* 
*功能：是否拥有一个家族
*细节描述： 
*参数： guid
*返回值： 无
*修改时间：2008-11-5
*/
bool	CFactionManager::HasFaction( CGUID guid )
{
	return this->GetFaction( guid )  !=  NULL;
}



/* 
*功能：获取一个家族
*细节描述： 
*参数： guid
*返回值： 无
*修改时间：2008-11-5
*/
CFaction*	CFactionManager::GetFaction( CGUID guid )
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
*功能：获取家族权限的等级限制
*细节描述： 
*参数： 
*返回值： 无
*修改时间：2008-11-5
*/
long	CFactionManager::GetFacLvlForPV( eFactionPurviewIndex ePVIndex )
{
	return COrganizingParam::getInstance().GetFacPurviewNeedLevel( ePVIndex );
}



/* 
*功能：获取家族等级的 最大人数
*细节描述： 
*参数： 
*返回值： 无
*修改时间：2008-11-5
*/
long	CFactionManager::GetMaxNumByFacLv( long lFacLvl )
{
	if( const tagFacLevelSetup* pFacLevelSetup = COrganizingParam::getInstance().GetLevelSetup( lFacLvl ) )
	{
		return pFacLevelSetup->lMaxMemNum;
	}

	return -1;
}


/* 
*功能：获取家族等级的 最低要求人数
*细节描述： 
*参数： 
*返回值： 无
*修改时间：2009-6-16
*/
long	CFactionManager::GetRequireNumByFacLv( long lFacLvl )
{
	if( const tagFacLevelSetup* pFacLevelSetup = COrganizingParam::getInstance().GetLevelSetup( lFacLvl ) )
	{
		return pFacLevelSetup->lDisbandMemNum;
	}

	return -1;
}


/*
*功能：获取自己的据点数量
*细节描述： 
*参数： 无
*返回值： 无
*修改时间：2009-10-26
*/
long	CFactionManager::GetNumOfMyBeachheads()
{
	long lNumOfMyBeachheads = 0;

	if( CFaction* pMyFaction = this->GetMyFaction() )
	{
		for( std::list<tagBaseInfo>::iterator p = m_listBeachheads.begin(); p != m_listBeachheads.end(); p++ )
		{
			if( pMyFaction->GetGUID() == (*p)._FacGuid )
				lNumOfMyBeachheads++;
		}
	}

	return lNumOfMyBeachheads;
}



/* 
*功能：获取我的家族
*细节描述： 
*参数： 
*返回值： 无
*修改时间：2008-11-5
*/
CFaction*	CFactionManager::GetMyFaction()
{
	return this->GetFaction( GetGame()->GetMainPlayer()->GetFactionID() );
}



/* 
*功能：获取我的会友
*细节描述： 
*参数： guid
*返回值： 无
*修改时间：2008-11-5
*/
CFacMember*	CFactionManager::GetMyConfrere( CGUID guid )
{
	if( CFaction* pMyFaction = this->GetMyFaction() )
	{
		return pMyFaction->GetMember( guid );
	}

	return NULL;
}


/* 
*功能：获取我的会友
*细节描述： 
*参数： guid
*返回值： 无
*修改时间：2008-11-5
*/
CFacMember*	CFactionManager::GetMyConfrere( LPCTSTR szName )
{
	if( CFaction* pMyFaction = this->GetMyFaction() )
	{
		return pMyFaction->GetMember( szName );
	}

	return NULL;
}


/* 
*功能：添加我的会友
*细节描述： 
*参数： pstMemberInfo
*返回值： 无
*修改时间：2008-11-5
*/
bool	CFactionManager::AddMyConfrere( CFacMember::SMemberInfo* pstMemberInfo )
{
	if( CFaction* pMyFaction = this->GetMyFaction() )
	{
		return pMyFaction->AddMember( pstMemberInfo );
	}

	return false;
}



/* 
*功能：获取我的官阶
*细节描述： 
*参数： 
*返回值： 无
*修改时间：2008-11-5
*/
long	CFactionManager::GetMyJob()
{
	return GetGame()->GetMainPlayer()->GetFactionJob();
}



/* 
*功能：设置我的官阶
*细节描述： 
*参数： lJob
*返回值： 无
*修改时间：2008-11-5
*/
bool	CFactionManager::SetMyJob( long lJob )
{
	if( lJob >= -1  &&  lJob <= MAXNUM_JobLvl )
		GetGame()->GetMainPlayer()->SetFactionJob( lJob );
	else
		GetGame()->GetMainPlayer()->SetFactionJob( 0 );

	return true;
}



/* 
*功能：获取我的家族公告
*细节描述： 
*参数： 
*返回值： 无
*修改时间：2008-11-5
*/
LPCTSTR	CFactionManager::GetMyPlacard()
{
	if( CFaction* pMyFaction = this->GetMyFaction() )
	{
		return pMyFaction->GetPlacard();
	}

	return "";
}



/* 
*功能：设置我的家族公告
*细节描述： 
*参数： szPlacard
*返回值： 无
*修改时间：2008-11-5
*/
bool	CFactionManager::SetMyPlacard( LPCTSTR szPlacard )
{
	if( CFaction* pMyFaction = this->GetMyFaction() )
	{
		pMyFaction->SetPlacard( szPlacard );
		return true;
	}

	return false;
}



/* 
*功能：获取我的家族 备忘录
*细节描述： 
*参数： 
*返回值： 无
*修改时间：2008-11-5
*/
LPCTSTR	CFactionManager::GetMyMemo()
{
	if( CFaction* pMyFaction = this->GetMyFaction() )
	{
		return pMyFaction->GetMemo();
	}

	return "";
}



/* 
*功能：设置我的家族 备忘录
*细节描述： 
*参数： szMemo
*返回值： 无
*修改时间：2008-11-5
*/
bool	CFactionManager::SetMyMemo( LPCTSTR szMemo )
{
	if( CFaction* pMyFaction = this->GetMyFaction() )
	{
		pMyFaction->SetMemo( szMemo );
		return true;
	}

	return false;
}



/* 
*功能：获取家族徽标路径
*细节描述： 
*参数： guidFac
*返回值： 无
*修改时间：2008-11-5
*/
string	CFactionManager::GetFacIconPath( CGUID guidFac )
{
	// 徽标文件名
	TCHAR	szFacGUID[ 128 ];
	guidFac.tostring( szFacGUID );

	string strFacIconFile = "setup\\FactionIcon\\";
	strFacIconFile += szFacGUID;
	strFacIconFile += ".jpg";

	return strFacIconFile;
}



/* 
*功能：获取家族徽标CRC
*细节描述： 
*参数： guidFac
*返回值： 无
*修改时间：2008-11-5
*/
DWORD	CFactionManager::GetFacIconCRC( CGUID guidFac )
{
	//
	DWORD dwCRC = 0;

	// 徽标文件名
	string strFacIconFile = this->GetFacIconPath( guidFac );

	// 读取文件
	ifstream fFacIcon( strFacIconFile.c_str(), ios::in | ios::binary );
	if( fFacIcon.is_open() )
	{
		fFacIcon.seekg( 0, ios::end );
		DWORD dwIconSize = fFacIcon.tellg();

		if( LPBYTE pIconData = new BYTE[ dwIconSize ] )
		{
			fFacIcon.seekg( 0, ios::beg );
			fFacIcon.read( (LPSTR)pIconData, dwIconSize );

			// 计算CRC
			CCrc32Static::DataCrc32( pIconData, dwIconSize, dwCRC );
			SAFE_DELETE_ARRAY( pIconData );
		}
		fFacIcon.close();
	}

	return dwCRC;
}


/* 
*功能：获取家族徽标CRC
*细节描述： 
*参数： guidFac
*		dwNewCRC
*返回值： 
*修改时间：2008-11-5
*/
bool	CFactionManager::CheckFacIconCRC( CGUID guidFac, DWORD dwNewCRC )
{
	if( CFaction*	pFaction = this->GetFaction( guidFac ) )
	{
		return pFaction->GetIconCRC() != dwNewCRC;
	}

	return false;
}



/* 
*功能：获取我的权利
*细节描述： 
*参数： guid
*返回值： bool
*修改时间：2008-11-5
*/
bool	CFactionManager::GetMyRights( eFactionPurviewIndex ePV )
{
	CFaction*	pMyFaction = this->GetMyFaction();
	CPlayer*	pMainPlayer = GetGame()->GetMainPlayer();

	// 是否有家族
	if( pMyFaction && pMainPlayer )
	{
		return pMyFaction->GetJobPurview( this->GetMyJob(), ePV );
	}

	return false;
}



/* 
*功能：我是否族长
*细节描述： 
*参数： 
*返回值： bool
*修改时间：2008-11-5
*/
bool	CFactionManager::AmIChairman()
{
	return this->IsPlayerChairman( GetGame()->GetMainPlayer() );
}



/* 
*功能：玩家是否族长
*细节描述： 
*参数： pPlayer
*返回值： bool
*修改时间：2008-11-5
*/
bool	CFactionManager::IsPlayerChairman( CPlayer* pPlayer )
{
	if( pPlayer )
	{
		if( CFaction* pFaction = this->GetFaction( pPlayer->GetFactionID() ) )
		{
			return pFaction->GetMasterGUID() == pPlayer->GetExID();
		}
	}

	return false;
}



/* 
*功能：我是否加入了家族
*细节描述： 
*参数： 
*返回值： bool
*修改时间：2008-11-5
*/
bool	CFactionManager::AmIJoinedFaction()
{
	return this->IsPlayerJoinedFaction( GetGame()->GetMainPlayer() );
}



/* 
*功能：我是否加入了这个家族
*细节描述： 
*参数： guidFaction
*返回值： bool
*修改时间：2008-11-5
*/
bool	CFactionManager::AmIJoinedFaction( CGUID guidFaction )
{
	return this->IsPlayerJoinedFaction( GetGame()->GetMainPlayer(), guidFaction );
}



/* 
*功能：玩家是否加入了家族
*细节描述： 
*参数： pPlayer
*返回值： bool
*修改时间：2008-11-5
*/
bool	CFactionManager::IsPlayerJoinedFaction( CPlayer* pPlayer )
{
	if( pPlayer )
		return pPlayer->GetFactionID() != NULL_GUID;

	return false;
}



/* 
*功能：玩家是否加入了这个家族
*细节描述： 
*参数： pPlayer
*		guidFaction
*返回值： bool
*修改时间：2008-11-5
*/
bool	CFactionManager::IsPlayerJoinedFaction( CPlayer* pPlayer, CGUID guidFaction )
{
	if( pPlayer  &&  NULL_GUID != guidFaction )
		return pPlayer->GetFactionID() == guidFaction;

	return false;
}




/* 
*功能：检查是否达到 条件可以升级此功能
*细节描述： 
*参数： eFun
*返回值： bool
*修改时间：2008-11-5
*/
bool	CFactionManager::CanUpgradeMyFun( CFaction::ENUM_FUN eFun )
{
	// 检测所有条件
	if( this->GetMyFaction()->GetLevel( eFun ) < this->GetFunMaxLevel( eFun )
		&& this->HasRes_ForUpgradeMyFun( eFun )
		&& this->HasExp_ForUpgradeMyFun( eFun )
		&& this->HasGold_ForUpgradeMyFun( eFun )
		&& this->HasProp_ForUpgradeMyFun( eFun )
		&& this->HasFacLvl_ForUpgradeMyFun( eFun )
		&& this->HasMasterLvl_ForUpgradeMyFun( eFun ) )
	{
		return true;
	}

	return false;
}


/* 
*功能：获取功能的最高等级
*细节描述： 
*参数： eFun
*返回值： bool
*修改时间：2008-11-5
*/
long	CFactionManager::GetFunMaxLevel( CFaction::ENUM_FUN eFun )
{
	return COrganizingParam::getInstance().GetFacFunMaxLevel( eFun );
}



/* 
*功能：检查 是否有足够的 资源 可以升级此功能
*细节描述： 
*参数： eFun
*返回值： bool
*修改时间：2008-11-5
*/
bool	CFactionManager::HasRes_ForUpgradeMyFun( CFaction::ENUM_FUN eFun )
{
	// 是否有家族
	if( CFaction* pMyFaction = this->GetMyFaction() )
	{
		long res = this->NeedRes_ForUpgradeMyFun( eFun );

		// 是否需要
		if( -1 != res )
		{
			// 是否达到要求
			if( pMyFaction->GetRes() >= res )
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return true;
		}
	}

	return false;
}



/* 
*功能：检查 是否有足够的 经验 可以升级此功能
*细节描述： 
*参数： eFun
*返回值： bool
*修改时间：2008-11-5
*/
bool	CFactionManager::HasExp_ForUpgradeMyFun( CFaction::ENUM_FUN eFun )
{
	// 是否有家族
	if( CFaction* pMyFaction = this->GetMyFaction() )
	{
		long exp = this->NeedExp_ForUpgradeMyFun( eFun );

		// 是否需要
		if( -1 != exp )
		{
			// 是否达到要求
			if( pMyFaction->GetExp() >= exp )
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return true;
		}
	}

	return false;
}



/* 
*功能：检查 是否有足够的 金币 可以升级此功能
*细节描述： 
*参数： eFun
*返回值： bool
*修改时间：2008-11-5
*/
bool	CFactionManager::HasGold_ForUpgradeMyFun( CFaction::ENUM_FUN eFun )
{
	// 是否有家族
	if( CFaction* pMyFaction = this->GetMyFaction() )
	{
		long gold = this->NeedGold_ForUpgradeMyFun( eFun );

		// 是否需要
		if( -1 != gold )
		{
			// 是否达到要求
			if( (long)GetGame()->GetMainPlayer()->GetMoney() >= gold )
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return true;
		}
	}

	return false;
}



/* 
*功能：检查 是否有足够的 家族等级 可以升级此功能
*细节描述： 
*参数： eFun
*返回值： bool
*修改时间：2008-11-5
*/
bool	CFactionManager::HasFacLvl_ForUpgradeMyFun( CFaction::ENUM_FUN eFun )
{
	// 是否有家族
	if( CFaction* pMyFaction = this->GetMyFaction() )
	{
		long level = this->NeedFacLvl_ForUpgradeMyFun( eFun );

		// 是否需要
		if( -1 != level )
		{
			// 是否达到要求
			if( pMyFaction->GetLevel() >= level )
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return true;
		}
	}

	return false;
}


/* 
*功能：检查 是否有足够的 族长等级 可以升级此功能
*细节描述： 
*参数： eFun
*返回值： bool
*修改时间：2008-11-5
*/
bool	CFactionManager::HasMasterLvl_ForUpgradeMyFun( CFaction::ENUM_FUN eFun )
{
	// 是否有家族
	if( CFaction* pMyFaction = this->GetMyFaction() )
	{
		long level = this->NeedMasterLvl_ForUpgradeMyFun( eFun );

		// 是否需要
		if( -1 != level )
		{
			// 是否达到要求
			if( GetGame()->GetMainPlayer()->GetLevel() >= level )
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return true;
		}
	}

	return false;
}



/* 
*功能：检查 是否有足够的 道具 可以升级此功能
*细节描述： 
*参数： eFun
*返回值： bool
*修改时间：2008-11-5
*/
bool	CFactionManager::HasProp_ForUpgradeMyFun( CFaction::ENUM_FUN eFun )
{
	// 是否有家族
	if( CFaction* pMyFaction = this->GetMyFaction() )
	{
		long index = this->NeedProp_ForUpgradeMyFun( eFun );

		// 是否需要
		if( -1 != index )
		{
			// 是否达到要求
			if( GetGame()->GetMainPlayer()->GetGoodsAmount( index ) > 0 )
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return true;
		}
	}

	return false;
}


/* 
*功能：检查 是否有足够的 据点 可以升级此功能
*细节描述： 
*参数： eFun
*返回值： bool
*修改时间：2009-10-26
*/
bool	CFactionManager::HasBeachhead_ForUpgradeMyFun( CFaction::ENUM_FUN eFun )
{
	// 是否有家族
	if( CFaction* pMyFaction = this->GetMyFaction() )
	{
		long lBeachhead = this->NeedBeachhead_ForUpgradeMyFun( eFun );

		// 是否需要
		if( -1 != lBeachhead )
		{
			// 是否达到要求
			if( this->GetNumOfMyBeachheads() >= lBeachhead )
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return true;
		}
	}

	return false;
}


/* 
*功能：获取升级此功能的 资源 条件
*细节描述： 
*参数： eFun
*返回值： bool
*修改时间：2008-11-5
*/
long	CFactionManager::NeedRes_ForUpgradeMyFun( CFaction::ENUM_FUN eFun )
{
	// 是否有家族
	if( CFaction* pMyFaction = this->GetMyFaction() )
	{
		VecCondition* pvec = (VecCondition*)COrganizingParam::getInstance().GetUpgradeVecCondition( eFun, pMyFaction->GetLevel( eFun ) +1 );

		if( pvec )
		{
			for( vector<tagUpgradeCondition>::iterator p = pvec->begin(); p != pvec->end(); p++ )
			{
				tagUpgradeCondition* pstUpgradeCondition = &(*p);

				if( eUCT_FacRes == pstUpgradeCondition->lConditionType )
				{
					return pstUpgradeCondition->lValue;
				}
			}
		}
	}

	return -1;
}



/* 
*功能：获取升级此功能的 经验 条件
*细节描述： 
*参数： eFun
*返回值： bool
*修改时间：2008-11-5
*/
long	CFactionManager::NeedExp_ForUpgradeMyFun( CFaction::ENUM_FUN eFun )
{
	// 是否有家族
	if( CFaction* pMyFaction = this->GetMyFaction() )
	{
		VecCondition* pvec = (VecCondition*)COrganizingParam::getInstance().GetUpgradeVecCondition( eFun, pMyFaction->GetLevel( eFun ) +1 );

		if( pvec )
		{
			for( vector<tagUpgradeCondition>::iterator p = pvec->begin(); p != pvec->end(); p++ )
			{
				tagUpgradeCondition* pstUpgradeCondition = &(*p);

				if( eUCT_FacExp == pstUpgradeCondition->lConditionType )
				{
					return pstUpgradeCondition->lValue;
				}
			}
		}
	}

	return -1;
}



/* 
*功能：获取升级此功能的 金币 条件
*细节描述： 
*参数： eFun
*返回值： bool
*修改时间：2008-11-5
*/
long	CFactionManager::NeedGold_ForUpgradeMyFun( CFaction::ENUM_FUN eFun )
{
	// 是否有家族
	if( CFaction* pMyFaction = this->GetMyFaction() )
	{
		VecCondition* pvec = (VecCondition*)COrganizingParam::getInstance().GetUpgradeVecCondition( eFun, pMyFaction->GetLevel( eFun ) +1 );

		if( pvec )
		{
			for( vector<tagUpgradeCondition>::iterator p = pvec->begin(); p != pvec->end(); p++ )
			{
				tagUpgradeCondition* pstUpgradeCondition = &(*p);

				if( eUCT_Money == pstUpgradeCondition->lConditionType )
				{
					return pstUpgradeCondition->lValue;
				}
			}
		}
	}

	return -1;
}


/* 
*功能：获取升级此功能的 家族等级 条件
*细节描述： 
*参数： eFun
*返回值： bool
*修改时间：2008-11-5
*/
long	CFactionManager::NeedFacLvl_ForUpgradeMyFun( CFaction::ENUM_FUN eFun )
{
	// 是否有家族
	if( CFaction* pMyFaction = this->GetMyFaction() )
	{
		VecCondition* pvec = (VecCondition*)COrganizingParam::getInstance().GetUpgradeVecCondition( eFun, pMyFaction->GetLevel( eFun ) +1 );

		if( pvec )
		{
			for( vector<tagUpgradeCondition>::iterator p = pvec->begin(); p != pvec->end(); p++ )
			{
				tagUpgradeCondition* pstUpgradeCondition = &(*p);

				if( eUCT_FacLevel == pstUpgradeCondition->lConditionType )
				{
					return pstUpgradeCondition->lValue;
				}
			}
		}
	}

	return -1;
}



/* 
*功能：获取升级此功能的 族长等级 条件
*细节描述： 
*参数： eFun
*返回值： bool
*修改时间：2008-11-5
*/
long	CFactionManager::NeedMasterLvl_ForUpgradeMyFun( CFaction::ENUM_FUN eFun )
{
	// 是否有家族
	if( CFaction* pMyFaction = this->GetMyFaction() )
	{
		VecCondition* pvec = (VecCondition*)COrganizingParam::getInstance().GetUpgradeVecCondition( eFun, pMyFaction->GetLevel( eFun ) +1 );

		if( pvec )
		{
			for( vector<tagUpgradeCondition>::iterator p = pvec->begin(); p != pvec->end(); p++ )
			{
				tagUpgradeCondition* pstUpgradeCondition = &(*p);

				if( eUCT_LeaderLevel == pstUpgradeCondition->lConditionType )
				{
					return pstUpgradeCondition->lValue;
				}
			}
		}
	}

	return -1;
}


/* 
*功能：获取升级此功能的 道具 条件
*细节描述： 
*参数： eFun
*返回值： bool
*修改时间：2008-11-5
*/
long	CFactionManager::NeedProp_ForUpgradeMyFun( CFaction::ENUM_FUN eFun )
{
	// 是否有家族
	if( CFaction* pMyFaction = this->GetMyFaction() )
	{
		VecCondition* pvec = (VecCondition*)COrganizingParam::getInstance().GetUpgradeVecCondition( eFun, pMyFaction->GetLevel( eFun ) +1 );

		if( pvec )
		{
			for( vector<tagUpgradeCondition>::iterator p = pvec->begin(); p != pvec->end(); p++ )
			{
				tagUpgradeCondition* pstUpgradeCondition = &(*p);

				if( eUCT_Goods == pstUpgradeCondition->lConditionType )
				{
					return pstUpgradeCondition->lValue;
				}
			}
		}
	}

	return -1;
}



/* 
*功能：获取升级此功能的 据点 条件
*细节描述： 
*参数： eFun
*返回值： bool
*修改时间：2009-10-26
*/
long	CFactionManager::NeedBeachhead_ForUpgradeMyFun( CFaction::ENUM_FUN eFun )
{
	// 是否有家族
	if( CFaction* pMyFaction = this->GetMyFaction() )
	{
		VecCondition* pvec = (VecCondition*)COrganizingParam::getInstance().GetUpgradeVecCondition( eFun, pMyFaction->GetLevel( eFun ) +1 );

		if( pvec )
		{
			for( vector<tagUpgradeCondition>::iterator p = pvec->begin(); p != pvec->end(); p++ )
			{
				tagUpgradeCondition* pstUpgradeCondition = &(*p);

				if( eUCT_BaseNum == pstUpgradeCondition->lConditionType )
				{
					return pstUpgradeCondition->lValue;
				}
			}
		}
	}

	return -1;
}


/*
*功能： 接收 初始化家族协议
*细节描述： 
*参数： pMsg	协议结构
*返回值： 无
*修改时间：2008-4-16
*/
void	CFactionManager::RecvInitMsg( CMessage* pMsg )
{
	// 整个消息 数据块大小
	long lDataSize = pMsg->GetLong();

	LPBYTE	pData = new BYTE[ lDataSize ];
	pMsg->GetEx( pData, lDataSize );

	//
	long lPos = 0;

	//! 家族全局配置
	COrganizingParam::getInstance().DecodeFromByteArray( pData, lPos );

	//// 获取ICON校验码
	//long crcNew = _GetLongFromByteArray( pData, lPos );

	//// 比较校验码是否需要更新ICON
	//long crcOld = GetPrivateProfileInt( "DefaultGameConfig", "CRC", 0, "setup/GameConfig.ini" );
	//if( crcNew != crcOld )
	//{
	//	// 发送请求更新所有ICON
	//	this->SendUpdateAllFacIconMsg();
	//}

	// 释放消息数据块
	SAFE_DELETE_ARRAY( pData );
}

/*
*功能： 接收 一个家族的公共数据通知
*细节描述： 
*参数： pMsg	协议结构
*返回值： 无
*修改时间：2008-4-16
*/
void	CFactionManager::RecvFacPublicDataMsg( CMessage* pMsg )
{
	// 取出 一条家族公共数据
	tagFacPublicData stFacPublicData;
	pMsg->GetEx( &stFacPublicData, sizeof(stFacPublicData ) );

	// 已经创建则更新数据
	if( CFaction* pFaction = this->GetFaction( stFacPublicData.guid ) )
	{
		pFaction->SetName( stFacPublicData.szName );
		pFaction->SetLevel( stFacPublicData.lLevel );
	}
	// 没有创建则创建
	else
	{
		CFaction::SFacInfo	stFacInfo;
		ZeroMemory( &stFacInfo, sizeof(stFacInfo) );
		stFacInfo.guidFac = stFacPublicData.guid;
		stFacInfo.strFacName = stFacPublicData.szName;
		stFacInfo.arrLevel[ CFaction::FUN_FACTION ] = stFacPublicData.lLevel;

		this->AddFaction( &stFacInfo );
	}
}




/*
*功能： 发送 家族基本资料请求协议
*细节描述： 
*参数： 无
*返回值： 无
*修改时间：2008-4-16
*/
void	CFactionManager::SendBaseInfoRequestMsg()
{
	CMessage msg( MSG_C2S_FacData_Request );

	msg.Send();
}



/*
*功能： 接收 家族基本资料请求协议
*细节描述： 
*参数： pMsg	协议结构
*返回值： 无
*修改时间：2008-4-16
*/
void	CFactionManager::RecvBaseInfoMsg( CMessage* pMsg )
{
	// 保存家族基本信息，目前只有自己的家族信息
	tagFacBaseData	stFacBaseData;
	pMsg->GetEx( &stFacBaseData, sizeof(stFacBaseData) );

	//
	if( CFaction* pMyFaction = this->GetMyFaction() )
	{
		pMyFaction->SetUnionGUID( stFacBaseData.SuperiorGuid );
		pMyFaction->SetMasterGUID( stFacBaseData.MasterGuid );
		pMyFaction->SetMasterName( stFacBaseData.szMasterName );
		pMyFaction->SetCountry( stFacBaseData.lCountryID );

		pMyFaction->SetRecruiting( stFacBaseData.bIsRecruiting != 0 );
		pMyFaction->SetTrade( stFacBaseData.bOpenResWar != 0 );

		pMyFaction->SetRes( stFacBaseData.lCurrRes );
		pMyFaction->SetExp( stFacBaseData.lCurrExp );
		pMyFaction->SetWarFund( stFacBaseData.lFacArmyInvest );
		pMyFaction->SetAchievement( stFacBaseData.lFacBattle );

		pMyFaction->SetLevel( CFaction::FUN_WELFARE, stFacBaseData.lSubExpLevel );
		pMyFaction->SetLevel( CFaction::FUN_STORAGE, stFacBaseData.lSpecialityLevel );
		pMyFaction->SetLevel( CFaction::FUN_SMELT, stFacBaseData.lBattleLevel );
		pMyFaction->SetLevel( CFaction::FUN_FORGING, stFacBaseData.lBussinessLevel );
		pMyFaction->SetLevel( CFaction::FUN_SEWING, stFacBaseData.lNoblelevel );

		// 设置官阶名字和权限
		for( long c = 1; c <= MAXNUM_JobLvl; c++ )
		{
			pMyFaction->SetJobName( c, stFacBaseData.arr_szJobName[ c-1 ] );
			pMyFaction->SetJobPurview( c, stFacBaseData.arrJobPurview[ c-1 ] );
		}
	}

	// 刷新 基本资料
	//if( CFactionPageEx*	pFactionPageEx = GetGame()->GetCGuiEx()->GetFactionPageEx() )
	//{
	//	pFactionPageEx->RefreshAll();
	//}
}




/*
*功能： 发送 所有家族图标数据请求
*细节描述： 
*参数： 无
*返回值： 无
*修改时间：2008-4-16
*/
void	CFactionManager::SendUpdateAllFacIconMsg()
{
		CMessage msg( MSG_C2S_Faction_AllIconData_Request );
		msg.Send();
}


/*
*功能： 发送 单个家族图标数据请求
*细节描述： 
*参数： 无
*返回值： 无
*修改时间：2008-4-16
*/
void	CFactionManager::SendUpdateFacIconMsg( CGUID guidFac )
{
	CMessage msg( MSG_C2S_Faction_IconData_Request );

		msg.Add( guidFac );

	msg.Send();
}


/*
*功能： 接收 所有家族图标数据更新
*细节描述： 
*参数： pMsg	协议结构
*返回值： 无
*修改时间：2008-4-16
*/
void	CFactionManager::RecvUpdateAllFacIconMsg( CMessage* pMsg )
{
	// 更新校验码，long
	long lCRC = pMsg->GetLong();

	TCHAR	szCRC[ 128 ];
	wsprintf( szCRC, "%d", lCRC );
	WritePrivateProfileString( "DefaultGameConfig", "CRC", szCRC, "setup/GameConfig.ini" );

	// 整个消息 数据块大小
	long lPos = 0;
	long lDataSize = pMsg->GetLong();

	LPBYTE	pData = new BYTE[ lDataSize ];
	pMsg->GetEx( pData, lDataSize );

	//
	long lIconNum = _GetLongFromByteArray( pData, lPos );

	for ( long i = 0; i < lIconNum; i++ )
	{
		CGUID IconGuid;
		_GetBufferFromByteArray( pData, lPos, IconGuid );

		if( long lIconSize = _GetLongFromByteArray( pData, lPos ) )
		{
			LPBYTE	pIconData = new BYTE[ lIconSize ];
			_GetBufferFromByteArray( pData, lPos, pIconData, lIconSize );

			// 存储到硬盘，转换成图片，以家族GUID为文件名
			TCHAR	szGUID[ 128 ];
			IconGuid.tostring( szGUID );
			TCHAR	szIconFileName[ MAX_PATH ];
			wsprintf( szIconFileName, "setup/FactionIcon/%s.jpg", szGUID );

			std::ofstream	fFactionIcon( szIconFileName, std::ios::out | std::ios::trunc | ios::binary );
			if( fFactionIcon.is_open() )
			{
				fFactionIcon.write( (char*)pIconData, lIconSize );
				fFactionIcon.close();
			}

			// 更新内存资源
			CBitmapX::GetBitmapX( szIconFileName, true );

			// 释放资源
			SAFE_DELETE_ARRAY( pIconData );
		}
	}

	// 释放资源
	SAFE_DELETE_ARRAY( pData );
}


/*
*功能： 接收 一个家族的图标数据更新通知
*细节描述： 
*参数： pMsg	协议结构
*返回值： 无
*修改时间：2008-4-16
*/
void	CFactionManager::RecvUpdateFacIconNotifyMsg( CMessage* pMsg )
{
	//// 更新校验码，long
	//long lCRC = pMsg->GetLong();

	//TCHAR	szCRC[ 128 ];
	//wsprintf( szCRC, "%d", lCRC );
	//WritePrivateProfileString( "DefaultGameConfig", "CRC", szCRC, "setup/GameConfig.ini" );

	// 家族GUID
	CGUID	guidFac;
	pMsg->GetGUID( guidFac );

	// 图标大小
	long lSize = pMsg->GetLong();

	// 图标数据
	LPBYTE	pData = new BYTE[ lSize ];
	pMsg->GetEx( pData, lSize );

	// 存储到硬盘，转换成图片，以家族GUID为文件名
	string strFacIconFile = this->GetFacIconPath( guidFac );

	std::ofstream	fFactionIcon( strFacIconFile.c_str(), std::ios::out | std::ios::trunc | std::ios::binary );
	if( fFactionIcon.is_open() )
	{
		fFactionIcon.write( (char*)pData, lSize );
		fFactionIcon.close();
	}

	// 更新内存资源
	CBitmapX::GetBitmapX( strFacIconFile, true );

	// 更新CRC
	if( CFaction*	pFaction = this->GetFaction( guidFac ) )
	{
		pFaction->SetIconCRC( this->GetFacIconCRC( guidFac ) );
	}

	// 释放资源
	SAFE_DELETE_ARRAY( pData );

	// 刷新 图标资料
	//if( CFactionPageEx*	pFactionPageEx = GetGame()->GetCGuiEx()->GetFactionPageEx() )
	//{
	//	pFactionPageEx->RefreshLogo();
	//}
}







/*
*功能： 接收 族员资料请求协议
*细节描述： 
*参数： pMsg	协议结构
*返回值： 无
*修改时间：2008-4-16
*/
void	CFactionManager::RecvMemberInfoMsg( CMessage* pMsg )
{
	// 族员人数
	long num = pMsg->GetLong();

	// 获取所有族员信息
	if( CFaction* pMyFaction = this->GetMyFaction() )
	{
		// 先清除之前的数据
		pMyFaction->ClearMembers();

		// 循环获取所有族员
		for( long c = 0; c < num; c++ )
		{
			tagFacMemInfo	stFacMemInfo;
			pMsg->GetEx( &stFacMemInfo, sizeof(tagFacMemInfo) );

			// 进行添加
			CFacMember::SMemberInfo	stMemberInfo;
			stMemberInfo.guidFac = GetGame()->GetMainPlayer()->GetFactionID();
			stMemberInfo.guidMember = stFacMemInfo.MemberGuid;
			stMemberInfo.strName = stFacMemInfo.szName;
			stMemberInfo.strTitle = stFacMemInfo.szTitle;
			stMemberInfo.lLevel = stFacMemInfo.lLvl;
			stMemberInfo.lJob = stFacMemInfo.lJobLvl;
			stMemberInfo.lOccupation = stFacMemInfo.lOccu;
			stMemberInfo.lContribute = stFacMemInfo.lContribute;
			stMemberInfo.bOnline = stFacMemInfo.bIsOnline != 0;
			stMemberInfo.strRegion = stFacMemInfo.szRegion;
			stMemberInfo.lAchievement = stFacMemInfo.lBattle;
			stMemberInfo.lWarFund = stFacMemInfo.lArmyInvest;
			stMemberInfo.bWar = stFacMemInfo.bJoinBattle ? true : false;
            stMemberInfo.LastOnlineTime = stFacMemInfo.LastOnlineTime;

			pMyFaction->AddMember( &stMemberInfo );
		}
// 王峰：方便调试，在 Debug 状态下生成模拟族员
//#ifdef _DEBUG
//		for( long c = 0; c < 50; c++ )
//		{
//			CFacMember::SMemberInfo	stMemberInfo;
//			stMemberInfo.guidFac = GetGame()->GetMainPlayer()->GetFactionID();
//			CGUID::CreateGUID( stMemberInfo.guidMember );
//			stMemberInfo.strName = CStringReading::LoadText( 208, 1+rand()%220 );
//			stMemberInfo.strTitle = CStringReading::LoadText( 208, 1+rand()%220 );
//			stMemberInfo.lLevel = 1+ rand()%200;
//			stMemberInfo.lJob = rand()%MAXNUM_JobLvl;
//			stMemberInfo.lOccupation = rand()%OCC_Max;
//			stMemberInfo.lContribute = rand()%100000;
//			stMemberInfo.bOnline = rand()%2 != 0;
//			stMemberInfo.strRegion = CCountryList::GetCountry( 1+rand()%6 )->strName.c_str();
//
//			stMemberInfo.lAchievement = rand()%10000;
//			stMemberInfo.lWarFund = rand()%10000;
//			stMemberInfo.bWar = rand()%2 ? true : false;
//
//			pMyFaction->AddMember( &stMemberInfo );
//		}
//#endif

		// 打开 族员面板
		//if( CFactionPageEx*	pFactionPageEx = GetGame()->GetCGuiEx()->GetFactionPageEx() )
		//{
		//	pFactionPageEx->RefreshMemberDlg();
		//}
        FireUIEvent("FactionPage","UpdateMem");
	}
}




/*
*功能： 接收 申请者资料请求协议
*细节描述： 
*参数： pMsg	协议结构
*返回值： 无
*修改时间：2008-4-16
*/
void	CFactionManager::RecvRequisitionInfoMsg( CMessage* pMsg )
{
	// 申请者人数
	long num = pMsg->GetLong();

	if( CFaction* pMyFaction = this->GetMyFaction() )
	{
		// 先清除之前的数据
		pMyFaction->ClearApply();

		// 获取所有申请者
		for( long c = 0; c < num; c++ )
		{
			tagFacApplyPerson	stFacApplyPerson;
			pMsg->GetEx( &stFacApplyPerson, sizeof(tagFacApplyPerson) );

			// 进行添加
			CFacMember::SMemberInfo	stMemberInfo;
			stMemberInfo.guidFac = GetGame()->GetMainPlayer()->GetFactionID();
			stMemberInfo.guidMember = stFacApplyPerson.PlayerGuid;
			stMemberInfo.strName = stFacApplyPerson.szName;
			stMemberInfo.lLevel = stFacApplyPerson.lLvl;
			stMemberInfo.lOccupation = stFacApplyPerson.lOccu;

			pMyFaction->AddApply( &stMemberInfo );
		}

		// 刷新 申请面板
		//if( CFactionPageEx*	pFactionPageEx = GetGame()->GetCGuiEx()->GetFactionPageEx() )
		//{
		//	pFactionPageEx->RefreshRequisitionDlg();
		//}
	}
}




/*
*功能： 接收 据点信息
*细节描述： 
*参数： pMsg	协议结构
*返回值： 无
*修改时间：2009-10-26
*/
void	CFactionManager::RecvBeachheadInfoMsg( CMessage* pMsg )
{
	m_listBeachheads.clear();

	long	lNumOfBeachheads = pMsg->GetLong();

	for( long c = 0; c < lNumOfBeachheads; c++ )
	{
		tagBaseInfo	stBeachheadInfo;
		pMsg->GetEx( &stBeachheadInfo, sizeof(stBeachheadInfo) );

		m_listBeachheads.push_back( stBeachheadInfo );
	}

	// 刷新 阿尔托战报
	//if( CFactionPageEx*	pFactionPageEx = GetGame()->GetCGuiEx()->GetFactionPageEx() )
	//{
	//	pFactionPageEx->RefreshBeachheadDlg();
	//}
}



/*
*功能： 接收 创建家族的脚本事件协议
*细节描述： 
*参数： pMsg	协议结构
*返回值： 无
*修改时间：2008-4-16
*/
void	CFactionManager::RecvCreateNotifyMsg( CMessage* pMsg )
{
	// 检测 成功失败
	if( PLAYER_OPT_SUCCEED == pMsg->GetLong() )
	{
		// 打开家族创建界面
		//if( CFactionPageEx*	pFactionPageEx = GetGame()->GetCGuiEx()->GetFactionPageEx() )
		//{
		//	pFactionPageEx->OpenCreationUI();
		//}
	}
	else
	{
		// 处理错误
		this->HandleErrorMsg( pMsg );
	}
}



/*
*功能： 发送 创建家族协议
*细节描述： 
*参数： strFactionName	家族名称
*返回值： 无
*修改时间：2008-4-16
*/
void	CFactionManager::SendCreateMsg( LPCTSTR strFactionName ,LPCTSTR Placard)
{
	// 创建家族协议
	CMessage msg( MSG_C2S_ORGSYS_CreateFaction_Reques );

		// 家族名称
		msg.Add( strFactionName );
        msg.Add( Placard );

	msg.Send();
}




/*
*功能： 发送 解散家族协议
*细节描述： 
*参数： strFactionName	家族名称
*返回值： 无
*修改时间：2008-4-16
*/
void	CFactionManager::SendDisbandMsg()
{
	// 解散家族协议
	CMessage msg( MSG_C2S_ORGSYS_DisbandFaction_Request );

	msg.Send();
}



/*
*功能： 发送 升级家族功能协议
*细节描述： 
*参数： eFun
*返回值： 无
*修改时间：2008-4-16
*/
void	CFactionManager::SendUpgradeMsg( CFaction::ENUM_FUN eFun )
{
	// 升级家族功能协议
	CMessage msg( MSG_C2S_FACTION_Upgrade_Request );

		msg.Add( (DWORD)eFun );

	msg.Send();
}



/*
*功能： 发送 退出家族协议
*细节描述： 
*参数： 无
*返回值： 无
*修改时间：2008-4-16
*/
void	CFactionManager::SendQuitMsg()
{
	// 退出家族协议
	CMessage msg( MSG_C2S_ORGSYS_Quit_Request );

	msg.Send();
}




/*
*功能： 发送 踢出家族协议
*细节描述： 
*参数： guidMember	被踢出的族员GUID
*返回值： 无
*修改时间：2008-4-16
*/
void	CFactionManager::SendKickMsg( CGUID guidMember )
{
	// 踢出家族协议
	CMessage msg( MSG_C2S_ORGSYS_Kick_Request );

		msg.Add( guidMember );

	msg.Send();
}




/*
*功能： 发送 修改公告请求 协议
*细节描述： 
*参数： szPlacard	更新的内容
*返回值： 无
*修改时间：2008-4-16
*/
void	CFactionManager::SendSetPlacardMsg( LPCTSTR szPlacard )
{
	CMessage msg( MSG_C2S_ORGSYS_SetPlacard_Request );

		msg.Add( szPlacard );

	msg.Send();
}



/*
*功能： 发送 修改备忘录请求 协议
*细节描述： 
*参数： szPlacard	更新的内容
*返回值： 无
*修改时间：2008-4-16
*/
void	CFactionManager::SendSetMemoMsg( LPCTSTR szMemo )
{
	CMessage msg( MSG_C2S_ORGSYS_SetLeaveword_Request );

		msg.Add( szMemo );

	msg.Send();
}




/*
*功能： 接收 公告资料请求协议
*细节描述： 
*参数： pMsg	协议结构
*返回值： 无
*修改时间：2008-4-16
*/
void	CFactionManager::RecvPlacardInfoMsg( CMessage* pMsg )
{
	tagOrgPronounceWord	stOrgPronounceWord;
	pMsg->GetEx( &stOrgPronounceWord, sizeof(stOrgPronounceWord) );

	// 脏话过滤
	string	strPlacard = stOrgPronounceWord.szBody;
	CClientWordsFilter::GetInstance()->Check( strPlacard, true, true );

	// 保存公告
	this->SetMyPlacard( strPlacard.c_str() );

	// 刷新 公告
	//if( CFactionPageEx*	pFactionPageEx = GetGame()->GetCGuiEx()->GetFactionPageEx() )
	//{
	//	pFactionPageEx->RefreshPlacard();
	//}
}



/*
*功能： 接收 备忘录资料请求协议
*细节描述： 
*参数： pMsg	协议结构
*返回值： 无
*修改时间：2008-4-16
*/
void	CFactionManager::RecvMemoInfoMsg( CMessage* pMsg )
{
	tagOrgPronounceWord	stOrgPronounceWord;
	pMsg->GetEx( &stOrgPronounceWord, sizeof(stOrgPronounceWord) );

	// 脏话过滤
	string	strMemo = stOrgPronounceWord.szBody;
	CClientWordsFilter::GetInstance()->Check( strMemo, true, true );

	// 保存备忘录
	this->SetMyMemo( strMemo.c_str() );

	// 刷新 备忘录
	//if( CFactionPageEx*	pFactionPageEx = GetGame()->GetCGuiEx()->GetFactionPageEx() )
	//{
	//	pFactionPageEx->RefreshMemo();
	//}
}


/*
*功能： 接收 战争信息协议
*细节描述： 
*参数： pMsg	协议结构
*返回值： 无
*修改时间：2009-10-21
*/
void	CFactionManager::RecvWarInfoMsg( CMessage* pMsg )
{
	if( CFaction* pMyFaction = this->GetMyFaction() )
	{
		long lNumOfWarType = pMsg->GetLong();

		for( long c = 0; c < lNumOfWarType; c++ )
		{
			BYTE byBT = pMsg->GetByte();
			long lNumOfEnemys = pMsg->GetLong();

			for( long n = 0; n < lNumOfEnemys; n++ )
			{
				tagEnemy stEnemy;
				pMsg->GetEx( &stEnemy, sizeof(stEnemy) );

				pMyFaction->SetEnemy( ( eBattleType )byBT, &stEnemy );
			}
		}

		// 刷新 战争信息
		//if( CFactionPageEx*	pFactionPageEx = GetGame()->GetCGuiEx()->GetFactionPageEx() )
		//{
		//	pFactionPageEx->RefreshWarInfoDlg();
		//}
	}
}


/*
*功能： 接收 族员功勋
*细节描述： 
*参数： pMsg	协议结构
*返回值： 无
*修改时间：2009-10-23
*/
void	CFactionManager::RecvMemberAchievementMsg( CMessage* pMsg )
{
	CGUID guid;
	pMsg->GetGUID( guid );

	long lAchievement = pMsg->GetLong();

	if( CFacMember* pMember =  this->GetMyConfrere( guid ) )
	{
		pMember->SetAchievement( lAchievement );

		// 刷新 参战列表
		//if( CFactionPageEx*	pFactionPageEx = GetGame()->GetCGuiEx()->GetFactionPageEx() )
		//{
		//	pFactionPageEx->RefreshWarListDlg();
		//}
	}
}


/*
*功能： 接收 族员微尘
*细节描述： 
*参数： pMsg	协议结构
*返回值： 无
*修改时间：2009-10-23
*/
void	CFactionManager::RecvMemberWarFundMsg( CMessage* pMsg )
{
	CGUID guid;
	pMsg->GetGUID( guid );

	long lWarFund = pMsg->GetLong();

	if( CFacMember* pMember =  this->GetMyConfrere( guid ) )
	{
		pMember->SetWarFund( lWarFund );

		// 刷新 参战列表
		//if( CFactionPageEx*	pFactionPageEx = GetGame()->GetCGuiEx()->GetFactionPageEx() )
		//{
		//	pFactionPageEx->RefreshWarListDlg();
		//}
	}
}




/*
*功能： 发送 修改官阶名字请求 协议
*细节描述： 
*参数： idJob		职业ID
*		szJobName	职业名字
*返回值： 无
*修改时间：2008-4-16
*/
void	CFactionManager::SendSetJobNameMsg( long idJob, LPCTSTR szJobName )
{
	CMessage msg( MSG_C2S_ORGSYS_SetJobName_Request );

		msg.Add( idJob );
		msg.Add( szJobName );

	msg.Send();
}




/*
*功能： 发送 修改官阶权限请求 协议
*细节描述： 
*参数： idJob
*		lPurview
*返回值： 无
*修改时间：2008-4-16
*/
void	CFactionManager::SendSetJobPurviewMsg( long idJob, long lPurview )
{
	CMessage msg( MSG_C2S_ORGSYS_SetJobPurview_Request );

		msg.Add( idJob );
		msg.Add( lPurview );

	msg.Send();
}




/*
*功能： 发送 设置成员官阶请求 协议
*细节描述： 
*参数： 无
*返回值： 无
*修改时间：2008-4-16
*/
void	CFactionManager::SendSetMemberJobMsg( CGUID guidMember, long idJob )
{
	CMessage msg( MSG_C2S_ORGSYS_SetMemberJob_Request );

		msg.Add( guidMember );
		msg.Add( idJob );

	msg.Send();
}




/*
*功能： 发送 审批入会请求 协议
*细节描述： 
*参数： guid
*返回值： 无
*修改时间：2008-4-16
*/
void	CFactionManager::SendCheckRequisitionMsg( long bAgree, CGUID guid )
{
	CMessage msg( MSG_C2S_ORGSYS_CheckRequisition_Request );

		msg.Add( bAgree );
		msg.Add( guid );

	msg.Send();
}




/*
*功能： 群发 审批入会请求 协议
*细节描述： 
*参数： bAgree
*返回值： 无
*修改时间：2008-4-16
*/
void	CFactionManager::SendCheckRequisitionMsg( long bAgree )
{
	// 
	if( CFaction* pMyFaction = this->GetMyFaction() )
	{
		// 遍历申请者链表
		std::list<CFacMember*>*	plistApply = pMyFaction->GetListOfApply();

		for( std::list<CFacMember*>::iterator p = plistApply->begin(); p != plistApply->end(); p++ )
		{
			// 查找选中的申请者
			if( CFacMember* pApply = *p )
			{
				if( pApply->GetSelected() )
				{
					this->SendCheckRequisitionMsg( bAgree, pApply->GetGUID() );
				}
			}
		}
	}
}



/*
*功能： 发送 邀请入会请求 协议
*细节描述： 
*参数： szInvitePlayerName
*返回值： 无
*修改时间：2008-4-16
*/
void	CFactionManager::SendInviteRequestMsg( LPCTSTR szInvitePlayerName )
{
	CMessage msg( MSG_C2S_Faction_InvitePlayer_Request );

		msg.Add( szInvitePlayerName );

	msg.Send();
}



/*
*功能： 发送 回复入会请求 协议
*细节描述： 
*参数： 无
*返回值： 无
*修改时间：2008-4-16
*/
void	CFactionManager::SendInviteAnswerMsg( CGUID guidInviter, long bAgree )
{
	CMessage msg( MSG_C2S_Faction_InvitePlayer_Answer );

		// long 布尔
		msg.Add( bAgree );
		// 邀请者GUID
		msg.Add( guidInviter );

	msg.Send();
}



/*
*功能： 发送 参战族员 协议
*细节描述： 
*参数： guidMember
*		byWar
*返回值： 无
*修改时间：2009-10-22
*/
void	CFactionManager::SendMemberWarMsg( CGUID guidMember, BYTE byWar )
{
	CMessage msg( MSG_C2S_Faction_ChangeJoinBattle );

		msg.Add( guidMember );
		msg.Add( byWar );

	msg.Send();
}




/*
*功能： 接收 参战族员 协议
*细节描述： 
*参数： 无
*返回值： 无
*修改时间：2009-10-22
*/
void	CFactionManager::RecvMemberWarMsg( CMessage* pMsg )
{
	// 族员GUID
	CGUID	guidMember;
	pMsg->GetGUID( guidMember );

	// 是否参战
	bool bWar = pMsg->GetByte() != 0;

	// 保存设置
	if( CFacMember*	pMember = this->GetMyConfrere( guidMember ) )
	{
		pMember->SetWar( bWar );
	}

	// 刷新参战名单
	//if( CFactionPageEx*	pFactionPageEx = GetGame()->GetCGuiEx()->GetFactionPageEx() )
	//{
	//	pFactionPageEx->RefreshWarDlg();
	//}
}


/*
*功能： 发送 设置称号 协议
*细节描述： 
*参数： 无
*返回值： 无
*修改时间：2008-4-16
*/
void	CFactionManager::SendSetTitleMsg( CGUID guidMember, LPCTSTR szTitle )
{
	CMessage msg( MSG_C2S_Faction_SetMemberTitle_Request );

		msg.Add( guidMember );
		msg.Add( szTitle );

	msg.Send();
}



/*
*功能： 发送 设置族长 协议
*细节描述： 
*参数： 无
*返回值： 无
*修改时间：2008-4-16
*/
void	CFactionManager::SendSetChairmanMsg( CGUID guidMember )
{
	CMessage msg( MSG_C2S_Faction_SetChairman_Request );

		msg.Add( guidMember );

	msg.Send();
}


/*
*功能： 发送 徽标图片
*细节描述： 
*参数： 无
*返回值： 无
*修改时间：2008-4-16
*/
void	CFactionManager::SendUploadFacIconMsg( LPCTSTR strFacIconFileName )
{
	DWORD dwIconSize = 0;
	LPBYTE pIconData = NULL;

	// 
	LPDIRECT3DDEVICE9 pd3dDevice = render::Interface::GetInstance()->GetDevice();

	TCHAR	szFullPath[ MAX_PATH + MAX_PATH ];
	wsprintf( szFullPath, "setup/FactionIcon/%s", strFacIconFileName );

	LPDIRECT3DSURFACE9	pSurf;
	if( SUCCEEDED( pd3dDevice->CreateOffscreenPlainSurface( 24, 16, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &pSurf, NULL ) ) )
	{
		if( SUCCEEDED( D3DXLoadSurfaceFromFile( pSurf, NULL, NULL, szFullPath, NULL, D3DX_FILTER_TRIANGLE, 0, NULL ) ) )
		{
			wsprintf( szFullPath, "setup/FactionIcon/Upload.tmp" );
			if( SUCCEEDED( D3DXSaveSurfaceToFile( szFullPath, D3DXIFF_JPG, pSurf, NULL, NULL ) ) )
			{
				// 
				ifstream fFacIcon( szFullPath, ios::in | ios::binary );
				if( fFacIcon.is_open() )
				{
					fFacIcon.seekg( 0, ios::end );
					dwIconSize = fFacIcon.tellg();
					if( pIconData = new BYTE[ dwIconSize ] )
					{
						fFacIcon.seekg( 0, ios::beg );
						fFacIcon.read( (LPSTR)pIconData, dwIconSize );
					}

					fFacIcon.close();
				}
			}
		}

		SAFE_RELEASE( pSurf );
	}

	// 
	if( pIconData )
	{
		CMessage msg( MSG_C2S_Faction_UpLoadIcon_Request );

			msg.Add( dwIconSize );
			msg.AddEx( pIconData, dwIconSize );

		msg.Send();

		SAFE_DELETE_ARRAY( pIconData );
	}
}




/*
*功能： 处理错误信息
*细节描述： 
*参数： pMsg	协议结构
*返回值： 无
*修改时间：2008-4-16
*/
void	CFactionManager::HandleErrorMsg( CMessage* pMsg )
{
	// 错误码
	long err = pMsg->GetLong();

	DWORD idErrStr = 0xffffffff;

	if( err > eFOEI_NotErr  &&  err < eFOEI_Err_Unknown )
	{
		idErrStr = 10000 + err;
	}
	else
	{
		idErrStr = 10000;
	}
    GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::LoadAppTextRes("FA_", idErrStr) );
}

void   CFactionManager::SendFactionList()
{
     CMessage msg(MSG_C2S_Faction_FindFactionList);
     msg.Send();
}

void   CFactionManager::RecvFactionList(CMessage* pMsg)
{
     m_listApplyInfo.clear();
     if ( PLAYER_OPT_SUCCEED == pMsg->GetLong() )
     {
         CGUID				m_guidFacApply;							// 当前申请的家族GUID
         pMsg->GetGUID( m_guidFacApply );

         long  numFactions = pMsg->GetLong();

         for ( long num = 0 ; num < numFactions ; num ++ )
         {
             tagApplyInfo       stFacInfo;
             char           szPronounce[MAX_PronounceCharNum];
             pMsg->GetEx( &stFacInfo , sizeof(tagApplyInfo) );
             pMsg->GetStr(szPronounce,MAX_PronounceCharNum);

             tagApplyInfoList   tagApplyList(stFacInfo,szPronounce);

             m_listApplyInfo.push_back( tagApplyList );
         }
         FireUIEvent("FactionList","Switch");
     }
     else
     {
         // 处理错误
         HandleErrorMsg( pMsg );
     }
}