


#include "stdafx.h"
#include "EquipMgr.h"
#include "Goods.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CEquipMgr::CEquipMgr()
{
	for(int i=0;i<EQUIP_SUM;i++)
	{
		m_Equip[i] = NULL;
		m_dwEquipIndex[i] = 0;
		m_btEquipLevel[i] = 0;
	}
}

CEquipMgr::~CEquipMgr()
{
	for(int i=0;i<EQUIP_SUM;i++)
	{
		SAFE_DELETE(m_Equip[i]);
	}
}

long CEquipMgr::GetEquipAmount()	
{
	long l = 0;
	for(int i=0; i!=EQUIP_SUM; i++)
	{
		if( m_Equip[i] != NULL )
			l++;
	}
	return l;
}


CGoods* CEquipMgr::GetEquip(int index)
{
	if(index<0 || index>EQUIP_SUM - 1)
		return NULL;
	return m_Equip[index];
}

bool CEquipMgr::GetWeapon(const char* strOrgName)
{
	if( m_Equip[EQUIP_WEAPON] != NULL && !strcmp( m_Equip[EQUIP_WEAPON]->GetOrginName(),strOrgName))
		return true;
	else
		return false;
}


void CEquipMgr::SetEquip(int index,CGoods *pGood)
{
	if(index<0 || index>EQUIP_SUM - 1)
		return ;
	m_Equip[index]=pGood;
}

void CEquipMgr::SetEquipIndex(long lIndex,DWORD dwIndex)
{
	if(lIndex>=0 && lIndex<EQUIP_SUM)
	{
		m_dwEquipIndex[lIndex]=dwIndex;
	}
}

void CEquipMgr::SetEquipLevel(long lIndex,BYTE level)
{
	if(lIndex>=0 && lIndex<EQUIP_SUM)
	{
		m_btEquipLevel[lIndex]=level;
	}
}

void CEquipMgr::DecodeFromDataBlock(DBReadSet& dbRead,bool bExData)
{
	bool bVal = false;
	long lNum = dbRead.GetWordFromByteArray();
	for(int i=0;i<lNum;i++)
	{
		CGoods* pGoods = new CGoods;
		bVal = pGoods->DecordFromDataBlock(dbRead,bExData);
		WORD wIndex = dbRead.GetWordFromByteArray();
		if(bVal&&wIndex>=0&&wIndex<EQUIP_SUM)
		{
			m_Equip[wIndex] = pGoods;
			pGoods = NULL;
		}
		else
		{
			SAFE_DELETE(pGoods);
		}
	}
}

