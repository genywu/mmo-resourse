/*
// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CTEAMATE_INCLUDED
#define _INC_CTEAMATE_INCLUDED

#include "CPlug.h"

class CTeamate : public CPlug
{
public:
	enum TEAMATE_STATE
	{
		//##定义开始
		TS_BEGIN				= CPlug::PS_USER_DEFINE_BEGIN,
		//##改变队长，需要跨服务器传送
		TS_CHANGE_LEADER,
		//##改变分配方式，需要跨服务器传送
		TS_CHANGE_ALLOCATION_SCHEME,
		//##改变场景，需要跨服务器传送
		TS_CHANGE_REGION,
		//##改变属性，例如HP，MP，RP等
		TS_PROPERTIES_UPDATED,
		//##聊天
		TS_CHAT,
		//##改变状态
		TS_CHANGE_STATE,
	};

private:

	LONG	m_lOwnerRegionID;
	DWORD	m_dwPlayerLoseTimeStamp;
	string	m_strOwnerName;

public:

	CTeamate();

	~CTeamate();

	virtual BOOL IsPlugAvailable();

	virtual BOOL OnSessionEnded();

	virtual BOOL OnSessionAborted();

	virtual BOOL OnPlugInserted();

	virtual BOOL OnPlugAborted();

	virtual BOOL OnPlugEnded();

	virtual BOOL OnChangeState(LONG lPlugID, LONG lStateID, BYTE* pData);

	//virtual VOID Release();

	VOID	SetOwnerRegionID( LONG lID );
	LONG	GetOwnerRegionID();

	VOID	SetOwnerName( CHAR* szName );
	CHAR*	GetOwnerName();

	virtual BOOL Serialize(vector<BYTE>* pStream);

	virtual BOOL Unserialize(BYTE* pStream, LONG& lOffset);

};

#endif
*/