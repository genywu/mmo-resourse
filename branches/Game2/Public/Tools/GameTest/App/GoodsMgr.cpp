


#include "stdafx.h"
#include "GoodsMgr.h"
#include "Goods.h"
#include "PacketMgr.h"
#include "EquipMgr.h"
#include "DepotMgr.h"
#include "Player.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CGoodsMgr::CGoodsMgr(CPlayer* pPlayer)
:m_pOwnerPlayer(pPlayer)
{
	m_pEquipMgr = new CEquipMgr;
	m_pPacketMgr = new CPacketMgr(this);
	m_pDepotMgr = new CDepotMgr;
}

CGoodsMgr::~CGoodsMgr()
{
	SAFE_DELETE(m_pEquipMgr);
	SAFE_DELETE(m_pPacketMgr);
	SAFE_DELETE(m_pDepotMgr);
}

void CGoodsMgr::Release()
{
	m_pPacketMgr->Release();
}

CGoods* CGoodsMgr::QueryGoodsByGuid(const CGUID& guid)
{
	list<tagGoods>::iterator it = m_listAllGoods.begin();
	for (; it != m_listAllGoods.end(); ++it )
	{
		if( it->pGoods && it->pGoods->GetExID() == guid )
			return it->pGoods;
	}
	return NULL;
}

CGoods* CGoodsMgr::QueryGoodsByGuid(const CGUID& guid,int* lPos,long* lType)
{
	list<tagGoods>::iterator it = m_listAllGoods.begin();
	for (; it != m_listAllGoods.end(); ++it )
	{
		if( it->pGoods && it->pGoods->GetExID() == guid )
			if( it->lPos == (*lPos) && it->lType == (*lType) )
				return it->pGoods;
	}
	return NULL;
}

CGoods* CGoodsMgr::GetGoodsByPos(long lExtendID,DWORD dwPos)
{
	if (lExtendID==PEI_PACKET&&dwPos>=0&&dwPos<HOST_GRIDITEM)
	{
		tagGoods* pstGoods = m_pPacketMgr->GetGoods((WORD)dwPos);
		if (pstGoods)
		{
			return pstGoods->pGoods;
		}
	}else if (lExtendID<=PEI_PACK5&&lExtendID>=PEI_PACK1)
	{
		WORD wGridNum = m_pPacketMgr->GetPackageDimen(lExtendID-PEI_PACK1);
		if (dwPos>=0&&dwPos<wGridNum)
		{
			tagGoods* pstGoods = m_pPacketMgr->GetSubGoods((WORD)(lExtendID-PEI_PACK1),(WORD)dwPos);
			if (pstGoods)
			{
				return pstGoods->pGoods;
			}
		}
	}else if (lExtendID==PEI_EQUIPMENT)//&&dwPos>=0&&dwPos<EQUIP_SUM)
	{
		return m_pEquipMgr->GetEquip(dwPos);
	}
	return NULL;
}

