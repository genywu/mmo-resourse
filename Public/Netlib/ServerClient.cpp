//=============================================================================
/**
*  file: serverClient.h
*
*  Brief:主要是服务器端使用的网络封装，用IOCP方式实现的网络接口
*		当有一个客户端的连接请求到达后，服务器生成一个ServerClient与之关联
*		本类负责和具体客户端通讯
*
*  Authtor:wangqiao
*	
*	Datae:2005-4-15
*
*	Modify:2007-4-13,整理了代码和结构，优化了效率
*/
//=============================================================================

#include "stdafx.h"
#include "BaseMessage.h"
#include <mmsystem.h>
#include "serverClient.h"
#include "Servers.h"
#include "../Module/Crc32Static.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CServerClient::CServerClient(CServer*	pServers,CDataBlockAllocator* pDBAllocator)
:m_pServers(pServers)
,m_pDBAllocator(pDBAllocator)
{
    m_nUseCount=1;
    m_bServerType = 0;
    Init();
}


void CServerClient::Init()
{
    assert(m_pServers);
    m_bLost = false;
    m_bShutDown = false;
    m_bQuit = false;
    m_bUsing= false;
    m_bTransfersStatus = false;
    m_bAccept = true;
    m_bSendZeroByteData = false;
    m_nReadDataSize = 0;
    m_ReadSequenceNumber = 1;
    m_CurrentReadSequenceNumber = 1;
    m_SendMessages.clear();
    m_ReadDataBlocks.clear();


    //统计接受
    m_dwRcvStartTime = timeGetTime();		//接受统计开始时间
    m_lCurTotalRcvSize = 0;					//接受包的总大小
    m_lCurRcvSizePerSecond = 0;				//当前每秒接受的数据大小
    m_lMaxRcvSizePerSecond = 4096;			//最大每秒接受大小

    //统计发送
    m_dwSendStartTime = timeGetTime();		//发送统计开始时间
    m_lCurTotalSendSize = 0;				//当前总发送大小
    m_lCurSendSizePerSecond = 0;			//当前每秒发送大小
    m_lMaxSendSizePerSecond = 8192;			//每秒最大发送大小

    m_bChecked = false;
    m_lMaxMsgLen  = 0x7FFFFFFF;				//最大消息长度
    m_lPendingWrBufNum = 0;					//当前挂起发送缓冲区的总大小
    m_lMaxPendingWrBufNum = 8192;			//最大挂起发送缓冲区的总大小
    m_lMaxPendingRdBufNum = 8192;			//挂起的接受缓冲区的总大小
    m_lMaxBlockSendMsnNum = 1024;			//最大阻塞未发送消息个数

    m_lSendCounter = 0;
    m_lRecvCounter = 0;


    m_lLastMsgTotalSize = 0;
    //上次消息的长度
    m_lLastMsgLen = 0;
    //上次消息的类型
    m_lLastMsgType = 0;
    //上次消息数据块的处理前个数
    m_lLastMsgPreDBNum = 0;
    //上次消息数据块的处理后个数
    m_lLastMsgPosDBNum = 0;
    m_lLastMsgRemainSize = 0;
    //上次消息移动内存的位置
    m_lLastMsgMemMoveDBPos = 0;
    //上次移动的最大
    m_lLastMsgMemMoveDBSize = 0;

}

void CServerClient::Release()
{
    itMsg it = m_SendMessages.begin();
    for(;it != m_SendMessages.end();it++)
    {
        CBaseMessage* pMsg = (*it);
        if(pMsg->RemoveRefCount() == 0)
            CBaseMessage::FreeMsg(pMsg);
    }
    m_SendMessages.clear();

    itDB itDB = m_ReadDataBlocks.begin();
    for(;itDB != m_ReadDataBlocks.end() ;itDB++)
    {
        m_pDBAllocator->FreeDB((*itDB));
    }
    m_ReadDataBlocks.clear();

    itDBMap itDBBuff = m_ReadBuffer.begin();
    for(;itDBBuff != m_ReadBuffer.end();itDBBuff++)
    {
        m_pDBAllocator->FreeDB((*itDBBuff).second);
    }
    m_ReadBuffer.clear();
}

