///
/// @file QuestionGen.cpp
/// @author Kevin Lynx
/// @brief generate question pictures in AntiCheat module.
///
#include "stdafx.h"
#include "QuestionManager.h"
#include "FileListManager.h"
#include "AntiCheatCommon.h"
#include "CConfiger.h"

#ifdef _USE_GDIPLUS
#include "img_utility/mem_image.h"
#include "img_utility/image_utility.h"

#pragma comment( lib, "gdiplus.lib" )


namespace AntiCheat
{
	/**
	  Create an image from buffer, must *delete* it manually.
	*/
	static Gdiplus::Image *CreateImgFromBuffer( const CFileListManager::Buffer &buffer )
	{
		if( buffer.buf == 0 )
		{
			// fatal error
			return NULL;
		}
		Gdiplus::Image *img = mi_from_memory( buffer.buf, buffer.size );
		return img;
	}

	/**
	  Split a rect into four rects averagely.
	*/
	static void SplitRectToFour( const Gdiplus::RectF &rect, Gdiplus::RectF *ret_rect )
	{
		float centerX = rect.Width / 2;
		float centerY = rect.Height / 2;
		ret_rect[0].X = 0;
		ret_rect[0].Y = 0;
		ret_rect[0].Width = centerX;
		ret_rect[0].Height = centerY;

		ret_rect[1].X = centerX;
		ret_rect[1].Y = 0;
		ret_rect[1].Width =	centerX;
		ret_rect[1].Height = centerY;

		ret_rect[3].X = centerX;
		ret_rect[3].Y = centerY;
		ret_rect[3].Width = centerX;
		ret_rect[3].Height = centerY;

		ret_rect[2].X = 0;
		ret_rect[2].Y = centerY;
		ret_rect[2].Width = centerX;
		ret_rect[2].Height = centerY;
	}

	/**
	  Get a random position in the upper-left rect.
	*/
	static Gdiplus::PointF GetRandPosInULRect( Gdiplus::REAL w, Gdiplus::REAL h, const Gdiplus::RectF &rect )
	{
		Gdiplus::PointF ret_p ;
		ret_p.X = rect.Width < w ? rect.X : random( rect.Width - w ) + rect.X;
		ret_p.Y = rect.Height < h ? rect.Y : random( rect.Height - h ) + rect.Y;
		return ret_p;
	}

	/**
	  Blend 'X' sign onto the image.
	*/
	static void BlendXSign( Gdiplus::Graphics *g, const Gdiplus::RectF &render_rect )
	{
		CFileListManager &file_manager = GetInst( CFileListManager );
		Gdiplus::Image *img = CreateImgFromBuffer( file_manager.GetRandX() );
		Gdiplus::PointF render_pos = GetRandPosInULRect( img->GetWidth(), img->GetHeight(), render_rect );
		img_alpha( g, img, render_pos.X, render_pos.Y, 255 );
		delete img;
	}

	/**
	  Blend 'A''B''C''D' sign onto the image.
	*/
	static void BlendSelSign( Gdiplus::Graphics *g, const Gdiplus::RectF &render_rect, int sel )
	{
		CFileListManager &file_manager = GetInst( CFileListManager );
		Gdiplus::Image *img = CreateImgFromBuffer( file_manager.GetSelSign( sel ) );
		Gdiplus::PointF render_pos = GetRandPosInULRect( img->GetWidth(), img->GetHeight(), render_rect );
		img_alpha( g, img, render_pos.X, render_pos.Y, 255 );
		delete img;
	}

