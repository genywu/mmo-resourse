/*CFaction.h
*功能：家族的逻辑功能处理
*摘要：对 家族 进行 处理。
*作者：Joe
*修改者：王峰
*修改日期:2008-11-5
*/

#pragma once

//**************************************************************************************
// 家族族员的 逻辑功能类
//**************************************************************************************
#include "../../../../Public/Common/OrganizingDef.h"

class CPlayer;
class CMessage;
class CFacMember
{
public:
	// 排序枚举
	enum	ENUM_SORT
	{
		SORT_BY_NORMAL,				// 通常 排序规则
		SORT_BY_NAME,				// 名字 排序规则
		SORT_BY_JOB,				// 官阶 排序规则
		SORT_BY_TITLE,				// 称号 排序规则
		SORT_BY_LEVEL,				// 等级 排序规则
		SORT_BY_STATE,				// 状态 排序规则
		SORT_BY_REGION,				// 地区 排序规则
		SORT_BY_ONLINE,				// 在线 排序规则
		SORT_BY_OCCUPATION,			// 职业 排序规则
		SORT_BY_CONTRIBUTE,			// 贡献 排序规则
		SORT_BY_SELECTION,			// 选中 排序规则

		MAXNUM_SORT
	};

	// 族员信息结构
	struct	SMemberInfo
	{
		CGUID		guidFac;		// 家族GUID
		CGUID		guidMember;		// 族员GUID
		string		strName;		// 族员名字
		string		strTitle;		// 族员称号
		long		lLevel;			// 族员等级
		long		lJob;			// 族员官阶
		long		lOccupation;	// 族员职业
		long		lContribute;	// 族员贡献
		long		lAchievement;	// 族员功勋
		long		lWarFund;		// 储备微尘
		long		eState;			// 当前状态
        long        LastOnlineTime; // 最后一次在线时间
		bool		bOnline;		// 是否在线
		string		strRegion;		// 所在场景
		bool		bSelected;		// 是否选中
		bool		bWar;			// 是否参战

		SMemberInfo()	
        { 
            //ZeroMemory(this, sizeof(SMemberInfo)); 
        }
	};

private:
	SMemberInfo			m_stMemberInfo;		// 族员信息

public:
	CFacMember(void);
	~CFacMember(void);
	CFacMember( SMemberInfo* pstMemberInfo );

	// 数据访问函数
	void			SetMemberInfo( SMemberInfo* pstMemberInfo )		{ m_stMemberInfo = *pstMemberInfo;				}
	SMemberInfo*	GetMemberInfo()									{ return &m_stMemberInfo;						}
	LPCTSTR			GetOccuText() const;

	void			SetFacGUID( CGUID guidFac )						{ m_stMemberInfo.guidFac = guidFac;				}
	CGUID			GetFacGUID() const								{ return m_stMemberInfo.guidFac;				}

	void			SetGUID( CGUID guid )							{ m_stMemberInfo.guidMember = guid;				}
	CGUID			GetGUID() const									{ return m_stMemberInfo.guidMember;				}

	void			SetName( LPCTSTR szName )						{ m_stMemberInfo.strName = szName;				}
	LPCTSTR			GetName() const									{ return m_stMemberInfo.strName.c_str();		}

	void			SetTitle( LPCTSTR szTitle )						{ m_stMemberInfo.strTitle = szTitle;			}
	LPCTSTR			GetTitle() const								{ return m_stMemberInfo.strTitle.c_str();		}

	void			SetLevel( long lLevel )							{ m_stMemberInfo.lLevel = lLevel;				}
	long			GetLevel() const								{ return m_stMemberInfo.lLevel;					}

	void			SetJob( long lJob )								{ m_stMemberInfo.lJob = lJob;					}
	long			GetJob() const									{ return m_stMemberInfo.lJob;					}

