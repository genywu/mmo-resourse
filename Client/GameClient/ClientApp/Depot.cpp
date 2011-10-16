/*DepotPageEx.cpp
*作者: 王峰
*创建日期:2008-1-18
*/

#include "stdafx.h"
#include "Depot.h"
#include "Goods/Goods.h"
#include "../ClientApp/Other/GlobeSetup.h"
#include "../GameClient/Game.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//**************************************************************************************
// 物品容器
//**************************************************************************************


/* 
*功能：构造函数
*细节描述： 
*参数： 无
*返回值： 无
*修改时间：2008-1-18
*/
CGoodsContainer::CGoodsContainer()
{
    m_dwContainerID	= 0xffffffff;
    m_numCubage	= 0;
}


/*
*功能：析构函数
*细节描述： 释放资源。
*参数： 无
*返回值： 无
*修改时间：2008-1-18
*/
CGoodsContainer::~CGoodsContainer()
{
    this->Clear();
}


/*
*功能：清空容器
*细节描述： 释放资源。
*参数： 无
*返回值： 无
*修改时间：2008-1-18
*/
void	CGoodsContainer::Clear()
{
    for( std::map<DWORD,CGoods*>::iterator  p = m_mapGoods.begin(); p != m_mapGoods.end(); p++ )
    {
        SAFE_DELETE( p->second );
    }

    m_mapGoods.clear();
}




/*
*功能：获取Goods
*细节描述： 
*参数： pos
*返回值： 无
*修改时间：2008-1-18
*/
CGoods*	CGoodsContainer::GetGoods( DWORD pos )
{
    std::map<DWORD,CGoods*>::iterator  p = m_mapGoods.find( pos );

    if( p != m_mapGoods.end() )
    {
        return p->second;
    }

    return NULL;
}



/*
*功能：获取Goods
*细节描述： 
*参数： guid
*返回值： Goods
*修改时间：2008-1-18
*/
CGoods*	CGoodsContainer::GetGoods( const CGUID& guid )
{
    for( std::map<DWORD,CGoods*>::iterator  p = m_mapGoods.begin(); p != m_mapGoods.end(); p++ )
    {
        if( p->second->GetExID() == guid )
        {
            return p->second;
        }
    }

    return NULL;
}


/*
*功能：获取位置
*细节描述： 
*参数： guid
*返回值： 位置
*修改时间：2008-1-18
*/
DWORD	CGoodsContainer::GetPos( const CGUID& guid )
{
    for( std::map<DWORD,CGoods*>::iterator  p = m_mapGoods.begin(); p != m_mapGoods.end(); p++ )
    {
        if( p->second->GetExID() == guid )
        {
            return p->first;
        }
    }

    return 0xffffffff;
}



/*
*功能：获取数量
*细节描述： 
*参数： 无
*返回值： Goods数量
*修改时间：2008-1-18
*/
DWORD	CGoodsContainer::GetAmount()
{
    return (DWORD)m_mapGoods.size();
}



/*
*功能：添加Goods
*细节描述： 
*参数： pGoods	物品
*		pos		位置
*返回值： 是否成功
*修改时间：2008-1-18
*/
bool	CGoodsContainer::AddGoods( CGoods* pGoods, DWORD pos )
{
    if( pGoods )
    {
        // 如果 原位置放有 相同的物品，则叠加
        CGoods* pOldGoods = this->GetGoods( pos );

        if( pOldGoods
            && pOldGoods->GetIndex() == pGoods->GetIndex()
            && ( pOldGoods->GetIndex() == CGlobeSetup::GetSetup()->dwMoneyIndex
            || pOldGoods->GetIndex() == CGlobeSetup::GetSetup()->dwSilverMoneyIndex
            || pOldGoods->GetNum() + pGoods->GetNum() <= pGoods->GetEffectVal( GAP_MAXOVERLAP, 1 ) ) )
        {
            pOldGoods->SetNum( pOldGoods->GetNum() + pGoods->GetNum() );

            SAFE_DELETE( pGoods );
        }
        // 否则添加新物品
        else
        {
            m_mapGoods[ pos ] = pGoods;
        }

        return true;
    }

    return false;
}



