/**
============================================================
*  Copyright (c) 2009
*  All rights reserved
*  文件名称：AllTestDlg.h
*  功能：在程序主对话框上提供CTab控件的对话框
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
#include "afxcmn.h"

#include "GameTest.h"
#include "SkillTest.h"
#include "MailDlg.h"
#include "LinkManDlg.h"
#include "GoodsTestDlg.h"
#include "ACAnswerDlg.h"

class CAllTestDlg : public CDialog
{
	DECLARE_DYNAMIC(CAllTestDlg)

public:
	CAllTestDlg(CWnd* pParent = NULL);
	virtual ~CAllTestDlg();

	enum { IDD = IDD_DIALOG_AllTest };

	struct tagPageUIInfo
	{
		char szText[64];
		CDialog* pDlg;
		long lFlag;
		tagPageUIInfo(void)
		{
			pDlg = NULL;
			lFlag = -1;
			ZeroMemory(szText,64);
		}
	};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
private:
	CTabCtrl m_tabTypePage;
	list<tagPageUIInfo> m_listPageUIInfo;

	//技能测试对话框
	CSkillTest m_dlgSkill;

	//邮件
	CMailDlg m_MailDlg;

	//好友
	CLinkManDlg m_LinkManDlg; 

	CGoodsTestDlg m_dlgGoodsTest;

	//反外挂
	CACAnswer m_ACADlg;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnTcnSelchangeTabTypepage(NMHDR *pNMHDR, LRESULT *pResult);
};
