/**
*	file:		SpecialEquipContainer.cpp
*	Brief:		法宝装备容器
*	detail:		法宝装备栏位， 天字、地字、人字
*	Authtor:	吴畏
*	Date:		2010-08-24
*/
#include "stdafx.h"
#include "SpecialEquipContainer.h"
#include "../Player.h"
#include "../Goods/CGoods.h"
#include "../Goods/CGoodsFactory.h"
#include "../MessagePackage/CS2CContainerObjectMove.h"
#include "../../../../Public/Setup/GoodsSetup.h"

DWORD CSpecialEquipContainer::s_dwEquipLimit = 8;

CSpecialEquipContainer::CSpecialEquipContainer()
{
    // 天字、地字及人字栏位均为8个
    m_vCells.resize(s_dwEquipLimit, NULL_GUID);
    AddListener(this);
}

CSpecialEquipContainer::~CSpecialEquipContainer()
{
    Release();
}

bool CSpecialEquipContainer::Add(CBaseObject* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid /* = NULL */)
{
    bool bResult = false;
    if(IsFull())
        return bResult;

    CGoods* pGoods = dynamic_cast<CGoods*>(pObject);
    if(pGoods == NULL)
        return bResult;

    CGoodsBaseProperties* pProperties = GoodsSetup::QueryGoodsBaseProperties(pGoods->GetBasePropertiesIndex());
    if(pProperties)
    {
        if(pGoods->GetTrumpType() == m_eTrumpType)
        {
            DWORD dwPosition = 0;
            if(FindPositionForGoods(pGoods, dwPosition))
            {
                bResult = Add(dwPosition, pGoods, pPreviousContainer, pVoid);
            }
        }       
    }


    return bResult;
}

bool CSpecialEquipContainer::Add(DWORD dwPostion, CGoods* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid /* = NULL */)
{
    bool bResult = false;
    if(pObject)
    {
        goodsIt it = m_mGoods.find(pObject->GetExID());
        if(it != m_mGoods.end())
            return bResult;

        CGoods* pGoods = GetGoods(dwPostion);
        if(pGoods)
        {
            // 位置已经被其他物品占用
            return bResult;
        }

        CGoodsBaseProperties* pProperties = GoodsSetup::QueryGoodsBaseProperties(pObject->GetBasePropertiesIndex());
        if(pProperties)
        {
            // 不是指定类型装备法宝
            if(pGoods->GetTrumpType() == m_eTrumpType)
            {
                m_mGoods[pObject->GetExID()] = pObject;
                m_vCells[dwPostion] = pObject->GetExID();

                for(size_t i = 0; i< m_vListeners.size(); i++)
                {
                    if(m_vListeners[i])
                    {
                        m_vListeners[i]->OnObjectAdded(this, pObject, pObject->GetAmount(), pVoid);
                    }
                }

                bResult = true;
            }
        }

    }
    return bResult;
}

CBaseObject* CSpecialEquipContainer::Find(const CGUID& guid)
{
    CGoods* pGoods = NULL;
    goodsIt it = m_mGoods.find(guid);
    if(it != m_mGoods.end())
    {
        pGoods = it->second;
    }

    return pGoods;
    
}

CBaseObject* CSpecialEquipContainer::Remove(const CGUID& guid, LPVOID pVoid )
{
    CGoods* pResult = NULL;
    goodsIt it = m_mGoods.find( guid );

    if( it != m_mGoods.end() )
    {
        //if( IsLocked(it -> second) == false )		
        pResult = it -> second;		
    }

    if( pResult )
    {
        for( size_t i = 0; i < m_vListeners.size(); i ++ )
        {
            if( m_vListeners[i] )
            {
                m_vListeners[i] -> OnObjectRemoved( this, pResult, pResult->GetAmount(), pVoid );
            }
        }
        m_mGoods.erase( it );
    }

    return pResult;
}

CBaseObject* CSpecialEquipContainer::Remove(CBaseObject* pObject, LPVOID pVoid /* = NULL  */)
{
    return CGoodsContainer::Remove( pObject, pVoid );
}

void CSpecialEquipContainer::Clear(LPVOID pVoid )
{
    goodsIt it = m_mGoods.begin(); 
    for( ; it != m_mGoods.end(); it ++ )
    {
        if( it -> second == NULL )
        {
            continue;
        }
        for( size_t i = 0; i < m_vListeners.size(); i ++ )
        {
            if( m_vListeners[i] )
            {
                m_vListeners[i] -> OnObjectRemoved( this, it -> second, 
                    it -> second -> GetAmount(), pVoid );
            }
        }
    }
    m_mGoods.clear();
    m_vCells.clear();
}

