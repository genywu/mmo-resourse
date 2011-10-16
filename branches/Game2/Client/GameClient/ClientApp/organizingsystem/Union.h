/*Union.h
*功能：联盟逻辑功能处理
*摘要：对 行会同盟 进行 处理。
*作者：Joe
*修改者：王峰
*修改日期:2008-11-5
*/
#pragma once
#include "Faction.h"
class CFaction;
      
//**************************************************************************************
// 联盟逻辑类
//**************************************************************************************
class CUnion
{
public:
	// 联盟信息结构
	struct	SUnionInfo
	{
		CGUID		guidUnion;							// 联盟 GUID
		CGUID		guidMaster;							// 盟主 GUID
		CGUID		guidMasterFac;						// 家族 GUID
		string		strUnionName;						// 联盟 名字
		string		strMasterName;						// 盟主 名字
		string		strPlacard;							// 联盟 公告
		string		strBeachhead;						// 联盟 据点

		long		lCountryID;							// 国家 ID
		long		lLevel;								// 联盟 等级

		string		arrJobName[ MAXNUM_JobLvl ];		// 官阶 名称
		long		arrJobPurview[ MAXNUM_JobLvl ];		// 官阶 权限

		DWORD		dwIconCRC;							// 图标校验码

		SUnionInfo()	{  }
	};

private:
	SUnionInfo								m_stUnionInfo;				// 联盟基本信息
	std::list<CFaction*>					m_listFactions;				// 家族成员链表

public:
	CUnion();
	virtual ~CUnion();
	CUnion( SUnionInfo* pstUnionInfo );

	// 数据访问函数
	CGUID				GetID() const							{ return m_stUnionInfo.guidUnion;					}
	void				SetID( CGUID id )						{ m_stUnionInfo.guidUnion = id;						}

	LPCTSTR				GetName() const							{ return m_stUnionInfo.strUnionName.c_str();		}
	void				SetName( LPCTSTR szName )				{ m_stUnionInfo.strUnionName = szName;				}

	CGUID				GetMasterID() const						{ return m_stUnionInfo.guidMaster;					}
	void				SetMasterID( CGUID id )					{ m_stUnionInfo.guidMaster = id;					}

	CGUID				GetMasterFacID() const					{ return m_stUnionInfo.guidMasterFac;				}
	void				SetMasterFacID( CGUID id )				{ m_stUnionInfo.guidMasterFac = id;					}

	LPCTSTR				GetMasterName() const					{ return m_stUnionInfo.strMasterName.c_str();		}
	void				SetMasterName( LPCTSTR szName )			{ m_stUnionInfo.strMasterName = szName;				}

	void				SetPlacard( LPCTSTR szPlacard )			{ m_stUnionInfo.strPlacard = szPlacard;				}
	LPCTSTR				GetPlacard() const						{ return m_stUnionInfo.strPlacard.c_str();			}

	void				SetBeachhead( LPCTSTR szBeachhead )		{ m_stUnionInfo.strBeachhead = szBeachhead;			}
	LPCTSTR				GetBeachhead() const					{ return m_stUnionInfo.strBeachhead.c_str();		}

	long				GetCountryID() const					{ return m_stUnionInfo.lCountryID;					}
	void				SetCountryID( long id )					{ m_stUnionInfo.lCountryID = id;					}

	long				GetLevel() const						{ return m_stUnionInfo.lLevel;						}
	void				SetLevel( long lLevel )					{ m_stUnionInfo.lLevel = lLevel;					}


	SUnionInfo*			GetUnionInfo()							{ return &m_stUnionInfo;							}
	list<CFaction*>*	GetListOfFactions()						{ return &m_listFactions;							}
	long				GetNumOfFactions() const				{ return (long)m_listFactions.size();				}
	long				GetMaxNumOfFactions() const				{ return 10;				}

	long				GetNumOfMembers() const;
	long				GetNumOfOnlineMembers() const;

	// 操作函数
	bool				Release();
	bool				SetUnionInfo( SUnionInfo* pstUnionInfo );

	bool				AddFaction( CFaction* pFaction );
	bool				DelFaction( CGUID guid );
	bool				HasFaction( CGUID guid );
	CFaction*			GetFaction( CGUID guid );
	CFaction*			GetFaction( LPCTSTR szName );

	long				GetFacPurview( CGUID guid );
	bool				SetFacPurview( CGUID guid, long lPurview );

	void				SortFactions( CFaction::ENUM_SORT eSort );
};



//**************************************************************************************
// 联盟逻辑管理类
//**************************************************************************************
class CUnionManager
{
	std::map<CGUID,CUnion*>			m_mapUnions;			// 联盟表

public:
	CUnionManager();
	virtual ~CUnionManager();

	void		Release();

	// 检测函数
	bool		AmIChairman();
	bool		IsPlayerChairman( CPlayer* pPlayer );
	bool		IsPlayerChairman( CGUID guidUnion, CGUID guidPlayer );

	bool		AmIJoinedUnion();
	bool		AmIJoinedUnion( CGUID guidUnion );

	bool		IsPlayerJoinedUnion( CPlayer* pPlayer );
	bool		IsPlayerJoinedUnion( CPlayer* pPlayer, CGUID guidUnion );

	// 操作函数
	bool		AddUnion( CUnion::SUnionInfo* pstUnionInfo );
	bool		DelUnion( CGUID guid );
	bool		HasUnion( CGUID guid );
	CUnion*		GetUnion( CGUID guid );

	long		GetMyJob();
	bool		SetMyJob( long lJob );

	LPCTSTR		GetMyPlacard();
	bool		SetMyPlacard( LPCTSTR szPlacard );

	LPCTSTR		GetMyBeachhead();
	bool		SetMyBeachhead( LPCTSTR szBeachhead );

	CUnion*		GetMyUnion();
	CUnion*		GetFacUnion( CGUID guidFaction );
	bool		GetMyRights( eFactionPurviewIndex ePV );

	std::map<CGUID,CUnion*>*		GetListOfUnions()	{ return &m_mapUnions;	}

	// 发送\接收 网络消息
	void		RecvScriptCreationMsg( CMessage* pMsg );
	void		RecvJoinMsg( CMessage* pMsg );
	void		RecvUnionInfoMsg( CMessage* pMsg );
	void		RecvUnionMemberMsg( CMessage* pMsg );

	void		SendCreationMsg( LPCTSTR szName );
	void		RecvCreationMsg( CMessage* pMsg );
	void		RecvCreationNotifyMsg( CMessage* pMsg );

	void		SendDisbandMsg();
	void		RecvDisbandMsg( CMessage* pMsg );

	void		SendQuitMsg();
	void		RecvQuitMsg( CMessage* pMsg );

	void		SendKickMsg( CGUID guidFaction );
	void		RecvKickMsg( CMessage* pMsg );

	void		SendSetChairmanMsg( CGUID guidChairman );
	void		RecvSetChairmanMsg( CMessage* pMsg );

	void		SendSetRightsMsg( CGUID guidFaction );
	void		RecvSetRightsMsg( CMessage* pMsg );
	void		RecvGetRightsMsg( CMessage* pMsg );

	void		SendInviteMsg( LPCTSTR szName );
	void		RecvInviteMsg( CMessage* pMsg );

	void		SendResponseMsg( CGUID guidFaction );
	void		RecvResponseMsg( CMessage* pMsg );
};