#include "StdAfx.h"
#include ".\area.h"
#include "Monster.h"
#include "MMSystem.h"
#include "Player.h"
#include "Collection.h"
#include "ServerRegion.h"
#include "Goods\CGoods.h"
#include "Message Packaging\CS2CContainerObjectMove.h"
#include "..\..\setup\globesetup.h"
#include "GoodsExManage.h"
#include "citygate.h"


long AREA_CELL_X = 15;		// 区域的宽度
long AREA_CELL_Y = 15;		// 区域的高度

long AREA_WIDTH = 15;		// 区域的宽度
long AREA_HEIGHT = 15;		// 区域的高度

enum  EventType
{
	ET_Hibernate=0,
};
CArea::CArea(void)
{
	SetType( TYPE_AREA );
	m_lX = -1;
	m_lY = -1;
	m_iPlayerAreaAround=0;
	m_bIsHangUp=TRUE;
	m_lTimerID=-1;
}

CArea::~CArea(void)
{
	m_setPlayers.clear();
	m_setActivedMonsters.clear();
	m_setSleepedMonsters.clear();
	m_setGoods.clear();
	m_setPets.clear();
	m_setBuild.clear();
	LogoutEvent(m_lTimerID);

	for (map<CGUID, tagGoodsProtection,guid_compare>::iterator it =	m_mGoodsProtection.begin();
		it!=m_mGoodsProtection.end() ;it++)
	{
		LogoutEvent(it->second.lTimerID);
	}
}


VOID CArea::PlayerEnter()
{
	//##唤醒周围的怪物。
	CServerRegion* pRegion = dynamic_cast<CServerRegion*>( GetFather() );
	LONG lX = GetX();
	LONG lY = GetY();

	for( LONG i = 0; i < 9; i ++ )
	{
		CArea* pArea = pRegion -> GetArea( lX + _area[i][0], lY + _area[i][1] );
		if( pArea )
		{
			{
				pArea -> WakeUpCall();//WakeUpMonsters();
			}
		}
	}
}

//##对Player的离开，进入一个Area的部分处理，一定要在Region那一层来控制，
//##不能在迭代Area内Shape的同时调用这个函数。否则极有可能导致iterator的指针错误。
VOID CArea::PlayerLeave()
{
	CServerRegion* pRegion = dynamic_cast<CServerRegion*>( GetFather() );
	LONG lX = GetX();
	LONG lY = GetY();

	for( LONG i = 0; i < 9; i ++ )
	{
		CArea* pArea = pRegion -> GetArea( lX + _area[i][0], lY + _area[i][1] );
		if( pArea )
		{
			{
				pArea ->HibernateCall();//WakeUpMonsters();
			}
		}
	}
}

//##这个函数很危险。尽量避免使用
hashsetGUID& CArea::GetAllPlayers()
{
	return m_setPlayers;
}

VOID CArea::WakeUpMonsters()
{
	CServerRegion* pRegion = dynamic_cast<CServerRegion*>( GetFather() );
	if( pRegion )
	{
		CMonster* pMonster = NULL;

		hsGUIDIt it = m_setSleepedMonsters.begin();
		for( ; it != m_setSleepedMonsters.end(); it ++ )
		{
			pMonster = dynamic_cast<CMonster*>( pRegion -> FindChildObject( TYPE_MONSTER, (*it) ) );
			if( pMonster )
			{
				pMonster -> GetAI() -> WakeUp();
				m_setActivedMonsters.insert( (*it) );
			}
		}
		m_setSleepedMonsters.clear();
	}
}


//##这儿ＡｄｄＯｂｊｅｃｔ函数只会添加物品的索引号，并保留索引号，不会产生新建物品之类的情况
void CArea::AddObject(CBaseObject* pObject)
{
	if( pObject )
	{
		//##添加引用在这儿
		switch( pObject -> GetType() )
		{
		case TYPE_PLAYER:
			{
				long lOldPlayerSize = m_setPlayers.size();
				m_setPlayers.insert(pObject -> GetExID());
				if(lOldPlayerSize == 0 && m_setPlayers.size()==1)
				{
					PlayerEnter();
				}
			}
			break;

		case TYPE_MONSTER:
			{
				m_setActivedMonsters.insert(pObject -> GetExID());
			}
			break;
		case TYPE_PET:
			{
				m_setPets.insert(pObject -> GetExID());
			}
			break;

		case TYPE_GOODS:
			{
				m_setGoods.insert(pObject -> GetExID());
			}
			break;

		case TYPE_COLLECTION:
			{
				m_setCollection.insert(pObject -> GetExID());
			}
			break;

		case TYPE_CITYGATE:
			{				
				m_setBuild.insert(pObject -> GetExID());
			}
			break;

		default:
			{
				PutDebugString("the object instance type error!");
			}
			break;
		}
	}
}

