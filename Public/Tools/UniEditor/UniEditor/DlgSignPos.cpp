// DlgSignPos.cpp : 实现文件
//

#include "stdafx.h"
#include "UniEditorDlg.h"
#include "DlgSignPos.h"


// CDlgSignPos 对话框

IMPLEMENT_DYNAMIC(CDlgSignPos, CDialog)

CDlgSignPos::CDlgSignPos(CWnd* pParent /*=NULL*/)
	: CDlgEdit(CDlgSignPos::IDD, pParent)
{

}

CDlgSignPos::~CDlgSignPos()
{
}

void CDlgSignPos::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgSignPos, CDialog)
	ON_EN_CHANGE(IDC_SIGNPOS_NO, &CDlgSignPos::OnEnChangeSignposNo)
	ON_EN_CHANGE(IDC_SIGNPOS_X, &CDlgSignPos::OnEnChangeSignposX)
	ON_EN_CHANGE(IDC_SIGNPOS_Y, &CDlgSignPos::OnEnChangeSignposY)
END_MESSAGE_MAP()

BOOL CDlgSignPos::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_pWndParent = GetParent();
	m_ptreeRegion = ((CUniEditorDlg *)m_pWndParent)->GetTreeCtrl();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
// CDlgSignPos 消息处理程序
void CDlgSignPos::SetDlg(CRegion *pRegion,CSignPos *pSignPos, BOOL bALL)
{
	m_pSignPos=pSignPos;
	SetDlgItemInt(IDC_SIGNPOS_NO,pSignPos->GetNo());
	SetDlgItemInt(IDC_SIGNPOS_X,pSignPos->GetxPos());
	SetDlgItemInt(IDC_SIGNPOS_Y,pSignPos->GetyPos());
	// 全部更新
	if(bALL)
	{
		m_hItem = m_ptreeRegion->GetItem();
		m_uOffset= m_ptreeRegion->GetOffset(m_hItem);
		//pmonster->Monster2String(m_szModifyNode);
		m_pRegion = pRegion;
		SetDlgItemInt(IDC_SIGNPOS_NO,pSignPos->GetNo());
		SetDlgItemInt(IDC_SIGNPOS_X,pSignPos->GetxPos());
		SetDlgItemInt(IDC_SIGNPOS_Y,pSignPos->GetyPos());
		CDlgEdit::SetDlg(pRegion, pSignPos);    
	}

	// 鼠标更新
	else
	{
		CUniEditorDlg::m_bUpdateSignPos = TRUE;
		((CUniEditorDlg *)m_pWndParent)->UpdateFileList(2);
	}
}
void CDlgSignPos::OnEnChangeSignposNo()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CDlgEdit::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	m_pSignPos->SetNo(GetDlgItemInt(IDC_SIGNPOS_NO));
	if(!CUniEditorDlg::m_bSelChange)
	{
		CUniEditorDlg::m_bUpdateSignPos = TRUE;
		((CUniEditorDlg *)m_pWndParent)->UpdateFileList(2);
	}
	
}

void CDlgSignPos::OnEnChangeSignposX()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CDlgEdit::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	m_pSignPos->SetxPos(GetDlgItemInt(IDC_SIGNPOS_X));
	::PostMessageA(GetParent()->GetSafeHwnd(), WM_PAINT, 0, 0);
	if(!CUniEditorDlg::m_bSelChange)
	{
		CUniEditorDlg::m_bUpdateSignPos = TRUE;
		((CUniEditorDlg *)m_pWndParent)->UpdateFileList(2);
	}
}

void CDlgSignPos::OnEnChangeSignposY()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CDlgEdit::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	m_pSignPos->SetyPos(GetDlgItemInt(IDC_SIGNPOS_Y));
	::PostMessageA(GetParent()->GetSafeHwnd(), WM_PAINT, 0, 0);
	if(!CUniEditorDlg::m_bSelChange)
	{
		CUniEditorDlg::m_bUpdateSignPos = TRUE;
		((CUniEditorDlg *)m_pWndParent)->UpdateFileList(2);
	}
}
