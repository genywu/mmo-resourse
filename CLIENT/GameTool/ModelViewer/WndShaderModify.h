#pragma once
#include "..\..\frostengine\ui.h"

class WndViewer;

class WndShaderModify: public ui::Wnd
{
private:
	ui::Button			*m_pAddTextureBtn;		//添加纹理按钮
	ui::Button			*m_pDelTextureBtn;		//删除纹理按钮
	ui::Button			*m_pAddShaderBtn;		//在最后的位置添加一个shader
	ui::Button			*m_pDelShaderBtn;		//在最后的位置删除Shader
	ui::Button			*m_pReplaceTextureBtn;	//替换纹理
	ui::CheckBox		*m_pTwoSideCheckEnable[2];		//复选框	
	ui::Combo			*m_pComboAlphaTest;
	ui::Combo			*m_pComboalphablend;
	ui::Combo			*m_pComboColorOP;
	ui::Combo			*m_pComboTexIndex;
	ui::TreeView::Node	*m_pNode;			//当前编辑的Node指针;
	ui::EditBox			*m_pTextureNameEdit;	//纹理名字编辑框

	ui::Button			*m_pConfirm;		//确定修改
	ui::Button			*m_pCancel;			//取消

public:
	WndShaderModify(void);
	~WndShaderModify(void);

	BOOL Create(const char *pszName,const RECT *pRect,DWORD dwStyle,ui::Wnd *pParent);
	void Destroy();

	void CaculateWhichToDisplay();	//计算那些需要显示以编辑
	virtual void Update();
	virtual void Render();
	void	SetCurNode(ui::TreeView::Node *pNode);
};