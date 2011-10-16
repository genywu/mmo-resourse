/*
 * Video.h
 * 功能: 视频播放对象类。
 * 摘要: 采用DShow模块
 *		 目前支持文件格式 : *.WMV, *.ASF, *.MPEG, *.AVI, QickTime
 * 作者: lpf
 * 创建日期: 2008-08-07
 */

#pragma once

#include <dshow.h>

class CVideo
{
	HWND m_hWnd;
	RECT m_rcSize;

	IGraphBuilder * m_pGB;
	IMediaControl * m_pMC;
	IVideoWindow  * m_pVW;
	IMediaSeeking * m_pMS;

public:
	CVideo();			//构造函数
	~CVideo();			//析构函数

	// 设定类成员函数
	bool IsFullScreen();
	bool IsComplete();
	bool SetPos(LONGLONG llPos);
	bool SetVisible(bool bIsVisible);

	RECT *		 GetSize()			{ return &m_rcSize; }
	FILTER_STATE GetState();					//得到当前媒体状态

	// 操作函数
	bool Load(HWND hWnd, const char * pszFileName);
	bool AdjustSize(const RECT * prcSize = NULL);
	bool SwitchScreenMode();
	bool Play();
	bool Pasue();
	bool Stop();
};