CServerClient::~CServerClient()
{
    Release();
}

//设置相应参数
void CServerClient::SetParam(bool bChecked,long lMaxMsgLen,long lMaxBlockSendMsgNum,
                             long lMaxPendingWrBufNum,long lMaxPendingRdBufNum,
                             long lMaxSendSizePerSecond,long lMaxRcvSizePerSecond,
                             long lFlag)
{
    m_bChecked = bChecked;
    m_lMaxMsgLen  = lMaxMsgLen;	
    m_lMaxBlockSendMsnNum = lMaxBlockSendMsgNum;

    m_lMaxPendingWrBufNum = lMaxPendingWrBufNum;
    m_lMaxPendingRdBufNum = lMaxPendingRdBufNum;

    m_lMaxSendSizePerSecond = lMaxSendSizePerSecond;
    m_lMaxRcvSizePerSecond = lMaxRcvSizePerSecond;

    lFlag = lFlag;
}

bool CServerClient::StartReadData()
{
    int nRecBuf = 0;
    while(nRecBuf<m_lMaxPendingRdBufNum)
    {
        PER_IO_OPERATION_DATA* pPerIOData = m_pServers->AllocIoOper();
        if( NULL==pPerIOData)	return false;
        CDataBlock *pDB = m_pDBAllocator->AllocDB(7);

        pPerIOData->hSocket = m_hSocket;
        //从完成端口上获得数据
        if(ReadFromCompletionPort(pPerIOData,pDB)==false)
        {
            m_pServers->FreeIoOper(pPerIOData);
            return false;
        }
        nRecBuf += pDB->GetMaxSize();
    }
    return true;
}
//**********************************
//获得完成端口上收到的数据
//**********************************
bool CServerClient::ReadFromCompletionPort(PER_IO_OPERATION_DATA* pIOData,CDataBlock *pDB)
{	
    if(NULL == pIOData ) return false;
    if(NULL == pDB)
        pDB = m_pDBAllocator->AllocDB(8);
    if(NULL == pDB)
    {
        PutErrorString(NET_MODULE,"%-15s %s",__FUNCTION__,"在函数CServerClient::ReadFromCompletionPort(...)中,分配内存失败！");
        return false;
    }

    memset( &pIOData->OverLapped,0,sizeof(OVERLAPPED) );
    if(pIOData->m_lDataBufNum < 1)
        pIOData->ResetDataBufSize(1);
    pIOData->pParam = pDB;
    pIOData->pDataBuf[0].len = pDB->GetMaxSize();
    pIOData->pDataBuf[0].buf = (char*)pDB->Base();
    pIOData->OperationType = SOT_Receive;
    pIOData->m_nSequenceNumber = m_ReadSequenceNumber;
    ulong dwFlag = 0;
    ulong dwReceivByte;
    int nRet = WSARecv(m_hSocket,pIOData->pDataBuf,1,&dwReceivByte,&dwFlag,&(pIOData->OverLapped),NULL);
    if(nRet == SOCKET_ERROR)
    {
        int nError = WSAGetLastError();
        if (nError != WSA_IO_PENDING)
        {
            m_pDBAllocator->FreeDB(pDB);
            PutErrorString(NET_MODULE,"%-15s 在函数CServerClient::ReadFromCompletionPort(...)中,WSARecv()操作失败！(ErrorID:%d)",__FUNCTION__,nError);
            return false;
        }
    }
    IncrReadSequenceNumber();
    return true;
}



