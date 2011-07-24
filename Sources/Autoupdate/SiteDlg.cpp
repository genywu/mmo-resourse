// SiteDlg.cpp : implementation file
//

#include "stdafx.h"
#include "autoupdate.h"
#include "SiteDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSiteDlg dialog


CSiteDlg::CSiteDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSiteDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSiteDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	site_list = NULL;
	nStart = 0;
}


void CSiteDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSiteDlg)
	DDX_Control(pDX, IDC_SITELIST, m_SiteList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSiteDlg, CDialog)
	//{{AFX_MSG_MAP(CSiteDlg)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_SITELIST, OnItemchangedSitelist)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSiteDlg message handlers

BOOL CSiteDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	ListView_SetExtendedListViewStyle(m_SiteList.m_hWnd, LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT);
	m_SiteList.InsertColumn(0, "", LVCFMT_LEFT, 200);
	char site_name[32];
	if(!site_list) return TRUE;
	for(int i = 0; i < site_list->GetSize(); i++) {
//		m_SiteList.InsertItem(0, "石家庄服务器");
//		m_SiteList.InsertItem(0, "石家庄服务器");
//		m_SiteList.InsertItem(0, (*site_list)[i]);
		sprintf(site_name, "自动升级站点%d", i + 1);
		m_SiteList.InsertItem(i, site_name);
	}
	m_SiteList.SetCheck(nStart, TRUE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSiteDlg::OnItemchangedSitelist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	/* If the event is not fired because of any change in individual items then the oldstate and 
 newstate members of pNMListView are not used.  So check for that.*/

 if (!pNMListView->uOldState && !pNMListView->uNewState)
  return;

 // m_listctl is the member variable of the dialog which is of type CListCtl.
 BOOL bChecked = ListView_GetCheckState(m_SiteList.m_hWnd, pNMListView->iItem);

 // if it's checked uncheck everything else.
 int nCount;

 if (bChecked)
  for(nCount=0; nCount<m_SiteList.GetItemCount();nCount++)
   if(nCount != pNMListView->iItem)
    m_SiteList.SetCheck(nCount, FALSE);
   else 
	   nStart = nCount;
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}
