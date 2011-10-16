/*
*	file:		RsLeaveWord.h
*	Brief:		留言DB系统
*	detail:		
*				
*				
*	Authtor:	张科智
*	Datae:		2008-03-07
*/
#pragma  once


#include "..\public\GUID.h"
#include "..\nets\netdb\message.h"
#include "../myadobase.h"
#include "../public/LeaveWordDef.h"


class RsLeaveWord
	: public CMyAdoBase
{
public:
	//!							得到唯一实例
	static		RsLeaveWord&	GetRsLeaveWord(void);

public:
	//!							创建[玩家留言存储过程]
	BOOL						CreateInsertPro(tagLeaveWordSetup &LeaveWordSetup, _ConnectionPtr &cn);
	//!							删除[玩家留言存储过程]
	BOOL						DeleteInsertPro(_ConnectionPtr &cn);
	//!							执行一个插入操作，返回操作结果值
	LONG						InsertLeaveWord(tagLeaveWord_W2DB &LeaveWord_W2DB, const char *pWord, _ConnectionPtr &cn);
	//!							取出一个玩家的留言到数组
	BOOL						PopLeaveWord_ToByteArray(const CGUID &PlayerGuid, vector<BYTE> &vData, _ConnectionPtr &cn);
	//!							清理过时的留言
	void						ClearLeaveWord(LONG lTime, _ConnectionPtr &cn);

private:

};
