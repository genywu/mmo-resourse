

/*
*	file:		RsLinkmanManage.h
*	Brief:		好友系统数据库操作管理
*	detail:		
*				
*				
*	Authtor:	张科智
*	Datae:		2008-02-25
*/

#pragma once

#include "..\..\..\dbaccess\worlddb\RsLinkman.h"

class DbLinkmanManage
{
public:
	DbLinkmanManage(void);
	~DbLinkmanManage(void);

	//!									得到唯一实例
	static		DbLinkmanManage&		GetDbLinkmanManage				(void);

public:

	//!									发送一个玩家的联系人数据到WS
	void								InitLinkmanDataToWs				(const CGUID& PlayerGuid);
	//!									响应WS的数据写入消息
	void								OnLinkmanWriteData				(CMessage *pMsg);


protected:
private:
	vector<tagLinkman_DB>	m_vData;
	
};