


#include "stdafx.h"
#include "DepotMgr.h"
#include "Goods.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDepotMgr::CDepotMgr()
{
	memset(&m_tagDepotInfo,0,sizeof(m_tagDepotInfo));
	m_tagDepotData = new tagDepotData;
	m_tagDepotData->bOpen = FALSE;
	m_tagDepotData->guidMoney = CGUID::GUID_INVALID;
	m_tagDepotData->guidSilver = CGUID::GUID_INVALID;
	m_tagDepotData->lMoneyAmount = 0;
	m_tagDepotData->lSilverAmount = 0;

}

CDepotMgr::~CDepotMgr()
{
	//仓库部分
	for( int i=0;i<DEPOT_PACK_NUM;i++)
	{
		//  		if( !m_tagDepotData->m_ArrPackData[i].mapGoodsList.empty())
		//  		{
		//  			hash_map<CGUID,tagGoods,hash_guid_compare>::iterator iter = m_tagDepotData->m_ArrPackData[i].mapGoodsList.begin();
		//  			for(;iter!=m_tagDepotData->m_ArrPackData[i].mapGoodsList.end();)
		//  			{
		//  				SAFE_DELETE(iter->second.pGoods);
		//  				++iter;
		//  				//				m_tagDepotData->m_ArrPackData[i].listGoods.erase(iter++);
		//  			}
		//  			m_tagDepotData->m_ArrPackData[i].mapGoodsList.clear();
		//  		}
	}
	SAFE_DELETE(m_tagDepotData);

}


CGoods* CDepotMgr::GetGoodsByGuid(const CGUID& guID,int *pPos,long* lExtendID)
{
	//查询主仓库
	for( int i = 0;i<DEPOT_PACK_NUM;i++)
	{
		if(m_tagDepotData)
		{
			hash_map<CGUID,tagGoods,hash_guid_compare>	GoodsList = m_tagDepotData->m_ArrPackData[i].mapGoodsList;
			hash_map<CGUID,tagGoods,hash_guid_compare>::iterator it = GoodsList.begin();
			if( it != GoodsList.end() )
			{
				if( pPos )
					pPos[0] = it->second.lPos;
				if(lExtendID)
					(*lExtendID) = DEPOT_MAIN_PACK + eDCT_Primary;
				return it->second.pGoods;
			}
		}
	}
	return NULL;
}

DWORD CDepotMgr::GetEmptyPos(DWORD iContainerID)
{
	if( iContainerID == eDCT_Primary || iContainerID == eDCT_Secondary || iContainerID == eDCT_Secondary1 ||
		iContainerID == eDCT_Secondary2 || iContainerID == eDCT_Secondary3 || iContainerID == eDCT_Secondary4 )
	{
		if( iContainerID == eDCT_Primary)
		{
			for( int i=0;i<DEPOT_MAIN_CELL_NUM;i++)
			{
				if( m_tagDepotData->m_ArrPackData[DEPOT_MAIN_PACK].mapHaveGoods[i] == FALSE )
					return i;
			}
		}
		else
		{
			for( int i=0;i<PACK_KB108_CELL_NUM;i++)
			{
				if( m_tagDepotData->m_ArrPackData[iContainerID - eDCT_Primary].mapHaveGoods[i] == FALSE)
					return i;
			}
		}
	}
	return -1;
}

void CDepotMgr::DecordDepotHeadInfo(DBReadSet & db_read)
{
	//获取仓库信息头 保存数据
	db_read.GetBufferFromByteArray(&m_tagDepotInfo,sizeof(m_tagDepotInfo));
}

void CDepotMgr::DecordDepotChildGoodsInfo(DBReadSet& db_read,long lDepotPackID,long lPackID)
{
	m_tagDepotData->m_ArrPackData[lDepotPackID].mapHaveGoods.clear();
	//
	m_tagDepotData->m_ArrPackData[lDepotPackID].lContainerID = lPackID;

	long lNum = db_read.GetLongFromByteArray();
	m_tagDepotData->m_ArrPackData[lDepotPackID].lGoodsAmount = lNum;
	for (long i=0;i < lNum;i++)
	{
		tagGoods tGoods;
		tGoods.bChange	= db_read.GetByteFromByteArray() != 0;
		tGoods.lNum		= db_read.GetWordFromByteArray();
		tGoods.lPos		= db_read.GetByteFromByteArray();
		tGoods.lType	= lPackID;
		tGoods.pGoods	= new CGoods;
		if( tGoods.pGoods->DecordFromDataBlock(db_read) )
		{
			//放入链表
			m_tagDepotData->m_ArrPackData[lDepotPackID].mapGoodsList[tGoods.pGoods->GetExID()] = tGoods;
			m_tagDepotData->m_ArrPackData[lDepotPackID].mapHaveGoods[tGoods.lPos] = TRUE;
		}
		else
		{
			SAFE_DELETE(tGoods.pGoods);
		}
	}
}


