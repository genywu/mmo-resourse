#pragma once
#include <hash_map>
using namespace stdext;

class CSession;

class CSessionFactory 
{
public:	
	
	static CSession* CreateSession(const char* strCdkey);
	
	static const CSession* QuerySession(const CGUID& SessionGuid);			

	static VOID AI();
	
	static VOID Release();
private:	
	static LONG s_lSessionID;	
	
	static hash_map<CGUID, CSession*, hash_guid_compare>  s_mSessions;		

protected:	
	static VOID GarbageCollect_Session(const CGUID& guid);	

	friend class CSession;
};