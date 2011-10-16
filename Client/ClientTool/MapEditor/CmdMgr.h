#pragma once

#include "..\..\engine\singleton.h"
#include <vector>

#define MAX_COMMAND_SAVED 16

class Cmd;

class CmdMgr
{
	DECLARE_SINGLETON(CmdMgr);
    
	std::vector<Cmd*> m_vCmdArray;

	Cmd *m_pCurCmd;//指令运行指针


public:
	CmdMgr(void);
	~CmdMgr(void);

	void Do(Cmd * pCmd);
	void ReDo();
	void UnDo();
	void Dump();
	void Clear();
};
