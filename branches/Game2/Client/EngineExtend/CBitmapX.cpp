
#include "stdafx.h"
#include "CBitmapX.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

map<string, CBitmapX*>	CBitmapX::m_map;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBitmapX::CBitmapX()
{
	m_pBitmap = NULL;
	m_dwMaxWidth = 
	m_dwMaxHeight =
	m_dwWidth =
	m_dwHeight =
	m_dwBitmapSize = 0;
//	m_pInterface = NULL;
//	m_pLayer2D = NULL;
	m_pGameGraphics2D = NULL;
	m_nCount = 1;
	m_color = 0xffffffff;
	m_rectClip.right = 0;
	m_rectClip.bottom = 0;
}

CBitmapX::~CBitmapX()
{
	m_pGameGraphics2D = GameGraphics2D::GetInstance();

	list< pair<int,RECT> >::iterator iter;
	iter = m_TexArray.begin();
	while ( iter != m_TexArray.end() )
	{
		m_pGameGraphics2D->DestroyImage(iter->first);
		iter++;
	}
}

void CBitmapX::ReMoveAllBitmap()
{
	map<string, CBitmapX *>::iterator itemp = m_map.begin();
	for(; itemp != m_map.end(); ++itemp)
	{
		delete (*itemp).second;
		(*itemp).second = NULL;
	}
	m_map.clear();
}

CBitmapX * CBitmapX::GetBitmapX(string filename,bool bRefresh)
{
	if (filename.empty())
		return NULL;
	std::transform(filename.begin(),filename.end(),filename.begin(),tolower);
	std::replace(filename.begin(),filename.end(),'/','\\');
	map<string,CBitmapX *>::iterator iter;

	iter = m_map.find(filename);

	if ( iter != m_map.end() && !bRefresh )
	{
		iter->second->IncRef();
		return iter->second;
	}
	else
	{
		if( iter != m_map.end() )
		{
			if( iter->second )
				delete iter->second;
		}
		CBitmapX * t = new CBitmapX;
		m_map[filename] = t;
		if (t->LoadBitmap(filename.c_str()))
            return t;
		else
			return NULL;
	}
}

CBitmapX * CBitmapX::GetBmpFromBuf(BYTE *buf,long size)
{
	CBitmapX * t = new CBitmapX;
	if (!(t->LoadJPGFromBuf(buf,size)))
	{
		//失败,清除t,返回null
		SAFE_DELETE(t);
		ASSERT("加载内存图片出错！");
		return NULL;
	}
	return t;
}

BOOL CBitmapX::LoadJPGFromBuf(BYTE *buf,long size)
{	
	render::Interface *pInterface = render::Interface::GetInstance();

	m_dwMaxWidth = pInterface->m_stInfo.d3dCaps.MaxTextureWidth;
	m_dwMaxHeight = pInterface->m_stInfo.d3dCaps.MaxTextureHeight;

	render::Texture *pTexture = new render::Texture;

	if (!pTexture->CreateJpgTextureFromMemory(buf,size,pInterface->GetDevice()))
	{
		SAFE_DELETE(pTexture);
		MessageBox(NULL,"错误","纹理内存读取失败",MB_OK);
		return FALSE;
	}

	m_dwBitmapSize = pTexture->GetBitmapSize();
	m_dwWidth = pTexture->GetWidth();
	m_dwHeight = pTexture->GetHeight();

	m_pBitmap = new DWORD[m_dwBitmapSize];
	memcpy(m_pBitmap, pTexture->GetBitmap(), sizeof(DWORD) * m_dwBitmapSize);

	pTexture->DestroyBitmap();
	SAFEDELETE(pTexture);

	m_rectOrigin.left = 0;
	m_rectOrigin.top = 0;
	m_rectOrigin.right = m_dwWidth;
	m_rectOrigin.bottom = m_dwHeight;

	DivideTexture(m_rectOrigin);

	// 创建各个分割纹理
	int extraX,extraY;
	RECT rectTemp;
	DWORD *pBitmapOld = m_pBitmap;

	list< pair<int,RECT> >::iterator iter;
	iter = m_TexArray.begin();
	while ( iter != m_TexArray.end() )
	{
		rectTemp = iter->second;
		DWORD *pdwBitsTemp = new DWORD[rectTemp.right*rectTemp.bottom];
		ZeroMemory(pdwBitsTemp, sizeof(DWORD)*rectTemp.right*rectTemp.bottom);
		DWORD *pdwBitsTempOld = pdwBitsTemp;
		m_pBitmap += rectTemp.top*m_rectOrigin.right+rectTemp.left;

		extraX = rectTemp.left + rectTemp.right - m_rectOrigin.right;
		if ( extraX < 0) extraX = 0;

		extraY = rectTemp.top + rectTemp.bottom - m_rectOrigin.bottom;
		if ( extraY < 0 ) extraY = 0;

		while ( rectTemp.bottom - extraY )
		{
			memcpy(pdwBitsTemp,m_pBitmap,(rectTemp.right-extraX)*sizeof(DWORD));
			m_pBitmap += m_rectOrigin.right;
			pdwBitsTemp += rectTemp.right;
			rectTemp.bottom--;
		}

		iter->first = m_pGameGraphics2D->CreateImage(iter->second.right,iter->second.bottom,pdwBitsTempOld);
		SAFEDELETEARRAY(pdwBitsTempOld);
		m_pBitmap = pBitmapOld;
		iter++;
	}
	//SAFEDELETEARRAY(m_pBitmap);
	return TRUE;
}

