// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CPERSONALSHOPBUYER_422677D80177_INCLUDED
#define _INC_CPERSONALSHOPBUYER_422677D80177_INCLUDED

#include "CPlug.h"

class CContainer;
class CPersonalShopSeller;

//##ModelId=422677D80177
class CPersonalShopBuyer 
: public CPlug
{
public:
	struct tagBuyInfo
	{
		LONG lId;
		CGUID guid;
		DWORD dwNum;
	};
public:
	//##ModelId=422677F80157
	CPersonalShopBuyer();

	//##ModelId=422677FE0271
	//CPersonalShopBuyer(LONG lID);

	//##ModelId=422677FB0399
	~CPersonalShopBuyer();

	//##ModelId=4226782B007D
	virtual BOOL IsPlugAvailable();

	//##ModelId=4226783100AB
	virtual BOOL OnSessionEnded();

	//##ModelId=42267837003E
	virtual BOOL OnSessionAborted();

	//##ModelId=4226783E032C
	virtual BOOL OnPlugInserted();

	//##ModelId=4226784402FD
	virtual BOOL OnPlugAborted();

	//##ModelId=4226784C038A
	virtual BOOL OnPlugEnded();

	//##ModelId=4226785503A9
	virtual BOOL OnChangeState(LONG lPlugID, LONG lStateID, BYTE* pData);

	//##ModelId=4226785D0167
	virtual VOID Release();

	//这个函数返回卖东西的角色的影子容器
	//##ModelId=422678630222
	virtual CContainer* GetContainer(LONG lID);

	//##ModelId=422E4F910119
	CPersonalShopSeller* GetSeller();

};

#endif /* _INC_CPERSONALSHOPBUYER_422677D80177_INCLUDED */
