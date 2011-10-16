// E:\Metempsychosis\App\BaseObject.cpp : implementation file
//

#include "stdafx.h"
#include "BaseObject.h"
#include "goods\cgoods.h"
#include "goods\CGoodsFactory.h"
#include "npc.h"
#include "Monster.h"
#include "player.h"
#include "region.h"
#include "../public/public.h"



#ifdef __MEMORY_LEAK_CHECK__
	long CBaseObject::m_nConstructFlag=0;
	CBaseObject::mapLLL	CBaseObject::m_mapConstructObjects;
#endif
// CBaseObject

CBaseObject::CBaseObject()
{
	m_lType = 0;
	m_lGraphicsID = 0;
	m_strName = "";
	m_pFather = NULL;

	m_bIncludeChild = true;

	m_byCanHide = 0; // 能否隐匿
	m_byHideFlag = 0; // 当前隐匿状态
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
	_AddToByteArray(pByteArray, GetType());
	_AddToByteArray(pByteArray, GetExID());
	_AddToByteArray(pByteArray, GetGraphicsID());
	_AddToByteArray(pByteArray, (char*)GetName());
	return true;
}

// 解码
bool CBaseObject::DecordFromByteArray(BYTE* pByte, long& pos, bool bExData)
{
	m_lType = _GetLongFromByteArray(pByte, pos);
	_GetBufferFromByteArray(pByte, pos, m_guExID);
	m_lGraphicsID = _GetLongFromByteArray(pByte, pos);

	char str[256];
	m_strName =_GetStringFromByteArray(pByte, pos, str);

	return true;
}

// 添加到CByteArray
bool CBaseObject::CodeToDataBlock(DBWriteSet& setWriteDB, bool bExData)
{
	// 编码隐匿功能
	setWriteDB.AddToByteArray((BYTE)m_byCanHide);
	setWriteDB.AddToByteArray((BYTE)m_byHideFlag);

	setWriteDB.AddToByteArray(GetType());
	setWriteDB.AddToByteArray(GetExID());
	setWriteDB.AddToByteArray(GetGraphicsID());
	setWriteDB.AddToByteArray((char*)GetName());
	return true;
}

