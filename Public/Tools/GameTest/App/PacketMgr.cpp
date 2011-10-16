/**
============================================================
*  Copyright (c) 2010
*  All rights reserved
*  文件名称：CMainFrame.cpp
*  功能：程度单文档框架
*  当前版本：1.1.0
*  作者：刘贵荣
*  完成日期：2010年8月23日 
*  取代版本：1.0.0
*  原作者：刘贵荣
*  完成日期：2010年8月23日
*
============================================================
*/



#include "stdafx.h"
#include "PacketMgr.h"
#include "Goods.h"
#include "GoodsMgr.h"
#include "Player.h"
#include "DepotMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CPacketMgr::CPacketMgr(CGoodsMgr* pFather)
{
	m_pFather = pFather;

	m_CoteBussinesPack = new tagCote;
	for (int i = 0;i<COTE_NUM;i++)
	{
		m_Cote[i] = new tagCote;
		m_Cote[i]->pGoods = NULL;
	}
}

CPacketMgr::~CPacketMgr()
{
	SAFE_DELETE(m_CoteBussinesPack);
	for (int i = 0;i<COTE_NUM;i++)
	{
		if (m_Cote[i]->pGoods != NULL)
			SAFE_DELETE(m_Cote[i]->pGoods);
		SAFE_DELETE(m_Cote[i]);
	}
}

void CPacketMgr::Release()
{
	//清空背包CGoods指针
	for(list<tagGoods>::iterator it = m_listGoods.begin(); it !=m_listGoods.end();it++)
	{	
		SAFE_DELETE(it->pGoods);
	}

	//清除背包物品链表容器
	m_listGoods.clear(); 

	//清空道具背包
	for (int i = 0;i<COTE_NUM;i++)  //遍历每个背包
	{
		if (m_Cote[i]!=NULL)
		{
			if( m_Cote[i]->pGoods != NULL )
			{
				if( !m_Cote[i]->m_listCoteGoods.empty())
				{
					list<tagGoods>::iterator it = m_Cote[i]->m_listCoteGoods.begin();
					for (;it!=m_Cote[i]->m_listCoteGoods.end();it++)
					{
						SAFE_DELETE( it->pGoods );		//清空每个背包里的物品的信息
					}
					m_Cote[i]->m_listCoteGoods.clear();	
				}
				SAFE_DELETE(m_Cote[i]->pGoods);
			}
		//	SAFE_DELETE(m_Cote[i]);
		}
	}

	/*if( m_CoteBussinesPack != NULL )
	{
	if(m_CoteBussinesPack->pGoods!=NULL)
	{
	if (!m_CoteBussinesPack->m_listCoteGoods.empty())
	{
	list<tagGoods>::iterator it = m_CoteBussinesPack->m_listCoteGoods.begin();
	for (;it!=m_CoteBussinesPack->m_listCoteGoods.end();it++)
	{
	SAFE_DELETE(it->pGoods);
	}
	m_CoteBussinesPack->m_listCoteGoods.clear();
	}
	SAFE_DELETE(m_CoteBussinesPack->pGoods);
	}
	//
	SAFE_DELETE(m_CoteBussinesPack);
	}*/
}

tagGoods* CPacketMgr:: GetGoodsByOrgName(const char* strOrgName)
{
	//通过原始名得到一个Goods
	//CGoods* pGoods = NULL;
	//原始背包
	/*list<tagGoods>::iterator it = m_listGoods.begin();
	for( ; it != m_listGoods.end(); it++)
	{
	if( !strcmp(it->pGoods->GetOrginName(), strOrgName) )
	{
	return &(*it);
	}
	}
	//查询子背包
	for (int i = 0;i<COTE_NUM;i++)
	{	
	//在子背包中查找
	list<tagGoods>* pGoodList = GetSubGoodslist(i);
	if (pGoodList)
	{
	list<tagGoods>::iterator GoodsIter=pGoodList->begin();
	for(;GoodsIter!=pGoodList->end();GoodsIter++)
	{
	if( !strcmp(GoodsIter->pGoods->GetOrginName(), strOrgName) )
	return &(*GoodsIter);
	}
	}
	}
	return NULL;*/


	//原始背包
	list<tagGoods>::iterator it = m_listGoods.begin();
	for( ; it != m_listGoods.end(); it++)
	{
		if( !strcmp(it->pGoods->GetOrginName(), strOrgName) )
		{
			return &(*it);
		}
	}
	//查询子背包
	/*for (int i = 0;i<COTE_NUM;i++)
	{	
	//在子背包中查找
	list<tagGoods>* pGoodList = GetSubGoodslist(i);
	if (pGoodList)
	{
	list<tagGoods>::iterator GoodsIter=pGoodList->begin();
	for(;GoodsIter!=pGoodList->end();GoodsIter++)
	{
	if( !strcmp(GoodsIter->pGoods->GetOrginName(), strOrgName) )
	return &(*GoodsIter);
	}
	}
	}*/
	return NULL;
}