bool CGoodsMgr::AddGoods(CGoods* pGoods,DWORD dwPos,DWORD lExtendID)
{
	if (pGoods==NULL)	return false;

	tagGoods stGoods;
	stGoods.pGoods = pGoods;
	stGoods.lPos = dwPos;
	stGoods.lType = lExtendID;

	m_listAllGoods.push_back(stGoods);

	bool bRet = false;			//	返回值
	bool bDelGoods = false;

	CGUID tempTestGoodsGuid = pGoods->GetExID();
	DWORD dwTempGoodsIndex = pGoods->GetIndex();

	DWORD dwInsertNum = pGoods->GetNum();

	if( lExtendID==PEI_WALLET )						//	金币
	{
		if( m_pOwnerPlayer->GetPlayerMoneyNum() == 0 )
			m_pOwnerPlayer->SetPlayerMoneyGuid(pGoods->GetExID());
		m_pOwnerPlayer->SetPlayerMoneyNum(m_pOwnerPlayer->GetPlayerMoneyNum() + pGoods->GetNum());
		bRet = true;
	}
	else if( lExtendID == PEI_SILVERWALLET )		//	银币
	{
		if( m_pOwnerPlayer->GetPlayerSilverMoneyNum() == 0 )
			m_pOwnerPlayer->SetPlayerSilverMoneyGuid(pGoods->GetExID());
		m_pOwnerPlayer->SetPlayerSilverMoneyNum(m_pOwnerPlayer->GetPlayerSilverMoneyNum() + pGoods->GetNum());
		bRet = true;
	}
	else if( lExtendID == PEI_YUANBAO )				//	元宝
	{
		if( m_pOwnerPlayer->GetPlayerYuanBaoNum() == 0)
			m_pOwnerPlayer->SetPlayerYuanBaoGuid(pGoods->GetExID());
		m_pOwnerPlayer->SetPlayerYuanBaoNum(m_pOwnerPlayer->GetPlayerYuanBaoNum() + pGoods->GetNum());
		bRet = true;
	}
	else if( lExtendID == PEI_JIFEN )
	{
		m_pOwnerPlayer->SetPlayerJiFenNum(pGoods->GetNum());
		bRet = true;
	}
	else if( lExtendID==PEI_EQUIPMENT )		// 装备
	{
		m_pEquipMgr->SetEquip(dwPos,pGoods);
		bRet = true;
	}
	else if (lExtendID==PEI_SUBPACK)					//子背包栏
	{
		if (dwPos>=PEI_PACK1&&dwPos<=PEI_PACK5)
		{
			m_pPacketMgr->SetSubPGoods(dwPos-PEI_PACK1,pGoods);
			bRet = true;
		}
	}
	else if(lExtendID==PEI_BUSSINESPACKFIELD)
	{
		m_pPacketMgr->SetBussinesPack(pGoods);
		bRet = true;
	}
	else if(lExtendID==PEI_BUSSINESPACK)
	{
		tagGoods* ptGoods = m_pPacketMgr->GetBusinessGoods(dwPos);
		list<tagGoods>* pLGoods = NULL;
		if(ptGoods&&ptGoods->pGoods&&ptGoods->pGoods->GetIndex() == pGoods->GetIndex() )
		{
			long num = ptGoods->pGoods->GetNum() + pGoods->GetNum();
			if(num <= pGoods->GetEffectVal(CGoods::GAP_MAXOVERLAP,1) && num >0)
			{
				ptGoods->pGoods->SetNum(num);
				ptGoods->lNum = num;
				ptGoods->bChange = true;
				bDelGoods = true;
			}
			else if( num > ptGoods->pGoods->GetEffectVal(CGoods::GAP_MAXOVERLAP,1))
			{
				CGoods tGoods;
				tGoods.SetNum( pGoods->GetNum() );
				pGoods->SetNum(ptGoods->pGoods->GetNum());
				ptGoods->pGoods->SetNum( tGoods.GetNum() );
				ptGoods->lNum = tGoods.GetNum();
			}
		}
		else
		{
			tagGoods stGoods;
			stGoods.bChange = true;
			stGoods.pGoods = pGoods;
			stGoods.lPos = dwPos;
			stGoods.lNum = pGoods->GetNum();
			stGoods.lType = PEI_BUSSINESPACK;
			if( m_pPacketMgr->GetBusinessCote() != NULL )
			{
				m_pPacketMgr->GetBusinessCote()->m_listCoteGoods.push_back(stGoods);
				m_pPacketMgr->GetBusinessCote()->m_setCoteHaveGoods.insert(stGoods.lPos);
			}
		}
		bRet = true;
	}
	else if( lExtendID == PEI_PACKET )				//原始背包
	{
		tagGoods* pstGoods = m_pPacketMgr->GetGoods(dwPos);
		list<tagGoods>* pLGoods = NULL;

		//	如果 目标格子(dwPos) 有物品
		if(pstGoods&& pstGoods->pGoods&& pstGoods->pGoods->GetIndex() == pGoods->GetIndex())
		{
			long num =0;
			num = pstGoods->pGoods->GetNum() + pGoods->GetNum();

			//	叠加
			if(num<=pGoods->GetEffectVal(CGoods::GAP_MAXOVERLAP,1)&&num>0)
			{
				pstGoods->pGoods->SetNum(num);
				pstGoods->lNum=num;
				pstGoods->bChange=true;

				bDelGoods = true;
			}
			//	交换
			else if (num>pstGoods->pGoods->GetEffectVal(CGoods::GAP_MAXOVERLAP,1))
			{
				CGoods stGoods;
				stGoods.SetNum(pGoods->GetNum());
				pGoods->SetNum(pstGoods->pGoods->GetNum());
				pstGoods->pGoods->SetNum(stGoods.GetNum());
				pstGoods->lNum = stGoods.GetNum();
				bRet = true;
			}
			m_pPacketMgr->AddGoodsPos(dwPos);
		}
		else
		{
			tagGoods tGoods;
			tGoods.bChange =  true;
			tGoods.pGoods = pGoods;
			tGoods.lPos = dwPos;
			tGoods.lType = PEI_PACKET;
			tGoods.lNum = pGoods->GetNum();
			pLGoods = m_pPacketMgr->GetGoodsList();
			if(pLGoods)
			{
				pLGoods->push_back(tGoods);
				m_pPacketMgr->AddGoodsPos(dwPos);
			}
		}

		bRet = true;
	}
	else if ( lExtendID == PEI_PACK1 || lExtendID == PEI_PACK2 || lExtendID == PEI_PACK3 || lExtendID == PEI_PACK4 || lExtendID <= PEI_PACK5) //子背包
	{
		tagGoods* pCoteGoods = m_pPacketMgr->GetSubGoods(lExtendID - PEI_PACK1,dwPos);
		list<tagGoods>* PLGoods = m_pPacketMgr->GetSubGoodslist(lExtendID-PEI_PACK1);
		//	目标位置是否有物品
		if(pCoteGoods&&pCoteGoods->pGoods&&pCoteGoods->pGoods->GetIndex()==pGoods->GetIndex())
		{
			long num;
			num = pCoteGoods->pGoods->GetNum() + pGoods->GetNum();
			//	叠加
			if(num<=pGoods->GetEffectVal(CGoods::GAP_MAXOVERLAP,1)&&num>0)
			{
				pCoteGoods->pGoods->SetNum(num);
				pCoteGoods->lNum = num;
				pCoteGoods->bChange = true;
				// 内存泄漏 [12/27/2008 Square]
				bDelGoods = true;
			}
			//	交换
			else if(num > pCoteGoods->pGoods->GetEffectVal(CGoods::GAP_MAXOVERLAP,1))
			{
				CGoods stGoods;
				stGoods.SetNum(pGoods->GetNum());
				pGoods->SetNum(pCoteGoods->pGoods->GetNum());
				pCoteGoods->pGoods->SetNum(stGoods.GetNum());
				pCoteGoods->lNum = stGoods.GetNum();
			}
			m_pPacketMgr->GetCoteGoods(lExtendID-PEI_PACK1)->m_setCoteHaveGoods.insert(dwPos);
		}
		else
		{
			tagGoods tGoods;
			tGoods.bChange = true;
			tGoods.pGoods = pGoods;
			tGoods.lType = lExtendID;
			tGoods.lPos = dwPos;
			tGoods.lNum = pGoods->GetNum();
			if(PLGoods)
			{	
				m_pPacketMgr->GetCoteGoods(lExtendID-PEI_PACK1)->m_setCoteHaveGoods.insert(dwPos);
				PLGoods->push_back(tGoods);
			}
		}

		bRet = true;
	}
	else if( lExtendID == eDCT_Primary || lExtendID == eDCT_Secondary || lExtendID == eDCT_Secondary1 ||
		lExtendID == eDCT_Secondary2 || lExtendID == eDCT_Secondary3 || lExtendID == eDCT_Secondary4 )
	{
		bRet = m_pDepotMgr->AddDepotGoods(lExtendID,dwPos, pGoods);
	}

	if( bDelGoods )
	{
		SAFE_DELETE(pGoods);
	}
	pGoods = NULL;
	return bRet;
}

