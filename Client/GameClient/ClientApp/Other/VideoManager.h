/*
 * VideoManager.h
 * 功能: 视频播放对象类。
 * 摘要: 采用DShow模块
 *		 目前支持文件格式 : *.WMV, *.ASF, *.MPEG, *.AVI, QickTime
 * 作者: lpf
 * 创建日期: 2008-08-14
 */

#pragma once
#include "Video.h"

#define NUM_VIDEO 5

class CVideoManager
{
	HWND m_hWnd;
	int m_iPlayThreadCount;					//播放线程计数器
	int m_iCurrentPlaying;					//当前播放的视频索引(-1表示当前没有视频播放)
	bool m_bIsFullScreen;					//当前视频是否全屏播放
	bool m_bIsUniquelyPlaying;

	RGNDATA * m_pClipList;

	CVideo * m_pVideosArray[NUM_VIDEO];		//视频列表
	unsigned int m_uiVideosLoop[NUM_VIDEO];	//视频循环计数

	static CVideoManager * s_pInstance;
	CVideoManager();		//构造函数

public:
	~CVideoManager();		//析构函数

	static CVideoManager * GetInstance();
	static void			   ReleaseInstance()		{ SAFE_DELETE(s_pInstance); }

	int GetPlayThreadCount()						{ return m_iPlayThreadCount; }
	bool IsFullScreen()								{ return m_bIsFullScreen; }
	RGNDATA * GetClipList();

	bool Init(HWND hWnd);
	void Destroy();
	void GarbageColloction();

	int	 PlayVideo(const char * pszFileName, unsigned int iLoop = 1, const RECT * rcSize = NULL);		//播放视频
	void StopVideo(int iVideoIndex = -1);																//停止视频
	void SetFullScreen(bool bIsFullScreen);																//设定该视频是否全屏
};