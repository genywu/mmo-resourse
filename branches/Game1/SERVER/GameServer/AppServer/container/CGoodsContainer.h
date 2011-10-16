// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CGOODSCONTAINER_41BF90730000_INCLUDED
#define _INC_CGOODSCONTAINER_41BF90730000_INCLUDED

#include "CContainer.h"

class CBaseObject;
class CPlayer;
class CGoods;
class CContainerListener;
class CTrader;
class CPersonalShopSeller;


//##ModelId=41BF90730000
class CGoodsContainer 
: public CContainer
{
public:

	//##ModelId=41FEF83B0207
	enum GOODS_CONTAINER_MODE
	{
		//##ModelId=41FEF83B0209
		GCM_NORMAL,
		//##ModelId=41FEF83B0217
		GCM_TEST,
	};

	//##ModelId=41BF91DC003E
	CGoodsContainer();

	//##ModelId=41BF91EE001F
	//CGoodsContainer(LONG lID);

	//##ModelId=41FEF83B00B0
	VOID SetContainerMode(GOODS_CONTAINER_MODE gcm);

	//##ModelId=41FEF8A30033
	GOODS_CONTAINER_MODE GetContainerMode();

	//##ModelId=41BF91E001E4
	virtual ~CGoodsContainer();

	//##ModelId=41BF92970167
	virtual BOOL Add(CBaseObject* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid = NULL);

	//##ModelId=41BF96FB01C5
	virtual BOOL Add(DWORD dwPosition, CGoods* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid = NULL);

	//##ModelId=41BF928C00FA
	virtual CBaseObject* Find(const CGUID& guid);

	//##ModelId=41BF92B5030D
	virtual CBaseObject* Remove(const CGUID& guid, LPVOID pVoid = NULL);

	//##ModelId=41C0E6B9032C
	virtual CBaseObject* Remove(DWORD dwPosition, DWORD dwAmount, LPVOID pVoid = NULL);

	//##ModelId=41BF92C1002E
	virtual VOID Clear(LPVOID pVoid = NULL);

	//##ModelId=41BF92C700EA
	virtual VOID Release();

	//##ModelId=41BF956500FA
	virtual VOID AI();

	//##ModelId=41BF992A001F
	virtual BOOL IsFull();

	//##ModelId=41BF92ED034B
	virtual VOID TraversingContainer(CContainerListener* pListener) = 0;

	//##ModelId=41BF92F70138
	//virtual DWORD GetContentsWeight() = 0;

	//##ModelId=41BF93370203
	virtual BOOL QueryGoodsPosition(CGoods* pGoods, DWORD& dwPosition) = 0;

	//##ModelId=41BF94DA037A
	virtual BOOL QueryGoodsPosition(const CGUID& guid, DWORD& dwPosition) = 0;

	//##ModelId=41BF97830148
	virtual BOOL IsGoodsExisted(DWORD dwGoodsBasePropertiesIndex) = 0;

	//##ModelId=41BF97B20290
	virtual CGoods* GetTheFirstGoods(DWORD dwGoodsBasePropertiesIndex) = 0;

	//##ModelId=41BF97CA02AF
	virtual CGoods* GetGoods(DWORD dwPosition) = 0;

	//##ModelId=41BF97E202DE
	virtual VOID GetGoods(DWORD dwGoodsBasePropertiesIndex, vector<CGoods*> vOut) = 0;

	virtual BOOL CanPutIn(CGoods* pGoods,CPlayer* pPlayer);

	//##ModelId=41BFEAA30157
	virtual DWORD GetGoodsAmount() = 0;
	virtual DWORD GetGoodsAmountByName(const char* strName)=0;
	virtual DWORD GetGoodsAmountByType(int nType)=0;

	LONG GetOwnerType();

	const CGUID& GetOwnerID();

	virtual CBaseObject*	Remove					( CBaseObject* pObject, LPVOID pVoid = NULL );

	virtual CBaseObject*	Remove					( LONG oType, const CGUID& guid, LPVOID pVoid = NULL );

	virtual CBaseObject*	Find					( CBaseObject* pObj );

	virtual CBaseObject*	Find					( LONG oType, const CGUID& guid );

	virtual BOOL			Serialize				( DBWriteSet& setWriteDB, BOOL b = TRUE ) = 0;
	virtual BOOL			Unserialize				( DBReadSet& setReadDB, BOOL b = TRUE ) = 0;

protected:
	
	friend class CPlayer;
	friend class CTrader;
	friend class CPersonalShopSeller;	
	friend class CEquipmentEnchase;
	friend class PlayerDepot;

	//##ModelId=41BF9944009C
	virtual VOID SetOwner(LONG lType, const CGUID& OwnerGuid);

private:
	//##ModelId=41BFA05B0109
	LONG m_lOwnerType;

	//##ModelId=41BFA06501B5
	CGUID m_OwnerGuid;

	//##ModelId=41FEF8A30081
	GOODS_CONTAINER_MODE m_gcmCurrentGoodsContainerMode;
};

#endif /* _INC_CGOODSCONTAINER_41BF90730000_INCLUDED */
