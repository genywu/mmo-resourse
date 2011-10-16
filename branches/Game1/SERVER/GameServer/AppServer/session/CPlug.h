// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CPLUG_41DCD123006D_INCLUDED
#define _INC_CPLUG_41DCD123006D_INCLUDED

#include "..\BaseObject.h"

class CSession;
class CMoveShape;
class CContainer;

//##ModelId=41DCD123006D
class CPlug 
: public CBaseObject
{
private:
	static long s_lIDSeed;

	virtual const CGUID&	GetExID()					{throw;return m_guid;};
	virtual void			SetExID(const CGUID& guid)	{throw;};
	long m_lID;
	
public:
	static long				Create32ID(void)			{return ++s_lIDSeed;};

	long					Get32ID(void)				{return m_lID;};
	void					Set32ID(long lID)			{m_lID = lID;};
	//##ModelId=41DE3A21002E
	enum PLUG_STATE
	{
		//##ModelId=41DE3A2100DA
		PS_INSERTED			= 0,
		//##ModelId=41DE3A2100EA
		PS_REMOVED,
		//##ModelId=41DE3A2100FA
		PS_ABORTED,
		//##ModelId=41DE3A210109
		PS_USER_DEFINE_BEGIN
	};

	//##ModelId=41DE37B900AB
	VOID SetOwner(LONG lType, const CGUID& OwnerID);

	//##ModelId=41DE25B8002E
	virtual BOOL ChangeState(LONG lState, BYTE* pData);

	//该函数主要是向会话发送退出消息。然后等待会话向自己调用On
	//Exit。
	//##ModelId=41DE250E0157
	virtual BOOL Exit();

	//##ModelId=41DCD92303A9
	CPlug();

	//##ModelId=41DCD92B002E
	//CPlug(const CGUID& guid);

	//##ModelId=41DCD932032C
	virtual ~CPlug();

	//##ModelId=41DCEF33006D
	virtual BOOL IsPlugAvailable();

	//##ModelId=41DCD96200EA
	virtual BOOL OnSessionEnded();

	//##ModelId=41DCD97402EE
	virtual BOOL OnSessionAborted();

	//##ModelId=41DDE92303A9
	virtual BOOL OnChangeState(long PlugID, LONG lStateID, BYTE *pData);

protected:
	DWORD m_dwPlugType;

	//##ModelId=41DF70860388
	BOOL m_bPlugEnded;

	//##ModelId=41DE248602EE
	CGUID m_SessionID;

	//##ModelId=41DCD8DE03C8
	LONG m_lOwnerType;

	//##ModelId=41DCD8E8032C
	CGUID m_OwnerID;

	//##ModelId=41DF550A002C
	CSession* GetSession();

	CGUID& GetSessionGUID() { return m_SessionID; }

	VOID	SetPlugType( DWORD dwPlugType );
	DWORD	GetPlugType();

public:
	//##ModelId=41DE49D8003E
	LONG GetOwnerType();

	//##ModelId=41DE49DD034B
	const CGUID& GetOwnerID();

	//##ModelId=41DE5134005D
	CMoveShape* GetOwner();

	//##ModelId=41DF54FD004C
	VOID SetSession(const CGUID& guid);

	//##ModelId=41DE46230213
	virtual BOOL OnPlugInserted();

	//##ModelId=41DE41140261
	virtual BOOL OnPlugAborted();

	//##ModelId=41DE411E034B
	virtual BOOL OnPlugEnded();

	//##ModelId=41DE3FF20186
	virtual BOOL IsPlugEnded();

	//##ModelId=41E1E28F01F4
	virtual VOID Release();

	//##ModelId=41EB65C20399
	virtual CContainer* GetContainer(long lID);

	virtual BOOL Serialize(vector<BYTE>* pStream);

	virtual BOOL Unserialize(BYTE* pStream, LONG& lOffset);
};

#endif /* _INC_CPLUG_41DCD123006D_INCLUDED */
