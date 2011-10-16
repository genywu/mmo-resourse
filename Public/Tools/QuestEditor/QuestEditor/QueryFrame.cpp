// QueryFrame.cpp : 实现文件
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "QueryFrame.h"


// CQueryFrame 对话框

IMPLEMENT_DYNAMIC(CQueryFrame, CDialog)

CQueryFrame::CQueryFrame(CWnd* pParent /*=NULL*/)
	: CDialog(CQueryFrame::IDD, pParent)
    , m_questname(_T(""))
{

}

CQueryFrame::~CQueryFrame()
{
}

void CQueryFrame::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT2, m_questname);
    DDX_Control(pDX, IDC_COMBO1, m_ComBox);
}


BEGIN_MESSAGE_MAP(CQueryFrame, CDialog)
    ON_BN_CLICKED(IDC_BUTTON1, &CQueryFrame::OnBnClickedButton1)
END_MESSAGE_MAP()


// CQueryFrame 消息处理程序

void CQueryFrame::OnBnClickedButton1()
{
    // TODO: 在此添加控件通知处理程序代码
    m_Sel = m_ComBox.GetCurSel();
    OnOK();
}
