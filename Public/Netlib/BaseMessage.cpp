
//=============================================================================
/**
*  file: basemessage.cpp
*
*  Brief:游戏的消息都从本类派生
*		包含每个消息的基本内容（大小、类型和时间）
*		提供基本的组装消息、发送消息功能
*
*  Authtor:wangqiao
*	
*	Datae:2005-4-15
*
*	Modify:2007-4-13,整理了代码和结构，优化了效率
*/
//=============================================================================

#include "stdafx.h"
#include "Basemessage.h"
#include "MsgCry.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDataBlockAllocator* CBaseMessage::m_pDBAllocator = NULL;
set<CBaseMessage*>	CBaseMessage::TestMsg;

ulong CBaseMessage::m_nMaxFreeMsgNum = 0;
CBaseMessage::listBaseMsgs CBaseMessage::m_FreeBaseMessages;
CRITICAL_SECTION CBaseMessage::m_CSFreeMsg;
CBaseMessage::NEWMSG CBaseMessage::NewMsg;
CBaseMessage::mapMsgPrioLvlParams CBaseMessage::m_MsgPrioLvl;

CBaseMessage::setDiscardMsgs CBaseMessage::m_DiscardMsgs;

CBaseMessage::CBaseMessage()
: m_bEncrypted (false)
, m_lMsgTotalSize (0)
{
    m_MsgData.clear();
    m_ReadParam.nCurNum = 0;
    m_ReadParam.nCurDBSize = 0;
    m_ReadParam.nCurPos = 0;
    m_ReadParam.pDBPtr = NULL;

    m_WriteParam.nCurNum = 0;
    m_WriteParam.nCurDBSize = 0;
    m_WriteParam.nCurPos = 0;
    m_WriteParam.pDBPtr = NULL;

    m_lRefCount = 0;
}

CBaseMessage::~CBaseMessage()
{
}

void CBaseMessage::Init(unsigned long type)
{
    m_lNetFlag = 0;
    m_lPriorityLvl = GetMsgPrio(type);
    m_dwStartSendTime = 0;
    m_lRefCount = 1;

    CDataBlock* pDB = CBaseMessage::m_pDBAllocator->AllocDB(type);
    m_MsgData.push_back(pDB);

    m_ReadParam.nCurNum = 0;
    m_ReadParam.nCurDBSize = 0;
    m_ReadParam.nCurPos = 0;
    m_ReadParam.pDBPtr = NULL;

    m_WriteParam.nCurNum = 0;
    m_WriteParam.nCurPos = 0;
    m_WriteParam.nCurDBSize = m_MsgData[m_WriteParam.nCurNum]->GetMaxSize();
    m_WriteParam.pDBPtr = m_MsgData[m_WriteParam.nCurNum]->Base();

    stMsg st = {0, type,0};
    m_WriteParam.nCurPos += HEAD_SIZE;
    Add((BYTE*)&st, sizeof(stMsg));
}

void CBaseMessage::Init(vector<CDataBlock*>& MsgData, const unsigned char kn[16][6], bool bDecrypt)
{
    m_lNetFlag = 0;
    m_lPriorityLvl = MAX_MSG_PRIO;
    m_dwStartSendTime = 0;
    m_lRefCount = 1;

    m_ReadParam.nCurNum = 0;
    m_ReadParam.nCurDBSize = 0;
    m_ReadParam.nCurPos = 0;
    m_ReadParam.pDBPtr = NULL;

    m_WriteParam.nCurNum = 0;
    m_WriteParam.nCurDBSize = 0;
    m_WriteParam.nCurPos = 0;
    m_WriteParam.pDBPtr = NULL;

    m_MsgData = MsgData;
    if(m_MsgData.size() > 0)
    {
        m_ReadParam.nCurDBSize = m_MsgData[m_ReadParam.nCurNum]->GetCurSize();
        m_ReadParam.pDBPtr = m_MsgData[m_ReadParam.nCurNum]->Base();
        m_ReadParam.nCurPos += sizeof(stMsg);
        m_ReadParam.nCurPos += HEAD_SIZE;

        //如果在接受的数据上继续写，设置写的位置
        m_WriteParam.nCurNum = (long)m_MsgData.size()-1;
        m_WriteParam.nCurPos = m_MsgData[m_WriteParam.nCurNum]->GetCurSize();
        m_WriteParam.nCurDBSize =  m_MsgData[m_WriteParam.nCurNum]->GetMaxSize();
        m_WriteParam.pDBPtr = m_MsgData[m_WriteParam.nCurNum]->Base();

        if (bDecrypt)
        {
            Decrypt(kn);
        }
    }
    else
    {
        PutErrorString(NET_MODULE,"%-15s %s",__FUNCTION__,"严重错误,初始化消息的时候,数据为空!");
    }
}

