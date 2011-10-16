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

#include "stdafx.h"
#include "GameTest.h"
#include "AddNameDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CAddNameDlg, CDialog)

CAddNameDlg::CAddNameDlg(CWnd* pParent)
	: CDialog(CAddNameDlg::IDD, pParent)
{

}

CAddNameDlg::~CAddNameDlg()
{
}

void CAddNameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_AddTitle, m_ctlAddTitle);
	DDX_Control(pDX, IDC_AddName, m_ctlAddName);
}

BEGIN_MESSAGE_MAP(CAddNameDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CAddNameDlg::OnBnClickedOk)
END_MESSAGE_MAP()

void CAddNameDlg::OnBnClickedOk()
{
	UpdateData(TRUE);
	m_ctlAddName.GetWindowText(m_strAddName);
	OnOK();
}
