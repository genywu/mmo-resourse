#include "StdAfx.h"
#include "progressbar.h"

#pragma warning( disable : 4312)  //type cast warring

ProgressBar::ProgressBar(void)
{
}

ProgressBar::~ProgressBar(void)
{
}


void ProgressBar::Begin(HWND hParentWnd,COLORREF color,int iRange)
{
    RECT rcClient;  // client area of parent window 
    int cyVScroll;  // height of scroll bar arrow 
   
    // Ensure that the common control DLL is loaded and create a 
    // progress bar along the bottom of the client area of the 
    // parent window. Base the height of the progress bar on 
    // the height of a scroll bar arrow. 

	//get INSTANCE 
    HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hParentWnd, GWL_HINSTANCE );

    InitCommonControls(); 
    GetClientRect(hParentWnd, &rcClient); 
    cyVScroll = GetSystemMetrics(SM_CYVSCROLL); 

    m_hWndPB = CreateWindowEx(0, PROGRESS_CLASS, (LPSTR) NULL, 
        WS_CHILD | WS_VISIBLE , rcClient.left, 
        rcClient.bottom - cyVScroll, 
        rcClient.right, cyVScroll, 
        hParentWnd, (HMENU) 0, hInstance, NULL); 

	SendMessage(m_hWndPB,PBM_SETBARCOLOR,0,(LPARAM)(COLORREF)color);

	// Set the range and increment of the progress bar. 
    SendMessage(m_hWndPB, PBM_SETRANGE, 0, MAKELPARAM(0, iRange)); 
    SendMessage(m_hWndPB, PBM_SETSTEP, (WPARAM) 1, 0); 
}

void ProgressBar::StepIt(void)
{
	SendMessage(m_hWndPB, PBM_STEPIT, 0, 0); 
}

void ProgressBar::End(void)
{
	DestroyWindow(m_hWndPB); 
}
