// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CAMOUNTLIMITGOODSSHADOWCONTAINER_41E61AE40119_INCLUDED
#define _INC_CAMOUNTLIMITGOODSSHADOWCONTAINER_41E61AE40119_INCLUDED

#include "CGoodsShadowContainer.h"

class CBaseObject;
class CGoods;

//##ModelId=41E61AE40119
class CAmountLimitGoodsShadowContainer 
: public CGoodsShadowContainer
{
public:
	//##ModelId=41EF273401C5
	CAmountLimitGoodsShadowContainer();

	//##ModelId=41EF275103A9
	//CAmountLimitGoodsShadowContainer(LONG lID);

	//##ModelId=41EF27470271
	~CAmountLimitGoodsShadowContainer();

	//##ModelId=41EF27430177
	virtual bool Add(CBaseObject* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid = NULL);

	//##ModelId=41EF27670186
	virtual bool Add(DWORD dwPosition, CGoods* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid = NULL);

	//##ModelId=41EF278D02CE
	virtual void Clear(LPVOID pVoid = NULL);

	//##ModelId=41EF2794034B
	virtual void Release();

	//##ModelId=41EF27A0002E
	virtual bool IsFull();

	//##ModelId=41EF56D402AF
	virtual bool Clone(CBaseObject* pObject);

	//##ModelId=41EF3A130213
	void SetGoodsAmountLimit(DWORD dwMaxNumber);

	//##ModelId=41EF3A1A0242
	DWORD GetGoodsAmountLimit();

private:
	//##ModelId=41EF26F900BB
	DWORD m_dwMaxGoodsAmount;

};

#endif /* _INC_CAMOUNTLIMITGOODSSHADOWCONTAINER_41E61AE40119_INCLUDED */
