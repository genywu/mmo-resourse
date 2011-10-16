/**
*File:  DBRankThread.cpp
*brief: 负责排行榜数据加载及更新线程
*Author:吴畏
*Date:  2010-03-03
*/
#include "stdafx.h"
#include "DBRankThread.h"

#include "DBAdoBase.h"
#include "../Entity/EntityManager.h"

DBRankThread::DBRankThread(string strProvider, 
                           string strDataSource, 
                           string strInitialCatalog, 
                           string strUserID, 
                           string strPSW)
:m_pConnPtr(NULL),m_hThread(NULL),m_nThreadID(0),m_bExit(false),m_bLoadNow(false)
{
    m_pConnPtr = new DBAdoBase(string(	"Provider=" + strProvider +
        "; Server=" + strDataSource + 
        "; Database=" + strInitialCatalog + 
        "; UID=" + strUserID + 
        "; PWD=" + strPSW + 
        "; OLE DB Services=-1; Driver={SQL Server}"
        ));

    m_pConnPtr->OpenCn();
}

DBRankThread::~DBRankThread()
{
    Exit();
    delete m_pConnPtr;
}

bool DBRankThread::Begin(void)
{
    m_bExit = true;
    WaitForSingleObject(m_hThread, INFINITE);
    CloseHandle(m_hThread);

    SetLoadTime(timeGetTime());
    m_bExit = false;
    m_hThread = (HANDLE)_beginthreadex(NULL, 0, RunThread, (void*)this, 0, &m_nThreadID);
    return true;
}

void DBRankThread::Exit(void)
{
    m_bExit = true;
    WaitForSingleObject(m_hThread, INFINITE);
    CloseHandle(m_hThread);
    m_pConnPtr->CloseCn();
}

