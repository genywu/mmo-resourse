#include "StdAfx.h"
#include "SignPos.h"
#include <iostream>
CSignPos::CSignPos(void)
{
	m_nNo=0;
	m_nxPos=0;
	m_nyPos=0;
}
CSignPos::CSignPos(int nNo,CRect rect)
{
	m_nNo=nNo;
	m_nxPos=rect.left;
	m_nyPos=rect.top;
}
CSignPos::~CSignPos(void)
{
}
//È¡XºÍY×ø±ê
string GetPos(string str)
{
	int i,j=0;
	char strVal[10];
	for(i=0;str[i]!='\0';i++)
	{
		if('"'==str[i])
		{
			for(j=0;j<10;j++,i++)
			{
				if(str[i+1]!='"')
				strVal[j]=str[i+1];
				else
					break;
			}
			strVal[j+1]='\0';
			return strVal;
		}
		
	}
	return NULL;
}
ifstream & operator>>(ifstream &rstream, CSignPos &rsignpos)
{
	string str,strTemp;
	rstream>>str>>strTemp;
	rsignpos.m_nxPos=atoi(GetPos(str).c_str());
	rsignpos.m_nyPos=atoi(GetPos(strTemp).c_str());
	return rstream;
}

ofstream & operator<<(ofstream &rstream, CSignPos &rsignpos)
{
	
	return rstream;
}