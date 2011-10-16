#include "StdAfx.h"
#include "Roads.h"

CRoads::CRoads(void)
{
	m_nNo=0;
}
CRoads::CRoads(int n,CRect rect)
{
	m_nNo=n;

}
CRoads::~CRoads(void)
{
	m_vecRoads.clear();
}

ifstream &operator>>(ifstream &rstream, CRoads &rRoads)
{
	rRoads.m_vecRoads.clear();
	rstream>>rRoads.m_nNo;
	string str;
	int i=1;
	while (1)
	{
		rstream>>str;
		if (str=="<end>")
		{
			break;
		}
		CRoads::tagRoadsPoint roadspoint;
		roadspoint.npointNo=i;
		rstream>>roadspoint.nx
			>>roadspoint.ny;
		rRoads.m_vecRoads.push_back(roadspoint);
		i++;
	}
	return rstream;	
}

ofstream &operator<<(ofstream &rstream, CRoads *rRoads)
{

	int u=(int)rRoads->m_vecRoads.size();
	for (int i=0;i<u;i++)
	{
		rstream<<">	\t"
			<<rRoads->m_vecRoads[u].nx
			<<"\t"
			<<rRoads->m_vecRoads[u].ny
			<<"\n"
			<<"<end>";
	}
	return rstream;
}