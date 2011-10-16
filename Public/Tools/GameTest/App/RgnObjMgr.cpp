


#include "StdAfx.h"
#include "RgnObjMgr.h"
#include "BaseObject.h"
#include "MoveShape.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CClientRgnObjMgr::CClientRgnObjMgr()
{

}

CClientRgnObjMgr::~CClientRgnObjMgr()
{

}

bool CClientRgnObjMgr::AddObject(CBaseObject* pObject)
{
	CMoveShape* pShape = (CMoveShape*)pObject;
	if( pShape != NULL )
	{
		m_mapObject.insert(make_pair(pShape->GetExID(),pShape));
		//	增加引用计数
		pShape->IncrUsedTimes();
		return true;
	}
	return false;
}

void CClientRgnObjMgr::RemoveObject(CBaseObject* pObject)
{
	if( pObject )
	{
		CMapRgnObj::iterator it = m_mapObject.find(pObject->GetExID());
		if( it != m_mapObject.end() )
		{
			((CMoveShape*)it->second)->ReduUsedTimes();

			m_mapObject.erase(it);
		}
	}
}

CBaseObject* CClientRgnObjMgr::FindObject(long lType,const CGUID& guid)
{
	CMapRgnObj::iterator it = m_mapObject.find(guid);
	if( it != m_mapObject.end() )
	{
		if( it->second->GetType() == lType )
			return it->second;
	}
	return NULL;
}

CBaseObject* CClientRgnObjMgr::FindObject(const CGUID& guid)
{
	CMapRgnObj::iterator it = m_mapObject.find(guid);
	if( it != m_mapObject.end() )
		return it->second;
	return NULL;
}