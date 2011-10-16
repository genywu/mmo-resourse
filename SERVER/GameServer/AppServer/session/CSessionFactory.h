// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CSESSIONFACTORY_41DDF0D60109_INCLUDED
#define _INC_CSESSIONFACTORY_41DDF0D60109_INCLUDED
#include <hash_map>
using namespace stdext;

class CSession;
class CPlug;

//##ModelId=41DDF0D60109
class CSessionFactory 
{
public:
	//##ModelId=41EE157C0290
	enum PLUG_TYPE
	{
		//##ModelId=41EE157C032C
		PT_NORMAL_PLUG,
		PT_TRADER,
		PT_PERSONAL_SHOP_SELLER,
		PT_PERSONAL_SHOP_BUYER,
		PT_EQUIPMENT_UPGRADE,
		PT_EQUIPMENT_ENCHASE,
		PT_TEAMATE,	
	};

	enum SESSION_TYPE
	{
		ST_NORMAL_SESSION,
		ST_TEAM,
	};

	//##ModelId=41EE149B0167
	static long CreatePlug( PLUG_TYPE pt, LONG lType, const CGUID& guid );

	//##ModelId=41DDF2410119
	static const CGUID& CreateSession(DWORD dwMinPlugs = 1, DWORD dwMaxPlugs = 1, DWORD dwLifeTime = 0, SESSION_TYPE st = ST_NORMAL_SESSION);

	//##ModelId=41DDF445000F
	static const CSession* QuerySession(const CGUID& SessionGuid);

	//##ModelId=41EE15B80261
	static const CPlug* QueryPlug(long lPlugID);

	//##ModelId=41DDF848034B
	static BOOL InsertPlug(const CGUID& SessionGuid, long lPlugID);

	//##ModelId=41DDF3B1036B
	static VOID AI();

	//##ModelId=41DE00BF0399
	static VOID Release();

	static CGUID UnserializeSession( BYTE* pStream, LONG& lOffset );

	static long UnserializePlug( BYTE* pStream, LONG& lOffset );

private:
	//##ModelId=41DDF142029F
	static LONG s_lSessionID;

	//##ModelId=41EE17AC0128
	static LONG s_lPlugID;

	//##ModelId=41DDF1B40196
	static hash_map<CGUID, CSession*, hash_guid_compare>  s_mSessions;

	//##ModelId=41EE15F801C5
	static hash_map<long, CPlug*> s_mPlugs;

protected:	
	static VOID GarbageCollect_Session(const CGUID& guid);
	static VOID GarbageCollect_Plug(long lPlugID);

	friend class CSession;
};

#endif /* _INC_CSESSIONFACTORY_41DDF0D60109_INCLUDED */