	void			SetOccupation( long lOccupation )				{ m_stMemberInfo.lOccupation = lOccupation;		}
	long			GetOccupation() const							{ return m_stMemberInfo.lOccupation;			}

	void			SetContribute( long lContribute )				{ m_stMemberInfo.lContribute = lContribute;		}
	long			GetContribute() const							{ return m_stMemberInfo.lContribute;			}

	void			SetAchievement( long lAchievement )				{ m_stMemberInfo.lAchievement = lAchievement;	}
	long			GetAchievement() const							{ return m_stMemberInfo.lAchievement;			}

	void			SetWarFund( long lWarFund )						{ m_stMemberInfo.lWarFund = lWarFund;			}
	long			GetWarFund() const								{ return m_stMemberInfo.lWarFund;				}

	void			SetState( long eState )							{ m_stMemberInfo.eState = eState;				}
	long			GetState() const								{ return m_stMemberInfo.eState;					}

    void            SetLastTime(long eLastTime)                     { m_stMemberInfo.LastOnlineTime = eLastTime;    }
    long            GetLastTime()const                              { return m_stMemberInfo.LastOnlineTime;         }

	void			SetOnline( bool bOnline )						{ m_stMemberInfo.bOnline = bOnline;				}
	bool			GetOnline() const								{ return m_stMemberInfo.bOnline;				}

	void			SetRegion( LPCTSTR szRegion )					{ m_stMemberInfo.strRegion = szRegion;			}
	LPCTSTR			GetRegion() const								{ return m_stMemberInfo.strRegion.c_str();		}

	void			SetSelected( bool bSelected )					{ m_stMemberInfo.bSelected = bSelected;			}
	bool			GetSelected() const								{ return m_stMemberInfo.bSelected;				}

	void			SetWar( bool bWar )								{ m_stMemberInfo.bWar = bWar;					}
	bool			GetWar() const									{ return m_stMemberInfo.bWar;					}

	// 排序比较函数
	static int		CompareForceRule( const CFacMember* pFacMember1, const CFacMember* pFacMember2 );

	static bool		CompareNormal( const CFacMember* pFacMember1, const CFacMember* pFacMember2 );
	static bool		CompareByName( const CFacMember* pFacMember1, const CFacMember* pFacMember2 );
	static bool		CompareByJob( const CFacMember* pFacMember1, const CFacMember* pFacMember2 );
	static bool		CompareByTitle( const CFacMember* pFacMember1, const CFacMember* pFacMember2 );
	static bool		CompareByLevel( const CFacMember* pFacMember1, const CFacMember* pFacMember2 );
	static bool		CompareByState( const CFacMember* pFacMember1, const CFacMember* pFacMember2 );
	static bool		CompareByRegion( const CFacMember* pFacMember1, const CFacMember* pFacMember2 );
	static bool		CompareByOnline( const CFacMember* pFacMember1, const CFacMember* pFacMember2 );
	static bool		CompareByOccupation( const CFacMember* pFacMember1, const CFacMember* pFacMember2 );
	static bool		CompareByContribute( const CFacMember* pFacMember1, const CFacMember* pFacMember2 );
	static bool		CompareBySelection( const CFacMember* pFacMember1, const CFacMember* pFacMember2 );
};




//**************************************************************************************
// 家族的 逻辑功能类
//**************************************************************************************

class CFaction
{
public:
	// 排序枚举
	enum	ENUM_SORT
	{
		SORT_BY_FACIOTN,			// 家族 排序规则
		SORT_BY_CHAIRMAN,			// 族长 排序规则
		SORT_BY_ONLINE,				// 在线 排序规则
		SORT_BY_LEVEL,				// 等级 排序规则

		MAXNUM_SORT
	};

	// 家族功能枚举
	enum	ENUM_FUN
	{
		FUN_FACTION,									// 家族
		FUN_WELFARE,									// 福利
		FUN_STORAGE,									// 专业
		FUN_SMELT,										// 商业
		FUN_FORGING,									// 战争
		FUN_SEWING,										// 显贵

