#include "stdafx.h"
#include "RollLantern.h"
#include "../../EngineExtend/CBitmapX.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*
* 功能: 构造函数
* 摘要: -
* 参数: -
* 返回值: -
* 作者: lpf
* 创建日期: 2009.01.05
*	2009.01.07 - lpf
*		增加了FontInfo的初始化赋值
*/
CRollLantern::CRollLantern(void)
{
	m_fSpeed	  = 0.1f;				//内容滚动速度(毫秒/像素)
	m_dwNumLoop   = 1;					//循环次数

	// 走马灯绘制坐标(即背景图片绘制坐标)
	m_ptPos.x = 100;
	m_ptPos.y = 100;

	m_lContentOffsetHeight = 5;
	m_pBackGroundPic	   = NULL;				//背景图片
	m_bIsFinished		   = true;				//走马灯已经显示完成
	m_lContentOffset	   = 0;					//内容绘制偏移
	m_lContentOffsetLeft   = 0;
	m_lContentOffsetRight  = 0;
	m_dwStartTime		   = 0;					//走马灯开始时间
	m_dwLoopCount		   = 0;					//内容循环计数
	m_fAlphaPer			   = 0.0f;
	m_dwAlphaTimeLen	   = 5000;				//渐变时间长度
	m_dwAlphaStartTime	   = 0;
	m_eAlphaMode		   = ALPHA_FADE_NONE;	//走马灯出现消失的渐变模式

	m_stFontInfo.iStyleIndex	= 0;
	m_stFontInfo.bBorder		= false;
	m_stFontInfo.bShadow		= true;
	m_stFontInfo.dwShadowOffset = 1;
	m_stFontInfo.fScale			= 1.0f;
	m_stFontInfo.dwColorFont	= 0xffff0000;
	m_stFontInfo.dwColorBorder	= 0xff000000;
	m_stFontInfo.dwColorShadow	= 0x80000000;

	m_pOffscreenLeft  = NULL;
	m_pOffscreenRight = NULL;

	// 裁剪的坐标(该坐标在相对坐标系,相对于走马灯绘制坐标)
	SetClip(150, 650);
}



/*
* 功能: 析构函数
* 摘要: -
* 参数: -
* 返回值: -
* 作者: lpf
* 创建日期: 2009.01.05
*/
CRollLantern::~CRollLantern(void)
{
	//SAFE_DECREF(m_pBackGroundPic);
	SAFERELEASE(m_pOffscreenLeft);
	SAFERELEASE(m_pOffscreenRight);
}



/*
* 功能: 得到内容滚动偏移最左值
* 摘要: 这个坐标在游戏主窗体坐标系
* 参数: -
* 返回值: 偏移最左值
* 作者: lpf
* 创建日期: 2009.01.05
*	2009.01.09 - lpf
*		为内容最左偏移值加了-1的修正，以适应现在的描边字体显示
*/
float CRollLantern::GetContentOffsetLeft()
{
	m_lContentOffsetLeft = m_ptPos.x + (m_lClipLeft - (long)m_sContent.length() * 8) - 1;
	return (float)m_lContentOffsetLeft;
}



/*
* 功能: 得到内容滚动偏移最右值
* 摘要: 这个坐标在游戏主窗体坐标系
* 参数: -
* 返回值: 偏移最右值
* 作者: lpf
* 创建日期: 2009.01.05
*/
float CRollLantern::GetContentOffsetRight()
{
	m_lContentOffsetRight = m_ptPos.x + m_lClipRight;
	return (float)m_lContentOffsetRight;
}



/*
* 功能: 设定走马灯绘制坐标(即背景图片绘制坐标)
* 摘要: -
* 参数: -
* 返回值: -
* 作者: lpf
* 创建日期: 2009.01.05
*/
void CRollLantern::SetPos(POINT & pt)
{
	m_ptPos = pt;

	GetContentOffsetLeft();
	GetContentOffsetRight();
	UpdateClipRect();
}



/*
* 功能: 设定裁剪的坐标(该坐标在相对坐标系,相对于走马灯绘制坐标)
* 摘要: -
* 参数: -
* 返回值: -
* 作者: lpf
* 创建日期: 2009.01.05
*/
void CRollLantern::SetClip(long lLeft, long lRight)
{
	m_lClipLeft  = lLeft;
	m_lClipRight = lRight;

	GetContentOffsetLeft();
	GetContentOffsetRight();
	UpdateClipRect();
}



