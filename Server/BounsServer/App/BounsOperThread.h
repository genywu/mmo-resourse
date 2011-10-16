#pragma once
#include "../../../Public/ServerPublic/db/CnPool.h"

class CBounsAdoBase;

class CBounsOperThread
{
public:
	CBounsOperThread(string strProvider,
					string strDataSource,
					string strInitialCatalog,
					string strUserID,
					string strPassword);
	virtual ~CBounsOperThread(void);

////////////////////////////////////////////////
	bool			OpenCn();
	_ConnectionPtr	GetConnPtr();
	CBounsAdoBase*	GetAdoBasePtr()		{ return m_pAdoBasePtr; }
	bool			ProcessData();
	static unsigned int __stdcall	OperFun(void* pArgument);

///////////////////////////////////////////////////////
// about thread
//////////////////////////////////////////////////////
	bool		Begin();
	void 		End();
	bool		GetRunningFlag()				{ return m_bThreadRunning; }
	void		SetRunningFalg(bool bRunFlag)	{ m_bThreadRunning = bRunFlag; }
	long		GetThreadID()					{ return m_lThreadID; }
	HANDLE&		GetQuitWaitHandle()				{ return m_hQuitWait; }
	HANDLE&		GetOperThreadHandle()			{ return m_hOperThread; }


private:
	CBounsAdoBase		*m_pAdoBasePtr;
	string				m_strInitialCatalog;

	HANDLE				m_hOperThread;
	HANDLE				m_hQuitWait;
	bool				m_bThreadRunning;
	unsigned int		m_lThreadID;
};
