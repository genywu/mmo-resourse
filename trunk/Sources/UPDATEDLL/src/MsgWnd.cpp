// MsgWnd.cpp : implementation file
//

#include "stdafx.h"
#include "..\updatedll.h"

#include "MsgWnd.h"

/////////////////////////////////////////////////////////////////////////////
// CMsgWnd

CMsgWnd::CMsgWnd()
{
}

CMsgWnd::~CMsgWnd()
{
}


BEGIN_MESSAGE_MAP(CMsgWnd, CWnd)
	//{{AFX_MSG_MAP(CMsgWnd)
	ON_WM_DESTROY()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
    ON_MESSAGE(WM_DOWNNOTIFY_DEFAULT, OnDefaultMessage)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMsgWnd message handlers

void CMsgWnd::OnDestroy() 
{
    CWnd::OnDestroy();
	
	// TODO: Add your message handler code here
}

void CMsgWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	

	// Do not call CWnd::OnPaint() for painting messages
}

BOOL CMsgWnd::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CWnd::PreTranslateMessage(pMsg);
}


extern CUpdateDLLApp g_theApp;

LRESULT CMsgWnd::OnDefaultMessage(WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = 0;
    
    lResult = g_theApp.DownDispatch(GetCurrentMessage());
    
    return lResult;
}