/*
*功能：删除Goods
*细节描述： 
*参数： guidSrc		要删除物品ID
*		guidDest	提出来的物品ID
*		num			删除数量
*返回值： 删除的物品
*修改时间：2008-1-18
*/
CGoods*	CGoodsContainer::DelGoods( const CGUID& guidSrc, const CGUID& guidDest, long num )
{
    // 查找物品
    std::map<DWORD,CGoods*>::iterator  p;
    for( p = m_mapGoods.begin(); p != m_mapGoods.end(); p++ )
    {
        if( p->second->GetExID() == guidSrc )
            break;
    }

    // 没找到则返回
    if( p == m_mapGoods.end() )
        return NULL;

    // 
    CGoods*	pNewGoods = NULL;
    CGoods* pOldGoods = p->second;

    // 如果 数量少于当前放置的物品 则拆分减少
    if( pOldGoods && pOldGoods->GetNum() > (long)num )
    {
        // 移出的部分
        pNewGoods = new CGoods( *pOldGoods );
        pNewGoods->SetNum( num );
        pNewGoods->SetExID( guidDest );

        // 残留的部分
        pOldGoods->SetNum( pOldGoods->GetNum() - num );
    }
    // 否则直接全部移出
    else
    {
        pNewGoods = pOldGoods;

        m_mapGoods.erase( p );
    }

    return pNewGoods;
}




/*
*功能：通常的物品解码
*细节描述： 
*参数： 无
*返回值： 
*修改时间：2008-1-18
*/
void	CGoodsContainer::Decode( DBReadSet& setReadDB )
{
    // 先清空
    this->Clear();

    // 获取 数量
    long num = setReadDB.GetLongFromByteArray();

    for( long c = 0; c < num; c++ )
    {
        CPlayer::tagGoods	stGoods;

        stGoods.bChange	= setReadDB.GetByteFromByteArray() != 0;
        stGoods.lNum	= setReadDB.GetWordFromByteArray();
        stGoods.lPos	= setReadDB.GetByteFromByteArray();
        stGoods.pGoods	= new CGoods;

        // 解码 Goods
        if( stGoods.pGoods->DecodeFromDataBlock( setReadDB ) )
        {
            // 保存Goods
            this->AddGoods( stGoods.pGoods, stGoods.lPos );

            // 处理套装
            WORD SuitID = (WORD)stGoods.pGoods->GetEffectVal( GAP_SUITID, 1 );			
            if( SuitID )
            {
                string stOrName = stGoods.pGoods->GetProperty()->strOrginName;					
                GetGame()->GetMainPlayer()->AddSuit( SuitID,stOrName );	
            }
        }
        // 失败就删除
        else
        {
            SAFE_DELETE( stGoods.pGoods );
        }
    }
}





/*
*功能：重新将子背包内的物品依次排列成数组
*细节描述： 
*参数： pMsg
*返回值： 无
*修改时间：2008-1-23
*/
void CGoodsContainer::Neaten( CMessage* pMsg )
{
    if( CGoodsContainer* pGC = CGCMana::Inst()->GetGC( m_dwContainerID ) )
    {
        std::map<DWORD,CGoods*> m_mapGoodsTmp;

        // 整理的GOODS数量
        DWORD num = pMsg->GetDWord();

        for( DWORD c = 0; c < num; c++ )
        {
            CGUID guidGoods;
            pMsg->GetGUID( guidGoods );

            if( CGoods* pGoods = pGC->GetGoods( guidGoods ) )
            {
                m_mapGoodsTmp[ pMsg->GetWord() ] = pGoods;
            }
            else
            {
                return;
            }
        }

        m_mapGoods.clear();
        m_mapGoods = m_mapGoodsTmp;

    }
}



//**************************************************************************************
// 物品容器管理
//**************************************************************************************

// 单件
CGCMana	CGCMana::m_Instance;


/* 
*功能：构造函数
*细节描述： 
*参数： 无
*返回值： 无
*修改时间：2008-1-18
*/
CGCMana::CGCMana()
{
}


/*
*功能：析构函数
*细节描述： 释放资源。
*参数： 无
*返回值： 无
*修改时间：2008-1-18
*/
CGCMana::~CGCMana()
{
    this->Clear();
}


