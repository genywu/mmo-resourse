/**
=========================================================================
备注：暂未使用，界面取消。2010.10.28  BY:LGR
=========================================================================
*/

/**
============================================================
*  Copyright (c) 2009
*  All rights reserved
*  文件名称：MailDlg.h
*  功能：邮件测试对话框
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

class CMailDlg : public CDialog
{

	DECLARE_DYNAMIC(CMailDlg)

public:

	CMailDlg(CWnd* pParent = NULL);
	virtual ~CMailDlg();
	
	virtual BOOL OnInitDialog();

	enum { IDD = IDD_PROPPAGE_MAIL };
	CButton	m_CheckMailSend;		
	CButton m_CheckMailRepratName;
	CButton m_CheckMailRec;
	CButton	m_CheckMailReject;
	CButton m_CheckMailDelete;
	CButton m_CheckMailGetDoods;
	CButton m_CheckMailAgreePay;
	CButton m_CheckMailGetMoney;
	CButton m_CheckMailExitClient;
	CButton m_CheckMailSendMsgRepeat;
	
	//登陆方式控键
	CButton m_CheckMailPtLogin;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	DECLARE_MESSAGE_MAP()

public:

	// 起始场景Id
	long m_lStartRegionID;
	// 坐标X
	long m_lPosX;
	// 坐标Y
	long m_lPosY;
	// 起始帐号
	long m_lStartCdKey;
	// 结束帐号
	long m_lEndCdKey;
	// 消息发送间隔
	long m_lMsgSendTimer;	
	// 服务器下拉列表控件
	CComboBox m_comboxServer;
	// 物品名
	CString m_cstrGoodsName;
	// 金钱
	long m_lSendMoney;
	// 邮件类型， -1为随机０－５发送
	long m_lMailType;
	// 物品数量
	long m_lGoodsNum;
	// 发送的接收者人名
	CString m_cstrSendName;
	
	// 重复发送次数
	long m_lSendRepeatNum;	
	// 退信几率
	long m_lRejectMailProb;
	// 上线线几率
	long m_lExitClientPro;	
	// 删信几率
	long m_lDeleteMailProb;
	// 服务器版本
	long m_lSeverSub;	
	// 重复发送一次消息
	long m_lSendMsgRepeatNum;
	
	//button for Lua val;
	bool m_isCanSendMail;			//是否能发信
	bool m_isCanRepeatSendName;		//是否重复发送给一个玩家
	bool m_isCanRecMail;			//是否能收信
	bool m_isCanRejectMail;			//是否退信
	bool m_isCanDeleteMail;			//是否删信
	bool m_isCanGetGoods;			//是否取物品
	bool m_isCanAgreePay;			//是否同意付款
	bool m_isCanGetMoney;			//是否取金钱
	bool m_isCanExitClient;			//是否上下线
	bool m_isCanSendMsgRepeat;		//是否重复发送一次消息
	bool m_isCanPtLogin;			//是否盛大的登陆方式

	afx_msg void OnBnClickedCheckMailSend();
	afx_msg void OnBnClickedCheckMailRepeatsendname();
	afx_msg void OnBnClickedCheckMailrec();
	afx_msg void OnBnClickedCheckMailReject();
	afx_msg void OnBnClickedCheckMailDelete();
	afx_msg void OnBnClickedCheckMailGetgoods();
	afx_msg void OnBnClickedCheckMailAgreedpay();
	afx_msg void OnBnClickedCheckMailGetmoney();
	afx_msg void OnBnClickedCheckMailExitclient();
	afx_msg void OnBnClickedButtonMailstart();
	afx_msg void OnBnClickedCheckMailRepeatsendmsg();

	//盛大登陆方式
	afx_msg void OnBnClickedCheckMailLogin();

};
