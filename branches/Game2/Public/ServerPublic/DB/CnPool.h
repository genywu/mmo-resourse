


#include "MyAdoBase.h"

#pragma once


struct tagCnPtr
{
	_ConnectionPtr ptr;
};


class CnPool
{
public:
	CnPool(long lMaxCnNum);
	~CnPool(void);
public:
	void						Init(const string &strCn);
	void						Release(void);

	bool						GetCnPtr(tagCnPtr &cnPtr);
	bool						RepairCnPtr(tagCnPtr &cnPtr);
	void						ReturnCnPtr(tagCnPtr cnPtr);

public:
	const	static	tagCnPtr	InvalidPtr;

private:
	list<tagCnPtr>				m_listCn;
	string						m_strCn;
	long						m_Num;
	bool						m_bCanGet;
	bool						m_bAbleRun;
	const	long				m_lMaxCnNum;
	CRITICAL_SECTION			m_CriticalSection;
};