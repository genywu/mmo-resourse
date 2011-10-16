// PackageToolView.cpp : CPackageToolView 类的实现
//

#include "stdafx.h"
#include "PackageTool.h"

#include "PackageToolDoc.h"
#include "PackageToolView.h"
#include "FolderSet.h"
#include "../../PUBLIC/Crc32Static.h"
#include "../../PUBLIC/ClientResource.h"
#include ".\packagetoolview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define ID_EDIT_FIELD   333
#define ID_TREE_LIST_HEADER 337
#define ID_TREE_LIST_CTRL   373
#define ID_TREE_LIST_SCROLLBAR   377

string NumToStr[2] = {"否","是"};

// CPackageToolView

IMPLEMENT_DYNCREATE(CPackageToolView, CView)

BEGIN_MESSAGE_MAP(CPackageToolView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_POPMENU_PackageWithCompress, OnPopmenuPackagewithcompress)
	ON_COMMAND(ID_POPMENU_Package, OnPopmenuPackage)
	ON_COMMAND(ID_POPMENU_Unpackage, OnPopmenuUnpackage)
	ON_COMMAND(ID_RESOUCE_PACKAGE, OnResoucePackage)
	//ON_COMMAND(ID_POPMENU_AddFOLDER, OnPopmenuAddfolder)
	//ON_COMMAND(ID_POPMENU_ADDFILE, OnPopmenuAddfile)
	//ON_COMMAND(ID_POPMENU_Delete, OnPopmenuDelete)
	ON_COMMAND(ID_POPMENU_Extract, OnPopmenuExtract)
	ON_COMMAND(ID_RESOUCE_REMAKEPACKAGE, OnResouceRemakepackage)
	ON_COMMAND(ID_RESOUCE_RELOADRESOURCE, OnResouceReloadresource)
	ON_COMMAND(ID_POPMENU_FILES_SET, OnPopmenuFilesSet)
	ON_COMMAND(ID_POPMENU_Reloadresource, OnPopmenuReloadresource)
	ON_COMMAND(ID_POPMENU_ReloadresourceEx, OnPopmenuReloadresourceex)
END_MESSAGE_MAP()

// CPackageToolView 构造/析构

CPackageToolView::CPackageToolView()
{
	// TODO: 在此处添加构造代码

}

CPackageToolView::~CPackageToolView()
{
}

BOOL CPackageToolView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改 CREATESTRUCT cs 来修改窗口类或
	// 样式

	return CView::PreCreateWindow(cs);
}

// CPackageToolView 绘制

void CPackageToolView::OnDraw(CDC* /*pDC*/)
{
	CPackageToolDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
}


// CPackageToolView 打印

BOOL CPackageToolView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CPackageToolView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 打印前添加额外的初始化
}

void CPackageToolView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 打印后添加清除过程
}


// CPackageToolView 诊断

#ifdef _DEBUG
void CPackageToolView::AssertValid() const
{
	CView::AssertValid();
}

void CPackageToolView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CPackageToolDoc* CPackageToolView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPackageToolDoc)));
	return (CPackageToolDoc*)m_pDocument;
}
#endif //_DEBUG


// CPackageToolView 消息处理程序


void CPackageToolView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();

	Initialize();	

	// create image list for tree
	m_cImageList.Create(IDB_TREE_PICTURES, 16, 10, RGB(255, 255, 255));
	m_tree.SetImageList(&m_cImageList, TVSIL_NORMAL);

	// create image list for check boxes assocaited with tree
	m_StatusImageList.Create(IDB_CHECKBOXSTATUS, 16, 2, RGB (255,255,255));	
	m_tree.SetImageList(&m_StatusImageList, TVSIL_STATE );

	CRect m_wndRect;
	GetClientRect(&m_wndRect);
	MoveWindow(0, 0, m_wndRect.Width(), m_wndRect.Height());


	// insert Columns in list for tree
	{
		m_tree.InsertColumn(0, "文件名", LVCFMT_LEFT, 200);
		m_tree.InsertColumn(1, "CRC32", LVCFMT_RIGHT, 100);
		m_tree.InsertColumn(2, "大小(BYTE)", LVCFMT_RIGHT, 100);
		m_tree.InsertColumn(3, "打包", LVCFMT_RIGHT, 180);
		m_tree.InsertColumn(4, "压缩", LVCFMT_RIGHT, 80);
		m_tree.InsertColumn(5, "更新", LVCFMT_RIGHT, 80);
	}

	m_tooltip.Create(this);
	m_tooltip.Activate(TRUE);
	m_tooltip.AddTool(GetDlgItem(ID_TREE_LIST_CTRL),"whatever");

	m_TLInitialized = TRUE;		

	m_tree.SetBackgroundColor(RGB(200,200,200));
	m_tree.SetGridLines(TRUE);
	m_tree.SetGridColor(RGB(23,43,100));
	//m_tree.SetGridStyle(BS_SOLID);
}


/////////////////////////////////////////////////////////////////////////////
// CNewTreeListCtrlView message handlers

LONG FAR PASCAL CPackageToolView::DummyWndProc(HWND h, UINT u, WPARAM w, LPARAM l)
{
	return ::DefWindowProc(h, u, w, l);
}

void CPackageToolView::Initialize()
{
	// creates all the objects in frame -
	// header, tree, horizontal scroll bar

	CRect m_wndRect;
	GetWindowRect(&m_wndRect);
	CRect m_headerRect;

	// create the header
	{
		m_headerRect.left = m_headerRect.top = -1;
		m_headerRect.right = m_wndRect.Width();

		m_tree.m_wndHeader.Create(WS_CHILD | WS_VISIBLE | HDS_BUTTONS | HDS_HORZ, m_headerRect, this, ID_TREE_LIST_HEADER);
	}

	CSize textSize;
	// set header's pos, dimensions and image list
	{
		CDC *pDC = m_tree.m_wndHeader.GetDC();
		pDC->SelectObject(&m_tree.m_headerFont);
		textSize = pDC->GetTextExtent("A");
		m_tree.m_wndHeader.ReleaseDC(pDC); 

		m_tree.m_wndHeader.SetWindowPos(&wndTop, -1, -1, m_headerRect.Width(), textSize.cy+4, SWP_SHOWWINDOW);

		m_tree.m_cImageList.Create(IDB_HEADER, 16, 10, 0);
		m_tree.m_wndHeader.SetImageList(&m_tree.m_cImageList);		
	}

	CRect m_treeRect;

	// create the tree itself
	{
		GetWindowRect(&m_wndRect);

		m_treeRect.left=0;
		m_treeRect.top = textSize.cy+4;
		m_treeRect.right = m_headerRect.Width()-5;
		m_treeRect.bottom = m_wndRect.Height()-GetSystemMetrics(SM_CYHSCROLL)-4;

		m_tree.Create(WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS,m_treeRect, this, ID_TREE_LIST_CTRL);
	}

	// finally, create the horizontal scroll bar
	{
		CRect m_scrollRect;
		m_scrollRect.left=0;
		m_scrollRect.top = m_treeRect.bottom;
		m_scrollRect.right = m_treeRect.Width()-GetSystemMetrics(SM_CXVSCROLL);
		m_scrollRect.bottom = m_wndRect.bottom;

		m_horScrollBar.Create(WS_CHILD | WS_VISIBLE | WS_DISABLED | SBS_HORZ | SBS_TOPALIGN, m_scrollRect, this, ID_TREE_LIST_SCROLLBAR);
	}

	SortTree(0, TRUE, m_tree.GetRootItem());	
}

