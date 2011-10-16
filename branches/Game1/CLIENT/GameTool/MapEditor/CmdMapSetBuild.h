#pragma once
#include "cmd.h"
#include "map.h"

class CmdMapSetBuild : public Cmd
{
	DWORD m_dwGridIndex;

	Grid::tagBuild m_tBuild;

	Grid::tagBuild m_tOldBuild;

public:
	CmdMapSetBuild(void);
	virtual ~CmdMapSetBuild(void);

	void SetBuild(DWORD dwGridIndex,const Grid::tagBuild &tBuild);

	virtual void Execute(void);
	virtual void UnExecute(void);

	virtual void Dump(void);
};