bool CDepotMgr::AddDepotGoods(long lContainerID, long pos, CGoods *& pGoods)
{
	bool bRet = false;
	switch( lContainerID )
	{
		//金币
	case eDCT_Gold:
		{
			if( !m_tagDepotData->lMoneyAmount )
				m_tagDepotData->guidMoney = pGoods->GetExID();
			m_tagDepotData->lMoneyAmount += pGoods->GetNum();
		}
		bRet = true;
		break;
		//宝石 --- 银币
	//case eDCT_Gem:
	//	{
	//		if( !m_tagDepotData->lSilverAmount )
	//			m_tagDepotData->guidSilver = pGoods->GetExID();
	//		m_tagDepotData->lSilverAmount += pGoods->GetNum();
	//	}
	//	bRet = true;
	//	break;
		//主仓库
	case eDCT_Primary:
		{
			hash_map<CGUID,tagGoods,hash_guid_compare>::iterator it = m_tagDepotData[DEPOT_MAIN_PACK].m_ArrPackData->mapGoodsList.begin();
			for(; it != m_tagDepotData[DEPOT_MAIN_PACK].m_ArrPackData->mapGoodsList.end(); it++)
			{
				if( it->second.lPos == pos )
				{
					tagGoods aimGoods = it->second;
					//	如果目标位置有物品,索引相同则叠加
					if( //aimGoods != NULL &&
						aimGoods.pGoods != NULL &&
						aimGoods.pGoods->GetIndex() == pGoods->GetIndex() &&
						aimGoods.pGoods->GetNum() + pGoods->GetNum() <= pGoods->GetEffectVal(CGoods::GAP_MAXOVERLAP,1) )
					{
						aimGoods.pGoods->SetNum( aimGoods.pGoods->GetNum() + pGoods->GetNum() );
						aimGoods.lNum = aimGoods.pGoods->GetNum();
						SAFE_DELETE( pGoods );
						bRet = true;
					}
					break;
				}
			}

			if( !bRet )
			{
				tagGoods goods;
				goods.bChange = true;
				goods.pGoods = pGoods;
				goods.lPos = pos;
				goods.lType = eDCT_Primary;
				goods.lNum = pGoods->GetNum();

				m_tagDepotData->m_ArrPackData[DEPOT_MAIN_PACK].mapGoodsList[goods.pGoods->GetExID()] = goods;
				m_tagDepotData->m_ArrPackData[DEPOT_MAIN_PACK].mapHaveGoods[goods.lPos] = TRUE;

				bRet = true;
			}
		}
		break;
	default:
		bRet = false;
		break;
	}
	return bRet;
}

CGoods* CDepotMgr::DelDepotGoods(long lContainerID, long pos,long lNum,const CGUID& guid,const CGUID& AimGuid)
{
	CGoods* pRetGoods = NULL;

	switch( lContainerID )
	{
	case eDCT_Gold:
		{
			pRetGoods = new CGoods;
			pRetGoods->SetNum(lNum);
			pRetGoods->SetType(TYPE_GOODS);
			pRetGoods->SetExID(m_tagDepotData->guidMoney);

			m_tagDepotData->lMoneyAmount -= lNum;
		}
		break;
	//case eDCT_Gem:
	//	{
	//		pRetGoods = new CGoods;
	//		pRetGoods->SetExID(m_tagDepotData->guidSilver);
	//		pRetGoods->SetType(TYPE_GOODS);
	//		pRetGoods->SetNum(lNum);

	//		m_tagDepotData->lSilverAmount -= lNum;
	//	}
	//	break;
	case eDCT_Primary:
		{
			hash_map<CGUID,tagGoods,hash_guid_compare> depotlist = m_tagDepotData->m_ArrPackData[DEPOT_MAIN_PACK].mapGoodsList;
			hash_map<CGUID,tagGoods,hash_guid_compare>::iterator it = depotlist.find(guid);
			if( it != depotlist.end())
			{
				CGoods *pGoods = it->second.pGoods;
				if( pGoods->GetNum() > lNum )
				{
					pRetGoods = new CGoods;
					pRetGoods->SetNum(lNum);
					pRetGoods->SetExID(AimGuid);
					pRetGoods->SetIndex( pGoods->GetIndex() );
					//					pRetGoods->SetBaseProperty(pGoods->GetBaseProperty());
					pRetGoods->SetName(pGoods->GetName());
					//					pRetGoods->SetGraphicsID(pGoods->GetGraphicsID());

					pGoods->SetNum( pGoods->GetNum() - lNum );
					it->second.lNum = pGoods->GetNum();
					m_tagDepotData->m_ArrPackData[DEPOT_MAIN_PACK].mapHaveGoods[it->second.lPos] = TRUE;
				}
				else 
				{
					pRetGoods = it->second.pGoods;
					m_tagDepotData->m_ArrPackData[DEPOT_MAIN_PACK].mapHaveGoods[it->second.lPos] = FALSE;
					m_tagDepotData->m_ArrPackData[DEPOT_MAIN_PACK].mapGoodsList.erase(it);
					m_tagDepotData->m_ArrPackData[DEPOT_MAIN_PACK].lGoodsAmount--;
				}
				break;
			}
		}
		break;
	default:	
		break;
	}
	return pRetGoods;
}