void CSpecialEquipContainer::Release()
{
    if(GetContainerMode() == CGoodsContainer::GCM_NORMAL)
    {
        goodsIt it = m_mGoods.begin();
        for(; it != m_mGoods.end(); ++it)
        {
            CGoodsFactory::GarbageCollect(&(it->second), 2);
        }
    }
    m_mGoods.clear();
    m_vCells.clear();
    CGoodsContainer::Release();
    AddListener(this);
}

void CSpecialEquipContainer::TraversingContainer(CContainerListener* pListener)
{
    if(pListener == NULL)
        return;

    goodsIt it = m_mGoods.begin();
    for(; it != m_mGoods.end(); ++it)
    {
        if(it->second)
        {
            pListener->OnTraversingContainer(this, it->second);
        }
    }
}

bool CSpecialEquipContainer::IsGoodsExisted(DWORD dwGoodsBasePropertiesIndex)
{
    bool bResult = false;
    goodsIt it = m_mGoods.begin();
    for(; it != m_mGoods.end(); ++it)
    {
        if(it->second)
        {
            if(it->second->GetBasePropertiesIndex() == dwGoodsBasePropertiesIndex)
            {
                bResult = true;
                break;
            }            
        }
    }
    return bResult;
}

bool CSpecialEquipContainer::OnObjectAdded(CContainer* pContainer, CBaseObject* pObj, DWORD dwAmount, LPVOID pVoid)
{
    CGoodsContainer* pGoodsContainer	= dynamic_cast<CGoodsContainer*>( pContainer );
    CGoods* pGoods						= dynamic_cast<CGoods*>( pObj );
    CS2CContainerObjectMove* pMsg		= reinterpret_cast<CS2CContainerObjectMove*>( pVoid );

    if( pGoodsContainer && pGoods && GetOwnerType() == TYPE_PLAYER )
    {
        CPlayer* pPlayer = GetGame() -> FindPlayer( GetOwnerID() );
        if( pPlayer )
        {
            DWORD dwPosition = 0;
            if( pGoodsContainer -> QueryGoodsPosition(pGoods, dwPosition) )
            {
                pGoods->RunScriptOnEquip(pPlayer);				

                pPlayer->MountEquipEx(pGoods,true);
                pPlayer->UpdateAttribute();

                //CMessage msg( MSG_S2C_PLAYER_CHANGE_EQUIPMENT );
                //msg.Add( GetOwnerID() );
                //msg.Add( dwPosition );
                //msg.Add( BYTE(1) /*表示穿上装备*/ );
                //msg.Add( pGoods -> GetBasePropertiesIndex() );
                //msg.Add( pGoods -> GetAddonPropertyValues(CGoodsBaseProperties::GAP_WEAPON_LEVEL, 1) );
                //msg.SendToAround( pPlayer, pPlayer );


                //pPlayer->ProduceEvent(ET_EquipMount,CS2CContainerObjectMove::PEI_EQUIPMENT,dwPosition,pGoods->GetGoodsBaseType(), 
                //    pGoods-> GetAddonPropertyValues( CGoodsBaseProperties::GAP_WEAPON_CATEGORY, 1 ),
                //    (void*)GoodsSetup::QueryGoodsOriginalName(pGoods->GetBasePropertiesIndex()));
            }
        }
    }
    if( pGoodsContainer && pGoods && pMsg )
    {
        DWORD dwPosition = 0;
        if( pGoodsContainer -> QueryGoodsPosition(pGoods, dwPosition) )
        {
            pMsg -> SetDestinationContainer( GetOwnerType(), GetOwnerID(), dwPosition );
            pMsg -> SetDestinationObject( pGoods -> GetType(),pGoods->GetExID() );
            pMsg -> SetDestinationObjectAmount( pGoods -> GetAmount() );
        }
    }
    return true;
}

