
#pragma once
#include "cmd.h"

class CmdMapSetDoor : public Cmd
{
	DWORD m_dwGridIndex;
    DWORD m_dwOldDoorIndex;
	DWORD m_dwDoorIndex;

public:
	CmdMapSetDoor(void);
	virtual ~CmdMapSetDoor(void);

	void SetDoorIndex(DWORD dwGridIndex,DWORD dwDoorIndex);

	virtual bool Execute(void);
	virtual void UnExecute(void);

	virtual void Dump(void);
};
