// TreeListComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "TreeListComboBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTreeListComboBox

CTreeListComboBox::CTreeListComboBox(HTREEITEM hItem, int iSubItem, CString sInitText)
{
	m_hItem=hItem;
	m_iSubItem=iSubItem;
	m_sInitText=sInitText;
}

CTreeListComboBox::~CTreeListComboBox()
{
}


BEGIN_MESSAGE_MAP(CTreeListComboBox, CComboBox)
	//{{AFX_MSG_MAP(CTreeListComboBox)
	ON_WM_CREATE()
	ON_WM_KILLFOCUS()
	ON_WM_DESTROY()
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTreeListComboBox message handlers

int CTreeListComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;		
	
	CFont* font = GetParent()->GetFont();		
	SetFont(font);

	InitStorage(10, 100);		
	
	AddString(m_sInitText);	
	AddString("whatever");
			
	SetCurSel(FindString(0,m_sInitText));
	SetFocus();			
	
	return 0;
}

void CTreeListComboBox::OnKillFocus(CWnd* pNewWnd) 
{
	CComboBox::OnKillFocus(pNewWnd);	
	
	// get text from the currently selected cell
	CString str;	
	GetWindowText(str);

	// Send Notification to parent of ListView ctrl
	LV_DISPINFO dispinfo;
	dispinfo.hdr.hwndFrom = GetParent()->m_hWnd;
	dispinfo.hdr.idFrom = GetDlgCtrlID();
	dispinfo.hdr.code = LVN_ITEMCHANGED;

	dispinfo.item.mask = LVIF_TEXT;
	dispinfo.item.iItem = -1;
	dispinfo.item.iSubItem = m_iSubItem;
	dispinfo.item.pszText =  LPTSTR((LPCTSTR)str);
	dispinfo.item.cchTextMax = str.GetLength();
	
	int sel=GetCurSel();
	
	GetParent()->GetParent()->SendMessage( WM_NOTIFY, GetParent()->GetDlgCtrlID(), 
					(LPARAM)&dispinfo );
		
}

void CTreeListComboBox::OnDestroy() 
{
	CComboBox::OnDestroy();
	
	delete this;
}

void CTreeListComboBox::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{	
	// if escape key or return key
	if( nChar == VK_ESCAPE || nChar == VK_RETURN)
	{		
		GetParent()->SetFocus();
		return;
	}

	CComboBox::OnChar(nChar, nRepCnt, nFlags);

	// Resize edit control if needed

	// Get text extent
	CString str;
	GetWindowText( str );

	CWindowDC dc(this);
	CFont *pFont = GetParent()->GetFont();
	CFont *pFontDC = dc.SelectObject( pFont );
	CSize size = dc.GetTextExtent( str );
	dc.SelectObject( pFontDC );
	size.cx += 5;			   	// add some extra buffer

	// Get client rect
	CRect rect, parentrect;
	GetClientRect( &rect );
	GetParent()->GetClientRect( &parentrect );

	// Transform rect to parent coordinates
	ClientToScreen( &rect );
	GetParent()->ScreenToClient( &rect );

	// Check whether control needs to be resized
	// and whether there is space to grow
	if( size.cx > rect.Width() )
	{
		if( size.cx + rect.left < parentrect.right )
			rect.right = rect.left + size.cx;
		else
			rect.right = parentrect.right;
		MoveWindow( &rect );
	}
}

BOOL CTreeListComboBox::PreTranslateMessage(MSG* pMsg) 
{
	if( pMsg->message == WM_KEYDOWN )
	{		
		bTab=FALSE;
		if(pMsg->wParam == VK_RETURN
				|| pMsg->wParam == VK_DELETE
				|| pMsg->wParam == VK_ESCAPE
				|| pMsg->wParam==VK_TAB
				|| pMsg->wParam==VK_LEFT
				|| pMsg->wParam==VK_RIGHT
				|| pMsg->wParam==VK_UP
				|| pMsg->wParam==VK_DOWN
				|| GetKeyState( VK_CONTROL)				
				)
		{			
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);
			if (pMsg->wParam==VK_TAB)
			{
				bTab=TRUE;
				OnKillFocus(this);
			}		
			
			return TRUE;		    	// DO NOT process further
		}		
	}

	return CComboBox::PreTranslateMessage(pMsg);
}

HTREEITEM CTreeListComboBox::GetItem()
{
	return m_hItem;
}