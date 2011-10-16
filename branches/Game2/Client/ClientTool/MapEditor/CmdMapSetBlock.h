#pragma once
#include "cmd.h"

class CmdMapSetBlock : public Cmd
{

	DWORD m_dwGridIndex;
    DWORD m_dwOldBlock;
	DWORD m_dwBlock;

public:
	CmdMapSetBlock(void);
	virtual ~CmdMapSetBlock(void);

	void SetBlock(DWORD dwGridIndex,DWORD dwBlock);

	virtual void Execute(void);
	virtual void UnExecute(void);

	virtual void Dump(void);
};
