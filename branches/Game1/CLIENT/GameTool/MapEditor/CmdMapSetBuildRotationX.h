#pragma once
#include "cmd.h"

class CmdMapSetBuildRotationX : public Cmd
{
	DWORD m_dwGridIndex;
    float m_fBuildRotationX;
	float m_fOldBuildRotationX;

public:
	CmdMapSetBuildRotationX(void);
	virtual ~CmdMapSetBuildRotationX(void);

	void SetBuildRotationX(DWORD dwGridIndex,float fBuildRotationX);

	virtual bool Execute(void);
	virtual void UnExecute(void);

	virtual void Dump(void);
};
