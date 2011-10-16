// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "stdafx.h"
#include "CEquipmentEnchase.h"
#include "..\Player.h"
#include "..\ServerRegion.h"
#include "..\Listener\CUpgradePriceListener.h"
#include "..\Goods\CGoodsFactory.h"
#include "..\..\..\setup\globesetup.h"
#include "../../../setup/LogSystem.h"



CEquipmentEnchase::CEquipmentEnchase()
{	
}

CEquipmentEnchase::~CEquipmentEnchase()
{	
	Release();
}

BOOL CEquipmentEnchase::IsPlugAvailable()
{
	return TRUE;
}

BOOL CEquipmentEnchase::OnSessionEnded()
{
	return OnPlugEnded();
}

BOOL CEquipmentEnchase::OnSessionAborted()
{
	return OnPlugEnded();
}

BOOL CEquipmentEnchase::OnPlugInserted()
{	
	
	//设置容器SESSION相关
	m_cEnchaseContainer.SetOwner( TYPE_SESSION, m_SessionID );
	m_cEnchaseContainer.SetContainerExtendID( Get32ID() << 8 );
	return TRUE;
}

BOOL CEquipmentEnchase::OnPlugAborted()
{
	return OnPlugEnded();
}

BOOL CEquipmentEnchase::OnPlugEnded()
{
	//##直接发送关闭界面的消息给客户端。
	//##把监听器加入到玩家的容器内。

	CPlayer* pPlayer = dynamic_cast<CPlayer*>( GetOwner() );
	if( pPlayer )
	{
		pPlayer ->getOriginPack()->RemoveListener( &m_cEnchaseContainer );
		pPlayer -> m_cEquipment.RemoveListener( &m_cEnchaseContainer );
	}		
	return TRUE;
}

BOOL CEquipmentEnchase::OnChangeState(LONG lPlugID, LONG lStateID, BYTE* pData)
{
	return TRUE;
}

VOID CEquipmentEnchase::Release()
{
}

CContainer* CEquipmentEnchase::GetContainer(LONG lID)
{
	// TODO: Add your specialized code here.
	CContainer* pResult = NULL;
	if( lID == 0 )
	{
		pResult = &m_cEnchaseContainer;
	}
	return pResult;
}

DWORD CEquipmentEnchase::GetSucceedResult()
{	
	return 0;
}

DWORD CEquipmentEnchase::GetFailedResult()
{
	return 0;
}

DWORD CEquipmentEnchase::GetProbability()
{
	return 0;
}

DWORD CEquipmentEnchase::GetUpgradePrice()
{		
	return 0;
}

BOOL CEquipmentEnchase::DoesUpgradeValid()
{
	return TRUE;
}

BOOL CEquipmentEnchase::Enchase()
{
	BOOL bResult = FALSE;	
	return bResult;
}

BOOL CEquipmentEnchase::DeleteGoods(CEquipmentEnchaseShadowContainer::ENCHASE_EQUIPMENT_PLACE_CELL uepc)
{
	BOOL bResult = FALSE;	
	return bResult;
}

CGoods* CEquipmentEnchase::GetGoodsByPos(DWORD dwPos)
{
	return m_cEnchaseContainer.GetGoods(dwPos);
}
