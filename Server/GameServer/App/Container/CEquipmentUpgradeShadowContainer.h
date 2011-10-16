// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CEQUIPMENTUPGRADESHADOWCONTAINER_423E2D75002E_INCLUDED
#define _INC_CEQUIPMENTUPGRADESHADOWCONTAINER_423E2D75002E_INCLUDED

#include "CAmountLimitGoodsShadowContainer.h"

class CBaseObject;
class CGoods;

//##ModelId=423E2D75002E
class CEquipmentUpgradeShadowContainer 
: public CAmountLimitGoodsShadowContainer
{
public:
	//##ModelId=423E3F8C007D
	enum UPGRADING_EQUIPMENT_PLACE_CELL
	{
		//##ModelId=423E3F8C0119
		UEPC_EQUIPMENT,
		//##ModelId=423E3F8C0186
		UEPC_BASE_GEM,
		//##ModelId=423E3F8C0196
		UEPC_GEM_ONE,
		//##ModelId=423E3F8C0197
		UEPC_GEM_TWO,
		//##ModelId=423E3F8C01A5
		UEPC_GEM_THREE,
	};

	//##ModelId=423E857102EE
	static DWORD s_dwGoodsLimit;

	//##ModelId=423E2DB400BB
	CEquipmentUpgradeShadowContainer();

	//##ModelId=423E2DB701B5
	//CEquipmentUpgradeShadowContainer(LONG lID);

	//##ModelId=423E2DC0034B
	~CEquipmentUpgradeShadowContainer();

	//##ModelId=423E2E5102EE
	virtual BOOL Add(CBaseObject* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid = NULL);

	//##ModelId=423E2E690119
	virtual BOOL Add(DWORD dwPosition, CGoods* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid = NULL);

	//##ModelId=423E2FCD0280
	//virtual CBaseObject* Remove(DWORD dwPosition, DWORD dwAmount, LPVOID pVoid = NULL);

	//##ModelId=423E2FDE0242
	virtual VOID Clear(LPVOID pVoid = NULL);

	//##ModelId=423E385B033C
	virtual VOID Release();

	//##ModelId=423E371001C5
	virtual BOOL IsFull();

	//##ModelId=423E374803C8
	virtual BOOL QueryGoodsPosition(CGoods* pGoods, DWORD& dwPosition);

	//##ModelId=423E37540242
	virtual BOOL QueryGoodsPosition(const CGUID& guid, DWORD& dwPosition);

	//##ModelId=423E377200EA
	virtual BOOL IsGoodsExisted(DWORD dwGoodsBasePropertiesIndex);

	//##ModelId=423E6B890119
	virtual VOID TraversingContainer(CContainerListener* pListener);

	virtual BOOL OnObjectAdded(CContainer* pContainer, CBaseObject* pObj, DWORD dwAmount, LPVOID pVoid);

	virtual BOOL OnObjectRemoved(CContainer* pContainer, CBaseObject* pObj, DWORD dwAmount, LPVOID pVoid);

	//##ModelId=423E6BB002AF
	virtual CGoods* GetTheFirstGoods(DWORD dwGoodsBasePropertiesIndex);

	//##ModelId=423E6BBA0399
	virtual CGoods* GetGoods(DWORD dwPosition);

	//##ModelId=423E6BC4002E
	virtual VOID GetGoods(DWORD dwGoodsBasePropertiesIndex, vector<CGoods*> vOut);

	//##ModelId=423E6BCB032C
	virtual DWORD GetGoodsAmount();

	////##ModelId=423E6BD7029F
	//virtual BOOL OnObjectAdded(CContainer* pContainer, CBaseObject* pObj, DWORD dwAmount, LPVOID pVoid);

	////##ModelId=423E6BDF035B
	//virtual BOOL OnObjectRemoved(CContainer* pContainer, CBaseObject* pObj, DWORD dwAmount, LPVOID pVoid);

	//##ModelId=423E6BE9001F
	BOOL Swap(UPGRADING_EQUIPMENT_PLACE_CELL uepcCell, CGoods* pIn, CGoods* * ppOut, LPVOID pVoid = NULL);

	virtual CBaseObject* Remove( CBaseObject* pObject, LPVOID pVoid = NULL );

	virtual CBaseObject* Remove( LONG oType, const CGUID& guid, LPVOID pVoid = NULL );

	virtual CBaseObject* Remove(DWORD dwPosition, DWORD dwAmount, LPVOID pVoid);

	virtual CBaseObject* Find( CBaseObject* pObj );

	virtual CBaseObject* Find( LONG oType, const CGUID& guid );

protected:
	//##ModelId=423E2E26030D
	virtual CBaseObject* Find(const CGUID& guid);

	//##ModelId=423E2FAE02DE
	virtual CBaseObject* Remove(const CGUID& guid, LPVOID pVoid = NULL);

private:
	//##ModelId=423E41880213
	map<UPGRADING_EQUIPMENT_PLACE_CELL, CGUID> m_mEquipmentPosition;
	typedef map<UPGRADING_EQUIPMENT_PLACE_CELL, CGUID>::iterator itPos;

};

#endif /* _INC_CEQUIPMENTUPGRADESHADOWCONTAINER_423E2D75002E_INCLUDED */