// [TCP]发送数据用完成端口
int CServerClient::Send(const void* lpBuf, int nBufLen, int nFlags)
{
    if( IsShutDown() )	return true;

    if(m_SendMessages.size() == 0 ||//是否有数据
        GetPendingWrBufNum() > m_lMaxPendingWrBufNum)//待发送的数据大小小于限定值
    {
        return true;
    }
    //如果平均发送数据大小超过限定值,则发送0大小的数据
    if(GetCurSendSizePerSecond() > m_lMaxSendSizePerSecond)
    {
        SendZeroByteData();
        return true;
    }

    itMsg it = m_SendMessages.begin();
    for(;it != m_SendMessages.end();)
    {
        //发送一个消息
        if(!Send((*it),nFlags))
            return false;
        it = m_SendMessages.erase(it);
        if(GetPendingWrBufNum() > m_lMaxPendingWrBufNum ||
            GetCurSendSizePerSecond() > m_lMaxSendSizePerSecond )
            break;
    }
    return true;
}


//当发送数据流量大于限制流量时,发送0字节数据
//用来在下一个周期中来检测、发送数据
void CServerClient::SendZeroByteData()
{
    if(m_bSendZeroByteData)	return;

    PER_IO_OPERATION_DATA* pPerIOData = m_pServers->AllocIoOper();
    //当分配不出内存时
    if(NULL == pPerIOData) return;
    memset( &pPerIOData->OverLapped,0,sizeof(OVERLAPPED) );
    pPerIOData->OperationType = SOT_SendZeroByte;
    bool bSuccess = PostQueuedCompletionStatus(m_pServers->m_hCompletionPort,0,(ulong)GetIndexID(),(OVERLAPPED*)pPerIOData) ? true : false;
    if( !bSuccess && WSAGetLastError() != ERROR_IO_PENDING )
    {
        PutErrorString(NET_MODULE,"%-15sCServerClient::SendZeroByteData().PostQueuedCompletionStatus .erros(errid:%d).",__FUNCTION__,WSAGetLastError());
        return;
    }
    m_bSendZeroByteData = true;
    return;
}

void CServerClient::OnSendZeroByteData()
{
    m_bSendZeroByteData = false;
}


int CServerClient::Send(CBaseMessage *pMsg,int nFlags)
{
    if(NULL == pMsg)	return false;

    //添加发送消息统计
    //m_pServers->AddSendMsgStat(pMsg->GetType(),pMsg->GetSize());

    PER_IO_OPERATION_DATA* pPerIOData = m_pServers->AllocIoOper();
    //当分配不出内存时
    if(NULL == pPerIOData) return false;
    long lTotalSendSize = 0, lTotalMsgSize = pMsg->GetTotalSize();
    if (m_pServers->IsEncryptType(GetFlag()))
    {
        pMsg->Encrypt(m_kn);
    }

    memset( &pPerIOData->OverLapped,0,sizeof(OVERLAPPED) );		
    pPerIOData->OperationType = SOT_Send;
    pPerIOData->pParam = pMsg;
    pPerIOData->hSocket = m_hSocket;

    vector<CDataBlock*>& DataBlocks = pMsg->GetMsgData();
    long lBlockSize = (long)DataBlocks.size();
    if(pPerIOData->m_lDataBufNum < lBlockSize)
        pPerIOData->ResetDataBufSize(lBlockSize);
    int i = 0;
    for(;i<lBlockSize;i++)
    {
        CDataBlock* pDB = DataBlocks[i];
        pPerIOData->pDataBuf[i].buf=(char*)pDB->Base();
        pPerIOData->pDataBuf[i].len = pDB->GetCurSize();
        lTotalSendSize += pDB->GetCurSize();
    }

    //错误
    if (lTotalSendSize != lTotalMsgSize)
    {
        free(pPerIOData);		
        PutErrorString(NET_MODULE,"%-15s Msg Length Error(NetFlag:%d,IndexID:%d,BlockSize:%d,MsgType:%d,MsgSize:%d,SendSize:%d)",
            __FUNCTION__,GetIndexID(),lBlockSize,pMsg->GetType(),pMsg->GetTotalSize(),lTotalSendSize);
        if(pMsg->RemoveRefCount() == 0)
            CBaseMessage::FreeMsg(pMsg);
        return true;
    }

    ulong dwSentNum = 0;
    int ret = WSASend(m_hSocket, pPerIOData->pDataBuf, lBlockSize, &dwSentNum, nFlags,
        (OVERLAPPED*)pPerIOData, NULL);

    if (ret == SOCKET_ERROR)
    {
        int nError = WSAGetLastError();
        if ( nError != WSA_IO_PENDING)
        {
            free(pPerIOData);
            PutErrorString(NET_MODULE,"%-15s 向客户端发送消息错误(errorID:%d)",__FUNCTION__,nError);
            return false;
        }
    }

    AddSendSize(lTotalSendSize);
    IncPendingWrBufNum(lTotalSendSize);
    return true;
}

