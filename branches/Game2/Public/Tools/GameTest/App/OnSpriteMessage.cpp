#include "StdAfx.h"
#include "GameClient.h"
#include "RgnObjMgr.h"
#include "..\Script\ScriptSys.h"
#include "..\GameTest\GameTestDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Ð¡¾«Áé [12/30/2008 Square] ===================================================
void CGameClient::OnSpriteMessage(CMessage* pMsg)
{
	switch( pMsg->GetType() )
	{
	case MSG_S2C_SPRITE_AutoSupply_ChangeSetup_Re:
		{
			long lRet = pMsg->GetLong();
			if( lRet )
			{
				long lIndex = pMsg->GetLong();
//				m_pMainPlayer->SetAutoSupplySetup(lIndex);
			}
			break;
		}
	case MSG_S2C_SPRITE_AutoSupply_UseItem_Notify:
		{
			long lItemNumber = pMsg->GetLong();
//			long lItemIndex = m_pMainPlayer->GetItemIndex(lItemNumber);
//			long lGoodsAmount = m_pMainPlayer->GetItemAmount(lItemNumber);
// 			if( lGoodsAmount !=0 )
// 			{
// 				CGoods* pGoods = new CGoods();
// 				CGoodsList::FillGoodsInfo(lItemIndex,pGoods);
// 				if( pGoods == NULL )
// 					return;
// 				SAFE_DELETE(pGoods);
// 			}
		}
	}
}
