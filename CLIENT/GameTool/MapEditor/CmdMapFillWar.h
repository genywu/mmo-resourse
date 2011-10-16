#pragma once

#include <vector>
#include "cmd.h"

class CmdMapFillWar :
	public Cmd
{
	
	//格子列表
	std::vector<int>   m_vGridIndexArray;
	std::vector<int>   m_vGridOldWarArray;
	
	DWORD m_dwWar;

public:
	CmdMapFillWar(void);
	virtual ~CmdMapFillWar(void);

	void AddGridIndex(DWORD dwGridIndex);
	void SetWar(DWORD dwWar) { m_dwWar = dwWar;}
	
	virtual void Execute(void);
	virtual void UnExecute(void);

	virtual void Dump(void);
};
