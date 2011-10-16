/*
*	file:		AutoFightData.h
*	Brief:		自动打怪对象
*	detail:		提供对象的编解码接口
*	Authtor:	袁洪俊
*	Date:		2011-02-14
*/

#include "SpriteDef.h"

#pragma once

class CAutoFightData
{
public:
    CAutoFightData();
    ~CAutoFightData();

public:
    //编码
    void     AddToByteArray    (vector<BYTE> &ByteArray);
    //解码    
    void     GetFromByteArray  (BYTE* pByteArray, long &lPos);

    //编码
    void	 CodeToDataBlock		(DBWriteSet& setWriteDB);
    //解码
    void	 DecodeFromDataBlock	(DBReadSet& setReadDB);

    //还原
    void     Reset(void);

    //得到一个类型配置
    const tagAutoFightNode*   GetOneSetup(LONG lType);
    //设置稳定挂机时间
    void     SetStableTime(DWORD dwTime) 
    {
        if (dwTime <= MAX_STABLETIME)
        {
            m_dwStableTime = dwTime;
        }
        else
        {
            m_dwStableTime = MAX_STABLETIME;
        }
    }
    //得到稳定挂机时间
    DWORD     GetStableTime(void) {return m_dwStableTime;}

    //修改一个类型的设置
    LONG    SetOneSetup       (LONG lType, tagAutoFightNode &AutoFightNode);
private:
    //自动打怪辅助数据
    tagAutoFightNode       m_AutoFightSetup[AutoHookTypeNum];
    //稳定挂机时间
    DWORD     m_dwStableTime;
};