#pragma once

#include "../../../../SCMPublic/def/PurviewDef.h"

class PurviewSetup
{
public:
	PurviewSetup(VOID);
	~PurviewSetup(VOID);

public:
	//!
	static		PurviewSetup&			GetInstance(VOID);

public:
	//!									获取配置
	BOOL								LoadSetup(const char *pXmlFilename);
	//!									通过用户类型得到权限
	const		tagGMTPurviewSetup*		GetPurviewByUserType(DWORD dwUserType);



private:
	//!									配置是否准备好
	BOOL								m_bIsReady;
	//!									一个用户类型所拥有的权限的列表
	map<DWORD, tagGMTPurviewSetup>		m_mapGMTPurviewSetup;

private:
	//!
	static		PurviewSetup			c_PurviewSetup;
};