BOOL CBitmapX::ReadBitmapFile(const char *filename)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Texture *pTexture = new render::Texture;
	if (!pTexture->CreateBitmap(filename))
	{
		//MessageBox(NULL,filename,"文件打开失败",MB_OK);
		
		/// 修正此处的内存泄露 Add By X.Y 2009.11.20
		SAFEDELETE(pTexture);
		return FALSE;
	}

	m_dwBitmapSize = pTexture->GetBitmapSize();
	m_dwWidth = pTexture->GetWidth();
	m_dwHeight = pTexture->GetHeight();

	m_pBitmap = new DWORD[m_dwBitmapSize];
	memcpy(m_pBitmap, pTexture->GetBitmap(), sizeof(DWORD) * m_dwBitmapSize);

	pTexture->DestroyBitmap();
	SAFEDELETE(pTexture);

	return TRUE;
}



/*
 * 功能: 读取一张贴图，然后自动切割
 * 摘要: -
 * 参数: filename - 贴图文件的路径名
 * 返回值: 读入成功返回TURE,否则返回FALSE
 * 作者: -
 * 创建日期: -
 * 修改日志:
 *	2008.05.26 - lpf
 *		修改了位图的载入模式,目前是根据当前显卡对纹理的要求来决定是否采用分割纹理的方式载入位图
 *	2008.06.02 - lpf
 *		暂时注释了根据显卡进行载入图片的方式,所有图片资源仍然按照分割纹理的方式载入
 */
