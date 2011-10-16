// Copyright (C) 1991 - 1999 Rational Software Corporation
#include "StdAfx.h"
#include "Container.h"
#include "../BaseObject.h"
#include "../Listener/ContainerListener.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//--------------------------------------------------------------------------------------
// 函数:	CContainer构造函数
// 参数:	无
// 返回值:	无
// 描述：	无
//--------------------------------------------------------------------------------------
//##ModelId=41A17B98003E
CContainer::CContainer()// : CBaseObject( TYPE_CONTAINER, 0 )
{
	RemoveAllListener();
}

//--------------------------------------------------------------------------------------
// 函数:	CContainer析构函数
// 参数:	无
// 返回值:	无
// 描述：	无
//--------------------------------------------------------------------------------------
//##ModelId=41A17F83000F
CContainer::~CContainer()
{
	RemoveAllListener();
	Release();
}

//--------------------------------------------------------------------------------------
// 函数:	CContainer构造函数
// 参数:	编号 : LONG
// 返回值:	无
// 描述：	无
//--------------------------------------------------------------------------------------
//##ModelId=41A1B5160290
//CContainer::CContainer( LONG lID ) : CBaseObject( TYPE_CONTAINER, lID )
//{
//	RemoveAllListener();
//}

//--------------------------------------------------------------------------------------
// 函数:	CalculateHashValue，计算哈西值的方法。
// 参数:	类型 : OBJECT_TYPE，编号 : LONG
// 返回值:	DWORD
// 描述：	返回计算的哈西值
//--------------------------------------------------------------------------------------
//##ModelId=41A1CD27037A
//DWORD CContainer::CalculateHashValue( /*OBJECT_TYPE*/LONG oType, LONG lID )
//{
//	DWORD dwHashValue = 0;
//	if( oType == TYPE_GOODS )
//	{
//		dwHashValue = lID;
//	}
//	else
//	{
//		dwHashValue = static_cast<BYTE>( oType );
//		dwHashValue	= dwHashValue << 24;
//		dwHashValue	= dwHashValue | ( lID & 0x00FFFFFF );
//	}
//	return dwHashValue;
//}

//--------------------------------------------------------------------------------------
// 函数:	根据哈西值查找容器内对象的方法。
// 参数:	该对象的哈西值 : DWORD
// 返回值:	CBaseObject*
// 描述：	返回找到的对象，如果为NULL则表示没有找到。
//--------------------------------------------------------------------------------------
//##ModelId=41A3256C0242
CBaseObject* CContainer::Find(const CGUID& guid)
{
	// TODO: Add your specialized code here.
	return NULL;
}

//--------------------------------------------------------------------------------------
// 函数:	设置自身的编号
// 参数:	编号 : LONG
// 返回值:	无
// 描述：	无
//--------------------------------------------------------------------------------------
//##ModelId=41A32B6F008C
//VOID CContainer::SetID( LONG lID )
//{
//	CBaseObject::SetID( lID );
//}

//--------------------------------------------------------------------------------------
// 函数:	设置自身的父容器
// 参数:	父容器对象 : CContainer*
// 返回值:	无
// 描述：	无
//--------------------------------------------------------------------------------------
//##ModelId=41A32B5E038A
//VOID CContainer::SetFather( const CContainer* pFather )
//{
//	CBaseObject::SetFather( pFather );
//}

//--------------------------------------------------------------------------------------
// 函数:	根据哈西值移出容器内某对象的方法。
// 参数:	该对象的哈西值 : DWORD
// 返回值:	CBaseObject*
// 描述：	返回移出了的对象指针，如果为NULL则表示容器内没有找到该对象。
//--------------------------------------------------------------------------------------
//##ModelId=41A3E8020148
CBaseObject* CContainer::Remove( const CGUID& guid, LPVOID pVoid )
{
	// TODO: Add your specialized code here.
	return NULL;
}

//--------------------------------------------------------------------------------------
// 函数:	AI
// 参数:	无
// 返回值:	无
// 描述：	无
//--------------------------------------------------------------------------------------
//##ModelId=41A3ED5B0290
//VOID CContainer::AI()
//{
//	// TODO: Add your specialized code here.
//	// NOTE: Requires a correct return value to compile.
//}

//--------------------------------------------------------------------------------------
// 函数:	根据对象指针查找容器内对象的方法。
// 参数:	该对象指针 : CBaseObject*
// 返回值:	CBaseObject*
// 描述：	返回找到的对象，如果为NULL则表示没有找到。
//--------------------------------------------------------------------------------------
//##ModelId=41A1913202FD
CBaseObject* CContainer::Find( CBaseObject* pObj )
{
	if( pObj != NULL )
	{
		//return Find( CContainer::CalculateHashValue( pObj->GetType(), pObj->GetID() ) );
		return Find( pObj->GetExID() );
	}
	return NULL;
}