// 数据加载操作
bool DBRankThread::LoadRanks(tagRankInfo *pRankInfo)
{
    if(NULL == pRankInfo)
        return false;

//    DWORD dwBegin = timeGetTime();   

    map<long, tagRankConfig>::iterator itor = GetGame()->GetEntityManager()->GetRankMap().find(pRankInfo->lRankID);
    if(itor != GetGame()->GetEntityManager()->GetRankMap().end())
    {
        tagRankConfig stRankConfig = itor->second;
        _RecordsetPtr rs;

        try
        {
            GetAdoBasePtr()->CreateRs(rs);
            GetAdoBasePtr()->OpenRs(stRankConfig.strCurSQL.c_str(), rs);

        }
        catch (_com_error &e)
        {
          //  _com_issue_error(E_FAIL);
            // 发生异常,通常表现为所连数据库的机子断网了(若数据库和程序在同一机器上不会，除非数据库宕了)
            // 这个时候连接指针已经失效了，需要关闭重新创建
            // 当然这种修复只有在下一次轮询中才会得到成功所用
            GetAdoBasePtr()->PrintErr("LoadRanks Error!!!",e);
            GetAdoBasePtr()->CloseCn();
            GetAdoBasePtr()->OpenCn();
            return false;
        }
   

        pRankInfo->EnterCS();
        // clear data first
        pRankInfo->Clear();

        // then begin update data 
        pRankInfo->ulUpdateTime = timeGetTime();
        for(long l=0; l < rs->GetFields()->GetCount(); l++)
        {
            tagColInfo tgColInfo;
            tgColInfo.strColName = rs->GetFields()->GetItem(l)->GetName();
            strlwr((char*)tgColInfo.strColName.c_str());
            DataTypeEnum dt = rs->GetFields()->GetItem(l)->GetType();
            switch(dt)
            {
            case adChar:
            case adBSTR:                    
            case adVarChar:
            case adLongVarChar:
            case adWChar:
            case adVarWChar:
            case adLongVarWChar:
                {
                    tgColInfo.dt = CT_STRING;
                }
                break;
            case adGUID:
                {
                    tgColInfo.dt = CT_GUID;
                }
                break;
            case adTinyInt:
            case adSmallInt:
            case adBigInt:
            case adUnsignedTinyInt:
            case adUnsignedSmallInt:
            case adUnsignedInt:
            case adUnsignedBigInt:
            case adSingle:
            case adInteger:
            case adNumeric:
            case adDecimal:
                {
                    tgColInfo.dt = CT_LONG;
                }
                break;

            case adDouble:
                {
                    tgColInfo.dt = CT_FLOAT;
                }
                break;

            case adDate:
            case adDBDate:
            case adDBTime:
            case adDBTimeStamp:
                {
                    tgColInfo.dt = CT_TIME;
                }
                break;
            case adBinary:
            case adVarBinary:
            case adLongVarBinary:
                {
                    tgColInfo.dt = CT_BUFFER;
                }
                break;
            default:
                {
                    tgColInfo.dt = CT_UNKNOWN;
                }
                break;
            }
            pRankInfo->vecColInfo.push_back(tgColInfo);
        }
        long lCurLvl = 1;
        while(!rs->GetadoEOF())
        {
            CTableManager* pDataManager = new CTableManager;                    
            long lAttrNum = 0;
            for(size_t lVecCol = 0; lVecCol < pRankInfo->vecColInfo.size(); lVecCol++)
            {
                tagColInfo stInfo = pRankInfo->vecColInfo[lVecCol];

                variant_t var = rs->GetCollect(stInfo.strColName.c_str());
                CDataObject* pEntity = pDataManager->CreateColumnData(lAttrNum);
                switch(stInfo.dt)
                {
                    //     case DATA_OBJECT_BYTE:
                case CT_STRING:
                    {
                        //    pEntity->SetDataType(DATA_OBJECT_STRING);
                        pEntity->SetStringValue(string((_bstr_t)var).c_str(), (long)string((_bstr_t)var).size());
                    }
                    break;

                case CT_GUID:
                    {
                        //     pEntity->SetDataType(DATA_OBJECT_GUID);
                        CGUID tGUID(string(_bstr_t(var)).c_str());
                        pEntity->SetGUIDValue(tGUID);
                    }
                    break;
                case CT_LONG:
                    {
                        long lVal = var.lVal;
                        pEntity->SetLongValue(lVal);                        
                    }
                    break;

                case CT_FLOAT:
                    {
                        float fValue = var.fltVal;
                        pEntity->SetBufValue((void*)&fValue, sizeof(float));
                    }
                    break;
                case CT_TIME:
                    {
                        //     pEntity->SetDataType(DATA_OBJECT_TIME);
                        long timeBuf[6] = {0};
                        SYSTEMTIME st;
                        VariantTimeToSystemTime(var.date, &st);
                        timeBuf[0] = st.wYear;
                        timeBuf[1] = st.wMonth;
                        timeBuf[2] = st.wDay;
                        timeBuf[3] = st.wHour;
                        timeBuf[4] = st.wMinute;
                        timeBuf[5] = st.wSecond;
                        if(timeBuf[0] == 1900 && timeBuf[1] == 1 && timeBuf[2] == 1)
                            memset(timeBuf, 0, sizeof(timeBuf));
                        pEntity->SetBufValue((void*)&timeBuf[0], sizeof(long)*6);
                    }
                    break;
                case CT_BUFFER:
                    {
                        //  pEntity->SetBufAttr()
                    }
                    break;
                }
                lAttrNum++;
            }
            pRankInfo->vecColData.push_back(pDataManager);                    
            rs->MoveNext();
            lCurLvl++;
        }
        // update data end
        GetAdoBasePtr()->ReleaseRs(rs);
        pRankInfo->LeaveCS();        
    }
    else
    {
        return false;
    }


    SendRanksToWS(pRankInfo->lRankID);
    UpdateRanks(pRankInfo->lRankID);


    return true;
}

// 整包发送或分包发送
void DBRankThread::SendRanksToWS(bool bSendAll)
{
    map<long, tagRankInfo*> mapRankInfo;
    mapRankInfo = GetGame()->GetEntityManager()->GetRankInfo();

    if(bSendAll)
    {
        CMessage msg(MSG_DB2W_SERVER_INIT_PLAYERDATABUFFER);
        msg.Add((char)bSendAll);        // 发送全部
        msg.Add((long)mapRankInfo.size());

        map<long ,tagRankInfo*>::iterator itor = mapRankInfo.begin();
        for(; itor != mapRankInfo.end(); ++itor)
        {   
            vector<BYTE> vecByte;
            tagRankInfo *pRankInfo = (*itor).second;
            if(pRankInfo)
            {
                pRankInfo->EnterCS();
                pRankInfo->AddToByteArray(&vecByte);                
                pRankInfo->LeaveCS();
            }  
            msg.Add((long)vecByte.size());
            if(vecByte.size())
            {
                msg.AddEx(&vecByte[0], (long)vecByte.size());
            }  
        }  
        GetGame()->SendAllWS(&msg);
    }
    else
    {
        map<long, tagRankInfo*>::iterator itor = mapRankInfo.begin();
        for(; itor != mapRankInfo.end(); ++itor)
        {
            tagRankInfo *pRankInfo = (*itor).second;
            if(pRankInfo)
            {
                CMessage msg(MSG_DB2W_SERVER_INIT_PLAYERDATABUFFER);
                msg.Add((char)bSendAll);
                vector<BYTE> vecByte;

                pRankInfo->EnterCS();
                pRankInfo->AddToByteArray(&vecByte);
                pRankInfo->LeaveCS();

                msg.Add((long)vecByte.size());
                if(vecByte.size())
                {
                    msg.AddEx(&vecByte[0], (long)vecByte.size());
                }  
                GetGame()->SendAllWS(&msg);
            }            
        }  
    }
}

