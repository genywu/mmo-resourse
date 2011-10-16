#pragma once
#include "..\..\frostengine\ui.h"

class WndViewer;

class WndParticleTree: public ui::Wnd
{
	ui::TreeView  * m_pParticleTreeView;			//显示模型的树形控件
	char		  m_pszParticleFilePathName[128];	//模型路径文件名
	
public:
	WndParticleTree(void);
	~WndParticleTree(void);

	BOOL Create(const char *pszName,const RECT *pRect,DWORD dwStyle,ui::Wnd *pParent);
	void Destroy();

	ui::TreeView * GetParticleTreeView() {return m_pParticleTreeView;}
	void CreateParticleTree(const char * strParticleFilePathName);
	virtual void Update();
	virtual void Render();
	const char *GetParticleFilePathName() {return m_pszParticleFilePathName;}
	void SaveParticleFile();
};