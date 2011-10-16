#pragma once
#include <vector>
#include "cmd.h"


class CmdMapFillWeatherEffectFlag :
	public Cmd
{
	
	//格子列表
	std::vector<int>    m_vGridIndexArray;
	std::vector<BOOL>   m_vGridOldWeatherEffectFlagArray;
	
	BOOL m_bWeatherEffectFlag;

public:
	CmdMapFillWeatherEffectFlag(void);
	virtual ~CmdMapFillWeatherEffectFlag(void);

	void AddGridIndex(DWORD dwGridIndex);
	void SetWeatherEffectFlag(BOOL bWeatherEffectFlag) { m_bWeatherEffectFlag = bWeatherEffectFlag;}
	
	virtual void Execute(void);
	virtual void UnExecute(void);

	virtual void Dump(void);
};
