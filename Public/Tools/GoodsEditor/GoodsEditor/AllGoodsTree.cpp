// AllGoodsTree.cpp : 实现文件
//

#include "stdafx.h"
#include "GoodsEditor.h"
#include "AllGoodsTree.h"
#include ".\allgoodstree.h"
#include "GoodAttrPage.h"
#include "ContainerPage.h"


// CAllGoodsTree
extern vector<tagGoodAttr> vecGoodAttr; 

IMPLEMENT_DYNCREATE(CAllGoodsTree, CFormView)

CAllGoodsTree::CAllGoodsTree()
	: CFormView(CAllGoodsTree::IDD)
{
	m_bNewItem=false;
	m_bCtrlInit=false;
}

CAllGoodsTree::~CAllGoodsTree()
{
}

void CAllGoodsTree::InitMyCtrls()
{
	//tree init
	TV_INSERTSTRUCT rootItem;
	rootItem.hParent=NULL;
	rootItem.hInsertAfter=TVI_FIRST;
	rootItem.item.pszText="所有物品";
	rootItem.item.mask=TVIF_TEXT| TVIF_PARAM;
	rootItem.item.lParam=0;
	m_hTreeRoot=m_cGoodsTree.InsertItem(&rootItem);
	
	vector<tagGoodAttr>::iterator iter;
	int i=0;
	for(iter=vecGoodAttr.begin();iter!=vecGoodAttr.end();iter++,i++)
	{
		TV_INSERTSTRUCT curItem;
		curItem.hParent=m_hTreeRoot;
		curItem.hInsertAfter=TVI_LAST;
		curItem.item.pszText=const_cast<char*>(iter->strOrigName.c_str());
		curItem.item.mask=TVIF_TEXT | TVIF_PARAM;
		curItem.item.lParam=iter->dwIndex;
		m_cGoodsTree.InsertItem(&curItem);
	}
	m_bCtrlInit=true;
}

void CAllGoodsTree::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ALLGOODSTREE, m_cGoodsTree);
	InitMyCtrls();
}

BEGIN_MESSAGE_MAP(CAllGoodsTree, CFormView)
	ON_NOTIFY(TVN_SELCHANGED, IDC_ALLGOODSTREE, OnTvnSelchangedAllgoodstree)
	ON_NOTIFY(NM_DBLCLK, IDC_ALLGOODSTREE, OnNMDblclkAllgoodstree)
	ON_WM_SIZE()
	ON_NOTIFY(NM_RCLICK, IDC_ALLGOODSTREE, OnNMRclickAllgoodstree)
	ON_COMMAND(ID_32785, On32785)
END_MESSAGE_MAP()


// CAllGoodsTree 诊断

#ifdef _DEBUG
void CAllGoodsTree::AssertValid() const
{
	CFormView::AssertValid();
}

void CAllGoodsTree::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG


// CAllGoodsTree 消息处理程序

void CAllGoodsTree::OnTvnSelchangedAllgoodstree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	HTREEITEM selectedItem=m_cGoodsTree.GetSelectedItem();
	if(selectedItem==m_hTreeRoot)
	{
		CSplitterWnd *pSplitter=(CSplitterWnd*)GetParent();
		CContainerPage *pContainer=(CContainerPage*)pSplitter->GetPane(0,1);		
		pContainer->m_wndGoodAttrPage.ShowWindow(SW_HIDE);
		pContainer->m_wndAllGoodsPage.ShowWindow(SW_NORMAL);
		pContainer->m_wndGoodAttrPage.SetEditFlag(false);
	}	
	else
	{
		CSplitterWnd *pSplitter=(CSplitterWnd*)GetParent();
		CContainerPage *pContainer=(CContainerPage*)pSplitter->GetPane(0,1);
		CAllGoodsTree *pGoodsTree=(CAllGoodsTree*)pSplitter->GetPane(0,0);
		
		int nSelected=0;
		vector<tagGoodAttr>::iterator iter;
		for(iter=vecGoodAttr.begin();iter!=vecGoodAttr.end();iter++,nSelected++)
		{			
			if(m_cGoodsTree.GetItemText(selectedItem).Compare(CString(iter->strOrigName.c_str()))==0)
				break;
		}
		pContainer->m_wndGoodAttrPage.SetEditFlag(false);
		pContainer->m_wndGoodAttrPage.ClearCtrls();
		pContainer->m_wndGoodAttrPage.FillCtrls(nSelected);		
		pContainer->m_wndAllGoodsPage.ShowWindow(SW_HIDE);
		pContainer->m_wndGoodAttrPage.ShowWindow(SW_NORMAL);		
		pContainer->m_wndGoodAttrPage.SetEditFlag(true);
	}
}

void CAllGoodsTree::OnNMDblclkAllgoodstree(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;	
}

void CAllGoodsTree::OnSize(UINT nType, int cx, int cy)
{	
	// TODO: 在此处添加消息处理程序代码
	if(m_bCtrlInit)
	    m_cGoodsTree.MoveWindow(0,0,cx,cy);
	
	CFormView::OnSize(nType, cx, cy);		
}

void CAllGoodsTree::OnNMRclickAllgoodstree(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	NMTREEVIEW* pNMTreeView = (NMTREEVIEW*)(pNMHDR);
	
/*
	CPoint pt(pNMTreeView->ptDrag.x,pNMTreeView->ptDrag.y);
	HTREEITEM item=m_cGoodsTree.HitTest(pt);
	if(item)
	{
		HTREEITEM root=m_cGoodsTree.GetRootItem();
		if(item==root)
		{
			int i=0;
		}
	}	*/
	CMenu menu;
	BOOL bVal=menu.LoadMenu(IDR_MENU1);	
	CMenu* pMenu=menu.GetSubMenu(0);
	CPoint pt;
	GetCursorPos(&pt);
	pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x,
		pt.y, this);

}

int CALLBACK CAllGoodsTree::CompareFunc(LPARAM lParam1,LPARAM lParam2,LPARAM lParamSort)
{
	CTreeCtrl *pTreeCtrl=(CTreeCtrl*)lParamSort;	
	CString strVal1=pTreeCtrl->GetItemText((HTREEITEM) lParam1);
	CString strVal2=pTreeCtrl->GetItemText((HTREEITEM) lParam2);
	return strcmp(strVal1, strVal2);
}

void CAllGoodsTree::On32785()
{
	// TODO: 在此添加命令处理程序代码
	/*TVSORTCB tvs;
	
	tvs.hParent = m_cGoodsTree.GetRootItem();
	tvs.lpfnCompare = CAllGoodsTree::CompareFunc;
	tvs.lParam = (LPARAM)( &m_cGoodsTree);
	m_cGoodsTree.SortChildrenCB(&tvs);*/
	m_cGoodsTree.SortChildren(m_cGoodsTree.GetRootItem());
	
}
