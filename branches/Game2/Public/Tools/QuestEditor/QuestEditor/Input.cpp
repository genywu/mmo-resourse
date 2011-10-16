// Input.cpp : 实现文件
//
#include "stdafx.h"
#include "QuestEditor.h"
#include "Input.h"
#include "CFileSave.h"
#include <algorithm>
// CInput 对话框
IMPLEMENT_DYNAMIC(CInput, CDialog)
CInput::CInput(CWnd* pParent /*=NULL*/)
	: CDialog(CInput::IDD, pParent)
	, m_nInPutRoleID(0)
{
}
CInput::~CInput()
{
}
void CInput::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_nInPutRoleID);
	DDV_MinMaxInt(pDX, m_nInPutRoleID, 0, 999999);
}
BEGIN_MESSAGE_MAP(CInput, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CInput::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON4, &CInput::OnBnClickedButton4)
    ON_BN_CLICKED(IDOK, &CInput::OnBnClickedOk)
END_MESSAGE_MAP()

//确定控件的响应
void CInput::OnBnClickedButton1()
{
	UpdateData(true);
	if (UpdateData(true))
	{
		SendMessage(WM_CLOSE);
	}		
}
//取消控件的响应
void CInput::OnBnClickedButton4()
{
	SendMessage(WM_CLOSE);
}

void CInput::OnBnClickedOk()
{
    // TODO: 在此添加控件通知处理程序代码
    OnOK();
}
