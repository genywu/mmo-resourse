#include "stdafx.h"
#include "Video.h"
#include <uuids.h>
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
 * 创建日期: 2008.08.07
 * 修改日志:
 */
CVideo::CVideo()
{
	m_hWnd = NULL;
	ZeroMemory(&m_rcSize, sizeof(RECT));

	m_pGB = NULL;
	m_pMC = NULL;
	m_pVW = NULL;
	m_pMS = NULL;
}



/*
 * 功能: 析构函数
 * 摘要: -
 * 参数: -
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2008.08.07
 * 修改日志:
 */
CVideo::~CVideo()
{
	if (m_pMC)
		m_pMC->Stop();

	m_hWnd = NULL;
	ZeroMemory(&m_rcSize, sizeof(RECT));

	SAFE_RELEASE(m_pMS);
	SAFE_RELEASE(m_pVW);
	SAFE_RELEASE(m_pMC);
	SAFE_RELEASE(m_pGB);
}



/*
 * 功能: 是否为全屏幕播放
 * 摘要: -
 * 参数: -
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2008.08.07
 * 修改日志:
 *	2008.10.17 - lpf
 *		增加了局部变量赋予初始值;
 */
bool CVideo::IsFullScreen()
{
	if (m_pVW != NULL)
	{
		long lMode = 0;
		m_pVW->get_FullScreenMode(&lMode);
		return lMode == OATRUE ? true : false;
	}else
		return false;
}



/*
 * 功能: 是否播放完毕
 * 摘要: -
 * 参数: -
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2008.08.07
 * 修改日志:
 *	2008.10.17 - lpf
 *		增加了局部变量赋予初始值;
 */
bool CVideo::IsComplete()
{
	if (m_pMS == NULL)
		return false;

	LONGLONG llCurrent = 0;
	LONGLONG llLength = 0;
	m_pMS->GetCurrentPosition(&llCurrent);
	m_pMS->GetDuration(&llLength);
	if (llCurrent >= llLength)
		return true;
	else
		return false;
}



/*
 * 功能: 设定当前播放位置
 * 摘要: -
 * 参数: llPos - 指定当前播放位置
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2008.08.07
 * 修改日志:
 */
bool CVideo::SetPos(LONGLONG llPos)
{
	if (m_pMS == NULL)
		return false;

	m_pMS->SetPositions(&llPos, AM_SEEKING_AbsolutePositioning, NULL, AM_SEEKING_NoPositioning);
	return true;
}



/*
 * 功能: 设定视频是否可见
 * 摘要: -
 * 参数: bIsVisible - 是否可见
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2008.08.07
 * 修改日志:
 */
bool CVideo::SetVisible(bool bIsVisible)
{
	if (m_pMS == NULL)
		return false;

	m_pVW->put_Visible(bIsVisible ? OATRUE : OAFALSE);
	return true;
}



/*
 * 功能: 得到当前媒体状态
 * 摘要: -
 * 参数: bIsVisible - 是否可见
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2008.08.07
 * 修改日志:
 *	2008.10.17 - lpf
 *		增加了局部变量赋予初始值;
 */
FILTER_STATE CVideo::GetState()
{
	if (m_pMC != NULL)
	{
		OAFilterState fs = 0;
		m_pMC->GetState(INFINITE, &fs);
		return (FILTER_STATE)fs;
	}

	return State_Stopped;
}



/*
 * 功能: 载入视频文件
 * 摘要: -
 * 参数: pszFileName - 视频文件名
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2008.08.07
 * 修改日志:
 */
bool CVideo::Load(HWND hWnd, const char * pszFileName)
{
	WCHAR wFileName[MAX_PATH] = { 0 };

	if (FAILED(CoCreateInstance(CLSID_FilterGraph,
								NULL,
					 			CLSCTX_INPROC_SERVER, 
					 			IID_IGraphBuilder,
					 			(void **)&m_pGB)))
		return false;
       
	if (FAILED(m_pGB->QueryInterface(IID_IMediaControl, (void **)&m_pMC)))
		return false;

	if (FAILED(m_pGB->QueryInterface(IID_IVideoWindow, (void **)&m_pVW)))
		return false;

	if (FAILED(m_pGB->QueryInterface(IID_IMediaSeeking, (void **)&m_pMS)))
		return false;

	MultiByteToWideChar(CP_ACP, 0, pszFileName, -1, wFileName, MAX_PATH);
	if (FAILED(m_pGB->RenderFile(wFileName, NULL)))
		return false;

	m_hWnd = hWnd;

	m_pVW->put_Owner((OAHWND)hWnd);
	m_pVW->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	return true;
}



/*
 * 功能: 调整视频尺寸和父窗体客户区一样
 * 摘要: -
 * 参数: prcSize - 尺寸矩形
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2008.08.07
 * 修改日志:
 */
bool CVideo::AdjustSize(const RECT * prcSize)
{
	if (m_pVW == NULL)
		return false;

	if (prcSize == NULL)
		GetClientRect(m_hWnd, &m_rcSize);
	else
		m_rcSize = *prcSize;

	m_pVW->SetWindowPosition(m_rcSize.left, m_rcSize.top, m_rcSize.right, m_rcSize.bottom);

	return true;
}



/*
 * 功能: 切换窗口全屏模式
 * 摘要: -
 * 参数: -
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2008.08.07
 * 修改日志:
 */
bool CVideo::SwitchScreenMode()
{
	if (m_pVW == NULL)
		return false;

    static HWND hDrain = NULL;

	if (IsFullScreen())
	{
		m_pVW->put_FullScreenMode(OAFALSE);
		m_pVW->put_MessageDrain((OAHWND)hDrain);
	}else
	{
		m_pVW->get_MessageDrain((OAHWND *)&hDrain);
		m_pVW->put_MessageDrain((OAHWND)m_hWnd);
		m_pVW->put_FullScreenMode(OATRUE);
	}

	return true;
}



/*
 * 功能: 开始播放视频
 * 摘要: -
 * 参数: -
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2008.08.07
 * 修改日志:
 */
bool CVideo::Play()
{
	if (m_pMC == NULL)
		return false;

	if (FAILED(m_pMC->Run()))
		return false;
	else
		return true;
}



/*
 * 功能: 暂停播放视频
 * 摘要: -
 * 参数: -
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2008.08.07
 * 修改日志:
 */
bool CVideo::Pasue()
{
	if (m_pMC == NULL)
		return false;

	if (FAILED(m_pMC->Pause()))
		return false;
	else
		return true;
}



/*
 * 功能: 停止播放视频
 * 摘要: -
 * 参数: -
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2008.08.07
 * 修改日志:
 */
bool CVideo::Stop()
{
	if (m_pMC == NULL)
		return false;

	if (FAILED(m_pMC->Stop()))
		return false;
	else
		return true;
}