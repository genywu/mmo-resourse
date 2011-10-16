/**
*	file:		PlayerPack.cpp
*	Brief:		背包类及货币类
*	detail:		关于背包、子背包及货币等的一些编码操作
*	Authtor:	吴畏
*	Date:		2010-07-05
*/

#include "stdafx.h"
#include "PlayerPack.h"
#include "Player.h"
#include "../../../Public/Setup/GoodsSetup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CPlayerPack::CPlayerPack(CPlayer* pPlayer)
:m_pPlayer(pPlayer)
,m_cWallet(GoodsSetup::GetGoldCoinIndex())
,m_cSilverWallet(GoodsSetup::GetSilverIndex())
,m_cGem(GoodsSetup::GetGemIndex())
{
    //原始背包
    m_pOrigGoodsContainer = OBJ_CREATE(CVolumeLimitGoodsContainer);
    m_pOrigGoodsContainer->SetContainerVolume(20);
    m_pSubpackContainer=OBJ_CREATE(CSubpackContainer);

    m_pOrigGoodsContainer->AddListener(m_pPlayer);
}

CPlayerPack::~CPlayerPack()
{
    Release();
}

void CPlayerPack::Release()
{
    m_pPlayer = NULL;

    OBJ_RELEASE(CVolumeLimitGoodsContainer, m_pOrigGoodsContainer);
    OBJ_RELEASE(CSubpackContainer, m_pSubpackContainer);
}

void CPlayerPack::SetOwner(CPlayer* pPlayer)
{
    if(pPlayer == NULL)
        return;

    m_pOrigGoodsContainer->SetOwner(pPlayer->GetType(), pPlayer->GetExID());
    m_pSubpackContainer->SetOwner(pPlayer->GetType(), pPlayer->GetExID());
    m_cWallet.SetOwner(pPlayer->GetType(), pPlayer->GetExID());
    m_cSilverWallet.SetOwner(pPlayer->GetType(), pPlayer->GetExID());
    m_cGem.SetOwner(pPlayer->GetType(), pPlayer->GetExID());
}

void CPlayerPack::CodeToDataBlockForServer(DBWriteSet& setWriteDB)
{
#ifdef _RUNSTACKINFO1_
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"Code OriginPack Start");
#endif
    m_pOrigGoodsContainer->Serialize(setWriteDB);
#ifdef _RUNSTACKINFO1_
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"Code Subpack Start");
#endif
    m_pSubpackContainer->Serialize(setWriteDB);

    // 货币类
    m_cWallet.Serialize(setWriteDB);
    m_cSilverWallet.Serialize(setWriteDB);
    m_cGem.Serialize(setWriteDB);
}

void CPlayerPack::DecordeDataBlockFromServer(DBReadSet& setReadDB)
{
#ifdef _RUNSTACKINFO1_
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"Decode package start.");
#endif
    m_pOrigGoodsContainer->Unserialize(setReadDB);
    for(DWORD l=0; l<m_pOrigGoodsContainer->GetVolume(); l++)
    {
        m_pPlayer->CheckGoodsTimeOut(m_pOrigGoodsContainer->GetGoods(l));
    }

#ifdef _RUNSTACKINFO1_
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"Decode SubpackContainer start.");
#endif
    m_pSubpackContainer->Unserialize(setReadDB);
    for(long j=0; j<CSubpackContainer::SUBPACK_NUM; j++)
    {
        for (long j = 0; j < (long)m_pSubpackContainer->GetSubpack(j)->pSubpackContainer->GetVolume(); ++j)
        {
            CGoods *pTmpGoods = m_pSubpackContainer->GetSubpack(j)->pSubpackContainer->GetGoods(j);
            m_pPlayer->CheckGoodsTimeOut(pTmpGoods);
        }
    }

    // 货币类
    m_cWallet.Unserialize(setReadDB);
    m_cSilverWallet.Unserialize(setReadDB);
    m_cGem.Unserialize(setReadDB);

}