void CBaseMessage::UnInit()
{
    m_bEncrypted = false;
    m_lMsgTotalSize = 0;
    itMsgData it = m_MsgData.begin();
    for(;it != m_MsgData.end();it++)
    {
        CBaseMessage::m_pDBAllocator->FreeDB((*it));
    }
    m_MsgData.clear();
}


bool CBaseMessage::Validate()
{
    if(m_MsgData.size() == 0)	return false;
    if( m_MsgData[0]->GetCurSize() < (HEAD_SIZE+sizeof(stMsg)) )	return false;
    if( *((long*)m_MsgData[0]->Base()) != (GetSize()+HEAD_SIZE) )	return false;
    return true;
}

//克隆消息数据
void CBaseMessage::Clone(CBaseMessage *pMsg)
{
    itMsgData it = m_MsgData.begin();
    for(;it != m_MsgData.end();it++)
    {
        //默认所有的消息块一样大
        CDataBlock *pSourDB = (*it);
        CDataBlock *pDestDB =  m_pDBAllocator->AllocDB(2);
        int minsize = min(pSourDB->GetCurSize(),pDestDB->GetMaxSize());
        if(minsize < pSourDB->GetCurSize())
        {
            Log4c::Warn(NET_MODULE,"在函数CBaseMessage::Clone()中，数据块大小不统一。");
        }
        memcpy(pDestDB->Base(),pSourDB->Base(),minsize);
        pDestDB->SetCurSize(minsize);
        pMsg->m_MsgData.push_back(pDestDB);
    }

    //设置相同的优先级
    pMsg->SetPriorityLvl(m_lPriorityLvl);
}

void CBaseMessage::AddWrDataBlock()
{
    m_WriteParam.nCurNum++;
    m_WriteParam.nCurDBSize = 0;
    m_WriteParam.nCurPos = 0;
    m_WriteParam.pDBPtr = NULL;

    CDataBlock* pDB = CBaseMessage::m_pDBAllocator->AllocDB(3);
    if(pDB == NULL)
    {
        PutErrorString(NET_MODULE,"%-15s %s",__FUNCTION__,"在函数CBaseMessage::AddWrDataBlock()中， 分配数据块出错。");
        return;
    }

    m_MsgData.push_back(pDB);
    m_WriteParam.nCurDBSize = m_MsgData[m_WriteParam.nCurNum]->GetMaxSize();
    m_WriteParam.pDBPtr = m_MsgData[m_WriteParam.nCurNum]->Base();
}

void CBaseMessage::AddRdDataBlock()
{
    m_ReadParam.nCurNum++;
    m_ReadParam.nCurDBSize = 0;
    m_ReadParam.nCurPos = 0;
    m_ReadParam.pDBPtr = NULL;
    if(m_ReadParam.nCurNum < m_MsgData.size())
    {
        m_ReadParam.nCurDBSize = m_MsgData[m_ReadParam.nCurNum]->GetCurSize();
        m_ReadParam.pDBPtr = m_MsgData[m_ReadParam.nCurNum]->Base();
    }
}

bool CBaseMessage::Initial(CDataBlockAllocator* pDBAllocator,long nMaxFreeMsgNum)
{
    ::EAInit();
    m_pDBAllocator = pDBAllocator;
    InitializeCriticalSection(&m_CSFreeMsg);
    m_nMaxFreeMsgNum = nMaxFreeMsgNum;
    ulong i = 0;
    for(;i< m_nMaxFreeMsgNum;i++)
    {
        CBaseMessage *pMsg = NewMsg();
        m_FreeBaseMessages.push_back(pMsg);
    }

    m_MsgPrioLvl.clear();
    m_DiscardMsgs.clear();
    return true;
}
bool CBaseMessage::Release()
{
    m_pDBAllocator = NULL;
    itBaseMsg it = m_FreeBaseMessages.begin();
    for(;it != m_FreeBaseMessages.end();it++)
    {
        SAFE_DELETE((*it));
    }
    m_FreeBaseMessages.clear();
    DeleteCriticalSection(&m_CSFreeMsg);
    return true;
}

CBaseMessage* __stdcall CBaseMessage::AllocMsg()
{
    CBaseMessage* pMsg = NULL;
    EnterCriticalSection(&m_CSFreeMsg);
    if(m_FreeBaseMessages.size() > 0)
    {
        pMsg = m_FreeBaseMessages.front();
        m_FreeBaseMessages.pop_front();			
    }

    if(NULL == pMsg)
        pMsg = NewMsg();
    pMsg->SetRefCount(1);

    //TestMsg.insert(pMsg);
    LeaveCriticalSection(&m_CSFreeMsg);
    return pMsg;
}

