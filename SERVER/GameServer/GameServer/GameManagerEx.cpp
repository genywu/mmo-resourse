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
#include "appserver/BaseObject.h"
#include "appserver\Player.h"
#include "appserver/monster.h"
#include "appserver/ServerRegion.h"
#include "appserver/goods/cgoods.h"
#include "appserver/collection.h"
#include "../setup/GlobeSetup.h"
#include "..\nets\netserver\message.h"
#include "..\..\GameServer\GameServer\GameManager.h"
#include "..\AppServer\RgnManager.h"
#include "..\AppServer\citygate.h"

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
			pObject = MP_NEW CGoods;
			m_GoodsList[guid] = pObject;
		}
		break;

	case TYPE_COLLECTION:
		{
			//! 如果断言失败，说明为同一个GUID已经分配了一块内存，再次分配会产生内存泄露
			assert(m_CollectionList.end() == m_CollectionList.find(guid));

			pObject = MP_NEW CCollection;
			m_CollectionList[guid] = pObject;
		}
		break;

	case TYPE_MONSTER:		// 怪物
		{
			//! 如果断言失败，说明为同一个GUID已经分配了一块内存，再次分配会产生内存泄露
			assert(m_MonsterList.end() == m_MonsterList.find(guid));

			pObject = MP_NEW CMonster;
			m_MonsterList[guid] = pObject;
		}
		break;

	case TYPE_PLAYER:		// 玩家
		{
			//! 如果断言失败，说明为同一个GUID已经分配了一块内存，再次分配会产生内存泄露
			assert(m_PlayerList.end() == m_PlayerList.find(guid));

			pObject = MP_NEW  CPlayer;
			m_PlayerList[guid] = pObject;
		}
		break;

	case TYPE_REGION:		// 地图
		{
			//! 如果断言失败，说明为同一个GUID已经分配了一块内存，再次分配会产生内存泄露
			assert(m_RegionList.end() == m_RegionList.find(guid));

			throw;
			//pObject = MP_NEW CRegion;
			//m_RegionList[guid] = pObject;
		}
		break;

	case TYPE_BUILD:
		{
			//! 如果断言失败，说明为同一个GUID已经分配了一块内存，再次分配会产生内存泄露
			assert(m_BuildList.end() == m_BuildList.find(guid));

			pObject = MP_NEW  CBuild;
			m_BuildList[guid] = pObject;
		}
		break;

	case TYPE_CITYGATE:
		{
			//! 如果断言失败，说明为同一个GUID已经分配了一块内存，再次分配会产生内存泄露
			assert(m_GateList.end() == m_GateList.find(guid));

			pObject = MP_NEW CGate;
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
			MP_DELETE(pObj);
		}
		break;

	case TYPE_COLLECTION:
		{
			m_CollectionList.erase(guid);
			MP_DELETE(pObj);
		}
		break;

	case TYPE_MONSTER:		// 怪物
		{
			m_MonsterList.erase(guid);
			MP_DELETE(pObj);
		}
		break;

	case TYPE_PLAYER:		// 玩家
		{
			m_PlayerList.erase(guid);
			MP_DELETE(pObj);
		}
		break;

	case TYPE_REGION:		// 地图
		{
			m_RegionList.erase(guid);
			throw;
			//MP_DELETE(pObj);
		}
		break;

	case TYPE_BUILD:
		{
			m_BuildList.erase(guid);
			MP_DELETE(pObj);
		}
		break;

	case TYPE_CITYGATE:
		{
			m_GateList.erase(guid);
			MP_DELETE(pObj);
		}
		break;

	default:
		assert(false);
	}

	*ppObj = NULL;
}