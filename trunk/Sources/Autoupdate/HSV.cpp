// HSV.cpp: implementation of the CHSV class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HSV.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHSV::CHSV(float R,float G,float B)
{
/*	float max  = MAX(R,MAX(G,B)),min = MIN(R,MIN(G,B));
	float delta =max - min;
	m_V =max;
	if(max = 0.0 )
	{
		m_S = delta/max;
	}
	else
	{
		m_S = 0.0;
	}
	if()
*/
}

CHSV::~CHSV()
{

}

float CHSV::GetH()
{
	return m_H;

}
float CHSV::GetV()
{
	return m_V;

}

float CHSV::GetS()
{
	return m_S;

}

