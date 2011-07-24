// olor.h: interface for the Color class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OLOR_H__7B9C7AE3_C923_40D8_8CEE_9D843937466B__INCLUDED_)
#define AFX_OLOR_H__7B9C7AE3_C923_40D8_8CEE_9D843937466B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Color  
{
public:
	int GetBlue();
	int GetGreen();
	int GetRed();
	float GetB();
	float GetS();
	float GetH();
	Color(int red,int green,int blue);
	virtual ~Color();
private:
	void RGBtoHSB(int red, int green, int blue, float * H,float * S ,float * B);
	
	int m_red;
	int m_green;
	int m_blue;
	float m_H;
    float m_S;
    float m_B;


    


};

#endif // !defined(AFX_OLOR_H__7B9C7AE3_C923_40D8_8CEE_9D843937466B__INCLUDED_)
