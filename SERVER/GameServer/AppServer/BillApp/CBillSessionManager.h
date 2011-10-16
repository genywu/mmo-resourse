#pragma once
#include <hash_map>
using namespace stdext;

class CBillSession;

class CBillSessionManager 
{
public:	
	
	static CBillSession* CreateSession(const char* strCdkey);
	
	static const CBillSession* QuerySession(const CGUID& SessionGuid);			

	static VOID AI();
	
	static VOID Release();
private:	
	static LONG s_lSessionID;	
	
	static hash_map<CGUID, CBillSession*, hash_guid_compare>  s_mSessions;		

protected:	
	static VOID GarbageCollect_Session(const CGUID& guid);	

	friend class CBillSession;
};