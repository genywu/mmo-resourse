#pragma once

#include <fui.h>

class TitleWnd :
	public ui::Wnd
{
public:

	virtual void Render();

	TitleWnd(void);
	~TitleWnd(void);
};
