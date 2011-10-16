



#include "stdafx.h"
#include "GameClient.h"
#include "GoodsMgr.h"
#include "EquipMgr.h"
#include "RgnObjMgr.h"
#include "PacketMgr.h"
#include "MedalMgr.h"
#include "DepotMgr.h"
#include "../Script/ScriptSys.h"

void CGameClient::OnGoodsMessage(CMessage* pMsg)
{
	switch( pMsg->GetType() )
	{
	case MSG_S2C_PICKGOOD:
	case MSG_S2C_EXCHANGEHOTKEY:
	case MSG_S2C_CLEARHOTKEY:
	case MSG_S2C_SETHOTKEY:
		break;
	case MSG_S2C_DROPMONEY:
		{
			int nRet = pMsg->GetChar();
			if( nRet == GOODS_DROP_SUCCEED )
			{
				DWORD dwNum = pMsg->GetDWord();
				m_pMainPlayer->SetPlayerMoneyNum( m_pMainPlayer->GetPlayerMoneyNum() - dwNum );
			}
		}
		break;
	case MSG_S2C_PICKMONEY:
		{
			int ret = pMsg->GetChar();
			if( ret == GOODS_PICK_SUCCEED )
			{
				DWORD lNum = pMsg->GetDWord();
				DWORD oldNum = m_pMainPlayer->GetPlayerMoneyNum();
				if(lNum>=0&&oldNum + lNum <= MAX_MONEY)
				{
					m_pMainPlayer->SetPlayerSilverMoneyNum(oldNum+lNum);
				}
			}
		}
		break;
	case MSG_S2C_EQUIPDAMAGE:
		{
			int nPOs = pMsg->GetByte();
			CGoods* pEquip = m_pMainPlayer->GetGoodsMgr()->GetEquipMgr()->GetEquip(nPOs);
			if(pEquip)
			{
				SAFE_DELETE( pEquip);
				m_pMainPlayer->GetGoodsMgr()->GetEquipMgr()->SetEquip(nPOs,NULL);
			}
		}
		break;
	case MSG_S2C_SET_YUANBAO:
		{
			DWORD dwBalance = pMsg->GetDWord();
			m_pMainPlayer->SetPlayerYuanBaoNum(dwBalance);
		}
		break;
	case MSG_S2C_SET_TICKET:
		{
			DWORD dwTicket = pMsg->GetDWord();
			m_pMainPlayer->SetPlayerJiFenNum(dwTicket);
		}
		break;
	case MSG_S2C_EQUIPDURUPDATE:
		{
			int nPOs= pMsg->GetByte();
			DWORD dwVal = pMsg->GetDWord();
			CGoods* pEquip = m_pMainPlayer->GetGoodsMgr()->GetEquipMgr()->GetEquip(nPOs);
			if(pEquip)
			{
				long lMaxDurValue = pEquip->GetEffectVal(CGoods::GAP_GOODS_MAXIMUM_DURABILITY,1);
				int nPercent = 100;
				if(lMaxDurValue>0)
				{
					nPercent = dwVal*100/lMaxDurValue;
				}
				pEquip->SetEffectVal(CGoods::GAP_GOODS_MAXIMUM_DURABILITY,2,dwVal);
			}
		}
		break;
	case MSG_S2C_EQUIP_REFINE_UPDATE:
		{
			BYTE nPos = pMsg->GetByte();
			DWORD dwVal = pMsg->GetDWord();
			CGoods* pEquip = m_pMainPlayer->GetGoodsMgr()->GetEquipMgr()->GetEquip(nPos);
			if(pEquip)
			{
				pEquip->SetEffectVal(CGoods::GAP_REFINE,1,dwVal);
			}
		}
		break;
	case MSG_S2C_DELETEEQUIP:
		{
			CGUID guid;
			pMsg->GetGUID(guid);
			int npos = pMsg->GetByte();
			if(guid == m_pMainPlayer->GetExID())
			{
				CGoods* pEquip = m_pMainPlayer->GetGoodsMgr()->GetEquipMgr()->GetEquip(npos);
				if(pEquip)
				{
					SAFE_DELETE(pEquip);
					m_pMainPlayer->GetGoodsMgr()->GetEquipMgr()->SetEquip(npos,NULL);
				}
			}
			else
			{
				CPlayer* pPlayer = (CPlayer*)m_pClientObjMgr->FindObject(TYPE_PLAYER,guid);
				if(pPlayer)
				{
					CEquipMgr* pMgr = pPlayer->GetGoodsMgr()->GetEquipMgr();
					if( pMgr )
					{
						pMgr->SetEquipIndex(npos,NULL);
						pMgr->SetEquipLevel(npos,NULL);
					}
				}
			}
		}
		break;
	case MSG_S2C_DELETEGOODS:
		{
			BYTE lPos = pMsg->GetByte();
			BYTE lNum = pMsg->GetByte();
			list<tagGoods>::iterator it = m_pMainPlayer->GetGoodsMgr()->GetPacketMgr()->GetGoodsList()->begin();
			for(;it != m_pMainPlayer->GetGoodsMgr()->GetPacketMgr()->GetGoodsList()->end();it++)
			{
				if( it->lPos == lPos)
				{
					int nXSize= 1;
					int nYSize = 1;
					int nTmepPos = it->lPos;
					for(int i=0;i<nYSize;i++)
					{
						nTmepPos += 8;
					}
					it->lNum -= lNum;
					break;
				}
			}
		}
		break;
	case MSG_S2C_DELETEALLGOODS:
		{
			BYTE lPos = pMsg->GetByte();
			list<tagGoods>* pGoodsList = m_pMainPlayer->GetGoodsMgr()->GetPacketMgr()->GetGoodsList();
			if( pGoodsList )
			{
				list<tagGoods>::iterator it = pGoodsList->begin();
				for(;it != pGoodsList->end();it++)
				{
					if( it->lPos == lPos)
					{
						SAFE_DELETE(it->pGoods);
						pGoodsList->erase(it);
						break;
					}
				}
			}
		}
		break;
	case MSG_S2C_VIEWEQUIP:
		break;
	case MSG_S2C_OPENGOODSUPGRADE:
		break;
	case MSG_S2C_GOODSUPGRADE_UPGRADE:
		{
			CGoods* pUpgradeGoods = NULL;
			if( pUpgradeGoods )
			{
				;
			}
		}
		break;
	case MSG_S2C_GOODS_UPDATEPROPERTY:
		{
			CGUID playerGuid;
			pMsg->GetGUID(playerGuid);
			CGUID goodsGuid;
			pMsg->GetGUID(goodsGuid);

			CPlayer*pPlayer = (CPlayer*)m_pClientObjMgr->FindObject(TYPE_PLAYER,playerGuid);
			if(pPlayer)
			{
				CGoods*pGoods = pPlayer->GetGoodsMgr()->QueryGoodsByGuid(goodsGuid);
				if(pGoods)
				{
					long lDataSize = pMsg->GetLong();
					BYTE* pData = new BYTE[lDataSize];
					pMsg->GetEx(pData,lDataSize);
					long lPos = 0;
					pGoods->DecordFromByteArray(pData,lPos,false);
					SAFE_DELETE_ARRAY(pData);
				}
			}
		}
		break;
	case MSG_S2C_OPENGOODSENCHASE:
		{
			CGUID SessionGuid;
			BYTE bPageType = pMsg->GetByte();
			pMsg->GetGUID(SessionGuid);
			long lPlayerID = pMsg->GetLong();

			if( bPageType>=0 && bPageType < CardEnchasePage_COUNT )
			{
				SetEnchasePageType(bPageType);
				SetEnchaseSessionGuid(SessionGuid);
				SetEnchasePlayerID(lPlayerID);
			}
		}
		break;
	case MSG_S2C_ENCHASE_ARRANGE:
		break;
	case MSG_S2C_ENCHASE_OPEN_HOLE:
		{
			BYTE holeNum = pMsg->GetByte();
		}
		break;
	case MSG_S2C_ENCHASE_CLOSE_HOLE:
		break;
	case MSG_S2C_ENCHASE_ENCHASECARD:
		break;
	case MSG_S2C_ENCHASE_REMOVECARD:
		break;
	case MSG_S2C_GOODS_OPEN_CONTAINER:
		break;
	case MSG_S2C_GOODS_OPEN_WINDOWS:
//		this->OpenGoodsHandleWindow(msg);
		break;
		//物品分解的最终结果
	case MSG_S2C_GOODS_RE_DISASSEMBLE_DO:
		break;
		//物品合成的结果
	case MSG_S2C_GOODS_RE_SYNTHESIZE_DO:
		{
			if( eGTC_Succeed == pMsg->GetLong())
			{
				long lNum = pMsg->GetLong();
				for(long c = 0;c<lNum;c++)
				{
					tagGoods stGoods;
					ZeroMemory(&stGoods,sizeof(stGoods));
					stGoods.lNum = pMsg->GetLong();
					CGUID goodsGuid;
					pMsg->GetGUID(goodsGuid);
					stGoods.pGoods = m_pMainPlayer->GetGoodsMgr()->QueryGoodsByGuid(goodsGuid,(int*)&stGoods.lPos,(long*)&stGoods.lType);
				}
			}
		}
		break;
	case MSG_S2C_GOODS_BIND:
		break;
	case MSG_S2C_DEPOT_RE_INPUT_PWD:
		{
			//密码正确
			if( eDMC_Succeed == pMsg->GetLong() )
			{
				//设置通行证
				CDepotMgr* pDepotMgr = m_pMainPlayer->GetGoodsMgr()->GetDepotMgr();
				if( pDepotMgr == NULL )	return;

				pDepotMgr->SetDepotHasPwd(true);

				DBReadSet db_r;
				pMsg->GetDBReadSet(db_r);

				pDepotMgr->DecordDepotInfo(db_r);

				SetCurOpenPageType(PT_DepotWindow);
			}
		}
		break;
	case MSG_S2C_DEPOT_RE_CLOSE:
		{
			char str[128];
			sprintf(str,"SetOpenDepot(%d,false);",pMsg->GetSocketID());
			CScriptSys::getInstance()->PerformString(str);
		}
		break;
	case MSG_S2C_MEDAL_ADD_NOTIFY:
		{
			long GoodsIndex = pMsg->GetLong();
			m_pMainPlayer->GetMedalMgr()->AddMedal(GoodsIndex);
		}
	default:
		break;
	}
}