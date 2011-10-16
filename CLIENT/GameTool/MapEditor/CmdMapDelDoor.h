#pragma once

#include <vector>
#include "cmd.h"

class CmdMapDelDoor :
	public Cmd
{
	
	//格子列表
	std::vector<int>   m_vGridIndexArray;
	
	DWORD *m_pGridOldDoorIndexArray;
	
public:
	CmdMapDelDoor(void);
	virtual ~CmdMapDelDoor(void);

	void AddGridIndex(DWORD dwGridIndex);
	
	virtual void Execute(void);
	virtual void UnExecute(void);

	virtual void Dump(void);
};
