/**
*File:  RankDef.cpp
*Brief: 排行榜数据定义
*Author:吴畏
*Date:  2010.03.10
*/

#include "stdafx.h"
#include "RankDef.h"

#define RANK_PAGE_SIZE  10
// ranks
bool tagRankInfo::AddToByteArray(vector<BYTE>* pByte)
{
    return AddToByteArray(pByte, (long)0, (long)vecColData.size()-1);
}

// 编码某个段的排行数据
bool tagRankInfo::AddToByteArray(vector<BYTE>* pByte, long lPosBegin, long lPosEnd)
{
    // 越界检测
    if(lPosEnd < lPosBegin || lPosEnd > (long)(vecColData.size() -1) )
    {
        return false;
    }

    _AddToByteArray(pByte, lRankID);
    _AddToByteArray(pByte, ulUpdateTime);
    _AddToByteArray(pByte, (long)vecColInfo.size());
    for(vector<tagColInfo>::iterator itor = vecColInfo.begin(); itor != vecColInfo.end(); ++itor)
    {
        tagColInfo tInfo = (*itor);
        _AddToByteArray(pByte, (long)tInfo.dt);
        _AddToByteArray(pByte, tInfo.strColName.c_str());
    }

    _AddToByteArray(pByte, GetPageCount());
    _AddToByteArray(pByte, FindPage(lPosBegin));

    _AddToByteArray(pByte, (long)(lPosEnd-lPosBegin+1));
    for(long lPos = lPosBegin; lPos <= lPosEnd; lPos++)
    {
        CTableManager *pData = vecColData[lPos];
        if(pData)
        {
            for(long lCol=0; lCol<(long)vecColInfo.size(); lCol++)
            {
                tagColInfo tInfo = vecColInfo[lCol];
                switch(tInfo.dt)
                {
                case CT_BYTE:
                case CT_STRING:
                    {
                        string str = pData->GetStringAttr(lCol);
                        _AddToByteArray(pByte, str.c_str());
                    }
                    break;

                case CT_GUID:
                    {
                        CGUID tGUID;
                        pData->GetGuidAttr(lCol, tGUID);
                        _AddToByteArray(pByte, tGUID);
                    }
                    break;
                case CT_LONG:
                    {
                        long lValue = pData->GetLongAttr(lCol);
                        _AddToByteArray(pByte, lValue);
                    }
                    break;
                case CT_FLOAT:
                    {
                        float fValue = 0.0;
                        pData->GetBufAttr(lCol, (void*)&fValue, sizeof(float));
                        _AddToByteArray(pByte, fValue);
                    }
                    break;
                case CT_TIME:
                    {
                        long lTime[6];
                        pData->GetBufAttr(lCol, (void*)&lTime, sizeof(long)*6);
                        _AddToByteArray(pByte, (void*)&lTime, sizeof(long)*6);
                    }
                    break;
                case CT_BUFFER:
                    {
                    }
                    break;

                default:
                    {
                    }
                    break;
                }
            }
        }
    }
    return true;
}

// 按照条件过滤编码排行数据
bool tagRankInfo::AddToByteArray(vector<BYTE>* pByte, CDataObject* pEntity, long lColPos)
{
    if(pEntity == NULL)
        return false;

    long lDataPos = FindDataPos(pEntity, lColPos);
    if(lDataPos == -1)
        return false;

    long lPage = FindPage(lDataPos);
    return AddToByteArray(pByte, lPage);
}

bool tagRankInfo::AddToByteArray(vector<BYTE>* pByte, CDataObject* pEntity, const char* szColName)
{
    if(pEntity == NULL)
        return false;

    long lColPos = FindColPos(szColName);
    if(lColPos == -1)
        return false;

    return AddToByteArray(pByte, pEntity, lColPos);
}

// 查询编码某页的数据
bool tagRankInfo::AddToByteArray(vector<BYTE>* pByte, long lPage)
{
    long lTotalPage = GetPageCount();
    // 检测查询页的有效性
    if(lPage > lTotalPage || lPage < 1)
    {
        return false;
    }

    long lPosBegin, lPosEnd;
    lPosBegin = (lPage - 1)*RANK_PAGE_SIZE;
    lPosEnd = lPage*RANK_PAGE_SIZE -1;
    if(lPosEnd > (long)vecColData.size()-1)
    {
        lPosEnd = static_cast<long>(vecColData.size() -1);
    }
    return AddToByteArray(pByte, lPosBegin, lPosEnd);
}

// 总页面数
long tagRankInfo::GetPageCount()
{
    return (long)vecColData.size()/RANK_PAGE_SIZE + (((long)vecColData.size()%RANK_PAGE_SIZE) ? 1:0);
}

// 找到数据所在页面
long tagRankInfo::FindPage(long lDataPos)
{
    return (lDataPos+1)/RANK_PAGE_SIZE + ( ((lDataPos+1)%RANK_PAGE_SIZE) ? 1:0 );
}