void CPacketMgr::DecodeFromDataBlock(DBReadSet& dbRead,bool bExData)
{
	bool bVal = false;

	//	主背包
	long lNum = dbRead.GetLongFromByteArray();
	if(lNum>0)
	{
		for(int i=0;i<lNum;++i)
		{
			tagGoods stGoods;
			stGoods.bChange = dbRead.GetByteFromByteArray() != 0;
			stGoods.lNum = dbRead.GetWordFromByteArray();
			stGoods.lPos = dbRead.GetByteFromByteArray();
			stGoods.lType = PEI_PACKET;
			stGoods.pGoods = new CGoods;
			bVal=stGoods.pGoods->DecordFromDataBlock(dbRead,bExData);

			if(bVal)
			{
				m_listGoods.push_back(stGoods);
				m_setPacketEmptyPos.insert(stGoods.lPos);
				stGoods.pGoods = NULL;
			}
			else
			{
				SAFE_DELETE(stGoods.pGoods);
			}
		}
	}

	//	背包栏位
	BYTE num = dbRead.GetByteFromByteArray();
	for(int i=0;i<num;i++)
	{
		CGoods* pGoods = new CGoods;
		bVal = pGoods->DecordFromDataBlock(dbRead,bExData);
		BYTE bIndex = dbRead.GetByteFromByteArray();
		if(bVal&&m_Cote[bIndex]!=NULL)
		{
			m_Cote[bIndex]->pGoods = pGoods;
			pGoods = NULL;
		}
		else
			SAFE_DELETE(pGoods);
	}

	//每个子背包内的物品
	for (int i=0;i<COTE_NUM;++i)
	{
		lNum = dbRead.GetLongFromByteArray();
		if(lNum>0)
		{
			for(int j=0;j<lNum;j++)
			{
				tagGoods stGoods;
				stGoods.bChange = dbRead.GetByteFromByteArray() == 0? false:true;
				stGoods.lNum = dbRead.GetWordFromByteArray();
				stGoods.lPos = dbRead.GetByteFromByteArray();
				stGoods.lType = PEI_PACK1 + i;
				stGoods.pGoods = new CGoods;
				bVal = stGoods.pGoods->DecordFromDataBlock(dbRead,bExData);

				if(bVal)
				{
					m_Cote[i]->m_listCoteGoods.push_back(stGoods);
					m_Cote[i]->m_setCoteHaveGoods.insert(stGoods.lPos);
					stGoods.pGoods = NULL;
				}
				else
				{
					SAFE_DELETE(stGoods.pGoods);
				}
			}
		}
	}
}


void CPacketMgr::DecodeBusinessFromDataBlock(DBReadSet& dbRead,bool bExData)
{
	bool bVal = false;

	BYTE bIsHaveBusinessPack = dbRead.GetByteFromByteArray();
	if( bIsHaveBusinessPack >0 )
	{
		CGoods* pBusinessPackGoods = new CGoods;
		bVal = pBusinessPackGoods->DecordFromDataBlock(dbRead,bExData);
		if( bVal && m_CoteBussinesPack != NULL )
		{
			m_CoteBussinesPack->pGoods = pBusinessPackGoods;							//pGoods中包含了道具背包的大小等属性
			pBusinessPackGoods = NULL;		
			//##初始化跑墒背包里的每个物品的信息
			long lNum=dbRead.GetLongFromByteArray();	// 物品个数
			if (lNum>0)
			{
				for(int j=0;j<lNum;j++)
				{
					tagGoods stGood;
					stGood.bChange=dbRead.GetByteFromByteArray() != 0;
					stGood.lNum = dbRead.GetWordFromByteArray();
					stGood.lPos = dbRead.GetByteFromByteArray();
					stGood.lType = PEI_BUSSINESPACK;
					stGood.pGoods = new CGoods;
					bVal = stGood.pGoods->DecordFromDataBlock(dbRead,bExData);
					if(bVal)
					{
						m_CoteBussinesPack->m_listCoteGoods.push_back(stGood);
						stGood.pGoods = NULL;
					}
					else
					{
						SAFE_DELETE(stGood.pGoods);
					}
				}
			}
		}
		else
		{
			SAFE_DELETE(pBusinessPackGoods);
		}
	}
}

