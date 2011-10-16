// TreeListEditBox.cpp : implementation file
//

#include "stdafx.h"
#include "TreeListEditBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTreeListEditBox

CTreeListEditBox::CTreeListEditBox(HTREEITEM hItem, int iSubItem, CString sInitText)
:m_sInitText( sInitText )
{
	// initialte variables
	m_hItem=hItem;
	m_iSubItem = iSubItem;
}

CTreeListEditBox::~CTreeListEditBox()
{
}


BEGIN_MESSAGE_MAP(CTreeListEditBox, CEdit)
	//{{AFX_MSG_MAP(CTreeListEditBox)
	ON_WM_KILLFOCUS()
	ON_WM_NCDESTROY()
	ON_WM_CHAR()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTreeListEditBox message handlers

BOOL CTreeListEditBox::PreTranslateMessage(MSG* pMsg)
{	
	if( pMsg->message == WM_KEYDOWN )
	{		
		bTab=FALSE;	
		bEscape=FALSE;

		if(pMsg->wParam == VK_RETURN
				|| pMsg->wParam == VK_DELETE
				|| pMsg->wParam == VK_ESCAPE
				|| pMsg->wParam==VK_TAB				
				|| GetKeyState( VK_CONTROL)				
				)
		{			
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);
			
			if (pMsg->wParam==VK_RETURN)
			{			
				OnKillFocus(this);
			}			
			else if (pMsg->wParam==VK_ESCAPE)
			{
				bEscape=TRUE;
				OnKillFocus(this);
			}			
			else if (pMsg->wParam==VK_TAB)
			{
				bTab=TRUE;
				OnKillFocus(this);
			}				
			
			return TRUE;		    	// DO NOT process further
		}		
	}

	return CEdit::PreTranslateMessage(pMsg);
}


void CTreeListEditBox::OnKillFocus(CWnd* pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);
	
	// get text from the currently selected cell
	CString str;	
	GetWindowText(str);

	// Send Notification to parent of ListView ctrl
	LV_DISPINFO dispinfo;
	dispinfo.hdr.hwndFrom = GetParent()->m_hWnd;
	dispinfo.hdr.idFrom = GetDlgCtrlID();
	dispinfo.hdr.code = LVN_ENDLABELEDIT;

	dispinfo.item.mask = LVIF_TEXT;
	dispinfo.item.iItem = -1;
	dispinfo.item.iSubItem = m_iSubItem;
	dispinfo.item.pszText = LPTSTR((LPCTSTR)str);
	dispinfo.item.cchTextMax = str.GetLength();
	
	GetParent()->GetParent()->SendMessage( WM_NOTIFY, GetParent()->GetDlgCtrlID(), 
					(LPARAM)&dispinfo );	
}

void CTreeListEditBox::OnNcDestroy()
{
	CEdit::OnNcDestroy();

	delete this;
}


void CTreeListEditBox::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	/*// if escape key or return key
	if( nChar == VK_ESCAPE || nChar == VK_RETURN)
	{
		if( nChar == VK_ESCAPE )
			bEscape=TRUE;	
		if ( nChar == VK_RETURN )
			bEnter=TRUE;
		
		GetParent()->SetFocus();
		return;
	}*/


	CEdit::OnChar(nChar, nRepCnt, nFlags);

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

int CTreeListEditBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CEdit::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Set the proper font
	CFont* font = GetParent()->GetFont();
	SetFont(font);

	SetWindowText( m_sInitText );
	SetFocus();
	SetSel( 0, -1 );
	return 0;
}



HTREEITEM CTreeListEditBox::GetItem()
{
	return m_hItem;
}