		MAXNUM_FUN
	};

	// 家族信息结构
	struct	SFacInfo
	{
		CGUID		guidFac;							// 家族 GUID
		CGUID		guidUnion;							// 联盟 GUID
		CGUID		guidMaster;							// 会长 GUID
		string		strFacName;							// 家族 名字
		string		strMasterName;						// 会长 名字
		string		strPlacard;							// 家族 公告
		string		strMemo;							// 家族 备忘
		long		idCountry;							// 家族 国籍
		long		resFac;								// 家族 资源
		long		expFac;								// 家族 经验
		bool		bWar;								// 是否 战争
		bool		bTrade;								// 是否 贸易
		long		lUnionJob;							// 联盟 官阶
		bool		bRecruiting;						// 是否 招募
		bool		bOnline;							// 族长 在线
		long		arrLevel[ MAXNUM_FUN ];				// 功能 等级
		string		arrJobName[ MAXNUM_JobLvl ];		// 官阶 名称
		long		arrJobPurview[ MAXNUM_JobLvl ];		// 官阶 权限
		long		lWarFund;							// 家族 微尘
		long		lAchievement;						// 阿尔托勋值

		long		numMembers;							// 族员 人数
		long		numOnlineMembers;					// 在线 人数

		time_t		tMasterLastOnlineTime;				// 族长最后在线时间
		time_t		tLessMemNumTime;					// 族员过少时间

		DWORD		dwIconCRC;							// 图标校验码

		SFacInfo()	{ ZeroMemory(this, sizeof(SFacInfo)); }
	};

private:
    std::list<CFacMember*>		m_listApply;			// 申请信息表
	std::list<CFacMember*>		m_listMembers;			// 族员信息表
	std::vector<tagEnemy>		m_vecWarInfo[ eMaxWar ];// 战争信息数组 
    SFacInfo					m_stFacInfo;			// 家族信息

public:
	CFaction(void);
	~CFaction(void);
	CFaction( SFacInfo* pstFacInfo);
    CFaction( const CFaction& fac);

	void	ClearMembers();
	void	ClearApply();

	// 数据访问函数
	std::list<CFacMember*> *	GetListOfMembers()						{ return &m_listMembers;							}
	std::list<CFacMember*> *	GetListOfApply()						{ return &m_listApply;								}

	void				SetFacInfo( SFacInfo* pstFacInfo )				{ m_stFacInfo = *pstFacInfo;						}
	SFacInfo*			GetFacInfo()									{ return &m_stFacInfo;								}

	void				SetGUID( CGUID guidFac )						{ m_stFacInfo.guidFac = guidFac;					}
	CGUID				GetGUID() const									{ return m_stFacInfo.guidFac;						}

	void				SetUnionGUID( CGUID guidUnion )					{ m_stFacInfo.guidUnion = guidUnion;				}
	CGUID				GetUnionGUID() const							{ return m_stFacInfo.guidUnion;						}

	void				SetMasterGUID( CGUID guidMaster )				{ m_stFacInfo.guidMaster = guidMaster;				}
	CGUID				GetMasterGUID() const							{ return m_stFacInfo.guidMaster;					}

	void				SetName( LPCTSTR szFacName )					{ m_stFacInfo.strFacName = szFacName;				}
	LPCTSTR				GetName() const									{ return m_stFacInfo.strFacName.c_str();			}

	void				SetMasterName( LPCTSTR szMasterName )			{ m_stFacInfo.strMasterName = szMasterName;			}
	LPCTSTR				GetMasterName() const							{ return m_stFacInfo.strMasterName.c_str();			}

	void				SetPlacard( LPCTSTR szPlacard )					{ m_stFacInfo.strPlacard = szPlacard;				}
	LPCTSTR				GetPlacard() const								{ return m_stFacInfo.strPlacard.c_str();			}

