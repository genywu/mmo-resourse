// DlgCollection.cpp : 实现文件
//

#include "stdafx.h"

#include "DlgCollection.h"


#include "UniEditorDlg.h"

// CDlgCollection 对话框

IMPLEMENT_DYNAMIC(CDlgCollection, CDialog)

CDlgCollection::CDlgCollection(CWnd* pParent /*=NULL*/)
	: CDlgEdit(CDlgCollection::IDD, pParent)
	, m_ncollleft(0)
{
	m_pCollection = NULL;
	m_ptreeRegion = NULL;
	m_pWndParent = NULL;
	m_bDlgEdit = FALSE;
	m_bError = FALSE;
}

CDlgCollection::~CDlgCollection()
{
}

void CDlgCollection::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_COLL_COOR_LEFT, m_ncollleft);
}

BOOL CDlgCollection::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_pWndParent = GetParent();
	m_pCollection=NULL;
	m_ptreeRegion = ((CUniEditorDlg *)m_pWndParent)->GetTreeCtrl();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
BEGIN_MESSAGE_MAP(CDlgCollection, CDialog)
	ON_EN_CHANGE(IDC_COLL_NO, &CDlgCollection::OnEnChangeCollNo)
	ON_EN_CHANGE(IDC_COLL_COOR_LEFT, &CDlgCollection::OnEnChangeCollCoorLeft)
	ON_EN_CHANGE(IDC_COLL_COOR_TOP, &CDlgCollection::OnEnChangeCollCoorTop)
	ON_EN_CHANGE(IDC_COLL_COOR_RIGHT, &CDlgCollection::OnEnChangeCollCoorRight)
	ON_EN_CHANGE(IDC_COLL_COOR_BOTTOM, &CDlgCollection::OnEnChangeCollCoorBottom)
	ON_EN_CHANGE(IDC_COLL_QUANTITY, &CDlgCollection::OnEnChangeCollQuantity)
	ON_EN_CHANGE(IDC_COLL_TIME_BORN, &CDlgCollection::OnEnChangeCollTimeBorn)
	ON_EN_CHANGE(IDC_COLL_TIME_START, &CDlgCollection::OnEnChangeCollTimeStart)
	ON_EN_CHANGE(IDC_COLL_DIRECTION, &CDlgCollection::OnEnChangeCollDirection)
	ON_BN_CLICKED(IDC_CHECK_BORN, &CDlgCollection::OnBnClickedCheckBorn)
	ON_BN_CLICKED(IDC_CHECK_COLL_BORN, &CDlgCollection::OnBnClickedCheckCollBorn)
END_MESSAGE_MAP()


// CDlgCollection 消息处理程序
// 显示Collection批次信息
void CDlgCollection::SetDlg(CRegion *pRegion, CCollection *pCollection, BOOL bALL)
{
	m_pCollection = pCollection;
	SetDlgItemInt(IDC_COLL_COOR_LEFT, pCollection->GetCoordinate().left);
	SetDlgItemInt(IDC_COLL_COOR_TOP, pCollection->GetCoordinate().top);
	SetDlgItemInt(IDC_COLL_COOR_RIGHT, pCollection->GetCoordinate().right);
	SetDlgItemInt(IDC_COLL_COOR_BOTTOM, pCollection->GetCoordinate().bottom);
	m_bDlgEdit = TRUE;

	// 全部更新
	if(bALL)
	{
		m_hItem = m_ptreeRegion->GetItem();
		m_uOffset= m_ptreeRegion->GetOffset(m_hItem);
	//	pCollection->Monster2String(m_szModifyNode);
		m_pRegion = pRegion;
		int i =	pCollection->GetNo();

		
		SetDlgItemInt(IDC_COLL_NO, i);
		SetDlgItemInt(IDC_COLL_QUANTITY, pCollection->GetQuantity());
		SetDlgItemInt(IDC_COLL_TIME_BORN, pCollection->GetTimeBorn());
		SetDlgItemInt(IDC_COLL_TIME_START, pCollection->GetTimeStart());
		SetDlgItemInt(IDC_COLL_DIRECTION, pCollection->GetDirection());

		((CButton *)GetDlgItem(IDC_CHECK_COLL_BORN))->SetCheck(pCollection->GetSwitch());
		CDlgEdit::SetDlg(pRegion, pCollection);    
	}

	// 鼠标更新
	else
	{
		CUniEditorDlg::m_bUpdateCollection = TRUE;
		((CUniEditorDlg *)m_pWndParent)->UpdateFileList(1);
	}
}

