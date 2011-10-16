#include "StdAfx.h"
#include "Image.h"

#include <malloc.h>
#include "ijl.h"

#pragma comment(lib, "../App/anticheat/ijl15l.lib")

/*
 * 功能: 构造函数
 * 摘要: -
 * 参数: -
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2009.06.02
 * 修改日志:
 */
CImage::CImage()
{
	m_pszImageFileName = NULL;
	m_pBitmapOri    = NULL;
	m_pBitmapBuffer = NULL;
	m_dwBitmapSize  = 0;
	m_dwWidth	    = 0;
	m_dwHeight	    = 0;

	m_pTmpFileData = NULL;
	m_uiDataPoint  = 0;
	m_uiDataSize   = 0;
}



/*
 * 功能: 析构函数
 * 摘要: -
 * 参数: -
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2009.06.02
 * 修改日志:
 */
CImage::~CImage()
{
	ClearData();
}



/*
 * 功能: 得到像素
 * 摘要: -
 * 参数: lX - 图像上的X坐标
 *		 lY - 图像上的Y坐标
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2009.06.02
 * 修改日志:
 */
DWORD CImage::GetPixel(long lX, long lY)
{
	if (lX < 0 || lX >= (long)m_dwWidth ||
		lY < 0 || lY >= (long)m_dwHeight)
		return 0;

	return m_pBitmapOri[lX + lY * (long)m_dwWidth];
}



/*
 * 功能: 得到缓冲像素
 * 摘要: -
 * 参数: lX - 图像上的X坐标
 *		 lY - 图像上的Y坐标
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2009.06.03
 * 修改日志:
 */
DWORD CImage::GetBufferPixel(long lX, long lY)
{
	if (lX < 0 || lX >= (long)m_dwWidth ||
		lY < 0 || lY >= (long)m_dwHeight)
		return 0;

	return m_pBitmapBuffer[lX + lY * (long)m_dwWidth];
}



/*
 * 功能: 得到缓冲像素
 * 摘要: -
 * 参数: lX - 图像上的X坐标
 *		 lY - 图像上的Y坐标
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2009.07.02
 * 修改日志:
 */
DWORD CImage::GetBufferPixel(DWORD * pBitmapBuffer, long lX, long lY)
{
	if (lX < 0 || lX >= (long)m_dwWidth ||
		lY < 0 || lY >= (long)m_dwHeight)
		return 0;

	return pBitmapBuffer[lX + lY * (long)m_dwWidth];
}



/*
 * 功能: 得到传入颜色的ALPHA值
 * 摘要: 返回的是一个0.0f ~ 1.0f的浮点值
 * 参数: dwColor - 需要分析的颜色
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2009.06.02
 * 修改日志:
 */
float CImage::GetAlphaValue(DWORD dwColor)
{
	return (float)((dwColor & 0xff000000) >> 24) / 255.0f;
}



/*
 * 功能: 设定像素
 * 摘要: -
 * 参数: lX			  - 图像上的X坐标
 *		 lY			  - 图像上的Y坐标
 *		 dwPixelValue - 需要设定的像素值
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2009.06.02
 * 修改日志:
 */
void CImage::SetPixel(long lX, long lY, DWORD dwPixelValue)
{
	if (lX < 0 || lX >= (long)m_dwWidth ||
		lY < 0 || lY >= (long)m_dwHeight)
		return;

	m_pBitmapOri[lX + lY * (long)m_dwWidth] = dwPixelValue;
}



/*
 * 功能: 设定缓冲像素
 * 摘要: -
 * 参数: lX			  - 图像上的X坐标
 *		 lY			  - 图像上的Y坐标
 *		 dwPixelValue - 需要设定的像素值
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2009.06.03
 * 修改日志:
 */
void CImage::SetBufferPixel(long lX, long lY, DWORD dwPixelValue)
{
	if (lX < 0 || lX >= (long)m_dwWidth ||
		lY < 0 || lY >= (long)m_dwHeight)
		return;

	m_pBitmapBuffer[lX + lY * (long)m_dwWidth] = dwPixelValue;
}



/*
 * 功能: 设定Alpha通道图
 * 摘要: -
 * 参数: pImage - 带有Alpha通道的图像对象
 *		 lPosX	- 设定图像左上角的X坐标
 *		 lPosY	- 设定图像左上角的Y坐标
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2009.07.09
 * 修改日志:
 */
void CImage::SetAlphaMap(CImage * pImage, long lPosX, long lPosY)
{
#ifdef TIMETEST
	DWORD dwStartTime = timeGetTime();
#endif

	if (pImage == NULL)
		return;

	DWORD dwColorOri;
	DWORD dwColorBuffer;
	for (DWORD dwY = 0; dwY < m_dwHeight; ++dwY)
	{
		for (DWORD dwX = 0; dwX < m_dwWidth; ++dwX)
		{
			dwColorOri = GetPixel(dwX, dwY);
			dwColorBuffer = GetBufferPixel(dwX, dwY);
			dwColorOri &= 0x00ffffff;
			dwColorBuffer &= 0x00ffffff;
			SetPixel(dwX, dwY, dwColorOri);
			SetBufferPixel(dwX, dwY, dwColorBuffer);
		}
	}

	RECT rcSrc;
	rcSrc.left	 = lPosX < 0 ? 0 : lPosX;
	rcSrc.top	 = lPosY < 0 ? 0 : lPosY;
	rcSrc.right	 = rcSrc.left + pImage->m_dwWidth;
	rcSrc.bottom = rcSrc.top + pImage->m_dwHeight;
	if (rcSrc.right > (long)m_dwWidth)
		rcSrc.right = (long)m_dwWidth;
	if (rcSrc.bottom > (long)m_dwHeight)
		rcSrc.bottom = (long)m_dwHeight;

	long lWidth	 = rcSrc.right - rcSrc.left;
	long lHeight = rcSrc.bottom - rcSrc.top;

	RECT rcBlend;
	rcBlend.left   = lPosX < 0 ? -lPosX : 0;
	rcBlend.top	   = lPosY < 0 ? -lPosY : 0;
	rcBlend.right  = rcBlend.left + lWidth;
	rcBlend.bottom = rcBlend.top + lHeight;

	DWORD dwSrcColor;
	DWORD dwSetAlpha;
	float fAlpha;
	long  lBlendY = rcBlend.top;
	DWORD r, g, b;
	float fr, fg, fb;
	DWORD dwFinal;
	for (long lY = rcSrc.top; lY < rcSrc.top + lHeight; ++lY, ++lBlendY)
	{
		long lBlendX = rcBlend.left;
		for (long lX = rcSrc.left; lX < rcSrc.left + lWidth; ++lX, ++lBlendX)
		{
			dwSrcColor = GetPixel(lX, lY);
			r = (dwSrcColor & 0x00ff0000) >> 16;
			g = (dwSrcColor & 0x0000ff00) >> 8;
			b =  dwSrcColor & 0x000000ff;

			dwSetAlpha = pImage->GetBufferPixel(lBlendX, lBlendY);
			dwSetAlpha &= 0xff000000;

			fAlpha = (float)(dwSetAlpha >> 24) / 255.0f;
			fr = (float)r * fAlpha;
			fg = (float)g * fAlpha;
			fb = (float)b * fAlpha;
			dwFinal = (dwSetAlpha | ((DWORD)fr << 16) | ((DWORD)fg << 8) | (DWORD)fb);

			SetPixel(lX, lY, dwFinal);
			SetBufferPixel(lX, lY, dwFinal);
		}
	}

#ifdef TIMETEST
	printf("[Usage time : %dms] Image file (%s) blend with (%s)\n", timeGetTime() - dwStartTime, m_pszImageFileName, pImage->m_pszImageFileName);
#endif
}

 

/*
 * 功能: 从文件读取BMP文件
 * 摘要: -
 * 参数: pszFileName - 文件路径名
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2009.06.02
 * 修改日志:
 *	2009.07.01
 *		增加了载入时对图片文件源中ALPHA通道的影响
 */
