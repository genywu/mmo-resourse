#include "StdAfx.h"
#include "../Engine/console.h"
#include "../Engine/utility.h"
#include <malloc.h>
#include "jpegencode.h"
#include "jpeg\jpeglib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace jpeg
{
	static bool jpeg_error;

static void 
error_exit(j_common_ptr pcinfo)
{
	// Create the message string
	char sz[256];
	(pcinfo->err->format_message) (pcinfo, sz);
//    static ofstream o("jerr.//log");
//    o << sz << endl;

    jpeg_error = true;
}

static void 
init_source(j_decompress_ptr cinfo)
{
}

static boolean 
fill_input_buffer(j_decompress_ptr cinfo)
{
    jpeg_error = true;
	return TRUE;
}

static void 
skip_input_data(j_decompress_ptr cinfo, long count) 
{
	jpeg_source_mgr * src = cinfo->src;
    if(count > 0) {
	    src->bytes_in_buffer -= count;
	    src->next_input_byte += count;
    }
}

static void 
term_source(j_decompress_ptr cinfo)
{

}
	//------------------------------------
	JpegEncode::JpegEncode(void):
	m_pData(NULL)
	{
	}

	JpegEncode::~JpegEncode(void)
	{
	}

	void JpegEncode::CreateBitmap(int iWidth,int iHeight)
	{
		m_iWidth = iWidth;
		m_iHeight= iHeight;
		m_pData = new tagRGB[iWidth * iHeight];
	}

	void JpegEncode::DestroyBitmap()
	{
		SAFEDELETEARRAY(m_pData);
	}

	void JpegEncode::SetPixel(int iX,int iY,DWORD dwColor)
	{
		int iPos = iX + iY * m_iWidth;

		m_pData[iPos].r = (BYTE)(dwColor >> 16 )& 0xFF;
		m_pData[iPos].g = (BYTE)(dwColor >> 8 )& 0xFF;
		m_pData[iPos].b = (BYTE)(dwColor & 0xFF);
	}

	void JpegEncode::Write(const char *pszFileName, int Q)
	{
		bool bReverse = false;
		FILE* f = fopen(pszFileName, "wb");
		if (f == NULL)
		{
			//MessageBox(0, "Error >> image_t::dumpjpg24, can't create file!", 0, 0);
			return ;
		}

		jpeg_compress_struct cinfo;
		jpeg_error = false;
		jpeg_error_mgr jerr;

		cinfo.err = jpeg_std_error(&jerr);
		jerr.error_exit = error_exit;

		jpeg_create_compress(&cinfo); 
		if(jpeg_error) return ;

		jpeg_stdio_dest(&cinfo, f);

		cinfo.image_width = m_iWidth;
		cinfo.image_height = m_iHeight;
		cinfo.input_components = 3;
		cinfo.in_color_space = JCS_RGB;
		cinfo.optimize_coding = 1;
		jpeg_set_defaults(&cinfo);
		jpeg_set_quality(&cinfo, Q, TRUE);
		jpeg_start_compress(&cinfo, TRUE);

		int rowstride = m_iWidth * cinfo.input_components;
		for (int i = 0 ;i < m_iHeight; i++)
		{
			byte* ptr = ((BYTE*)m_pData + rowstride*i);
			jpeg_write_scanlines(&cinfo, &ptr, 1);
		}

		jpeg_finish_compress(&(cinfo));
		jpeg_destroy_compress(&(cinfo));

		fflush(f);
		fclose(f);
	}

} 