	void				SetMemo( LPCTSTR szMemo )						{ m_stFacInfo.strMemo = szMemo;						}
	LPCTSTR				GetMemo() const									{ return m_stFacInfo.strMemo.c_str();				}

	void				SetRes( long res )								{ m_stFacInfo.resFac = res;							}
	long				GetRes() const									{ return m_stFacInfo.resFac;						}

	void				SetExp( long exp )								{ m_stFacInfo.expFac = exp;							}
	long				GetExp() const									{ return m_stFacInfo.expFac;						}

	void				SetLevel( long level )							{ m_stFacInfo.arrLevel[ FUN_FACTION ] = level;		}
	long				GetLevel() const								{ return m_stFacInfo.arrLevel[ FUN_FACTION ];		}

	void				SetCountry( long idCountry )					{ m_stFacInfo.idCountry = idCountry;				}
	long				GetCountry() const								{ return m_stFacInfo.idCountry;						}

	void				SetRecruiting( bool bRecruiting )				{ m_stFacInfo.bRecruiting = bRecruiting;			}
	bool				GetRecruiting() const							{ return m_stFacInfo.bRecruiting;					}

	void				SetWar( bool bWar )								{ m_stFacInfo.bWar = bWar;							}
	bool				GetWar() const									{ return m_stFacInfo.bWar;							}

	void				SetTrade( bool bTrade )							{ m_stFacInfo.bTrade = bTrade;						}
	bool				GetTrade() const								{ return m_stFacInfo.bTrade;						}

	void				SetIconCRC( DWORD dwIconCRC )					{ m_stFacInfo.dwIconCRC = dwIconCRC;				}
	DWORD				GetIconCRC() const								{ return m_stFacInfo.dwIconCRC;						}

	void				SetUnionJob( long lUnionJob )					{ m_stFacInfo.lUnionJob = lUnionJob;				}
	long				GetUnionJob() const								{ return m_stFacInfo.lUnionJob;						}

	void				SetWarFund( long lWarFund )						{ m_stFacInfo.lWarFund = lWarFund;					}
	long				GetWarFund() const								{ return m_stFacInfo.lWarFund;						}

	void				SetAchievement( long lAchievement )				{ m_stFacInfo.lAchievement = lAchievement;			}
	long				GetAchievement() const							{ return m_stFacInfo.lAchievement;					}

	void				SetNumOfMembers( long numMembers )				{ m_stFacInfo.numMembers = numMembers;				}
	void				SetNumOfOnlineMembers( long numOnlineMembers )	{ m_stFacInfo.numOnlineMembers = numOnlineMembers;	}

	long				GetNumOfMembers() const;
	long				GetNumOfOnlineMembers() const;

	void				SetLevel( ENUM_FUN eFun, long level );
	long				GetLevel( ENUM_FUN eFun ) const;

	void				SetOnline( bool bOnline );
	bool				GetOnline() const;

	void				SetJobName( long lJobLvl, LPCTSTR szJobName );
	LPCTSTR				GetJobName( long lJobLvl ) const;

	void				SetJobPurview( long lJobLvl, long lJobPurview );
	bool				GetJobPurview( long lJobLvl, eFactionPurviewIndex ePV ) const;

	void				SetEnemy( eBattleType eBT, tagEnemy* pEnemy );
	vector<tagEnemy>*	GetEnemy( eBattleType eBT );

	// 操作函数
	long				GetNumOfApply()									{ return (long)m_listApply.size();					}
	long				GetMaxNumOfMembers();
	long				GetRequireNumOfMembers();

	bool				AddMember( CFacMember::SMemberInfo* pstMemberInfo );
	bool				DelMember( CGUID guid );
	bool				HasMember( CGUID guid ) const;
	CFacMember*			GetMember( CGUID guid ) const;
	CFacMember*			GetMember( LPCTSTR szName ) const;