bool CImage::ReadBitmapFromFile(const char * pszFileName)
{
#ifdef TIMETEST
	DWORD dwStartTime = timeGetTime();
#endif

	ClearData();

	FILE * pFile = fopen(pszFileName, "rb");
	if (pFile == NULL)
		return false;

	//BYTE  * pFileData;			//临时文件数据
	//DWORD   dwFileSize;
	//ReadFileToMemory(pFile, pFileData, dwFileSize);

	tagBMPFILEHEADER stFileHdr;
	tagBMPINFOHEADER stInfoHdr;
	fread(&stFileHdr, sizeof(tagBMPFILEHEADER), 1, pFile);
	fread(&stInfoHdr, sizeof(tagBMPINFOHEADER), 1, pFile);

	if (stFileHdr.bfType != 0x4d42 || stInfoHdr.biCompression != BI_RGB) 
		return false;

	CreateBitmap(stInfoHdr.biWidth, stInfoHdr.biHeight, 0);

	DWORD * pBuf = m_pBitmapOri;
	switch(stInfoHdr.biBitCount)
	{
	//case 8:
	//	{
	//		RGBQUAD * pColorTable = (RGBQUAD *)(pFileData + sizeof(_tagBMPFILEHEADER));
	//		fseek(pFile, stHdr.bfOffBits, SEEK_SET);

	//		for (int y = 0; y < stHdr.biHeight; y++)
	//		{
	//			for(int x = 0; x < stHdr.biWidth; x++)
	//			{
	//				BYTE byIndex;
	//				fread(&byIndex, sizeof(BYTE), 1, pFile);
	//				DWORD c  = pColorTable[byIndex].rgbRed << 16;
	//				c |= pColorTable[byIndex].rgbGreen << 8;
	//				c |= pColorTable[byIndex].rgbBlue;

	//				*pBuf++ |= c;
	//			}
	//		}
	//	}
	//	break;
	case 24:
		{
			fseek(pFile, stFileHdr.bfOffBits, SEEK_SET);

			BYTE r, g, b;
			for (int y = 0; y < stInfoHdr.biHeight; y++)
			{
				for (int x = 0; x < stInfoHdr.biWidth; x++)
				{
					fread(&b, sizeof(BYTE), 1, pFile);
					fread(&g, sizeof(BYTE), 1, pFile);
					fread(&r, sizeof(BYTE), 1, pFile);
					(*pBuf) |= (((DWORD)r << 16) | ((DWORD)g << 8) | (DWORD)b);
					pBuf++;
				}
			}
		}
		break;
	case 32:
		{
			fseek(pFile, stFileHdr.bfOffBits, SEEK_SET);

			float fAlpha;
			BYTE a, r, g, b;
			float fr, fg, fb;
			for (int y = 0; y < stInfoHdr.biHeight; y++)
			{
				for (int x = 0; x < stInfoHdr.biWidth; x++)
				{
					fread(&b, sizeof(BYTE), 1, pFile);
					fread(&g, sizeof(BYTE), 1, pFile);
					fread(&r, sizeof(BYTE), 1, pFile);
					fread(&a, sizeof(BYTE), 1, pFile);

					if (a != 0x00)
					{
						fAlpha = (float)a / 255.0f;
						fr = (float)r * fAlpha;
						fg = (float)g * fAlpha;
						fb = (float)b * fAlpha;
						(*pBuf) = (((DWORD)a << 24) | ((DWORD)fr << 16) | ((DWORD)fg << 8) | (DWORD)fb);
					}
					//(*pBuf) = (((DWORD)a << 24) | ((DWORD)r << 16) | ((DWORD)g << 8) | (DWORD)b);

					pBuf++;
				}
			}
		}
		break;
	default:
		return false;
	}

//	SAFEDELETEARRAY(pFileData);
	FlipVertical(m_pBitmapOri);
	fclose(pFile);

	size_t st = strlen(pszFileName);
	m_pszImageFileName = new char[st + 1];
	strcpy(m_pszImageFileName, pszFileName);
	m_pszImageFileName[st] = 0;

	memcpy(m_pBitmapBuffer, m_pBitmapOri, sizeof(DWORD) * m_dwBitmapSize);

#ifdef TIMETEST
	printf("[Usage time : %dms] ReadBitmapFromFile->%s\n", timeGetTime() - dwStartTime, pszFileName);
#endif

	return true;
}



/*
 * 功能: 从内存读取BMP
 * 摘要: -
 * 参数: pData	   - 内存数据指针
 *		 uiDataLen - 数据长度
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2009.06.02
 * 修改日志:
 *	2009.07.01
 *		增加了载入时对图片源中ALPHA通道的影响
 */
bool CImage::ReadBitmapFromMemory(const BYTE * pData, unsigned int uiDataLen)
{
#ifdef TIMETEST
	DWORD dwStartTime = timeGetTime();
#endif

	ClearData();

	if (!ReadDataBegin(pData, uiDataLen))
		return false;

	BYTE * pFileData = new BYTE[uiDataLen + 1];			//临时文件数据
	memcpy(pFileData, pData, uiDataLen);
	pFileData[uiDataLen] = '\0';

	tagBMPFILEHEADER stFileHdr;
	tagBMPINFOHEADER stInfoHdr;
	ReadData(&stFileHdr, sizeof(tagBMPFILEHEADER));
	ReadData(&stInfoHdr, sizeof(tagBMPINFOHEADER));

	if (stFileHdr.bfType != 0x4d42 || stInfoHdr.biCompression != BI_RGB) 
		return false;

	CreateBitmap(stInfoHdr.biWidth, stInfoHdr.biHeight, 0);

	DWORD * pBuf = m_pBitmapOri;
	switch(stInfoHdr.biBitCount)
	{
	//case 8:
	//	{
	//		RGBQUAD * pColorTable = (RGBQUAD *)(pFileData + sizeof(_tagBMPFILEHEADER));
	//		fseek(pFile, stHdr.bfOffBits, SEEK_SET);

	//		for (int y = 0; y < stHdr.biHeight; y++)
	//		{
	//			for(int x = 0; x < stHdr.biWidth; x++)
	//			{
	//				BYTE byIndex;
	//				fread(&byIndex, sizeof(BYTE), 1, pFile);
	//				DWORD c  = pColorTable[byIndex].rgbRed << 16;
	//				c |= pColorTable[byIndex].rgbGreen << 8;
	//				c |= pColorTable[byIndex].rgbBlue;

	//				*pBuf++ |= c;
	//			}
	//		}
	//	}
	//	break;
	case 24:
		{
			m_uiDataPoint = stFileHdr.bfOffBits;

			BYTE r, g, b;
			for (int y = 0; y < stInfoHdr.biHeight; y++)
			{
				for (int x = 0; x < stInfoHdr.biWidth; x++)
				{
					ReadData(&b, sizeof(BYTE));
					ReadData(&g, sizeof(BYTE));
					ReadData(&r, sizeof(BYTE));
					(*pBuf) |= (((DWORD)r << 16) | ((DWORD)g << 8) | (DWORD)b);
					pBuf++;
				}
			}
		}
		break;
	case 32:
		{
			m_uiDataPoint = stFileHdr.bfOffBits;

			float fAlpha;
			BYTE a, r, g, b;
			float fr, fg, fb;
			for (int y = 0; y < stInfoHdr.biHeight; y++)
			{
				for (int x = 0; x < stInfoHdr.biWidth; x++)
				{
					ReadData(&b, sizeof(BYTE));
					ReadData(&g, sizeof(BYTE));
					ReadData(&r, sizeof(BYTE));
					ReadData(&a, sizeof(BYTE));

					if (a != 0x00)
					{
						fAlpha = (float)a / 255.0f;
						fr = (float)r * fAlpha;
						fg = (float)g * fAlpha;
						fb = (float)b * fAlpha;
						(*pBuf) = (((DWORD)a << 24) | ((DWORD)fr << 16) | ((DWORD)fg << 8) | (DWORD)fb);
					}

					pBuf++;
				}
			}
		}
		break;
	default:
		return false;
	}

	FlipVertical(m_pBitmapOri);
	SAFEDELETEARRAY(pFileData);

	memcpy(m_pBitmapBuffer, m_pBitmapOri, sizeof(DWORD) * m_dwBitmapSize);

#ifdef TIMETEST
	printf("[Usage time : %dms] ReadBitmapFromMemory\n", timeGetTime() - dwStartTime);
#endif

	return true;
}



