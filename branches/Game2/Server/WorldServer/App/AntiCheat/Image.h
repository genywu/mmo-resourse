/*
 * CImage.h
 * 功能: 图像操作类
 * 摘要: 该类的图像操作都是纯手工打造,不带任何附加运行库
 * 作者: lpf
 * 创建日期: 2009 - 06 - 02
 */

#ifdef TIMETEST
#pragma comment(lib, "winmm.lib")
#endif

#pragma once
#include <windows.h>
#include <stdio.h>
#include <math.h>

#ifndef SAFEDELETE
#define SAFEDELETE(p)			{ if (p)  { delete(p); (p) = NULL; } }								//安全删除指针对象
#endif

#ifndef SAFEDELETEARRAY
#define SAFEDELETEARRAY(ap)		{ if (ap)  { delete [] (ap); (ap) = NULL; } }						//安全删除指针数组对象
#endif

#define A2R(fA)		((float)fA * 3.14159265f / 180.0f)			//角度到弧度



class CImage
{
#pragma pack(1)
	// 位图文件头
	struct tagBMPFILEHEADER
	{
		WORD  bfType; 
		DWORD bfSize; 
		WORD  bfReserved1; 
		WORD  bfReserved2; 
		DWORD bfOffBits;
	};

	// 位图信息头
	struct tagBMPINFOHEADER
	{
		DWORD biSize;
		LONG  biWidth;
		LONG  biHeight;
		WORD  biPlanes;
		WORD  biBitCount;
		DWORD biCompression;
		DWORD biSizeImage;
		LONG  biXPelsPerMeter;
		LONG  biYPelsPerMeter;
		DWORD biClrUsed;
		DWORD biClrImportant;
	}; 
#pragma pack()

	char * m_pszImageFileName;	//图像资源文件(如果由内存载入则为空)

	DWORD * m_pBitmapOri;		//位图点阵
	DWORD * m_pBitmapBuffer;	//位图点阵缓冲数据
	DWORD   m_dwBitmapSize;		//位图尺寸

	DWORD m_dwWidth;			//宽度
	DWORD m_dwHeight;			//高度

	// 用于载入的临时文件相关
	const BYTE * m_pTmpFileData;
	unsigned int m_uiDataPoint;
	unsigned int m_uiDataSize;

public:
	CImage();			//构造函数
	~CImage();			//析构函数

	// 访问类成员函数
	DWORD GetPixel(long lX, long lY);						//得到像素
	DWORD GetBufferPixel(long lX, long lY);					//得到缓冲像素
	DWORD GetBufferPixel(DWORD * pBitmapBuffer, long lX, long lY);					//得到缓冲像素
	float GetAlphaValue(DWORD dwColor);						//得到传入颜色的ALPHA值
	DWORD GetWidth()			{ return m_dwWidth; }		//得到图像宽度
	DWORD GetHeight()			{ return m_dwHeight; }		//得到图像高度

	// 设定类成员函数
	void SetPixel(long lX, long lY, DWORD dwPixelValue);		//设定像素
	void SetBufferPixel(long lX, long lY, DWORD dwPixelValue);	//设定缓冲像素
	void SetAlphaMap(CImage * pImage, long lPosX, long lPosY);	//设定Alpha通道图

	// 文件操作函数
	bool ReadBitmapFromFile(const char * pszFileName);									//从文件读取BMP文件
	bool ReadBitmapFromMemory(const BYTE * pData, unsigned int uiDataLen);				//从内存读取BMP
	bool ReadFileToMemory(FILE * pFile, BYTE *& pData, DWORD & dwSize);					//将文件数据读到内存中
	bool ExportBitmapToFile(const char * pszFileName, WORD wBit = 32);					//导出BMP文件
	bool ExportJpegToFile(const char * pszFileName, int iQuality = 80);					//导出Jpeg文件
	bool ExportJpegToMemory(BYTE *& pData, DWORD & dwSize, int iQuality = 80);			//导出Jpeg到内存
	void ClearData();																	//清除图像数据

	// 图像操作
	void AlphaBlend(CImage * pImage, long lPosX, long lPosY, float fAlphaBlendValue);	//当前图像与另一个图像进行ALPHA混合
	void Rotate(float fRotAngle);														//旋转图像
	void Rotate(float fRotAngle, long lPosX, long lPosY);								//指定旋转中心坐标的旋转图像
	void RotateEx(float fRotAngle);														//旋转图像
	void RotateEx(float fRotAngle, long lPosX, long lPosY);								//指定旋转中心坐标的旋转图像
	void Zoom(int iScale);																//缩放图像
	void Blur();																		//模糊图像

private:
	bool  CreateBitmap(DWORD dwWidth, DWORD dwHeight, DWORD dwColor);					//创建位图
	void  FlipVertical(DWORD * szpBitmap);												//翻转位图
	DWORD AveragePixel1x1(DWORD * pBitmapBuffer, long lX, long lY);						//计算指定像素点周围像素均值
	DWORD AveragePixel3x3(DWORD * pBitmapBuffer, long lX, long lY);						//计算指定像素点周围像素均值
	DWORD AveragePixelEx(DWORD * pBitmapBuffer, long lX, long lY, float fCosA, float fSinA);		//计算指定像素点周围像素均值

	// 数据读取辅助
	bool ReadDataBegin(const BYTE * pFileData, unsigned int uiDataSize);				//开始读取数据
	bool ReadData(void * pBuf, unsigned long ulReadSize);								//读取数据
	void ReadDataEnd();																	//读取数据结束
};