
/*
*	file:		RsLinkmanManage.h
*	Brief:		好友系统管理
*	detail:		
*				
*				
*	Authtor:	张科智
*	Datae:		2008-02-21
*/
#pragma  once


#include "..\public\GUID.h"
#include "..\nets\netdb\message.h"
#include "../myadobase.h"
#include "../public/LinkmanDef.h"
#include <set>


class RsLinkmanManage
	: public CMyAdoBase
{
public:
	//!								得到唯一实例
	static		RsLinkmanManage&	GetRsLinkmanManage(void);
	
public:
	//!								在数据库中读取初始化数据
	void							LoadPlayerInitData_FromDB(const CGUID &PlayerGuid, _ConnectionPtr &cn, vector<tagLinkman_DB>& vData);
	//!								储存一条联系人记录
	BOOL							SaveLinkman(tagLinkman_DB &Linkman_DB, _ConnectionPtr &cn);
	//!								删除一条联系人记录
	BOOL							DeleteLinkman(tagLinkman_DB &Linkman_DB, _ConnectionPtr &cn);

private:
	//!								要获取的 AimGUID字段为输入ID的GroupID字段值
	set<LONG>						m_ExceptiveGroupID;

};