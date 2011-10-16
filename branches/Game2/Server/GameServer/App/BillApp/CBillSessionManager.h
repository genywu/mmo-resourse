#pragma once


class CBillSession;

class CBillSessionManager 
{
public:	
	
	static CBillSession* CreateSession(const char* strCdkey);
	
	static const CBillSession* QuerySession(const CGUID& SessionGuid);			

	static void AI();
	
	static void Release();
private:	
	static LONG s_lSessionID;	
	
	static hash_map<CGUID, CBillSession*, hash_guid_compare>  s_mSessions;		

protected:	
	static void GarbageCollect_Session(const CGUID& guid);	

	friend class CBillSession;
};