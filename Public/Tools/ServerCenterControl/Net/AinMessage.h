
//! 简单封装的自己发送给自己的消息类（AinMessage）和消息队列（AinMsgQueue）
//! 只有消息类型和数据指针
//! 数据的操作使用：
//!		_AddToByteArray();
//!		_GetBufferFromByteArray();
//!	等函数
//!

//#include "../../public/UseMemManageSCC.h"

#pragma once


//////////////////////////////////////////////////
class AinMessage
{
public:
	static AinMessage*	Create(LONG lType);
	static VOID			Release(AinMessage **ppAinMsg);

public:
	LONG				GetType(VOID){return m_lType;}
	VOID				Run(VOID);

public:
	VOID				AddByte(BYTE cData);
	VOID				AddWord(WORD wData);
	VOID				AddDword(DWORD dwData);
	VOID				AddStr(LPCSTR pStr);
	VOID				AddEx(LPCVOID pData, DWORD dwAddSize);

	BYTE				GetByte(VOID);
	WORD				GetWord(VOID);
	DWORD				GetDword(VOID);
	BOOL				GetStr(LPSTR pStr, DWORD lBufMaxSize);
	BOOL				GetEx(LPVOID pData, DWORD lGetSize);
	
private:
	LONG			m_lType;
	vector<BYTE>	m_vData;
	LONG			m_lPos;

private:
	AinMessage(LONG lType):m_lType(lType),m_lPos(0){}
	~AinMessage(VOID){}
	AinMessage(AinMessage&);
	AinMessage& operator=(AinMessage&);
};



class AinMsgQueue
{
public:
	AinMsgQueue(VOID);
	~AinMsgQueue(VOID);

public:
	//!				
	VOID			Release(VOID);
	//!				得到消息数目
	LONG			GetSize(VOID);
	//!				压入消息
	BOOL			PushMessage(AinMessage* pMsg);
	//!				压入一个定时消息
	BOOL			PushTimeMessage(AinMessage* pMsg, DWORD dwMillisecond);
	//!				弹出消息
	VOID			PopMessage(list<AinMessage*> &listMsg);
	//!				清空消息（不释放内存）
	VOID			Clear(VOID);

private:
	list<AinMessage*>				m_msgQueue;
	
	LockOfWin32						m_Lock;

	map<DWORD, AinMessage*>			m_timeMsgQueue;
	DWORD							m_dwMinBeginTime;
};