#pragma once
#include "../../GMTPublic/def/InfoDef.h"


class InfoManage
{
public:
	InfoManage(VOID);
	~InfoManage(VOID);

public:
	//!
	static	InfoManage&		GetInstance(VOID);

public:
	//!						初始化对象
	BOOL					Init(VOID);
	//!						释放对象
	VOID					Release(VOID);


private:
	//!						得到一个类型的父类型
	DWORD					GetParentType(DWORD dwInfoType);

private:
	//!						从属对应表
	map<DWORD, DWORD>		m_mapHypotaxisTable;

private:
	static	InfoManage		c_InfoManage;
};