/**
 * @file Public/Nets/BaseMessage.h
 *
 * @brief 游戏的消息都从本类派生包含每个消息的基本内容（大小、类型和时间）
 * 提供基本的组装消息、发送消息功能
 *
 * @authtor wangqiao
 * @date 2005-4-15
 * Modify 2007-4-13,整理了代码和结构，优化了效率
 */

#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MsgQueue.h"
#include "../DataStream/DataBlock.h"
#include "../DataStream/DataBlockAllocator.h"
#include "../DataStream/DataBlockSetWriteRead.h"

class CMySocket;
class CMessage;
class CGUID;
struct tagDataBlockWriteSet;
struct tagDataBlockReadSet;

//定义消息的最大优先级
const int MAX_MSG_PRIO = 100;
const int HEAD_SIZE = 4;

enum eBaseMsgType
{
	BASEMSG_Socket_Accept = 1,
	BASEMSG_Socket_Close,
	BASEMSG_Transfer_Congestion,//传输拥塞
	BASEMSG_Transfer_Smoothly,//传输畅通
};

class CBaseMessage
{
public:

	CBaseMessage();
	virtual ~CBaseMessage();

	struct stMsg
	{
		ulong lSize;		// 消息大小
		ulong eType;		// 消息类型
		ulong lVerifyCode;	// 校验码
	};

	////////////////////////////////////////////////////////////////////////
	//	基本数据
	////////////////////////////////////////////////////////////////////////
private:

	//消息的数据部分
	typedef vector<CDataBlock*>::iterator itMsgData;
	vector<CDataBlock*> m_MsgData;
	
	//总数据大小
	
	tagDBOpParam m_ReadParam;
	////当前读数据块编号
	//long	m_nCurRdNum;
	////当前读数据块的大小
	//long	m_nCurRdDBSize;
	////当前读数据的位置
	//long	m_nCurRdPos;
	////当前读数据块的指针
	//uchar	*m_pRdDBPtr;

	tagDBOpParam m_WriteParam;
	////当前写数据块编号
	//long	m_nCurWrNum;
	////当前写数据块的大小
	//long	m_nCurWrDBSize;
	////当前写数据的位置
	//long	m_nCurWrPos;
	////当前写数据块的指针
	//uchar	*m_pWrDBPtr;

	long	m_lNetFlag;
	//该消息的优先级别
	long	m_lPriorityLvl;
	//开始发送该消息的事件
	ulong	m_dwStartSendTime;
	//消息的引用计数
	long m_lRefCount;

protected:
	uchar* Base()
	{
		if(m_MsgData.size() > 0)
			return m_MsgData[0]->Base();
		return NULL;
	}

	uchar* GetMsgBuf()
	{
		return m_MsgData[0]->Base()+HEAD_SIZE;
	}
	
	void SetSize(ulong l)	{((stMsg*)GetMsgBuf())->lSize = l;}

	void Add(uchar*, ulong size);
	void* Get(uchar*, ulong size);
public:
	void Init(ulong type);
	void Init(vector<CDataBlock*>& MsgData, const uchar kn[16][6], bool bDecrypt);
	void UnInit();
	//验证消息内容是否合法
	bool Validate();

	void Clone(CBaseMessage *pMsg);

	void AddWrDataBlock();
	void AddRdDataBlock();

	void SetType(ulong t)
	{
        if (m_MsgData.empty() ||
            m_MsgData[0]->GetMaxSize() < (HEAD_SIZE+sizeof(stMsg)) )
			return;	
		((stMsg*)GetMsgBuf())->eType = t;
	}

	//void SetTime(ulong l)
	//{
	//	if(m_MsgData.size()==0 || m_MsgData[0]->GetMaxSize() < (HEAD_SIZE+sizeof(stMsg)))
	//		return;	
	//	((stMsg*)GetMsgBuf())->lTime = l;
	//}
	void SetCode(ulong l)
	{
		if (m_MsgData.empty() ||
            m_MsgData[0]->GetMaxSize() < (HEAD_SIZE+sizeof(stMsg)))
            return;	
		((stMsg*)GetMsgBuf())->lVerifyCode = l;
	}
	ulong GetSize()
	{
		if(m_MsgData.size()==0 || m_MsgData[0]->GetMaxSize() < (HEAD_SIZE+sizeof(stMsg)))
			return 0;	
		return ((stMsg*)GetMsgBuf())->lSize;
	}
	ulong GetType()
	{
		if(m_MsgData.size()==0 || m_MsgData[0]->GetMaxSize() < (HEAD_SIZE+sizeof(stMsg)))
			return 0;	
		return ((stMsg*)GetMsgBuf())->eType;
	}
	/*ulong GetMsgSendTime()
      {
      if(m_MsgData.size()==0 || m_MsgData[0]->GetMaxSize() < (HEAD_SIZE+sizeof(stMsg)))
      return 0;	
      return ((stMsg*)GetMsgBuf())->lTime;
      }*/
	ulong GetCode()
	{
		if(m_MsgData.size()==0 || m_MsgData[0]->GetMaxSize() < (HEAD_SIZE+sizeof(stMsg)))
			return 0;		
		return ((stMsg*)GetMsgBuf())->lVerifyCode;
	}