void __stdcall CBaseMessage::FreeMsg(CBaseMessage* pMsg)
{
    if(NULL == pMsg)	return;
    pMsg->UnInit();	
    EnterCriticalSection(&m_CSFreeMsg);
    //TestMsg.erase(pMsg);
    if(m_FreeBaseMessages.size() < m_nMaxFreeMsgNum)
    {
        pMsg->Reset();
        m_FreeBaseMessages.push_back(pMsg);
    }
    else
    {
        SAFE_DELETE(pMsg);
    }

    LeaveCriticalSection(&m_CSFreeMsg);
}

////////////////////////////////////////////////////////////////////////
//	添加和获取数据
////////////////////////////////////////////////////////////////////////

/// 添加 ///

void CBaseMessage::Add(char l)
{
    Add( (BYTE*)&l,sizeof(char) );
}

void CBaseMessage::Add(BYTE l)
{
    Add( (BYTE*)&l,sizeof(BYTE) );
}

void CBaseMessage::Add(short l)
{
    Add( (BYTE*)&l,sizeof(short) );
}

void CBaseMessage::Add(WORD l)
{
    Add( (BYTE*)&l,sizeof(WORD) );
}

void CBaseMessage::Add(long l)
{
    Add( (BYTE*)&l,sizeof(long) );
}

void CBaseMessage::Add(LONG64 l)
{
    Add( (BYTE*)&l,sizeof(LONG64) );
}

void CBaseMessage::Add(ulong l)
{
    Add( (BYTE*)&l,sizeof(ulong) );
}

void CBaseMessage::Add(float l)
{
    Add( (BYTE*)&l,sizeof(float) );
}

void CBaseMessage::Add(const char* pStr)
{
    long size = (long)(strlen(pStr));
    Add( (BYTE*)&size,sizeof(long) );
    Add( (BYTE*)pStr,size );
}

void CBaseMessage::Add(const CGUID&	guid)
{
    long size = sizeof(CGUID);
    if(guid == CGUID::GUID_INVALID)
    {
        size = 0;
    }
    Add((BYTE)size);
    if(size != 0)
        Add((BYTE*)&guid,size);
}

void CBaseMessage::Add(BYTE* pBuf, ulong size)
{
    long lTempSize = size;
    while(m_WriteParam.pDBPtr != NULL && size > 0)
    {
        if(m_WriteParam.nCurDBSize >= m_WriteParam.nCurPos)
        {
            ulong minsize = min(size,m_WriteParam.nCurDBSize-m_WriteParam.nCurPos);
            memcpy(m_WriteParam.pDBPtr+m_WriteParam.nCurPos,pBuf,minsize);
            m_WriteParam.nCurPos += minsize;
            m_MsgData[m_WriteParam.nCurNum]->SetCurSize(m_WriteParam.nCurPos);

            size -= minsize;
            pBuf += minsize;
            if(m_WriteParam.nCurPos >= m_WriteParam.nCurDBSize && size > 0)
            {
                AddWrDataBlock();
            }
        }
        else
        {
            size = 0;
            PutErrorString(NET_MODULE,"%-15s %s",__FUNCTION__,"在函数CBaseMessage::Add()中， 压数据出错。");
        }
    }
    SetSize(GetSize()+lTempSize);
}

void CBaseMessage::AddEx(void* pBuf, long size)
{
    BYTE* p = (BYTE*)pBuf;
    Add(size);
    Add((BYTE*)pBuf,size);
}

bool CBaseMessage::GetDBWriteSet(tagDataBlockWriteSet& DBWriteSet)
{
    DBWriteSet.Initial(CBaseMessage::m_pDBAllocator,
        &m_WriteParam,&m_MsgData,&(((stMsg*)GetMsgBuf())->lSize));
    return true;
}

/// 获取 ///

char CBaseMessage::GetChar()
{
    char l  = 0;
    Get((BYTE*)&l,sizeof(char));
    return l;
}

BYTE CBaseMessage::GetByte()
{
    BYTE l = 0;
    Get((BYTE*)&l,sizeof(BYTE));
    return l;
}

short CBaseMessage::GetShort()
{
    short l = 0;
    Get((BYTE*)&l,sizeof(short));
    return l;
}

WORD CBaseMessage::GetWord()
{
    WORD l= 0;
    Get((BYTE*)&l,sizeof(WORD));
    return l;
}

long CBaseMessage::GetLong()
{
    long l = 0;
    Get((BYTE*)&l,sizeof(long));
    return l;
}

LONG64 CBaseMessage::GetLONG64()
{
    LONG64 l = 0;
    Get((BYTE*)&l,sizeof(LONG64));
    return l;
}

ulong CBaseMessage::GetDWord()
{
    ulong l = 0;
    Get((BYTE*)&l,sizeof(ulong));
    return l;
}