// 单个发送
void DBRankThread::SendRanksToWS(long lRankID)
{
    map<long, tagRankInfo*> mapRankInfo;
    mapRankInfo = GetGame()->GetEntityManager()->GetRankInfo();

    map<long, tagRankInfo*>::iterator itor = mapRankInfo.find(lRankID);
    if(itor != mapRankInfo.end())
    {
        tagRankInfo *pRankInfo = itor->second;
        if(pRankInfo)
        {
            CMessage msg(MSG_DB2W_SERVER_INIT_PLAYERDATABUFFER);
            msg.Add((char)false);
            vector<BYTE> vecByte;

            pRankInfo->EnterCS();
            pRankInfo->AddToByteArray(&vecByte);
            pRankInfo->LeaveCS();

            msg.Add((long)vecByte.size());
            if(vecByte.size())
            {
                msg.AddEx(&vecByte[0], (long)vecByte.size());
            }  
            GetGame()->SendAllWS(&msg);
        }            
    }  
}

// 更新排行存储表记录
bool DBRankThread::UpdateRanks(long lRankID)
{
    map<long, tagRankConfig>::iterator itor = GetGame()->GetEntityManager()->GetRankMap().find(lRankID);
    if(itor != GetGame()->GetEntityManager()->GetRankMap().end())
    {
        // 找到排行数据
        tagRankConfig stRankConfig = itor->second;     

        // 清除上一次排行记录
        string strSQL;
    //    strSQL = "DELETE FROM " + stRankConfig.strUpdateTable; 
    //    GetAdoBasePtr()->ExecuteCn(strSQL.c_str());

        strSQL = stRankConfig.strUpdateSQL; 
        GetAdoBasePtr()->ExecuteCn(strSQL.c_str());

        return true;        
    }

    return false;
    
}

// 加载线程
unsigned __stdcall DBRankThread::RunThread(void* pArgument)
{
#ifndef _DEBUG
    __try
    {
#endif
        DBRankThread *pThis = (DBRankThread*)pArgument;
        if(pThis)
        {
            map<long, tagRankInfo*>& mapRankInfo = GetGame()->GetEntityManager()->GetRankInfo();
            static bool bLoadNow = false;
            while(true)
            {    
                if(pThis->m_bExit)
                    break;

                Sleep(1000);

                if(pThis->IsLoadNow())
                {
                    bLoadNow = true;
                }
                
                map<long, tagRankInfo*>::iterator itor = mapRankInfo.begin();
                for(;itor != mapRankInfo.end(); ++itor)
                {
                    tagRankInfo *pRankInfo = itor->second;
                    if(pRankInfo && 
                        ((timeGetTime() - pRankInfo->ulUpdateTime) > GetGame()->GetSetup()->dwRefreshRanksTime * 60 * 1000 || bLoadNow))
                    {
                        // 定时更新排行数据
                        pThis->LoadRanks(pRankInfo);
                    }
                }

                if(bLoadNow)
                {
                    bLoadNow = false;
                    pThis->SetReLoad(false);
                }
            }
        }
   
#ifndef _DEBUG
    }
    __except(_Sword3::CrashFilter(GetExceptionInformation(), GetExceptionCode()))
    {
       Log4c::Warn(ROOT_MODULE,GetText("DB_DBTHREAD_2"));//程序出现错误，请查看最新的错误报告文件
        _endthreadex( 0 );
        return 0;
    }
#endif

    _endthreadex(0);
    return 0;
}