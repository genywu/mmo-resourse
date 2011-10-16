#include "stdafx.h"
#include "color.h"

Color::Color(void):
m_fRed(0),
m_fGreen(0),
m_fBlue(0)
{
}

Color::Color(float r,float g,float b)
{
	m_fRed = r;
	m_fGreen = g;
	m_fBlue = b;
}

Color::Color(DWORD color)
{
	m_fRed   = (float)((color & 0x00FF0000) >> 16) / 255.0f;
	m_fGreen = (float)((color & 0x0000FF00) >> 8) / 255.0f;
	m_fBlue  = (float)(color & 0x000000FF) / 255.0f;
}

void Color::Set(DWORD color)
{
	m_fRed   = (float)((color & 0x00FF0000) >> 16) * (1.0f / 255.0f);
	m_fGreen = (float)((color & 0x0000FF00) >> 8)  * (1.0f / 255.0f);
	m_fBlue  = (float)(color & 0x000000FF) * (1.0f / 255.0f);
}

Color::~Color(void)
{
}

void Color::Fix(void)
{					  
	if (m_fRed >1.0f)	m_fRed = 1.0f;
	if (m_fGreen >1.0f)	m_fGreen = 1.0f;
	if (m_fBlue >1.0f)	m_fBlue = 1.0f;
	if (m_fRed <0.0f)   m_fRed = 0.0f;
	if (m_fGreen <0.0f) m_fGreen = 0.0f;
	if (m_fBlue <0.0f)  m_fBlue = 0.0f;
}

DWORD Color::GetColor(void) 
{
	 return D3DCOLOR_ARGB(0xff,(int)(m_fRed * 255.0f),(int)(m_fGreen * 255.0f),(int)(m_fBlue * 255.0f));
}