VOID CArea::SetGoodsProtection( CPlayer* pPlayer, CGoods* pGoods ,BOOL bTeamProtected)
{
	if( pPlayer && pGoods )
	{
		tagGoodsProtection gp;
		gp.dwTimeStamp = timeGetTime();

		if( pPlayer -> GetTeamID() != NULL_GUID && bTeamProtected)
		{
			gp.plLevel	= PL_TEAM;
			gp.guid		= pPlayer -> GetTeamID();
		}
		else
		{
			gp.plLevel = PL_PLAYER;
			gp.guid		= pPlayer -> GetExID();
		}
		AddGoodsDelTime(pGoods,gp);
		m_mGoodsProtection[pGoods->GetExID()] = gp;
	}
}

eGoodsPickValue CArea::CanPickUpGoods( CPlayer* pPlayer, CGoods* pGoods )
{
	eGoodsPickValue bResult = eGPV_Cant;

	if( pPlayer && pGoods )
	{
		itGoodsPro it = m_mGoodsProtection.find( pGoods -> GetExID() );
		if( it != m_mGoodsProtection.end() && timeGetTime()< it->second.dwTimeStamp+ CGlobeSetup::GetSetup() -> dwGoodsProtectedTimer )
		{
			if( it -> second.plLevel == PL_TEAM )
			{
				if( it -> second.guid == pPlayer -> GetTeamID() )
				{
					bResult = eGPV_Allow_IsYourTeams;
				}
			}
			else
			{
				if( it -> second.guid == pPlayer -> GetExID() )
				{
					bResult = eGPV_Allow_IsYours;
				}
			}
		}
		else
		{
			bResult = eGPV_Allow_NoOwner;
		}
	}
	return bResult;
}

//关于物品在区域定时删除
//添加物品删除时间
VOID CArea::AddGoodsDelTime(CGoods *pGoods,tagGoodsProtection& gp)
{
	if (pGoods== NULL)	return;

	DWORD dwGoodsDisappearTime	= CGlobeSetup::GetSetup() -> dwGoodsDisappearTimer;
	if( ( pGoods -> GetAddonPropertyValues(GAP_PARTICULAR_ATTRIBUTE, 1) & 
		CGoodsBaseProperties::PA_DELETE_WHEN_DROP_DOWN ) == CGoodsBaseProperties::PA_DELETE_WHEN_DROP_DOWN )
	{
		dwGoodsDisappearTime = 0;
	}
	if (GoodsExManage::GetInstance().IsBind(pGoods)) //! 如果绑定
	{
		dwGoodsDisappearTime = 0;
	}

	tagTimerVar* pTimvar = CreateTimerVar(34);
	stDestObj* pDestObj = (stDestObj*)M_ALLOC(sizeof(stDestObj));
	pTimvar->TimerType = ChildObjType_DelGoods;
	pDestObj->nDestType= pGoods->GetType();
	pDestObj->DestID= pGoods->GetExID();
	pTimvar->pvar = pDestObj;
	pTimvar->pvar_size = sizeof(stDestObj);
	gp.lTimerID=GameManager::GetInstance()->Schedule(this,(void*)pTimvar,timeGetTime()+dwGoodsDisappearTime,0);
}
VOID CArea::AddGoodsDelTime(CGoods *pGoods)
{
	tagGoodsProtection gp;
	AddGoodsDelTime(pGoods,gp);
	m_mGoodsProtection[pGoods->GetExID()] = gp;
}
//取消物品删除时间
VOID CArea::LogoutGoodsDelTime(CGoods* pGoods)
{
	itGoodsPro it= m_mGoodsProtection.find(pGoods->GetExID());
	if (it!= m_mGoodsProtection.end())
	{
		LONG lTimerID=(*it).second.lTimerID;
		GameManager::GetInstance()->Cancel(lTimerID,0);
	}
}
//保护超时，执行删除
VOID CArea::OnGoodsDelTimeOut(stDestObj* pDestObj)
{
	if (pDestObj==NULL)	return;
	do 
	{
		CServerRegion* pServerRgn= dynamic_cast<CServerRegion*>(GetFather());
		if (pServerRgn== NULL)	
			return;
		CGoods* pGoods=dynamic_cast<CGoods*>(pServerRgn->FindChildObject(pDestObj->nDestType,pDestObj->DestID));
		if (pGoods==NULL)
			break;

		itGoodsPro it= m_mGoodsProtection.find(pGoods->GetExID());
		if (it!= m_mGoodsProtection.end())
		{
			m_mGoodsProtection.erase(it);
		}

		if (pServerRgn!=NULL)
		{
			pServerRgn->DelShapeToAround(pGoods);
			pServerRgn->DeleteChildObject(pGoods);
		}
	} while(false);

	M_FREE(pDestObj, sizeof(stDestObj));
}

