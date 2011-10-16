#pragma once
#include "cmd.h"

class CmdMapSetBuildRotationZ : public Cmd
{
	DWORD m_dwGridIndex;
    float m_fBuildRotationZ;
	float m_fOldBuildRotationZ;

public:
	CmdMapSetBuildRotationZ(void);
	virtual ~CmdMapSetBuildRotationZ(void);

	void SetBuildRotationZ(DWORD dwGridIndex,float fBuildRotationZ);

	virtual bool Execute(void);
	virtual void UnExecute(void);

	virtual void Dump(void);
};
