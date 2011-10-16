/**
============================================================
*  Copyright (c) 2010
*  All rights reserved
*  文件名称：TestResult.h
*  功能：测试结果类
*  当前版本：1.0.0
*  作者：    刘贵荣
*  完成日期：2010.11.8
*  取代版本：---
*  原作者：	 刘贵荣
*  完成日期：2010.11.8
*
============================================================
*/

#pragma once

#include "stdafx.h"

class CTestResult : public CDialog
{
	DECLARE_DYNAMIC(CTestResult)

public:
	CTestResult(CWnd* pParent = NULL);
	virtual ~CTestResult();

	enum { IDD = IDD_DIALOG_TESTRESULT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	BOOL	OnInitDialog();
	HBRUSH  OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	void	OnTimer(UINT nIDEvent);
	void	OnCancel();


	DECLARE_MESSAGE_MAP()

public:
	void SetAndOutPutTestResultInfo();

private:
	long m_lStartTime;
	CTime m_tTime;
	bool m_bIsIniTime;
};




