#pragma once
#include "..\..\frostengine\ui.h"

class WndViewer;

class WndModelModify: public ui::Wnd
{
	ui::Button	* m_pBtnAddGroup;					//添加GROUP
	ui::Button	* m_pBtnDelGroup;					//删除最后一组Group
	ui::Button	* m_pBtnAddAction;					//添加动作
	ui::Button	* m_pBtnDelAction;					//删除动作
	ui::Button  * m_pBtnSetActionFile;				//指定动作anim文件
	ui::Button  * m_pBtnAddPart;					//添加part组
	ui::Button	* m_pBtnDelPart;					//删除part组
	ui::Button	* m_pBtnSetPartFile;				//设置part文件

	ui::Button	* m_pBtnAddTrail_emitter;			//添加尾迹发射器组
	ui::Button  * m_pBtnDelTrail_emitter;			//删除尾迹发射器组
	ui::Button	* m_pBtnSetTrail_MeshFile;			//设置尾迹MESH文件
	ui::Button	* m_pBtnSetTrail_TrailFile;			//设置尾迹Trail文件

	ui::Button	* m_pBtnAddParticle_emitter;		//添加粒子发射器组
	ui::Button  * m_pBtnDelParticle_emitter;		//删除粒子发射器组
	ui::Button	* m_pBtnSetParticle_MeshFile;		//设置粒子MESH文件
	ui::Button	* m_pBtnSetParticle_ParticleFile;	//设置粒子Trail文件
	ui::EditBox * m_pEditNumber;					//设置粒子数量

	ui::Button	* m_pConfirm;						//确定并保存
	ui::Button  * m_pCancel;						//取消
	ui::TreeView::Node *m_pNode;					//当前编辑的Node
public:
	WndModelModify(void);
	~WndModelModify(void);

	BOOL Create(const char *pszName,const RECT *pRect,DWORD dwStyle,ui::Wnd *pParent);
	void Destroy();

	virtual void Update();
	virtual void Render();
	void SetNode(ui::TreeView::Node *pNode) {if (pNode == m_pNode) return; m_pNode = pNode;}
	void CaculateWhichToDisplay();

	const char * OpenFileNamePath(string *filestring,const char * postfix);

};