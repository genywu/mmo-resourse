/**
 @file image_utility.cpp
 @author Kevin Lynx
 @brief provide some utility functions to render gdi+ images 
*/
#include "image_utility.h"

static void GetAlphaImageAttr( Gdiplus::ImageAttributes *ImgAttr, int alpha )
{
	Gdiplus::ColorMatrix colorMatrix = {
		1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, alpha / 255.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 1.0f
	};
	ImgAttr->SetColorMatrices( &colorMatrix, NULL );
}

void img_alpha( Gdiplus::Graphics *g, Gdiplus::Image *src, float x, float y, int alpha )
{
	Gdiplus::ImageAttributes ImgAttr;
	GetAlphaImageAttr( &ImgAttr, alpha );
	g->DrawImage( src, Gdiplus::RectF( x, y, (float)src->GetWidth(), (float)src->GetHeight() ), 
		0.0f, 0.0f, (float)src->GetWidth(), (float)src->GetHeight(), Gdiplus::UnitPixel, &ImgAttr );
}

/*
  Actually, to rotate an image, and render it at the specified position, we should
  rotate it at the origin and translate it then. Finally we render it at (0,0).
  Whatever, these codes below work correctly.
*/
void img_rotate( Gdiplus::Graphics *g, Gdiplus::Image *src, float x, float y, 
				float centerX, float centerY, float angle, int alpha )
{
	Gdiplus::Matrix m;
	g->GetTransform( &m );
	Gdiplus::Matrix tm( 1, 0, 0, 1, 1, 1 );
	tm.RotateAt( angle, Gdiplus::PointF( centerX, centerY ) );
	g->SetTransform( &tm );
	g->TranslateTransform( x, y, Gdiplus::MatrixOrderAppend );
	if( alpha == 255 )
	{
		g->DrawImage( src, 0, 0 );
	}
	else
	{
		Gdiplus::ImageAttributes ImgAttr;
		GetAlphaImageAttr( &ImgAttr, alpha );
		g->DrawImage( src, Gdiplus::RectF( 0, 0, (float)src->GetWidth(), (float)src->GetHeight() ), 
			0.0f, 0.0f, (float)src->GetWidth(), (float)src->GetHeight(), Gdiplus::UnitPixel, &ImgAttr ); 
	}
	g->SetTransform( &m );
}