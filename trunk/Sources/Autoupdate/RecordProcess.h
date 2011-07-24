// RecordProcess.h: interface for the RecordProcess class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RECORDPROCESS_H__E16E38FE_1A3D_4DD6_9FE6_76B234F53415__INCLUDED_)
#define AFX_RECORDPROCESS_H__E16E38FE_1A3D_4DD6_9FE6_76B234F53415__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#pragma warning(disable:4786)

#include"ItermProcess.h"
#include"itermProcessTypeCheck.h"
#include<vector>
#include<algorithm>
using namespace std;

typedef vector<ItermProcess *> ItermProcessSet;

class RecordProcess  
{
public:
	void AnalyseRecordStream(istream& RecordStream);
	void RemoveProcess(const string& Type);
	void AddProcessFunction(ItermProcess *  Function);
	RecordProcess();
	virtual ~RecordProcess();
	void Reset();
private:
	void AnalyseARecord(istream &RecordStream);
	

	ItermProcessSet m_FunctionSet;
	ItermProcessSet::iterator FindFunction(const string& Type);
    
	

};

#endif // !defined(AFX_RECORDPROCESS_H__E16E38FE_1A3D_4DD6_9FE6_76B234F53415__INCLUDED_)