BOOL CBitmapX::LoadBitmap(const char* filename)
{
	if (!filename)
		return FALSE;
	m_filename = filename;
	std::transform(m_filename.begin(),m_filename.end(),m_filename.begin(),tolower);
	std::replace(m_filename.begin(),m_filename.end(),'/','\\');
	
	render::Interface *pInterface = render::Interface::GetInstance();

	m_dwMaxWidth = pInterface->m_stInfo.d3dCaps.MaxTextureWidth;
	m_dwMaxHeight = pInterface->m_stInfo.d3dCaps.MaxTextureHeight;

	//BitmapReadFileTGA(filename);
	//ReadTextureFile(filename);
	if (!ReadBitmapFile(filename))
	{
		return FALSE;
	}
	m_rectOrigin.left = 0;
	m_rectOrigin.top = 0;
	m_rectOrigin.right = m_dwWidth;
	m_rectOrigin.bottom = m_dwHeight;

	// 判断显卡功能,采用相应的载入方式
//	if (render::Interface::GetInstance()->m_stInfo.d3dCaps.TextureCaps & D3DPTEXTURECAPS_POW2)
		DivideTexture(m_rectOrigin);
	//else
	//	m_TexArray.push_back(pair<DWORD,RECT>(0, m_rectOrigin));

	// 创建各个分割纹理
	int extraX,extraY;
	RECT rectTemp;
	DWORD *pBitmapOld = m_pBitmap;
	list< pair<int,RECT> >::iterator iter;
	iter = m_TexArray.begin();
	while ( iter != m_TexArray.end() )
	{
		rectTemp = iter->second;
		DWORD *pdwBitsTemp = new DWORD[rectTemp.right*rectTemp.bottom];
		ZeroMemory(pdwBitsTemp, sizeof(DWORD)*rectTemp.right*rectTemp.bottom);
		DWORD *pdwBitsTempOld = pdwBitsTemp;
		m_pBitmap += rectTemp.top*m_rectOrigin.right+rectTemp.left;
		
		extraX = rectTemp.left + rectTemp.right - m_rectOrigin.right;
		if ( extraX < 0) extraX = 0;
		
		extraY = rectTemp.top + rectTemp.bottom - m_rectOrigin.bottom;
		if ( extraY < 0 ) extraY = 0;

		while ( rectTemp.bottom - extraY )
		{
			memcpy(pdwBitsTemp,m_pBitmap,(rectTemp.right-extraX)*sizeof(DWORD));
			m_pBitmap += m_rectOrigin.right;
			pdwBitsTemp += rectTemp.right;
			rectTemp.bottom--;
		}
		
		iter->first = m_pGameGraphics2D->CreateImage(iter->second.right,iter->second.bottom,pdwBitsTempOld);
		//delete [] pdwBitsTempOld;
		SAFE_DELETE_ARRAY(pdwBitsTempOld);
		m_pBitmap = pBitmapOld;
		iter++;
	}
	//delete [] m_pBitmap;
	SAFE_DELETE_ARRAY(m_pBitmap);

	return TRUE;
}



int CBitmapX::GetFirstTextureHandle()
{
	if(m_TexArray.empty())
		return 0;
	return m_TexArray.front().first;
}

// 显示整张贴图
void CBitmapX::Draw(int x, int y, DWORD color)
{
	pair<int,RECT> texTemp;
	list< pair<int,RECT> >::iterator iter;
	iter = m_TexArray.begin();
	if (color==0xffffffff&&m_color!=0xffffffff)
	{
		color = m_color;
	}
	// 无剪裁
	if (m_rectClip.right == 0)
	{
		while ( iter != m_TexArray.end() )
		{
			texTemp = (*iter);
			m_pGameGraphics2D->DrawImage(x+texTemp.second.left,y+texTemp.second.top,texTemp.second.right,texTemp.second.bottom,texTemp.first, color);
			iter++;
		}
	}
	else	// 带剪裁
	{
		while ( iter != m_TexArray.end() )
		{
			texTemp = (*iter);

			RECT rect = {x+texTemp.second.left, y+texTemp.second.top, texTemp.second.right + rect.left, texTemp.second.bottom + rect.top};

			if (rect.left > m_rectClip.right || rect.right < m_rectClip.left ||
				rect.top > m_rectClip.bottom || rect.bottom < m_rectClip.top)
			{
				iter++;
				continue;
			}

			long lx = rect.left;
			long ly = rect.top;
			RECT rectDraw = texTemp.second;

			if (lx < m_rectClip.left)
			{
				rectDraw.left += m_rectClip.left - lx;
				lx = m_rectClip.left;
			}
			if (rect.right > m_rectClip.right)
			{
				rectDraw.right -= rect.right - m_rectClip.right;
			}
			if (ly < m_rectClip.top)
			{
				rectDraw.top += m_rectClip.top - ly;
				ly = m_rectClip.top;
			}
			if (rect.bottom > m_rectClip.bottom)
			{
				rectDraw.bottom -= rect.bottom - m_rectClip.bottom;
			}

			m_pGameGraphics2D->DrawImageSubRect(lx, ly, &rectDraw, texTemp.first, color);
			iter++;
		}
	}
}


// 显示整张贴图（拉伸）
void CBitmapX::Draw(int x, int y, float w, float h)
{
	w = w/m_rectOrigin.right;
	h = h/m_rectOrigin.bottom;
	pair<int,RECT> texTemp;
	list< pair<int,RECT> >::iterator iter;
	iter = m_TexArray.begin();
	while ( iter != m_TexArray.end() )
	{
		texTemp = (*iter);
		m_pGameGraphics2D->DrawImage((INT)(x+w*texTemp.second.left),(INT)(y+h*texTemp.second.top), 
			(INT)(w*(texTemp.second.left+texTemp.second.right))-(INT)(w*texTemp.second.left),
			(INT)(h*(texTemp.second.top+texTemp.second.bottom))-(INT)(h*texTemp.second.top),
			texTemp.first,m_color);
		iter++;
	}
}