/*
*功能：清空函数
*细节描述： 释放资源。
*参数： bGoodsOnly
*返回值： 无
*修改时间：2008-1-18
*/
void	CGCMana::Clear( bool bGoodsOnly )
{
    for( std::map<DWORD,CGoodsContainer*>::iterator  p = m_mapGC.begin(); p != m_mapGC.end(); p++ )
    {
        if( p->second )
        {
            if( bGoodsOnly )
            {
                p->second->Clear();
            }
            else
            {
                SAFE_DELETE( p->second );
            }
        }
    }
}


/*
*功能：获取数量
*细节描述： 
*参数： 无
*返回值： 无
*修改时间：2008-1-18
*/
DWORD	CGCMana::GetAmount( DWORD id )
{
    if( CGoodsContainer* pGC = this->GetGC( id ) )
    {
        return pGC->GetAmount();
    }

    return 0;
}


/*
*功能：添加物品容器
*细节描述： 
*参数： id
*		numCubage
*返回值： 
*修改时间：2008-1-18
*/
bool	CGCMana::AddGC( DWORD id, DWORD numCubage )
{
    std::map<DWORD,CGoodsContainer*>::iterator  p = m_mapGC.find( id );

    if( p == m_mapGC.end() )
    {
        m_mapGC[ id ] = new CGoodsContainer;
        m_mapGC[ id ]->SetContainerID( id );
        m_mapGC[ id ]->SetCubage( numCubage );

        return true;
    }

    return false;
}



/*
*功能：删除物品容器
*细节描述： 
*参数： id
*返回值： 
*修改时间：2008-1-18
*/
bool	CGCMana::DelGC( DWORD id )
{
    std::map<DWORD,CGoodsContainer*>::iterator  p = m_mapGC.find( id );

    if( p != m_mapGC.end() )
    {
        SAFE_DELETE( p->second );

        m_mapGC.erase( p );

        return true;
    }

    return false;
}



/*
*功能：获取物品容器
*细节描述： 
*参数： id
*返回值： 
*修改时间：2008-1-18
*/
CGoodsContainer*	CGCMana::GetGC( DWORD id )
{
    std::map<DWORD,CGoodsContainer*>::iterator  p = m_mapGC.find( id );

    if( p != m_mapGC.end() )
    {
        return p->second;
    }

    return NULL;
}



/*
*功能：添加物品
*细节描述： 
*参数： pGoods
*		id
*		pos
*返回值： 
*修改时间：2008-1-18
*/
bool	CGCMana::AddGoods( CGoods* pGoods, DWORD id, DWORD pos )
{
    if( CGoodsContainer* pGC = this->GetGC( id ) )
    {
        return pGC->AddGoods( pGoods, pos );
    }

    return false;
}



/*
*功能：删除Goods
*细节描述： 
*参数： guidSrc		要删除物品ID
*		guidDest	提出来的物品ID
*		num			删除数量
*返回值： 删除的物品
*修改时间：2008-1-18
*/
CGoods*	CGCMana::DelGoods( const CGUID& guidSrc, const CGUID& guidDest, DWORD num )
{
    for( std::map<DWORD,CGoodsContainer*>::iterator  p = m_mapGC.begin(); p != m_mapGC.end(); p++ )
    {
        if( CGoods* pGoods = p->second->DelGoods( guidSrc, guidDest, num ) )
            return pGoods;
    }

    return NULL;
}


/*
*功能：获取物品
*细节描述： 
*参数： id
*		pos
*返回值： 
*修改时间：2008-1-18
*/
CGoods*	CGCMana::GetGoods( DWORD id, DWORD pos )
{
    std::map<DWORD,CGoodsContainer*>::iterator  p = m_mapGC.find( id );

    if( p != m_mapGC.end() )
    {
        return p->second->GetGoods( pos );
    }

    return NULL;
}


/*
*功能：获取物品
*细节描述： 
*参数： guid
*返回值： 
*修改时间：2008-1-18
*/
CPlayer::tagGoods	CGCMana::GetGoods( const CGUID& guid )
{
    CPlayer::tagGoods	stGoods;

    for( std::map<DWORD,CGoodsContainer*>::iterator  p = m_mapGC.begin(); p != m_mapGC.end(); ++p )
    {
        CGoodsContainer* pGC = p->second;

        if( stGoods.pGoods = pGC->GetGoods( guid ) )
        {
            stGoods.lType = pGC->GetContainerID();
            stGoods.lPos = pGC->GetPos( guid );
            stGoods.lNum = stGoods.pGoods->GetNum();
            break;
        }
    }

    return stGoods;
}


