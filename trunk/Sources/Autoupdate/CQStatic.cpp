// CQStatic.cpp : implementation file
//

#include "stdafx.h"
#include "autoupdate.h"
#include "CQStatic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCQStatic

CCQStatic::CCQStatic()
{
}

CCQStatic::~CCQStatic()
{
}


BEGIN_MESSAGE_MAP(CCQStatic, CStatic)
	//{{AFX_MSG_MAP(CCQStatic)
	ON_WM_PAINT()
	ON_WM_DRAWITEM()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCQStatic message handlers







void CCQStatic::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	 
     CRect rect; 
     GetClientRect(&rect); 
     
     CString m_strStatic; 
     CWnd* wndStatic = dc.GetWindow(); 
     wndStatic->GetWindowText(m_strStatic); 
     
     CBitmap bitmap; 
     bitmap.Attach(m_BackImage); 
     CDC dcmem; 
     dcmem.CreateCompatibleDC(&dc); 
     CBitmap* poldbitmap = dcmem.SelectObject(&bitmap); 
     if(!dc.BitBlt(0,0,rect.Width(),rect.Height(),&dcmem,0,0,SRCCOPY))
	 {
		 DisplayErrorInfo(string("BitBlt(0,0,rect.Width(),rect.Height(),&dcmem,0,0,SRCCOPY)"));
	 }
     dcmem.SelectObject(poldbitmap); 
     
     dc.SetBkMode(TRANSPARENT); 
     dc.DrawText(m_strStatic,(LPRECT)rect,DT_CENTER); 
	
	 bitmap.Detach();
     // Do not call CStatic::OnPaint() for painting messages 
     
	// Do not call CStatic::OnPaint() for painting messages

}

void CCQStatic::SetBackGround(UINT BackImage,HINSTANCE AppHandle)
{
	if(!(m_BackImage   = LoadBitmap(AppHandle,MAKEINTRESOURCE(BackImage))))
	{
		DisplayErrorInfo(string("LoadBitmap"));
	}
	


}

 void CCQStatic::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
 {
	 // TODO: Add your message handler code here and/or call default
	
	 
	 CPaintDC dc(this);
	 CRect rect; 
	 GetClientRect(&rect); 
	 
	 CString m_strStatic; 
     CWnd* wndStatic = dc.GetWindow(); 
     wndStatic->GetWindowText(m_strStatic); 
	 
	 CBitmap bitmap; 
	 bitmap.Attach(m_BackImage); 
	 CDC dcmem; 
	 dcmem.CreateCompatibleDC(&dc); 
     CBitmap* poldbitmap = dcmem.SelectObject(&bitmap); 
	 dc.BitBlt(0,0,rect.Width(),rect.Height(),&dcmem,0,0,SRCCOPY); 
	 dcmem.SelectObject(poldbitmap); 
     
	 dc.SetBkMode(TRANSPARENT); 
	 CDC * pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
     pDC->TextOut(0,0,m_strStatic); 
	 bitmap.Detach();
	// CStatic::OnDrawItem(nIDCtl, lpDrawItemStruct);
 }
 
 //DEL BOOL CCQStatic::OnEraseBkgnd(CDC* pDC) 
 //DEL {
 //DEL 	// TODO: Add your message handler code here and/or call default
 //DEL 	
 //DEL 	 
 //DEL 	return CStatic::OnEraseBkgnd(pDC);
 //DEL }
 
 
 
 //DEL HBRUSH CCQStatic::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
 //DEL {
 //DEL 	HBRUSH hbr = CStatic::OnCtlColor(pDC, pWnd, nCtlColor);
 //DEL 	
 //DEL 	// TODO: Change any attributes of the DC here
 //DEL 	  
 //DEL 	// TODO: Return a different brush if the default is not desired
 //DEL 	return hbr;
 //DEL }