	/**
	  generate some small images and render them on the backgroun.
	*/
	static void GenSmallImage( int img_count, Gdiplus::Graphics *g, const Gdiplus::RectF &render_rect, int sel )
	{
#define RAND_ALPHA ( 100 + random( 55 ) )

		if( img_count < 1 )
		{
			return;
		}

		CFileListManager &file_manager = GetInst( CFileListManager );
		// render without alpha value
		Gdiplus::Image *img = CreateImgFromBuffer( file_manager.GetRandSmall() );
		Gdiplus::PointF render_pos = GetRandPosInULRect( img->GetWidth(), img->GetHeight(), render_rect );
		Gdiplus::RectF img_rect( render_pos.X, render_pos.Y, (float)img->GetWidth(), (float)img->GetHeight() );
		img_alpha( g, img, render_pos.X, render_pos.Y, img_count == 1 ? 255 : RAND_ALPHA );
		delete img;

		// render with random alpha value and random rotation degree
		g->SetClip( img_rect );
		for( int i = 1; i < img_count; ++ i )
		{
			img = CreateImgFromBuffer( file_manager.GetRandSmall() );
			img_rotate( g, img, render_pos.X, render_pos.Y, img->GetWidth() / 2, img->GetHeight() / 2,
				random( 360 ), // [0-360)
				RAND_ALPHA ); // [100-155)
			delete img;
		}
		g->ResetClip();

		// blend a 'X' sign onto the picture if it's not the answer
		if( img_count > 1 )
		{
			//BlendXSign( g, img_rect );
		}

		// blend 'A''B''C''D' sign.
		BlendSelSign( g, img_rect, sel );
	}

	/**
	  generate small pictures and draw them on the background. Return the answer which has been generated.
	*/
	static int BlendSmallImage( Gdiplus::Graphics *g, Gdiplus::Image *img_src )
	{
		Gdiplus::RectF area[4];
		SplitRectToFour( Gdiplus::RectF( 0, 0, (float)img_src->GetWidth(), (float)img_src->GetHeight() ), 
			area );
		int answer = random( 4 ) + 1; /* [1,4] */
		for( int i = 1; i <= 4; ++ i )
		{
			if( i == answer )
			{
				// only render a random image only.
				GenSmallImage( 1, g, area[i - 1], i ); 
			}
			else
			{
				// random image count
				GenSmallImage( random( 2 ) + 2, g, area[i - 1], i );
			}
		}
		
		return answer;
	}

// for debug purpose only
#ifdef DUMP_IMG
	void WriteBufToFile( const char *file, const void *buf, size_t size )
	{
		FILE *fp = fopen( file, "wb" );
		if( fp == NULL )
		{
			return ;
		}
		fwrite( buf, size, 1, fp );
		fclose( fp );
	}

	void DumpImage( Gdiplus::Image *img )
	{
		static int s_index = 0;
		void *buf;
		size_t size;
		mi_to_memory( img, &buf, &size );
		char file[512];
		sprintf( file, "anticheat/question%d.jpg", s_index++ );
		WriteBufToFile( file, buf, size );
	}
#else
#define DumpImage( a )
#endif
	CQuestionManager::Question CQuestionManager::GenOneQuestion()
	{
		Question ques = { { 0  }, 0 };
		CFileListManager &file_manager = GetInst( CFileListManager );
		int r = 0;

		// 1.generate the background.
		Gdiplus::Image *img_back = CreateImgFromBuffer( file_manager.GetRandBack() );
		Gdiplus::Graphics g( img_back );
		r = random( 3 );
		for( int i = 0; i < r; ++ i )
		{
			Gdiplus::Image *img_tmp = CreateImgFromBuffer( file_manager.GetRandBack() );
			// alpha-blend on the background 
			img_alpha( &g, img_tmp, 0, 0, random( 156 ) + 100 ); // 40%-100% alpha
			// delete the image
			delete img_tmp;
		}

		// 2.generate small pictures.
		ques.answer = BlendSmallImage( &g, img_back );

		// convert the image to raw buffer
		mi_to_memory( img_back, &ques.data.buf, &ques.data.size );
		DumpImage( img_back );
		delete img_back;
		return ques;
	}
}

#else

#include "CImage.h"

namespace AntiCheat
{
	struct RectF
	{
		float X, Y, Width, Height;

		RectF( float x, float y, float w, float h )
		{
			this->X = x; this->Y = y; this->Width = w; this->Height = h;
		}

		RectF()
		{
			memset( this, 0, sizeof( *this ) );
		}
	};

	struct PointF
	{
		float X, Y;
	};

#define GETA( c ) (DWORD)( ( c & 0xff000000 ) >> 24 )

