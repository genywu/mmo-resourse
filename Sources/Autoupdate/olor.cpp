// olor.cpp: implementation of the Color class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "olor.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Color::Color(int red,int green,int blue)
:m_red(red)
,m_green(green)
,m_blue(blue)
{
	RGBtoHSB(red,green,blue,&m_H,&m_S,&m_B);
	
}

Color::~Color()
{
	
}

void Color::RGBtoHSB(int red, int green, int blue, float *H, float *S, float *B)
{
	int largestColor; /* holds the largest color (rgb) at the start */
	int lowestColor; /* opposite of above */
	float hue; /* it puts the `h' in "hsb" */
	float saturation; /* `s' */
	float brightness; /* and the `b' */
	
	/* Make sure that the array that is returned actually has space */
	
    
	/* assign largestColor to the greater of the red or green */
	if (red <= green)
		largestColor = green;
	else
		largestColor = red;
    
	/* now see if blue is bigger */
	if (blue > largestColor)
		largestColor = blue;
	
	/* set lowestColor = to the smallest value */
	if (green < red)
		lowestColor = green;
	else
		lowestColor = red;
	
	if (blue < lowestColor)
		lowestColor = blue;
	
	/* brightness is calculated like so. */  
	brightness = (float) largestColor / 255;
	
	/* if the largestColor isn't zero (so we don't divide by zero) set the variable *
	* to the difference of the two as a percentage of the largest                  */
	if (largestColor != 0)
		saturation = (float) (largestColor - lowestColor) / (float) largestColor;
	else
		saturation = 0.0F;
	
	if (saturation == 0.0F) {
		hue = 0.0F;
	} else {
		/* some temporary variables to figure out the hue */
		float redRatio = (float) (largestColor - red) / (float) (largestColor - lowestColor);
		float greenRatio = (float) (largestColor - green) / (float) (largestColor - lowestColor);
		float blueRatio = (float) (largestColor - blue) / (float) (largestColor - lowestColor);
		
		/* depending on which of the 3 was the highest, we calculate our hue. */
		if (red == largestColor)
			hue = blueRatio - greenRatio;
		else if (green == largestColor)
			hue = (2.0F + redRatio) - blueRatio;
		else /* blue is largest */
			hue = (4.0F + greenRatio) - redRatio;
		
		/* divide it by 6 */
		hue /= 6.0F;
		
		/* I don't know if this prevents us from being negative (as in the worst outcome from the above *
		* operations is -1, or if this does something different).                                      */
		if (hue < 0.0F)
			hue++;
	}
	
	* H = hue;
	* S = saturation;
	* B = brightness;
	
	
	
}

float Color::GetH()
{
	return m_H;

}

float Color::GetS()
{
	return m_S;


}

float Color::GetB()
{
	return m_B;


}

int Color::GetRed()
{
	return m_red;

}

int Color::GetGreen()
{

	return m_green;
}

int Color::GetBlue()
{

	return m_blue;
}