BOOL CPackageToolView::VerticalScrollVisible()
{	
	int sMin, sMax;
	m_tree.GetScrollRange(SB_VERT, &sMin, &sMax);	
	return sMax!=0;
}

BOOL CPackageToolView::HorizontalScrollVisible()
{
	int sMin, sMax;
	m_horScrollBar.GetScrollRange(&sMin, &sMax);
	return sMax!=0;
}


const string& CPackageToolView::GetPackFormatSring(DWORD dwPackID)
{
	return GetDocument()->GetPackFormatSring(dwPackID);
}

const string& CPackageToolView::GetCompFormatSring(DWORD dwCompID)
{
	return GetDocument()->GetCompFormatSring(dwCompID);
}

int CPackageToolView::StretchWidth(int m_nWidth, int m_nMeasure)
{	
	return ((m_nWidth/m_nMeasure)+1)*m_nMeasure;//put the fixed for +1 in brackets f/0 error
}


void CPackageToolView::ResetScrollBar()
{
	// resetting the horizontal scroll bar

	int m_nTotalWidth=0, m_nPageWidth;

	CRect m_treeRect;
	m_tree.GetClientRect(&m_treeRect);

	CRect m_wndRect;
	GetClientRect(&m_wndRect);

	CRect m_headerRect;
	m_tree.m_wndHeader.GetClientRect(&m_headerRect);

	CRect m_barRect;
	m_horScrollBar.GetClientRect(m_barRect);

	m_nPageWidth = m_treeRect.Width();

	m_nTotalWidth = m_tree.GetColumnsWidth();

	if(m_nTotalWidth > m_nPageWidth)
	{
		// show the scroll bar and adjust it's size
		{
			m_horScrollBar.EnableWindow(TRUE);

			m_horScrollBar.ShowWindow(SW_SHOW);

			// the tree becomes smaller
			m_tree.SetWindowPos(&wndTop, 0, 0, m_wndRect.Width(), m_wndRect.Height()-m_barRect.Height()-m_headerRect.Height(), SWP_NOMOVE);

			if(!VerticalScrollVisible())
				// i.e. vertical scroll bar isn't visible
			{
				m_horScrollBar.SetWindowPos(&wndTop, 0, 0, m_wndRect.Width(), m_barRect.Height(), SWP_NOMOVE);
			}
			else
			{
				m_horScrollBar.SetWindowPos(&wndTop, 0, 0, m_wndRect.Width() - GetSystemMetrics(SM_CXVSCROLL), m_barRect.Height(), SWP_NOMOVE);
			}
		}

		m_horScrollBar.SetScrollRange(0, m_nTotalWidth-m_nPageWidth);

		// recalculate the offset
		{
			CRect m_wndHeaderRect;
			m_tree.m_wndHeader.GetWindowRect(&m_wndHeaderRect);
			ScreenToClient(&m_wndHeaderRect);

			m_tree.m_nOffset = m_wndHeaderRect.left;
			m_horScrollBar.SetScrollPos(-m_tree.m_nOffset);
		}
	}
	else
	{
		m_horScrollBar.EnableWindow(FALSE);

		// we no longer need it, so hide it!
		{
			m_horScrollBar.ShowWindow(SW_HIDE);

			m_tree.SetWindowPos(&wndTop, 0, 0, m_wndRect.Width(), m_wndRect.Height() - m_headerRect.Height(), SWP_NOMOVE);
			// the tree takes scroll's place
		}

		m_horScrollBar.SetScrollRange(0, 0);

		// set scroll offset to zero
		{
			m_tree.m_nOffset = 0;
			m_tree.Invalidate();
			CRect m_headerRect;
			m_tree.m_wndHeader.GetWindowRect(&m_headerRect);
			CRect m_wndRect;
			GetClientRect(&m_wndRect);
			m_tree.m_wndHeader.SetWindowPos(&wndTop, m_tree.m_nOffset, 0, max(StretchWidth(m_tree.GetColumnsWidth(),m_wndRect.Width()),m_wndRect.Width()), m_headerRect.Height(), SWP_SHOWWINDOW);
		}
	}
}

void CPackageToolView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CRect m_treeRect;
	m_tree.GetClientRect(&m_treeRect);

	// if horizontal scroll bar 
	if(pScrollBar == &m_horScrollBar)
	{
		int m_nCurPos = m_horScrollBar.GetScrollPos();
		int m_nPrevPos = m_nCurPos;
		// decide what to do for each diffrent scroll event
		switch(nSBCode)
		{
		case SB_LEFT:			m_nCurPos = 0;
			break;
		case SB_RIGHT:			m_nCurPos = m_horScrollBar.GetScrollLimit()-1;
			break;
		case SB_LINELEFT:		m_nCurPos = max(m_nCurPos-6, 0);
			break;
		case SB_LINERIGHT:		m_nCurPos = min(m_nCurPos+6, m_horScrollBar.GetScrollLimit()-1);
			break;
		case SB_PAGELEFT:		m_nCurPos = max(m_nCurPos-m_treeRect.Width(), 0);
			break;
		case SB_PAGERIGHT:		m_nCurPos = min(m_nCurPos+m_treeRect.Width(), m_horScrollBar.GetScrollLimit()-1);
			break;
		case SB_THUMBTRACK:
		case SB_THUMBPOSITION:  if(nPos==0)
									m_nCurPos = 0;
								else
									m_nCurPos = min(StretchWidth(nPos, 6), m_horScrollBar.GetScrollLimit()-1);
			break;
		}		

		m_horScrollBar.SetScrollPos(m_nCurPos);
		m_tree.m_nOffset = -m_nCurPos;

		// smoothly scroll the tree control
		{
			CRect m_scrollRect;
			m_tree.GetClientRect(&m_scrollRect);
			m_tree.ScrollWindow(m_nPrevPos - m_nCurPos, 0, &m_scrollRect, &m_scrollRect);
		}

		CRect m_headerRect;
		m_tree.m_wndHeader.GetWindowRect(&m_headerRect);
		CRect m_wndRect;
		GetClientRect(&m_wndRect);		

		m_tree.m_wndHeader.SetWindowPos(&wndTop, m_tree.m_nOffset, 0, max(StretchWidth(m_tree.GetColumnsWidth(),m_treeRect.Width()),m_wndRect.Width()), m_headerRect.Height(), SWP_SHOWWINDOW);
	}

	CView::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CPackageToolView::SortTree(int nCol, BOOL bAscending, HTREEITEM hParent)
{		
	HTREEITEM hChild=NULL;
	HTREEITEM hPa = hParent;

	if(hParent != NULL)
	{
		if(m_tree.ItemHasChildren(hPa))
		{
			m_tree.SortItems(nCol, bAscending, hPa);

			HTREEITEM ChildOfParent=NULL;
			ChildOfParent=m_tree.GetChildItem(hPa);
			while(ChildOfParent!=NULL)
			{						
				SortTree(nCol,bAscending,ChildOfParent);

				ChildOfParent = m_tree.GetNextSiblingItem(ChildOfParent);
			}
		}
	}
}