//**************************************************************************************
// 银行管理
//**************************************************************************************

// 单件
CDepot	CDepot::m_Instance;


/* 
*功能：构造函数
*细节描述： 
*参数： 无
*返回值： 无
*修改时间：2008-1-18
*/
CDepot::CDepot()
{
    CGCMana::Inst()->AddGC( eDCT_Primary, DEPOT_PRIMARY_MAXNUM );
    CGCMana::Inst()->AddGC( eDCT_Secondary, DEPOT_SUBPACK_MAXNUM );
    CGCMana::Inst()->AddGC( eDCT_Gold, 1 );
    CGCMana::Inst()->AddGC( eDCT_Silver, 1 );

    for( long c = 0; c < DEPOT_SUBPACK_MAXNUM; c++ )
    {
        CGCMana::Inst()->AddGC( eDCT_Secondary1 + c, 0 );
    }
}


/*
*功能：析构函数
*细节描述： 释放资源。
*参数： 无
*返回值： 无
*修改时间：2008-1-18
*/
CDepot::~CDepot()
{
}


/*
*功能： 是否是 仓库容器
*细节描述： 
*参数： id
*返回值： 无
*修改时间：2008-1-23
*/
bool	CDepot::IsDepotGC( DWORD id )
{
    return id > DEPOT_CONTAINER_TYPE_BEGIN_VALUE
        && id < DEPOT_CONTAINER_TYPE_END_VALUE;
}


/*
*功能： 
*细节描述： 
*参数： 
*返回值： 无
*修改时间：2008-1-23
*/
CGUID	CDepot::GetGoldGUID()
{
    CGoods* pGoodsGold = CGCMana::Inst()->GetGoods( eDCT_Gold, 0 );

    return pGoodsGold ? pGoodsGold->GetExID() : NULL_GUID;
}



/*
*功能： 
*细节描述： 
*参数： 
*返回值： 无
*修改时间：2008-1-23
*/
CGUID	CDepot::GetSilverGUID()
{
    CGoods* pGoodsSilver = CGCMana::Inst()->GetGoods( eDCT_Silver, 0 );

    return pGoodsSilver ? pGoodsSilver->GetExID() : NULL_GUID;
}



/*
*功能： 
*细节描述： 
*参数： 
*返回值： 无
*修改时间：2008-1-23
*/
long	CDepot::GetGoldAmount()
{
    CGoods* pGoodsGold = CGCMana::Inst()->GetGoods( eDCT_Gold, 0 );

    return pGoodsGold ? pGoodsGold->GetNum() : 0;
}


/*
*功能： 
*细节描述： 
*参数： 
*返回值： 无
*修改时间：2008-1-23
*/
long	CDepot::GetSilverAmount()
{
    CGoods* pGoodsSilver = CGCMana::Inst()->GetGoods( eDCT_Silver, 0 );

    return pGoodsSilver ? pGoodsSilver->GetNum() : 0;
}



/*
*功能： 获取子背包容量
*细节描述： 
*参数： idSubPack	子背包的容器编号
*返回值： 子背包UI
*修改时间：2008-1-25
*/
DWORD	CDepot::GetSubPackCubage( DWORD idSubPack )
{
    if( CGoods* pGoods =  CGCMana::Inst()->GetGC( eDCT_Secondary )->GetGoods( idSubPack - eDCT_Secondary1 + DEPOT_SUBPACK_BASEPOS ) )
    {
        return pGoods->GetEffectVal( GAP_PACK_CAPACITY, 1 );
    }

    return 0;
}



