#pragma once

#include <vector>
#include "..\..\EngineExtend\displaymodel.h"


const DWORD dwDoorColor[8] =
{
	0x8000ffff,	    //普通
	0x800040ff,		//随机
	0xa01280ff,		//脚本
};


class SelectFrustum;
class EditWnd;

enum DOOREDITSTATUETYPE 
{	
	DEST_SELECT,  	//选择
	DEST_PALACE		//放置
};

class ToolDoorEdit
{
	DOOREDITSTATUETYPE m_eEditStatus;

	std::vector<int> m_vSelGridIndexArray;

	BOOL m_bDrag;
	POINT m_ptDragStart,m_ptDragEnd;

	//入口模型
	AnimInfo m_cAnimInfo;
	DWORD m_dwCurTime;
	CDisplayModel *m_pDoorModel;
	
public:
	ToolDoorEdit(void);
	~ToolDoorEdit(void);

	void MessageProc(BOOL bActive);
	void Render();
	void Create();
	void Destroy(void);
	void _ShowStatus(void);
	void DrawSelected(void);
	void RnderDoorModel(const D3DXMATRIX *pMat);

	CDisplayModel * GetDoorModel() const { return m_pDoorModel;}
};
