// ItermProcessTypeCheck.h: interface for the ItermProcessTypeCheck class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITERMPROCESSTYPECHECK_H__253AD217_5CF7_4D4E_9778_1CBDE5673F2C__INCLUDED_)
#define AFX_ITERMPROCESSTYPECHECK_H__253AD217_5CF7_4D4E_9778_1CBDE5673F2C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include"itermProcess.h"

class ItermProcessTypeCheck  
{
public:
	ItermProcessTypeCheck(string Type);
	virtual ~ItermProcessTypeCheck();
	bool  operator()(const ItermProcess* Source) const
	{
		return (Source->GetType() == m_value) ;
	}
private:
	string m_value;

};

#endif // !defined(AFX_ITERMPROCESSTYPECHECK_H__253AD217_5CF7_4D4E_9778_1CBDE5673F2C__INCLUDED_)
