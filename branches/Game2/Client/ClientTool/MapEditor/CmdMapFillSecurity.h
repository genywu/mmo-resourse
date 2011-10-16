#pragma once

#include <vector>
#include "cmd.h"

class CmdMapFillSecurity : public Cmd
{
	//格子列表
	std::vector<int>   m_vGridIndexArray;
	std::vector<int>   m_vGridOldSecurityArray;
	
	DWORD m_dwSecurity;

public:
	CmdMapFillSecurity(void);
	virtual ~CmdMapFillSecurity(void);

	void AddGridIndex(DWORD dwGridIndex);
	void SetSecurity(DWORD dwSecurity) { m_dwSecurity = dwSecurity;}
	
	virtual bool Execute(void);
	virtual void UnExecute(void);

	virtual void Dump(void);
};