//--------------------------------------------------------------------------------------
// 函数:	根据类型和编号查找容器内对象的方法。
// 参数:	类型 : OBJECT_TYPE，编号 : LONG
// 返回值:	CBaseObject*
// 描述：	返回找到的对象，如果为NULL则表示没有找到。
//--------------------------------------------------------------------------------------
//##ModelId=41A17F7601D4
CBaseObject* CContainer::Find( /*OBJECT_TYPE*/LONG oType, const CGUID& guid )
{
	return Find( guid );
}

//--------------------------------------------------------------------------------------
// 函数:	添加一对象到容器内的方法。
// 参数:	该对象指针 : CBaseObject*
// 返回值:	BOOL
// 描述：	返回TRUE表示添加对象成功，如果为FALSE则表示失败。
//--------------------------------------------------------------------------------------
//##ModelId=41A17D3B0186
BOOL CContainer::Add( CBaseObject* pObject, LPVOID pVoid )
{
	return FALSE;
}

//--------------------------------------------------------------------------------------
// 函数:	清空容器内所有对象（但不删除这些对象）的方法。
// 参数:	无
// 返回值:	无
// 描述：	无
//--------------------------------------------------------------------------------------
//##ModelId=41A17EA20242
VOID CContainer::Clear( LPVOID pVoid )
{
}

//--------------------------------------------------------------------------------------
// 函数:	清空容器内所有对象（并且删除这些对象）的方法。
// 参数:	无
// 返回值:	无
// 描述：	无
//--------------------------------------------------------------------------------------
//##ModelId=41A191F100AB
VOID CContainer::Release()
{
	RemoveAllListener();
}

//--------------------------------------------------------------------------------------
// 函数:	移出容器内某一对象（但不删除这些对象）的方法。
// 参数:	该对象的指针 : CBaseObject*
// 返回值:	CBaseObject*
// 描述：	返回移出的对象，如果为NULL则表示容器内不存在该对象。
//--------------------------------------------------------------------------------------
//##ModelId=41A17D5702EE
CBaseObject* CContainer::Remove( CBaseObject* pObject, LPVOID pVoid )
{
	if( pObject != NULL )
	{
		return Remove( pObject->GetExID(), pVoid );
	}
	return NULL;
}

//--------------------------------------------------------------------------------------
// 函数:	移出容器内某一对象（但不删除这些对象）的方法。
// 参数:	类型 : OBJECT_TYPE，编号 : LONG
// 返回值:	CBaseObject*
// 描述：	返回移出的对象，如果为NULL则表示容器内不存在该对象。
//--------------------------------------------------------------------------------------
//##ModelId=41A17DFC0213
CBaseObject* CContainer::Remove( /*OBJECT_TYPE*/LONG oType, const CGUID& guid, LPVOID pVoid )
{
	return Remove( guid, pVoid );
}

//--------------------------------------------------------------------------------------
// 函数:	添加一个容器监听器
// 参数:	pListener:监听器对象
// 返回值:	TRUE or FALSE
// 描述：	
//--------------------------------------------------------------------------------------
//##ModelId=41A434660119
BOOL CContainer::AddListener(CContainerListener* pListener)
{
	if( pListener == NULL ) return FALSE;
	vector<CContainerListener*>::iterator it = m_vListeners.begin();
	for ( ; it != m_vListeners.end() ; it++ )
	{
		if( (*it) == pListener )
		{
			return FALSE;
		}
	}
	m_vListeners.push_back( pListener );
	return TRUE;
}

//--------------------------------------------------------------------------------------
// 函数:	删除一个容器监听器
// 参数:	pListener:监听器对象
// 返回值:	TRUE or FALSE
// 描述：	
//--------------------------------------------------------------------------------------
//##ModelId=41A4348A02DE
BOOL CContainer::RemoveListener(CContainerListener* pListener)
{
	if( pListener == NULL ) return FALSE;
	vector<CContainerListener*>::iterator it = m_vListeners.begin();
	for ( ; it != m_vListeners.end() ; it++ )
	{
		if( (*it) == pListener )
		{
			m_vListeners.erase( it );
			return TRUE;
		}
	}
	return FALSE;
}

//--------------------------------------------------------------------------------------
// 函数:	删除所有容器监听器
// 参数:	
// 返回值:	
// 描述：	
//--------------------------------------------------------------------------------------
//##ModelId=41A4349E00CB
VOID CContainer::RemoveAllListener()
{
	m_vListeners.clear();
}