	static CImage &CreateImgFromBuffer( const CFileListManager::Buffer &buf, CImage &img, bool bCheck = false )
	{
		if( !img.ReadBitmapFromMemory( (const BYTE*) buf.buf, buf.size ) )
		{
#ifdef RECORD_NAME
			PutoutLog( LOG_FILE, LT_ERROR, "Create BMP from memory failed : %s", buf.name.c_str() );
#else
			PutoutLog( LOG_FILE, LT_ERROR, "Create BMP from memory failed." );
#endif
			return img;
		}
		// check the image format
		if( bCheck && GETA( img.GetPixel( 0, 0 ) ) == 0 )
		{
#ifdef RECORD_NAME
			PutoutLog( LOG_FILE, LT_ERROR, "Alpha value is invalid : %s", buf.name.c_str() );
#else
			PutoutLog( LOG_FILE, LT_ERROR, "Alpha value is invalid." );
#endif
		}
		return img;
	}

	static void SplitRectToFour( const RectF &rect, RectF *ret_rect )
	{
		float centerX = rect.Width / 2;
		float centerY = rect.Height / 2;
		ret_rect[0].X = 0;
		ret_rect[0].Y = 0;
		ret_rect[0].Width = centerX;
		ret_rect[0].Height = centerY;

		ret_rect[1].X = centerX;
		ret_rect[1].Y = 0;
		ret_rect[1].Width =	centerX;
		ret_rect[1].Height = centerY;

		ret_rect[3].X = centerX;
		ret_rect[3].Y = centerY;
		ret_rect[3].Width = centerX;
		ret_rect[3].Height = centerY;

		ret_rect[2].X = 0;
		ret_rect[2].Y = centerY;
		ret_rect[2].Width = centerX;
		ret_rect[2].Height = centerY;
	}

	static PointF GetRandPosInULRect( float w, float h, const RectF &rect )
	{
		PointF ret_p ;
		ret_p.X = rect.Width < w ? rect.X : random( rect.Width - w ) + rect.X;
		ret_p.Y = rect.Height < h ? rect.Y : random( rect.Height - h ) + rect.Y;
		return ret_p;
	}

	static void BlendSelSign( CImage &back, const RectF &render_rect, int sel )
	{
		CFileListManager &file_manager = GetInst( CFileListManager );
		CImage img;
		CreateImgFromBuffer( file_manager.GetSelSign( sel ), img );
		PointF render_pos = GetRandPosInULRect( img.GetWidth(), img.GetHeight(), render_rect );
		back.AlphaBlend( &img, render_pos.X, render_pos.Y, 1.0f );
	}

	static PointF GetAlphaImgBlendPos( CImage &dest, CImage &alpha )
	{
		PointF pos;
		if( dest.GetWidth() <= alpha.GetWidth() || 
			dest.GetHeight() <= alpha.GetHeight() )
		{
			PutoutLog( LOG_FILE, LT_WARNING, "Alpha image's size is invalid.[%u,%u].",
				alpha.GetWidth(), alpha.GetHeight() );
			pos.X = pos.Y = 0;
		}
		else
		{
			pos.X = random( dest.GetWidth() - alpha.GetWidth() + 1 );
			pos.Y = random( dest.GetHeight() - alpha.GetHeight() + 1 );
		}
		return pos;
	}

	static PointF GetRotPos( CImage &alpha, const PointF &pos )
	{
		PointF ret = {
			pos.X + alpha.GetWidth() / 2,
			pos.Y + alpha.GetHeight() / 2 
		};
		return ret;
	}

	static PointF RotateImage( CImage &img, CImage &img_alpha, float angel )
	{
		PointF pos = GetAlphaImgBlendPos( img, img_alpha );
		PointF rot_pos = GetRotPos( img_alpha, pos );
		img.RotateEx( angel, (long)rot_pos.X, (long)rot_pos.Y );
		img.SetAlphaMap( &img_alpha, (long)pos.X, (long)pos.Y );
		return pos;
	}