VOID CArea::OnRefreshMonster(long lRefeashIndex)
{
	//CServerRegion* pRegion = dynamic_cast<CServerRegion*>(GetFather());
	//if (pRegion == NULL)
	//	return;

	//DWORD dwNumMonster = 0;
	//list<CBaseObject*>* pList = GetObjectList();
	//if( pList )
	//{
	//	//##首先记录冬眠的怪物。
	//	vector<LONG> vMonsters;
	//	list<CBaseObject*>::iterator it = pList -> begin();
	//	for( ; it != pList -> end(); it ++ )
	//	{
	//		if( (*it) && (*it) -> GetType() == TYPE_MONSTER )
	//		{
	//			++ dwNumMonster;
	//			CMonster* pMonster = dynamic_cast<CMonster*>( *it );
	//			//##只删除冬眠的怪物
	//			if( pMonster && pMonster->GetProperty()->lRefeashIndex == lRefeashIndex &&
	//				pMonster -> GetAI() && pMonster -> GetAI() -> IsHibernated() )
	//			{
	//				// 在刷新范围内随机换一个位置
	//				CServerRegion::tagMonster * pRefeashMonster = pRegion->GetMonsterRefeash(pMonster->GetProperty()->lRefeashIndex);
	//				if (pRefeashMonster)
	//				{
	//					// 寻找刷新点
	//					long x, y;
	//					bool bRet = pRegion->GetRandomPosInRange(x, y, 
	//						pRefeashMonster->rectRange.left,
	//						pRefeashMonster->rectRange.top,
	//						pRefeashMonster->rectRange.right-pRefeashMonster->rectRange.left,
	//						pRefeashMonster->rectRange.bottom-pRefeashMonster->rectRange.top);

	//					if( bRet == false )
	//					{
	//						char str[256];
	//						_snprintf(str, 256, "无法为Monster “%s” 找到合适的位置！", pMonster->GetName());
	//						AddLogText(str);
	//						return;
	//					}

	//					// 周围有玩家的话
	//					long lX = x / AREA_WIDTH;
	//					long lY = y / AREA_HEIGHT;
	//					for( LONG i = 0; i < 9; i ++ )
	//					{
	//						CArea* pArea = pRegion -> GetArea( lX + _area[i][0], lY + _area[i][1] );
	//						if( pArea )
	//						{
	//							if( pArea -> GetAllPlayers().size() )
	//							{
	//								// 唤醒怪物
	//								if ( pMonster->GetAI() )
	//								{
	//									pMonster -> GetAI() -> WakeUp();
	//									break;
	//								}
	//							}
	//						}
	//					}

	//					pMonster->SetHP(pMonster->GetMaxHP());
	//					pMonster->SetPosXY(x, y);
	//				}
	//			}
	//		}
	//	}
	//}
}

