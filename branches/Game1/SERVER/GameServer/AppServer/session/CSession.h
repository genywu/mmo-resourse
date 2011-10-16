// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CSESSION_41DCC469033C_INCLUDED
#define _INC_CSESSION_41DCC469033C_INCLUDED

#include "..\BaseObject.h"

class CPlug;

//##ModelId=41DCC469033C
class CSession 
: public CBaseObject
{
public:
	//##ModelId=41DE28EC0242
	virtual BOOL IsSessionEnded();

	//##ModelId=41DE28F8031C
	virtual CPlug* QueryPlugByOwner(LONG lType, const CGUID& guid);

	//##ModelId=41DE36050109
	virtual CPlug* QueryPlugByID(long PlugID);

	//##ModelId=41DE197F0290
	virtual VOID Release();

	//##ModelId=41DDF8FA00EA
	virtual BOOL IsSessionAvailable();

	virtual BOOL IsSessionShouldBeenRemoved();

	//##ModelId=41DDF2B20157
	virtual BOOL InsertPlug(long lPlugID);

	//##ModelId=41DCD1EF008C
	CSession(DWORD dwMinPlugs, DWORD dwMaxPlugs, DWORD dwLifeTime);
	
	//##ModelId=41DCD1F60157
	CSession(const CGUID& guid, DWORD dwMinPlugs, DWORD dwMaxPlugs, DWORD dwLifeTime);

	//##ModelId=41DCD2070109
	virtual ~CSession();

	//##ModelId=41DCD273036B
	virtual VOID AI();

	//正常开始
	//##ModelId=41DCD60D0119
	virtual BOOL Start( BOOL bFlag = FALSE );

	//正常结束
	//##ModelId=41DCD61A00DA
	virtual BOOL End( BOOL bFlag = FALSE );

	//异常结束
	//##ModelId=41DCD62A0186
	virtual BOOL Abort( BOOL bFlag = FALSE );

	virtual BOOL OnSessionStarted( BOOL bFlag );

	virtual BOOL OnSessionEnded( BOOL bFlag );

	virtual BOOL OnSessionAborted( BOOL bFlag );

	//##ModelId=41DE46B6004E
	virtual BOOL OnPlugInserted(long PlugID);

	//##ModelId=41DE24E400FA
	virtual BOOL OnPlugEnded(long PlugID);

	//当某个Plug丢失的时候，AI部分会自动调用这个函数。这个函数
	//会通知所有其他的Plug。然后根据他们的返回值来确定是否结束
	//掉本次会话。返回值就是是否结束的标志。
	//##ModelId=41DCD7D0033C
	virtual BOOL OnPlugAborted(long PlugID);

	//##ModelId=41DDE95103A9
	virtual BOOL OnPlugChangeState(long PlugID, LONG lState, BYTE* pData, BOOL bSendToTrigger = FALSE);

	//##ModelId=41E3634A0109
	list<long>& GetPlugList();

	//##ModelId=421158C70330
	VOID SendNotification(CHAR* strMsg, DWORD dwColor = 0xffffffff);

	VOID PlugExit(long lPlugId);

	virtual BOOL Serialize(vector<BYTE>* pStream);

	virtual BOOL Unserialize(BYTE* pStream, LONG& lOffset);

protected:
	DWORD m_dwSessionType;

private:
	//##ModelId=41DE31E700BB
	BOOL m_bSessionEnded;

	//##ModelId=41DE2D520232
	BOOL m_bSessionStarted;

	//##ModelId=41DE2D7401D4
	BOOL m_bSessionAborted;

	//##ModelId=41DCC5110167
	DWORD m_dwMaximumPlug;

	//##ModelId=41DCD02101F4
	DWORD m_dwMinimumPlug;

	//##ModelId=41DCD08E000F
	DWORD m_dwStartingTimeStamp;

	//##ModelId=41DCD0A7000F
	DWORD m_dwLifeTime;

	//##ModelId=41DCD1320148
	list<long> m_lPlugs;

	BOOL m_bRemoveFlag;
};

#endif /* _INC_CSESSION_41DCC469033C_INCLUDED */
