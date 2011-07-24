// lientGameOptionProcess.cpp: implementation of the ClientGameOptionProcess class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "lientGameOptionProcess.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


ClientGameOptionProcess::ClientGameOptionProcess()
{

}

ClientGameOptionProcess::~ClientGameOptionProcess()
{

}

const string ClientGameOptionProcess::GetType() const
{
	return ClientGameOptionID;

}

bool ClientGameOptionProcess::ProcessIterm(const string &sType, istream &DataStream)
{
	
	if(sType == ClientGameOptionID)
	{
		m_OptionS.push_back(sType);
		
		
		string Elem;
		
		if(!DataStream.eof())
		{	
			//throw exception("Stream End when UpdateRelativeFileProcess::ProcessIterm ; File:__FILE__;Line:__LINE__"");
			GetALineFromStream(Elem,DataStream);
		}
		
		while(!CheckIsEnd(Elem))
		{
			
			m_OptionS.push_back(Elem); 
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
	
		return true;
		
		
		
	}
	else
	{
		return false;
	}
	
	
	
	

}

void ClientGameOptionProcess::Reset()
{
	m_OptionS.clear();



}

const TextSet ClientGameOptionProcess::GetOptionS()
{
	return m_OptionS;


}
