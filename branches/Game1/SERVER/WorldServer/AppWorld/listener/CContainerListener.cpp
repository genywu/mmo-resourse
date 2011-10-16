// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "stdafx.h"
#include "..\Container\CContainer.h"
#include "..\BaseObject.h"
#include "CContainerListener.h"



//--------------------------------------------------------------------------------------
// 函数:	容器添加一个对象
// 参数:	无
// 返回值:	无
// 描述：	返回TRUE则继续往下派法执行,FALSE则终止派法
//--------------------------------------------------------------------------------------
//##ModelId=41A4308B01B5
BOOL CContainerListener::OnObjectAdded(CContainer* pContainer, CBaseObject* pObj, LPVOID pVoid)
{
	return TRUE;
}

//--------------------------------------------------------------------------------------
// 函数:	容器删除一个对象
// 参数:	无
// 返回值:	无
// 描述：	返回TRUE则继续往下派法执行,FALSE则终止派法
//--------------------------------------------------------------------------------------
//##ModelId=41A430B10232
BOOL CContainerListener::OnObjectRemoved(CContainer* pContainer, CBaseObject* pObj, LPVOID pVoid)
{
	return TRUE;
}

//--------------------------------------------------------------------------------------
// 函数:	容器遍历一个对象
// 参数:	无
// 返回值:	无
// 描述：	返回TRUE则继续往下派法执行,FALSE则终止派法
//--------------------------------------------------------------------------------------
//##ModelId=41A43146003E
BOOL CContainerListener::OnTraversingContainer(CContainer* pContainer, CBaseObject* pObject)
{
	return TRUE;
}

