#pragma once
#include "cmd.h"

class CmdMapSetBuildScalingZ : public Cmd
{
	DWORD m_dwGridIndex;
    float m_fScalingZ;
	float m_fOldScalingZ;

public:
	CmdMapSetBuildScalingZ(void);
	virtual ~CmdMapSetBuildScalingZ(void);

	void SetBuildScalingZ(DWORD dwGridIndex, float fScalingZ);

	virtual bool Execute(void);
	virtual void UnExecute(void);

	virtual void Dump(void);
};
