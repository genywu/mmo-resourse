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
	//王晓轩：所有孔槽都默认为开启状态	
	m_bIsActived = TRUE;
	m_pCard=NULL;
}

CEnchaseHole::~CEnchaseHole()
{	
	CGoodsFactory::GarbageCollect(&m_pCard,41);
}

BOOL CEnchaseHole::CanEnchase(CGoods* pGoods)
{
	return FALSE;
}

BOOL CEnchaseHole::EnchaseCard(CPlayer* pPlayer,CGoods* pCard)
{
	//王晓轩：所有孔槽都默认为开启状态，所以删除此判断	
	if ( m_pCard )
		return FALSE;	
	
	//创建与所要镶嵌卡属性相同的卡片，解决卡片叠加造成的物品指针被删除问题
	if(pCard->GetAmount()==1)
		m_pCard=pCard;
	else
		m_pCard = CGoodsFactory::CreateGoods( pCard->GetBasePropertiesIndex(), 55 );
	
	m_pParent->EnchaseAttrUpdate(pPlayer,m_pCard);
	return TRUE;
}

BOOL CEnchaseHole::RemoveCard(CPlayer* pPlayer,bool bReserve)
{
	//王晓轩：所有孔槽都默认为开启状态，所以删除此判断
	if ( !m_pCard )	
		return FALSE;
	m_pParent->EnchaseAttrRemove(pPlayer,m_pCard);
	if(bReserve)
	{
		m_pCard=NULL;
	}
	else
	{
		CGoodsFactory::GarbageCollect(&m_pCard,41);	
	}	
	return TRUE;
}