/*
* 功能: 设定显示内容
* 摘要: 该函数会更新一些计算数据
* 参数: -
* 返回值: -
* 作者: lpf
* 创建日期: 2009.01.05
*/
void CRollLantern::SetContent(const char * psz)
{
	m_sContent = psz;

	GetContentOffsetLeft();
	GetContentOffsetRight();
}



/*
* 功能: 设定背景图片
* 摘要: -
* 参数: pszFilePathName - 背景图片路径名
* 返回值: -
* 作者: lpf
* 创建日期: 2009.01.05
*/
bool CRollLantern::SetBGPic(const char * pszFilePathName)
{
	//SAFE_DECREF(m_pBackGroundPic);

	m_pBackGroundPic = CBitmapX::GetBitmapX(pszFilePathName);
	if (m_pBackGroundPic == NULL)
		return false;
	else
		return true;
}



/*
* 功能: 开始显示走马灯
* 摘要: -
* 参数: -
* 返回值: -
* 作者: lpf
* 创建日期: 2009.01.05
* 修改日志:
*/
void CRollLantern::Start()
{
	m_bIsFinished	   = false;
	m_dwAlphaStartTime = timeGetTime();
	m_eAlphaMode	   = ALPHA_FADE_IN;
	m_dwLoopCount	   = 0;
	m_lContentOffset   = 0;
}



/*
* 功能: 更新列表中的对象
* 摘要: 完成渐出操作的对象需要删除
* 参数: -
* 返回值: -
* 作者: lpf
* 创建日期: 2009.01.05
* 修改日志:
*/
void CRollLantern::Update()
{
	if (m_bIsFinished)
		return;

	DWORD dwElapseTime  = 0;
	DWORD dwCurrentTime = timeGetTime();

	// 处理不同的阶段
	switch (m_eAlphaMode)
	{
	case ALPHA_FADE_IN:
		{
			dwElapseTime = dwCurrentTime - m_dwAlphaStartTime;
			m_fAlphaPer  = (float)dwElapseTime / (float)m_dwAlphaTimeLen;

			if (m_fAlphaPer > 1.0f)
			{
				m_fAlphaPer	  = 1.0f;
				m_eAlphaMode  = ALPHA_FADE_NONE;
				m_dwStartTime = dwCurrentTime;
			}
		}
		break;
	case ALPHA_FADE_NONE:
		{
			dwElapseTime	 = dwCurrentTime - m_dwStartTime;
			m_lContentOffset = (long)(m_fSpeed * (float)dwElapseTime);
			m_lContentOffset *= -1;

			if (m_lContentOffset + m_lContentOffsetRight < m_lContentOffsetLeft)
			{
				m_dwLoopCount++;

				// 检测是否循环完毕
				if (m_dwLoopCount == m_dwNumLoop)
				{
					m_dwAlphaStartTime = dwCurrentTime;
					m_eAlphaMode	   = ALPHA_FADE_OUT;
					m_lContentOffset   = m_lContentOffsetLeft - m_lContentOffsetRight;
				}else
				{
					m_lContentOffset = 0;
					m_dwStartTime	 = dwCurrentTime;
				}
			}
		}
		break;
	case ALPHA_FADE_OUT:
		{
			dwElapseTime = dwCurrentTime - m_dwAlphaStartTime;
			m_fAlphaPer  = (float)((float)m_dwAlphaTimeLen - (float)dwElapseTime) / (float)m_dwAlphaTimeLen;

			if (m_fAlphaPer < 0.0f)
			{
				m_fAlphaPer		 = 0.0f;
				m_eAlphaMode	 = ALPHA_FADE_NONE;
				m_dwLoopCount	 = 0;
				m_lContentOffset = 0;
				m_bIsFinished	 = true;
			}
		}
		break;
	}
}



