// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif

#include "CAmountLimitGoodsShadowContainer.h"

class CBaseObject;
class CGoods;

class CEquipmentEnchaseShadowContainer 
: public CAmountLimitGoodsShadowContainer
{
public:	
	enum ENCHASE_EQUIPMENT_PLACE_CELL
	{	
		EEPC_EQUIPMENT,	
		EEPC_YUANBAO_ITEM1,
		EEPC_YUANBAO_ITEM2,
		EEPC_YUANBAO_ITEM3,
		EEPC_CARD1,
		EEPC_CARD2,
		EEPC_CARD3,
		EEPC_CARD4,
		EEPC_CARD5,
		EEPC_NUM
	};	
	
	
	CEquipmentEnchaseShadowContainer();	
	
	~CEquipmentEnchaseShadowContainer();
	
	virtual BOOL Add(CBaseObject* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid = NULL);
	
	virtual BOOL Add(DWORD dwPosition, CGoods* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid = NULL);	
	
	virtual VOID Clear(LPVOID pVoid = NULL);
	
	virtual VOID Release();
	
	virtual BOOL IsFull();
	
	virtual BOOL QueryGoodsPosition(CGoods* pGoods, DWORD& dwPosition);
	
	virtual BOOL QueryGoodsPosition(const CGUID& guid, DWORD& dwPosition);
	
	virtual BOOL IsGoodsExisted(DWORD dwGoodsBasePropertiesIndex);
	
	virtual VOID TraversingContainer(CContainerListener* pListener);

	virtual BOOL OnObjectAdded(CContainer* pContainer, CBaseObject* pObj, DWORD dwAmount, LPVOID pVoid);

	virtual BOOL OnObjectRemoved(CContainer* pContainer, CBaseObject* pObj, DWORD dwAmount, LPVOID pVoid);
	
	virtual CGoods* GetTheFirstGoods(DWORD dwGoodsBasePropertiesIndex);
	
	virtual CGoods* GetGoods(DWORD dwPosition);
	
	virtual VOID GetGoods(DWORD dwGoodsBasePropertiesIndex, vector<CGoods*> vOut);
	
	virtual DWORD GetGoodsAmount();
	
	BOOL Swap(ENCHASE_EQUIPMENT_PLACE_CELL uepcCell, CGoods* pIn, CGoods* * ppOut, LPVOID pVoid = NULL);

	virtual CBaseObject* Remove( CBaseObject* pObject, LPVOID pVoid = NULL );

	virtual CBaseObject* Remove( LONG oType, const CGUID& guid, LPVOID pVoid = NULL );

	virtual CBaseObject* Remove(DWORD dwPosition, DWORD dwAmount, LPVOID pVoid);

	virtual CBaseObject* Find( CBaseObject* pObj );

	virtual CBaseObject* Find( LONG oType, const CGUID& guid );

	CGoodsShadowContainer::tagGoodsShadow* GetGoodsShadowTag(CGoods* pGoods);

	BOOL RemoveShadowEx(CGUID& guid);

protected:	
	virtual CBaseObject* Find(const CGUID& guid);
	
	virtual CBaseObject* Remove(const CGUID& guid, LPVOID pVoid = NULL);

private:	
	map<ENCHASE_EQUIPMENT_PLACE_CELL, CGUID> m_mEquipmentPosition;
	typedef map<ENCHASE_EQUIPMENT_PLACE_CELL, CGUID>::iterator itPos;

};