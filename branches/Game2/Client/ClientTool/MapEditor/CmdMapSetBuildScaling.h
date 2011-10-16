#pragma once
#include "cmd.h"

class CmdMapSetBuildScaling : public Cmd
{
	DWORD m_dwGridIndex;
    D3DXVECTOR3 m_vBuildScaling;
	D3DXVECTOR3 m_vOldBuildScaling;

public:
	CmdMapSetBuildScaling(void);
	virtual ~CmdMapSetBuildScaling(void);

	void SetBuildScaling(DWORD dwGridIndex, D3DXVECTOR3 & vBuildScaling);

	virtual bool Execute(void);
	virtual void UnExecute(void);

	virtual void Dump(void);
};
