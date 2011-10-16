#pragma once
#include "cmd.h"

class CmdMapFillGridVisible : public Cmd
{
	//格子列表
	std::vector<int>   m_vGridIndexArray;
	std::vector<BOOL>   m_vGridOldVisibleArray;
	
	BOOL m_bVisible;

public:
	CmdMapFillGridVisible(void);
	virtual ~CmdMapFillGridVisible(void);

	void AddGridIndex(DWORD dwGridIndex);
	void SetVisible(BOOL bVisible) { m_bVisible = bVisible;}
	
	virtual bool Execute(void);
	virtual void UnExecute(void);

	virtual void Dump(void);
};
