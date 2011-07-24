// DefaultProcess.cpp: implementation of the DefaultProcess class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DefaultProcess.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const string DefaultProcessID = "DefaultProcess";

DefaultProcess::DefaultProcess()
{

}

DefaultProcess::~DefaultProcess()
{

}

void DefaultProcess::Reset()
{
	m_TextInfoBuffer.clear();

}

bool DefaultProcess::ProcessIterm(const string &sType, istream &DataStream)
{
	
	m_TextInfoBuffer << sType << endl;
	string Elem;
	
	if(!DataStream.eof())
	{	
		//throw exception("Stream End when UpdateRelativeFileProcess::ProcessIterm ; File:__FILE__;Line:__LINE__"");
		GetALineFromStream(Elem,DataStream);
	}
	
	while(!CheckIsEnd(Elem))
	{
		
		m_TextInfoBuffer<< Elem <<endl;
		Elem = string("");
		
		if(!DataStream.eof())
		{	
			//throw exception("Stream End when UpdateRelativeFileProcess::ProcessIterm ; File:__FILE__;Line:__LINE__"");
			GetALineFromStream(Elem,DataStream);
		}
		else
		{
			break;
		}
		
		
	}

	m_TextInfoBuffer << Elem;
	m_TextInfoBuffer << endl;

		

	



	


	
	


	return true;
	
	
	


}

const string DefaultProcess::GetType() const
{
	return DefaultProcessID;

}

strstream& DefaultProcess::GetTextBuffer()
{

	return m_TextInfoBuffer;

}