// 寻找表列在Vector中的位置
long tagRankInfo::FindColPos(const char* szColName)
{
    long lRet = -1;
    for(long lPos=0; lPos< (long)vecColInfo.size(); lPos++)
    {
        tagColInfo tInfo = vecColInfo[lPos];
        if(0 == strcmp(tInfo.strColName.c_str(), szColName))
        {
            lRet = lPos;
            break;
        }
    }

    return lRet;
}

// 找到复合过滤条件的数据位置
long tagRankInfo::FindDataPos(CDataObject* pEntity, long lColPos)
{
    long lRet = -1;

    if(pEntity == NULL)
    {
        return lRet;
    }

    for(long lPos=0; lPos<(long)vecColData.size(); lPos++)
    {
        CTableManager* pData = vecColData[lPos];
        if(pData == NULL)
            continue;

        tagColInfo tInfo = vecColInfo[lColPos];
        switch(tInfo.dt)
        {
        case CT_BYTE:
        case CT_STRING:
            {
                if(strcmp(pData->GetStringAttr(lColPos), pEntity->GetStringValue()) == 0)
                {
                    lRet = lPos;
                }
            }
            break;

        case CT_GUID:
            {
                CGUID guid1, guid2;
                pData->GetGuidAttr(lColPos, guid1);
                guid2 = pEntity->GetGUIDValue();
                if(guid1 == guid2)
                    lRet = lPos;
            }
            break;
        case CT_LONG:
            {
                if(pData->GetLongAttr(lColPos) == pEntity->GetLongValue())
                {
                    lRet = lPos;
                }
            }
            break;
        case CT_FLOAT:
            {
                float fValue1, fValue2 = 0.0;
                pData->GetBufAttr(lColPos, (void*)&fValue1, sizeof(float));
                pEntity->GetBufValue((void*)&fValue2, sizeof(float));
                if(fValue2 == fValue1)
                {
                    lRet = lPos;
                }
            }
            break;
        case CT_TIME:
            {
            }
            break;
        case CT_BUFFER:
            {
            }
            break;

        default:
            {
            }
            break;
        }
        if(lRet != -1)
        {
            break;
        }
    }
    return lRet;
}

void tagRankInfo::DecordFromByteArray(BYTE* pByte, long& lPos)
{
    Clear();

    char szTemp[128];
    lRankID = _GetLongFromByteArray(pByte, lPos);
    ulUpdateTime = _GetDwordFromByteArray(pByte, lPos);
    long lColSize = _GetLongFromByteArray(pByte, lPos);
    for(long lCol=0; lCol<lColSize; lCol++)
    {
        tagColInfo tInfo;
        tInfo.dt = (COL_TYPE)_GetLongFromByteArray(pByte, lPos);
        tInfo.strColName = _GetStringFromByteArray(pByte, lPos, szTemp);
        vecColInfo.push_back(tInfo);
    }

    lTotalPage = _GetLongFromByteArray(pByte, lPos);
    lCurPage = _GetLongFromByteArray(pByte, lPos);

    long lDataSize = _GetLongFromByteArray(pByte, lPos);
    for(long lData=0; lData<lDataSize; lData++)
    {     
        CTableManager* pData = new CTableManager;
        for(long lCol=0; lCol<(long)vecColInfo.size(); lCol++)
        {  
            CDataObject* pEntity = pData->CreateColumnData(lCol);
            tagColInfo tInfo = vecColInfo[lCol];
            switch(tInfo.dt)
            {
            case CT_BYTE:
            case CT_STRING:
                {                    
                    string str = _GetStringFromByteArray(pByte, lPos, szTemp);
                    pEntity->SetStringValue(str.c_str(), (long)str.size());
                }
                break;

            case CT_GUID:
                {
                    CGUID tGUID;
                    _GetBufferFromByteArray(pByte, lPos, tGUID);
                    pEntity->SetGUIDValue(tGUID);
                }
                break;
            case CT_LONG:
                {
                    long lValue = _GetLongFromByteArray(pByte, lPos);
                    pEntity->SetLongValue(lValue);
                }
                break;
            case CT_FLOAT:
                {
                    float fValue = _GetFloatFromByteArray(pByte, lPos);
                    pEntity->SetBufValue((void*)&fValue, sizeof(float));
                }
                break;
            case CT_TIME:
                {
                    long lTime[6];
                    _GetBufferFromByteArray(pByte, lPos, (void*)&lTime, sizeof(long)*6);
                    pEntity->SetBufValue((void*)&lTime, sizeof(long)*6);
                }
                break;
            case CT_BUFFER:
                {
                }
                break;

            default:
                {
                }
                break;
            }                
        }
        vecColData.push_back(pData);       
    }
}