bool CServerClient::AddSendMsg(CBaseMessage* pMsg)
{
    char strInfo[256]="";
    if(NULL == pMsg)	return false;
    if( IsShutDown() )
    {
        if(pMsg->RemoveRefCount() == 0)
            CBaseMessage::FreeMsg(pMsg);
        return false;
    }
    //如果阻塞的消息数量大于规定数量，则强制性断开该连接
    if(m_SendMessages.size() > m_lMaxBlockSendMsnNum)
    {
        ShutDown();
        if(pMsg->RemoveRefCount() == 0)
        {
            CBaseMessage::FreeMsg(pMsg);
        }
        PutErrorString(NET_MODULE,"%-15s the blocked send messge count(num:%d) greater the max count(num:%d)",
            __FUNCTION__,m_SendMessages.size(),m_lMaxBlockSendMsnNum);

        return false;
    }

    //OnTransferChange();
    //ulong dwCurTime = timeGetTime();
    //long lCurMsgPri = pMsg->GetPriorityValue(dwCurTime);
    ////该消息类型是否可以丢弃
    //bool bDiscard = CBaseMessage::GetIsDiscard(pMsg->GetType());
    ////进行优先级判断,确定插入的位置
    //itMsg it  = m_SendMessages.begin();
    //for(;it != m_SendMessages.end();)
    //{
    //	CBaseMessage* pTemptMsg = (*it);
    //	//如果消息类型一样，查看是否可以丢弃已存在的消息
    //	if(pMsg->GetType() == pTemptMsg->GetType())
    //	{
    //		//如果丢弃标记一样，则丢弃已经存在的消息
    //		if(bDiscard && pMsg->IsDiscardFlagEqual(pTemptMsg) )
    //		{
    //			//_snprintf(strInfo,256,"Discard a messge(type:%d)",pTemptMsg->GetType());
    //			//PutErrorString(NET_MODULE,"%-15s %s",__FUNCTION__,strInfo);
    //			it = m_SendMessages.erase(it);
    //			CBaseMessage::FreeMsg(pTemptMsg);
    //			continue;
    //		}
    //	}
    //	//类型不一样，判断优先级
    //	else
    //	{
    //		if( lCurMsgPri > pTemptMsg->GetPriorityValue(dwCurTime))
    //		{
    //			_snprintf(strInfo,256,"the messge(type:%d) prio greater the msg(type:%d)",
    //						pMsg->GetType(),pTemptMsg->GetType());
    //			PutErrorString(NET_MODULE,"%-15s %s",__FUNCTION__,strInfo);
    //			break;
    //		}
    //	}
    //	it++;
    //}
    //
    //m_SendMessages.insert(it,pMsg);
    m_SendMessages.push_back(pMsg);
    //如果已发送的数据小于设置大小，则发送数据
    if( !Send(NULL,0,0) )
        return false;
    return true;
}

bool CServerClient::AddReceiveData(long lSequeNumber,CDataBlock *pDB)
{
    if(NULL == pDB)	return false;

    itDBMap it = m_ReadBuffer.find(m_CurrentReadSequenceNumber);
    if(it != m_ReadBuffer.end())
    {
        Log4c::Warn(NET_MODULE,"The key has already exist in the read-buffs");
    }

    if(lSequeNumber == m_CurrentReadSequenceNumber)
    {
        PushReadDataBack(pDB);
        IncrCurReadSequenceNumber();
    }
    else
    {
        m_ReadBuffer[lSequeNumber] = pDB;
    }

    while(pDB = GetNextReadDB())
    {
        PushReadDataBack(pDB);
        IncrCurReadSequenceNumber();
    }
    return true;
}

