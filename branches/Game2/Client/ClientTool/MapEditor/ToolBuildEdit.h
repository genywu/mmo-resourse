#pragma once

#include <vector>
#include "..\..\EngineExtend\gamemodel.h"
#include "..\..\EngineExtend\displaymodel.h"

//此处保存编辑的
//数据和方法
//分三个状态
//1 选择状态
//		多选：	删除
//		单选：  重新放置（移动）
//				旋转
//				缩放
//2 放置状态

class GameBuildList;
class EditWnd;
class GameSelectRay;

class ToolBuildEdit
{
	BOOL m_bBoxSelectedMode;
	public:
	struct tagCopyData
	{
		int ox,oy;//偏移
		WORD  wBuildID;
		D3DXVECTOR3 vRotation;
		D3DXVECTOR3 vScaling;
		D3DXVECTOR3 vPosOffset;
	};

private:
	tagCopyData * m_pClipboard;
	DWORD m_dwCopyDataCount;

	bool  m_bActivityGridSwitch;
	float m_fPalaceAngle;

	enum BUILDEDITSTATUETYPE {BUILD_EDIT,BUILD_PALACE,BUILD_MARKVISIBLE};
	BUILDEDITSTATUETYPE m_eEditStatus;

	enum BUILDEDITSTATUESUBTYPE { BESST_SELECT,BESST_MOVE};
	BUILDEDITSTATUESUBTYPE m_eEditSubStatus;

	enum BUILDPALACESUBTYPE  { BPST_PALACE,BPST_SUCK};
	BUILDPALACESUBTYPE   m_ePalaceSubStatus;

	//------------------------------------
	std::vector<int> m_vSelGridIndexArray;		//格子,用于建筑编辑
	std::vector<int> m_vNonActivityGridIndex;	//非活动格子

	BOOL  m_bDragMouse;//框选
	POINT m_ptDragStart,m_ptDragEnd;

	AnimInfo m_cCursorAnimInfo;

	//GameBuildList * m_pGameBuildList;

	DWORD m_dwCurHoverGridIndex;
	DWORD m_dwCurHoverBuildGridIndex;

	void  _ShowStatus(void);
	void  _DrawSelected(void);
	void  _CalculateHoverGrid(void);
	void  _DoRotationX(int iAngle = 0);
	void  _DoRotationY(int iAngle = 0);
	void  _DoRotationZ(int iAngle = 0);
	void  _DoEdit(void);
	void  _DoChangeHeight(void);
	void  _DoChangeOffsetAtXZ(void);
	void  _DoScaling(void);
	void  _DoScalingX(void);
	void  _DoScalingY(void);
	void  _DoScalingZ(void);
	void  _AlignAxisX(void);
	void  _AlignAxisY(void);
	void  _AlignAxisZ(void);
	void  _CalculateHoverBuildGrid(GameSelectRay *pRay);
	float _CalculateDir(const D3DXVECTOR3 * pPosition, const GameSelectRay * pRay);

	void  Draw_BuildMarkVisibleSelect();

//	DWORD m_bSuckBuildID;

public:
	ToolBuildEdit(void);
	~ToolBuildEdit(void);

	void MessageProc(BOOL bActive);
	void Render();
	void Create();
	void Destroy(void);

	//GameBuildList *GetGameBuildList() const { return m_pGameBuildList;}
	BOOL FindSelectIndex(DWORD dwGridIndex);

	void _DoFixRotation();
	void Reset(void);

	void Render_BuildMarkVisible(void);
	void Proc_BuildMarkVisible(void);
	void ClearClipboard(void);
	DWORD GetHoverBuildGridIndex() const				{ return m_dwCurHoverBuildGridIndex;}
	int GetSelGridIndex(int iIndex)						{ return m_vSelGridIndexArray.size() ? m_vSelGridIndexArray[iIndex] : -1; }
	std::vector<int> & GetNonActivityGridIndex()		{ return m_vNonActivityGridIndex; }
};