	static void GenSmallImage( int img_count, CImage &back, const RectF &render_rect, int sel, CImage &img_alpha )
	{
#define RAND_ALPHA ( ( 130 + random( 50 ) ) / 255.0f )
#define ROT_ANGLE ( random( 20 ) + 160 )
		if( img_count < 1 )
		{
			return ;
		}

		CFileListManager &file_manager = GetInst( CFileListManager );

		const Range &alpha_r = GetInst( CConfiger ).GetSystem().AnswerAlphaRange;
		float fAnswerAlpha = ( random( alpha_r.second - alpha_r.first ) + alpha_r.first ) / 255.0f;
		CImage img_first;
		CreateImgFromBuffer( file_manager.GetRandSmall(), img_first, true );
		float fFirstRot = ROT_ANGLE;
		float fFirstAlpha = RAND_ALPHA;
		PointF pos = RotateImage( img_first, img_alpha, img_count == 1 ? ( random( 60 ) - 30.0f ) : fFirstRot );
		PointF render_pos = GetRandPosInULRect( img_first.GetWidth(), img_first.GetHeight(), render_rect );
		RectF img_rect( render_pos.X + 20, render_pos.Y + 20, img_first.GetWidth() - 40, img_first.GetHeight() - 40 );
		back.AlphaBlend( &img_first, render_pos.X, render_pos.Y, 
			img_count == 1 ? fAnswerAlpha : fFirstAlpha );

		CImage *img_cover = NULL;
		PointF rot_pos = GetRotPos( img_alpha, pos );
		float fRotQuo = 20.0f;
		float fAlphaQuo = -0.08f;
		for( int i = 1; i < img_count; ++ i, fRotQuo += fRotQuo, fAlphaQuo += fAlphaQuo )
		{
			img_cover = MP_NEW CImage();
			CreateImgFromBuffer( file_manager.GetRandSmall(), *img_cover, true );
			img_cover->RotateEx( fFirstRot + fRotQuo, rot_pos.X, rot_pos.Y );
			img_cover->SetAlphaMap( &img_alpha, pos.X, pos.Y );
			back.AlphaBlend( img_cover, render_pos.X, render_pos.Y, fFirstAlpha + fAlphaQuo );
			MP_DELETE(img_cover);
		}

		BlendSelSign( back, img_rect, sel );
	}

	static int BlendSmallImage( CImage &back, CImage &img_alpha )
	{
		RectF area[4];
		SplitRectToFour( RectF( 0, 0, (float)back.GetWidth(), (float)back.GetHeight() ), 
			area );
		int answer = random( 4 ) + 1; /* [1,4] */
		for( int i = 1; i <= 4; ++ i )
		{
			if( i == answer )
			{
				// only render a random image only.
				GenSmallImage( 1, back, area[i - 1], i, img_alpha ); 
			}
			else
			{
				// random image count
				GenSmallImage( random( 2 ) + 2, back, area[i - 1], i, img_alpha );
			}
		}
		
		return answer;
	}
#ifdef DUMP_IMG
	void WriteBufToFile( const char *file, const void *buf, size_t size )
	{
		FILE *fp = fopen( file, "wb" );
		if( fp == NULL )
		{
			return ;
		}
		fwrite( buf, size, 1, fp );
		fclose( fp );
	}
	
	void DumpImage( CImage *img )
	{
		static int s_index = 1;
		char file[512];
		sprintf( file, "anticheat/question%d.jpg", s_index++ );
		BYTE *buf;
		DWORD size;
		img->ExportJpegToMemory( buf, size, 85 );
		WriteBufToFile( file, buf, size );
		free( buf );
	}
#else
#define DumpImage( img ) 
#endif

	CQuestionManager::Question CQuestionManager::GenOneQuestion()
	{
		Question ques = { { 0 }, 0 };
		CFileListManager &file_manager = GetInst( CFileListManager );
		int r = 0;

		// alpha image
		CImage img_alpha;
		CreateImgFromBuffer( file_manager.GetAlpha(), img_alpha );

		// 1.generate the background.
		CImage back;
		CreateImgFromBuffer( file_manager.GetRandBack(), back );
		r = random( 3 );
		CImage *img_cover;
		for( int i = 0; i < r; ++ i )
		{
			img_cover = MP_NEW CImage();
			CreateImgFromBuffer( file_manager.GetRandBack(), *img_cover );
			back.AlphaBlend( img_cover, 0, 0, ( random( 55 ) + 100 ) / 255.0f );
			MP_DELETE(img_cover);
		}

		// 2.generate small pictures.
		ques.answer = BlendSmallImage( back, img_alpha );

		// 3.covert the image to raw buffer.
		if( !back.ExportJpegToMemory( (BYTE*&) ques.data.buf, (DWORD&) ques.data.size, 85 ) )
		{
			ques.data.buf = 0;
			ques.data.size = 0;
			PutoutLog( LOG_FILE, LT_ERROR, "Export jpeg failed." );
		}

		DumpImage( &back );
		return ques;
	}
}

#endif
