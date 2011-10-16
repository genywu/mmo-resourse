#pragma once

#include <vector>
#include "cmd.h"

class CmdMapFillTargetHide :
	public Cmd
{
	//旧格子数据列表
	std::vector<DWORD> m_vGridIndexArray;
	DWORD *m_pOldTargetHideGridIndexArray;
	
	DWORD m_dwTargetHideGridIndex;

public:
	CmdMapFillTargetHide(void);
	virtual ~CmdMapFillTargetHide(void);

	void AddGridIndex(DWORD dwGridIndex);
	void SetTargetHideGridIndex(DWORD dwTargetHideGridIndex) { m_dwTargetHideGridIndex = dwTargetHideGridIndex;}
	
	virtual void Execute(void);
	virtual void UnExecute(void);

	virtual void Dump(void);
};