float CBaseMessage::GetFloat()
{
    float l = 0;
    Get((BYTE*)&l,sizeof(float));
    return l;
}

bool  CBaseMessage::GetGUID(CGUID& guid)
{
    long size = GetByte();
    if(size == 0)
    {
        guid = CGUID::GUID_INVALID;
        return false;
    }

    Get((BYTE*)&guid,sizeof(CGUID));
    return true;
}


char* CBaseMessage::GetStr( char* pStr,long lMaxLen )
{
    if(lMaxLen <= 1)	return NULL;
    long len = GetLong();
    if(len < 0)	len = 0;
    len = min(len,lMaxLen-1);
    Get((BYTE*)pStr,len);
    pStr[len] = '\0';
    return pStr;
}

//得到读数据
bool CBaseMessage::GetDBReadSet(tagDataBlockReadSet& DBReadSet)
{
    DBReadSet.Initial(&m_ReadParam,&m_MsgData);
    return true;
}


void* CBaseMessage::Get(BYTE* pBuf, ulong size)
{
    while(m_ReadParam.pDBPtr != NULL && size > 0)
    {
        if(m_ReadParam.nCurDBSize >= m_ReadParam.nCurPos)
        {
            ulong minsize = min(size,m_ReadParam.nCurDBSize-m_ReadParam.nCurPos);
            memcpy(pBuf, m_ReadParam.pDBPtr+m_ReadParam.nCurPos, minsize);
            size -= minsize;
            m_ReadParam.nCurPos += minsize;
            pBuf += minsize;
            if(m_ReadParam.nCurPos >= m_ReadParam.nCurDBSize)
                AddRdDataBlock();
        }
        else
        {
            size = 0;
            PutErrorString(NET_MODULE,"%-15s %s",__FUNCTION__,"在函数CBaseMessage::Get()中， 得数据出错。");
        }
    }

    return pBuf;
}

void* CBaseMessage::GetEx(void* pBuf, long size)
{	
    if(size <= 0)	return NULL;

    long minsize = GetLong();
    if(minsize < 0)
        minsize = 0;
    minsize = min(minsize,size);
    Get((BYTE*)pBuf,minsize);
    return pBuf;
}

void CBaseMessage::Reset(void)
{
}

//设置消息的优先级数据
//nPrio[1-99]值越小,优先级越高
void CBaseMessage::RegisterMsgPrio(long lMsgType,short nPrio)
{
    if(MAX_MSG_PRIO >= nPrio )
        m_MsgPrioLvl[lMsgType] = MAX_MSG_PRIO-nPrio;
}
short CBaseMessage::GetMsgPrio(long lMsgType)
{
    itMsgPrio it = m_MsgPrioLvl.find(lMsgType);
    if(it != m_MsgPrioLvl.end())
        return (*it).second;
    return MAX_MSG_PRIO;
}

//得到计算出优先级别值
long CBaseMessage::GetPriorityValue(ulong dwCurTime)
{
    ulong dwDifTime = dwCurTime-m_dwStartSendTime;
    return (m_lPriorityLvl << 10) + dwDifTime;
}


void CBaseMessage::RegisterDiscardMsg(long lMsgType)
{
    m_DiscardMsgs.insert(lMsgType);
}
//得到该消息类型是否可以丢弃
bool CBaseMessage::GetIsDiscard(long lMsgType)
{
    itDiscaMsg it = m_DiscardMsgs.find(lMsgType);
    if(it != m_DiscardMsgs.end())
        return true;
    return false;
}

/// 对消息进行加密
void CBaseMessage::Encrypt(const unsigned char kn[16][6])
{
    if (!m_bEncrypted)
    {
        m_bEncrypted = true;
        vector<CDataBlock*>& DataBlocks = GetMsgData();
        itMsgData itDB = DataBlocks.begin();
        if (itDB!=DataBlocks.end())
        {
            CDataBlock* pDB = (*itDB);
            ::Encrypt(pDB->Base() + HEAD_SIZE, pDB->GetCurSize() - HEAD_SIZE, kn);
        }
    }
}

/// 对消息进行解密
void CBaseMessage::Decrypt(const unsigned char kn[16][6])
{
    vector<CDataBlock*>& DataBlocks = GetMsgData();
    itMsgData itDB = DataBlocks.begin();
    if (itDB!=DataBlocks.end())
    {
        CDataBlock* pDB = (*itDB);
        ::Decrypt(pDB->Base() + HEAD_SIZE, pDB->GetCurSize() - HEAD_SIZE, kn);
    }
}

//得到总大小(包括实际消息大小和消息前的附加头)
unsigned long CBaseMessage::GetTotalSize(void)
{
    return m_lMsgTotalSize ? m_lMsgTotalSize : (m_lMsgTotalSize = GetSize() + HEAD_SIZE);
}
