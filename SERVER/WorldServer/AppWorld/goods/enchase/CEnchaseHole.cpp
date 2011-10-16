#include "stdafx.h"
#include "cenchasehole.h"
#include "../cgoods.h"
#include "../CGoodsFactory.h"

CEnchaseHole::CEnchaseHole()
{	
}

CEnchaseHole::CEnchaseHole(CGoods *pParent)
{
	m_pParent=pParent;
	m_bIsActived=FALSE;
	dwCardGoodsId=0;
	m_pCard=NULL;
}

CEnchaseHole::~CEnchaseHole()
{	
	CGoodsFactory::GarbageCollect(&m_pCard);
}

BOOL CEnchaseHole::CanEnchase(CGoods* pGoods,int nIndex)
{
	return FALSE;
}

BOOL CEnchaseHole::EnchaseCard(CGoods* pGoods,int nIndex)
{
	return FALSE;
}

BOOL CEnchaseHole::RemoveCard(int nIndex)
{
	return FALSE;
}