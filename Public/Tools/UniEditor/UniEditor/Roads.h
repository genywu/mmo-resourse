#pragma once
#include "info.h"

class CRoads
{
public:
	CRoads(void);
	CRoads(int n,CRect rect);
	~CRoads(void);
	struct  tagRoadsPoint
	{
		int npointNo;
		int nx;
		int ny;
	};
	friend ifstream &operator>>(ifstream &rstream, CRoads &rRoads);
	friend ofstream &operator<<(ofstream &rstream, CRoads *rRoads);
	int GetNo()				{return m_nNo;}
	//void SetxPos(int xpos)			{m_nxPos=xpos;}
	//void SetyPos(int ypos)			{m_nyPos=ypos;}
	vector<CRoads::tagRoadsPoint> &GetRoadsPoints()	{return m_vecRoads;}
	
public:

	
	int m_nNo;
	vector<tagRoadsPoint> m_vecRoads;
};