void CPlayerPack::CodeToDataBlockForClient(DBWriteSet& setWriteDB)
{

#ifdef _RUNSTACKINFO1_
    char pszStatckInfo[1024]="";
    _snprintf(pszStatckInfo,1024,"Code OriginPack(Num:%d) Start",m_pPlayer->getOriginPack()->GetGoodsAmount());
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStatckInfo);
#endif
    // 主背包
    setWriteDB.AddToByteArray((long)4);
    setWriteDB.AddToByteArray((LONG)m_pOrigGoodsContainer->GetGoodsAmount());
    CPacketListener cPacketListener;
    cPacketListener.m_pDBWriteData = &setWriteDB;
    m_pOrigGoodsContainer->TraversingContainer(&cPacketListener);

    // 子背包
    BYTE btSubAmount = static_cast<BYTE>(m_pSubpackContainer->GetGoodsAmount());
    setWriteDB.AddToByteArray(btSubAmount);
    for(BYTE c=0; c<CSubpackContainer::SUBPACK_NUM; c++)
    {
        if(m_pSubpackContainer->GetSubpack(c)->pGoods)
        {
            m_pSubpackContainer->GetSubpack(c)->pGoods->SerializeForOldClient(setWriteDB);
            setWriteDB.AddToByteArray(c);
        }
    }

    // 子背包里的物品
    for(int i=0; i<CSubpackContainer::SUBPACK_NUM; i++)
    {
        LONG lGoodsNum = m_pSubpackContainer->GetSubpack(i)->pSubpackContainer->GetGoodsAmount();
        setWriteDB.AddToByteArray(lGoodsNum);
        CPacketListener cPacketListener;
        cPacketListener.m_pDBWriteData = &setWriteDB;
        m_pSubpackContainer->GetSubpack(i)->pSubpackContainer->TraversingContainer( &cPacketListener );
    }

    setWriteDB.AddToByteArray((long)5);
    setWriteDB.AddToByteArray( m_cWallet.GetGoldCoinsAmount() );
    CGoods* pGold = m_cWallet.GetGoods( 0 );
    if( pGold )
    {
        setWriteDB.AddToByteArray( static_cast<CGUID>(pGold -> GetExID()) );
    }
    else
    {
        setWriteDB.AddToByteArray( CGUID::GUID_INVALID );
    }

    setWriteDB.AddToByteArray( m_cSilverWallet.GetGoldCoinsAmount() );
    CGoods* pSilver = m_cSilverWallet.GetGoods( 0 );
    if( pSilver )
    {
        setWriteDB.AddToByteArray( static_cast<CGUID>(pSilver -> GetExID()) );
    }
    else
    {
        setWriteDB.AddToByteArray( CGUID::GUID_INVALID );
    }

    setWriteDB.AddToByteArray( m_cGem.GetGoldCoinsAmount() );
    CGoods* pGem = m_cGem.GetGoods( 0 );
    if( pGem )
    {
        setWriteDB.AddToByteArray( static_cast<CGUID>(pGem -> GetExID()) );
    }
    else
    {
        setWriteDB.AddToByteArray( CGUID::GUID_INVALID );
    }
}

BOOL CPlayerPack::SerializeSubpack(DBWriteSet& setWriteDB,CS2CContainerObjectMove::PLAYER_EXTEND_ID extendId)
{
    if(extendId<CS2CContainerObjectMove::PEI_PACK1 || extendId>CS2CContainerObjectMove::PEI_PACK5)
        return FALSE;
    int nIndex=extendId-CS2CContainerObjectMove::PEI_PACK1;

    CGoods* pPackGoods = m_pSubpackContainer->GetSubpack(nIndex)->pGoods;
    CVolumeLimitGoodsContainer* pPackContainer = m_pSubpackContainer->GetSubpack(nIndex)->pSubpackContainer;
    if(pPackGoods && pPackContainer)
    {
        setWriteDB.AddToByteArray((BYTE)1);
        setWriteDB.AddToByteArray(static_cast<DWORD>(extendId));
        pPackGoods->Serialize(setWriteDB);
        pPackContainer->Serialize(setWriteDB);
    }
    else
    {
        setWriteDB.AddToByteArray((BYTE)0);
        setWriteDB.AddToByteArray(static_cast<DWORD>(extendId));

    }
    return TRUE;

}