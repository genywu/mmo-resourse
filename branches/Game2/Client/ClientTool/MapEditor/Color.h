#pragma once

#include <d3d9.h>
#include <d3dx9.h>

class Color
{
	float m_fRed;
	float m_fGreen;
	float m_fBlue;

public:

	inline Color operator+= (const Color &color)
	{
		m_fRed   += color.m_fRed;
		m_fGreen += color.m_fGreen;
		m_fBlue  += color.m_fBlue;

		return (*this);
	}

	inline Color operator *= (const Color &color)
	{
		m_fRed   *= color.m_fRed;
		m_fGreen *= color.m_fGreen;
		m_fBlue  *= color.m_fBlue;

		return (*this);
	}

	inline Color operator-= (const Color &color)
	{
		m_fRed   -= color.m_fRed;
		m_fGreen -= color.m_fGreen;
		m_fBlue  -= color.m_fBlue;

		return (*this);
	}

	inline Color operator* (float value)
	{
		return Color( m_fRed * value, m_fGreen * value, m_fBlue * value);
	}	         
	
	inline Color operator* (const Color &color)
	{
		return Color( m_fRed * color.m_fRed, m_fGreen * color.m_fGreen, m_fBlue * color.m_fBlue );
	}

	Color(float r,float g,float b);

	Color(DWORD color);

	inline void Set(float r,float g, float b) { m_fRed = r;m_fGreen = g;m_fBlue = b;}
	void Set(DWORD color);
	
	Color(void);
	~Color(void);

	DWORD GetColor(void); 
	void Fix(void);
};
