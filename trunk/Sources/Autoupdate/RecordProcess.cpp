// RecordProcess.cpp: implementation of the RecordProcess class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RecordProcess.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RecordProcess::RecordProcess()
{

}

RecordProcess::~RecordProcess()
{

}

void RecordProcess::AddProcessFunction(ItermProcess * Function)
{
	if(FindFunction(Function->GetType()) == m_FunctionSet.end())
	{
		m_FunctionSet.push_back(Function);

	}

	



}

ItermProcessSet::iterator RecordProcess::FindFunction(const string &Type)
{
	ItermProcessSet::iterator Pointer;

/*	for(Pointer = m_FunctionSet.begin();Pointer!=m_FunctionSet.end();Pointer++)
	{
		if((* Pointer)->GetType() == Type)
		{
			return Pointer;
		}

	}
*/
    Pointer = find_if(m_FunctionSet.begin(),m_FunctionSet.end(),ItermProcessTypeCheck(Type));

	return Pointer;
    
	

}

void RecordProcess::RemoveProcess(const string &Type)
{
	ItermProcessSet::iterator Position = FindFunction(Type);
	if(Position != m_FunctionSet.end())
	{
		m_FunctionSet.erase(Position);

	}

}

void RecordProcess::Reset()
{
	
	ItermProcessSet::iterator Pointer;

	for(Pointer = m_FunctionSet.begin();Pointer!=m_FunctionSet.end();Pointer++)
	{
		(*Pointer)->Reset();
		

	}




}

void RecordProcess::AnalyseRecordStream(istream &RecordStream)
{
	while(!RecordStream.eof())
	{
		AnalyseARecord(RecordStream);

	}


    

	

}

void RecordProcess::AnalyseARecord(istream &RecordStream)
{
	string Title;
	GetALineFromStream(Title,RecordStream);
	ItermProcessSet::iterator Pointer;

	for(Pointer = m_FunctionSet.begin();Pointer!=m_FunctionSet.end();Pointer++)
	{
		if((* Pointer)->ProcessIterm(Title,RecordStream))
		{
			break;
		}
		

	}

}