CGoods* CPacketMgr::GetGoodsByGUID(const CGUID& guID,int *pPos,long* lExtendID)
{
	CGoods* pGoods = NULL;

	list<tagGoods>::iterator it = m_listGoods.begin();
	for(;it != m_listGoods.end(); ++it )
	{
		pGoods = it->pGoods;
		if(pGoods && pGoods->GetExID() == guID)
		{
			if(pPos)
				pPos[0] = it->lPos;
			if(lExtendID)
				(*lExtendID) = PEI_PACKET;
			return pGoods;
		}
	}
	//查询子背包
	for (int i = 0;i<COTE_NUM;i++)
	{	
		//在子背包栏位上查找
		pGoods = GetSubPGoods(i);
		if (pGoods&&pGoods->GetExID()==guID)
		{
			if(pPos)pPos[0]=i;
			if(lExtendID) 
				(*lExtendID) = PEI_SUBPACK;
			return pGoods;
		}
		//在子背包中查找
		list<tagGoods>* pGoodList = GetSubGoodslist(i);
		if (pGoodList!=NULL)
		{
			list<tagGoods>::iterator it =pGoodList->begin();
			for(;it !=pGoodList->end();it++)
			{
				pGoods = it->pGoods;
				if(pGoods && pGoods->GetExID() == guID)
				{
					if(pPos)pPos[0]= it->lPos;
					if(lExtendID)
						(*lExtendID) = (PEI_PACK1 + i);
					return pGoods;
				}
			}
		}
	}

	if( m_CoteBussinesPack != NULL && m_CoteBussinesPack->pGoods != NULL )
	{
		if (m_CoteBussinesPack->pGoods->GetExID()==guID)
		{
			return m_CoteBussinesPack->pGoods;
		}
		list<tagGoods>* pGoodList = &m_CoteBussinesPack->m_listCoteGoods;
		if (pGoodList)
		{
			list<tagGoods>::iterator GoodsIter=pGoodList->begin();
			for(;GoodsIter!=pGoodList->end();GoodsIter++)
			{
				pGoods = GoodsIter->pGoods;
				if(pGoods && pGoods->GetExID() == guID)
				{
					if(pPos!=NULL) pPos[0]=GoodsIter->lPos;
					if (lExtendID!=NULL) (*lExtendID) = PEI_BUSSINESPACK;
					return pGoods;
				}
			}
		}
	}

	return NULL;
}

CGoods* CPacketMgr::FindGoodsByOrgName(const char* name)
{
	//在背包中查找
	list<tagGoods>* pGoodList = GetGoodsList();
	if ( pGoodList != NULL )
	{
		list<tagGoods>::iterator GoodsIter=pGoodList->begin();
		for(;GoodsIter!=pGoodList->end();GoodsIter++)
		{
			if( !strcmp( GoodsIter->pGoods->GetOrginName(),name) )
				return /*(CGoods*)*/GoodsIter->pGoods;
		}
	}
	//查询子背包
	for (int i = 0;i<COTE_NUM;i++)
	{	
		//在子背包栏位上查找
		CGoods* pGoods = GetSubPGoods(i);
		if ( !strcmp( pGoods->GetOrginName(), name) )
		{
			return pGoods;
		}
		//在子背包中查找
		pGoodList = GetSubGoodslist(i);
		if (pGoodList!=NULL)
		{
			list<tagGoods>::iterator GoodsIter=pGoodList->begin();
			for(;GoodsIter!=pGoodList->end();GoodsIter++)
			{
				CGoods* pGoods = GoodsIter->pGoods;
				if(pGoods && !strcmp( pGoods->GetOrginName(), name))
				{
					return pGoods;
				}
			}
		}
	}
	return NULL;
}


//获取指定序号的子背包指定位置的物品
tagGoods* CPacketMgr::GetSubGoods(WORD ExtendID,WORD lPos)
{
	if (ExtendID>=0&&ExtendID<=4)
	{
		list<tagGoods>* pLGoods = GetSubGoodslist(ExtendID);
		if (pLGoods==NULL)
		{
			return NULL;
		}
		list<tagGoods>::iterator it  = pLGoods->begin();
		for (;it!=pLGoods->end();it++)
		{
			if (it->lPos==lPos)
			{
				return &(*it);
			}
		}
	}
	return NULL;
}