CDataBlock* CServerClient::GetNextReadDB()
{
    CDataBlock *pDB = NULL;
    itDBMap it = m_ReadBuffer.find(m_CurrentReadSequenceNumber);
    if(it != m_ReadBuffer.end())
    {
        pDB = (*it).second;
        m_ReadBuffer.erase(it);
    }
    return pDB;
}

//从数据头开始获得一个long值
long CServerClient::GetCurMsgLen()
{
    char strInfo[1024]="";
    //赋一个最大值
    long len = 0x7FFFFFFF;
    if(m_nReadDataSize < 4)	return len;

    BYTE* pLen = (BYTE*)&len;
    long lPos = 0;
    long size = sizeof(long);	
    itDB it = m_ReadDataBlocks.begin();
    for(;it != m_ReadDataBlocks.end() && size > 0;it++)
    {
        CDataBlock *pDB = (*it);
        if(pDB)
        {
            long minSize = min(size,pDB->GetCurSize());
            memcpy(pLen+lPos,pDB->Base(),minSize);
            if(len<=0)
            {
                PutErrorString(NET_MODULE,"%-15s Get MsgLen Error1!(flag:%d,Index:%d)len:%d,ReadDataBlocksSize:%d,minSize:%d.(m_lLastMsgTotalSize:%d,\
                             m_lLastMsgLen:%d,m_lLastMsgType:%d,m_lLastMsgPreDBNum:%d,m_lLastMsgPosDBNum:%d,m_lLastMsgMemMoveDBPos:%d,\
                             m_lLastMsgMemMoveDBSize:%d,m_lLastMsgRemainSize:%d)",
                             __FUNCTION__,len,m_ReadDataBlocks.size(),minSize,m_lLastMsgTotalSize,m_lLastMsgLen,m_lLastMsgType,m_lLastMsgPreDBNum,
                             m_lLastMsgPosDBNum,m_lLastMsgMemMoveDBPos,m_lLastMsgMemMoveDBSize,m_lLastMsgRemainSize);
            }
            lPos+=minSize;
            size -= minSize;
            //退出
            if(size <= 0 )
                break;
        }
    }

    if(m_nReadDataSize >= 4)
    {
        if(m_bChecked && len > m_lMaxMsgLen)
        {
            PutErrorString(NET_MODULE,"Get MsgLen Error2!(flag:%d,Index:%d)len:%d,MaxMsgLen:%d(m_lLastMsgTotalSize:%d,m_lLastMsgLen:%d,\
                         m_lLastMsgType:%d,m_lLastMsgPreDBNum:%d,m_lLastMsgPosDBNum:%d,m_lLastMsgMemMoveDBPos:%d,m_lLastMsgMemMoveDBSize:%d,\
                         m_lLastMsgRemainSize:%d)",
                         __FUNCTION__,GetFlag(),GetIndexID(),len,m_lMaxMsgLen,m_lLastMsgTotalSize,m_lLastMsgLen,m_lLastMsgType, 
                         m_lLastMsgPreDBNum,m_lLastMsgPosDBNum,m_lLastMsgMemMoveDBPos,m_lLastMsgMemMoveDBSize,m_lLastMsgRemainSize);
__FUNCTION__,
            //关闭网络连接
            Close();
            return 0x7FFFFFFF;
        }
        if( len < long(4+sizeof(CBaseMessage::stMsg)) )
        {
            PutErrorString(NET_MODULE,"%-15s Get MsgLen Error3!(flag:%d,Index:%d)m_nReadDataSize:%d,ReadDataBlocksSize:%d,len:%d(m_lLastMsgTotalSize:%d,\
                         m_lLastMsgLen:%d,m_lLastMsgType:%d,m_lLastMsgPreDBNum:%d,m_lLastMsgPosDBNum:%d,m_lLastMsgMemMoveDBPos:%d,\
                         m_lLastMsgMemMoveDBSize:%d,m_lLastMsgRemainSize:%d)",
                         __FUNCTION__,GetFlag(),GetIndexID(),m_nReadDataSize,m_ReadDataBlocks.size(),len,m_lLastMsgTotalSize,m_lLastMsgLen,
                         m_lLastMsgType,m_lLastMsgPreDBNum,m_lLastMsgPosDBNum,m_lLastMsgMemMoveDBPos,m_lLastMsgMemMoveDBSize,m_lLastMsgRemainSize);
            Close();
            return 0x7FFFFFFF;
        }
    }

    if(size > 0)
    {
        PutErrorString(NET_MODULE,"%-15s Get MsgLen Error4!(flag:%d,Index:%d)m_nReadDataSize:%d,ReadDataBlocksSize:%d,size:%d(m_lLastMsgTotalSize:%d,\
                     m_lLastMsgLen:%d,m_lLastMsgType:%d,m_lLastMsgPreDBNum:%d,m_lLastMsgPosDBNum:%d,m_lLastMsgMemMoveDBPos:%d,\
                     m_lLastMsgMemMoveDBSize:%d,m_lLastMsgRemainSize:%d)",
                     __FUNCTION__,GetFlag(),GetIndexID(),m_nReadDataSize,m_ReadDataBlocks.size(),size,m_lLastMsgTotalSize,m_lLastMsgLen,
                     m_lLastMsgType,m_lLastMsgPreDBNum,m_lLastMsgPosDBNum,m_lLastMsgMemMoveDBPos,m_lLastMsgMemMoveDBSize,m_lLastMsgRemainSize);
        len = 0x7FFFFFFF;
    }
    return len;
}

