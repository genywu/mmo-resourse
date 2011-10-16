#pragma once
#include "cmd.h"

class CmdMapSetBuildScalingX : public Cmd
{
	DWORD m_dwGridIndex;
    float m_fScalingX;
	float m_fOldScalingX;

public:
	CmdMapSetBuildScalingX(void);
	virtual ~CmdMapSetBuildScalingX(void);

	void SetBuildScalingX(DWORD dwGridIndex, float fScalingX);

	virtual bool Execute(void);
	virtual void UnExecute(void);

	virtual void Dump(void);
};
