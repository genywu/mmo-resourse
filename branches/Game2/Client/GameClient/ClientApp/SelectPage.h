#pragma once
#include "gameui\mwindow.h"

class CSelectPage :
	public CMWindow
{
private:
	BOOL		m_bIsLoad;
protected:
public:
	CSelectPage(void);
	virtual ~CSelectPage(void);
	BOOL IsLoad(){return m_bIsLoad;};
	void SetLoad( BOOL t ){m_bIsLoad = t;};

};
