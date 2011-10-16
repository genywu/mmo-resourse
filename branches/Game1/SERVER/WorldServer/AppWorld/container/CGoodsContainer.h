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

//##ModelId=41BF90730000
class CGoodsContainer 
: public CContainer
{
private:
	//##ModelId=41BFA05B0109
	LONG m_lOwnerType;

	//##ModelId=41BFA06501B5
	LONG m_lOwnerID;

public:
	//##ModelId=41BF91DC003E
	CGoodsContainer();

	//##ModelId=41BF91EE001F
	//CGoodsContainer(LONG lID);

	//##ModelId=41BF91E001E4
	virtual ~CGoodsContainer();

	//##ModelId=41BF92970167
	virtual BOOL Add(CBaseObject* pObject, LPVOID pVoid = NULL);

	//##ModelId=41BF96FB01C5
	virtual BOOL Add(DWORD dwPosition, CGoods* pObject, LPVOID pVoid = NULL);

	virtual BOOL AddFromDB ( CGoods* pObject, DWORD dwPosition = 0 );
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
	virtual DWORD GetContentsWeight() = 0;

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

	//##ModelId=41BFEAA30157
	virtual DWORD GetGoodsAmount() = 0;

	LONG GetOwnerType();

	LONG GetOwnerID();

	virtual CBaseObject*	Remove					( CBaseObject* pObject, LPVOID pVoid = NULL );

	virtual CBaseObject*	Remove					( LONG oType, const CGUID& guid, LPVOID pVoid = NULL );

	virtual CBaseObject*	Find					( CBaseObject* pObj );

	virtual CBaseObject*	Find					( LONG oType, const CGUID& guid );

	virtual BOOL			Serialize				( vector<BYTE>* pStream, BOOL b = TRUE ) = 0;

	virtual BOOL			Unserialize				( BYTE* pStream, LONG& lOffset, BOOL b = TRUE ) = 0;

	virtual BOOL			Serialize				( DBWriteSet& setWriteDB, BOOL b = TRUE ) = 0;

	virtual BOOL			Unserialize				( DBReadSet& setReadDB, BOOL b = TRUE ) = 0;

protected:
	
	friend class CPlayer;

	//##ModelId=41BF9944009C
	virtual VOID SetOwner(LONG lType, LONG lID);
};

#endif /* _INC_CGOODSCONTAINER_41BF90730000_INCLUDED */
