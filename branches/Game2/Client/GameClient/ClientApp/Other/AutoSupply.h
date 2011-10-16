#pragma once
#include "../../../Public/Common/SpriteDef.h"
class AutoSupply
{
public:
    AutoSupply(){}
    ~AutoSupply(){}
    void DecodeFromDataBlockAndInit(DBReadSet& setReadDB);
private:
    tagAotoSupplyNode m_AotoSupplySetup[AotoSupplyTypeNum];
};