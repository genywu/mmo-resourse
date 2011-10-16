// CaseNameDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "GameTest.h"
#include "CaseNameDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CCaseNameDlg 对话框

IMPLEMENT_DYNAMIC(CCaseNameDlg, CDialog)
CCaseNameDlg::CCaseNameDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCaseNameDlg::IDD, pParent)
	, m_strCaseName(_T(""))
	, m_strTitle(_T(""))
	, m_Name(_T(""))
{
}

CCaseNameDlg::~CCaseNameDlg()
{
}

void CCaseNameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_CaseName, m_strCaseName);
	DDX_Text(pDX, IDC_STATIC_Title, m_strTitle);
	DDX_Text(pDX, IDC_STATIC_Name, m_Name);
}


BEGIN_MESSAGE_MAP(CCaseNameDlg, CDialog)
END_MESSAGE_MAP()


// CCaseNameDlg 消息处理程序
