// ItermProcess.h: interface for the ItermProcess class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITERMPROCESS_H__E80889D6_7CDD_44E4_BB53_FCF879196D0C__INCLUDED_)
#define AFX_ITERMPROCESS_H__E80889D6_7CDD_44E4_BB53_FCF879196D0C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include<string>

using namespace std;
const string UpdateRelativeFile = "[UpdateRelativeFile]";


void GetALineFromStream(string &LineString, istream &Datastream);



class ItermProcess  
{
public:
	virtual void Reset() = 0;
	virtual const string GetType() const =  0;
	
	virtual bool ProcessIterm(const string& sType,istream& DataStream) = 0;
	ItermProcess();
	virtual ~ItermProcess();
protected:
	virtual bool CheckIsEnd(const string& Sample);

};

#endif // !defined(AFX_ITERMPROCESS_H__E80889D6_7CDD_44E4_BB53_FCF879196D0C__INCLUDED_)