BOOL CPackageToolView::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	HD_NOTIFY *pHDN = (HD_NOTIFY*)lParam;	

	if((wParam == ID_TREE_LIST_HEADER) && (pHDN->hdr.code == HDN_ITEMCLICK))
	{
		int nCol = pHDN->iItem;

		BOOL bAscending = FALSE;

		if(m_tree.m_wndHeader.GetItemImage(nCol)==-1)
			bAscending = TRUE;
		else
			if(m_tree.m_wndHeader.GetItemImage(nCol)==1)
				bAscending = TRUE;

		for(int i=0;i<m_tree.GetColumnsNum();i++)
		{
			m_tree.m_wndHeader.SetItemImage(i, -1);
		}

		if(bAscending)
			m_tree.m_wndHeader.SetItemImage(nCol, 0);
		else
			m_tree.m_wndHeader.SetItemImage(nCol, 1);

		m_tree.SortItems(nCol, bAscending, NULL);
		SortTree(nCol, bAscending, m_tree.GetRootItem());

		m_tree.UpdateWindow();
	}
	else
		if((wParam == ID_TREE_LIST_HEADER) && (pHDN->hdr.code == HDN_ITEMCHANGED))
		{
			int m_nPrevColumnsWidth = m_tree.GetColumnsWidth();
			m_tree.RecalcColumnsWidth();
			ResetScrollBar();

			// in case we were at the scroll bar's end,
			// and some column's width was reduced,
			// update header's position (move to the right).
			CRect m_treeRect;
			m_tree.GetClientRect(&m_treeRect);

			CRect m_headerRect;
			m_tree.m_wndHeader.GetClientRect(&m_headerRect);

			if((m_nPrevColumnsWidth > m_tree.GetColumnsWidth()) &&
				(m_horScrollBar.GetScrollPos() == m_horScrollBar.GetScrollLimit()-1) &&
				(m_treeRect.Width() < m_tree.GetColumnsWidth()))
			{
				m_tree.m_nOffset = -m_tree.GetColumnsWidth()+m_treeRect.Width();
				m_tree.m_wndHeader.SetWindowPos(&wndTop, m_tree.m_nOffset, 0, 0, 0, SWP_NOSIZE);
			}

			m_tree.Invalidate();
		}
		else
			GetParent()->SendMessage(WM_NOTIFY, wParam, lParam);

	return CView::OnNotify(wParam, lParam, pResult);
}

void CPackageToolView::OnContextMenu(CWnd* pWnd, CPoint point) 
{	
	GetParent()->SendMessage(WM_CONTEXTMENU, (WPARAM)pWnd, MAKELPARAM(point.x, point.y) ); 
}

void CPackageToolView::OnSize(UINT nType, int cx, int cy) 
{
	SetScrollRange(SB_HORZ, 0, 0);
	SetScrollRange(SB_VERT, 0, 0);

	if(m_TLInitialized)
	{	
		// resize all the controls	
		CRect m_wndRect;
		GetClientRect(&m_wndRect);

		CRect m_headerRect;
		m_tree.m_wndHeader.GetClientRect(&m_headerRect);
		m_tree.m_wndHeader.SetWindowPos(&wndTop, 0, 0, m_wndRect.Width(), m_headerRect.Height(), SWP_NOMOVE);

		CRect m_scrollRect;
		m_horScrollBar.GetClientRect(&m_scrollRect);

		m_tree.SetWindowPos(&wndTop, 0, 0, m_wndRect.Width(), m_wndRect.Height() - m_scrollRect.Height(), SWP_NOMOVE);

		CRect m_treeRect;
		m_tree.GetClientRect(&m_treeRect);
		m_horScrollBar.MoveWindow(0, m_treeRect.bottom, m_wndRect.Width(), m_scrollRect.Height());

		if(m_tree.GetColumnsWidth() > m_treeRect.Width())
		{
			// show the horz scroll bar
			{
				CRect m_barRect;
				m_horScrollBar.GetClientRect(&m_barRect);			

				m_horScrollBar.EnableWindow(TRUE);

				m_horScrollBar.ShowWindow(SW_SHOW);

				// the tree becomes smaller
				m_tree.SetWindowPos(&wndTop, 0, 0, m_wndRect.Width(), m_wndRect.Height()-m_barRect.Height()-m_headerRect.Height(), SWP_NOMOVE);
				int tfdsfsd=m_wndRect.Width() - GetSystemMetrics(SM_CXVSCROLL);
				m_horScrollBar.SetWindowPos(&wndTop, 0, 0, m_wndRect.Width() - GetSystemMetrics(SM_CXVSCROLL), m_barRect.Height(), SWP_NOMOVE);

			}
			m_horScrollBar.SetScrollRange(0, m_tree.GetColumnsWidth()-m_treeRect.Width());

		}
		else
		{
			// hide the scroll bar
			{
				m_horScrollBar.EnableWindow(FALSE);

				m_horScrollBar.ShowWindow(SW_HIDE);

				// the tree becomes larger
				m_tree.SetWindowPos(&wndTop, 0, 0, m_wndRect.Width(), m_wndRect.Height()-m_headerRect.Height(), SWP_NOMOVE);				
			}

			m_horScrollBar.SetScrollRange(0, 0);
		}

		m_tree.ResetVertScrollBar();

		// Reset the header control position and scroll
		// the tree control as required.
		int m_nCurPos = m_horScrollBar.GetScrollPos();
		m_tree.m_nOffset = -m_nCurPos;

		// smoothly scroll the tree control
		{
			CRect m_scrollRect;
			m_tree.GetClientRect(&m_scrollRect);
			m_tree.ScrollWindow(m_nCurPos, 0, &m_scrollRect, &m_scrollRect);
		}
		m_tree.m_wndHeader.GetWindowRect(&m_headerRect);	
		GetClientRect(&m_wndRect);

		m_tree.m_wndHeader.SetWindowPos(&wndTop, m_tree.m_nOffset, 0,max(StretchWidth(m_tree.GetColumnsWidth(),m_treeRect.Width()),m_wndRect.Width()), m_headerRect.Height(),SWP_SHOWWINDOW);			
	}	
}