/*
* 功能: 渲染列表中的对象
* 摘要: -
* 参数: -
* 返回值: -
* 作者: lpf
* 创建日期: 2009.01.05
* 修改日志:
*	2009.01.07 - lpf
*		采用FontInfo来进行字体内容的显示
*	2009.01.09 - lpf
*		在矩形拷贝时,考虑到了失败的情况.以处理设备重置时此处可能拷贝失败的情况
*/
void CRollLantern::Draw()
{
	if (m_bIsFinished)
		return;

	LPDIRECT3DSURFACE9 pRenderTarget = NULL;
	LPDIRECT3DDEVICE9  pD3DDevice	 = render::Interface::GetInstance()->GetDevice();

	DWORD dwAlpha = ((DWORD)(m_fAlphaPer * 255.0f)) << 24;
	DWORD dwColor = 0xffffffff;

	// 绘制背景图片
	if (m_pBackGroundPic != NULL)
		m_pBackGroundPic->Draw(m_ptPos.x, m_ptPos.y, (dwAlpha | 0x00ffffff));

	//render::Interface::GetInstance()->EndScene();
	pD3DDevice->GetRenderTarget(0, &pRenderTarget);

	if (pD3DDevice->StretchRect(pRenderTarget, &m_rcSourceLeft, m_pOffscreenLeft, NULL, D3DTEXF_NONE) != D3D_OK)
		CreateOffscreenLeft();
	if (pD3DDevice->StretchRect(pRenderTarget, &m_rcSourceRight, m_pOffscreenRight, NULL, D3DTEXF_NONE) != D3D_OK)
		CreateOffscreenRight();

	// 绘制字符串内容
	POINT lDrawPos = { m_lContentOffset + m_lContentOffsetRight, m_ptPos.y + m_lContentOffsetHeight + 1 };
	render::Interface::GetInstance()->GetLayer2D()->DrawText(lDrawPos.x, lDrawPos.y, m_sContent.c_str(), &m_stFontInfo);

	if (pD3DDevice->StretchRect(m_pOffscreenLeft, NULL, pRenderTarget, &m_rcSourceLeft, D3DTEXF_NONE) != D3D_OK)
		CreateOffscreenLeft();
	if (pD3DDevice->StretchRect(m_pOffscreenRight, NULL, pRenderTarget, &m_rcSourceRight, D3DTEXF_NONE) != D3D_OK)
		CreateOffscreenRight();

	SAFERELEASE(pRenderTarget);
}



/*
* 功能: 更新裁剪矩形
* 摘要: -
* 参数: -
* 返回值: -
* 作者: lpf
* 创建日期: 2009.01.05
* 修改日志:
*	2009.01.09 - lpf
*		调整了公告栏的裁剪矩形,以适应18像素的字体(由于现在是描边显示)
*/
void CRollLantern::UpdateClipRect()
{
	m_rcSourceLeft.left   = 0;
	m_rcSourceLeft.top	  = m_ptPos.y + m_lContentOffsetHeight;
	m_rcSourceLeft.right  = m_ptPos.x + m_lClipLeft;
	m_rcSourceLeft.bottom = m_rcSourceLeft.top + 18;

	m_rcSourceRight.left   = m_ptPos.x + m_lClipRight;
	m_rcSourceRight.top	   = m_rcSourceLeft.top;
	m_rcSourceRight.right  = render::Interface::GetInstance()->GetSetting()->iWndWidth;
	m_rcSourceRight.bottom = m_rcSourceLeft.bottom;

	CreateOffscreenLeft();
	CreateOffscreenRight();
}



/*
* 功能: 创建左方离屏表面
* 摘要: -
* 参数: -
* 返回值: -
* 作者: lpf
* 创建日期: 2009.01.09
* 修改日志:
*/
bool CRollLantern::CreateOffscreenLeft()
{
	SAFERELEASE(m_pOffscreenLeft);

	if (render::Interface::GetInstance()->GetDevice()->CreateRenderTarget(m_rcSourceLeft.right - m_rcSourceLeft.left,
		18,
		render::Interface::GetInstance()->GetSetting()->d3dfmtBackBuffer,
		D3DMULTISAMPLE_NONE,
		0,
		TRUE,
		&m_pOffscreenLeft,
		NULL) != D3D_OK)
		return false;
	else
		return true;
}



/*
* 功能: 创建右方离屏表面
* 摘要: -
* 参数: -
* 返回值: -
* 作者: lpf
* 创建日期: 2009.01.09
* 修改日志:
*/
bool CRollLantern::CreateOffscreenRight()
{
	SAFERELEASE(m_pOffscreenRight);

	if (render::Interface::GetInstance()->GetDevice()->CreateRenderTarget(m_rcSourceRight.right - m_rcSourceRight.left,
		18,
		render::Interface::GetInstance()->GetSetting()->d3dfmtBackBuffer,
		D3DMULTISAMPLE_NONE,
		0,
		TRUE,
		&m_pOffscreenRight,
		NULL) != D3D_OK)
		return false;
	else
		return true;
}