//##该处的RemoveObject只是移出该物品在该Area内的索引，并不删除该对象。
//##对删除操作，一定要在Region那一层来控制，不能在迭代Area内Shape的同时调用这个函数。
//##否则极有可能导致iterator的指针错误。
void CArea::RemoveObject(CBaseObject* pObject)
{
	if( pObject )
	{
		CGoods* pGoods = dynamic_cast<CGoods*>( pObject );
		switch( pObject -> GetType() )
		{
		case TYPE_PLAYER:
			{
				long lOldPlayerNum = m_setPlayers.size();				
				m_setPlayers.erase( pObject->GetExID() );
				if (lOldPlayerNum == 1 && m_setPlayers.size()==0)
				{
					PlayerLeave();
				}
			}
			break;

		case TYPE_MONSTER:
			{				
				m_setActivedMonsters.erase( pObject->GetExID() );				
				m_setSleepedMonsters.erase( pObject->GetExID() );
			}
			break;

		case TYPE_PET:
			{				
				m_setPets.erase(pObject->GetExID());
			}
			break;

		case TYPE_GOODS:
			{				
				m_setGoods.erase( pObject->GetExID() );
			}
			break;

		case TYPE_COLLECTION:
			{				
				m_setCollection.erase( pObject->GetExID() );
			}
			break;

		case TYPE_CITYGATE:
			{
				m_setBuild.erase(pObject->GetExID());
			}
			break;
		}
	}
}

BOOL CArea::FindShapes(long lType,hashsetGUID& setOut)
{
	setOut.clear();
	switch( lType )
	{
	case TYPE_PLAYER:
		{
			setOut = m_setPlayers;
		}
		break;
	case TYPE_MONSTER:
		{
			setOut = m_setActivedMonsters;
		}
		break;
	case TYPE_PET:
		{
			setOut = m_setPets;
		}
		break;
	case TYPE_GOODS:
		{
			setOut = m_setGoods;
		}
		break;
	case TYPE_COLLECTION:
		{
			setOut = m_setCollection;
		}
		break;
	case TYPE_CITYGATE:
		{
			setOut=m_setBuild;
		}
		break;
	}
	return TRUE;
}

BOOL CArea::FindShapes( LONG lType, vector<CShape*>& vOut )
{
	BOOL bResult = FALSE;

	CServerRegion* pRegion = dynamic_cast<CServerRegion*>( GetFather() );
	if( pRegion )
	{
		CShape* pShape = NULL;
		switch( lType )
		{
		case TYPE_PLAYER:
			{
				for( hsGUIDIt it = m_setPlayers.begin(); it != m_setPlayers.end(); it++ )
				{
					pShape = (CShape*)( pRegion -> FindChildObject(lType, (*it)) );
					if( pShape )
					{
						vOut.push_back( pShape );
					}
				}
			}
			break;
		case TYPE_MONSTER:
			{
				for( hsGUIDIt it = m_setActivedMonsters.begin(); it != m_setActivedMonsters.end(); it++ )
				{
					pShape = (CShape*)( pRegion -> FindChildObject(lType, (*it)) );
					if( pShape )
					{
						vOut.push_back( pShape );
					}
				}
				for( hsGUIDIt it = m_setSleepedMonsters.begin(); it != m_setSleepedMonsters.end(); it++ )
				{
					pShape = (CShape*)( pRegion -> FindChildObject(lType, (*it)) );
					if( pShape )
					{
						vOut.push_back( pShape );
					}
				}
			}
			break;
		case TYPE_PET:
			{
				for( hsGUIDIt it = m_setPets.begin(); it != m_setPets.end(); it++ )
				{
					pShape = (CShape*)(pRegion->FindChildObject(lType, *it));
					if( pShape != NULL )
					{
						vOut.push_back( pShape );
					}
				}
			}
			break;
		case TYPE_GOODS:
			{
				for( hsGUIDIt it = m_setGoods.begin(); it != m_setGoods.end(); it++ )
				{
					pShape = dynamic_cast<CShape*>( pRegion -> FindChildObject(lType, (*it)) );
					if( pShape )
					{
						vOut.push_back( pShape );
					}
				}
			}
			break;
		case TYPE_COLLECTION:
			{
				for( hsGUIDIt it = m_setCollection.begin(); it != m_setCollection.end(); it++ )
				{
					pShape= dynamic_cast<CCollection*>(pRegion->FindChildObject(lType,(*it)));
					if (pShape)
					{
						vOut.push_back(pShape);
					}
				}
			}
			break;
		case TYPE_CITYGATE:
			{
				for( hsGUIDIt it = m_setBuild.begin(); it != m_setBuild.end(); it++ )
				{
					pShape= dynamic_cast<CGate*>(pRegion->FindChildObject(lType,(*it)));
					if (pShape)
					{
						vOut.push_back(pShape);
					}
				}
			}
			break;
		default:
			{
				PutDebugString("the lType param error!");
			}
			break;
		}
		bResult = TRUE;
	}
	return bResult;
}

