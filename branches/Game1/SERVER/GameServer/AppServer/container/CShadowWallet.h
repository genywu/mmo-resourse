// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CSHADOWWALLET_41F0702C0008_INCLUDED
#define _INC_CSHADOWWALLET_41F0702C0008_INCLUDED

#include "CAmountLimitGoodsShadowContainer.h"

class CBaseObject;
class CGoods;

class CContainer;

//##ModelId=41F0702C0008
class CShadowWallet 
: public CAmountLimitGoodsShadowContainer
{
public:
	//##ModelId=41F071F30047
	virtual BOOL OnObjectAdded(CContainer* pContainer, CBaseObject* pObj, DWORD dwAmount, LPVOID pVoid);

	//##ModelId=41F071FB01EC
	virtual BOOL OnObjectRemoved(CContainer* pContainer, CBaseObject* pObj, DWORD dwAmount, LPVOID pVoid);

	//##ModelId=41F0713100D0
	CShadowWallet();

	//##ModelId=41F0713A0293
	//CShadowWallet(LONG lID);

	//##ModelId=41F0713F035D
	~CShadowWallet();

	//##ModelId=41F0714A0195
	virtual BOOL Add(CBaseObject* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid = NULL);

	//##ModelId=41F0715201E2
	virtual BOOL Add(DWORD dwPosition, CGoods* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid = NULL);

	//##ModelId=41F5EBAD0188
	DWORD GetGoldCoinsAmount();
};

#endif /* _INC_CSHADOWWALLET_41F0702C0008_INCLUDED */
