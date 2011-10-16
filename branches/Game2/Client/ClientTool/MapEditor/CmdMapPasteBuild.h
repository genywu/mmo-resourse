#pragma once
#include "cmd.h"
#include "map.h"
#include "ToolBuildEdit.h"

class CmdMapPasteBuild : public Cmd
{
	//格子列表
	DWORD m_dwCX,m_dwCZ;
	ToolBuildEdit::tagCopyData * m_pClipboard;
	ToolBuildEdit::tagCopyData * m_pOldArray;
	DWORD m_dwBuildCount;
	
public:
	CmdMapPasteBuild(void);
	virtual ~CmdMapPasteBuild(void);

	void SetClipboard(const ToolBuildEdit::tagCopyData *pClipboard,DWORD dwCount);

	virtual bool Execute(void);
	virtual void UnExecute(void);

	virtual void Dump(void);
	void SetCenter(DWORD dwCX, DWORD dwCZ);
};