void CDlgCollection::OnEnChangeCollNo()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CDlgEdit::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	unsigned uNo = GetDlgItemInt(IDC_COLL_NO);
	unsigned uInd = 0;

	if (0 == uNo || !m_pRegion->CheckCollectionNo(uNo, uInd))
	{
		m_pCollection->SetNo(uNo);
		// 更新树形控件
		CString str;
		str.Format(_T("%u"), uNo);
		m_ptreeRegion->SetItemText(m_ptreeRegion->GetItem(), str);

		if(!CUniEditorDlg::m_bSelChange)
		{
			CUniEditorDlg::m_bUpdateCollection = TRUE;
			((CUniEditorDlg *)m_pWndParent)->UpdateFileList(1);
		}
	}
}

void CDlgCollection::OnEnChangeCollCoorLeft()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CDlgEdit::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	if(m_bDlgEdit)
	{
		int i = GetDlgItemInt(IDC_COLL_COOR_LEFT);
		if(i>0 && i<=m_lRegionWidth)
		{
			//if(i <= m_pCollection->GetCoordinate().right)
			{
				m_pCollection->SetCoorLeft(i);
				::PostMessageA(GetParent()->GetSafeHwnd(), WM_PAINT, 0, 0);

				// 如果不是切换结点时改变,需要更新
				if(!CUniEditorDlg::m_bSelChange)
				{
					CUniEditorDlg::m_bUpdateCollection = TRUE;
					((CUniEditorDlg *)m_pWndParent)->UpdateFileList(1);
				}
			}
		}

		else
		{
			SetDlgItemInt(IDC_COLL_COOR_LEFT, m_pCollection->GetCoordinate().left);
		}
	}
}

void CDlgCollection::OnEnChangeCollCoorTop()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CDlgEdit::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	if(m_bDlgEdit)
	{
		int i = GetDlgItemInt(IDC_COLL_COOR_TOP);
		if(i>0 && i<=m_lRegionHeight)
		{
		//	if(i <= m_pCollection->GetCoordinate().bottom)
			{
				m_pCollection->SetCoorTop(i);
				static int a=1;
				char szNo[32]={0};
				sprintf(szNo,"%d\n",a++);
				TRACE(szNo);
				::PostMessageA(GetParent()->GetSafeHwnd(), WM_PAINT, 0, 0);

				// 如果不是切换结点时改变,需要更新
				if(!CUniEditorDlg::m_bSelChange)
				{
					CUniEditorDlg::m_bUpdateCollection = TRUE;
					((CUniEditorDlg *)m_pWndParent)->UpdateFileList(1);
				}
			}
		}

		else
		{
			SetDlgItemInt(IDC_COLL_COOR_TOP, m_pCollection->GetCoordinate().top);
		}
	}
}

void CDlgCollection::OnEnChangeCollCoorRight()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CDlgEdit::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	if(m_bDlgEdit)
	{
		int i = GetDlgItemInt(IDC_COLL_COOR_RIGHT);
		if(i>0 && i<=m_lRegionWidth)
		{
		//	if(i >= m_pCollection->GetCoordinate().left)
			{
				m_pCollection->SetCoorRight(i);
				::PostMessageA(GetParent()->GetSafeHwnd(), WM_PAINT, 0, 0);

				// 如果不是切换结点时改变,需要更新
				if(!CUniEditorDlg::m_bSelChange)
				{
					CUniEditorDlg::m_bUpdateCollection = TRUE;
					((CUniEditorDlg *)m_pWndParent)->UpdateFileList(1);
				}
			}
		}

		else
		{
			SetDlgItemInt(IDC_COLL_COOR_RIGHT, m_pCollection->GetCoordinate().right);
		}
	}
}

