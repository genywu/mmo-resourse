// lientGameOptionProcess.h: interface for the ClientGameOptionProcess class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LIENTGAMEOPTIONPROCESS_H__E8259479_CCB9_4A98_A452_0F4875415EF0__INCLUDED_)
#define AFX_LIENTGAMEOPTIONPROCESS_H__E8259479_CCB9_4A98_A452_0F4875415EF0__INCLUDED_

#pragma warning(disable:4786)


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include<vector>
#include<string>
#include"ItermProcess.h"

using namespace std;
const string ClientGameOptionID = "[Client]";

typedef vector<string> TextSet; 
class ClientGameOptionProcess : public ItermProcess  
{
public:
	const TextSet GetOptionS();
	void Reset();
	bool ProcessIterm(const string& sType, istream& DataStream);
	const string GetType() const;
	ClientGameOptionProcess();
	virtual ~ClientGameOptionProcess();
private:
	TextSet m_OptionS;
    
	

};

#endif // !defined(AFX_LIENTGAMEOPTIONPROCESS_H__E8259479_CCB9_4A98_A452_0F4875415EF0__INCLUDED_)
