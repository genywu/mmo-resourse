#pragma once

#include "../Engine/singleton.h"

#include "gamemodel.h"

//2D辅助函数库接口
class GameGraphics2D
{
	DECLARE_SINGLETON(GameGraphics2D);

	DWORD m_dwFontTextureHandle;//[2];
	DWORD m_dwFontTextureWidth;//[2];
	DWORD m_dwFontTextureHeight;//[2];
	DWORD m_dwFontWidth;//[2];
	DWORD m_dwFontHeight;//[2];
	static DWORD m_dwFogEnable;

	D3DTEXTUREFILTERTYPE m_eTextureFilterType;			//纹理过滤类型

public:
	GameGraphics2D(void);
	~GameGraphics2D(void);

	DWORD CreateImage(const char *szFileName);
	DWORD CreateImage(DWORD dwWidth,DWORD dwHeight,DWORD *pBitmap);
	void  DestroyImage(DWORD dwHandle);

	void DrawImage(int iX,int iY,DWORD dwImageHandle,D3DCOLOR d3dColor = 0XFFFFFFFF);
	void DrawImageBlend(int iX,int iY,DWORD dwImageHandle,D3DCOLOR d3dColor = 0XFFFFFFFF,float fScale = 1.0f);
	void DrawImage(int iX,int iY,int iWidth,int iHeight,DWORD dwImageHandle,D3DCOLOR color = 0XFFFFFFFF);
	void DrawImageBlend(int iX,int iY,int iWidth,int iHeight,DWORD dwImageHandle,D3DCOLOR d3dColor);
	void DrawImageFlip(int iX,int iY,int iWidth,int iHeight,DWORD dwImageHandle,D3DCOLOR color = 0XFFFFFFFF);
	void DrawImage(int iX,int iY,int iWidth,int iHeight,float fUOffset,float fVOffset,float fScale,DWORD dwImageHandle,D3DCOLOR color = 0XFFFFFFFF);
	void DrawImageSkew(int iX,int iY,int iWidth,int iHeight,float fUOffset,float fVOffset,float fScale,DWORD dwImageHandle,D3DCOLOR color = 0XFFFFFFFF);
	void DrawImageSubRect(int iX,int iY,const RECT *pRect,DWORD dwImageHandle,D3DCOLOR color = 0XFFFFFFFF);
	void DrawImageSubRect(int iX,int iY,int iWidth,int iHeight,const RECT *pRect,DWORD dwImageHandle,D3DCOLOR color = 0XFFFFFFFF);
	void DrawImageSubRectEx(int iX,int iY,const RECT *pRect,DWORD dwImageHandle,D3DCOLOR color = 0XFFFFFFFF);
	void DrawImageSubRectBlend(int iX,int iY,const RECT *pRect,DWORD dwImageHandle,D3DCOLOR color = 0XFFFFFFFF);
	void DrawImageSubRectBlend(int iX,int iY,int iWidth,int iHeight,const RECT *pRect,DWORD dwImageHandle,D3DCOLOR color = 0XFFFFFFFF);
	void DrawImageRotation(int iX,int iY,float fAngle,DWORD dwImageHandle,D3DCOLOR color = 0xFFFFFFFF);
	void DrawImageRotation(float fX,float fY,float fAngel,DWORD dwImageHandle,D3DCOLOR color = 0xFFFFFFFF);

	void DrawImageRotation2(int iX,int iY,int iRX,int iRY,float fAngle,float fScale,DWORD dwImageHandle,D3DCOLOR color);
	void DrawGameModel(int iX,int iY,GameModel * pGameModel,float fScale,float fRotation,GameModel::AnimInfo *pAnimInfo);
	void DrawGameModel(int iX,int iY,GameModel * pGameModel,float fScale,float fRotation,GameModel::AnimInfo *pAnimInfo,D3DXMATRIX &matmodelworld,D3DXMATRIX &matworld);
	
	void DrawLine(int iStartX,int iStartY,int iEndX,int iEndY,D3DCOLOR color);
	void DrawSolidQuad(int iX,int iY,int iWidth,int iHeight,D3DCOLOR color = 0XFFFFFFFF);

	// add by lpf 2007-11-28
	void DrawText(int iX, int iY, const char * pszString, PFONTINFO pFontInfo = NULL);				//根据字体信息来输出文本

	// add by lpf 2008-01-03
	void DrawText(int iX, int iY, const char * pszString, int iLen, PFONTINFO pFontInfo = NULL);	//根据字体信息来输出指定长度的文本

	// modify by lpf 2007-11-27 - 增加了一个参数iStyleIndex
	void OutPutText(int iX, int iY, const char *pszString, int iLen, D3DCOLOR color, int iStyleIndex = 0, float fScale = 1.0f);
	void NumberOut(int iX,int iY,const char *pszString,int iLen,D3DCOLOR color,float fScale = 1.0f);
	void NumberOutFilter(int iX,int iY,const char *pszString,int iLen,D3DCOLOR color,float fScale = 1.0f);
	void NumberOutBlend(int iX,int iY,const char *pszString,int iLen,D3DCOLOR color,float fScale);
	void NumberOut1(int iX,int iY,const char *pszString,int iLen,D3DCOLOR color,float fScale = 1.0f);

	DWORD GetFontWidth(int iStyleIndex);

	DWORD GetNumberWidth() const { return m_dwFontWidth;}//[0];}
	DWORD GetNumberHeight() const { return m_dwFontHeight;}//[0];}
	DWORD GetNumberWidth1() const { return m_dwFontWidth;}//[1];}
	DWORD GetNumberHeight1() const { return m_dwFontHeight;}//[1];}

	void Create();
	void Destroy();

	//ADD by 彭谦
	static void SetUIModelRendState();
	static void RecoverFogState();
	//ADD by 彭谦

};