/*
 * 功能: 将文件数据读到内存中
 * 摘要: -
 * 参数: pFile  - 文件指针
 *		 pData  - 返回数据指针
 *		 dwSize - 返回数据长度
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2009.06.02
 * 修改日志:
 */
bool CImage::ReadFileToMemory(FILE * pFile, BYTE *& pData, DWORD & dwSize)
{
	if (pFile == NULL)
		return false;

	fseek(pFile, 0L, SEEK_END);
	dwSize = ftell(pFile);
	fseek(pFile, 0L, SEEK_SET);
	pData = new BYTE[dwSize + 1];
	memset(pData, 0, dwSize);
	fread(pData, sizeof(BYTE), dwSize, pFile);
	pData[dwSize] = '\0';
	fseek(pFile, 0L, SEEK_SET);
	return true;
}



/*
 * 功能: 导出BMP文件
 * 摘要: -
 * 参数: pszFileName - 导出文件路径名
 *		 wBit		 - 导出文件位深
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2009.06.02
 * 修改日志:
 */
bool CImage::ExportBitmapToFile(const char * pszFileName, WORD wBit)
{
#ifdef TIMETEST
	DWORD dwStartTime = timeGetTime();
#endif

	tagBMPFILEHEADER stFileHdr;
	tagBMPINFOHEADER stInfoHdr;
	ZeroMemory(&stFileHdr, sizeof(tagBMPFILEHEADER));
	ZeroMemory(&stInfoHdr, sizeof(tagBMPINFOHEADER));

	stFileHdr.bfType	  = 0x4d42;
	stFileHdr.bfSize	  = m_dwBitmapSize * (wBit >> 3) + sizeof(tagBMPFILEHEADER) + sizeof(tagBMPINFOHEADER);
	stFileHdr.bfReserved1 = 0;
	stFileHdr.bfReserved2 = 0;
	switch (wBit)
	{
	case 8:
		stFileHdr.bfOffBits = sizeof(tagBMPFILEHEADER) + sizeof(tagBMPINFOHEADER) + 256 * sizeof(RGBQUAD);
		break;
	case 16:
	case 24:
	case 32:
		stFileHdr.bfOffBits = sizeof(tagBMPFILEHEADER) + sizeof(tagBMPINFOHEADER);
		break;
	}

	stInfoHdr.biSize		  = sizeof(tagBMPINFOHEADER);
	stInfoHdr.biWidth		  = m_dwWidth;
	stInfoHdr.biHeight		  = m_dwHeight;
	stInfoHdr.biPlanes		  = 1;
	stInfoHdr.biBitCount	  = wBit;
	stInfoHdr.biCompression	  = BI_RGB;
	stInfoHdr.biSizeImage	  = m_dwBitmapSize;
	stInfoHdr.biXPelsPerMeter = 0;
	stInfoHdr.biYPelsPerMeter = 0;
	stInfoHdr.biClrUsed		  = 0;
	stInfoHdr.biClrImportant  = 0;

	FILE * pFile = fopen(pszFileName, "w");
	fclose(pFile);

	pFile = fopen(pszFileName, "ab");
	fwrite(&stFileHdr, sizeof(tagBMPFILEHEADER), 1, pFile);
	fwrite(&stInfoHdr, sizeof(tagBMPINFOHEADER), 1, pFile);

	FlipVertical(m_pBitmapBuffer);

	BYTE a, r, g, b;
	DWORD * pBuf = m_pBitmapBuffer;
	switch (wBit)
	{
	case 8:
		break;
	case 16:
		break;
	case 24:
		for (int y = 0; y < stInfoHdr.biHeight; ++y)
		{
			for (int x = 0; x < stInfoHdr.biWidth; ++x)
			{
				DWORD dw = *pBuf++;
				r = (BYTE)((dw & 0x00ff0000) >> 16);
				g = (BYTE)((dw & 0x0000ff00) >> 8);
				b = (BYTE) (dw & 0x000000ff);

				fwrite(&b, sizeof(BYTE), 1, pFile);
				fwrite(&g, sizeof(BYTE), 1, pFile);
				fwrite(&r, sizeof(BYTE), 1, pFile);
			}
		}
		break;
	case 32:
		for (int y = 0; y < stInfoHdr.biHeight; ++y)
		{
			for (int x = 0; x < stInfoHdr.biWidth; ++x)
			{
				DWORD dw = *pBuf++;
				a = (BYTE)((dw & 0xff000000) >> 24);
				r = (BYTE)((dw & 0x00ff0000) >> 16);
				g = (BYTE)((dw & 0x0000ff00) >> 8);
				b = (BYTE) (dw & 0x000000ff);

				fwrite(&b, sizeof(BYTE), 1, pFile);
				fwrite(&g, sizeof(BYTE), 1, pFile);
				fwrite(&r, sizeof(BYTE), 1, pFile);
				fwrite(&a, sizeof(BYTE), 1, pFile);
			}
		}
		break;
	}

	fclose(pFile);

#ifdef TIMETEST
	printf("[Usage time : %dms] ExportBitmapToFile->%s\n", timeGetTime() - dwStartTime, pszFileName);
#endif

	return true;
}



/*
 * 功能: 导出Jpeg文件
 * 摘要: -
 * 参数: pszFileName - 导出文件路径名
 *		 iQuality	 - 导出质量(0~100+ 数值越小质量越差,文件占用磁盘空间越小)
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2009.06.02
 * 修改日志:
 */
bool CImage::ExportJpegToFile(const char * pszFileName, int iQuality)
{
#ifdef TIMETEST
	DWORD dwStartTime = timeGetTime();
#endif

	DWORD dwSize;
	BYTE * byData;
	ExportJpegToMemory(byData, dwSize, iQuality );
	FILE * pFile = fopen(pszFileName, "wb");
	fwrite(byData, dwSize, 1, pFile);
	fclose(pFile);

#ifdef TIMETEST
	printf("[Usage time : %dms] ExportJpegToFile->%s\n", timeGetTime() - dwStartTime, pszFileName);
#endif

	return true;
}



/*
 * 功能: 导出Jpeg到内存
 * 摘要: -
 * 参数: pData	  - 返回数据指针
 *		 dwSize	  - 返回数据尺寸
 *		 iQuality - Jpeg质量
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2009.06.04
 * 修改日志:
 */
