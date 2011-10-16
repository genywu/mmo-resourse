#pragma once

#include "..\..\frostengine\ui.h"

class WndAnimFrameEx : public ui::Wnd
{
	long m_lExportWidth;
	long m_lExportHeight;
	char m_szAnimFrameSaveDir[_MAX_PATH];			//动画帧保存目录

	ui::Static  * m_pLabel1;
	ui::Static  * m_pLabel2;
	ui::Static  * m_pLabel3;
	ui::ListBox * m_pFPS;
	ui::ListBox * m_pFileFormat;
	ui::Button * m_pBtnStart;
	ui::Button * m_pBtnReturn;
	ui::Button * m_pBtnSetDir;
	ui::EditBox * m_pExportWidth;
	ui::EditBox * m_pExportHeight;
	ui::EditBox * m_pBoxDir;

public:
	WndAnimFrameEx(void);
	~WndAnimFrameEx(void);

	virtual void Update();
	virtual void Render();

	virtual BOOL Create(const char * pszName, const RECT * pRect, DWORD dwStyle, ui::Wnd * pParent);
	virtual void Destroy();
	void BeginPopUp(void);
	void EndPopUp(void);

	bool CheckInputSize();															//检查输入的尺寸
	BOOL IsStartExAnimFrame()		{ return m_pBtnStart->IsClicked(); }			//是否开始导出动画序列帧
	long   GetExportWidth()			{ return m_lExportWidth; }						//得到导出宽度
	long   GetExportHeight()		{ return m_lExportHeight; }						//得到导出高度
	DWORD  GetFPS();																//得到动画帧时间间隔
	char * GetFileFormat();															//得到导出格式
	char * GetAnimFrameSaveDir()	{ return m_szAnimFrameSaveDir; }				//返回动画序列帧的保存目录
};