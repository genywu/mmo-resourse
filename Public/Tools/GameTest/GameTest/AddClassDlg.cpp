// AddClassDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "GameTest.h"
#include "AddClassDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CAddClassDlg 对话框

IMPLEMENT_DYNAMIC(CAddClassDlg, CDialog)
CAddClassDlg::CAddClassDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddClassDlg::IDD, pParent)
	, m_strClassName(_T(""))
	, m_strOneCaseName(_T(""))
{
}

CAddClassDlg::~CAddClassDlg()
{
}

void CAddClassDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strClassName);
	DDX_Text(pDX, IDC_EDIT2, m_strOneCaseName);
}


BEGIN_MESSAGE_MAP(CAddClassDlg, CDialog)
END_MESSAGE_MAP()


// CAddClassDlg 消息处理程序
