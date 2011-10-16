/*
=========================================================================
备注：暂未使用，界面取消。2010.10.28  BY:LGR
=========================================================================
*/

/**
============================================================
*  Copyright (c) 2009
*  All rights reserved
*  文件名称：LinkManDlg.h
*  功能：好友测试对话框
*  当前版本：1.1.0
*  作者：
*  完成日期：
*  取代版本：1.0.0
*  原作者：MartySa
*  完成日期： 2009.4.24
*
============================================================
*/

#pragma once
#include "afxwin.h"

class CLinkManDlg : public CDialog
{
	DECLARE_DYNAMIC(CLinkManDlg)

public:
	CLinkManDlg(CWnd* pParent = NULL); 
	virtual ~CLinkManDlg();
	
	virtual BOOL OnInitDialog();

	enum { IDD = IDD_PROPPAGE_LINKMAN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()

public:
	//初始化场景ID
	long m_lRegionID;
	long m_lPosX;
	long m_lPosY;
	long m_lStartCdKey;
	long m_lEndCdKey;
	long m_lSendMsgTimer;
	long m_lSeverSub;	
	long m_lMoveRrob;
	long m_lDelProb;
	long m_lExitProb;
	
	CComboBox m_comboServerIP;
	
	bool m_isCanSendAdd;
	bool m_isCanResponseAdd;
	bool m_isCanMoveLink;
	bool m_isCanDelLink;
	bool m_isCanExit;
	
	//是否盛大登陆方式
	bool m_isCanPtLogin;		

	afx_msg void OnBnClickedCheckLinkmanSendadd();
	afx_msg void OnBnClickedCheckLinkmanResponseadd();
	afx_msg void OnBnClickedCheckLinkmanSendmove();
	afx_msg void OnBnClickedCheckLinkmanDel();
	afx_msg void OnBnClickedCheckLinkmanExitclient();
	afx_msg void OnBnClickedButtonLinkmanStart();
	//
	afx_msg void OnBnClickedCheckLinkmanPtLogin();

};