BOOL CPackageToolView::PreTranslateMessage(MSG* pMsg) 
{	
	//if(pMsg->message == WM_MOUSEMOVE && pMsg->hwnd == m_tree.m_hWnd)
	//{		
	//	CDC* pDC;
	//	pDC=GetDC();
	//	HDC hDC=pDC->GetSafeHdc();
	//	CDC HeadDC;
	//	HeadDC.Attach(hDC);
	//	// Save DC
	//	int nSavedDC = HeadDC.SaveDC();
	//	CFont font;
	//	font.CreateFont(14,0,                        // nWidth
	//		0,                         // nEscapement
	//		0,                         // nOrientation
	//		FW_NORMAL,                 // nWeight
	//		FALSE,                     // bItalic
	//		FALSE,                     // bUnderline
	//		0,                         // cStrikeOut
	//		ANSI_CHARSET,              // nCharSet
	//		OUT_DEFAULT_PRECIS,        // nOutPrecision
	//		CLIP_DEFAULT_PRECIS,       // nClipPrecision
	//		DEFAULT_QUALITY,           // nQuality
	//		DEFAULT_PITCH,			   // nPitchAndFamily
	//		NULL);                     // lpszFacename

	//	HeadDC.SelectObject(font);

	//	TVHITTESTINFO hInfo;
	//	CPoint point(LOWORD(pMsg->lParam),HIWORD(pMsg->lParam));
	//	hInfo.pt = point;	
	//	m_tree.HitTest(&hInfo);				

	//	if (hInfo.hItem!=NULL)
	//	{
	//		CArray<RECT,RECT> ColumnRectArray;
	//		CArray<int,int>	ColumnWidthArray;
	//		CArray<CString,CString>	ItemTextArray;
	//		CArray<CSize,CSize>	ItemTextWidthArray;

	//		for (int i=0;i<m_tree.m_wndHeader.GetItemCount();i++)
	//		{
	//			RECT ColumnRect;					
	//			m_tree.m_wndHeader.GetItemRect(i,&ColumnRect);
	//			ColumnRectArray.Add(ColumnRect);
	//			int WidthOfColumn=ColumnRect.right-ColumnRect.left;				
	//			ColumnWidthArray.Add(WidthOfColumn);
	//			CString Text=m_tree.GetItemText(hInfo.hItem,i);	
	//			ItemTextArray.Add(Text);
	//			if(pDC)
	//			{
	//				CSize Width=pDC->GetTextExtent(Text);				
	//				ItemTextWidthArray.Add(Width);
	//			}
	//		}		

	//		for (i=0;i<m_tree.m_wndHeader.GetItemCount();i++)
	//		{
	//			if (i==0)
	//			{					
	//				CPoint point=GetCurrentMessage()->pt;
	//				RECT TextItemRect;
	//				m_tree.GetItemRect(hInfo.hItem,&TextItemRect,TRUE);					
	//				point.x=TextItemRect.right-int((TextItemRect.right-TextItemRect.left)/2);					
	//				if ( (hInfo.flags & TVHT_ONITEMRIGHT) && (hInfo.pt.x<ColumnRectArray.GetAt(i).right) &&
	//					(ItemTextWidthArray.GetAt(i).cx>
	//					(ColumnWidthArray.GetAt(i)-TextItemRect.right-4-int((TextItemRect.right-TextItemRect.left)/2))) )					
	//				{				
	//					CString colText=m_tree.GetItemText(hInfo.hItem,i);	
	//					DisplayToolTip(point,hInfo,colText);				
	//					m_tooltip.RelayEvent(pMsg);	
	//					break;
	//				}
	//				else
	//					m_tooltip.Pop();					
	//			}
	//			else
	//			{
	//				if ((hInfo.pt.x>ColumnRectArray.GetAt(i-1).right) &&
	//					(hInfo.pt.x<ColumnRectArray.GetAt(i).right) &&
	//					(ItemTextWidthArray.GetAt(i).cx>(ColumnWidthArray.GetAt(i)-8)) )
	//				{						
	//					CString colText=m_tree.GetItemText(hInfo.hItem,i);							
	//					point.x=ColumnRectArray.GetAt(i).left;						
	//					DisplayToolTip(point,hInfo,colText);				
	//					m_tooltip.RelayEvent(pMsg);	
	//					break;
	//				}
	//				else 
	//					m_tooltip.Pop();			
	//			}
	//		}		

	//		ColumnRectArray.RemoveAll();
	//		ColumnWidthArray.RemoveAll();
	//		ItemTextArray.RemoveAll();
	//		ItemTextWidthArray.RemoveAll();
	//	}
	//	else
	//		m_tooltip.Pop();					

	//	// Restore dc

	//	HeadDC.RestoreDC( nSavedDC );

	//	// Detach the dc before returning
	//	HeadDC.Detach();

	//}
	return CView::PreTranslateMessage(pMsg);
}


void CPackageToolView::DisplayToolTip(CPoint point, TVHITTESTINFO hInfo, CString colText)
{	
	RECT ItemRect;
	CToolInfo ToolInfo;

	if (m_tooltip.GetToolInfo(ToolInfo,GetDlgItem(ID_TREE_LIST_CTRL)))
	{					
		short height=m_tree.GetItemHeight();
		m_tree.GetItemRect(hInfo.hItem,&ItemRect,TRUE);	

		point.y=ItemRect.top+height+3;
		ClientToScreen(&point);

		m_tooltip.SendMessage(TTM_TRACKACTIVATE, TRUE, (LPARAM)&ToolInfo);
		m_tooltip.SendMessage(TTM_TRACKPOSITION, 0, (LPARAM) (DWORD)MAKELONG(point.x, point.y));				

		ToolInfo.uFlags|=TTF_TRACK; 
		ToolInfo.uFlags|=TTF_ABSOLUTE; 

		m_tooltip.SetToolInfo(&ToolInfo);

		if (hInfo.hItem==m_tree.GetSelectedItem())
		{		
			m_tooltip.SetTipBkColor(RGB(128,128,95));
			m_tooltip.SetTipTextColor(RGB(255,255,255));			
		}
		else
		{
			m_tooltip.SetTipBkColor(RGB(250,254,218));
			m_tooltip.SetTipTextColor(RGB(0,0,0));
		}
		m_tooltip.UpdateTipText(colText,&m_tree);								
		m_tooltip.Update();
	}						
}

