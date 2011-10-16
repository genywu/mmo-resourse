// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CAMOUNTLIMITGOODSCONTAINER_41A6BC970128_INCLUDED
#define _INC_CAMOUNTLIMITGOODSCONTAINER_41A6BC970128_INCLUDED

#include "../Listener/CContainerListener.h"
#include "CGoodsContainer.h"

class CPlayer;
class CGoods;
class CBaseObject;

class CContainer;

//物品普通容器类，特殊的容器不能使用这个
//##ModelId=41A6BC970128
class CAmountLimitGoodsContainer 
: public CGoodsContainer
, public CContainerListener
{
public:
	//##ModelId=41BFBADA0251
	virtual bool OnObjectAdded(CContainer* pContainer, CBaseObject* pObj, DWORD dwAmount, LPVOID pVoid);

	//##ModelId=41BFBAE102CE
	virtual bool OnObjectRemoved(CContainer* pContainer, CBaseObject* pObj, DWORD dwAmount, LPVOID pVoid);

	//##ModelId=41BFA243005D
	virtual bool Add(DWORD dwPosition, CGoods* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid = NULL);

	//##ModelId=41BFA2680186
	virtual void AI();

	//##ModelId=41BFA29701D4
	virtual void SetOwner(LONG lType, const CGUID& OwnerGuid);

	//##ModelId=41BFA2E302EE
	virtual bool QueryGoodsPosition(const CGUID& guid, DWORD& dwPosition);

	//##ModelId=41A6C4F4030D
	CAmountLimitGoodsContainer();

	////##ModelId=41A6C4FD00FA
	//CAmountLimitGoodsContainer(LONG lID);

	//##ModelId=41A6C50402FD
	~CAmountLimitGoodsContainer();

	//##ModelId=41AFC3CC00E5
	virtual bool Add(CBaseObject* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid = NULL);

	//##ModelId=41AF080400BB
	virtual CBaseObject* Find(const CGUID& guid);

	CBaseObject* FindIgnoreLock(const CGUID& guid);

	//##ModelId=41AF080B038A
	virtual CBaseObject* Remove(const CGUID& guid, LPVOID pVoid = NULL);

	//##ModelId=41C0E6E40109
	virtual CBaseObject* Remove(DWORD dwPosition, DWORD dwAmount, LPVOID pVoid = NULL);

	//##ModelId=41AFC3E302AA
	virtual void Clear(LPVOID pVoid = NULL);

	//##ModelId=41AFC3F0024D
	virtual void Release();

	//##ModelId=41BFA32503C8
	virtual bool Clone(CGoodsContainer* pObject);

	//##ModelId=41B00EDE0246
	virtual void TraversingContainer(CContainerListener* pListener);

	
	//virtual DWORD GetContentsWeight();

	//##ModelId=41B021240144
	virtual CGoods* GetGoods(DWORD dwPosition);

	//##ModelId=41B690D2003E
	virtual void GetGoods(DWORD dwGoodsBasePropertiesIndex, vector<CGoods*> vOut);

	//##ModelId=41BFA2FB0290
	virtual CGoods* GetTheFirstGoods(DWORD dwGoodsBasePropertiesIndex);

	virtual CGoods* GetTheFirstGoods(DWORD dwGoodsBasePropertiesIndex, vector<CGoods*> &vNotInThis);

	//##ModelId=41BFA2DB0213
	virtual bool QueryGoodsPosition(CGoods* pGoods, DWORD& dwPosition);

	//##ModelId=41BFA2F1007D
	virtual bool IsGoodsExisted(DWORD dwGoodsBasePropertiesIndex);

	//被锁定的物品，将不能再被移出，被查找。直到解除锁定。
	//##ModelId=41B7E36D03D7
	virtual bool Lock(CGoods* pGoods);

	//##ModelId=41B7E3870165
	virtual bool Unlock(CGoods* pGoods);

	//##ModelId=41BFA280004E
	virtual bool IsFull();

	//##ModelId=41A6C5350213
	void SetGoodsAmountLimit(DWORD dwMaxNumber);

	//##ModelId=41B020E001B1
	DWORD GetGoodsAmountLimit();

	//##ModelId=41BFEAE30000
	virtual DWORD GetGoodsAmount();

	virtual DWORD GetGoodsAmountByName(const char* strName);
	virtual DWORD GetGoodsAmountByOriginName(const char* strOriginName);

	virtual DWORD GetGoodsAmountByType(int nType);

	virtual CBaseObject*	Remove					( CBaseObject* pObject, LPVOID pVoid = NULL );

	virtual CBaseObject*	Remove					( LONG oType, const CGUID& guid, LPVOID pVoid = NULL );

	virtual CBaseObject*	Find					( CBaseObject* pObj );

	virtual CBaseObject*	Find					( LONG oType, const CGUID& guid );

	virtual bool			Serialize				( DBWriteSet& setWriteDB, bool b = true );
	virtual bool			Unserialize				( DBReadSet& setReadDB, bool b = true );

	hash_map<CGUID,CGoods*,hash_guid_compare>* GetGoodsMap() {return    &m_mGoods;}



	//##ModelId=41B7F790000E
	virtual bool IsLocked(CGoods* pGoods);
protected:
	//##ModelId=41A6BD0A035B
	//map<DWORD, CGoods*> m_mGoods;
	hash_map<CGUID,CGoods*,hash_guid_compare>	m_mGoods;
public:
	typedef hash_map<CGUID,CGoods*,hash_guid_compare>::iterator goodsIt;

private:
	//##ModelId=41A6C2DA0232
	DWORD m_dwGoodsAmountLimit;

	//##ModelId=41B7F7D600FA
	vector<CGUID> m_vLockedGoods;
	typedef vector<CGUID>::iterator itLockGoods;
};

#endif /* _INC_CAMOUNTLIMITGOODSCONTAINER_41A6BC970128_INCLUDED */
