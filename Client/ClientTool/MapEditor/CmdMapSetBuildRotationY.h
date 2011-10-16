#pragma once
#include "cmd.h"

class CmdMapSetBuildRotationY : public Cmd
{
	DWORD m_dwGridIndex;
    float m_fBuildRotationY;
	float m_fOldBuildRotationY;

public:
	CmdMapSetBuildRotationY(void);
	virtual ~CmdMapSetBuildRotationY(void);

	void SetBuildRotationY(DWORD dwGridIndex,float fBuildRotationY);

	virtual bool Execute(void);
	virtual void UnExecute(void);

	virtual void Dump(void);
};
