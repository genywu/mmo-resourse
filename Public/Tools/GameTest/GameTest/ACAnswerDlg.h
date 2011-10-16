/*
=========================================================================
备注：该功能已取消，界面取消。2010.10.28  BY:LGR
=========================================================================
*/

/**
============================================================
*  Copyright (c) 2009
*  All rights reserved
*  文件名称：ACAnswerDlg.h
*  功能：活力点测试对话框界面
*  当前版本：1.1.0
*  作者：
*  完成日期：
*  取代版本：1.0.0
*  原作者：
*  完成日期：
*
============================================================
*/

#pragma once

class CACAnswer : public CDialog
{
	DECLARE_DYNAMIC(CACAnswer)

public:
	CACAnswer(CWnd* pParent = NULL);
	virtual ~CACAnswer();

	enum { IDD = IDD_PROPPAGE_AC_ANSWER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX); 

	DECLARE_MESSAGE_MAP()
public:
	// 场景ID
	long m_lRegionID;
	long m_lPosX;
	long m_lPosY;
	long m_lStartCdKey;
	long m_lEndCdKey;
	long m_lSendMsgTimer;
	long m_lSeverSub;
	// 是否激活答题
	BOOL m_bIsCanAnswer;
	// 是否激活安全测试
	BOOL m_bIsCanSafeTest;
	// 是否复制整块消息
	BOOL m_bIsCopyMsg;
	// 是否在消息Send那复制
	BOOL m_bIsCopySnedMsg;
	// 是否上下线
	BOOL m_bIsExitClient;
	// 是否盛大方式登陆
	BOOL m_bIsCanPtLogin;	
	afx_msg void OnBnClickedButtonAccStart();
	afx_msg void OnBnClickedAccCheckAnswer();
	afx_msg void OnBnClickedAccCheckSafe();
	afx_msg void OnBnClickedAccCheckCopymsg();
	afx_msg void OnBnClickedAccCheckCopysend();
	afx_msg void OnBnClickedAccCheckExitclient();
	afx_msg void OnBnClickedCheckAcaPtLogin();
};
