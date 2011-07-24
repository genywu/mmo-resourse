// BitMapProgress.cpp : implementation file
//

#include "stdafx.h"
#include "autoupdate.h"
#include "BitMapProgress.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// BitMapProgress

BitMapProgress::BitMapProgress()
:m_BackColor(0)
{
}

BitMapProgress::~BitMapProgress()
{
}


BEGIN_MESSAGE_MAP(BitMapProgress, CProgressCtrl)
	//{{AFX_MSG_MAP(BitMapProgress)
	ON_WM_PAINT()
	ON_WM_NCPAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// BitMapProgress message handlers

void BitMapProgress::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	//进度条的上下文环境句柄
	
	
	int pos=GetPos();
	int low,hight;
	low=hight=0;
	GetRange(low,hight);
	int range=hight-low;
	
		

	//得到当前进度(%)
	float percent=float(pos)/(float)range;
	CRect rect;
	GetClientRect(&rect);
	dc.FillSolidRect(&rect,m_BackColor);
	
	rect.right=rect.left+percent*rect.Width();

    
	
	//根据进度填充矩形
	if((HBITMAP)m_cbForeground != NULL)
	{
		
		HDC memdc = CreateCompatibleDC(dc.GetSafeHdc());
		HBITMAP h = (HBITMAP)SelectObject(memdc, m_cbForeground);
	    BITMAP bm;
		
		m_cbForeground.GetBitmap(&bm);
		
		CRect ForegroundRect;
		ForegroundRect.left = 0;
        ForegroundRect.top = 0;
        ForegroundRect.bottom = bm.bmHeight;
		ForegroundRect.right =ForegroundRect.left + percent * bm.bmWidth; 


         

		
		StretchBlt(dc.GetSafeHdc(), rect.left, rect.top, rect.right , rect.bottom, memdc, ForegroundRect.left, ForegroundRect.top,ForegroundRect.right  ,ForegroundRect.bottom, SRCCOPY);
		
		
		SelectObject(memdc,h);
		DeleteDC(memdc);
		
	}
	else
	{
		dc.FillSolidRect(&rect,RGB(0,255,0));
		
	}// Do not call CProgressCtrl::OnPaint() for painting messages
}

void BitMapProgress::SetForegroundBmp(UINT ForegroundBmpID)
{
	if((HBITMAP)m_cbForeground)
	{
		if(!m_cbForeground.DeleteObject())
		{
			DisplayErrorInfo(string("DeleteObject"));
			
		}
	}

	if(!m_cbForeground.LoadBitmap(ForegroundBmpID))
	{
		DisplayErrorInfo(string("LoadBitmap"));
		
	}

	


}

void BitMapProgress::SetBackColor(COLORREF BackColor)
{
	m_BackColor = BackColor;

}



void BitMapProgress::OnNcPaint() 
{
	// TODO: Add your message handler code here
	
	// Do not call CProgressCtrl::OnNcPaint() for painting messages
}