BOOL CPackageToolView::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: 在此添加专用代码和/或调用基类
	switch(message)
	{
	case TREELIST_ADD:
		{
			OnAddFileInfo((CFilesInfo*)wParam,(tagFileInfo*)lParam);
		}
		break;
	case TREELIST_UPDATE:
		{
			OnUpdateFileInfo((CFilesInfo*)wParam,(tagFileInfo*)lParam);
		}
		break;
	case TREELIST_DELETE:
		{
			OnDeleteFileInfo((CFilesInfo*)wParam,(tagFileInfo*)lParam);
		}
		break;
	case TREELIST_DELALL:
		{
			OnDeleteAllFileInfo();
		}
		break;
	case TREELIST_INITIAL:
		{
			OnInitialFileInfo();
		}
		break;
	case TREELIST_UPDATEWITHCHILD:
		{
			UpdateItemWithChild((CFilesInfo*)wParam,(tagFileInfo*)lParam);
		}
		break;


	//资源类消息
	case LOADRESOURCE_START:
		{
			OnLoadResourceStart((CClientResource*)wParam);
		}
		break;
	case LOADRESOURCE_END:
		{
			OnLoadResourceEnd((CClientResource*)wParam);
		}
		break;
	case RELOADRESOURCE_START:
		{
			OnReLoadResourceStart((CClientResource*)wParam);
		}
		break;
	case RELOADRESOURCE_END:
		{
			OnReLoadResourceEnd((CClientResource*)wParam);
		}
		break;
	case MAKEPACKAGE_START:
		{
			OnMakePackageStart((CClientResource*)wParam);
		}
		break;
	case MAKEPACKAGE_ING:
		{
			OnMakePackageing((CFilesInfo*)lParam,(tagFileInfo*)wParam);
		}
		break;
	case MAKEPACKAGE_END:
		{
			OnMakePackageEnd((CClientResource*)wParam);
		}
		break;
	case REMAKEPACKAGE_START:
		{
			OnReMakePackageStart((CClientResource*)wParam);
		}
		break;
 	case REMAKEPACKAGE_END:
		{
			OnReMakePackageEnd((CClientResource*)wParam);
		}
		break;
	}

	return CView::OnWndMsg(message, wParam, lParam, pResult);
}

void CPackageToolView::DoWithItem(HTREEITEM Item,DoItemType Type,
								  DWORD dwPackType,DWORD dwCompressType,CPackageToolDoc* pDoc)
{
	if(pDoc == NULL)	return;

	// TODO: 在此添加命令处理程序代码
	HTREEITEM hChild=NULL;
	if(Item != NULL)
	{
		if(m_tree.ItemHasChildren(Item))
		{
			HTREEITEM ChildOfParent=NULL;
			ChildOfParent=m_tree.GetChildItem(Item);
			while(ChildOfParent!=NULL)
			{						
				//DoWithItem(ChildOfParent,Type,pDoc);
				ChildOfParent = m_tree.GetNextSiblingItem(ChildOfParent);
			}
		}
		
		string strFileName = m_tree.GetItemTextWithParent(Item);
		DWORD dwSize=0,dwCrc32=0,dwPackageType=-1,dwCompressType=-1;bool bFolder=false;
		if(pDoc->GetFileInfoFromOpFilesInfo(strFileName,dwSize,dwCrc32,dwPackageType,dwCompressType,bFolder))
		{
			if(Type==DO_PacWithCom)
			{
				if(dwPackageType !=1 || dwCompressType != 1)
				{
					pDoc->UpdateFileInfoToOpFilZesInfo(strFileName,dwCrc32,dwSize,bFolder,1,1);
				}
			}
			else if(Type==DO_Pac)
			{
				if(dwPackageType !=1 || dwCompressType != 0)
				{
					pDoc->UpdateFileInfoToOpFilZesInfo(strFileName,dwCrc32,dwSize,bFolder,1,0);
				}
			}
			else if(Type ==DO_UnPac)
			{
				if(dwPackageType != 0 || dwCompressType != 0)
				{
					pDoc->UpdateFileInfoToOpFilZesInfo(strFileName,dwCrc32,dwSize,bFolder,0,0);
				}
			}
		}
	}
}

void CPackageToolView::OnInsertItem(tagFileIndex* FileIndex)
{
	if(FileIndex==NULL)	return;
	HTREEITEM htItem = NULL;//m_tree.InsertItem(FileIndex->szName);
	if(htItem)
	{
		m_tree.SetItemText(htItem,1,"0");
		m_tree.SetItemText(htItem,2,"是");

		//m_tree.SetItemText(htItem,3,GetPackFormatSring(FileIndex->dwCompressType).c_str() );
		if( (FileIndex->dwCompressType & 0x1) == 1)
		{
			m_tree.SetItemText(htItem,3,"否");
		}
		else
		{
			m_tree.SetItemText(htItem,3,"是");
		}
		m_tree.SetItemText(htItem,4,"是");
	
		m_tree.AddNumber(htItem,1,FileIndex->dwSize);
	}
}

void CPackageToolView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CView::OnRButtonDown(nFlags, point);

}

void CPackageToolView::OnPopmenuFilesSet()
{
	// TODO: 在此添加命令处理程序代码
	CPackageToolDoc* pDoc = GetDocument();
	if(pDoc==NULL)	return;

	if(pDoc->GetDisplayOriInfo() == 1)
	{
		//选择包名
		string strFileName = m_tree.GetItemTextWithParent(m_tree.GetSelectedItem());
		DWORD dwSize=0,dwCrc32=0,dwPackageType=-1,dwCompressType=-1;bool bFolder=false;
		if(pDoc->GetFileInfoFromOpFilesInfo(strFileName,dwSize,dwCrc32,dwPackageType,dwCompressType,bFolder) )
		{		
			CFolderSet Sets(strFileName,pDoc->GetPackageInfos(),pDoc->GetCompressInfos(),
							dwPackageType,dwCompressType);
			if( Sets.DoModal() == IDOK)
			{
				if(dwPackageType != Sets.m_nPackType || dwCompressType != Sets.m_nCompressType)
				{
					pDoc->UpdateFileInfoToOpFilesWithChild(strFileName,
															Sets.m_nPackType,Sets.m_nCompressType,
															Sets.m_bForceModifyPackType,
															Sets.m_bForceModifyCompressType);
				}
			}
		}
	}
}

void CPackageToolView::OnPopmenuPackagewithcompress()
{
	// TODO: 在此添加命令处理程序代码
	CPackageToolDoc* pDoc = GetDocument();
	if(pDoc==NULL)	return;

	//if(pDoc->GetDisplayOriInfo())
	//{
	//	//选择包名
	//	CFolderSet Sets;
	//	if( Sets.DoModal() == IDOK)
	//	{
	//		//DoWithItem(m_tree.GetSelectedItem(),DO_PacWithCom,pDoc);
	//	}
	//}

	//m_tree.UpdateData();
}

void CPackageToolView::OnPopmenuPackage()
{
	// TODO: 在此添加命令处理程序代码
	//CPackageToolDoc* pDoc = GetDocument();
	//if(pDoc==NULL)	return;

	//if(pDoc->GetDisplayOriInfo())
	//{
	//	//DoWithItem(m_tree.GetSelectedItem(),DO_Pac,pDoc);
	//}

	//m_tree.UpdateData();
}

