// HSV.h: interface for the CHSV class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HSV_H__26662803_F825_4869_9C3F_3E097EA27384__INCLUDED_)
#define AFX_HSV_H__26662803_F825_4869_9C3F_3E097EA27384__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CHSV  
{
public:
	float GetH();
	float GetV();
	float GetS();
	CHSV(float R,float G,float B);
	virtual ~CHSV();
 	float m_H;
    float m_S;
    float m_V;


};

#endif // !defined(AFX_HSV_H__26662803_F825_4869_9C3F_3E097EA27384__INCLUDED_)
