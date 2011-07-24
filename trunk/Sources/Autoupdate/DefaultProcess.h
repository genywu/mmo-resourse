// DefaultProcess.h: interface for the DefaultProcess class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEFAULTPROCESS_H__2B739F02_B049_4908_8F8E_1682612BA77A__INCLUDED_)
#define AFX_DEFAULTPROCESS_H__2B739F02_B049_4908_8F8E_1682612BA77A__INCLUDED_

#pragma warning(disable:4786)

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ItermProcess.h"
#include<strstream>

using namespace std;

class DefaultProcess : public ItermProcess  
{
public:
	strstream& GetTextBuffer();
	const string GetType() const;
	bool ProcessIterm(const string& sType, istream& DataStream);
	void Reset();
	DefaultProcess();
	virtual ~DefaultProcess();

private:
	strstream m_TextInfoBuffer;
};

#endif // !defined(AFX_DEFAULTPROCESS_H__2B739F02_B049_4908_8F8E_1682612BA77A__INCLUDED_)