void CServerClient::PushReadDataBack(CDataBlock *pDB)
{
    if(NULL == pDB)	return;
    m_ReadDataBlocks.push_back(pDB);
    m_nReadDataSize += pDB->GetCurSize();
}
CDataBlock *CServerClient::PopReadDataFront()
{
    CDataBlock *pDB = NULL;
    if(m_ReadDataBlocks.size() > 0)
    {
        pDB = m_ReadDataBlocks.front();
        m_ReadDataBlocks.pop_front();
    }
    return pDB;
}
void CServerClient::PushReadDataFront(CDataBlock *pDB)
{
    if(NULL == pDB)	return;
    m_ReadDataBlocks.push_front(pDB);
}

//用数据块的方式接受数据
void CServerClient::OnReceive(int nErrorCode)
{
    if (m_pServers == NULL)	return;
    long recvMsgLen = 0;

    // 判断是否接受了足够长的数据
    while ((m_nReadDataSize >= (HEAD_SIZE + sizeof(CBaseMessage::stMsg)) &&
        (recvMsgLen = GetCurMsgLen()) <= m_nReadDataSize))
    {
        char strInfo[512] = "";
        if (recvMsgLen < long(HEAD_SIZE + sizeof(CBaseMessage::stMsg)))
        {
            PutErrorString(NET_MODULE,"%-15s %s",__FUNCTION__,"error，the got data block length size is to small");
            Close();
            return;
        }

        m_lLastMsgTotalSize = m_nReadDataSize;
        m_lLastMsgLen = recvMsgLen;
        m_lLastMsgPreDBNum = (long)m_ReadDataBlocks.size();

        long curMsgRestSize = recvMsgLen;
        vector<CDataBlock*> curMsgDBs;
        curMsgDBs.clear();

        // 计算数据的开始位置
        CDataBlock* recvDB = PopReadDataFront();
        long recvDBSize = recvDB->GetCurSize();
        if (recvDBSize == 0)
        {
            PutErrorString(NET_MODULE,"%-15s %s",__FUNCTION__,"错误，弹出的消息块长度为0");
        }

        CDataBlock* allocDB = m_pDBAllocator->AllocDB(9);
        long maxDBSize = allocDB->GetMaxSize();
        long allocDBPos = 0;
        long recvDBPos = 0;
        while (curMsgRestSize > 0 && recvDB && allocDB)
        {
            long minSize = min(min(curMsgRestSize, recvDBSize - recvDBPos),
                maxDBSize - allocDBPos);

            memcpy(allocDB->Base() + allocDBPos, recvDB->Base() + recvDBPos, minSize);
            recvDBPos += minSize;
            allocDBPos += minSize;
            curMsgRestSize -= minSize;

            if(recvDBPos >= recvDBSize && curMsgRestSize > 0)
            {
                m_pDBAllocator->FreeDB(recvDB);
                recvDB = PopReadDataFront();
                if(recvDB )
                {
                    recvDBSize = recvDB->GetCurSize();
                    if(recvDBSize == 0)
                    {
                        PutErrorString(NET_MODULE,"%-15s %s",__FUNCTION__,"error，the got data block length size is 0");
                    }
                }
                else
                {
                    PutErrorString(NET_MODULE,"%-15s %s",__FUNCTION__,"error,pop a empty data block!");
                    recvDBSize = 0;
                }
                recvDBPos = 0;
            }

            if(allocDBPos >= maxDBSize && curMsgRestSize > 0)
            {
                allocDB->SetCurSize(maxDBSize);
                curMsgDBs.push_back(allocDB);
                allocDB = m_pDBAllocator->AllocDB(10);
                if(allocDB)
                {
                    maxDBSize = allocDB->GetMaxSize();
                }
                else
                {
                    maxDBSize = 0;
                    PutErrorString(NET_MODULE,"%-15s %s",__FUNCTION__,"error,the pointer allocDB is null");
                }
                allocDBPos = 0;
            }
        }
        allocDB->SetCurSize(allocDBPos);
        curMsgDBs.push_back(allocDB);

        m_lLastMsgRemainSize = curMsgRestSize;
        m_lLastMsgMemMoveDBPos = recvDBPos;
        m_lLastMsgMemMoveDBSize = recvDBSize;

        if (recvDBPos < recvDBSize)
        {
            memmove(recvDB->Base(),
                recvDB->Base() + recvDBPos,
                recvDBSize - recvDBPos);
            recvDB->SetCurSize(recvDBSize-recvDBPos);
            PushReadDataFront(recvDB);
        }
        else if (recvDBPos == recvDBSize)
        {
            m_pDBAllocator->FreeDB(recvDB);
        }

        m_nReadDataSize -= recvMsgLen;
        CBaseMessage* pMsg = CBaseMessage::AllocMsg();
        pMsg->Init(curMsgDBs, m_kn, m_pServers->IsEncryptType(GetFlag()));

        pMsg->SetSocketID(GetIndexID());
        pMsg->SetNetFlag(GetFlag());
        pMsg->SetIP(GetPeerIP());
        pMsg->SetIP(GetDWPeerIP());

        m_lLastMsgType = pMsg->GetType();
        m_lLastMsgPosDBNum = (long)m_ReadDataBlocks.size();
        m_pServers->GetRecvMessages()->PushMessage( pMsg );
    }
}

