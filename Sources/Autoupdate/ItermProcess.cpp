// ItermProcess.cpp: implementation of the ItermProcess class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ItermProcess.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
void GetALineFromStream(string &LineString, istream &Datastream)
{
	char Buffer;
	

	if(!Datastream.eof())
	{
		Datastream.get(Buffer);

	}
	else
	{
		return;
	}
	
	if(Datastream.bad())
	{
		throw exception("GetALineFromStream Error when iostream Get get(Buffer); File:__FILE__;Line:__LINE__");
	}
	
	while(Buffer!='\n')
	{
		
		
     	
	    if(!Datastream.eof())
		{
			LineString += Buffer;
			Datastream.get(Buffer);
			
		
		}
		else
		{
			break;
		}
		
		if(Datastream.bad())
		{
			throw exception("GetALineFromStream Error when iostream Get get(Buffer); File:__FILE__;Line:__LINE__");
		}
		 
	}

}
ItermProcess::ItermProcess()
{

}

ItermProcess::~ItermProcess()
{

}

bool ItermProcess::CheckIsEnd(const string &Sample)
{
	string EndLineSample = " ";  
	int Pos = Sample.find_first_not_of(EndLineSample); 
	
	if(Pos != string::npos)
	{
		return false;
	}
	else
	{
		return true;
	}


}

