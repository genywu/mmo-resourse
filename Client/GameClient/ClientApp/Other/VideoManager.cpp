#include "stdafx.h"
#include "VideoManager.h"
#include "../../Engine/Render.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CVideoManager * CVideoManager::s_pInstance = NULL;

/*
 * 功能: 构造函数
 * 摘要: -
 * 参数: -
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2008.08.14
 * 修改日志:
 */
CVideoManager::CVideoManager()
{
	m_iPlayThreadCount = 0;
	m_iCurrentPlaying  = -1;
	m_bIsFullScreen	   = false;
	m_bIsUniquelyPlaying = true;

	m_pClipList = NULL;

	ZeroMemory(m_pVideosArray, sizeof(CVideo *) * NUM_VIDEO);
	ZeroMemory(m_uiVideosLoop, sizeof(int) * NUM_VIDEO);
}



/*
 * 功能: 析构函数
 * 摘要: -
 * 参数: -
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2008.08.14
 * 修改日志:
 */
CVideoManager::~CVideoManager()
{
	m_iPlayThreadCount = 0;
	m_iCurrentPlaying  = -1;

	if (m_pClipList)
	{
		free(m_pClipList);
		m_pClipList = NULL;
	}
}



/*
 * 功能: 得到静态单例
 * 摘要: -
 * 参数: -
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2008.08.14
 * 修改日志:
 */
CVideoManager * CVideoManager::GetInstance()
{
	if (!s_pInstance)
		s_pInstance = new CVideoManager();

    return s_pInstance;
}



/*
 * 功能: 得到当前正在播放的视频指针
 * 摘要: -
 * 参数: -
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2008.08.14
 * 修改日志:
 */
RGNDATA * CVideoManager::GetClipList()
{
	if (m_iCurrentPlaying == -1)
		return NULL;

	if (m_pClipList)
	{
		free(m_pClipList);
		m_pClipList = NULL;
	}

	static int NumRect = 4;
	m_pClipList = (RGNDATA *)malloc(sizeof(RGNDATAHEADER) + sizeof(RECT) * NumRect);

	RECT * pRC = m_pVideosArray[m_iCurrentPlaying]->GetSize();

	static RECT rcClipRect[4];
	static int iWidth = render::Interface::GetInstance()->GetWndWidth();
	static int iHeight= render::Interface::GetInstance()->GetWndHeight();

	rcClipRect[0].left = 0;
	rcClipRect[0].top = 0;
	rcClipRect[0].right = pRC->left;
	rcClipRect[0].bottom = iHeight;

	rcClipRect[1].left = pRC->left + pRC->right;
	rcClipRect[1].top = 0;
	rcClipRect[1].right = iWidth;
	rcClipRect[1].bottom = iHeight;

	rcClipRect[2].left = pRC->left;
	rcClipRect[2].top = 0;
	rcClipRect[2].right = pRC->left + pRC->right;
	rcClipRect[2].bottom = pRC->top;

	rcClipRect[3].left = pRC->left;
	rcClipRect[3].top = pRC->top + pRC->bottom;
	rcClipRect[3].right = pRC->left + pRC->right;
	rcClipRect[3].bottom = iHeight;

	memcpy(m_pClipList->Buffer, &rcClipRect, sizeof(RECT) * NumRect);
	m_pClipList->rdh.dwSize = sizeof(RGNDATAHEADER);
	m_pClipList->rdh.iType = RDH_RECTANGLES;
	m_pClipList->rdh.nCount = NumRect;
	m_pClipList->rdh.nRgnSize = sizeof(RECT) * NumRect;
	m_pClipList->rdh.rcBound.left = 0;
	m_pClipList->rdh.rcBound.top = 0;
	m_pClipList->rdh.rcBound.right = iWidth;
	m_pClipList->rdh.rcBound.bottom = iHeight;

	return m_pClipList;
}



/*
 * 功能: 设定是否为全屏幕
 * 摘要: -
 * 参数: bIsFullScreen - 是否为全屏幕
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2008.08.14
 * 修改日志:
 */
void CVideoManager::SetFullScreen(bool bIsFullScreen)
{
	if (m_iCurrentPlaying == -1 ||
		!render::Interface::GetInstance()->GetSetting()->bIsWindow)
		return;

	m_bIsFullScreen = bIsFullScreen;

	if (m_pVideosArray[m_iCurrentPlaying])
	{
		if (m_pVideosArray[m_iCurrentPlaying]->IsFullScreen() != m_bIsFullScreen)
			m_pVideosArray[m_iCurrentPlaying]->SwitchScreenMode();
	}
}



/*
 * 功能: 初始化
 * 摘要: -
 * 参数: hWnd - 窗体句柄
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2008.08.14
 * 修改日志:
 */
bool CVideoManager::Init(HWND hWnd)
{
	if (CoInitialize(NULL) != S_OK)
		return false;

	m_hWnd = hWnd;

	return true;
}



