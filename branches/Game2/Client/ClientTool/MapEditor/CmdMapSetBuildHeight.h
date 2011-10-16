#pragma once
#include "cmd.h"

class CmdMapSetBuildHeight : public Cmd
{
	DWORD m_dwGridIndex;
    float m_fBuildHeight;
	float m_fOldBuildHeight;

public:
	CmdMapSetBuildHeight(void);
	virtual ~CmdMapSetBuildHeight(void);

	void MapSetBuildHeight(DWORD dwGridIndex,float fBuildHeight);

	virtual bool Execute(void);
	virtual void UnExecute(void);

	virtual void Dump(void);
};