list<tagGoods>* CPacketMgr::GetSubGoodslist(WORD index) 
{
	if (index<0||index>=COTE_NUM)
	{
		return	NULL;
	}
	return &(m_Cote[index]->m_listCoteGoods);
}

//获取原始背包指定位置的物品
tagGoods* CPacketMgr::GetGoods(WORD lPos)
{
	if (lPos>=0&&lPos<=20)
	{
		list<tagGoods>* pLGoods = GetGoodsList();
		if (pLGoods==NULL)
		{
			return NULL;
		}
		list<tagGoods>::iterator it = pLGoods->begin();
		for (;it!=pLGoods->end();it++)
		{
			if (it->lPos==lPos)
			{
				return &(*it);
			}
		}
	}
	return NULL;
}

//获取指定序号的子背包的容积
WORD CPacketMgr::GetPackageDimen(WORD index)	
{
	if( index < 0 || index >= COTE_NUM || m_Cote[index] == NULL || m_Cote[index]->pGoods == NULL)
	{ return 0; }
	else if(m_Cote[index]->pGoods != NULL)
	{ return (WORD)m_Cote[index]->pGoods->GetEffectVal(CGoods::GAP_PACK_CAPACITY,1); }				//获取道具背包的格子数

	return 0;

}

//判断是否所有的背包都是满的
bool CPacketMgr::IsAllPackageFull()
{
	for (int i = 0;i<PACK_NUM;i++)
	{
		long num = GetGoodsList(i)->size();
		long dimension = 0;
		if (i == 0)
			dimension = HOST_GRIDITEM;
		else if (GetSubPGoods(i-1))
		{
			dimension = GetPackageDimen(i-1);
		}
		if (num == dimension)
		{
			return false;
		}
	}
	return true;
}

//获取未满背包的序号（从原始背包开始计）
WORD CPacketMgr::GetNotFullPackage()
{
	for (int i= 0;i<PACK_NUM;i++)
	{
		long num = GetGoodsList(i)->size();
		long dimension = 0;
		if (i == 0)
			dimension = HOST_GRIDITEM;
		else if (GetSubPGoods(i-1))
		{
			dimension = GetPackageDimen(i-1);
		}
		if (num != dimension)
		{
			return	i;
		}
	}
	return -1;
}

//获取某个容器的物品链表
list<tagGoods>* CPacketMgr::GetContainerGoodsList(int iIndex)
{
	if( iIndex == PEI_PACKET )
	{
		return &m_listGoods;
	}
	else if( iIndex == PEI_PACK1 || iIndex == PEI_PACK2 || iIndex == PEI_PACK3 || iIndex == PEI_PACK4 || iIndex == PEI_PACK5 )
	{
		return GetSubGoodslist(iIndex - PEI_PACK1);
	}
	/*else*/
	return NULL;
}

//获取某个指定容器中空格子
DWORD CPacketMgr::GetContainerEmptyPos(int iContainerID)
{
	if( iContainerID == PEI_PACKET )
	{
		for( int i=1;i<=HOST_GRIDITEM;i++)
		{
			if ( m_setPacketEmptyPos.find(i) == m_setPacketEmptyPos.end() )
			{
				return (abs(HOST_GRIDITEM-i));
			}
		}
	}
	if( iContainerID == PEI_PACK1 || iContainerID == PEI_PACK2 || iContainerID == PEI_PACK3 ||
		iContainerID == PEI_PACK4 || iContainerID == PEI_PACK5 || iContainerID == PEI_SUBPACK)
	{
		if( iContainerID == PEI_SUBPACK)
		{
			for( int i=0;i<COTE_NUM;i++)
			{
				if( m_Cote[i] != NULL && m_Cote[i]->pGoods == NULL)
				{
					return (i + PEI_PACK1);
				}
			}
		}
		else
		{
			for( int i=0;i<PACK_KB108_CELL_NUM;i++)
			{
				if( m_Cote[iContainerID - PEI_PACK1] != NULL && m_Cote[iContainerID-PEI_PACK1]->pGoods != NULL  == FALSE)
					return i;
			}
		}
	}
	CDepotMgr CDepotObj;
	DWORD dwNum;
	dwNum=CDepotObj.GetEmptyPos(iContainerID);
	//空位
	return dwNum;
	//return m_pDepotMgr->GetEmptyPos(iContainerID);
}