// 解码
bool CBaseObject::DecodeFromDataBlock(DBReadSet& setReadDB,bool bExData )
{
	m_byCanHide = setReadDB.GetByteFromByteArray();
	m_byHideFlag = setReadDB.GetByteFromByteArray();

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

//查找子object!
CBaseObject* CBaseObject::FindChildObject(long typeID, const CGUID& guExID)
{
	itBaseObject it;
	for (it = m_listObject.begin(); it != m_listObject.end(); it++)
	{
		CBaseObject* pObject = (*it);
		if (pObject->IsThisObject(typeID,guExID))
		{
			return pObject;
		}
	}
	return NULL;
}

// 递归查找
CBaseObject* CBaseObject::RecursiveFindObject(long typeID, const CGUID& guExID)
{
	if(IsThisObject(typeID, guExID))
	{
		return this;
	}

	itBaseObject it;
	for (it = m_listObject.begin(); it != m_listObject.end(); it++)
	{
		CBaseObject* pObject = (*it);
		CBaseObject* p = pObject->RecursiveFindObject(typeID,guExID);
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
	if( typeID == m_lType && strcmp(strName, m_strName.c_str()) == 0 )
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

// 创建Object
CBaseObject* CBaseObject::CreateObject(long typeID, const CGUID& guid)
{
	int *nNum = 0;
	CBaseObject* pObject = NULL;

	switch(typeID)
	{
	case TYPE_GOODS:		// 物品
		{
#ifdef __MEMORY_LEAK_CHECK__
			CBaseObject::SetConstructFlag(2);
#endif
			pObject = MP_NEW CGoods;
			CGoodsFactory::AddCreateGoodsLog((CGoods*)pObject,27);
		}
		break;

	case TYPE_MONSTER:		// 怪物
		{
			pObject = MP_NEW CMonster;
		}
		break;
	case TYPE_PLAYER:		// 玩家
		{
			pObject = MP_NEW CPlayer;
		}
		break;

	case TYPE_REGION:		// 地图
		{
			pObject = MP_NEW CRegion;
		}
		break;

	default:
		return NULL;
	}
	
	pObject->SetExID(guid);
	pObject->SetType(typeID);
	return pObject;
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
void CBaseObject::DeleteChildObject(long typeID, CGUID& guExID)
{
	CBaseObject* pObj = FindChildObject(typeID,guExID);
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
		listBaseObject list = m_listObject;
		for (itBaseObject it = list.begin(); it != list.end(); it++)
		{
			CBaseObject* pObject = (*it);
			if( pExpect != pObject )
			{
				DeleteChildObject(pObject);
			}
		}
		list.clear();
	}
}

// 移出OBJECT
void CBaseObject::RemoveObject(CBaseObject* pObject)
{
	m_listObject.remove(pObject);

	// 删除playerlist中记录
	if( pObject->GetType() == TYPE_PLAYER )
	{
#if _DEBUG
		char str[256];
		sprintf(str, "%s 离开场景 %s(%f,%f)\n", pObject->GetName(), GetFather()->GetName(), 
			((CPlayer*)pObject)->GetPosX(), ((CPlayer*)pObject)->GetPosY());
		OutputDebugString(str);
#endif
	}
}

// 添加OBJECT
void CBaseObject::AddObject(CBaseObject* pObject)
{
	pObject->SetFather(this);
	m_listObject.push_back(pObject);

	// 更新playerlist列表
	if( pObject->GetType() == TYPE_PLAYER )
	{
#if _DEBUG
		char str[256];
		sprintf(str, "%s 进入场景 %s(%f,%f)\n", pObject->GetName(), GetFather()->GetName(),
			((CPlayer*)pObject)->GetPosX(), ((CPlayer*)pObject)->GetPosY());
		OutputDebugString(str);
#endif
	}
}

// 回调一个函数对所有查找到的对应TYPE对象做处理
void CBaseObject::DgFindObjectsByTypes( long lType, P_FUNCTION_DO p_function_do, LPARAM lparam)
{
	listBaseObject listBack = m_listObject;
	for (itBaseObject it = listBack.begin(); it != listBack.end(); it++)
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
		listBaseObject listBack = m_listObject;
		itBaseObject it;
		for (it = listBack.begin(); it != listBack.end(); it++)
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
	for (it = m_listObject.begin(); it != m_listObject.end(); it++)
	{
		(*it)->BoardCast(lCommand, lParam);
	}
}


#ifdef __MEMORY_LEAK_CHECK__

void CBaseObject::AddConstructRef(long lType)
{
	itLLL itObj = m_mapConstructObjects.find(lType);
	if( itObj != m_mapConstructObjects.end() )
	{
		mapLL &ObjNum = (*itObj).second;
		itLL itNum = ObjNum.find(m_nConstructFlag);
		if(itNum != ObjNum.end())
		{
			(*itNum).second++;
		}
		else
		{
			ObjNum[m_nConstructFlag]=1;
		}
	}
	else
	{
		m_mapConstructObjects[lType][m_nConstructFlag]=1;
	}
}

void CBaseObject::RefConstructRef(long lType,long lConFlag)
{
	itLLL itObj = m_mapConstructObjects.find(lType);
	if( itObj != m_mapConstructObjects.end() )
	{
		mapLL &ObjNum = (*itObj).second;
		itLL itNum = ObjNum.find(lConFlag);
		if(itNum != ObjNum.end())
		{
			(*itNum).second--;
		}
	}
}

void CBaseObject::OutputConstructRef(long lType)
{
	char pszStrInfo[256]="";
	itLLL itObj = m_mapConstructObjects.begin();
	for( ;itObj != m_mapConstructObjects.end();itObj++ )
	{
		mapLL &ObjNum = (*itObj).second;
		itLL itNum = ObjNum.begin();
		for(;itNum != ObjNum.end();itNum++)
		{
			_snprintf(pszStrInfo,256,"objType:%d,flag:%d,Num:%d",(*itObj).first,(*itNum).first,(*itNum).second);
			PutStringToFile("ObjReleaseCheck",pszStrInfo);
		}
	}
}

#endif


tagTimerVar* CBaseObject::CreateTimerVar(long Flag)
{
	tagTimerVar *pRe = (tagTimerVar*)M_ALLOC(sizeof(tagTimerVar));
#ifdef _DEBUG_TIMER_VAR_
	new(pRe)tagTimerVar(Flag);
#else
	new(pRe)tagTimerVar();
#endif

	return pRe;
}

void CBaseObject::ReleaseTimerVar(tagTimerVar **ppTimer)
{
	if (NULL != ppTimer && NULL != *ppTimer)
	{
		tagTimerVar* pTimvar = *ppTimer;

		M_FREE(pTimvar, sizeof(tagTimerVar));
		*ppTimer = NULL;
	}
}
