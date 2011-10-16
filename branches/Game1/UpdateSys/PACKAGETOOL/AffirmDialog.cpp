// AffirmDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "PackageTool.h"
#include "AffirmDialog.h"
#include ".\affirmdialog.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CAffirmDialog 对话框

IMPLEMENT_DYNCREATE(CAffirmDialog, CDHtmlDialog)

CAffirmDialog::CAffirmDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CAffirmDialog::IDD,pParent)
	, m_strText(_T(""))
	, m_bAlwaysCheck(FALSE)
	,m_iResult(-1)
{
}

CAffirmDialog::~CAffirmDialog()
{
}

void CAffirmDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_TEXT, m_strText);
	DDX_Check(pDX, IDC_CHECK_ALWAY, m_bAlwaysCheck);
}

BOOL CAffirmDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

BEGIN_MESSAGE_MAP(CAffirmDialog, CDialog)
//	ON_BN_CLICKED(IDOK, OnBnClickedOk)
ON_BN_CLICKED(ID_OK, OnBnClickedOk)
ON_BN_CLICKED(ID_NO, OnBnClickedNo)
END_MESSAGE_MAP()


// CAffirmDialog 消息处理程序
//
//HRESULT CAffirmDialog::OnButtonOK(IHTMLElement* /*pElement*/)
//{
//	OnOK();
//	return S_OK;  // 除非将焦点设置到控件，否则返回 TRUE
//}
//
//HRESULT CAffirmDialog::OnButtonCancel(IHTMLElement* /*pElement*/)
//{
//	OnCancel();
//	return S_OK;  // 除非将焦点设置到控件，否则返回 TRUE
//}

//void CAffirmDialog::OnBnClickedOk()
//{
//	// TODO: 在此添加控件通知处理程序代码
//}

void CAffirmDialog::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类
	CDialog::OnCancel();
}

void CAffirmDialog::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	m_iResult = 1;
	OnOK();
}

void CAffirmDialog::OnBnClickedNo()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	m_iResult = 0;
	OnOK();
}
