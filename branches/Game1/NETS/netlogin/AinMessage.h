
//! 简单封装的自己发送给自己的消息类（AinMessage）和消息队列（AinMsgQueue）
//! 只有消息类型和数据指针
//! 数据的操作使用：
//!		_AddToByteArray();
//!		_GetBufferFromByteArray();
//!	等函数
//!

#include "..\public\UseMemManageLS.h"

#pragma once

enum eAinMsgType
{
	AIN_MSG_ACCOUNT_Validate_Re,
	AIN_MSG_SDOA_VALIDARE_RETURN,	//! 盛大验证结果返回
	AIN_MSG_SDOA_FCM_NOTIFY,		//! 盛大防沉迷通知
	AIN_MSG_ACCOUNT_Business_Re,    //! 盛大资料查询返回
	AIN_MSG_SDOA_Business_RETURN,	//! 盛大验证结果返回
	AIN_MSG_SDOA_Business_Validate, //! 盛大验证操作
	AIN_MSG_SDOA_Business_Re_Error, //! 盛大验证操作（返回错误）
};

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
	BOOL				GetStr(LPSTR pStr, WORD lBufMaxSize);
	BOOL				GetEx(LPVOID pData, DWORD lGetSize);

private:
	LONG			m_lType;
	vector<BYTE>	m_vData;
	DWORD			m_dwPos;

private:
	AinMessage(LONG lType):m_lType(lType),m_dwPos(0){}
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

	AsyncLock						m_Lock;

	map<DWORD, AinMessage*>			m_timeMsgQueue;
	DWORD							m_dwMinBeginTime;
};