//获取某个背包的物品链表（包括原始背包）
list<tagGoods>* CPacketMgr::GetGoodsList(WORD index)
{
	if (index<0||index>=PACK_NUM)
	{
		return NULL;
	}
	else if(index==0)
	{
		return &m_listGoods;
	}

	return GetSubGoodslist(index-1);
}

//获取人物指定序号的子背包
tagCote* CPacketMgr::GetCotePackage(WORD index) 
{
	if (index<0||index>=COTE_NUM)
	{
		return	NULL;
	}
	return m_Cote[index];
}										

CGoods* CPacketMgr::GetSubPGoods(WORD index)
{
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
//	子背包
//////////////////////////////////////////////////////////////////////////
void CPacketMgr::SetSubPGoods(WORD index,CGoods* pGoods)
{
	if (index>=0&&index<5&&m_Cote[index]!=NULL)
	{
		m_Cote[index]->pGoods = pGoods;
	}
}

//////////////////////////////////////////////////////////////////////////
//	跑商
//////////////////////////////////////////////////////////////////////////
tagGoods* CPacketMgr::GetBusinessGoods(WORD wPos)
{
	if (wPos>=0&&wPos<=GRIDITEM&&m_CoteBussinesPack!=NULL)
	{
		list<tagGoods>* pLGoods = &m_CoteBussinesPack->m_listCoteGoods;
		if (pLGoods==NULL)
		{
			return NULL;
		}
		list<tagGoods>::iterator it = pLGoods->begin();
		for (;it!=pLGoods->end();it++)
		{
			if (it->lPos==wPos)
			{
				return &(*it);
			}
		}
	}
	return NULL;
}

void CPacketMgr::SetBussinesPack(CGoods*pPackGoods)
{
	if (m_CoteBussinesPack!=NULL&&pPackGoods!=NULL)
	{
		ClearBussinesPackGoods();
		m_CoteBussinesPack->pGoods = pPackGoods;
	}
}

///清理已有的跑墒背包（跑墒背包的CGoods对象，背包里面的物品链表没有变化）
bool CPacketMgr::ClearBussinesPackGoods()
{
	if (m_CoteBussinesPack!=NULL&&m_CoteBussinesPack->pGoods!=NULL)
	{
		m_CoteBussinesPack->pGoods = NULL;
		return true;
	}
	return false;
}

void CPacketMgr::ClearAllGoods(long lSocketID)
{
	if( m_pFather == NULL )	return;
	CPlayer* pPlayer = m_pFather->GetOwnerPlayer();

	char strScript[64];

	if( !m_listGoods.empty())
	{
		list<tagGoods>::iterator it =  m_listGoods.begin();
		for( ;it != m_listGoods.end(); ++it)
		{
			CGoods* pGoods = it->pGoods;

			sprintf(strScript,"DelGoods(\"%s\",%d);",pGoods->GetOrginName(),it->lNum);

			CMessage *pMsg = (CMessage *)CBaseMessage::AllocMsg();
			pMsg->Reset();
			pMsg->Init(MSG_C2S_OTHER_TALK);
			pMsg->Add((BYTE)0);
			pMsg->Add((long)RUN_SCRIPT);
			pMsg->Add((long)TYPE_PLAYER);
			pMsg->Add(pPlayer->GetExID());
			pMsg->Add((long)0);
			pMsg->Add(strScript);
			pMsg->SendBySocket(lSocketID);
			CBaseMessage::FreeMsg(pMsg);
		}
	}

	//子背包
	for (int i=0;i<COTE_NUM;i++)
	{
		if( m_Cote[i] != NULL )
		{
			list<tagGoods>::iterator it = m_Cote[i]->m_listCoteGoods.begin();
			for(; it!= m_Cote[i]->m_listCoteGoods.end();++it)
			{
				CGoods* pGoods = it->pGoods;
				sprintf(strScript,"DelGoods(\"%s\",%d);",pGoods->GetOrginName(),it->lNum);
				CMessage *pMsg = (CMessage *)CBaseMessage::AllocMsg();
				pMsg->Reset();
				pMsg->Init(MSG_C2S_OTHER_TALK);
				pMsg->Add((BYTE)0);
				pMsg->Add((long)RUN_SCRIPT);
				pMsg->Add((long)TYPE_PLAYER);
				pMsg->Add(pPlayer->GetExID());
				pMsg->Add((long)0);
				pMsg->Add(strScript);
				pMsg->SendBySocket(lSocketID);
				CBaseMessage::FreeMsg(pMsg);
			}
		}
	}
}

