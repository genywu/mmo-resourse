#pragma once
#include "cmd.h"

class CmdMapMoveBuild :
	public Cmd
{
	DWORD m_dwSrcGridIndex;

	DWORD m_dwDstGridIndex;


public:

	void SetBuildData(DWORD dwDstGrid,DWORD dwSrcIndex);
	
	virtual void Execute(void);
	virtual void UnExecute(void);

	virtual void Dump(void);

	CmdMapMoveBuild(void);
	~CmdMapMoveBuild(void);
};