bool CImage::ExportJpegToMemory(BYTE *& pData, DWORD & dwSize, int iQuality)
{
#ifdef TIMETEST
	DWORD dwStartTime = timeGetTime();
#endif

	const int bpp = 3;
	JPEG_CORE_PROPERTIES jcprops;

	if ( ijlInit(&jcprops) != IJL_OK )
	{
		ijlFree(&jcprops);
		return false;
	}

	BYTE * byBuffer = new BYTE[m_dwWidth * m_dwHeight * bpp];
	BYTE * buffer = byBuffer;
	for(DWORD y = 0; y < m_dwHeight; ++y)
	{
		for(DWORD x = 0; x < m_dwWidth; ++x)
		{
			DWORD dwColor = GetBufferPixel(x, y);
			*buffer++ = GetBValue(dwColor);
			*buffer++ = GetGValue(dwColor);
			*buffer++ = GetRValue(dwColor);
		}
	}

	jcprops.DIBWidth    = m_dwWidth;
	jcprops.DIBHeight   = m_dwHeight;
	jcprops.JPGWidth    = m_dwWidth;
	jcprops.JPGHeight   = m_dwHeight;
	jcprops.DIBBytes    = byBuffer;
	jcprops.DIBPadBytes = 0;
	jcprops.DIBChannels = bpp;
	jcprops.JPGChannels = bpp;

	if ( bpp == 3 )
	{
		jcprops.DIBColor = IJL_RGB;
		jcprops.JPGColor = IJL_YCBCR;
		jcprops.JPGSubsampling = IJL_411;
		jcprops.DIBSubsampling = (IJL_DIBSUBSAMPLING) 0;
	}
	else
	{
		jcprops.DIBColor = IJL_G;
		jcprops.JPGColor = IJL_G;
		jcprops.JPGSubsampling = (IJL_JPGSUBSAMPLING) 0;
		jcprops.DIBSubsampling = (IJL_DIBSUBSAMPLING) 0;
	}

	int size = m_dwWidth * m_dwHeight * bpp;
	pData = new BYTE[size];

	jcprops.JPGSizeBytes = size;
	jcprops.JPGBytes     = pData;
	jcprops.jquality = iQuality;

	if ( ijlWrite(&jcprops, IJL_JBUFF_WRITEWHOLEIMAGE) != IJL_OK )
	{
		ijlFree(&jcprops);
		SAFEDELETEARRAY(pData);
		return false;
	}

	if ( ijlFree(&jcprops) != IJL_OK )
	{
		SAFEDELETEARRAY(pData);
		return false;
	}

	dwSize = jcprops.JPGSizeBytes;
	SAFEDELETEARRAY(byBuffer);

#ifdef TIMETEST
	printf("[Usage time : %dms] ExportJpegToMemory->%s\n", timeGetTime() - dwStartTime, m_pszImageFileName);
#endif

	return true;
}



/*
 * 功能: 清除图像数据
 * 摘要: -
 * 参数: -
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2009.06.02
 * 修改日志:
 */
void CImage::ClearData()
{
	SAFEDELETEARRAY(m_pszImageFileName);
	SAFEDELETEARRAY(m_pBitmapOri);
	SAFEDELETEARRAY(m_pBitmapBuffer);

	m_dwWidth  = 0;
	m_dwHeight = 0;

	m_pTmpFileData = NULL;
	m_uiDataPoint  = 0;
	m_uiDataSize   = 0;
}



/*
 * 功能: 当前图像与另一个图像进行ALPHA混合
 * 摘要: -
 * 参数: pImage			  - 需要混合的图像指针
 *		 lPosX			  - 混合到当前图像的左上角x坐标
 *		 lPosY			  - 混合到当前图像的左上角y坐标
 *		 fAlphaBlendValue - 混合Alpha值
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2009.06.02
 * 修改日志:
 *	2009.07.01
 *		增加了图片混合时考虑边缘羽化功能(羽化依据需要混合的图像之Alpha通道)
 */
void CImage::AlphaBlend(CImage * pImage, long lPosX, long lPosY, float fAlphaBlendValue)
{
#ifdef TIMETEST
	DWORD dwStartTime = timeGetTime();
#endif

	float fAlphaValue;
	DWORD dwBlendColor;

	if (fAlphaBlendValue <= 0.0f)
		return;
	else if (fAlphaBlendValue > 1.0f)
		fAlphaBlendValue = 1.0f;

	RECT rcSrc;
	rcSrc.left	 = lPosX < 0 ? 0 : lPosX;
	rcSrc.top	 = lPosY < 0 ? 0 : lPosY;
	rcSrc.right	 = rcSrc.left + pImage->m_dwWidth;
	rcSrc.bottom = rcSrc.top + pImage->m_dwHeight;
	if (rcSrc.right > (long)m_dwWidth)
		rcSrc.right = (long)m_dwWidth;
	if (rcSrc.bottom > (long)m_dwHeight)
		rcSrc.bottom = (long)m_dwHeight;

	long lWidth	 = rcSrc.right - rcSrc.left;
	long lHeight = rcSrc.bottom - rcSrc.top;

	RECT rcBlend;
	rcBlend.left   = lPosX < 0 ? -lPosX : 0;
	rcBlend.top	   = lPosY < 0 ? -lPosY : 0;
	rcBlend.right  = rcBlend.left + lWidth;
	rcBlend.bottom = rcBlend.top + lHeight;

	//if (fAlphaBlendValue == 1.0f)
	//{
	//	long lBlendY = rcBlend.top;
	//	long lHei	 = rcSrc.top + lHeight;
	//	for (long lY = rcSrc.top; lY < lHei; ++lY, ++lBlendY)
	//	{
	//		long lBlendX = rcBlend.left;
	//		long lWid	 = rcSrc.left + lWidth;
	//		for (long lX = rcSrc.left; lX < lWid; ++lX, ++lBlendX)
	//		{
	//			dwBlendColor = pImage->GetBufferPixel(lBlendX, lBlendY);
	//			fAlphaValue  = GetAlphaValue(dwBlendColor);
	//			if (fAlphaValue != 0.0f)
	//				SetBufferPixel(lX, lY, dwBlendColor);
	//		}
	//	}
	//}else
	{
		DWORD dwSrcColor;
		float fBlendSrcAlpha;
		long  lBlendY		 = rcBlend.top;
		int srcA, srcR, srcG, srcB, blendA, blendR, blendG, blendB, newA, newR, newG, newB;
		for (long lY = rcSrc.top; lY < rcSrc.top + lHeight; ++lY, ++lBlendY)
		{
			long lBlendX = rcBlend.left;
			for (long lX = rcSrc.left; lX < rcSrc.left + lWidth; ++lX, ++lBlendX)
			{
				dwBlendColor = pImage->GetBufferPixel(lBlendX, lBlendY);
				fAlphaValue  = GetAlphaValue(dwBlendColor);
				if (fAlphaValue != 0.0f)
				{
					fBlendSrcAlpha = 1.0f - (fAlphaBlendValue * fAlphaValue);
					dwSrcColor	   = GetBufferPixel(lX, lY);
					srcA = (int)((float)((dwSrcColor & 0xff000000) >> 24) * fBlendSrcAlpha);
					srcR = (int)((float)((dwSrcColor & 0x00ff0000) >> 16) * fBlendSrcAlpha);
					srcG = (int)((float)((dwSrcColor & 0x0000ff00) >> 8) * fBlendSrcAlpha);
					srcB = (int)((float) (dwSrcColor & 0x000000ff) * fBlendSrcAlpha);

					blendA = (int)((float)((dwBlendColor & 0xff000000) >> 24) * fAlphaBlendValue);
					blendR = (int)((float)((dwBlendColor & 0x00ff0000) >> 16) * fAlphaBlendValue);
					blendG = (int)((float)((dwBlendColor & 0x0000ff00) >> 8) * fAlphaBlendValue);
					blendB = (int)((float) (dwBlendColor & 0x000000ff) * fAlphaBlendValue);

					newA = srcA + blendA;
					if (newA > 0xff)	newA = 0xff;
					newR = srcR + blendR;
					if (newR > 0xff)	newR = 0xff;
					newG = srcG + blendG;
					if (newG > 0xff)	newG = 0xff;
					newB = srcB + blendB;
					if (newB > 0xff)	newB = 0xff;

					SetBufferPixel(lX, lY, (((DWORD)newA << 24) | ((DWORD)newR << 16) | ((DWORD)newG << 8) | (DWORD)newB));
				}
			}
		}
	}

#ifdef TIMETEST
	printf("[Usage time : %dms] Image file (%s) blend with (%s)\n", timeGetTime() - dwStartTime, m_pszImageFileName, pImage->m_pszImageFileName);
#endif
}



