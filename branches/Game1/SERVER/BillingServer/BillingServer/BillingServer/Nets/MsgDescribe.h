#ifndef _MSG_DESCRIBUTE
#include <map>
#include "BaseMessage.h"

class CMsgDescribe
{
private:
	map<DWORD,string>								m_mapMsg;

	CMsgDescribe									();
	~CMsgDescribe									();

public:
	//##设置全局指针
	static CMsgDescribe*	g_pMsgDesc;

	static CMsgDescribe*	GetInstance				();
	static VOID				FinalRelease			();

	const CHAR*				GetDesc					( DWORD dwID );
};

#endif