// 局部显示贴图
void CBitmapX::Draw(int x, int y, RECT& rect)
{
	pair<int,RECT> texTemp;
	list< pair<int,RECT> >::iterator iter;
	iter = m_TexArray.begin();
	while ( iter != m_TexArray.end() )
	{
		texTemp = (*iter);
		
		// 判定此分块是否有涉及
		if ( 
			texTemp.second.left <  rect.left + rect.right && texTemp.second.top < rect.top + rect.bottom 
			&& 
			texTemp.second.left + texTemp.second.right > rect.left && texTemp.second.top + texTemp.second.bottom > rect.top
			)
		{
			// 计算切割部分
			RECT rectTemp;
			rectTemp.left = rect.left - texTemp.second.left;
			rectTemp.top = rect.top - texTemp.second.top;
			if ( rectTemp.left < 0 ) rectTemp.left = 0;
			if ( rectTemp.top < 0 ) rectTemp.top = 0;
			
			rectTemp.right = rect.left + rect.right - texTemp.second.left;
			if ( rectTemp.right > texTemp.second.right ) rectTemp.right = texTemp.second.right;
			
			rectTemp.bottom = rect.top + rect.bottom - texTemp.second.top;
			if ( rectTemp.bottom > texTemp.second.bottom ) rectTemp.bottom = texTemp.second.bottom;

			// draw
			m_pGameGraphics2D->DrawImageSubRectEx(x + texTemp.second.left + rectTemp.left - rect.left,
				y + texTemp.second.top + rectTemp.top - rect.top, 
				&rectTemp, texTemp.first,m_color);
		}	
		iter++;
	}
}

void CBitmapX::DrawBlend(int x, int y, RECT& rect)
{
	pair<int,RECT> texTemp;
	list< pair<int,RECT> >::iterator iter;
	iter = m_TexArray.begin();
	while ( iter != m_TexArray.end() )
	{
		texTemp = (*iter);
		
		// 判定此分块是否有涉及
		if ( 
			texTemp.second.left <  rect.left + rect.right && texTemp.second.top < rect.top + rect.bottom 
			&& 
			texTemp.second.left + texTemp.second.right > rect.left && texTemp.second.top + texTemp.second.bottom > rect.top
			)
		{
			// 计算切割部分
			RECT rectTemp;
			rectTemp.left = rect.left - texTemp.second.left;
			rectTemp.top = rect.top - texTemp.second.top;
			if ( rectTemp.left < 0 ) rectTemp.left = 0;
			if ( rectTemp.top < 0 ) rectTemp.top = 0;
			
			rectTemp.right = rect.left + rect.right - texTemp.second.left;
			if ( rectTemp.right > texTemp.second.right ) rectTemp.right = texTemp.second.right;
			
			rectTemp.bottom = rect.top + rect.bottom - texTemp.second.top;
			if ( rectTemp.bottom > texTemp.second.bottom ) rectTemp.bottom = texTemp.second.bottom;

			// draw
			m_pGameGraphics2D->DrawImageSubRectBlend(x + texTemp.second.left + rectTemp.left - rect.left,
				y + texTemp.second.top + rectTemp.top - rect.top, 
				&rectTemp, texTemp.first);
		}	
		iter++;
	}
}

D3DCOLOR CBitmapX::GetPixel(int x, int y)
{
	render::Texture * pTex;
	render::Interface *pInterface = render::Interface::GetInstance();
	pair<int,RECT> texTemp;
	list< pair<int,RECT> >::iterator iter;
	iter = m_TexArray.begin();

	while ( iter != m_TexArray.end() )
	{
		texTemp = (*iter);

		// 判定此分块是否有涉及
		if ( 
			texTemp.second.left <=  x 
			&& 
			texTemp.second.top <= y 
			&& 
			texTemp.second.left + texTemp.second.right > x
			&&
			texTemp.second.top + texTemp.second.bottom > y
			)
		{
			pTex = pInterface->GetTexture(texTemp.first);
			return pTex->GetPixel(x-texTemp.second.left,y-texTemp.second.top);
		}

		iter++;
	}
	return D3DCOLOR_XRGB(255,255,255);
}   