void CPackageToolView::OnPopmenuUnpackage()
{
	// TODO: 在此添加命令处理程序代码

	CPackageToolDoc* pDoc = GetDocument();
	if(pDoc==NULL)	return;

	if(pDoc->GetDisplayOriInfo())
	{
		//选择包名
		string strFileName = m_tree.GetItemTextWithParent(m_tree.GetSelectedItem());
		DWORD dwSize=0,dwCrc32=0,dwPackageType=-1,dwCompressType=-1;bool bFolder=false;
		if(pDoc->GetFileInfoFromOpFilesInfo(strFileName,dwSize,dwCrc32,dwPackageType,dwCompressType,bFolder) )
		{	
			if( dwPackageType != 1 || dwCompressType != 1 )
			{
				pDoc->UpdateFileInfoToOpFilesWithChild(strFileName,
														1,1,true,true);
			}
		}
	}

	m_tree.UpdateData();
}


void CPackageToolView::OnResoucePackage()
{
	// TODO: 在此添加命令处理程序代码
	CPackageToolDoc* pDoc = GetDocument();
	if(pDoc==NULL)	return;

	pDoc->MakePakcage();
}

void CPackageToolView::OnPopmenuAddfolder()
{
	CPackageToolDoc* pDoc = GetDocument();
	if(pDoc==NULL)	return;

	
	// TODO: 在此添加命令处理程序代码
	HTREEITEM htSelect = m_tree.GetSelectedItem();
	if(htSelect==NULL)	return;

	CString strFolder;
	BROWSEINFO bi;
	ZeroMemory(&bi,sizeof(BROWSEINFO));
	bi.hwndOwner=GetSafeHwnd();
	bi.lpszTitle="选择要操作的资源文件夹:";
	LPITEMIDLIST idl=SHBrowseForFolder(&bi);
	if(idl==NULL)
		return;
	SHGetPathFromIDList(idl,strFolder.GetBuffer(MAX_PATH));

	if(strFolder != "")
	{
		pDoc->OnAddFolder(string(m_tree.GetItemTextWithParent(htSelect)),string(strFolder));
	}
}

void CPackageToolView::OnPopmenuAddfile()
{
	CPackageToolDoc* pDoc = GetDocument();
	if(pDoc==NULL)	return;

	HTREEITEM htSelect = m_tree.GetSelectedItem();
	if(htSelect==NULL)	return;
	// TODO: 在此添加命令处理程序代码
	CFileDialog	OpenDlg(true);
	if (OpenDlg.DoModal() == IDOK)
	{
		CString strFile = OpenDlg.GetPathName();
		pDoc->OnAddFile( string(m_tree.GetItemTextWithParent(htSelect)),string(strFile) );
	}
}

void CPackageToolView::OnPopmenuDelete()
{
	// TODO: 在此添加命令处理程序代码
	CPackageToolDoc* pDoc = GetDocument();
	if(pDoc==NULL)	return;

	HTREEITEM htSelect = m_tree.GetSelectedItem();
	if(htSelect==NULL || htSelect == m_tree.GetRootItem())	return;

	if( AfxMessageBox("此操作将删除磁盘上的原始数据，确认是否要删除?",MB_OKCANCEL)==IDOK)
	{
		pDoc->OnDelFileFolder(string(m_tree.GetItemTextWithParent(htSelect)),string(""));
	}
}

void CPackageToolView::OnPopmenuExtract()
{
	// TODO: 在此添加命令处理程序代码
	CPackageToolDoc* pDoc = GetDocument();
	if(pDoc==NULL)	return;
	// TODO: 在此添加命令处理程序代码
	HTREEITEM htSelect = m_tree.GetSelectedItem();
	if(htSelect==NULL || htSelect == m_tree.GetRootItem())	return;

	CString strFolder;
	BROWSEINFO bi;
	ZeroMemory(&bi,sizeof(BROWSEINFO));
	bi.hwndOwner=GetSafeHwnd();
	bi.lpszTitle="选择要操作的资源文件夹:";
	LPITEMIDLIST idl=SHBrowseForFolder(&bi);
	if(idl==NULL)
		return;
	SHGetPathFromIDList(idl,strFolder.GetBuffer(MAX_PATH));

	if(strFolder != "")
	{
		pDoc->OnExtrFileFolder(string(m_tree.GetItemTextWithParent(htSelect)),string(strFolder));
	}
	::AfxMessageBox("提取文件完毕!");
}


void CPackageToolView::InsertAllFile(CFilesInfo* pFilesInfo,tagFileInfo* ptagFileInfo)
{
	if(pFilesInfo ==NULL || ptagFileInfo==NULL)	return;

	HTREEITEM hTmp1=NULL; 
	if(pFilesInfo->GetRootFile() != ptagFileInfo)
	{
		int nImage=0;
		int nSelectImage=0;
		this->GetItemImage(ptagFileInfo,nImage,nSelectImage);
		HTREEITEM hTmp1 = m_tree.InsertItemByStr(pFilesInfo->GetFileTextWithParent(ptagFileInfo).c_str(),nImage,nSelectImage);	
		CString strNum;

		strNum.Format("%u",ptagFileInfo->dwCrc32);
		m_tree.SetItemText(hTmp1,1,(LPCTSTR)strNum);

		strNum.Format("%u",ptagFileInfo->dwSize);
		m_tree.SetItemText(hTmp1,2,(LPCTSTR)strNum);
		
		/*if(ptagFileInfo->dwPackageType  & 0x1 == 1)
			m_tree.SetItemText(hTmp1,2,"否");
		else
			m_tree.SetItemText(hTmp1,2,"是");*/
		m_tree.SetItemText(hTmp1,3,GetPackFormatSring(ptagFileInfo->dwPackageType).c_str() );

		//if( (ptagFileInfo->dwCompressType & 0x1) == 1)
		//	m_tree.SetItemText(hTmp1,3,"否");
		//else
		//	m_tree.SetItemText(hTmp1,3,"是");
		m_tree.SetItemText(hTmp1,4,GetCompFormatSring(ptagFileInfo->dwCompressType).c_str() );

		m_tree.SetItemText(hTmp1,5,"是");

		if( (ptagFileInfo->dwPackageType & 0x1) == 0)
		{
			if( (ptagFileInfo->dwCompressType & 0x1) != 1)
				m_tree.SetItemColor(hTmp1,RGB(200,0,0));
			else
				m_tree.SetItemColor(hTmp1,RGB(100,0,0));
		}

		m_tree.SetItemFolder(hTmp1,ptagFileInfo->bFolder);
	}
	

	if(pFilesInfo->HaveChild(ptagFileInfo))
	{
		if(hTmp1)
			m_tree.SetItemFolder(hTmp1,TRUE);
		
		itFileInfo it = ptagFileInfo->ChildFilesInfo.begin();
		for(;it != ptagFileInfo->ChildFilesInfo.end();it++)
		{
			InsertAllFile(pFilesInfo,(*it).second);
		}
	}
	else
	{
		if(hTmp1)
			m_tree.SetItemFolder(hTmp1,FALSE);
	}
	
}