/*
*功能： 解码 仓库头信息
*细节描述： 
*参数： setReadDB	数据源
*返回值： 无
*修改时间：2008-1-23
*/
void	CDepot::DecodeDepotHeadInfo( DBReadSet& setReadDB)
{
    // 获得仓库信息头，并保存数据
    setReadDB.GetBufferFromByteArray( &m_stDepotInfo, sizeof(m_stDepotInfo) );

    // 复位
    m_bHasPassport = false;
    ZeroMemory( m_szOldPW, DEPOT_PASSWORD_ARRAY_LEN );
    ZeroMemory( m_szNewPW, DEPOT_PASSWORD_ARRAY_LEN );
}


/*
*功能： 解码 仓库信息
*细节描述： 
*参数： setReadDB	数据源
*返回值： 无
*修改时间：2008-1-23
*/
void	CDepot::DecodeDepotInfo( DBReadSet& setReadDB )
{
    // 获取 主仓库 物品
    CGCMana::Inst()->GetGC( eDCT_Primary )->Decode( setReadDB );

    // 扩展栏位的 编解码 需要统一下
    CGCMana::Inst()->GetGC( eDCT_Secondary )->Clear();

    BYTE numSubItemGoods = setReadDB.GetByteFromByteArray();

    for( BYTE c = 0; c < numSubItemGoods; c++ )
    {
        CGoods* pGoods	= new CGoods;

        // 解码
        if( pGoods->DecodeFromDataBlock( setReadDB ) )
        {
            DWORD pos = DEPOT_SUBPACK_BASEPOS + (DWORD)setReadDB.GetByteFromByteArray();

            CGCMana::Inst()->GetGC( eDCT_Secondary )->AddGoods( pGoods, pos );
        }
        else	// 失败就删除
        {
            setReadDB.GetByteFromByteArray();

            SAFE_DELETE( pGoods );
        }
    }

    // 获取 子背包 物品
    for( long c = 0; c < DEPOT_SUBPACK_MAXNUM; c++ )
    {
        CGCMana::Inst()->GetGC( eDCT_Secondary1 + c )->Decode( setReadDB );
    }


    // 金币
    CGCMana::Inst()->GetGC( eDCT_Gold )->Clear();

    long numGold = setReadDB.GetLongFromByteArray();
    CGUID guidGold;
    setReadDB.GetBufferFromByteArray( guidGold );

    if( numGold && NULL_GUID != guidGold )
    {
        CGoods* pGoodsGold	= new CGoods;
        pGoodsGold->SetExID( guidGold );
        pGoodsGold->SetIndex( CGlobeSetup::GetSetup()->dwMoneyIndex );
        pGoodsGold->GetProperty()->dwType = GBT_MONEY;
        pGoodsGold->SetNum( numGold );

        CGCMana::Inst()->GetGC( eDCT_Gold )->AddGoods( pGoodsGold, 0 );
    }

    // 银币
    CGCMana::Inst()->GetGC( eDCT_Silver )->Clear();

    long numSilver = setReadDB.GetLongFromByteArray();
    CGUID guidSilver;
    setReadDB.GetBufferFromByteArray( guidSilver );

    if( numSilver && NULL_GUID != guidSilver )
    {
        CGoods* pGoodsSilver	= new CGoods;
        pGoodsSilver->SetExID( guidSilver );
        pGoodsSilver->SetIndex( CGlobeSetup::GetSetup()->dwSilverMoneyIndex );
        pGoodsSilver->GetProperty()->dwType = GBT_SILVERMONEY;
        pGoodsSilver->SetNum( numSilver );

        CGCMana::Inst()->GetGC( eDCT_Silver )->AddGoods( pGoodsSilver, 0 );
    }
}




/*
*功能： 发送 打开协议
*细节描述： 
*参数： pText	密码字符串
*返回值： 无
*修改时间：2008-1-21
*/
void	CDepot::SendEnterMsg( LPCTSTR pText )
{
    CMessage msg( MSG_C2S_DEPOT_INPUT_PWD );

    msg.AddEx( (void*)pText, DEPOT_PASSWORD_SIZE );

    msg.Send();
}



/*
*功能： 发送 设置密码协议
*细节描述： 
*参数： pText	密码字符串
*返回值： 无
*修改时间：2008-1-21
*/
void	CDepot::SendSetPWMsg( LPCTSTR pText )
{
    CMessage msg( MSG_C2S_DEPOT_REQUEST_CHANGE_PWD );

    msg.AddEx( (void*)m_szOldPW, DEPOT_PASSWORD_SIZE );
    msg.AddEx( (void*)pText, DEPOT_PASSWORD_SIZE );

    msg.Send();
}

