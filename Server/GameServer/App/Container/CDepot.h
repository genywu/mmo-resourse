// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CDEPOT_41DA1CCD03D8_INCLUDED
#define _INC_CDEPOT_41DA1CCD03D8_INCLUDED

#include "CVolumeLimitGoodsContainer.h"

class CBaseObject;
class CGoods;

//##ModelId=41DA1CCD03D8
class CDepot 
: public CVolumeLimitGoodsContainer
{
public:
	//##ModelId=41DA1CEF035B
	CDepot();

	//##ModelId=41DA1CF3007D
	//CDepot(LONG lID);

	//##ModelId=41DA1CF700CB
	~CDepot();

	//##ModelId=41DA1D0902AF
	virtual VOID Clear(LPVOID pVoid = NULL);

	//##ModelId=41DA1D1E0119
	virtual VOID Release();

	//##ModelId=41DA1D520148
	virtual BOOL Add(CBaseObject* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid = NULL);

	//##ModelId=41DA1D5A0290
	virtual BOOL Add(DWORD dwPosition, CGoods* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid = NULL);

	//##ModelId=41DA1D6C01B5
	BOOL Lock();

	//##ModelId=41DA1D820177
	BOOL Unlock(CHAR* strKey);

	virtual BOOL Unserialize( BYTE* pStream, LONG& lOffset, BOOL b = TRUE );

	BOOL IsDepotLocked();

protected:
	//##ModelId=41DA1D3501E4
	virtual CBaseObject* Find(const CGUID& guid);

	//##ModelId=41DA1D4401F4
	virtual CBaseObject* Remove(const CGUID& guid, LPVOID pVoid = NULL);

private:
	//##ModelId=41DA1CE00222
	BOOL m_bIsLocked;

};

#endif /* _INC_CDEPOT_41DA1CCD03D8_INCLUDED */
