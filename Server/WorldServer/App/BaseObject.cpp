// E:\Metempsychosis\App\BaseObject.cpp : implementation file
//

#include "StdAfx.h"
#include "BaseObject.h"
#include "Goods/Goods.h"
#include "Goods/GoodsFactory.h"
#include "Monster.h"
#include "Player.h"
#include "Region.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef __MEMORY_LEAK_CHECK__
	long CBaseObject::m_nConstructFlag=0;
	CBaseObject::mapLLL	CBaseObject::m_mapConstructObjects;
#endif
// CBaseObject

CBaseObject::CBaseObject()
{
	m_lType = 0;
	//m_lID = 0;
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
//CBaseObject* CBaseObject::FindChildObject(long typeID,long lID)
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
			pObject = new CGoods;
			CGoodsFactory::AddCreateGoodsLog((CGoods*)pObject,27);
		}
		break;

	case TYPE_MONSTER:		// 怪物
		{
			pObject = new CMonster;
		}
		break;

	//case TYPE_NPC:			// NPC
	//	{
	//		pObject = new CNpc;
	//	}
	//	break;

	case TYPE_PLAYER:		// 玩家
		{
			pObject = new CPlayer;
		}
		break;

	case TYPE_REGION:		// 地图
		{
			pObject = new CRegion;
		}
		break;

	default:
		return NULL;
	}
	
	pObject->SetExID(guid);
	pObject->SetType(typeID);
	return pObject;
}

// 创建子OBJECT
/*CBaseObject* CBaseObject::CreateChildObject(long typeID, const CGUID& guExID, const char* name, long lGraphicsID)
{
	CBaseObject* pObj = CreateObject(typeID,guExID);
	if( !pObj )
	{
		return NULL;
	}

	// 新建obj
	//if( lID == 0 )
	//{
	//	CRsSetup* pSetup = theApp.GetDataBase()->m_pRsSetup;
	//	int i=0;
	//	//pSetup->_Open();
	//	if( pSetup->_Edit() )
	//	{
	//		switch(typeID)
	//		{
	//		case TYPE_GOODS:		// 物品
	//			{
	//				lID = pSetup->GetNewGoodsId();
	//			}
	//			break;

	//		case TYPE_PLAYER:		// 玩家
	//			{
	//				lID = pSetup->GetNewPlayerID();
	//			}
	//			break;
	//		}

	//		//pSetup->RefreshSeedNum();
	//	}
	//	else
	//	{
	//		ASSERT(0);
	//		delete pObj;
	//		return NULL;
	//	}
	//}

	if( name != NULL )
	{
		pObj->m_strName = name;
	}
	CGUID lBackID = pObj->GetExID();
	pObj->SetExID(guExID);

	// 加入世界
	if( typeID == TYPE_PLAYER && lBackID == NULL_GUID  || 
		typeID == TYPE_GOODS && lBackID == NULL_GUID && strcmp(name, "NoAdd") == 0 )
	{
	}
	else
	{
		AddObject(pObj);
	}

	pObj->SetExID(lBackID);
	pObj->Load();
	pObj->SetExID( guExID );

	if( lGraphicsID != 0 )
	{
		pObj->SetGraphicsID( lGraphicsID );
	}

	if( name != NULL )
	{
		pObj->m_strName = name;
	}

	return pObj;
}*/

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
//void CBaseObject::DeleteChildObject(long typeID,long lID)
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

	// 删除PlayerList中记录
	if( pObject->GetType() == TYPE_PLAYER )
	{
	//	theApp.GetPlayerList()->remove((CPlayer*)pObject);

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

	// 更新PlayerList列表
	if( pObject->GetType() == TYPE_PLAYER )
	{
	//	theApp.GetPlayerList()->push_back((CPlayer*)pObject);

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
// 			_snprintf(pszStrInfo,256,"objType:%d,flag:%d,Num:%d",(*itObj).first,(*itNum).first,(*itNum).second);
// 			PutStringToFile("ObjReleaseCheck",pszStrInfo);

            Log4c::Trace(ROOT_MODULE,"objType:%d,flag:%d,Num:%d",(*itObj).first,(*itNum).first,(*itNum).second);
		}
	}
}

#endif