bool CSpecialEquipContainer::OnObjectRemoved(CContainer* pContainer, CBaseObject* pObj, DWORD dwAmount, LPVOID pVoid)
{
    CGoodsContainer* pGoodsContainer	= dynamic_cast<CGoodsContainer*>( pContainer );
    CGoods* pGoods						= dynamic_cast<CGoods*>( pObj );
    CS2CContainerObjectMove* pMsg		= reinterpret_cast<CS2CContainerObjectMove*>( pVoid );

    if( pGoodsContainer && pGoods && GetOwnerType() == TYPE_PLAYER )
    {
        CPlayer* pPlayer = GetGame() -> FindPlayer( GetOwnerID() );
        if( pPlayer )
        {
            //##为了实现刷新属性,必须先移出该物品.但是刷新后又必须放回去.
            //##才能保证逻辑的正确性.
            DWORD dwPosition = 0;
            if( pGoodsContainer -> QueryGoodsPosition(pGoods, dwPosition) )
            {
                goodsIt it = m_mGoods.find(pGoods->GetExID());
                if( it != m_mGoods.end() )
                {
                    pGoods->RunScriptOnRemoveEquip(pPlayer);
                    //##保存零时变量
                    CGoods* pUnmountedEquipment = it -> second;
                    it -> second = NULL;

                    LONG lCurrentDurability = pUnmountedEquipment->GetAddonPropertyValues( GAP_GOODS_MAX_DURABILITY, 2 );
                    if( !(pGoods->HasAddonProperty(GAP_GOODS_MAX_DURABILITY)&&lCurrentDurability == 0) )
                        pPlayer->MountEquipEx(pGoods,false);
                    pPlayer->UpdateAttribute( true );

                    //##恢复
                    it -> second = pUnmountedEquipment;
                }

                //##给周围的玩家发送消息.
                //CMessage msg( MSG_S2C_PLAYER_CHANGE_EQUIPMENT );
                //msg.Add( GetOwnerID() );
                //msg.Add( dwPosition );
                //msg.Add( BYTE(0) /*表示卸下装备*/ );
                //msg.Add( pGoods -> GetBasePropertiesIndex() );
                //msg.Add( pGoods -> GetAddonPropertyValues(CGoodsBaseProperties::GAP_WEAPON_LEVEL, 1) );
                //msg.SendToAround( pPlayer );
                ////pPlayer->Proc(EVENT_OnEquipUnMount,(const void*)dwPosition,(const void *)(pGoods -> GetBasePropertiesIndex()));
                //pPlayer->ProduceEvent(ET_EquipUnmount,CS2CContainerObjectMove::PEI_EQUIPMENT,dwPosition,
                //    pGoods->GetGoodsBaseType(), pGoods-> GetAddonPropertyValues( CGoodsBaseProperties::GAP_WEAPON_CATEGORY, 1 ),  
                //    (void*)GoodsSetup::QueryGoodsOriginalName(pGoods->GetBasePropertiesIndex()));
            }
        }
    }
    if( pGoodsContainer && pGoods && pMsg )
    {
        DWORD dwPosition = 0;
        if( pGoodsContainer -> QueryGoodsPosition(pGoods, dwPosition) )
        {
            pMsg -> SetSourceContainer( GetOwnerType(), GetOwnerID(), dwPosition );
            pMsg -> SetSourceObject( pGoods -> GetType(),pGoods->GetExID() );
            pMsg -> SetSourceObjectAmount( dwAmount );
        }
    }
    return true;
}

void CSpecialEquipContainer::AI()
{
    goodsIt it = m_mGoods.begin();
    for(; it != m_mGoods.end(); ++it)
    {
        if(it->second)
        {
            it->second->AI();
        }
    }
}

bool CSpecialEquipContainer::IsFull()
{
    bool bResult = false;
    if(GetGoodsAmount() == s_dwEquipLimit)
    {
        bResult = true;
    }

    return bResult;
}

bool CSpecialEquipContainer::QueryGoodsPosition(const CGUID& guid, DWORD& dwPosition)
{
    for(size_t i = 0; i<m_vCells.size(); i++)
    {
        if(m_vCells[i] == guid)
        {
            dwPosition = static_cast<DWORD>(i);
            return true;
        }
    }
    return false;
}

bool CSpecialEquipContainer::QueryGoodsPosition(CGoods* pGoods, DWORD& dwPositon)
{
    if(pGoods == NULL)
        return false;

    return QueryGoodsPosition(pGoods->GetExID(), dwPositon);
}

