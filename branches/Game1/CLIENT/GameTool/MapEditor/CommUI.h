#pragma once

#include "..\..\frostengine\singleton.h"

class EditWnd;
class NewMapDlg;

class CommUI
{
	DECLARE_SINGLETON(CommUI);

	EditWnd * m_pEditWnd;
	


public:
	CommUI(void);
	~CommUI(void);

	EditWnd * GetEditWnd() { return m_pEditWnd;}

	void Create(void);
	void Destroy(void);
};