// 局部显示贴图（拉伸）
void CBitmapX::Draw(int x, int y, float w, float h, RECT& rect, DWORD color)
{
	w = w/rect.right;
	h = h/rect.bottom;
	pair<int,RECT> texTemp;
	list< pair<int,RECT> >::iterator iter;
	iter = m_TexArray.begin();
	while ( iter != m_TexArray.end() )
	{
		texTemp = (*iter);

		// 判定此分块是否有涉及
		if ( 
			texTemp.second.left <  rect.left + rect.right && texTemp.second.top < rect.top + rect.bottom 
			&& 
			texTemp.second.left + texTemp.second.right > rect.left && texTemp.second.top + texTemp.second.bottom > rect.top
			)
		{
			// 计算切割部分		
			RECT rectTemp;
			rectTemp.left = rect.left - texTemp.second.left;
			rectTemp.top = rect.top - texTemp.second.top;
			if ( rectTemp.left < 0 ) rectTemp.left = 0;
			if ( rectTemp.top < 0 ) rectTemp.top = 0;

			rectTemp.right = rect.left + rect.right - texTemp.second.left;
			if ( rectTemp.right > texTemp.second.right ) rectTemp.right = texTemp.second.right;

			rectTemp.bottom = rect.top + rect.bottom - texTemp.second.top;
			if ( rectTemp.bottom > texTemp.second.bottom ) rectTemp.bottom = texTemp.second.bottom;

			m_pGameGraphics2D->DrawImageSubRect(
				(INT)(x + w*(texTemp.second.left + rectTemp.left - rect.left)), 
				(INT)(y + h*(texTemp.second.top + rectTemp.top - rect.top)),
				(INT)(w*(texTemp.second.left + rectTemp.right - rect.left))- 
				(INT)(w*(texTemp.second.left + rectTemp.left - rect.left)),  
				(INT)(h*(texTemp.second.top + rectTemp.bottom - rect.top))- 
				(INT)(h*(texTemp.second.top + rectTemp.top - rect.top)),  
				&rectTemp, texTemp.first, color);
		}	
		iter++;
	}
}

//////////////////////////////////////////////////////////////////////////
// 获取大于size的最小2n次方数
int CBitmapX::GetOptionalSize(int size)
{
	int t = 0x01;
	while(t < size)
		t <<= 1;
	return t;
}