	//得到总大小(包括实际消息大小和消息前的附加头)
	ulong GetTotalSize(void);/*
                               {
                               return m_lMsgTotalSize ? m_lMsgTotalSize : (m_lMsgTotalSize = GetSize() + HEAD_SIZE);
                               }*/
	void SetTotalSize() { *((long*)Base()) = GetSize()+HEAD_SIZE; }

	vector<CDataBlock*>& GetMsgData() {return m_MsgData;}
	void SetMsgData(vector<CDataBlock*>& MsgData)	{ UnInit(); m_MsgData=MsgData; }
	void ClearMsgData()		{m_MsgData.clear();}

	void SetRefCount(long lNum)	{ m_lRefCount = lNum; }
	void AddRefCount(long lNum)	{ m_lRefCount += lNum; }
	long RemoveRefCount()		{ return --m_lRefCount; }
	void Encrypt(const uchar kn[16][6]);	/// 对消息进行加密
	void Decrypt(const uchar kn[16][6]);	/// 对消息进行解密

	////////////////////////////////////////////////////////////////////////
	//	添加和获取数据
	////////////////////////////////////////////////////////////////////////
public:
	void Add(char);
	void Add(uchar);
	void Add(short);
	void Add(ushort);
	void Add(long);
	void Add(LONG64);
	void Add(ulong);
	void Add(float);
	void Add(const char*);
	void Add(const CGUID&	guid);
	void AddEx(void*, long size);//增加安全性，和GetEx配合使用
	bool GetDBWriteSet(tagDataBlockWriteSet& DBWriteSet);

	virtual void Reset(void);           //清空消息内容

	
	char GetChar();
	uchar GetByte();
	short GetShort();
	ushort GetWord();
	long GetLong();
	LONG64 GetLONG64();
	ulong GetDWord();
	float GetFloat();
	bool  GetGUID(CGUID& guid);
	void* GetEx(void*, long size);
	char* GetStr( char* pStr,long lMaxLen );
	bool GetDBReadSet(tagDataBlockReadSet& DBReadSet);
	
protected:
	bool m_bEncrypted;						/// 消息无需加密或已加密
	ulong m_lMsgTotalSize;			/// 防止加密破坏
	typedef list<CBaseMessage*>	listBaseMsgs;
	typedef list<CBaseMessage*>::iterator itBaseMsg;
	static ulong m_nMaxFreeMsgNum;
	static listBaseMsgs m_FreeBaseMessages;
	static CRITICAL_SECTION m_CSFreeMsg;

	//保存消息的相关优先级数据
	typedef map<long,short>	mapMsgPrioLvlParams;
	typedef map<long,short>::iterator itMsgPrio;
	//保存部分消息的优先级级别
	static mapMsgPrioLvlParams m_MsgPrioLvl;

	typedef set<long>	setDiscardMsgs;
	typedef set<long>::iterator itDiscaMsg;
	//保存可以丢弃的消息类型
	static setDiscardMsgs m_DiscardMsgs;
public:
	virtual void SetSocketID(long lSocketID) = 0;
	virtual long GetSocketID()=0;

	virtual void SetIP(ulong dwIP)=0;
	virtual ulong GetIP()=0;

	virtual void SetIP(const char* pszIP){};

	void SetNetFlag(long lFlag)	{m_lNetFlag = lFlag;}
	long GetNetFlag()			{return m_lNetFlag;}

	//设置消息的优先级数据
	void SetPriorityLvl(long lPrioLvl)	{m_lPriorityLvl=lPrioLvl;}
	void SetStartSendTime(ulong dwSendTime) {m_dwStartSendTime=dwSendTime;}
	//得到计算出优先级别值
	long GetPriorityValue(ulong dwCurTime);
	//判断两个消息的丢弃标记是否一样
	virtual bool IsDiscardFlagEqual(CBaseMessage* pMsg) {return false;}

	static CDataBlockAllocator*	m_pDBAllocator;
	static set<CBaseMessage*>	TestMsg;

	static bool Initial(CDataBlockAllocator* pDBAllocator,long nMaxFreeMsgNum);
	static bool Release();

	static void RegisterMsgPrio(long lMsgType,short nPrio);
	static short GetMsgPrio(long lMsgType);

	static void RegisterDiscardMsg(long lMsgType);
	//得到该消息类型是否可以丢弃
	static bool GetIsDiscard(long lMsgType);

	//申请具体消息的函数
	typedef CBaseMessage*(*NEWMSG)();
	static NEWMSG NewMsg; 

	static CBaseMessage* __stdcall AllocMsg();
	static void __stdcall FreeMsg(CBaseMessage* pMsg);

	////////////////////////////////////////////////////////////////////////
	//	消息处理
	////////////////////////////////////////////////////////////////////////
	virtual long DefaultRun() = 0;     //缺省的消息解析函数
	virtual long Run() = 0;            //纯虚函数，消息运行函数；

	friend class CMsgQueue;
};

#endif