CGoods* CGoodsMgr::RemoveGoods(const CGUID& SourceGUID,const CGUID& DesGUID,DWORD dwPos,LONG lExtendID,DWORD dwAmount)
{
	CGoods* pRetGoods = NULL;

	//金币钱包
	if(lExtendID == PEI_WALLET)
	{
		m_pOwnerPlayer->SetPlayerMoneyNum(m_pOwnerPlayer->GetPlayerMoneyNum() - dwAmount);

		pRetGoods = new CGoods();
		pRetGoods->SetExID(SourceGUID);
		pRetGoods->SetIndex(MONEY_INDEX);
		pRetGoods->GetProperty()->dwType = GBT_MONEY;
		pRetGoods->SetNum(dwAmount);
		pRetGoods->SetType(TYPE_GOODS);
	}
	//银币钱包
	else if( lExtendID == PEI_SILVERWALLET )	// 银币钱包
	{
		m_pOwnerPlayer->SetPlayerSilverMoneyNum(m_pOwnerPlayer->GetPlayerSilverMoneyNum()-dwAmount);
		pRetGoods = new CGoods();
		pRetGoods->SetExID(SourceGUID);
		pRetGoods->SetIndex(YUANBAO_INDEX);
		pRetGoods->GetProperty()->dwType = GBT_SILVERMONEY;
		pRetGoods->SetNum(dwAmount);
	}
	//元宝
	else if(lExtendID == PEI_YUANBAO)
	{
		m_pOwnerPlayer->SetPlayerYuanBaoNum(m_pOwnerPlayer->GetPlayerYuanBaoNum() - dwAmount);
		pRetGoods = new CGoods();
		pRetGoods->SetExID(SourceGUID);
		pRetGoods->SetType(TYPE_GOODS);
		pRetGoods->SetIndex(YUANBAO_INDEX);
		pRetGoods->SetNum(dwAmount);
	}
	//积分
	else if(lExtendID == PEI_JIFEN)
	{
		m_pOwnerPlayer->SetPlayerJiFenNum(m_pOwnerPlayer->GetPlayerJiFenNum() - dwAmount);
		pRetGoods = new CGoods;
		pRetGoods->SetExID(SourceGUID);
		pRetGoods->SetType(TYPE_GOODS);
		pRetGoods->SetIndex(JIFEN_INDEX);
		pRetGoods->SetNum(dwAmount);
	}
	//装备
	else if(lExtendID == PEI_EQUIPMENT)
	{
		pRetGoods = m_pEquipMgr->GetEquip(dwPos);
		m_pEquipMgr->SetEquip(dwPos,pRetGoods);
	}
	//背包
	else if(lExtendID == PEI_PACKET)
	{
		int nIndex = (int)dwPos;
		list<tagGoods>* pGoodsList = m_pPacketMgr->GetGoodsList();
		if(pGoodsList == NULL)
		{
			return NULL;
		}

		CGoods* pTemp = NULL;
		list<tagGoods>::iterator it = pGoodsList->begin();

		for(; it!= pGoodsList->end(); )
		{
			//根据传入的GUID查找物品链表中对应的物品
			if(it->pGoods->GetExID() == SourceGUID )
			{
				//如果传入的操作数量小于物品的数量，则表示要进行拆分操作，否则只进行拿起所有物品的操作
				if( it->pGoods->GetNum() > dwAmount && dwAmount!=0 )
				{
					DWORD index = it->pGoods->GetIndex();

					pRetGoods = new CGoods;
					pRetGoods->SetNum(dwAmount);
					pRetGoods->SetExID(DesGUID);
					pRetGoods->SetIndex( index );
					pRetGoods->SetName(it->pGoods->GetName());

					it->pGoods->SetNum( it->pGoods->GetNum() - dwAmount );
					it->lNum = it->pGoods->GetNum();

					m_pPacketMgr->AddGoodsPos(it->lPos);
				}
				else 
				{
					m_pPacketMgr->DelGoodsPos(it->lPos);

					pRetGoods = it->pGoods;
					pGoodsList->erase(it);
				}
				break;
			}
			else
			{
				it++;
			}
		}
	}
	else if(lExtendID == PEI_SUBPACK)						
	{
		int nIndex = (int)dwPos;
		if ( nIndex >= PEI_PACK1 && nIndex <=PEI_PACK5 )
		{
			pRetGoods = m_pPacketMgr->GetCotePackage(nIndex - PEI_PACK1)->pGoods;
			if(pRetGoods ==NULL)	
			{
				return NULL;
			}
			m_pPacketMgr->SetSubPGoods(nIndex - PEI_PACK1,NULL);
		}
	}
	else if( lExtendID == PEI_PACK1 || lExtendID == PEI_PACK2 || lExtendID == PEI_PACK3 || lExtendID == PEI_PACK4 || lExtendID == PEI_PACK5 )
	{
		int nIndex = (int)dwPos;
		CGoods* pTemp = NULL;
		list<tagGoods> *pGoodsList = m_pPacketMgr->GetSubGoodslist( lExtendID - PEI_PACK1);
		if ( pGoodsList ==NULL)
		{
			return NULL;
		}
		list<tagGoods>::iterator it = pGoodsList->begin();
		for(; it!= pGoodsList->end(); )
		{
			/*根据传入的GUID查找物品链表中对应的物品*/
			if( it->pGoods->GetExID()== SourceGUID)
			{
				/*如果传入的操作数量小于物品的数量，则表示要进行拆分操作，否则只进行拿起所有物品的操作*/
				if( it->pGoods->GetNum()>dwAmount && dwAmount )
				{
					pRetGoods = new CGoods;
					pRetGoods->SetNum(dwAmount);
					pRetGoods->SetExID(DesGUID);
					pRetGoods->SetIndex( it->pGoods->GetIndex() );
					//					pRetGoods->SetBaseProperty(it->pGoods->GetBaseProperty());
					pRetGoods->SetName(it->pGoods->GetName());
					//					pRetGoods->SetGraphicsID(it->pGoods->GetGraphicsID());

					it->pGoods->SetNum( it->pGoods->GetNum() - dwAmount );
					it->lNum = it->pGoods->GetNum();
					m_pPacketMgr->GetCoteGoods(lExtendID - PEI_PACK1)->m_setCoteHaveGoods.insert(it->lPos);
				}
				else
				{
					pRetGoods = it->pGoods;
					
					set<DWORD>::iterator itPos = m_pPacketMgr->GetCoteGoods(lExtendID - PEI_PACK1)->m_setCoteHaveGoods.find(it->lPos);
					if( itPos != m_pPacketMgr->GetCoteGoods(lExtendID - PEI_PACK1)->m_setCoteHaveGoods.end() )
						m_pPacketMgr->GetCoteGoods(lExtendID - PEI_PACK1)->m_setCoteHaveGoods.erase(itPos);

					pGoodsList->erase(it);
				}			
				break;
			}else
			{
				it++;
			}
		}
	}
	else if(lExtendID>= eDCT_Gold && lExtendID <DEPOT_CONTAINER_TYPE_END_VALUE)
	{
		pRetGoods = m_pDepotMgr->DelDepotGoods(lExtendID,dwPos,dwAmount,SourceGUID,DesGUID);
	}

	return pRetGoods;
}