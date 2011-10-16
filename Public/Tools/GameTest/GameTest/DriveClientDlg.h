/**
============================================================
*  Copyright (c) 2009
*  All rights reserved
*  文件名称：CDriveClientDlg.h
*  功能：主要用于管理操作DriveClient Dlg
*  当前版本：1.1.0
*  作者： MartySa
*  完成日期：
*  取代版本：1.0.0
*  原作者：
*  完成日期：  2009.2.2
*
============================================================
*/

#pragma once

#include "afxcmn.h"
#include "../Utility/TimerDef.h"

class CDriveClient;

class CDriveClientDlg : public CDialog
{
	DECLARE_DYNAMIC(CDriveClientDlg)
private:
	
	//驱动客户端管理器
	CDriveClient	*m_DriveClientManager;

	//定时器
	Timer			*m_pTimer;

public:
	
	//设置DirverClient管理器
	void		    SetDriveClientManager(CDriveClient *driveclient)  { m_DriveClientManager = driveclient; }

	//设置定时器
	void			SetDriverClientDlgTimer(Timer *pPimer)			  {	m_pTimer = pPimer; }

public:
	
	CDriveClientDlg(CWnd* pParent = NULL);
	virtual ~CDriveClientDlg();

	enum { IDD = IDD_DriveClient };

protected:
	
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
public:
	
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnEnChangeEdit1();
	long m_lStartCDkey;
	long m_lEndCDkey;
	void _CreatDriveGameClient(long startcdkey, long endcdkey, unsigned uID);

//public:
//	struct tagOutputInfo
//	{
//		HANDLE hThrd;
//		HANDLE hEvent;
//		BOOL bExit;
//		tagOutputInfo(void)
//		{
//			memset(this,0,sizeof(tagOutputInfo));
//		}
//	};
//
//private:
//	tagOutputInfo m_stOutputInfo;
//
//public:
//	void SetThreadExit(BOOL b)	{ m_stOutputInfo.bExit = b; }
//	BOOL GetThreadExit()	{ return m_stOutputInfo.bExit; }
//	void SetEvent(BOOL b)	{ ::SetEvent(m_stOutputInfo.hEvent); }
//	tagOutputInfo *GetstOutPutInfo()  { return &m_stOutputInfo; }

};