BOOL CArea::GetAllShapes( vector<CShape*>& vOut )
{
	BOOL bResult = FALSE;
	if( FindShapes(TYPE_PLAYER, vOut) &&
		FindShapes(TYPE_PET, vOut) &&
		FindShapes(TYPE_MONSTER, vOut) &&
		FindShapes(TYPE_GOODS, vOut) &&
		FindShapes(TYPE_COLLECTION, vOut) &&
		FindShapes(TYPE_CITYGATE,vOut) )
	{						
		bResult = TRUE;
	}

	if( bResult == FALSE )
	{
		vOut.clear();
	}

	return bResult;
}

DWORD CArea::GetNumShapes()
{
	DWORD dwResult = 0; 
	dwResult = static_cast<DWORD>( m_setPlayers.size() + m_setActivedMonsters.size() + m_setSleepedMonsters.size() + m_setPets.size() + 
		m_setGoods.size() + m_setCollection.size()+m_setBuild.size());
	return dwResult;
}

//超时调用
long CArea::OnTimeOut(DWORD timerid,DWORD curtime,const void* var)
{		
	DWORD dwStartTime = timeGetTime();
	tagTimerVar* pTimvar = (tagTimerVar*)var;
	long lTimerType =pTimvar ->lvar1<<16 | pTimvar->TimerType;
	if (pTimvar->TimerType== ChildObjType_AreaHiberate)
	{
		DelEvent();
		if (m_iPlayerAreaAround==0)
		{
			Hibernate();
		}
	}
	else if (pTimvar->TimerType == ChildObjType_DelGoods)
	{
		if(NULL != pTimvar->pvar)
		{
			if(sizeof(stDestObj) == pTimvar->pvar_size)
			{
				OnGoodsDelTimeOut((stDestObj*)pTimvar->pvar);
				pTimvar->pvar = NULL;
				pTimvar->pvar_size = 0;
			}
			else
				assert(false);
		}	
	}

	else
		return 0;

	//
	//if ((int)var==ET_Hibernate && m_iPlayerAreaAround==0)
	//{
	//	Hibernate();
	//}
	GetGame()->AddTimerExpendTime(lTimerType,timeGetTime()-dwStartTime);
	return 0;
}
//注册事件
BOOL CArea::AddEvent(DWORD timerid,DWORD curtime)
{
	if (m_lTimerID==-1)
	{
		tagTimerVar* pTimvar = CreateTimerVar(35);
		pTimvar->TimerType = ChildObjType_AreaHiberate;
		m_lTimerID=GameManager::GetInstance()->Schedule(this,(void*)pTimvar,timeGetTime()+curtime,0);

		return TRUE;
	}
	return FALSE;

}
//删除事件
BOOL CArea::DelEvent()
{
	m_lTimerID= -1;
	return TRUE;
}
//注销事件
BOOL CArea::LogoutEvent(long& timerid)
{
	if (timerid!=-1)
	{
		GameManager::GetInstance()->Cancel(timerid , 0);
		timerid=-1;
		return TRUE;
	}
	return FALSE;
}

//休眠通知
VOID CArea::HibernateCall()
{
	--m_iPlayerAreaAround;
	if(m_iPlayerAreaAround<=0 && m_bIsHangUp== FALSE)
	{
		AddEvent(ET_Hibernate,AREA_HIBERNATE_DELAYTIME);
	}
}
//唤醒通知
VOID CArea::WakeUpCall()
{
	++m_iPlayerAreaAround;
	LogoutEvent(m_lTimerID);
	if (m_bIsHangUp==TRUE)
	{
		m_bIsHangUp=FALSE;
		WakeUp();
	}
}
//休眠本区域
VOID CArea::Hibernate()
{
	m_bIsHangUp=TRUE;
}
//唤醒本区域
VOID CArea::WakeUp()
{
	CServerRegion* pRegion = dynamic_cast<CServerRegion*>( GetFather() );
	for( hsGUIDIt it = m_setActivedMonsters.begin(); it != m_setActivedMonsters.end(); it++ )
	{
		CMonster* pShape = dynamic_cast<CMonster*>( pRegion -> FindChildObject(TYPE_MONSTER , (*it)) );
		if( pShape && pShape->GetAIHangupType()==0 && pShape->GetAI()->IsHibernated())
		{
			pShape->GetAI()->WakeUp();
		}
	}
}