CGoods* CSpecialEquipContainer::GetGoods(DWORD dwPosition)
{
    CGoods* pGoods = NULL;
    if(dwPosition >= m_vCells.size())
    {
        return pGoods;
    }

    if(m_vCells[dwPosition] == CGUID::GUID_INVALID)
    {
        return pGoods;
    }

    return dynamic_cast<CGoods*>(Find(m_vCells[dwPosition]));
}


DWORD CSpecialEquipContainer::GetGoodsAmount()
{
    DWORD dwAmount = 0;
    goodsIt it = m_mGoods.begin();
    for(; it != m_mGoods.end(); ++it)
    {
        if(it->second &&        
            GoodsSetup::QueryGoodsBaseProperties(it->second->GetBasePropertiesIndex()))
        {
            ++dwAmount;
        }
    }

    return dwAmount;

}

bool CSpecialEquipContainer::FindPositionForGoods(CGoods* pGoods, DWORD& dwPosition)
{
    if(pGoods == NULL)
        return false;

    goodsIt it = m_mGoods.find(pGoods->GetExID());
    if(it != m_mGoods.end())
        return false;

    for(size_t i = 0; i < m_vCells.size(); i++)
    {
        if(m_vCells[i] == CGUID::GUID_INVALID)
        {
            dwPosition = static_cast<DWORD>(i);
            return true;
        }
    }

    return false;
}

bool CSpecialEquipContainer::Swap(DWORD dwPosition, CGoods* pIn, CGoods** ppOut, LPVOID pVoid /* = NULL */)
{
    bool bResult = false;
    if(pIn && ppOut)
    {
        CGoodsBaseProperties* pProperties = GoodsSetup::QueryGoodsBaseProperties(pIn->GetBasePropertiesIndex());
        if(pProperties && pIn->GetTrumpType() == m_eTrumpType)
        {
            // 复合特定类型的装备法宝
            CGoods* pOriginalGoods = GetGoods(dwPosition);
            CPlayer* pPlayer = GetGame()->FindPlayer(GetOwnerID());
            if(pOriginalGoods && pPlayer)
            {
                Remove(pOriginalGoods->GetExID());
                if(Add(dwPosition, pIn, NULL))
                {
                    CS2CContainerObjectMove* pMsg = reinterpret_cast<CS2CContainerObjectMove*>( pVoid );
                    if(pMsg)
                    {
                        pMsg->SetSourceObject(pIn->GetType(), pIn->GetExID());
                        pMsg->SetDestinationObject(pOriginalGoods->GetType(), pOriginalGoods->GetExID());
                        *ppOut = pOriginalGoods;
                        bResult = true;
                    }
                }
            }

        }
    }
    return bResult;
}

bool CSpecialEquipContainer::Serialize(DBWriteSet& setWriteDB, bool b )
{
    setWriteDB.AddToByteArray(GetGoodsAmount());
    DWORD dwPos = 0;
    goodsIt it = m_mGoods.begin();
    for(; it != m_mGoods.end(); ++it)
    {
        if(it->second)
        {
            if(QueryGoodsPosition(it->second, dwPos))
            {
                setWriteDB.AddToByteArray(dwPos);
                it->second->Serialize(setWriteDB, b);
            }
        }
    }
    return true;
}

bool CSpecialEquipContainer::SerializeForClient(DBWriteSet& setWriteDB, bool b )
{
    setWriteDB.AddToByteArray(GetGoodsAmount());
    DWORD dwPos = 0;
    goodsIt it = m_mGoods.begin();
    for(; it != m_mGoods.end(); ++it)
    {
        if(it->second)
        {
            if(QueryGoodsPosition(it->second, dwPos))
            {
                setWriteDB.AddToByteArray(static_cast<BYTE>(dwPos));
                it->second->Serialize(setWriteDB, b);
            }
        }
    }
    return true;
}

bool CSpecialEquipContainer::Unserialize(DBReadSet& setReadDB, bool b )
{
    Clear();
    
    DWORD dwGoodsAmount = setReadDB.GetDwordFromByteArray();
    for(DWORD i = 0; i < dwGoodsAmount; i++)
    {
        DWORD dwPos = setReadDB.GetDwordFromByteArray();
        CGoods* pGoods = CGoodsFactory::UnserializeGoods(setReadDB, 19);
        if(pGoods && pGoods->GetAmount() > 0)
        {
            CGoodsFactory::UpdateEquipmentAttr(pGoods);
            Add(dwPos, pGoods, NULL);
        }
    }
    return true;
}