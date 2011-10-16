// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CBANK_41DA01D103D8_INCLUDED
#define _INC_CBANK_41DA01D103D8_INCLUDED

#include "Wallet.h"

class CBaseObject;
class CGoods;

//##ModelId=41DA01D103D8
class CBank 
: public CWallet
{
private:
	//##ModelId=41DA0366033C
	BOOL m_bLocked;

public:
	//##ModelId=41DA0384001F
	virtual BOOL Add(CBaseObject* pObject, LPVOID pVoid = NULL);

	//##ModelId=41DA038C00DA
	virtual BOOL Add(DWORD dwPosition, CGoods* pObject, LPVOID pVoid = NULL);

	virtual BOOL AddFromDB( CGoods* pObject, DWORD dwPosition  = 0 );
	//##ModelId=41DA039601B5
	BOOL Lock();

	//##ModelId=41DA039E00EA
	BOOL Unlock(CHAR* strKey);

	//##ModelId=41DA01E80186
	CBank();

	//##ModelId=41DA01FD00DA
	//CBank(LONG lID);

	//##ModelId=41DA01ED0280
	~CBank();

	//##ModelId=41DA09B702FD
	virtual VOID Clear(LPVOID pVoid = NULL);

	//##ModelId=41DA099C01A5
	virtual VOID Release();

protected:
	//##ModelId=41DA0217029F
	virtual CBaseObject* Find(const CGUID& guid);

	//##ModelId=41DA022600EA
	virtual CBaseObject* Remove(const CGUID& guid, LPVOID pVoid = NULL);

};

#endif /* _INC_CBANK_41DA01D103D8_INCLUDED */
