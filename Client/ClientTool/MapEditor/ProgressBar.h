#pragma once

class ProgressBar
{
	HWND m_hWndPB;    // handle of progress bar 

public:
	ProgressBar(void);
	~ProgressBar(void);
	void Begin(HWND hParentWnd,COLORREF color,int iRange);
	void StepIt(void);
	void End(void);
};
