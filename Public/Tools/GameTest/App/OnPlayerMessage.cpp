


#include "stdafx.h"
#include "GameClient.h"
#include "SkillMgr.h"
#include "RgnObjMgr.h"
#include "GoodsMgr.h"
#include "TradeMgr.h"


void CGameClient::OnPlayerMessage(CMessage* pMsg)
{
	switch ( pMsg->GetType() )
	{
	case MSG_S2C_PLAYER_ADDSKILL:
		{
			CGUID guid;
			pMsg->GetGUID(guid);

			long lSkillID			= pMsg->GetLong();
			WORD wSkillLevel		= pMsg->GetWord();
			DWORD dwRestoreTime	= pMsg->GetDWord();
			DWORD dwRangeMin		= pMsg->GetDWord();
			DWORD dwRangeMax		= pMsg->GetDWord();
			//
			if( m_pMainPlayer->GetExID() == guid)
			{
				m_pMainPlayer->GetSkillMgr()->AddSkill(lSkillID);
			}
		}
		break;
	case MSG_S2C_PLAYER_ANSWER_RELIVE:
		{
			WORD dwAction = pMsg->GetWord();
			WORD wState =  pMsg->GetWord();
			DWORD dwHp = pMsg->GetDWord();
			WORD wMp = pMsg->GetWord();

			m_pMainPlayer->SetState(wState);
			m_pMainPlayer->SetAction(dwAction);
			m_pMainPlayer->SetHp((DWORD)10000);
			m_pMainPlayer->SetMp((WORD)10000);
		}
		break;
	case MSG_S2C_PLAYER_LEVELUP:
		{
			CGUID guid;
			pMsg->GetGUID(guid);
			DWORD dwMaxHp=pMsg->GetDWord();
			WORD wMaxMp=pMsg->GetWord();
			WORD wLevel=pMsg->GetWord();
			DWORD dwExp=pMsg->GetDWord();
			DWORD dwUpExp=pMsg->GetDWord();
			DWORD dwPresentExp = pMsg->GetDWord();

			CPlayer* pPlayer =  (CPlayer*)m_pClientObjMgr->FindObject(TYPE_PLAYER,guid);
			if( pPlayer && pPlayer == m_pMainPlayer )
			{
				pPlayer->SetMaxHp(dwMaxHp);
				pPlayer->SetMaxMp(wMaxMp);

				pPlayer->SetHp(dwMaxHp);
				pPlayer->SetMp(wMaxMp);

				pPlayer->SetPresentExp(dwPresentExp);
				pPlayer->SetLevel(wLevel);
			}
		}
		break;
	case MSG_S2C_PLAYER_CHANGE_PROPERTIES:
		{
			CGUID guid;
			CGoods* pGoods = m_pMainPlayer->GetGoodsMgr()->QueryGoodsByGuid(guid);

			if( pGoods == NULL )
				pGoods = dynamic_cast<CGoods*>(m_pClientObjMgr->FindObject(TYPE_GOODS,guid));

			if( pGoods )
				pGoods->UpdateProperty(pMsg);
		}
		break;
	//case MSG_S2C_PLAYER_TRADEREQUEST:
	//	{
	//		CGUID guid;
	//		pMsg->GetGUID(guid);
	//		m_pMainPlayer->GetTradeMgr()->SetRequestGuid( guid );
	//	}
	//	break;
	//case MSG_S2C_PLAYER_TRADEBEGIN:
	//	{
	//		CGUID SessionID,RequestPlayerID,ResponsePlayerID;
	//		pMsg->GetGUID(SessionID);								//获得交易会话ID
	//		pMsg->GetGUID(RequestPlayerID);						//获得邀请方玩家的ID
	//		long lRequestPlugID = pMsg->GetLong();				//获得邀请方玩家的PLUGID
	//		pMsg->GetGUID(ResponsePlayerID);						//获得响应方玩家的ID
	//		long lResponsePlugID = pMsg->GetLong();				//获得响应方玩家的PLUGID

	//		CTradeMgr* pTradeMgr = m_pMainPlayer->GetTradeMgr();
	//		if( pTradeMgr == NULL ) break;

	//		if(m_pMainPlayer->GetExID() == RequestPlayerID)
	//		{
	//			pTradeMgr->StartTrade ( SessionID, RequestPlayerID,lRequestPlugID,ResponsePlayerID,lResponsePlugID);
	//		}
	//		else if(m_pMainPlayer->GetExID() == ResponsePlayerID)
	//		{
	//			pTradeMgr->StartTrade ( SessionID,ResponsePlayerID,lResponsePlugID,RequestPlayerID,lRequestPlugID );
	//		}
	//	}
	//	break;
	//case MSG_S2C_PLAYER_TRADELOCK:
	//	{
	//		m_pMainPlayer->GetTradeMgr()->SetYouLock( true );
	//	}
	//	break;
	//case MSG_S2C_PLAYER_WAITFORTRADE:
	//	{
	//		long lPlugId = pMsg->GetLong();
	//		BYTE bReady = pMsg->GetByte();

	//		if( bReady == 1 )
	//		{
	//			m_pMainPlayer->GetTradeMgr()->SetYouLock( true );
	//		}
	//		else
	//		{
	//			m_pMainPlayer->GetTradeMgr()->SetYouLock( false );
	//			m_pMainPlayer->GetTradeMgr()->SetMeLock( false );
	//		}
	//	}
	//	break;
	//case MSG_S2C_PLAYER_TRADECANCEL:
	//	{
	//		m_pMainPlayer->GetTradeMgr()->SetCanTrade( false );
	//	}
	//	break;
	//case MSG_S2C_PLAYER_TRADERESULT:
	//	{
	//		m_pMainPlayer->GetTradeMgr()->StartTrade(CGUID::GUID_INVALID,CGUID::GUID_INVALID,0,CGUID::GUID_INVALID,0);
	//		m_pMainPlayer->GetTradeMgr()->SetCanTrade( false );
	//	}
	//	break;
	default:
		break;
	}
}