void CServerClient::OnAccept(int errorCode)
{
    ulong dwPeerIP = GetDWPeerIP();
    CBaseMessage* pMsg = CBaseMessage::AllocMsg();
    pMsg->Init(BASEMSG_Socket_Accept);
    pMsg->SetNetFlag(GetFlag());
    pMsg->SetSocketID(GetIndexID());
    pMsg->SetIP(GetPeerIP());
    pMsg->SetIP(dwPeerIP);
    pMsg->SetTotalSize();
    m_pServers->GetRecvMessages()->PushMessage( pMsg );
}

void CServerClient::OnClose(int errorCode)
{
    CMySocket::OnClose();
    //向逻辑层发送连接断开的消息
    CBaseMessage* pMsg = CBaseMessage::AllocMsg();
    pMsg->Init(BASEMSG_Socket_Close);
    pMsg->SetNetFlag(GetFlag());
    pMsg->SetSocketID(GetIndexID());
    pMsg->SetIP(GetPeerIP());
    pMsg->SetIP(GetDWPeerIP());
    pMsg->SetTotalSize();
    m_pServers->GetRecvMessages()->PushMessage( pMsg );
}

//当网络传输变化的时候
void CServerClient::OnTransferChange()
{
    //当阻塞的消息太多时
    if(m_SendMessages.size() > m_lMaxBlockSendMsnNum/10)
    {
        //通知上层传输拥拥塞
        if(!m_bTransfersStatus)
        {
            m_bTransfersStatus = true;
            CBaseMessage* pMsg = CBaseMessage::AllocMsg();
            pMsg->Init(BASEMSG_Transfer_Congestion);
            pMsg->SetNetFlag(GetFlag());
            pMsg->SetSocketID(GetIndexID());
            pMsg->SetIP(GetPeerIP());
            pMsg->SetIP(GetDWPeerIP());
            m_pServers->GetRecvMessages()->PushMessage( pMsg );
        }
    }
    else if(m_SendMessages.size() <= m_lMaxBlockSendMsnNum/100)
    {
        //通知上层传输畅通
        if(m_bTransfersStatus)
        {
            m_bTransfersStatus = false;
            CBaseMessage* pMsg = CBaseMessage::AllocMsg();
            pMsg->Init(BASEMSG_Transfer_Smoothly);
            pMsg->SetNetFlag(GetFlag());
            pMsg->SetSocketID(GetIndexID());
            pMsg->SetIP(GetPeerIP());
            pMsg->SetIP(GetDWPeerIP());
            m_pServers->GetRecvMessages()->PushMessage( pMsg );
        }
    }
}