void CDepotMgr::DecordDepotInfo(DBReadSet& db_read)
{
	//清空所有仓库
	for( int i=0;i<DEPOT_PACK_NUM;i++)
	{
		hash_map<CGUID,tagGoods,hash_guid_compare> goodsList = m_tagDepotData->m_ArrPackData[i].mapGoodsList;
		m_tagDepotData->m_ArrPackData[i].mapHaveGoods.clear();
		if ( !goodsList.empty() )
		{
			hash_map<CGUID,tagGoods,hash_guid_compare>::iterator iter = goodsList.begin();
			for(;iter != goodsList.end();)
			{
				SAFE_DELETE(iter->second.pGoods);
				++iter;
			}
			goodsList.clear();
		}
	}

	//	获取主仓库物品
	//long lNum;
	//{
	//	获取数量
	long lNum = db_read.GetLongFromByteArray();
	for(int i=0;i<lNum;i++)
	{
		tagGoods pTagGoods;// = new tagGoods;
		pTagGoods.bChange	= db_read.GetByteFromByteArray() != 0;
		pTagGoods.lNum		= db_read.GetWordFromByteArray();
		pTagGoods.lPos		= db_read.GetByteFromByteArray();
		pTagGoods.lType	= eDCT_Primary;
		pTagGoods.pGoods	= new CGoods;
		if(pTagGoods.pGoods->DecordFromDataBlock(db_read))
		{
			m_tagDepotData->m_ArrPackData[DEPOT_MAIN_PACK].mapGoodsList[pTagGoods.pGoods->GetExID()] = pTagGoods;
			m_tagDepotData->m_ArrPackData[DEPOT_MAIN_PACK].mapHaveGoods[pTagGoods.lPos] = TRUE;
		}
		else
		{
			SAFE_DELETE(pTagGoods.pGoods);
		}
	}
	//}

	//扩展栏位的字背包
	BYTE lSubPackNum = db_read.GetByteFromByteArray();

	for(BYTE i = 0;i<lSubPackNum;i++)
	{
		tagGoods pTagGoods;// = new tagGoods;
		pTagGoods.lNum		= 1;
		pTagGoods.lType	= eDCT_Secondary;
		pTagGoods.pGoods	= new CGoods;
		if(pTagGoods.pGoods->DecordFromDataBlock(db_read))
		{
			pTagGoods.lPos = DEPOT_SUBPACK_BASEPOS + (DWORD)db_read.GetByteFromByteArray();
			m_tagDepotData->m_ArrPackData[DEPOT_ITEM_PACK].mapGoodsList[pTagGoods.pGoods->GetExID()] = pTagGoods;
			m_tagDepotData->m_ArrPackData[DEPOT_ITEM_PACK].mapHaveGoods[pTagGoods.lPos] = TRUE;
		}
		else
		{
			pTagGoods.lPos = (DWORD)db_read.GetByteFromByteArray();
			SAFE_DELETE(pTagGoods.pGoods);
		}
	}

	// 获取 子背包 物品
	for(long i=2;i<DEPOT_SUBPACK_NUM+2;i++)
	{
		DecordDepotChildGoodsInfo(db_read,i,eDCT_Secondary1+i);
	}

	//	金币
	m_tagDepotData->lMoneyAmount = db_read.GetLongFromByteArray();
	db_read.GetBufferFromByteArray(m_tagDepotData->guidMoney);
	//	银币
	m_tagDepotData->lSilverAmount = db_read.GetLongFromByteArray();
	db_read.GetBufferFromByteArray(m_tagDepotData->guidSilver);
	//
	m_tagDepotData->m_ArrPackData[DEPOT_MAIN_PACK].lGoodsAmount = lNum + lSubPackNum;
	m_tagDepotData->m_ArrPackData[DEPOT_MAIN_PACK].lContainerID = eDCT_Primary;


}