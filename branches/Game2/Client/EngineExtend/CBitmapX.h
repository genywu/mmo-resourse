#pragma once

#include "../Engine/console.h"
#include "../Engine/utility.h"

#include "../EngineExtend/GameGraphics2D.h"

using namespace std;

class CBitmapX
{
public:
	CBitmapX();
	virtual ~CBitmapX();

	BOOL LoadBitmap(const char* filename);		// 读取一张贴图，然后自动切割
	void Draw(int x, int y, DWORD color = 0xffffffff);		// 显示整张贴图
	void Draw(int x, int y, float w, float h);	// 显示整张贴图（拉伸）
	void Draw(int x, int y, RECT& rect);		// 局部显示贴图
	void DrawBlend(int x, int y, RECT& rect);		// 局部显示贴图
	void Draw(int x, int y, float w, float h, RECT& rect, DWORD color = 0xffffffff);	// 局部显示贴图（拉伸）
	void DrawRotation(INT x, INT y, FLOAT angle, DWORD color = 0xffffffff);	// 画旋转的位图
	void DrawRotation(float x,float y ,FLOAT angle,DWORD color = 0xffffffff);
	void DrawRotation2(INT x, INT y, int rx,int ry,FLOAT angle,float fScale, DWORD color = 0xffffffff);//根据中心点画旋转的位图

	DWORD GetWidth()	{return m_dwWidth;}
	DWORD GetHeight()	{return m_dwHeight;}
	D3DCOLOR	GetPixel(int x, int y);
	DWORD* GetBitmap()	{return m_pBitmap;}

	int GetFirstTextureHandle();

	void SetClip(long left, long top, long right, long bottom) {m_rectClip.left=left; m_rectClip.top=top; m_rectClip.right=right; m_rectClip.bottom=bottom;}
	void SetClip(RECT *p) {m_rectClip.left=p->left; m_rectClip.top=p->top; m_rectClip.right=p->right; m_rectClip.bottom=p->bottom;}
	RECT *GetClip()	{return &m_rectClip;}
	void SetColor(DWORD color)	{m_color = color;}
	DWORD GetColor()			{return m_color;}
	string	GetFileName()	{return m_filename;}
private:
	enum
	{
		TEX_MINIMUM = 64
	};


	// 获取贴图信息
	BOOL ReadBitmapFile(const char *filename);

	void DivideTexture(RECT); // 分割纹理
	int GetOptionalSize(int); // 获取最佳大小

	DWORD * m_pBitmap; // 图形数据比特串
	list< pair<int,RECT> > m_TexArray; // 纹理序列
	DWORD m_dwMaxWidth, m_dwMaxHeight; // 显卡支持
	DWORD m_dwWidth, m_dwHeight; // 原图大小
	DWORD m_dwBitmapSize; // 原图大小
	RECT m_rectOrigin; // 原图位置
	//render::Interface *m_pInterface;
	//render::Interface::Layer2D *m_pLayer2D;
	GameGraphics2D * m_pGameGraphics2D;
	string m_filename;
	int m_nCount;
	
	DWORD m_color;		//用户自定义颜色值
	RECT m_rectClip;	// 剪裁区域 - 目前只对Draw原始图片有效

public:
	void IncRef() { ++m_nCount; }
	void DecRef() 
	{ 
		--m_nCount; 
		if ( m_nCount == 0 ) 
		{ 
			m_map.erase(m_map.find(m_filename)); delete this;
		} 
	}
	static map<string, CBitmapX *>	m_map;
	static CBitmapX * GetBitmapX(string filename,bool bRefresh=false);
	static CBitmapX * GetBmpFromBuf(BYTE *buf,long size);	// 因函数内部分配的内存 需要每次得出后，手工释放
	BOOL LoadJPGFromBuf(BYTE *buf,long size);				// 根据内存中字节读取一张贴图
	static void ReMoveAllBitmap();
};