/*
 * 功能: 销毁函数
 * 摘要: -
 * 参数: -
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2008.08.14
 * 修改日志:
 */
void CVideoManager::Destroy()
{
	for (DWORD dw = 0; dw < NUM_VIDEO; ++dw)
		SAFE_DELETE(m_pVideosArray[dw]);
}



/*
 * 功能: 播放视频函数
 * 摘要: -
 * 参数: pszFileName - 视频文件路径名
 *		 uiLoop		 - 循环次数,-1表示无限循环
 *		 prcSize	 - 视频尺寸,NULL表示整个窗体客户区尺寸
 * 返回值: 如果调用成功,则返回当前正在播放的视频索引
 * 作者: lpf
 * 创建日期: 2008.08.14
 * 修改日志:
 */
int CVideoManager::PlayVideo(const char * pszFileName, unsigned int uiLoop, const RECT * prcSize)
{
	if (m_bIsUniquelyPlaying)
	{
		if (m_iCurrentPlaying != -1)
		{
			SAFE_DELETE(m_pVideosArray[m_iCurrentPlaying]);
			m_uiVideosLoop[m_iCurrentPlaying] = 0;
			m_iCurrentPlaying = -1;
			--m_iPlayThreadCount;
		}

		CVideo * pVideo = NULL;
		for (DWORD dw = 0; dw < NUM_VIDEO; ++dw)
		{
			pVideo = m_pVideosArray[dw];
			if (!pVideo)
			{
				m_iCurrentPlaying = dw;
				break;
			}
		}

		if (m_iCurrentPlaying != -1)
		{
			m_pVideosArray[m_iCurrentPlaying] = new CVideo();
			if (!m_pVideosArray[m_iCurrentPlaying]->Load(g_hWnd, pszFileName))
			{
				SAFE_DELETE(m_pVideosArray[m_iCurrentPlaying]);
				m_iCurrentPlaying = -1;
				return -1;
			}

			SetFullScreen(m_bIsFullScreen);
			m_pVideosArray[m_iCurrentPlaying]->AdjustSize(prcSize);
			m_pVideosArray[m_iCurrentPlaying]->Play();

			if (uiLoop != -1)
				m_uiVideosLoop[m_iCurrentPlaying] = uiLoop - 1;
			else
				m_uiVideosLoop[m_iCurrentPlaying] = -1;

			++m_iPlayThreadCount;

			return m_iCurrentPlaying;
		}else
			return -1;
	}else
	{
		return -1;
	}
}



/*
 * 功能: 停止视频播放
 * 摘要: -
 * 参数: iVideoIndex - 需要停止的视频索引(从0开始,如果为-1则表示,当前播放的视频)
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2008.08.18
 * 修改日志:
 */
void CVideoManager::StopVideo(int iVideoIndex)
{
	if (iVideoIndex < -1 || iVideoIndex >= NUM_VIDEO)
		return;

	if (iVideoIndex == -1)
	{
		if (m_iCurrentPlaying != -1)
		{
			SAFE_DELETE(m_pVideosArray[m_iCurrentPlaying]);
			m_uiVideosLoop[m_iCurrentPlaying] = 0;
			m_iCurrentPlaying = -1;
			--m_iPlayThreadCount;
		}else
			return;
	}else
	{
		if (iVideoIndex == m_iCurrentPlaying)
		{
			SAFE_DELETE(m_pVideosArray[m_iCurrentPlaying]);
			m_uiVideosLoop[m_iCurrentPlaying] = 0;
			m_iCurrentPlaying = -1;
			--m_iPlayThreadCount;
		}else
		{
			SAFE_DELETE(m_pVideosArray[iVideoIndex]);
			m_uiVideosLoop[iVideoIndex] = 0;
		}
	}
}



/*
 * 功能: 管理器垃圾回收
 * 摘要: 播放完毕的视频对象一律释放空间,并对循环进行处理
 * 参数: -
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2008.08.14
 * 修改日志:
 *	2008.10.17 - lpf
 *		增加了局部变量赋予初始值;
 *	2009.08.25 - lpf
 *		修改了销毁时机,一旦对象stop后,就立即效果,不再去通过GetState()方法判断其是否停止,因为有些机器无法得到正确状态;
 */
void CVideoManager::GarbageColloction()
{
	CVideo * pVideo = NULL;
	for (DWORD dw = 0; dw < NUM_VIDEO; ++dw)
	{
		pVideo = m_pVideosArray[dw];

		if (pVideo)
		{
			if (pVideo->IsComplete())
			{
				if (m_uiVideosLoop[dw] == -1)
				{
					pVideo->SetPos(0);
				}else if (m_uiVideosLoop[dw])
				{
					pVideo->SetPos(0);
					--m_uiVideosLoop[dw];
				}else
				{
					pVideo->Stop();
					SAFE_DELETE(m_pVideosArray[dw]);
					m_uiVideosLoop[dw] = 0;
					m_iCurrentPlaying = -1;
					--m_iPlayThreadCount;
				}
			}
		}
	}
}