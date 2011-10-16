/**
============================================================
*  Copyright (c) 2009
*  All rights reserved
*  文件名称：AddNameDlg.h
*  功能：为TestScript的添加case功能提供名字输入对话框
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
#include "afxwin.h"

class CAddNameDlg : public CDialog
{
	DECLARE_DYNAMIC(CAddNameDlg)

public:
	CAddNameDlg(CWnd* pParent = NULL); 
	virtual ~CAddNameDlg();

	enum { IDD = IDD_DIALOG_AddName };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
private:
	CStatic m_ctlAddTitle;
	CEdit m_ctlAddName;

	CString m_strAddName;
public:
	afx_msg void OnBnClickedOk();
	CString GetAddName()	{ return m_strAddName;}
};
