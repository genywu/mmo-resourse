//CSessionFactory.h/////////////////////////////////////////////////////////////////////
//对象类:会话对象管理类
//Author:安海川
//Create Time:2008.11.03
#ifndef _INC_CSESSIONFACTORY_H
#define _INC_CSESSIONFACTORY_H
#include <map>
#include "../public/GUID.h"
#include "../public/DBDefine.h"

class CSession;

//##ModelId=41DDF0D60109
class CSessionFactory 
{
public:

	//! 输出个数
	static void	OutSessionInfo(VOID);

	//##ModelId=41DDF2410119
	static CSession* CreateSession(DWORD dwLifeTime = 0, SESSION_TYPE st = ST_NORMAL_SESSION);

	//##ModelId=41DDF445000F
	static const CSession* QuerySession(const CGUID& SessionGuid);


	//##ModelId=41DDF3B1036B
	static VOID AI();

	//##ModelId=41DE00BF0399
	static VOID Release();

	static VOID GarbageCollect_Session(const CGUID& guid);

private:
	//##ModelId=41DDF142029F
	static LONG s_lSessionID;

	//##ModelId=41DDF1B40196
	static hash_map<CGUID, CSession*, hash_guid_compare>  s_mSessions;
protected:
	friend class CSession;
};
#endif /* _INC_CSESSIONFACTORY_41DDF0D60109_INCLUDED */
