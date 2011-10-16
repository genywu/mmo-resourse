// SetupServer.cpp : 实现文件
//

#include "stdafx.h"
#include "urlfile.h"
#include "SetupServer.h"
#include "SetupDialog.h"


// SetupServer 对话框

IMPLEMENT_DYNAMIC(SetupServer, CDialog)

SetupServer::SetupServer(CWnd* pParent /*=NULL*/)
	: CDialog(SetupServer::IDD, pParent)
{
}

SetupServer::~SetupServer()
{
}

void SetupServer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_CCombo);
}


BEGIN_MESSAGE_MAP(SetupServer, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDOK, &SetupServer::OnBnClickedOk)
	//ON_CBN_SELCHANGE(IDC_COMBO1, &SetupServer::OnCbnSelchangeCombo1)
	ON_BN_CLICKED(IDCANCEL, &SetupServer::OnBnClickedCancel)
END_MESSAGE_MAP()


// SetupServer 消息处理程序
void SetupServer::OnBnClickedOk()
{
	int sel = m_CCombo.GetCurSel();
	CSetupDialog::m_Sever=m_CCombo.GetCurSel();
	OnOK();
}

BOOL SetupServer::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetPage();
	return 1;
}


void SetupServer::SetPage()
{
	m_CCombo.InsertString(-1,"电信");
	m_CCombo.InsertString(-1,"网通");
	m_CCombo.SetCurSel(CSetupDialog::m_Sever);
}

int SetupServer::GetCurSelect()
{
	return m_CCombo.GetCurSel();
}

void SetupServer::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	OnCancel();
}
