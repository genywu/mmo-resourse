// CQImageButton.cpp : implementation file
//

#include "stdafx.h"
#include "autoupdate.h"
#include "CQImageButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCQImageButton

CCQImageButton::CCQImageButton()
:m_csDisableBmpID(NULL)
,m_csFocusBmpID(NULL)
,m_csNormalBmpID(NULL)
{
}

CCQImageButton::~CCQImageButton()
{
	if(m_csDisableBmpID)
	{
		DeleteObject(m_csDisableBmpID);

	}
	if(m_csFocusBmpID)
	{
		DeleteObject(m_csFocusBmpID);

			
	}
	if(m_csNormalBmpID)
	{
		DeleteObject(m_csNormalBmpID);

			
	}
}
void CCQImageButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
 	
	CDC* pDC;
	CRect rc;
	HBITMAP hBitmap =NULL;
	UINT state = lpDrawItemStruct->itemState;//按钮当前状态
	pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
 	rc.CopyRect(&lpDrawItemStruct->rcItem);

	
	if (state & ODS_DISABLED   )//按下
 	{
		hBitmap = m_csDisableBmpID;

		//用位图创建区域
		//BitmapToRgn(pDC, hBitmap, rgn);
 	}
 
	
	if(state & ODS_FOCUS    )
	{
 		hBitmap = m_csFocusBmpID;
        
		
		//用位图创建区域
		//BitmapToRgn(pDC, hBitmap, rgn);
 	}
	if(hBitmap == NULL)
	{
		
		hBitmap = m_csNormalBmpID;
	
		
		
		//用位图创建区域
		//BitmapToRgn(pDC, hBitmap, rgn);
		
	}
	
	if(hBitmap!=NULL)
	{
		
		CDC memDC;
		CBitmap bitmap, *pOldBmp;
		
		
		memDC.CreateCompatibleDC(pDC);
		bitmap.Attach(hBitmap);
		//	bitmap.GetBitmap(&bm);
		
		pOldBmp = memDC.SelectObject(&bitmap);
		pDC->BitBlt(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
			&memDC, 0, 0, SRCCOPY);
		
		memDC.SelectObject(pOldBmp);
		//将区域设置为窗口
		
		bitmap.Detach();
	}
	//rgn.CreateEllipticRgn(0, 0, bm.bmWidth, bm.bmHeight);
	//SetWindowRgn((HRGN)rgn, TRUE);
}


void CCQImageButton::SetButtonBmp(UINT csDisableBmpID, UINT csNormalBmpID,UINT csFocusBmpID,HINSTANCE AppHandle)
{
	LoadBitMapRes(m_csDisableBmpID,csDisableBmpID ,AppHandle);
    LoadBitMapRes(m_csNormalBmpID,csNormalBmpID,AppHandle);
    LoadBitMapRes(m_csFocusBmpID,csFocusBmpID,AppHandle);

	

}


BEGIN_MESSAGE_MAP(CCQImageButton, CButton)
	//{{AFX_MSG_MAP(CCQImageButton)
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCQImageButton message handlers

void CCQImageButton::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	CButton::OnMouseMove(nFlags, point);

	if(IsWindowEnabled())
	{
		CRect rect;
		GetWindowRect(&rect);
		ScreenToClient(&rect);
		if(rect.PtInRect(point))
		{	
			SetCapture();
			SetFocus();	
			
			Invalidate(FALSE);
		}
		else
		{
			ReleaseCapture();
			GetParent()->SetFocus();

			Invalidate(FALSE);

		}
	}
}

//DEL void CCQImageButton::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
//DEL {
//DEL 	CButton::OnActivate(nState, pWndOther, bMinimized);
//DEL 	
//DEL 	// TODO: Add your message handler code here
//DEL 	
//DEL }
void LoadBitMapRes(HBITMAP& BitMap, UINT BitMapRes,HINSTANCE AppHandle)
{
	if(BitMap)
	{
		if(!DeleteObject(BitMap))
		{
			DisplayErrorInfo(string("DeleteObject"));

		}
	}
 	if(!(BitMap   = LoadBitmap(AppHandle,MAKEINTRESOURCE(BitMapRes))))
	{
		DisplayErrorInfo(string("LoadBitmap"));
	}

}

