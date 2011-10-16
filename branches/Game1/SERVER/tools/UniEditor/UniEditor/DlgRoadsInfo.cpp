// DlgRoadsInfo.cpp : 实现文件
//

#include "stdafx.h"
#include "UniEditor.h"
#include "DlgRoadsInfo.h"
#include "UniEditorDlg.h"



// CDlgRoadsInfo 对话框

IMPLEMENT_DYNAMIC(CDlgRoadsInfo, CDialog)

CDlgRoadsInfo::CDlgRoadsInfo(CWnd* pParent /*=NULL*/)
	: CDlgEdit(CDlgRoadsInfo::IDD, pParent)
{
	m_bDlgEdit = FALSE;
}

CDlgRoadsInfo::~CDlgRoadsInfo()
{
}

void CDlgRoadsInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgRoadsInfo, CDialog)
	ON_EN_CHANGE(IDC_ROADSPOINTX, &CDlgRoadsInfo::OnEnChangeRoadspointx)
	ON_EN_CHANGE(IDC_ROADSPOINTY, &CDlgRoadsInfo::OnEnChangeRoadspointy)
	ON_EN_CHANGE(IDC_ROADSNO, &CDlgRoadsInfo::OnEnChangeRoadsno)
END_MESSAGE_MAP()
void CDlgRoadsInfo::SetDlg(CRegion *pRegion, int n,CRoads::tagRoadsPoint *roadspoint, BOOL bALL)
{
	if (pRegion == NULL || roadspoint == NULL || m_ptreeRegion == NULL)
		return;
	m_roadspoint = roadspoint;
	m_hItem = m_ptreeRegion->GetItem();
	m_uOffset= m_ptreeRegion->GetOffset(m_hItem);
	m_bDlgEdit = TRUE;
	
	if(bALL&&roadspoint!=NULL)
	{
		SetDlgItemInt(IDC_ROADSNO,n);
		GetDlgItem(IDC_ROADSNO)->EnableWindow(false);
		GetDlgItem(IDC_ROADSPOINTX)->EnableWindow(true);
		GetDlgItem(IDC_ROADSPOINTY)->EnableWindow(true);
		SetDlgItemInt(IDC_ROADSPOINTX, m_roadspoint->nx);
		SetDlgItemInt(IDC_ROADSPOINTY, m_roadspoint->ny);
	}
	else
	{
		CUniEditorDlg::m_bUpdateRoads = TRUE;
		((CUniEditorDlg *)m_pWndParent)->UpdateFileList(3);
	}
	CDlgEdit::SetDlg(pRegion,m_roadspoint);
}

void CDlgRoadsInfo::SetDlgNo(MAPROADS& mapRoads,int n)
{
	m_MapRoads = mapRoads;
	m_bDlgEdit = TRUE;
	m_RoadsID = n;
	SetDlgItemInt(IDC_ROADSNO,n);
	GetDlgItem(IDC_ROADSNO)->EnableWindow(true);
	GetDlgItem(IDC_ROADSPOINTX)->EnableWindow(false);
	GetDlgItem(IDC_ROADSPOINTY)->EnableWindow(false);
}

// CDlgRoadsInfo 消息处理程序
BOOL CDlgRoadsInfo::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_pWndParent = GetParent();
	m_ptreeRegion = ((CUniEditorDlg *)m_pWndParent)->GetTreeCtrl();
	return TRUE;
}
void CDlgRoadsInfo::OnEnChangeRoadspointx()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CDlgEdit::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	if(m_bDlgEdit)
	{
		int i = GetDlgItemInt(IDC_ROADSPOINTX);
		HTREEITEM hItem=m_ptreeRegion->GetSelectedItem();
		CString str=m_ptreeRegion->GetItemText(hItem);
		
		if(i>0 && i<=m_lRegionWidth)
		{
			
			m_roadspoint->nx=i;
			::PostMessageA(GetParent()->GetSafeHwnd(), WM_PAINT, 0, 0);

			// 如果不是切换结点时改变,需要更新
			if(!CUniEditorDlg::m_bSelChange)
			{
				CUniEditorDlg::m_bUpdateRoads = TRUE;
				((CUniEditorDlg *)m_pWndParent)->UpdateFileList(3);
			}
		}
	}
}

void CDlgRoadsInfo::OnEnChangeRoadspointy()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CDlgEdit::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	if(m_bDlgEdit)
	{
		int i = GetDlgItemInt(IDC_ROADSPOINTY);
		HTREEITEM hItem=m_ptreeRegion->GetSelectedItem();
		CString str=m_ptreeRegion->GetItemText(hItem);
		
		if(i>0 && i<=m_lRegionWidth)
		{
			m_roadspoint->ny=i;
			HTREEITEM hItem=m_ptreeRegion->GetSelectedItem();
			
			::PostMessageA(GetParent()->GetSafeHwnd(), WM_PAINT, 0, 0);

			// 如果不是切换结点时改变,需要更新
			if(!CUniEditorDlg::m_bSelChange)
			{
				CUniEditorDlg::m_bUpdateRoads = TRUE;
				((CUniEditorDlg *)m_pWndParent)->UpdateFileList(3);
			}
			
		}
	}
}

void CDlgRoadsInfo::OnEnChangeRoadsno()
{
	//if(m_bDlgEdit)
	//{
	//	if (m_roadspoint == NULL || m_ptreeRegion == NULL)
	//	{
	//		return;
	//	}
	//	int i = GetDlgItemInt(IDC_ROADSNO);
	//	HTREEITEM hItem=m_ptreeRegion->GetSelectedItem();
	//	m_RoadsID=i;
	//	
	//	char strVal[32];
	//	sprintf_s(strVal,"<%d,%d>",m_roadspoint->nx,m_roadspoint->ny);
	//	m_ptreeRegion->SetItemText(m_hItem,strVal);
	//	// 如果不是切换结点时改变,需要更新
	//	if(!CUniEditorDlg::m_bSelChange)
	//	{
	//		CUniEditorDlg::m_bUpdateRoads = TRUE;
	//		((CUniEditorDlg *)m_pWndParent)->UpdateFileList(3);
	//	}

	//	
	//}
}