void CDlgCollection::OnEnChangeCollCoorBottom()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CDlgEdit::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	if(m_bDlgEdit)
	{
		int i = GetDlgItemInt(IDC_COLL_COOR_BOTTOM);
		if(i>0 && i<=m_lRegionHeight)
		{
		//	if(i >= m_pCollection->GetCoordinate().top)
			{
				m_pCollection->SetCoorBottom(i);
				::PostMessageA(GetParent()->GetSafeHwnd(), WM_PAINT, 0, 0);

				// 如果不是切换结点时改变,需要更新
				if(!CUniEditorDlg::m_bSelChange)
				{
					CUniEditorDlg::m_bUpdateCollection = TRUE;
					((CUniEditorDlg *)m_pWndParent)->UpdateFileList(1);
				}
			}
		}

		else
		{
			SetDlgItemInt(IDC_COLL_COOR_BOTTOM, m_pCollection->GetCoordinate().bottom);
		}
	}
}

void CDlgCollection::OnEnChangeCollQuantity()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CDlgEdit::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	m_pCollection->SetQuantity(GetDlgItemInt(IDC_COLL_QUANTITY));
	if(!CUniEditorDlg::m_bSelChange)
	{
		CUniEditorDlg::m_bUpdateCollection = TRUE;
		((CUniEditorDlg *)m_pWndParent)->UpdateFileList(1);
	}
}

void CDlgCollection::OnEnChangeCollTimeBorn()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CDlgEdit::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	m_pCollection->SetTimeBorn(GetDlgItemInt(IDC_COLL_TIME_BORN));
	if(!CUniEditorDlg::m_bSelChange)
	{
		CUniEditorDlg::m_bUpdateCollection = TRUE;
		((CUniEditorDlg *)m_pWndParent)->UpdateFileList(1);
	}
}


void CDlgCollection::OnEnChangeCollTimeStart()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CDlgEdit::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	m_pCollection->SetTimeStart(GetDlgItemInt(IDC_COLL_TIME_START));
	if(!CUniEditorDlg::m_bSelChange)
	{
		CUniEditorDlg::m_bUpdateCollection = TRUE;
		((CUniEditorDlg *)m_pWndParent)->UpdateFileList(1);
	}
}

void CDlgCollection::OnEnChangeCollDirection()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CDlgEdit::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	m_pCollection->SetDirection(GetDlgItemInt(IDC_COLL_DIRECTION));
	if(!CUniEditorDlg::m_bSelChange)
	{
		CUniEditorDlg::m_bUpdateCollection = TRUE;
		((CUniEditorDlg *)m_pWndParent)->UpdateFileList(1);
	}
}
	//复选框
void CDlgCollection::OnBnClickedCheckBorn()
{
	// TODO: 在此添加控件通知处理程序代码
	//CButton *pbtn = (CButton *)GetDlgItem(IDC_CHECK_COLL_BORN);
	//if (m_pCollection->GetSwitch())
	//{
	//	pbtn->SetCheck(BST_UNCHECKED);
	//	m_pCollection->SetSwitch(false);
	//}
	//else
	//{
	//	pbtn->SetCheck(BST_CHECKED);
	//	m_pCollection->SetSwitch();
	//}

	//if(!CUniEditorDlg::m_bSelChange)
	//{
	//	CUniEditorDlg::m_bUpdateCollection = TRUE;
	//	((CUniEditorDlg *)m_pWndParent)->UpdateFileList(TRUE);
	//}
}

void CDlgCollection::OnBnClickedCheckCollBorn()
{
	// TODO: 在此添加控件通知处理程序代码
	CButton *pbtn = (CButton *)GetDlgItem(IDC_CHECK_COLL_BORN);
	if (m_pCollection->GetSwitch())
	{
		pbtn->SetCheck(BST_UNCHECKED);
		m_pCollection->SetSwitch(false);
	}
	else
	{
		pbtn->SetCheck(BST_CHECKED);
		m_pCollection->SetSwitch();
	}

	if(!CUniEditorDlg::m_bSelChange)
	{
		CUniEditorDlg::m_bUpdateCollection = TRUE;
		((CUniEditorDlg *)m_pWndParent)->UpdateFileList(1);
	}
}
