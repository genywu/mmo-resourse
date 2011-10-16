#include "stdafx.h"
#include "AutoSupply.h"
#include "../../../Public/Common/AutoSupplyData.h"
void AutoSupply::DecodeFromDataBlockAndInit(DBReadSet& setReadDB)
{
    LONG lGoodsNum = setReadDB.GetLongFromByteArray();
    //解码前要初始化基础数据
    for(int i=0;i<AotoSupplyTypeNum;i++)
    {
        m_AotoSupplySetup[i].lGoodsIndex = 0;
        m_AotoSupplySetup[i].lSpringScale = 0;
        m_AotoSupplySetup[i].bActive = 0;
    }
    for (int i = 0; i < lGoodsNum;i++) 
    {
        long lItemlist = setReadDB.GetLongFromByteArray();
        DWORD dwItemUse = setReadDB.GetLongFromByteArray(); 
        
    }
    AutoSupplyData *AutoData = new AutoSupplyData;
    AutoData->DecodeFromDataBlock(setReadDB);
    for(int i=0;i<AotoSupplyTypeNum;i++)
    {
        m_AotoSupplySetup[i].lGoodsIndex = AutoData->GetOneSetup(i)->lGoodsIndex;
        m_AotoSupplySetup[i].lSpringScale = AutoData->GetOneSetup(i)->lSpringScale;
        m_AotoSupplySetup[i].bActive = AutoData->GetOneSetup(i)->bActive;
    }
    SAFE_DELETE(AutoData);
}