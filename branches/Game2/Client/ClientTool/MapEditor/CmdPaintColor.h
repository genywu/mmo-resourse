#pragma once
#include <vector>
#include "cmd.h"

class CmdMapPaintColor : public Cmd
{
	DWORD	  m_dwVertexIndex;
	DWORD     m_dwColor;
	DWORD     m_dwBrushType;
	float     m_fBrushRadius;
	float	  m_fColorChroma;

	std::vector<DWORD> m_vVertexIndexArray;
	std::vector<DWORD> m_vVertexColorArray;

public:
	CmdMapPaintColor(void);
	~CmdMapPaintColor(void);

	void Destroy();

	void SetVertex(DWORD dwVertexIndex) { m_dwVertexIndex = dwVertexIndex;}
	void SetColor(D3DCOLOR dwColor,float fColorChroma) { m_dwColor = dwColor;m_fColorChroma = fColorChroma;}
	void SetBrush(DWORD dwBrushType,float  fBrushRadius) { m_dwBrushType = dwBrushType; m_fBrushRadius = fBrushRadius;}

	virtual bool Execute(void);
	virtual void UnExecute(void);
	virtual void Dump(void);
};
