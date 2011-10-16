//Account.h/////////////////////////////////////////////////////////////////////
//账号类:
//Author:安海川
//Create Time:2008.11.03
#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <string>
#include <list>
#include <iostream>
#include "../public/GUID.h"
#include "../../../../public/BaseDef.h"
#include "EntityGroup.h"
#include "../session/WorldServerSession.h"


	// ============================================================================
	//  Account Instance Class
	// ============================================================================
class Account : public CEntityGroup
	{
	public:
		Account();
		~Account();


		CWorldServerSession::WORLD_SESSION_STEP_DEF GetCurWsSessionStep(void) { return m_eWsSessionStepStatus; }
		void SetCurWsSessionStep(CWorldServerSession::WORLD_SESSION_STEP_DEF step) { m_eWsSessionStepStatus = step; }
	//	virtual void ProcessMsg(const CGUID& SessionID, long dbsPlugID, long wsPlugID, long retFlag,DB_OPERATION_TYPE op, BYTE* msgBuf, long& msgBufPos, long bufSize);
	//	virtual void GetFromByteArray(BYTE* msgBuf, long& msgBufPos, long bufSize=-1);
	//	virtual void AddToByteArray(std::vector<BYTE>& pBA);

		const CGUID& GetPlayerIdByName(const char* szName);

		long GetPlayerState(void) { return m_PlayerState; }
		void SetPlayerState(long state) { m_PlayerState = state;}

		long GetSignCode(void) { return m_lSignCode; }
		void SetSignCode(long code) { m_lSignCode = code; }

		long GetDelTime(void) { return m_lDelTime; }
		void SetDelTime(long time) { m_lDelTime = time; }

		CEntityGroup* FindLoginPlayer(const CGUID& guid);
		void	AddLoginPlayer(CEntityGroup* lPlayer);
		void	DelLoginPlayer(const CGUID& guid);
		void	ClearAllLoginPlayer(void);

		const CGUID& GetCurGamedPlayerGuid(void) { return m_CurGamedPlayerGuid; }
		void	SetCurGamedPlayerGuid(const CGUID& guid) { m_CurGamedPlayerGuid = guid; }

		bool	GetIsInDeleteMailSession(void) { return m_bIsInDeleteMailSession; }
		void	SetIsInDeleteMailSession(bool flag) { m_bIsInDeleteMailSession = flag; }
		bool	GetIsInLoadMailSession(void) { return m_bIsLoadMailSession; }
		void	SetIsInLoadMailSession(bool flag) { m_bIsLoadMailSession = flag; }

		long	GetLoginTimeoutValue(void) { return m_lLoginTimeoutValue; }// 登录超时时间间隔
		void	SetLoginTimeoutValue(long value) { m_lLoginTimeoutValue = value; }// 登录超时时间间隔
	private:
		long m_PlayerState;
		long m_lSignCode;
		long m_lDelTime;
		CWorldServerSession::WORLD_SESSION_STEP_DEF m_eWsSessionStepStatus;
		CGUID m_CurGamedPlayerGuid;
		bool	m_bIsInDeleteMailSession;
		bool	m_bIsLoadMailSession;
		long	m_lLoginTimeoutValue; // 登录超时时间间隔
	};  // end class Account


#endif
