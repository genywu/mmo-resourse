//=============================================================================
/**
 *  file: GameManager.cpp
 *
 *  Brief:游戏实体的管理类，包括事件和消息
 *
 *  Authtor:wangqiao
 *	
 *	Datae:2007-5-17
 */
//=============================================================================
#include "StdAfx.h"

#include "Game.h"
#include "GameManager.h"
#include "../App/BaseObject.h"
#include "../App/Collection.h"
#include "../App/Goods/CGoods.h"
#include "../App/Monster.h"
#include "../App/Player.h"
#include "../App/RgnManager.h"
#include "../App/ServerRegion.h"

//分配一个CMonster*
CBaseObject* GameManager::CreateObject(long typeID,const CGUID& guid, const char *pInfo)
{
	int *nNum = 0;
	CBaseObject* pObject = NULL;

	switch(typeID)
	{
	case TYPE_GOODS:		// 物品
		{
			//! 如果断言失败，说明为同一个GUID已经分配了一块内存，再次分配会产生内存泄露
			assert(m_GoodsList.end() == m_GoodsList.find(guid));

			/*pObject = (CBaseObject*)m_MultiMemPool.Multi_Alloc(sizeof(CGoods), pInfo);
			new(pObject)CGoods();*/
			pObject = OBJ_CREATE(CGoods);
			m_GoodsList[guid] = pObject;
		}
		break;

	case TYPE_COLLECTION:
		{
			//! 如果断言失败，说明为同一个GUID已经分配了一块内存，再次分配会产生内存泄露
			assert(m_CollectionList.end() == m_CollectionList.find(guid));

			/*pObject = (CBaseObject*)m_MultiMemPool.Multi_Alloc(sizeof(CCollection), pInfo);
			new(pObject)CCollection();*/
			pObject = OBJ_CREATE(CCollection);
			m_CollectionList[guid] = pObject;
		}
		break;

	case TYPE_MONSTER:		// 怪物
		{
			//! 如果断言失败，说明为同一个GUID已经分配了一块内存，再次分配会产生内存泄露
			assert(m_MonsterList.end() == m_MonsterList.find(guid));

			/*pObject = (CBaseObject*)m_MultiMemPool.Multi_Alloc(sizeof(CMonster), pInfo);
			new(pObject)CMonster;*/
			pObject = OBJ_CREATE(CMonster);
			m_MonsterList[guid] = pObject;
		}
		break;

	case TYPE_PLAYER:		// 玩家
		{
			//! 如果断言失败，说明为同一个GUID已经分配了一块内存，再次分配会产生内存泄露
			assert(m_PlayerList.end() == m_PlayerList.find(guid));

			/*pObject = (CBaseObject*)m_MultiMemPool.Multi_Alloc(sizeof(CPlayer), pInfo);
			new(pObject)CPlayer();*/
			pObject = OBJ_CREATE(CPlayer);
			m_PlayerList[guid] = pObject;
		}
		break;

	case TYPE_REGION:		// 地图
		{
			//! 如果断言失败，说明为同一个GUID已经分配了一块内存，再次分配会产生内存泄露
			assert(m_RegionList.end() == m_RegionList.find(guid));

			/*pObject = (CBaseObject*)m_MultiMemPool.Multi_Alloc(sizeof(CRegion), pInfo);
			new(pObject)CRegion(guid);*/
			pObject = OBJ_CREATE(CRegion);
			m_RegionList[guid] = pObject;
		}
		break;

	case TYPE_BUILD:
		{
			//! 如果断言失败，说明为同一个GUID已经分配了一块内存，再次分配会产生内存泄露
			assert(m_BuildList.end() == m_BuildList.find(guid));

			/*pObject = (CBaseObject*)m_MultiMemPool.Multi_Alloc(sizeof(CBuild), pInfo);
			new(pObject)CBuild();*/
			pObject = OBJ_CREATE(CBuild);
			m_BuildList[guid] = pObject;
		}
		break;

	case TYPE_CITYGATE:
		{
			//! 如果断言失败，说明为同一个GUID已经分配了一块内存，再次分配会产生内存泄露
			assert(m_GateList.end() == m_GateList.find(guid));

			/*pObject = (CBaseObject*)m_MultiMemPool.Multi_Alloc(sizeof(CGate), pInfo);
			new(pObject)CGate();*/
			pObject = OBJ_CREATE(CGate);
			m_GateList[guid] = pObject;
		}
		break;

	default:
		return NULL;
	}

	pObject->SetExID(guid);
	pObject->SetType(typeID);

	return pObject;
}

void GameManager::FreeObject(CBaseObject** ppObj)
{
	if (NULL == ppObj)
	{
		return;
	}
	if (NULL == *ppObj)
	{
		return;
	}
	CBaseObject *pObj = *ppObj;
	
	const CGUID&	guid	= pObj->GetExID();

	switch(pObj->GetType())
	{
	case TYPE_GOODS:		// 物品
		{
			m_GoodsList.erase(guid);
			/*((CGoods*)pObj)->~CGoods();
			m_MultiMemPool.Multi_Free((void*)pObj, sizeof(CGoods));*/
			OBJ_RELEASE(CGoods, pObj);
		}
		break;

	case TYPE_COLLECTION:
		{
			m_CollectionList.erase(guid);
			/*((CCollection*)pObj)->~CCollection();
			m_MultiMemPool.Multi_Free((void*)pObj, sizeof(CCollection));*/
			OBJ_RELEASE(CCollection, pObj);
		}
		break;

	case TYPE_MONSTER:		// 怪物
		{
			m_MonsterList.erase(guid);
			/*((CMonster*)pObj)->~CMonster();
			m_MultiMemPool.Multi_Free((void*)pObj, sizeof(CMonster));*/
			OBJ_RELEASE(CMonster, pObj);
		}
		break;

	case TYPE_PLAYER:		// 玩家
		{
			m_PlayerList.erase(guid);
// 			((CPlayer*)pObj)->~CPlayer();
// 			m_MultiMemPool.Multi_Free((void*)pObj, sizeof(CPlayer));
			OBJ_RELEASE(CPlayer, pObj);
		}
		break;

	case TYPE_REGION:		// 地图
		{
			m_RegionList.erase(guid);
// 			((CRegion*)pObj)->~CRegion();
// 			m_MultiMemPool.Multi_Free((void*)pObj, sizeof(CRegion));
			OBJ_RELEASE(CServerRegion, pObj);
		}
		break;

	case TYPE_BUILD:
		{
			m_BuildList.erase(guid);
// 			((CBuild*)pObj)->~CBuild();
// 			m_MultiMemPool.Multi_Free(pObj, sizeof(CBuild));
			OBJ_RELEASE(CBuild, pObj);
		}
		break;

	case TYPE_CITYGATE:
		{
			m_GateList.erase(guid);
// 			((CGate*)pObj)->~CGate();
// 			m_MultiMemPool.Multi_Free(pObj, sizeof(CGate));
			OBJ_RELEASE(CGate, pObj);
		}
		break;

	default:
		assert(false);
	}

	*ppObj = NULL;
}