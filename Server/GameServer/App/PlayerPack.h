/**
*	file:		PlayerPack.h
*	Brief:		背包类及货币类
*	detail:		关于背包、子背包及货币等的一些编码操作
*	Authtor:	吴畏
*	Date:		2010-07-05
*/

#pragma once
#include "Container/CVolumeLimitGoodsContainer.h"
#include "Container/CSubpackContainer.h"
#include "Container/CWallet.h"

class CPlayerPack
{
public:
    CPlayerPack(CPlayer* pPlayer);
    ~CPlayerPack(void);
    void Release();

    void SetOwner(CPlayer* pPlayer);

    void CodeToDataBlockForServer(DBWriteSet& setWriteDB);
    void CodeToDataBlockForClient(DBWriteSet& setWriteDB);

    void DecordeDataBlockFromServer(DBReadSet& setReadDB);

    BOOL SerializeSubpack(DBWriteSet& setWriteDB,CS2CContainerObjectMove::PLAYER_EXTEND_ID extendId);


    CSubpackContainer::tagSubpack* GetSubpack(int i) { return m_pSubpackContainer->GetSubpack(i); }
    CVolumeLimitGoodsContainer* GetOriginPack() {return m_pOrigGoodsContainer;}
    CSubpackContainer* GetSubpackContainer()    {return m_pSubpackContainer;}
    CWallet& GetWallet()        { return m_cWallet; }
    CWallet& GetSilverWallet()  { return m_cSilverWallet; }
    CWallet& GetGem()           { return m_cGem; }

private:
    CPlayer*                    m_pPlayer;                          // 所属玩家
    CVolumeLimitGoodsContainer* m_pOrigGoodsContainer;              // 背包
    CSubpackContainer*          m_pSubpackContainer;                // 子背包容器

   
    CWallet				m_cWallet;          // 钱包   (古元) 
    CWallet				m_cSilverWallet;    // 银币钱包 (绑定古元、玩家之间不可交易)   
    CWallet				m_cGem;             // 神石
    CWallet             m_cSenbei;          // 仙贝

};