// CQChildStatic.h: interface for the CCQChildStatic class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CQCHILDSTATIC_H__371AE85C_B0A1_47A6_A77B_A342ECE58D8A__INCLUDED_)
#define AFX_CQCHILDSTATIC_H__371AE85C_B0A1_47A6_A77B_A342ECE58D8A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include<string>
using namespace std;
void DisplayErrorInfo(string& ErrorInfo);
class CCQChildStatic  
{
public:
	CCQChildStatic();
	void SetText(int x,int y, CString text,COLORREF TextColor);
	CCQChildStatic(CDC * ParentDevice,CRect& Range);
	virtual ~CCQChildStatic();
private:
	CDC * m_ParentDC;
	CRect m_Range;

	
};

#endif // !defined(AFX_CQCHILDSTATIC_H__371AE85C_B0A1_47A6_A77B_A342ECE58D8A__INCLUDED_)
