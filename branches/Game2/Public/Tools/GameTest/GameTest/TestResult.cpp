/**
============================================================
*  Copyright (c) 2010
*  All rights reserved
*  文件名称：TestResult.cpp
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

#include "stdafx.h"
#include "GameTest.h"
#include "TestResult.h"
#include "TestReporter.h"
#include "GameTestDlg.h"

IMPLEMENT_DYNAMIC(CTestResult, CDialog)

CTestResult::CTestResult(CWnd* pParent /*=NULL*/)
: CDialog(CTestResult::IDD, pParent)
{
	m_lStartTime = 0;
	m_bIsIniTime = false;
	m_tTime = CTime::GetCurrentTime();
}

CTestResult::~CTestResult()
{
}

void CTestResult::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTestResult, CDialog)
	ON_WM_TIMER()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()



BOOL CTestResult::OnInitDialog()
{
	CFont *f = new CFont;
	f->CreateFont(50,0,0,0,FW_BOLD,TRUE,FALSE,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,OUT_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH | FF_SWISS, _T("Arial"));
	GetDlgItem(IDC_STATIC_TESTRS)->SetFont(f);
	GetDlgItem(IDC_STATIC_TESTRS)->SetWindowText("GT Test Reporter");
	DELETE_SAFE(f);

	SetTimer(1,60000,NULL);

	return TRUE;
}


/*
=========================================================================
函数功能:控件颜色重载  By: LGR
=========================================================================
*/
HBRUSH CTestResult::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	switch(pWnd->GetDlgCtrlID())
	{
		case
IDC_EDIT_TESTRT:
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(GREEN);
			HBRUSH b = CreateSolidBrush(WHITE);
			return (HBRUSH)b;
		}
	default:
		return CWnd::OnCtlColor(pDC,pWnd,nCtlColor);	
	}		
}


/*
=========================================================================
函数功能:OnTimer控制输出测试信息  By: LGR
=========================================================================
*/
void CTestResult::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == 1)
	{
		if(m_bIsIniTime)
		{
 			SetAndOutPutTestResultInfo();
		}
	}
	CDialog::OnTimer(nIDEvent);
}


void CTestResult::OnCancel()
{
	KillTimer(1);
	CDialog::OnCancel();
}

void CTestResult::SetAndOutPutTestResultInfo()
{
	CGameTestDlg* pDLG = (CGameTestDlg*)CTestReporter::GetInstance()->GetUIInstance();
	CString info(""),infoEx("");

	pDLG->GetTestScript()->GetDlgItemText(IDC_COMBO_ServerList,info);
	CTestReporter::SetConctIp(info.GetBuffer(info.GetLength()));

	pDLG->GetTestScript()->GetDlgItemText(IDC_StartCDKey,info);
	pDLG->GetTestScript()->GetDlgItemText(IDC_EndCDKey,infoEx);
	CTestReporter::SetConctTotlNum(atoi(infoEx)-atoi(info));

	pDLG->GetTestScript()->GetDlgItemText(IDC_COMBO_TestType,info);
	CTestReporter::SetTestTypeName(info.GetBuffer(info.GetLength()));

	pDLG->GetTestScript()->GetDlgItemText(IDC_COMBO_TDLG_PROJNAME,info);
	CTestReporter::SetProjName(info.GetBuffer(info.GetLength()));

	pDLG->GetTestScript()->GetDlgItemText(IDC_COMBO_TDLG_PROJNAME,info);

	if (!m_bIsIniTime)
	{
		m_tTime	= CTime::GetCurrentTime();
		m_bIsIniTime = true;
	}
	info.Format("%d年%d月%d日%d时%d分%d秒",m_tTime.GetYear(),m_tTime.GetMonth(),m_tTime.GetDay(),m_tTime.GetHour(),m_tTime.GetMinute(),m_tTime.GetSecond());
	CTestReporter::SetStartTestTime(info.GetBuffer(info.GetLength()));

	if (m_lStartTime == 0 )
		m_lStartTime = timeGetTime();
	info.Format("%d 分钟",(timeGetTime()-m_lStartTime)/60000);
	CTestReporter::SetEndTestTime(info.GetBuffer(info.GetLength()));

	pDLG->GetTestScript()->GetDlgItemText(IDC_StartCDKey,info);
	CTestReporter::SetStartCDKEY(info.GetBuffer(info.GetLength()));

	pDLG->GetTestScript()->GetDlgItemText(IDC_EndCDKey,info);
	CTestReporter::SetEndCDKEY(info.GetBuffer(info.GetLength()));

	CTestReporter::SetMaxConctNum(pDLG->GetTestInfo()->GetMaxConnect());

	CTestReporter::SetMinConctNum(pDLG->GetTestInfo()->GetMinConnect());



	/*CTestReporter::SetKickedByServerNum(long _arg)
	CTestReporter::SetPerHourConctNum(long _arg)
	CTestReporter::SetIsCanRun(bool _arg)
	CTestReporter::SetTotalRuntimem(long _arg)
	CTestReporter::SetMsgInterchangeNum(long _arg)
	CTestReporter::SetMaxConctNumSysTest(long _arg)
	CTestReporter::SetIsRadHeadRadDatat(bool _arg)
	CTestReporter::SetIsFixedHeadFixedData(bool _arg)
	CTestReporter::SetIsFixedHeadRadData(bool _arg)
	CTestReporter::SetIsRadHeadFixeData(bool _arg)
	CTestReporter::SetIsJumpLogicDrive(bool _arg)*/

	SetDlgItemText(IDC_EDIT_TESTRT,CTestReporter::GetTestResultInfo().c_str());
}



