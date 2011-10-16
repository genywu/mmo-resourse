// DlgEdit.cpp : 实现文件
//

#include "stdafx.h"
#include "UniEditor.h"
#include "DlgEdit.h"


// CDlgEdit 对话框

IMPLEMENT_DYNAMIC(CDlgEdit, CDialog)

CDlgEdit::CDlgEdit(UINT nIDTemplate, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgEdit::IDD, pParent)
{
	m_lRegionWidth = 0;
	m_lRegionHeight = 0;
}

CDlgEdit::~CDlgEdit()
{
}

void CDlgEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgEdit, CDialog)
END_MESSAGE_MAP()

// 显示对话框信息
void CDlgEdit::SetDlg(CRegion *pRegion, void *pmonster, BOOL bALL)
{
	m_szRegionName = pRegion->GetRegionName();
 	m_lRegionWidth = pRegion->Width();
	m_lRegionHeight = pRegion->Height();
}

// CDlgEdit 消息处理程序