//////////////////////////////////////////////////////////////////////////
// 四叉树分割
void CBitmapX::DivideTexture(RECT rectTex)
{
	if (rectTex.right <=0 || rectTex.bottom <= 0)
		return;
	
	//if ( rectTex.right < TEX_MINIMUM || rectTex.bottom < TEX_MINIMUM )	
	//{
	//	bool isAlignX = rectTex.right > rectTex.bottom ? true : false;

	//	int n;
	//	RECT rectTemp;
	//	ZeroMemory(&rectTemp,sizeof(rectTemp));
	//	
	//	if ( isAlignX )
	//	{
	//		n = GetOptionalSize(rectTex.right);
	//		n =>> 1;
	//	}
	//	else
	//	{
	//		n = GetOptionalSize(rectTex.bottom);
	//		n =>> 1;
	//	}

	//	for ( int i = 0; i < n; ++i )
	//	{
	//		if ( isAlignX )
	//		{
 //               rectTemp.left = rectTex.left + TEX_MINIMUM*n;
	//			rectTemp.right = TEX_MINIMUM;
	//			rectTemp.top = rectTex.top;
	//			rectTemp.bottom = TEX_MINIMUM;

	//			// 压入当前分割
	//			m_TexArray.push_back(pair<DWORD,RECT>(0,rectTemp));
	//		}
	//		else
	//		{
	//			rectTemp.left = rectTex.left;
	//			rectTemp.right = TEX_MINIMUM;
	//			rectTemp.top = rectTex.top + TEX_MINIMUM*n;
	//			rectTemp.bottom = TEX_MINIMUM;

	//			// 压入当前分割
	//			m_TexArray.push_back(pair<DWORD,RECT>(0,rectTemp));
	//		}
	//	}
	//
	//	return;
	//}

	RECT rectMain, rectCorner, rectDown, rectRight;

//	ZeroMemory(&rectExtra,sizeof(rectExtra));
	
	rectMain.left = rectTex.left;
	rectMain.top  = rectTex.top;
	
	// 横起弄
	DWORD t = GetOptionalSize(rectTex.right);
	
	if ((t > TEX_MINIMUM) && (t != rectTex.right))
		t >>= 1;
		
	while (t > m_dwMaxWidth)
		t >>= 1;

	rectMain.right = t;
	
	// 竖起弄
	t = GetOptionalSize(rectTex.bottom);

	if ((t > TEX_MINIMUM) && (t != rectTex.bottom))
		t >>= 1;
	
	while (t > m_dwMaxHeight)
		t >>= 1;

	rectMain.bottom = t;
	
	//if ( render::Interface::GetInstance()->m_stInfo.d3dCaps.TextureCaps & D3DPTEXTURECAPS_SQUAREONLY )
	//{
	//	rectMain.bottom = rectMain.right = rectMain.bottom > rectMain.right ? rectMain.right : rectMain.bottom;
	//}

	// 压入当前分割
	m_TexArray.push_back(pair<DWORD,RECT>(0, rectMain));
	
	// 计算剩余部分
	rectDown.left	= rectMain.left;
	rectDown.top	= rectMain.top + rectMain.bottom;
	rectDown.right	= rectMain.right;
	rectDown.bottom = rectTex.bottom - rectMain.bottom;

	rectRight.left	 = rectMain.left + rectMain.right;
	rectRight.top	 = rectMain.top;
	rectRight.right	 = rectTex.right - rectMain.right;
	rectRight.bottom = rectMain.bottom;
	
	rectCorner.left	  = rectMain.left + rectMain.right;
	rectCorner.top	  = rectMain.top + rectMain.bottom;
	rectCorner.right  = rectTex.right - rectMain.right;
	rectCorner.bottom = rectTex.bottom - rectMain.bottom;
	
	//// 处理额外块
	//if ( (rectRight.right != 0) && (rectRight.right < TEX_MINIMUM) )
	//{
	//	rectExtra.left = rectRight.left;
	//	rectExtra.right = GetOptionalSize(rectTex.right-rectMain.right);
	//	rectExtra.top = rectRight.top;
	//	rectExtra.bottom = rectRight.bottom;

	//	// 压入Extra分割
	//	m_TexArray.push_back(pair<DWORD,RECT>(0,rectExtra));
	//	
	//	// 修正Right
	//	rectRight.right = 0;
	//}

	//if ( rectDown.bottom != 0 && (rectDown.bottom < TEX_MINIMUM) )
	//{
	//	rectExtra.left = rectDown.left;
	//	rectExtra.right = rectMain.right;
	//	rectExtra.top = rectDown.top;
	//	rectExtra.bottom = GetOptionalSize(rectDown.bottom);

	//	// 压入Extra分割
	//	m_TexArray.push_back(pair<DWORD,RECT>(0,rectExtra));
	//	
	//	// 修正Down
	//	rectDown.left += rectMain.right;
	//	rectDown.right = rectTex.right - rectMain.right;
	//}
	
	// 递规
//	if ( rectMain)
	DivideTexture(rectCorner);
	DivideTexture(rectDown);
	DivideTexture(rectRight);
}

// 画旋转的位图
void CBitmapX::DrawRotation(INT x, INT y, FLOAT angle, DWORD color)
{
	if(GetFirstTextureHandle())
		m_pGameGraphics2D->DrawImageRotation(x, y, angle, GetFirstTextureHandle(), color);
}
void CBitmapX::DrawRotation(float x,float y ,FLOAT angle,DWORD color /* = 0xffffffff */)
{
	if(GetFirstTextureHandle())
		m_pGameGraphics2D->DrawImageRotation(x,y,angle,GetFirstTextureHandle(),color);
}
void CBitmapX::DrawRotation2(INT x, INT y,INT rx,INT ry,FLOAT angle, float fScale,DWORD color)
{
	if(GetFirstTextureHandle())
		m_pGameGraphics2D->DrawImageRotation2(x, y, rx,ry,angle, fScale,GetFirstTextureHandle(), color);
}


