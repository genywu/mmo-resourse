/**
 @file image_utility.h
 @author Kevin Lynx
 @brief provide some utility functions to render gdi+ images 
*/
#ifndef ___IMAGE_UTILITY_H_
#define ___IMAGE_UTILITY_H_

#include <comdef.h>
#include <gdiplus.h>

/**
  Render an image with alpha blending function.
  
  @param alpha [0-255]
*/
void img_alpha( Gdiplus::Graphics *g, Gdiplus::Image *src, float x, float y, int alpha );

/**
  Rotate the image and render it at the (x,y) position.

  @param centerX the center x of the rotation
  @param centerY the center y of the rotation
  @param angel rotate in angle, in degrees
  @param alpha [0-255]
*/
void img_rotate( Gdiplus::Graphics *g, Gdiplus::Image *src, float x, float y, 
				float centerX, float centerY, float angle, int alpha );

#endif