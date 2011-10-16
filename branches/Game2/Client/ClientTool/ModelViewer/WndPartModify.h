#pragma once
#include "..\..\frostengine\ui.h"

class WndViewer;

class WndPartModify: public ui::Wnd
{
	char				m_pszPartFilePathName[128];	//模型part路径文件名
	ui::TreeView::Node	*m_pNode;

	ui::CheckBox	*m_Check[2];
	ui::Button		*m_pBtnConfirm;		//确定保存
	ui::Button		*m_pBtnCancel;		//取消
	ui::Combo		*m_pCombo;
	ui::Button		*m_pBtnSetMeshFile;

	ui::CheckBox		*m_pAnimcolorCheck;		
	ui::CheckBox		*m_pAnimuvCheck;
	ui::CheckBox		*m_pAnimvisibilityCheck;
	ui::CheckBox		*m_pAnimtextureCheck;
	ui::CheckBox		*m_pEnviromentmapsCheck;

	ui::Button			*m_pBtnSetAnimcolor;			//设置AC文件
	ui::Button			*m_pBtnSetAnimuv;				//设置UV文件
	ui::Button			*m_pBtnSetAnimvisibility;		//设置VIS文件
	ui::Button			*m_pBtnSetAnimtexture;			//设置AT文件

	ui::Button			*m_pBtnAddRenderMesh;			//增加一组RenderMesh
	ui::Button			*m_pBtnDelRenderMesh;			//删除一组RenderMesh

	ui::Button			*m_pBtnAddLevel;				//增加一组levelarray
	ui::Button			*m_pBtnDelLevel;				//删除一组levelarray

	ui::Button			*m_pBtnAddMeshIndex;			//增加一组MeshIndex
	ui::Button			*m_pBtnDelMeshIndex;			//删除一组MeshIndex
	ui::Combo			*m_pComboMeshList;				//编辑MeshIndex时 选择当前的MESH表

	ui::Button			*m_pBtnAddRenderMeshItem;		//增加RenderMesh里面的项目
	ui::Button			*m_pBtnDelRenderMeshItem;		//删除RenderMesh里面的最后一个项目

public:
	WndPartModify(void);
	~WndPartModify(void);

	BOOL Create(const char *pszName,const RECT *pRect,DWORD dwStyle,ui::Wnd *pParent);
	void Destroy();

	virtual void Update();
	virtual void Render();

	void SetPartFilePathName (const char * pathname) {strcpy(m_pszPartFilePathName,pathname);}
	const char *GetPartFilePathName() {return m_pszPartFilePathName;}
	void SetCurPartNode(ui::TreeView::Node *pNode);
	void CaculateToDisplay();
};