#pragma once
#include "..\..\frostengine\ui.h"

class WndViewer;

class WndModelTree: public ui::Wnd
{
	ui::TreeView  * m_pModelTreeView;			//显示模型的树形控件

	char		  m_pszModelFilePathName[128];	//模型路径文件名
	char		  m_pszModelName[128];		//当前操作的model名字
public:
	WndModelTree(void);
	~WndModelTree(void);

	BOOL Create(const char *pszName,const RECT *pRect,DWORD dwStyle,ui::Wnd *pParent);
	void Destroy();

	ui::TreeView * GetModelTreeView() {return m_pModelTreeView;}
	void CreateModelTree(const char * ModelFilePathName);
	virtual void Update();
	virtual void Render();
	void SetModelFilePathName (const char * pathname) {strcpy(m_pszModelFilePathName,pathname);}
	void SetCurrentModelName(const char *name){strcpy(m_pszModelName,name);}
	const char * GetModelFilePathName() {return m_pszModelFilePathName;}
	const char * GetCurModelName()		{return m_pszModelName;}
	void SaveModelFile();
};