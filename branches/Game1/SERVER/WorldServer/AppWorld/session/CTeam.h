//#pragma once
//#include "csession.h"
//
//class CTeam :
//	public CSession
//{
//public:
//	enum ALLOCATION_SCHEME
//	{
//		AS_EXCLUSIVE,
//		AS_SHARE,
//		AS_FORCE_DWORD		= 0xFFFFFFFF,
//	};
//
//private:
//	DWORD					m_dwTeamID;
//	string					m_strPassword;
//	string					m_strTeamName;
//	LONG					m_lTeamLeaderID;
//
//	DWORD					m_dwLastCheckedTimeStamp;
//	ALLOCATION_SCHEME		m_asAllocationScheme;
//
//
//
//	//##执行了查询请求的队列,在每一次查询之后使用.
//	struct tagTeamateInfo
//	{
//		LONG lRegionID;
//		LONG lPlayerID;
//	};
//
//	vector<tagTeamateInfo>	m_vQueriedPlugs;
//
//	//Joe,添加此变量的目的是最大限度修正创建队伍时的bug(加入队伍立即解散)
//	LONG					m_lDelayTime;			//一个解散延迟时间
//
//public:
//	CTeam									(DWORD dwMinPlugs, DWORD dwMaxPlugs, DWORD dwLifeTime);
//	CTeam									(LONG lID, DWORD dwMinPlugs, DWORD dwMaxPlugs, DWORD dwLifeTime);
//	virtual				~CTeam				();
//
//	VOID				SetTeamID			( DWORD dwID );
//	DWORD				GetTeamID			();
//
//	VOID				SetLeader			( LONG lPlayerID );
//	LONG				GetLeader			();
//
//	VOID				SetPassword			( CHAR* pPassword );
//	CHAR*				GetPassword			();
//
//	VOID				SetTeamName			( CHAR* pTeamName );
//	CHAR*				GetTeamName			();
//
//	VOID				SetAllocationScheme	( ALLOCATION_SCHEME as );
//	ALLOCATION_SCHEME	GetAllocationScheme	();
//
//	VOID				KickPlayer( LONG lPlayerID );
//
//	VOID				OnProcessQueryMessage( LONG lRegionID, LONG lPlayerID );
//
//	virtual VOID AI();
//
//	virtual BOOL		Serialize(vector<BYTE>* pStream);
//
//	virtual BOOL		Unserialize(BYTE* pStream, LONG& lOffset);
//
//	VOID				GetGameServers( vector<DWORD>& vOut, LONG lTriggerPlugID = 0 );
//
//	//##会话的开始,中止操作.
//	virtual BOOL IsSessionAvailable();
//
//	virtual BOOL OnSessionStarted();
//
//	virtual BOOL OnSessionEnded();
//
//	virtual BOOL OnSessionAborted();
//
//	virtual BOOL OnPlugChangeState(LONG lPlugID, LONG lState, BYTE* pData, BOOL bSendToTrigger = FALSE);
//};
