// E:\Metempsychosis\App\BaseObject.cpp : implementation file
//

#include "stdafx.h"
#include "BaseObject.h"
#include "goods.h"
#include "Monster.h"
#include "player.h"
#include "region.h"
#include "../Script/ScriptSys.h"
#include "../GameTest/GameTestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CBaseObject

CBaseObject::CBaseObject()
:m_lType(0),
m_bIsHide(true),
m_bIncludeChild(true),
m_pFather(NULL)
{
	ZeroMemory(m_strName,32);
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
	/*m_lGraphicsID = */_GetLongFromByteArray(pByte, pos);

	_GetStringFromByteArray(pByte, pos, m_strName);

	return true;
}
bool CBaseObject::DecordFromDataBlock(DBReadSet& db_r,bool bEx /* = true */)
{
	m_bIsHide = (db_r.GetByteFromByteArray() != 0);
	m_lType = db_r.GetLongFromByteArray();
	db_r.GetBufferFromByteArray(m_guExID);
	/*m_lGraphicsID = */db_r.GetLongFromByteArray();
	db_r.GetStringFromByteArray(m_strName,256);
	return true;
}

/////////////////////////////////////////////////////////////////////
// 子节点列表（构成世界树）
/////////////////////////////////////////////////////////////////////

//查找子object!
bool CBaseObject::FindChildObject(CBaseObject* p)
{
	itBaseObject it;
	for (it = m_listObject.begin(); it != m_listObject.end(); it++)
	{
		CBaseObject* pObject = (*it);
		if (pObject == p)
		{
			return true;
		}
	}
	return false;
}

CBaseObject* CBaseObject::FindChildObject(const CGUID & guid)
{
	itBaseObject it;
	for (it = m_listObject.begin(); it != m_listObject.end(); ++it)
	{
		CBaseObject* pObject = (*it);
		if (pObject && pObject->GetExID() == guid)
		{
			return pObject;
		}
	}
	return NULL;
}
//查找子object!
CBaseObject* CBaseObject::FindChildObject(long typeID,const CGUID& gID)
{
	itBaseObject it;
	for (it = m_listObject.begin(); it != m_listObject.end(); it++)
	{
		CBaseObject* pObject = (*it);
		if (pObject->IsThisObject(typeID,gID))
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
	for (it = m_listObject.begin(); it != m_listObject.end(); it++)
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
CBaseObject* CBaseObject::RecursiveFindObject(long typeID,CGUID& gID)
{
	if(IsThisObject(typeID, gID))
	{
		return this;
	}

	itBaseObject it;
	for (it = m_listObject.begin(); it != m_listObject.end(); it++)
	{
		CBaseObject* pObject = (*it);
		CBaseObject* p = pObject->RecursiveFindObject(typeID,gID);
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
	if( typeID == m_lType && strcmp(strName, m_strName) == 0 )
	{
		return this;
	}

	itBaseObject it;
	for (it = m_listObject.begin(); it != m_listObject.end(); it++)
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
	if(pObj)
	{
		SAFE_DELETE(pObj);
	}
}

// 删除子OBJECT
void CBaseObject::DeleteChildObject(long typeID,const CGUID& gID)
{
	CBaseObject* pObj = FindChildObject(typeID,gID);
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
				CMoveShape* pShape = dynamic_cast<CMoveShape*>(pObject);
				if( pShape )
				{
					//if( pShape->GetUsedTimes() == 1 || pShape->GetUsedTimes() == 0 )
// 					if( pShape->GetUsedTimes() == 0 )
// 					{
						SAFE_DELETE(pObject);
/*					}*/
				}
				m_listObject.erase(it++);				
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

#if _DEBUG
	// 删除playerlist中记录
	if( pObject->GetType() == TYPE_PLAYER )
	{
		char str[256];
		sprintf(str, "%s 离开场景 %s(%d,%d)\n", pObject->GetName(), GetName(), 
			((CPlayer*)pObject)->GetTileX(), ((CPlayer*)pObject)->GetTileY());
		OutputDebugString(str);
	}
#endif
}

// 添加OBJECT
void CBaseObject::AddObject(CBaseObject* pObject)
{
	pObject->SetFather(this);
	m_listObject.push_back(pObject);

#if _DEBUG
	// 更新playerlist列表
	if( pObject->GetType() == TYPE_PLAYER )
	{
		char str[256];
		sprintf(str, "%s 进入场景 %s(%d,%d)\n", pObject->GetName(),	GetName(),
			((CPlayer*)pObject)->GetTileX(), ((CPlayer*)pObject)->GetTileY());
		OutputDebugString(str);
	}
#endif
}

// 回调一个函数对所有查找到的对应TYPE对象做处理
void CBaseObject::DgFindObjectsByTypes( long lType, P_FUNCTION_DO p_function_do, LPARAM lparam)
{
	listBaseObject listBack = m_listObject;
	for( itBaseObject it = listBack.begin();it!=listBack.end();it++)
	{
		CBaseObject* pObject = (*it);
		if( pObject->GetType() == lType )
		{
			p_function_do((LPARAM)pObject,lparam);
		}
		else{
			pObject->DgFindObjectsByTypes(lType,p_function_do,lparam);
		}
	}
}

Timer *QueryTimer()
{
    CGameTestDlg *dlg = (CGameTestDlg*) theApp.GetMainWnd();
    return dlg->GetTimer();
}