	bool				AddApply( CFacMember::SMemberInfo* pstMemberInfo );
	bool				DelApply( CGUID guid );
	bool				HasApply( CGUID guid ) const;
	CFacMember*			GetApply( CGUID guid ) const;
	CFacMember*			GetApply( LPCTSTR szName ) const;

	void				SortMembers( CFacMember::ENUM_SORT eSort );
	void				SortApply( CFacMember::ENUM_SORT eSort );

	// 排序比较函数
	static int		CompareForceRule( const CFaction* pFaction1, const CFaction* pFaction2 );

	static bool		CompareByFaction( const CFaction* pFaction1, const CFaction* pFaction2 );
	static bool		CompareByChairman( const CFaction* pFaction1, const CFaction* pFaction2 );
	static bool		CompareByOnline( const CFaction* pFaction1, const CFaction* pFaction2 );
	static bool		CompareByLevel( const CFaction* pFaction1, const CFaction* pFaction2 );
};

//**************************************************************************************
// 家族逻辑管理类
//**************************************************************************************
class CFactionManager
{
	std::list<CFaction*>			    m_listFactions;				// 家族表
	std::list<tagBaseInfo>				m_listBeachheads;			// 据点列表
    std::list<tagApplyInfoList>         m_listApplyInfo;            // 申请列表

public:
	CFactionManager();
	virtual ~CFactionManager();

	void	ClearFactions();

	// 数据访问函数
	long			GetFacLvlForPV( eFactionPurviewIndex ePVIndex );
	long			GetMaxNumByFacLv( long lFacLvl );
	long			GetRequireNumByFacLv( long lFacLvl );
	long			GetNumOfMyBeachheads();

	CFaction*		GetMyFaction();
	bool			GetMyRights( eFactionPurviewIndex ePV );

	CFacMember*		GetMyConfrere( CGUID guid );
	CFacMember*		GetMyConfrere( LPCTSTR szName );
	bool			AddMyConfrere( CFacMember::SMemberInfo* pstMemberInfo );

	long			GetMyJob();
	bool			SetMyJob( long lJob );

	LPCTSTR			GetMyPlacard();
	bool			SetMyPlacard( LPCTSTR szPlacard );

	LPCTSTR			GetMyMemo();
	bool			SetMyMemo( LPCTSTR szMemo );

	string			GetFacIconPath( CGUID guidFac );
	DWORD			GetFacIconCRC( CGUID guidFac );
	bool			CheckFacIconCRC( CGUID guidFac, DWORD dwNewCRC );

	list<CFaction*>*		GetListOfFactions()		{ return &m_listFactions;	}
	list<tagBaseInfo>*		GetListOfBeachheads()	{ return &m_listBeachheads;	}
    list<tagApplyInfoList>*	GetListOfApplyInfo()	{ return &m_listApplyInfo;	}

	// 检测函数
	bool			AmIChairman();
	bool			IsPlayerChairman( CPlayer* pPlayer );

	bool			AmIJoinedFaction();
	bool			AmIJoinedFaction( CGUID guidFaction );

	bool			IsPlayerJoinedFaction( CPlayer* pPlayer );
	bool			IsPlayerJoinedFaction( CPlayer* pPlayer, CGUID guidFaction );

	bool			CanUpgradeMyFun( CFaction::ENUM_FUN eFun );

	long			GetFunMaxLevel( CFaction::ENUM_FUN eFun );

	bool			HasRes_ForUpgradeMyFun( CFaction::ENUM_FUN eFun );
	bool			HasExp_ForUpgradeMyFun( CFaction::ENUM_FUN eFun );
	bool			HasGold_ForUpgradeMyFun( CFaction::ENUM_FUN eFun );
	bool			HasProp_ForUpgradeMyFun( CFaction::ENUM_FUN eFun );
	bool			HasFacLvl_ForUpgradeMyFun( CFaction::ENUM_FUN eFun );
	bool			HasMasterLvl_ForUpgradeMyFun( CFaction::ENUM_FUN eFun );
	bool			HasBeachhead_ForUpgradeMyFun( CFaction::ENUM_FUN eFun );

