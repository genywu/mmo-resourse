#include "StdAfx.h"
#include "cenchasehole.h"
#include "../CGoods.h"
#include "../CGoodsFactory.h"
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
	//王晓轩：所有孔槽都默认为开启状态
	//m_bIsActived=false;
	m_bIsActived = true;
	m_pCard=NULL;
}

CEnchaseHole::~CEnchaseHole()
{	
	CGoodsFactory::GarbageCollect(&m_pCard,41);
}

bool CEnchaseHole::CanEnchase(CGoods* pGoods)
{
	return false;
}

bool CEnchaseHole::EnchaseCard(CPlayer* pPlayer,CGoods* pCard)
{
	//王晓轩：所有孔槽都默认为开启状态，所以删除此判断
	//if(m_pCard || !m_bIsAc tived)
	if ( m_pCard )
		return false;
	//m_pCard=pCard;
	
	//创建与所要镶嵌卡属性相同的卡片，解决卡片叠加造成的物品指针被删除问题
	if(pCard->GetAmount()==1)
		m_pCard=pCard;
	else
		m_pCard = CGoodsFactory::CreateGoods( pCard->GetBasePropertiesIndex(), 55 );
	//CGoodsFactory::GarbageCollect(&pCard,41);
	//m_pParent->EnchaseAttrUpdate(pPlayer,pCard);
	m_pParent->EnchaseAttrUpdate(pPlayer,m_pCard);
	return true;
}

bool CEnchaseHole::RemoveCard(CPlayer* pPlayer,bool bReserve)
{
	//王晓轩：所有孔槽都默认为开启状态，所以删除此判断
	if ( !m_pCard )
	//if(!m_bIsActived || !m_pCard)
		return false;
	m_pParent->EnchaseAttrRemove(pPlayer,m_pCard);
	if(bReserve)
	{
		m_pCard=NULL;
	}
	else
	{
		CGoodsFactory::GarbageCollect(&m_pCard,41);	
	}	
	return true;
}