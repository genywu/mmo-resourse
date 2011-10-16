#pragma once
#include "cmd.h"

class CmdMapSetBuildID : public Cmd
{
	DWORD m_dwGridIndex;
    WORD m_wBuildID;
	WORD m_wOldBuildID;

public:
	CmdMapSetBuildID(void);
	virtual ~CmdMapSetBuildID(void);

	void SetBuildID(DWORD dwGridIndex,WORD wBuildID);

	virtual void Execute(void);
	virtual void UnExecute(void);

	virtual void Dump(void);
};