	long			NeedRes_ForUpgradeMyFun( CFaction::ENUM_FUN eFun );
	long			NeedExp_ForUpgradeMyFun( CFaction::ENUM_FUN eFun );
	long			NeedGold_ForUpgradeMyFun( CFaction::ENUM_FUN eFun );
	long			NeedProp_ForUpgradeMyFun( CFaction::ENUM_FUN eFun );
	long			NeedFacLvl_ForUpgradeMyFun( CFaction::ENUM_FUN eFun );
	long			NeedMasterLvl_ForUpgradeMyFun( CFaction::ENUM_FUN eFun );
	long			NeedBeachhead_ForUpgradeMyFun( CFaction::ENUM_FUN eFun );

	// 操作函数
    bool			AddFaction( CFaction::SFacInfo* pstFacInfo );
	bool			DelFaction( CGUID guid );
	bool			HasFaction( CGUID guid );
	CFaction*		GetFaction( CGUID guid );

	// 发送\接收 网络消息
	void			SendCreateMsg( LPCTSTR strFactionName ,LPCTSTR Placard);
	void			RecvCreateNotifyMsg( CMessage* pMsg );

	void			RecvUpdateFacIconNotifyMsg( CMessage* pMsg );
	void			RecvUpdateAllFacIconMsg( CMessage* pMsg );

	void			RecvInitMsg( CMessage* pMsg );
	void			RecvFacPublicDataMsg( CMessage* pMsg );
	void			RecvBaseInfoMsg( CMessage* pMsg );
	void			RecvPlacardInfoMsg( CMessage* pMsg );
	void			RecvMemoInfoMsg( CMessage* pMsg );
	void			RecvWarInfoMsg( CMessage* pMsg );
	void			RecvMemberAchievementMsg( CMessage* pMsg );
	void			RecvMemberWarFundMsg( CMessage* pMsg );

	void			RecvMemberInfoMsg( CMessage* pMsg );
	void			RecvRequisitionInfoMsg( CMessage* pMsg );
	void			RecvBeachheadInfoMsg( CMessage* pMsg );

	void			SendQuitMsg();
	void			SendDisbandMsg();
	void			SendKickMsg( CGUID guidMember );

	void			SendBaseInfoRequestMsg();
	void			SendUpgradeMsg( CFaction::ENUM_FUN eFun );

	void			SendSetTitleMsg( CGUID guidMember, LPCTSTR szTitle );
	void			SendSetChairmanMsg( CGUID guidMember );
	void			SendSetPlacardMsg( LPCTSTR szPlacard );
	void			SendSetMemoMsg( LPCTSTR szMemo );
	void			SendSetJobNameMsg( long idJob, LPCTSTR szJobName );
	void			SendSetJobPurviewMsg( long idJob, long lPurview );
	void			SendSetMemberJobMsg( CGUID guidMember, long idJob );
	void			SendCheckRequisitionMsg( long bAgree, CGUID guid );
	void			SendCheckRequisitionMsg( long bAgree );

	void			SendInviteRequestMsg( LPCTSTR szInvitePlayerName );
	void			SendInviteAnswerMsg( CGUID guidInviter, long bAgree );

	void			SendMemberWarMsg( CGUID guidMember, BYTE byWar );
	void			RecvMemberWarMsg( CMessage* pMsg );

	void			SendUploadFacIconMsg( LPCTSTR strFacIconFileName );
	void			SendUpdateFacIconMsg( CGUID guidFac );
	void			SendUpdateAllFacIconMsg();

    /// 
    void            SendFactionList();
    void            RecvFactionList(CMessage* pMsg);

	void			HandleErrorMsg( CMessage* pMsg );
};