long	CServerClient::AddRecvSize(long lSize)
{
    m_pServers->AddRecvSize(GetFlag(),lSize);
    m_lCurTotalRcvSize += lSize;
    //当数据累计达到一定数量的时候,重置统计数据
    if( m_lCurTotalRcvSize >= (m_lMaxRcvSizePerSecond<<2) )
    {
        ulong dwCurTime = timeGetTime();
        ulong dwDifTime = dwCurTime - m_dwRcvStartTime;
        if(dwDifTime ==0)	dwDifTime = 1;
        //计算每秒平均接受大小
        m_lCurRcvSizePerSecond = m_lCurTotalRcvSize*1000/dwDifTime;
        m_dwRcvStartTime = dwCurTime;
        m_lCurTotalRcvSize = 0;
    }
    return m_lCurTotalRcvSize;
}

long CServerClient::AddSendSize(long lSize)
{
    m_pServers->AddSendSize(GetFlag(),lSize);

    ulong dwCurTime = timeGetTime();
    if(dwCurTime - m_dwSendStartTime >= 30000)
    {
        m_dwSendStartTime = dwCurTime;
        m_lCurTotalSendSize = 0;
    }

    m_lCurTotalSendSize += lSize;
    return m_lCurTotalSendSize;
}

//得到当前每秒接受大小
long CServerClient::GetCurRecvSizePerSecond()
{
    return m_lCurRcvSizePerSecond;
}

//得到当前每秒发送大小
long CServerClient::GetCurSendSizePerSecond()
{
    ulong dwDifTime = timeGetTime()-m_dwSendStartTime;
    if(dwDifTime ==0)	dwDifTime = 1;
    //计算每秒平均接受大小
    m_lCurSendSizePerSecond = m_lCurTotalSendSize*1000/dwDifTime;
    return m_lCurSendSizePerSecond;
}

void CServerClient::IncUseCount()
{
    if(m_nUseCount >= 65535)
    {
        PutErrorString(NET_MODULE,"%-15s %s",__FUNCTION__,"连接索引计数超过了最大值.");
        m_nUseCount = 1;
    }
    m_nUseCount++;
}