/*
 * 功能: 旋转图像
 * 摘要: 绕图像中心旋转,
 *		 经过旋转后的图像不会改变尺寸
 * 参数: fRotAngle - 顺时针角度
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2009.06.02
 * 修改日志:
 *	2009.07.01
 *		修正了该操作会导致之前变换图片的信息丢失
 */
void CImage::Rotate(float fRotAngle)
{
#ifdef TIMETEST
	DWORD dwStartTime = timeGetTime();
#endif

	DWORD * pBitmapBuffer = new DWORD[m_dwBitmapSize];
	memcpy(pBitmapBuffer, m_pBitmapBuffer, sizeof(DWORD) * m_dwBitmapSize);

	ZeroMemory(m_pBitmapBuffer, sizeof(DWORD) * m_dwBitmapSize);

	//DWORD *pDest = m_pBitmapOri;
	//DWORD dwSize = m_dwBitmapSize;
	//__asm
	//{
	//	mov edi,[pDest]
	//	mov eax,0xffff0000
	//		cld 
	//		mov ecx,dwSize
	//		rep stosd
	//}

	//memset(m_pBitmapOri, 0xff, sizeof(DWORD) * m_dwBitmapSize);

	float	fR		 = A2R(fRotAngle);
	float	fCosA	 = cos(fR);
	float	fSinA	 = sin(fR);
	POINT	ptCenter = { (m_dwWidth/* + 1*/) >> 1, (m_dwHeight/* + 1*/) >> 1 };
	DWORD * pOri	 = pBitmapBuffer;
	POINT	ptNew;
	DWORD	dwTmp;

	for (long lY = 0; lY < (long)m_dwHeight; ++lY)
	{
		for (long lX = 0; lX < (long)m_dwWidth; ++lX)
		{
			ptNew.x = (long)((float)(lX - ptCenter.x) * fCosA - (float)(lY - ptCenter.y) * fSinA + (float)ptCenter.x + 0.5f);
			ptNew.y = (long)((float)(lY - ptCenter.y) * fCosA + (float)(lX - ptCenter.x) * fSinA + (float)ptCenter.y + 0.5f);

			SetBufferPixel(ptNew.x, ptNew.y, *pOri);
			pOri++;
		}
	}

	//pOri = m_pBitmapBuffer;
	//for (long l = 2; l < (long)m_dwBitmapSize; ++l)
	//{
	//	if (*pOri == 0)
	//		*pOri = m_pBitmapBuffer[l - 1];
	//	pOri++;
	//}

	memcpy(pBitmapBuffer, m_pBitmapBuffer, sizeof(DWORD) * m_dwBitmapSize);
	pOri = pBitmapBuffer;
	for (long lY = 0; lY < (long)m_dwHeight; ++lY)
	{
		for (long lX = 0; lX < (long)m_dwWidth; ++lX)
		{
			if (*pOri == 0)
			{
				dwTmp = AveragePixelEx(pBitmapBuffer, lX, lY, fCosA, fSinA);
				SetBufferPixel(lX, lY, dwTmp);
			}

			pOri++;
		}
	}

	SAFEDELETEARRAY(pBitmapBuffer);

#ifdef TIMETEST
	printf("[Usage time : %dms] Rotate->%s\n", timeGetTime() - dwStartTime, m_pszImageFileName);
#endif
}



/*
 * 功能: 旋转图像
 * 摘要: 指定旋转中心坐标的旋转图像,
 *		 经过旋转后的图像不会改变尺寸
 * 参数: fRotAngle - 顺时针角度
 *		 lPosX	   - 旋转中心坐标
 *		 lPosY	   - 旋转中心坐标
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2009.07.10
 * 修改日志:
 */
void CImage::Rotate(float fRotAngle, long lPosX, long lPosY)
{
#ifdef TIMETEST
	DWORD dwStartTime = timeGetTime();
#endif

	DWORD * pBitmapBuffer = new DWORD[m_dwBitmapSize];
	memcpy(pBitmapBuffer, m_pBitmapBuffer, sizeof(DWORD) * m_dwBitmapSize);

	ZeroMemory(m_pBitmapBuffer, sizeof(DWORD) * m_dwBitmapSize);

	//DWORD *pDest = m_pBitmapOri;
	//DWORD dwSize = m_dwBitmapSize;
	//__asm
	//{
	//	mov edi,[pDest]
	//	mov eax,0xffff0000
	//		cld 
	//		mov ecx,dwSize
	//		rep stosd
	//}

	//memset(m_pBitmapOri, 0xff, sizeof(DWORD) * m_dwBitmapSize);

	float	fR	  = A2R(fRotAngle);
	float	fCosA = cos(fR);
	float	fSinA = sin(fR);
	DWORD * pOri  = pBitmapBuffer;
	POINT	ptNew;
	DWORD	dwTmp;

	for (long lY = 0; lY < (long)m_dwHeight; ++lY)
	{
		for (long lX = 0; lX < (long)m_dwWidth; ++lX)
		{
			ptNew.x = (long)((float)(lX - lPosX) * fCosA - (float)(lY - lPosY) * fSinA + (float)lPosX);
			ptNew.y = (long)((float)(lY - lPosY) * fCosA + (float)(lX - lPosX) * fSinA + (float)lPosY);

			SetBufferPixel(ptNew.x, ptNew.y, *pOri);
			pOri++;
		}
	}

	//pOri = m_pBitmapBuffer;
	//for (long l = 2; l < (long)m_dwBitmapSize; ++l)
	//{
	//	if (*pOri == 0)
	//		*pOri = m_pBitmapBuffer[l - 1];
	//	pOri++;
	//}

	memcpy(pBitmapBuffer, m_pBitmapBuffer, sizeof(DWORD) * m_dwBitmapSize);
	pOri = pBitmapBuffer;
	for (long lY = 0; lY < (long)m_dwHeight; ++lY)
	{
		for (long lX = 0; lX < (long)m_dwWidth; ++lX)
		{
			if (*pOri == 0)
			{
				dwTmp = AveragePixelEx(pBitmapBuffer, lX, lY, fCosA, fSinA);
				SetBufferPixel(lX, lY, dwTmp);
			}

			pOri++;
		}
	}

	SAFEDELETEARRAY(pBitmapBuffer);

#ifdef TIMETEST
	printf("[Usage time : %dms] Rotate->%s\n", timeGetTime() - dwStartTime, m_pszImageFileName);
#endif
}



/*
 * 功能: 旋转图像
 * 摘要: 绕图像中心旋转,
 *		 经过旋转后的图像不会改变尺寸
 *		 该函数可以做到几乎无损的图像旋转,结果图像会有效抑制锯齿发生,但效率却...
 * 参数: fRotAngle - 顺时针角度
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2009.07.06
 * 修改日志:
 */
void CImage::RotateEx(float fRotAngle)
{
	Zoom(2);
	Blur();
	Rotate(fRotAngle);
	Zoom(-2);
}



/*
 * 功能: 旋转图像
 * 摘要: 指定旋转中心坐标的旋转图像,
 *		 经过旋转后的图像不会改变尺寸
 *		 该函数可以做到几乎无损的图像旋转,结果图像会有效抑制锯齿发生,但效率却...
 * 参数: fRotAngle - 顺时针角度
 *		 lPosX	   - 旋转中心坐标
 *		 lPosY	   - 旋转中心坐标
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2009.07.10
 * 修改日志:
 */
void CImage::RotateEx(float fRotAngle, long lPosX, long lPosY)
{
	Zoom(2);
	Blur();
	Rotate(fRotAngle, lPosX * 2, lPosY * 2);
	Zoom(-2);
}



/*
 * 功能: 缩放图像
 * 摘要: 图像宽高同比例缩放
 * 参数: iScale - 缩放比例(正数为放大,负数为缩小,[-1, 1]区间无效注意是封闭区间)
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2009.07.06
 * 修改日志:
 */