void CPackageToolView::OnInitialFileInfo()
{
	// TODO: 在此添加命令处理程序代码
	CPackageToolDoc* pDoc = GetDocument();
	if(pDoc==NULL)	return;

	CString strFolder = pDoc->GetCurFolder().c_str();
	//删去所有节点
	m_tree.DeleteAllItems();
	HTREEITEM hTmp;
	int nImage=1;
	int nSelectImage=0;
	hTmp = m_tree.InsertItem( (LPCTSTR)strFolder,TRUE,nImage,nSelectImage,0,0,FALSE,FALSE);	
	m_tree.SetItemFolder(hTmp,true);

	CString str;
	str.Format("%d",0);
	m_tree.SetItemText(hTmp,1,(LPCTSTR)str);
}

void CPackageToolView::GetItemImage(tagFileInfo* ptagFileInfo,int& nImage,int& nSelectImage)
{
	if(ptagFileInfo==NULL)	return;

	if(ptagFileInfo->bFolder)
	{
		if( (ptagFileInfo->dwPackageType & 0x1) == 1)
		{
			nImage=1;
		}
		else
		{
			if( (ptagFileInfo->dwCompressType & 0x1) == 1)
				nImage=2;
			else
				nImage=4;
		}
	}
	else
	{
		if( (ptagFileInfo->dwPackageType & 0x1) == 1)
		{
			nImage=6;
		}
		else
		{
			if( (ptagFileInfo->dwCompressType & 0x1) == 1)
				nImage=3;
			else
				nImage=5;
		}
	}
	nSelectImage = 0;
}

void CPackageToolView::OnAddFileInfo(CFilesInfo* pFileInfo,tagFileInfo* ptagFileInfo)
{
	if(pFileInfo==NULL || ptagFileInfo==NULL)	return;

	int nImage=0;
	int nSelectImage=0;	
	GetItemImage(ptagFileInfo,nImage,nSelectImage);

	string strTemptFileName(pFileInfo->GetFileTextWithParent(ptagFileInfo).c_str());
	HTREEITEM htItem;
	if(strTemptFileName=="")
	{
		htItem=m_tree.GetRootItem();
	}
	else
	
	{
		htItem = m_tree.InsertItemByStr(strTemptFileName.c_str(),nImage,nSelectImage);	
	}
	CString strNum;
	strNum.Format("%u",ptagFileInfo->dwCrc32);
	m_tree.SetItemText(htItem,1,(LPCTSTR)strNum);

	strNum.Format("%u",ptagFileInfo->dwSize);
	m_tree.SetItemText(htItem,2,(LPCTSTR)strNum);

	m_tree.SetItemText(htItem,3,GetPackFormatSring(ptagFileInfo->dwPackageType).c_str() );
	m_tree.SetItemText(htItem,4,GetCompFormatSring(ptagFileInfo->dwCompressType).c_str() );

	m_tree.SetItemText(htItem,5,"是");
	if( (ptagFileInfo->dwPackageType & 0x1) == 1)
	{
		m_tree.SetItemColor(htItem,RGB(0,0,0));
	}
	else
	{
		if( (ptagFileInfo->dwCompressType & 0x1) == 1)
			m_tree.SetItemColor(htItem,RGB(100,0,0));
		else
			m_tree.SetItemColor(htItem,RGB(200,0,0));
	}
	m_tree.SetItemFolder(htItem,ptagFileInfo->bFolder);

	m_tree.UpdateData();
	OnOperatonFile((char*)strTemptFileName.c_str());
}
void CPackageToolView::OnUpdateFileInfo(CFilesInfo* pFileInfo,tagFileInfo* ptagFileInfo)
{
	if(pFileInfo==NULL || ptagFileInfo==NULL)	return;

	CString strName(pFileInfo->GetFileTextWithParent(ptagFileInfo).c_str());
	HTREEITEM htItem = m_tree.GetItemByText(strName);
	
	SetItemInfo(htItem,ptagFileInfo);

	m_tree.UpdateData();

	OnOperatonFile((char*)pFileInfo->GetFileTextWithParent(ptagFileInfo).c_str());
}

void CPackageToolView::SetItemInfo(HTREEITEM hItem,tagFileInfo* ptagFileInfo)
{
	if(hItem == NULL || ptagFileInfo == NULL)	return;
	int nImage=0;
	int nSelectImage=0;	
	GetItemImage(ptagFileInfo,nImage,nSelectImage);
	m_tree.SetItemImage(hItem,nImage,nSelectImage);
	

	CString strNum;
	strNum.Format("%u",ptagFileInfo->dwCrc32);
	m_tree.SetItemText(hItem,1,(LPCTSTR)strNum);

	strNum.Format("%u",ptagFileInfo->dwSize);
	m_tree.SetItemText(hItem,2,(LPCTSTR)strNum);

	m_tree.SetItemText(hItem,3,GetPackFormatSring(ptagFileInfo->dwPackageType).c_str() );
	m_tree.SetItemText(hItem,4,GetCompFormatSring(ptagFileInfo->dwCompressType).c_str() );

	m_tree.SetItemText(hItem,5,"是");
	if( (ptagFileInfo->dwPackageType & 0x1) == 1)
	{
		m_tree.SetItemColor(hItem,RGB(0,0,0));
	}
	else
	{
		if( (ptagFileInfo->dwCompressType & 0x1) == 1)
			m_tree.SetItemColor(hItem,RGB(100,0,0));
		else
			m_tree.SetItemColor(hItem,RGB(200,0,0));
	}
	m_tree.SetItemFolder(hItem,ptagFileInfo->bFolder);
}

void CPackageToolView::UpdateItemWithChild(CFilesInfo* pFileInfo,tagFileInfo* ptagFileInfo)
{
	if(pFileInfo == NULL || ptagFileInfo == NULL)	return;
	string strUpdateFolder = pFileInfo->GetFileTextWithParent(ptagFileInfo);

	CString strName(strUpdateFolder.c_str());
	HTREEITEM hItem = m_tree.GetItemByText(strName);

	if(hItem == NULL)	return;
	//先删除所有子节点
	m_tree.DeleteSubItems(hItem);
	SetItemInfo(hItem,ptagFileInfo);

	InsertItemWithChild(hItem,pFileInfo,ptagFileInfo);

	m_tree.UpdateData();
	
}
void CPackageToolView::InsertItemWithChild(HTREEITEM hItem,CFilesInfo* pFileInfo,tagFileInfo* ptagFileInfo)
{
	if(pFileInfo->HaveChild(ptagFileInfo))
	{
		for(itFileInfo it = ptagFileInfo->ChildFilesInfo.begin();
			it != ptagFileInfo->ChildFilesInfo.end();it++)
		{
			//插入节点
			tagFileInfo* ptagChildFileInfo = (*it).second;
			HTREEITEM hChildItem = m_tree.InsertItem(ptagChildFileInfo->strFileName.c_str(),
									ptagChildFileInfo->bFolder,0,0,hItem);
			SetItemInfo(hChildItem,ptagChildFileInfo);
			InsertItemWithChild(hChildItem,pFileInfo,(*it).second);
		}
	}
}

