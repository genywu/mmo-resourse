#include "StdAfx.h"
#include "EnchaseHole.h"

#include "../Goods.h"
#include "../GoodsFactory.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
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