void CImage::Zoom(int iScale)
{
	if (iScale <= 1 && iScale >= -1)
		return;

	DWORD	dwZoomW;
	DWORD	dwZoomH;
	LPDWORD apNew;
	LPDWORD apBuffer;
	DWORD	dwIndex;

	if (iScale > 1)
	{
		dwZoomW		   = m_dwWidth * iScale;
		dwZoomH		   = m_dwHeight * iScale;
		m_dwBitmapSize = dwZoomW * dwZoomH;
		apNew		   = new DWORD[m_dwBitmapSize];
		apBuffer	   = apNew;
		ZeroMemory(apNew, sizeof(DWORD) * m_dwBitmapSize);

		for (DWORD dwH = 0; dwH < m_dwHeight; ++dwH)
		{
			for (DWORD dwW = 0; dwW < m_dwWidth; ++dwW)
			{
				dwIndex = dwW + dwH * m_dwWidth;
				for (int iStep = 0; iStep < iScale; ++iStep)
					apBuffer[iStep] = m_pBitmapBuffer[dwIndex];

				apBuffer += iScale;
			}

			for (int iStep = 0; iStep < iScale - 1; ++iStep)
			{
				memcpy(apBuffer, apBuffer - dwZoomW, sizeof(DWORD) * dwZoomW);
				apBuffer += dwZoomW;
			}
		}
	}else
	{
		iScale		   = -iScale;
		dwZoomW		   = m_dwWidth / iScale;
		dwZoomH		   = m_dwHeight / iScale;
		m_dwBitmapSize = dwZoomW * dwZoomH;
		apNew		   = new DWORD[m_dwBitmapSize];
		apBuffer	   = apNew;
		ZeroMemory(apNew, sizeof(DWORD) * m_dwBitmapSize);

		for (DWORD dwH = 0; dwH < m_dwHeight; dwH += iScale)
		{
			for (DWORD dwW = 0; dwW < m_dwWidth; dwW += iScale)
			{
				dwIndex	  = dwW + dwH * m_dwWidth;
				*apBuffer = m_pBitmapBuffer[dwIndex];

				++apBuffer;
			}
		}
		
	}

	m_dwWidth  = dwZoomW;
	m_dwHeight = dwZoomH;
	SAFEDELETEARRAY(m_pBitmapOri);
	SAFEDELETEARRAY(m_pBitmapBuffer);

	m_pBitmapOri	= new DWORD[m_dwBitmapSize];
	m_pBitmapBuffer = apNew;
	memcpy(m_pBitmapOri, m_pBitmapBuffer, sizeof(DWORD) * m_dwBitmapSize);
}



/*
 * 功能: 模糊图像
 * 摘要: -
 * 参数: -
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2009.07.02
 * 修改日志:
 */
void CImage::Blur()
{
#ifdef TIMETEST
	DWORD dwStartTime = timeGetTime();
#endif

	DWORD * pBitmapBuffer = new DWORD[m_dwBitmapSize];
	memcpy(pBitmapBuffer, m_pBitmapBuffer, sizeof(DWORD) * m_dwBitmapSize);

	ZeroMemory(m_pBitmapBuffer, sizeof(DWORD) * m_dwBitmapSize);

	for (long lY = 0; lY < (long)m_dwHeight; ++lY)
	{
		for (long lX = 0; lX < (long)m_dwWidth; ++lX)
			SetBufferPixel(lX, lY, AveragePixel1x1/*AveragePixel3x3*/(pBitmapBuffer, lX, lY));
	}

	SAFEDELETEARRAY(pBitmapBuffer);

#ifdef TIMETEST
	printf("[Usage time : %dms] Blur->%s\n", timeGetTime() - dwStartTime, m_pszImageFileName);
#endif
}



/*
 * 功能: 创建位图
 * 摘要: 创建点阵数据
 * 参数: dwWidth  - 创建宽度
 *		 dwHeight - 创建高度
 *		 dwColor  - 初始化填充颜色
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2009.06.02
 * 修改日志:
 */
bool CImage::CreateBitmap(DWORD dwWidth, DWORD dwHeight, DWORD dwColor)
{
	m_dwWidth	    = dwWidth;
	m_dwHeight	    = dwHeight;
	m_dwBitmapSize  = dwWidth * dwHeight;
	m_pBitmapOri    = new DWORD[m_dwBitmapSize];
	m_pBitmapBuffer = new DWORD[m_dwBitmapSize];
	if (m_pBitmapOri == NULL || m_pBitmapBuffer == NULL)
		return false;

	DWORD * pDest  = m_pBitmapOri;
	DWORD   dwSize = m_dwBitmapSize;
	__asm
	{
		mov edi,[pDest]
		mov eax,dwColor
			cld
			mov ecx,dwSize
			rep stosd
	}

	memcpy(m_pBitmapBuffer, m_pBitmapOri, sizeof(DWORD) * m_dwBitmapSize);

	return true;
}



/*
 * 功能: 翻转位图
 * 摘要: -
 * 参数: szpBitmap - 需要翻转的位图数据指针
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2009.06.02
 * 修改日志:
 */
void CImage::FlipVertical(DWORD * szpBitmap)
{
	DWORD * pLineTemp  = new DWORD[m_dwWidth];
	if (pLineTemp == NULL)
		return;

	DWORD * pUp		   = szpBitmap;
	DWORD * pBottom	   = szpBitmap + (m_dwHeight - 1) * m_dwWidth;
	DWORD	dwCopySize = m_dwWidth * sizeof(DWORD);
	DWORD	dwHeightH  = m_dwHeight >> 1;

	for (DWORD i = 0; i < dwHeightH; ++i)
	{
		memcpy(pLineTemp, pUp, dwCopySize);			//save line to temp
		memcpy(pUp, pBottom, dwCopySize);			//move line
		memcpy(pBottom, pLineTemp, dwCopySize);		//restor line

		pUp		+= m_dwWidth;
		pBottom -= m_dwWidth;
	}

	SAFEDELETEARRAY(pLineTemp);
}



/*
 * 功能: 计算指定像素点周围像素均值
 * 摘要: -
 * 参数: pBitmapBuffer - 位图缓冲
 *		 lX			   - 图像上的X坐标
 *		 lY			   - 图像上的Y坐标
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2009.07.02
 * 修改日志:
 */
DWORD CImage::AveragePixel1x1(DWORD * pBitmapBuffer, long lX, long lY)
{
	DWORD dwPixel;

	DWORD dwA[5] = { 0, 0, 0, 0, 0 };
	DWORD dwR[5] = { 0, 0, 0, 0, 0 };
	DWORD dwG[5] = { 0, 0, 0, 0, 0 };
	DWORD dwB[5] = { 0, 0, 0, 0, 0 };
	DWORD dwReA;
	DWORD dwReR;
	DWORD dwReG;
	DWORD dwReB;

	dwPixel = GetBufferPixel(pBitmapBuffer, lX, lY - 1);
	dwA[0] = (dwPixel & 0xff000000) >> 24;
	dwR[0] = (dwPixel & 0x00ff0000) >> 16;
	dwG[0] = (dwPixel & 0x0000ff00) >> 8;
	dwB[0] = (dwPixel & 0x000000ff);

	dwPixel = GetBufferPixel(pBitmapBuffer, lX - 1, lY);
	dwA[1] = (dwPixel & 0xff000000) >> 24;
	dwR[1] = (dwPixel & 0x00ff0000) >> 16;
	dwG[1] = (dwPixel & 0x0000ff00) >> 8;
	dwB[1] = (dwPixel & 0x000000ff);

	dwPixel = GetBufferPixel(pBitmapBuffer, lX, lY);
	dwA[2] = (dwPixel & 0xff000000) >> 24;
	dwR[2] = (dwPixel & 0x00ff0000) >> 16;
	dwG[2] = (dwPixel & 0x0000ff00) >> 8;
	dwB[2] = (dwPixel & 0x000000ff);

	dwPixel = GetBufferPixel(pBitmapBuffer, lX + 1, lY);
	dwA[3] = (dwPixel & 0xff000000) >> 24;
	dwR[3] = (dwPixel & 0x00ff0000) >> 16;
	dwG[3] = (dwPixel & 0x0000ff00) >> 8;
	dwB[3] = (dwPixel & 0x000000ff);

	dwPixel = GetBufferPixel(pBitmapBuffer, lX, lY + 1);
	dwA[4] = (dwPixel & 0xff000000) >> 24;
	dwR[4] = (dwPixel & 0x00ff0000) >> 16;
	dwG[4] = (dwPixel & 0x0000ff00) >> 8;
	dwB[4] = (dwPixel & 0x000000ff);

	dwReA = 0;
	dwReR = 0;
	dwReG = 0;
	dwReB = 0;
	static DWORD dwDiv = 5;

	for (DWORD dw = 0; dw < 5; ++dw)
		dwReA += dwA[dw];
	dwReA /= dwDiv;
	for (DWORD dw = 0; dw < 5; ++dw)
		dwReR += dwR[dw];
	dwReR /= dwDiv;
	for (DWORD dw = 0; dw < 5; ++dw)
		dwReG += dwG[dw];
	dwReG /= dwDiv;
	for (DWORD dw = 0; dw < 5; ++dw)
		dwReB += dwB[dw];
	dwReB /= dwDiv;

	return (((DWORD)dwReA << 24) | ((DWORD)dwReR << 16) | ((DWORD)dwReG << 8) | (DWORD)dwReB);
}