void CEquipMgr::DecodeFromDataBlockEx(DBReadSet& readDB)
{
	// 装备编号
	// -- 左边装备栏位
	m_dwEquipIndex[EQUIP_HEAD] = readDB.GetDwordFromByteArray();
	m_dwEquipIndex[EQUIP_NECKLACE] = readDB.GetDwordFromByteArray();
	m_dwEquipIndex[EQUIP_WING] = readDB.GetDwordFromByteArray();
	m_dwEquipIndex[EQUIP_BODY] = readDB.GetDwordFromByteArray();
	m_dwEquipIndex[EQUIP_WAISTBAND] = readDB.GetDwordFromByteArray();
	m_dwEquipIndex[EQUIP_GLOVE] = readDB.GetDwordFromByteArray();
	m_dwEquipIndex[EQUIP_SHOES] = readDB.GetDwordFromByteArray();

	// -- 右边装备栏位
	m_dwEquipIndex[EQUIP_HEADGEAR] = readDB.GetDwordFromByteArray();
	m_dwEquipIndex[EQUIP_FROCK] =  readDB.GetDwordFromByteArray();
	m_dwEquipIndex[EQUIP_LRING] =  readDB.GetDwordFromByteArray();
	m_dwEquipIndex[EQUIP_RRING] =  readDB.GetDwordFromByteArray();
	m_dwEquipIndex[EQUIP_MEDAL1] =  readDB.GetDwordFromByteArray();		
	m_dwEquipIndex[EQUIP_MEDAL2] = readDB.GetDwordFromByteArray();
	m_dwEquipIndex[EQUIP_MEDAL3] = readDB.GetDwordFromByteArray();

	// 下边装备栏位
	m_dwEquipIndex[EQUIP_FAIRY] =  readDB.GetDwordFromByteArray();		
	m_dwEquipIndex[EQUIP_WEAPON] = readDB.GetDwordFromByteArray();
	m_dwEquipIndex[EQUIP_WEAPON2] = readDB.GetDwordFromByteArray();
	m_dwEquipIndex[EQUIP_HORSE] = readDB.GetDwordFromByteArray();
	m_dwEquipIndex[EQUIP_PIFENG] = readDB.GetDwordFromByteArray();

	// 装备等级
	// -- 左边装备栏位
	m_btEquipLevel[EQUIP_HEAD] = readDB.GetByteFromByteArray();
	m_btEquipLevel[EQUIP_NECKLACE] = readDB.GetByteFromByteArray();
	m_btEquipLevel[EQUIP_WING] = readDB.GetByteFromByteArray();
	m_btEquipLevel[EQUIP_BODY] = readDB.GetByteFromByteArray();
	m_btEquipLevel[EQUIP_WAISTBAND] = readDB.GetByteFromByteArray();
	m_btEquipLevel[EQUIP_GLOVE] = readDB.GetByteFromByteArray();
	m_btEquipLevel[EQUIP_SHOES] = readDB.GetByteFromByteArray();

	// -- 右边装备栏位
	m_btEquipLevel[EQUIP_HEADGEAR] = readDB.GetByteFromByteArray();
	m_btEquipLevel[EQUIP_FROCK] = readDB.GetByteFromByteArray();
	m_btEquipLevel[EQUIP_LRING] = readDB.GetByteFromByteArray();
	m_btEquipLevel[EQUIP_RRING] = readDB.GetByteFromByteArray();
	m_btEquipLevel[EQUIP_MEDAL1] = readDB.GetByteFromByteArray();
	m_btEquipLevel[EQUIP_MEDAL2] = readDB.GetByteFromByteArray();
	m_btEquipLevel[EQUIP_MEDAL3] = readDB.GetByteFromByteArray();


	// -- 下边装备栏位
	m_btEquipLevel[EQUIP_FAIRY] = readDB.GetByteFromByteArray();
	m_btEquipLevel[EQUIP_WEAPON] = readDB.GetByteFromByteArray();
	m_btEquipLevel[EQUIP_WEAPON2] = readDB.GetByteFromByteArray();
	m_btEquipLevel[EQUIP_HORSE] = readDB.GetByteFromByteArray();
	m_btEquipLevel[EQUIP_PIFENG] = readDB.GetByteFromByteArray();
}

CGoods* CEquipMgr::GetGoodsByGUID(const CGUID& guID,int *pPos,long* lExtendID)
{
	for(int i=0;i<EQUIP_SUM;i++)
	{
		CGoods* pGoods = GetEquip(i);
		if(pGoods)
		{
			if(pGoods->GetExID() == guID)
			{
				if( pPos ) 
					pPos[0] = i;
				if( lExtendID != NULL )
					(*lExtendID) = PEI_EQUIPMENT;
				return pGoods;
			}
		}
	}
	return NULL;
}

WORD CEquipMgr::GetGoodsContainerID(CGoods* pGoods)
{
	if(pGoods!=NULL)
	{
		for(int i=0;i<EQUIP_SUM;i++)
		{
			CGoods* pEquip = GetEquip(i);
			if(pEquip)
			{
				if(pEquip->GetExID() == pGoods->GetExID())
				{
					return (WORD)PEI_EQUIPMENT;
				}
			}
		}
	}
	return -1;
}