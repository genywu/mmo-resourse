#pragma once
#include "cmd.h"

class CmdMapMoveBuild :	public Cmd
{
	DWORD m_dwSrcGridIndex;
	DWORD m_dwDstGridIndex;

public:
	CmdMapMoveBuild(void);
	~CmdMapMoveBuild(void);

	void SetBuildData(DWORD dwDstGrid,DWORD dwSrcIndex);
	
	virtual bool Execute(void);
	virtual void UnExecute(void);

	virtual void Dump(void);
};
