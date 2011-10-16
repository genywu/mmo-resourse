#pragma once
#include "cmd.h"

class CmdMapSetBuildOffsetXZ : public Cmd
{
	DWORD m_dwGridIndex;
    float m_fBuildOffsetX;
	float m_fBuildOffsetZ;
	float m_fOldBuildOffsetX;
	float m_fOldBuildOffsetZ;

public:
	CmdMapSetBuildOffsetXZ(void);
	virtual ~CmdMapSetBuildOffsetXZ(void);

	void MapSetBuildPosOffsetXZ(DWORD dwGridIndex, float fPosX, float fPosZ);

	virtual bool Execute(void);
	virtual void UnExecute(void);

	virtual void Dump(void);
};
