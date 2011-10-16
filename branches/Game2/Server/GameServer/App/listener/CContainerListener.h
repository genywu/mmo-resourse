// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CCONTAINERLISTENER_41A43051038A_INCLUDED
#define _INC_CCONTAINERLISTENER_41A43051038A_INCLUDED


class CContainer;
class CBaseObject;

//##ModelId=41A43051038A
class CContainerListener 
{
public:
	//##ModelId=41A4308B01B5
	virtual bool OnObjectAdded(CContainer* pContainer, CBaseObject* pObj, DWORD dwAmount, LPVOID pVoid);

	//##ModelId=41A430B10232
	virtual bool OnObjectRemoved(CContainer* pContainer, CBaseObject* pObj, DWORD dwAmount, LPVOID pVoid);

	//##ModelId=41A43146003E
	virtual bool OnTraversingContainer(CContainer* pContainer, CBaseObject* pObject);

};

#endif /* _INC_CCONTAINERLISTENER_41A43051038A_INCLUDED */