/*
 * 功能: 计算指定像素点周围像素均值
 * 摘要: -
 * 参数: pBitmapBuffer - 位图缓冲
 *		 lX			   - 图像上的X坐标
 *		 lY			   - 图像上的Y坐标
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2009.07.02
 * 修改日志:
 */
DWORD CImage::AveragePixel3x3(DWORD * pBitmapBuffer, long lX, long lY)
{
	DWORD dwPixel;

	DWORD dwA[9];
	DWORD dwR[9];
	DWORD dwG[9];
	DWORD dwB[9];
	DWORD dwReA;
	DWORD dwReR;
	DWORD dwReG;
	DWORD dwReB;

	dwPixel = GetBufferPixel(pBitmapBuffer, lX - 1, lY - 1);
	dwA[0] = (dwPixel & 0xff000000) >> 24;
	dwR[0] = (dwPixel & 0x00ff0000) >> 16;
	dwG[0] = (dwPixel & 0x0000ff00) >> 8;
	dwB[0] = (dwPixel & 0x000000ff);

	dwPixel = GetBufferPixel(pBitmapBuffer, lX, lY - 1);
	dwA[1] = (dwPixel & 0xff000000) >> 24;
	dwR[1] = (dwPixel & 0x00ff0000) >> 16;
	dwG[1] = (dwPixel & 0x0000ff00) >> 8;
	dwB[1] = (dwPixel & 0x000000ff);

	dwPixel = GetBufferPixel(pBitmapBuffer, lX + 1, lY - 1);
	dwA[2] = (dwPixel & 0xff000000) >> 24;
	dwR[2] = (dwPixel & 0x00ff0000) >> 16;
	dwG[2] = (dwPixel & 0x0000ff00) >> 8;
	dwB[2] = (dwPixel & 0x000000ff);

	dwPixel = GetBufferPixel(pBitmapBuffer, lX - 1, lY);
	dwA[3] = (dwPixel & 0xff000000) >> 24;
	dwR[3] = (dwPixel & 0x00ff0000) >> 16;
	dwG[3] = (dwPixel & 0x0000ff00) >> 8;
	dwB[3] = (dwPixel & 0x000000ff);

	dwPixel = GetBufferPixel(pBitmapBuffer, lX, lY);
	dwA[4] = (dwPixel & 0xff000000) >> 24;
	dwR[4] = (dwPixel & 0x00ff0000) >> 16;
	dwG[4] = (dwPixel & 0x0000ff00) >> 8;
	dwB[4] = (dwPixel & 0x000000ff);

	dwPixel = GetBufferPixel(pBitmapBuffer, lX + 1, lY);
	dwA[5] = (dwPixel & 0xff000000) >> 24;
	dwR[5] = (dwPixel & 0x00ff0000) >> 16;
	dwG[5] = (dwPixel & 0x0000ff00) >> 8;
	dwB[5] = (dwPixel & 0x000000ff);

	dwPixel = GetBufferPixel(pBitmapBuffer, lX - 1, lY + 1);
	dwA[6] = (dwPixel & 0xff000000) >> 24;
	dwR[6] = (dwPixel & 0x00ff0000) >> 16;
	dwG[6] = (dwPixel & 0x0000ff00) >> 8;
	dwB[6] = (dwPixel & 0x000000ff);

	dwPixel = GetBufferPixel(pBitmapBuffer, lX, lY + 1);
	dwA[7] = (dwPixel & 0xff000000) >> 24;
	dwR[7] = (dwPixel & 0x00ff0000) >> 16;
	dwG[7] = (dwPixel & 0x0000ff00) >> 8;
	dwB[7] = (dwPixel & 0x000000ff);

	dwPixel = GetBufferPixel(pBitmapBuffer, lX + 1, lY + 1);
	dwA[8] = (dwPixel & 0xff000000) >> 24;
	dwR[8] = (dwPixel & 0x00ff0000) >> 16;
	dwG[8] = (dwPixel & 0x0000ff00) >> 8;
	dwB[8] = (dwPixel & 0x000000ff);

	dwReA = 0;
	dwReR = 0;
	dwReG = 0;
	dwReB = 0;
	for (DWORD dw = 0; dw < 9; ++dw)
		dwReA += dwA[dw];
	dwReA /= 9;
	for (DWORD dw = 0; dw < 9; ++dw)
		dwReR += dwR[dw];
	dwReR /= 9;
	for (DWORD dw = 0; dw < 9; ++dw)
		dwReG += dwG[dw];
	dwReG /= 9;
	for (DWORD dw = 0; dw < 9; ++dw)
		dwReB += dwB[dw];
	dwReB /= 9;

	return (((DWORD)dwReA << 24) | ((DWORD)dwReR << 16) | ((DWORD)dwReG << 8) | (DWORD)dwReB);
}



/*
 * 功能: 计算指定像素点周围像素均值
 * 摘要: 不计算为0的像素
 * 参数: pBitmapBuffer - 位图缓冲
 *		 lX			   - 图像上的X坐标
 *		 lY			   - 图像上的Y坐标
 *		 fRotAngle	   - 旋转角度
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2009.07.02
 * 修改日志:
 */