void CPackageToolView::OnDeleteFileInfo(CFilesInfo* pFileInfo,tagFileInfo* ptagFileInfo)
{
	if(pFileInfo==NULL || ptagFileInfo==NULL)	return;

	CString strName(pFileInfo->GetFileTextWithParent(ptagFileInfo).c_str());
	HTREEITEM htItem = m_tree.GetItemByText(strName);
	if(htItem)
		m_tree.DeleteItem(htItem);
	m_tree.UpdateData();

	OnOperatonFile((char*)(LPCTSTR)strName);
}
void CPackageToolView::OnDeleteAllFileInfo()
{
	m_tree.DeleteAllItems();

	m_tree.UpdateData();
}

void CPackageToolView::OnResouceRemakepackage()
{
	::AfxMessageBox("不可用！");
	// TODO: 在此添加命令处理程序代码
	CPackageToolDoc* pDoc = GetDocument();
	if(pDoc==NULL)	return;

	pDoc->ReMakePakcage();
}

void CPackageToolView::OnResouceReloadresource()
{
	// TODO: 在此添加命令处理程序代码
	// TODO: 在此添加命令处理程序代码
	CPackageToolDoc* pDoc = GetDocument();
	if(pDoc==NULL)	return;

	HTREEITEM htSelect = m_tree.GetSelectedItem();
	if(htSelect==NULL)	return;

	string strFolder=string(m_tree.GetItemTextWithParent(htSelect));

	pDoc->ReLoadFolder(strFolder);
}

void CPackageToolView::OnOperatonFile(char* strFileName)
{
	string strTemptName(strFileName);

	CPackageToolDoc* pDoc = GetDocument();
	if(pDoc==NULL)	return;

	pDoc->SetStatStatuFileName(strTemptName);
}
//装载资源开始
void CPackageToolView::OnLoadResourceStart(CClientResource* pClientReousrce)
{
	if(pClientReousrce==NULL)	return;

	CPackageToolDoc* pDoc = GetDocument();
	if(pDoc==NULL)	return;

	OnInitialFileInfo();
	pDoc->SetStatStatuOp(string("装载资源..."));
}
//装载资源结束
void CPackageToolView::OnLoadResourceEnd(CClientResource* pClientReousrce)
{
	if(pClientReousrce==NULL)	return;

	CPackageToolDoc* pDoc = GetDocument();
	if(pDoc==NULL)	return;

	m_tree.UpdateData();
	m_tree.SortItems(0,false,m_tree.GetRootItem());

	pDoc->SetStatStatuOp(string(""));
	AfxMessageBox("装载资源完毕!");
}

//从新装载资源开始
void CPackageToolView::OnReLoadResourceStart(CClientResource* pClientReousrce)
{
	if(pClientReousrce==NULL)	return;

	CPackageToolDoc* pDoc = GetDocument();
	if(pDoc==NULL)	return;

	pDoc->SetStatStatuOp(string("从新装载资源..."));
}
//从新装载资源结束
void CPackageToolView::OnReLoadResourceEnd(CClientResource* pClientReousrce)
{
	if(pClientReousrce==NULL)	return;

	CPackageToolDoc* pDoc = GetDocument();
	if(pDoc==NULL)	return;

	m_tree.UpdateData();
	m_tree.SortItems(0,false,m_tree.GetRootItem());

	pDoc->SetStatStatuOp(string(""));
	AfxMessageBox("从新装载资源完毕!");
}

//打包开始
void CPackageToolView::OnMakePackageStart(CClientResource* pClientReousrce)
{
	if(pClientReousrce==NULL)	return;

	CPackageToolDoc* pDoc = GetDocument();
	if(pDoc==NULL)	return;

	pDoc->SetStatStatuOp(string("打包..."));
}

//打包中
void  CPackageToolView::OnMakePackageing(CFilesInfo* pFileInfo,tagFileInfo* ptagFileInfo)
{
	if(pFileInfo && ptagFileInfo)
		OnOperatonFile((char*)pFileInfo->GetFileTextWithParent(ptagFileInfo).c_str());
}

//打包结束
void CPackageToolView::OnMakePackageEnd(CClientResource* pClientReousrce)
{
	if(pClientReousrce==NULL)	return;

	CPackageToolDoc* pDoc = GetDocument();
	if(pDoc==NULL)	return;

	m_tree.UpdateData();

	pDoc->SetStatStatuOp(string(""));
	pDoc->MakePakcageEnd();

	::AfxMessageBox("打包完毕!");
}

//整理包开始
void CPackageToolView::OnReMakePackageStart(CClientResource* pClientReousrce)
{
	if(pClientReousrce==NULL)	return;

	CPackageToolDoc* pDoc = GetDocument();
	if(pDoc==NULL)	return;

	pDoc->SetStatStatuOp(string("整理包..."));
}
//整理包结束
void CPackageToolView::OnReMakePackageEnd(CClientResource* pClientReousrce)
{
	if(pClientReousrce==NULL)	return;

	CPackageToolDoc* pDoc = GetDocument();
	if(pDoc==NULL)	return;

	::AfxMessageBox("整理包完毕!");
	pDoc->SetStatStatuOp(string(""));
}

void CPackageToolView::OnPopmenuReloadresource()
{
	// TODO: 在此添加命令处理程序代码
	CPackageToolDoc* pDoc = GetDocument();
	if(pDoc==NULL)	return;

	HTREEITEM htSelect = m_tree.GetSelectedItem();
	if(htSelect==NULL)	return;

	string strFolder=string(m_tree.GetItemTextWithParent(htSelect));

	pDoc->ReLoadFolder(strFolder,true);
}

void CPackageToolView::OnPopmenuReloadresourceex()
{
	// TODO: 在此添加命令处理程序代码
	// TODO: 在此添加命令处理程序代码
	CPackageToolDoc* pDoc = GetDocument();
	if(pDoc==NULL)	return;

	HTREEITEM htSelect = m_tree.GetSelectedItem();
	if(htSelect==NULL)	return;

	string strFolder=string(m_tree.GetItemTextWithParent(htSelect));

	pDoc->ReLoadFolder(strFolder,false);
}
