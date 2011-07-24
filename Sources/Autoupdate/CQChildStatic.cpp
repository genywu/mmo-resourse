// CQChildStatic.cpp: implementation of the CCQChildStatic class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "autoupdate.h"
#include "CQChildStatic.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCQChildStatic::CCQChildStatic(CDC * ParentDevice,CRect& Range )
:m_ParentDC(ParentDevice)
,m_Range(Range)
{
	


}

CCQChildStatic::~CCQChildStatic()
{

}

void CCQChildStatic::SetText(int x, int y, CString text,COLORREF TextColor)
{
	


	if(m_ParentDC !=NULL)
	{
		
		int  OldMode = m_ParentDC->SetBkMode(TRANSPARENT);
		if(OldMode == 0 )
		{
			DisplayErrorInfo(string("SetBkMode"));
		}
		
		COLORREF OldColor = m_ParentDC->SetTextColor(TextColor);
		if(OldColor == CLR_INVALID)
		{
			DisplayErrorInfo(string("SetTextColor"));
			
		}
		
		if(!m_ParentDC->TextOut(m_Range.left +x ,m_Range.top + y,text,text.GetLength()))
		{
			DisplayErrorInfo(string("TextOut"));
			
			
		}
		
		if(!m_ParentDC->SetBkMode(OldMode))
		{
			DisplayErrorInfo(string("SetBkMode"));
			
		}
		
		if(m_ParentDC->SetTextColor(OldColor) == CLR_INVALID)
		{
			DisplayErrorInfo(string("SetTextColor"));
			
		}
	}
	
}

CCQChildStatic::CCQChildStatic()
:m_ParentDC(NULL)

{

}