DWORD CImage::AveragePixelEx(DWORD * pBitmapBuffer, long lX, long lY, float fCosA, float fSinA)
{
	DWORD dwPixel;
	DWORD dwCount;

	DWORD dwA[9];
	DWORD dwR[9];
	DWORD dwG[9];
	DWORD dwB[9];
	DWORD dwReA;
	DWORD dwReR;
	DWORD dwReG;
	DWORD dwReB;

	POINT ptPos[9];
	ptPos[0].x = (long)((float)((lX - 1) - lX) * fCosA - (float)((lY - 1) - lY) * fSinA + (float)lX + 0.5f);
	ptPos[0].y = (long)((float)((lY - 1) - lY) * fCosA + (float)((lX - 1) - lX) * fSinA + (float)lY + 0.5f);

	ptPos[1].x = (long)((float)((lX) - lX) * fCosA - (float)((lY - 1) - lY) * fSinA + (float)lX + 0.5f);
	ptPos[1].y = (long)((float)((lY - 1) - lY) * fCosA + (float)((lX) - lX) * fSinA + (float)lY + 0.5f);

	ptPos[2].x = (long)((float)((lX + 1) - lX) * fCosA - (float)((lY - 1) - lY) * fSinA + (float)lX + 0.5f);
	ptPos[2].y = (long)((float)((lY - 1) - lY) * fCosA + (float)((lX + 1) - lX) * fSinA + (float)lY + 0.5f);

	ptPos[3].x = (long)((float)((lX - 1) - lX) * fCosA - (float)((lY) - lY) * fSinA + (float)lX + 0.5f);
	ptPos[3].y = (long)((float)((lY) - lY) * fCosA + (float)((lX - 1) - lX) * fSinA + (float)lY + 0.5f);

	ptPos[4].x = (long)((float)((lX) - lX) * fCosA - (float)((lY) - lY) * fSinA + (float)lX + 0.5f);
	ptPos[4].y = (long)((float)((lY) - lY) * fCosA + (float)((lX) - lX) * fSinA + (float)lY + 0.5f);

	ptPos[5].x = (long)((float)((lX + 1) - lX) * fCosA - (float)((lY) - lY) * fSinA + (float)lX + 0.5f);
	ptPos[5].y = (long)((float)((lY) - lY) * fCosA + (float)((lX + 1) - lX) * fSinA + (float)lY + 0.5f);

	ptPos[6].x = (long)((float)((lX - 1) - lX) * fCosA - (float)((lY + 1) - lY) * fSinA + (float)lX + 0.5f);
	ptPos[6].y = (long)((float)((lY + 1) - lY) * fCosA + (float)((lX - 1) - lX) * fSinA + (float)lY + 0.5f);

	ptPos[7].x = (long)((float)((lX) - lX) * fCosA - (float)((lY + 1) - lY) * fSinA + (float)lX + 0.5f);
	ptPos[7].y = (long)((float)((lY + 1) - lY) * fCosA + (float)((lX) - lX) * fSinA + (float)lY + 0.5f);

	ptPos[8].x = (long)((float)((lX + 1) - lX) * fCosA - (float)((lY + 1) - lY) * fSinA + (float)lX + 0.5f);
	ptPos[8].y = (long)((float)((lY + 1) - lY) * fCosA + (float)((lX + 1) - lX) * fSinA + (float)lY + 0.5f);

	//ptPos[0].x = (long)((float)((lX - 1) - lX) * fCosA - (float)((lY - 1) - lY) * fSinA + (float)lX);
	//ptPos[0].y = (long)((float)((lY - 1) - lY) * fCosA + (float)((lX - 1) - lX) * fSinA + (float)lY);

	//ptPos[1].x = (long)((float)((lX) - lX) * fCosA - (float)((lY - 1) - lY) * fSinA + (float)lX);
	//ptPos[1].y = (long)((float)((lY - 1) - lY) * fCosA + (float)((lX) - lX) * fSinA + (float)lY);

	//ptPos[2].x = (long)((float)((lX + 1) - lX) * fCosA - (float)((lY - 1) - lY) * fSinA + (float)lX);
	//ptPos[2].y = (long)((float)((lY - 1) - lY) * fCosA + (float)((lX + 1) - lX) * fSinA + (float)lY);

	//ptPos[3].x = (long)((float)((lX - 1) - lX) * fCosA - (float)((lY) - lY) * fSinA + (float)lX);
	//ptPos[3].y = (long)((float)((lY) - lY) * fCosA + (float)((lX - 1) - lX) * fSinA + (float)lY);

	//ptPos[4].x = (long)((float)((lX) - lX) * fCosA - (float)((lY) - lY) * fSinA + (float)lX);
	//ptPos[4].y = (long)((float)((lY) - lY) * fCosA + (float)((lX) - lX) * fSinA + (float)lY);

	//ptPos[5].x = (long)((float)((lX + 1) - lX) * fCosA - (float)((lY) - lY) * fSinA + (float)lX);
	//ptPos[5].y = (long)((float)((lY) - lY) * fCosA + (float)((lX + 1) - lX) * fSinA + (float)lY);

	//ptPos[6].x = (long)((float)((lX - 1) - lX) * fCosA - (float)((lY + 1) - lY) * fSinA + (float)lX);
	//ptPos[6].y = (long)((float)((lY + 1) - lY) * fCosA + (float)((lX - 1) - lX) * fSinA + (float)lY);

	//ptPos[7].x = (long)((float)((lX) - lX) * fCosA - (float)((lY + 1) - lY) * fSinA + (float)lX);
	//ptPos[7].y = (long)((float)((lY + 1) - lY) * fCosA + (float)((lX) - lX) * fSinA + (float)lY);

	//ptPos[8].x = (long)((float)((lX + 1) - lX) * fCosA - (float)((lY + 1) - lY) * fSinA + (float)lX);
	//ptPos[8].y = (long)((float)((lY + 1) - lY) * fCosA + (float)((lX + 1) - lX) * fSinA + (float)lY);

	for (DWORD dw = 0; dw < 9; ++dw)
	{
		dwPixel = GetBufferPixel(pBitmapBuffer, ptPos[dw].x, ptPos[dw].y);
		dwA[dw] = (dwPixel & 0xff000000) >> 24;
		dwR[dw] = (dwPixel & 0x00ff0000) >> 16;
		dwG[dw] = (dwPixel & 0x0000ff00) >> 8;
		dwB[dw] = (dwPixel & 0x000000ff);
	}

	dwReA = 0;
	dwReR = 0;
	dwReG = 0;
	dwReB = 0;
	dwCount = 0;
	for (DWORD dw = 0; dw < 9; ++dw)
	{
		if (dwA[dw] != 0)
		{
			dwReA += dwA[dw];
			++dwCount;
		}
	}
	if (dwCount > 0)
		dwReA /= dwCount;
	dwCount = 0;
	for (DWORD dw = 0; dw < 9; ++dw)
	{
		if (dwR[dw] != 0)
		{
			dwReR += dwR[dw];
			++dwCount;
		}
	}
	if (dwCount > 0)
		dwReR /= dwCount;
	dwCount = 0;
	for (DWORD dw = 0; dw < 9; ++dw)
	{
		if (dwG[dw] != 0)
		{
			dwReG += dwG[dw];
			++dwCount;
		}
	}
	if (dwCount > 0)
		dwReG /= dwCount;
	dwCount = 0;
	for (DWORD dw = 0; dw < 9; ++dw)
	{
		if (dwB[dw] != 0)
		{
			dwReB += dwB[dw];
			++dwCount;
		}
	}
	if (dwCount > 0)
		dwReB /= dwCount;

	return (((DWORD)dwReA << 24) | ((DWORD)dwReR << 16) | ((DWORD)dwReG << 8) | (DWORD)dwReB);
}



/*
 * 功能: 开始读取数据
 * 摘要: -
 * 参数: pFileData  - 数据指针
 *		 uiDataSize - 数据长度
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2009.06.02
 * 修改日志:
 */
bool CImage::ReadDataBegin(const BYTE * pFileData, unsigned int uiDataSize)
{
	if (pFileData == NULL)
		return false;

	m_pTmpFileData = pFileData;
	m_uiDataPoint  = 0;
	m_uiDataSize   = uiDataSize;

	return true;
}



/*
 * 功能: 读取数据
 * 摘要: -
 * 参数: pBuf		- 返回数据的指针
 *		 ulReadSize - 读取数据尺寸
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2009.06.02
 * 修改日志:
 */
bool CImage::ReadData(void * pBuf, unsigned long ulReadSize)
{
	if (m_uiDataPoint + ulReadSize > m_uiDataSize)
		return false;

	memcpy(pBuf, m_pTmpFileData + m_uiDataPoint, ulReadSize);
	m_uiDataPoint += ulReadSize;
	return true;
}



/*
 * 功能: 读取数据结束
 * 摘要: -
 * 参数: -
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2009.06.02
 * 修改日志:
 */
void CImage::ReadDataEnd()
{
	m_pTmpFileData = NULL;
	m_uiDataPoint  = 0;
	m_uiDataSize   = 0;
}