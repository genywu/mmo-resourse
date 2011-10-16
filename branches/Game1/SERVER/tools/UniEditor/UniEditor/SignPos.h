#pragma once
#include "info.h"
class CSignPos : public CInfo
{
public:
	CSignPos(void);
	CSignPos(int nNo,CRect rect);
	~CSignPos(void);
public:
	friend ifstream &operator>>(ifstream &rstream, CSignPos &rsignpos);
	friend ofstream &operator<<(ofstream &rstream, const CSignPos &rrsignpos);
	void SetNo(int no)				{m_nNo=no;}
	void SetxPos(int xpos)			{m_nxPos=xpos;}
	void SetyPos(int ypos)			{m_nyPos=ypos;}
	int GetNo()						{return m_nNo;}
	int GetxPos()					{return m_nxPos;}
	int GetyPos()					{return m_nyPos;}
	int m_nNo;
	int m_nxPos;
	int m_nyPos;
};