/*
*功能： 接收 打开协议
*细节描述： 
*参数： pMsg	协议结构
*返回值： 无
*修改时间：2008-1-21
*/
void	CDepot::RecvEnterMsg( CMessage* pMsg )
{
    //CDepotPageEx* pDepotPageEx = GetGame()->GetCGuiEx()->GetDepotPageEx();

    // 检测 成功失败
    if( eDMC_Succeed == pMsg->GetLong() )
    {
        // 获得通行证
        m_bHasPassport = true;

        // 解码仓库物品数据
        DBReadSet setReadDB;
        pMsg->GetDBReadSet( setReadDB );
        this->DecodeDepotInfo( setReadDB );

        //pDepotPageEx->Open();
    }
    else
    {
        // 处理错误，"密码错误，打开失败"
        //GetGame()->GetCGuiEx()->GetMessageBoxPageEx()->m_lCallerID = -1;
        //GetGame()->GetCGuiEx()->GetMessageBoxPageEx()->Open( NULL, pDepotPageEx->GetStrRes( eIDS_PasswordError ), CMessageBoxPageEx::MSG_OK, true );

       // GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddOtherInfo( pDepotPageEx->GetStrRes( eIDS_PasswordError ) );
    }
}

/*
*功能： 接收 设置密码协议
*细节描述： 
*参数： pMsg	协议结构
*返回值： 无
*修改时间：2008-1-21
*/
void	CDepot::RecvSetPWMsg( CMessage* pMsg )
{
   
    // 检测 成功失败
    if( eDMC_Succeed == pMsg->GetLong() )
    {
        // 空密码字符串则表示没有密码
        m_stDepotInfo.lHasPassword = (long)strlen( this->GetNewPW() );

        this->SetOldPW( this->GetNewPW() );

        // "密码设置成功"
    }
    else
    {
        // 处理错误，"密码设置失败"
    }
}




/*
*功能： 发送 购买栏位协议
*细节描述： 
*参数： 无
*返回值： 无
*修改时间：2008-1-21
*/
void	CDepot::SendBuyItemMsg()
{
    CMessage msg( MSG_C2S_DEPOT_REQUEST_BUY_SUBPACK );

    msg.Send();
}



/*
*功能： 接收 购买栏位协议
*细节描述： 
*参数： pMsg	协议结构
*返回值： 无
*修改时间：2008-1-21
*/
void	CDepot::RecvBuyItemMsg( CMessage* pMsg )
{
   

    // 检测 成功失败
    if( eGTC_Succeed == pMsg->GetLong() )
    {

    }
    else
    {
        // 处理错误，"购买栏位失败"

    }
}



/*
*功能： 接收 冻结协议
*细节描述： 
*参数： pMsg	协议结构
*返回值： 无
*修改时间：2008-2-2
*/
void	CDepot::RecvFreezeMsg( CMessage* pMsg )
{
    m_stDepotInfo.lThawDate = pMsg->GetLong();

    // "由于多次输入错误密码，仓库已被冻结，请等待解冻"
}



/*
*功能： 排序比较
*细节描述： 
*参数： pstGoods1
*		pstGoods2
*返回值： bool
*修改时间：2009-1-1
*/
bool	CDepot::Compare( const CPlayer::tagGoods* pstGoods1, const CPlayer::tagGoods* pstGoods2 )
{
    if( !pstGoods1 || !pstGoods2 || !pstGoods1->pGoods || !pstGoods2->pGoods )
        return false;

    if( pstGoods1->pGoods->GetIndex() != pstGoods2->pGoods->GetIndex() )
    {
        LPCSTR	szOrginName1 = pstGoods1->pGoods->GetOrginName();
        LPCSTR	szOrginName2 = pstGoods2->pGoods->GetOrginName();

        if( szOrginName1 && szOrginName2 )
            return lstrcmp( szOrginName1, szOrginName2 ) < 0;
    }
    else if( pstGoods1->lNum != pstGoods2->lNum )
    {
        return pstGoods1->lNum > pstGoods2->lNum;
    }

    return false;
}
