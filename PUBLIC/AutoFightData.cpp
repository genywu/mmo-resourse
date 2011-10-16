#include "StdAfx.h"
#include "public.h"
#include "AutoFightData.h"


CAutoFightData::CAutoFightData(void)
:m_dwStableTime(0)
{
    Reset();
}

CAutoFightData::~CAutoFightData()
{

}

#define DATA_SIZE   sizeof(tagAutoFightNode) * AutoHookTypeNum

//编码
void CAutoFightData::AddToByteArray(vector<BYTE> &ByteArray)
{
    _AddToByteArray(&ByteArray, m_AutoFightSetup, DATA_SIZE);
}

//解码
void CAutoFightData::GetFromByteArray(BYTE* pByteArray, long &lPos)
{
    _GetBufferFromByteArray(pByteArray,lPos,m_AutoFightSetup,DATA_SIZE);
}

//编码
void CAutoFightData::CodeToDataBlock(DBWriteSet& setWriteDB)
{
    //由于不需要保存稳定挂机和普通挂机的状态，不需要发给GS，需要是启用
    //setWriteDB.AddToByteArray(m_AutoFightSetup,DATA_SIZE);
    //给GS传稳定时间
    setWriteDB.AddToByteArray((DWORD)GetStableTime());
}

//解码
void CAutoFightData::DecodeFromDataBlock(DBReadSet& setReadDB)
{
    //由于不需要保存稳定挂机和普通挂机的状态，不需要从GS得到，需要是启用
    //setReadDB.GetBufferFromByteArray(m_AutoFightSetup, DATA_SIZE);
    //从GS解码稳定时间
    SetStableTime(setReadDB.GetDwordFromByteArray());
}

//重置对象
void CAutoFightData::Reset(void)
{
    memset(m_AutoFightSetup,0,DATA_SIZE);
}

//获得一个设置
const tagAutoFightNode* CAutoFightData::GetOneSetup(LONG lType)
{
    if (AutoHookTypeNum > lType)
    {
        return &(m_AutoFightSetup[lType]);
    }
    return NULL;
}

//修改一个设置
LONG CAutoFightData::SetOneSetup(LONG lType, tagAutoFightNode &AutoFightNode)
{
    if (AutoHookTypeNum <= lType)
        return eASET_TypeWithout;

    //! 相同的对象值不重新设置
    if(0 == memcmp(&(m_AutoFightSetup[lType]), &AutoFightNode, sizeof(tagAutoFightNode)))
        return eASET_NotChange;

    memmove(&(m_AutoFightSetup[lType]), &AutoFightNode, sizeof(tagAutoFightNode));
    return eASET_NotErr;
}