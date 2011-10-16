#pragma once
#include "cmd.h"

class CmdMapSetBuildScalingY : public Cmd
{
	DWORD m_dwGridIndex;
    float m_fScalingY;
	float m_fOldScalingY;

public:
	CmdMapSetBuildScalingY(void);
	virtual ~CmdMapSetBuildScalingY(void);

	void SetBuildScalingY(DWORD dwGridIndex, float fScalingY);

	virtual bool Execute(void);
	virtual void UnExecute(void);

	virtual void Dump(void);
};
