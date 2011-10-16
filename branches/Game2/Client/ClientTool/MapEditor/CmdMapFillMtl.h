#pragma once

#include <vector>
#include "cmd.h"

class CmdMapFillMtl : public Cmd
{
	//格子列表
	std::vector<int>   m_vGridIndexArray;
	
	DWORD *m_pGridOldMtlArray;
	
	DWORD m_dwMtl;

public:
	CmdMapFillMtl(void);
	virtual ~CmdMapFillMtl(void);

	void AddGridIndex(DWORD dwGridIndex);
	void SetMtl(DWORD dwMtl) { m_dwMtl = dwMtl;}
	
	virtual bool Execute(void);
	virtual void UnExecute(void);

	virtual void Dump(void);
};
