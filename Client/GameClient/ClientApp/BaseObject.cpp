// E:\Metempsychosis\App\BaseObject.cpp : implementation file
//

#include "stdafx.h"
#include "BaseObject.h"
#include "../../../Public/Common/Public.h"
#include "../../../Public/Common/BaseDef.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CBaseObject

CBaseObject::CBaseObject()
{
	m_bIsHide = false;
	m_lType = 0;
	m_lGraphicsID = 0;
	m_strName = "";
	m_pFather = NULL;

	m_bIncludeChild = true;
}

CBaseObject::~CBaseObject()
{
	DeleteAllChildObject();
}

// CBaseObject member functions

/////////////////////////////////////////////////////////////////////
// 存取接口
/////////////////////////////////////////////////////////////////////

// 读取
BOOL CBaseObject::Load()
{
	return true;
}

// 存储
BOOL CBaseObject::Save()
{
	return true;
}

// 添加到CByteArray
bool CBaseObject::AddToByteArray(vector<BYTE>* pByteArray, bool bExData)
{
	return true;
}

// 解码
bool CBaseObject::DecordFromByteArray(BYTE* pByte, long& pos, bool bExData)
{
	m_bIsHide = (_GetByteFromByteArray(pByte, pos) != 0);
	m_lType = _GetLongFromByteArray(pByte, pos);
	_GetBufferFromByteArray(pByte,pos,m_guExID);
	m_lGraphicsID = _GetLongFromByteArray(pByte, pos);

	char str[256];
	m_strName = _GetStringFromByteArray(pByte, pos, str);

	return true;
}

// 解码
bool CBaseObject::DecodeFromDataBlock(DBReadSet& setReadDB,bool bExData )
{
	m_bIsHide = (setReadDB.GetByteFromByteArray() != 0);
	m_lType = setReadDB.GetLongFromByteArray();
	setReadDB.GetBufferFromByteArray(m_guExID);
	m_lGraphicsID =setReadDB.GetLongFromByteArray();
	char str[256];
	m_strName =setReadDB.GetStringFromByteArray(str,256);
	return true;
}


/////////////////////////////////////////////////////////////////////
// 子节点列表（构成世界树）
/////////////////////////////////////////////////////////////////////

//查找子object!
bool CBaseObject::FindChildObject(CBaseObject* p)
{
	itBaseObject it;
	for (it = m_listObject.begin(); it != m_listObject.end(); ++it)
	{
		CBaseObject* pObject = (*it);
		if (pObject == p)
		{
			return true;
		}
	}
	return false;
}

//查找子object!
CBaseObject* CBaseObject::FindChildObject(long typeID,const CGUID& id)
{
	itBaseObject it;
	for (it = m_listObject.begin(); it != m_listObject.end(); ++it)
	{
		CBaseObject* pObject = (*it);
		if (pObject->IsThisObject(typeID,id))
		{
			return pObject;
		}
	}
	return NULL;
}


CBaseObject* CBaseObject::FindChildObject(const CGUID& id)
{
	itBaseObject it;
	for (it = m_listObject.begin(); it != m_listObject.end(); ++it)
	{
		CBaseObject* pObject = (*it);
		if (pObject && pObject->GetExID() == id)
		{
			return pObject;
		}
	}
	return NULL;
}

// 查找子goods
CBaseObject* CBaseObject::FindChildGoods(const CGUID& guExID)
{
	itBaseObject it;
	for (it = m_listObject.begin(); it != m_listObject.end(); ++it)
	{
		CBaseObject* pObject = (*it);
		if (pObject->GetType() == TYPE_GOODS && pObject->GetExID() == guExID)
		{
			return pObject;
		}
	}
	return NULL;
}

// 递归查找
CBaseObject* CBaseObject::RecursiveFindObject(long typeID,CGUID& id)
{
	if(IsThisObject(typeID, id))
	{
		return this;
	}

	itBaseObject it;
	for (it = m_listObject.begin(); it != m_listObject.end(); ++it)
	{
		CBaseObject* pObject = (*it);
		CBaseObject* p = pObject->RecursiveFindObject(typeID,id);
		if (p != NULL)
		{
			return p;
		}
	}
	return NULL;
}

// 按名字查找
CBaseObject* CBaseObject::RecursiveFindObject(long typeID,char* strName)
{
	if( typeID == m_lType && strcmp(strName, GetOrigName()) == 0 )
	{
		return this;
	}

	itBaseObject it;
	for (it = m_listObject.begin(); it != m_listObject.end(); ++it)
	{
		CBaseObject* pObject = (*it);
		CBaseObject* p = pObject->RecursiveFindObject(typeID, strName);
		if (p != NULL)
		{
			return p;
		}
	}
	return NULL;
}

// 删除子OBJECT
void CBaseObject::DeleteChildObject(CBaseObject* pObj)
{
	RemoveObject(pObj);
	if(pObj){
		delete pObj;
		pObj = NULL;
	}
}

// 删除子OBJECT
void CBaseObject::DeleteChildObject(long typeID,const CGUID& ID)
{
	CBaseObject* pObj = FindChildObject(typeID,ID);
	if (pObj == NULL)
	{
		return ;
	}

	DeleteChildObject(pObj);
}

// 删除所有子OBJECT(pExpect = 除了)
void CBaseObject::DeleteAllChildObject(CBaseObject* pExpect)
{
	if( m_listObject.size() )
	{
		for (itBaseObject it=m_listObject.begin(); it!=m_listObject.end();)
		{
			CBaseObject* pObject = (*it);
			if( pExpect != pObject )
			{
				m_listObject.erase(it++);
				SAFE_DELETE(pObject);
			}
			else
			{
				++it;
			}
		}
	}
}

// 移出OBJECT
void CBaseObject::RemoveObject(CBaseObject* pObject)
{
	m_listObject.remove(pObject);

}

// 添加OBJECT
void CBaseObject::AddObject(CBaseObject* pObject)
{
	pObject->SetFather(this);
	m_listObject.push_back(pObject);

}

// 回调一个函数对所有查找到的对应TYPE对象做处理
void CBaseObject::DgFindObjectsByTypes( long lType, P_FUNCTION_DO p_function_do, LPARAM lparam)
{
	listBaseObject listBack = m_listObject;
	for (itBaseObject it = listBack.begin(); it != listBack.end(); ++it)
	{
		CBaseObject* pObject = (*it);
		if( pObject->GetType() == lType )
		{
			p_function_do((LPARAM)pObject, lparam);
		}
		else
		{
			pObject->DgFindObjectsByTypes(lType, p_function_do, lparam);
		}
	}
}

/////////////////////////////////////////////////////////////////////
// AI
/////////////////////////////////////////////////////////////////////

void CBaseObject::AI()		// AI处理
{
	if( m_listObject.size() )
	{
		itBaseObject it;
		for (it = m_listObject.begin(); it != m_listObject.end(); ++it)
		{
			CBaseObject* pObject = (*it);
			pObject->AI();
		}
	}
}

/////////////////////////////////////////////////////////////////////
// 广播
/////////////////////////////////////////////////////////////////////

// 广播
void CBaseObject::BoardCast(long lCommand, long lParam)
{
	itBaseObject it;
	for (it = m_listObject.begin(); it != m_listObject.end(); ++it)
	{
		(*it)->BoardCast(lCommand, lParam);
	}
}
